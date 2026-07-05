#include "NoteEditor.h"
#include "Icons.h"
#include "Design.h"

#include <QMenu>
#include <QAction>
#include <QContextMenuEvent>
#include <QEvent>
#include <QApplication>
#include <QClipboard>
#include <QPalette>
#include <QTextCharFormat>
#include <QTextCursor>
#include <QTextBlock>
#include <QFont>
#include <QScrollBar>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QFocusEvent>
#include <QPainter>
#include <QWindow>


NoteEditor::NoteEditor(QWidget *parent)
    : QTextEdit(parent)
{
    setTabChangesFocus(false);
    setAcceptRichText(true);
    document()->setDocumentMargin(0);
    setCursorWidth(3);
    setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    connect(this, &QTextEdit::cursorPositionChanged, this, &NoteEditor::emitState);
    connect(this, &QTextEdit::selectionChanged, this, &NoteEditor::emitState);
    connect(this, &QTextEdit::textChanged, this, &NoteEditor::emitState);

    connect(QApplication::clipboard(), &QClipboard::dataChanged, this, &NoteEditor::emitState);
}

namespace
{
constexpr auto isBold      = [](const QTextCharFormat &format) { return format.fontWeight() >= QFont::Bold; };
constexpr auto isItalic    = [](const QTextCharFormat &format) { return format.fontItalic(); };
constexpr auto isUnderline = [](const QTextCharFormat &format) { return format.fontUnderline(); };
constexpr auto isStrike    = [](const QTextCharFormat &format) { return format.fontStrikeOut(); };
} // namespace

void NoteEditor::toggleBold()
{
    toggleProperty(isBold,
                   [](QTextCharFormat &format, bool on) { format.setFontWeight(on ? QFont::Bold : QFont::Normal); });
}

void NoteEditor::toggleItalic()
{
    toggleProperty(isItalic, [](QTextCharFormat &format, bool on) { format.setFontItalic(on); });
}

void NoteEditor::toggleUnderline()
{
    toggleProperty(isUnderline, [](QTextCharFormat &format, bool on) { format.setFontUnderline(on); });
}

void NoteEditor::toggleStrikethrough()
{
    toggleProperty(isStrike, [](QTextCharFormat &format, bool on) { format.setFontStrikeOut(on); });
}

void NoteEditor::clearFormatting()
{
    QTextCharFormat fmt;
    fmt.setFontWeight(QFont::Normal);
    fmt.setFontItalic(false);
    fmt.setFontUnderline(false);
    fmt.setFontStrikeOut(false);
    fmt.clearBackground();

    QTextCursor cursor = textCursor();
    if (!cursor.hasSelection())
        cursor.select(QTextCursor::Document);
    cursor.setCharFormat(fmt);
    setFocus();
}

void NoteEditor::setLocked(bool locked)
{
    m_locked = locked;
    if (locked) {
        m_tempEdit = false;
        setReadOnly(true);
        viewport()->setCursor(Qt::ArrowCursor);
    }
    else {
        setReadOnly(false);
        viewport()->setCursor(Qt::IBeamCursor);
    }
    emitState();
}

void NoteEditor::setBaseFontSize(int size)
{
    if (size < 1)
        return;
    QFont font = document()->defaultFont();
    font.setPointSize(size);
    document()->setDefaultFont(font);
}

void NoteEditor::setBaseFontFamily(const QString &family)
{
    if (family.isEmpty())
        return;
    QFont font = document()->defaultFont();
    font.setFamily(family);
    document()->setDefaultFont(font);
}

void NoteEditor::setBaseTextColor(const QColor &color)
{
    m_baseTextColor = color;
    updateStylesheet();
}

void NoteEditor::setBaseBgColor(const QColor &color)
{
    m_baseBgColor = color;
    updateStylesheet();
}

void NoteEditor::updateStylesheet()
{
    setStyleSheet(
        QStringLiteral(
            "QTextEdit { background-color: %1; color: %2; border: none; padding: 6px 6px 0 6px; }"
            "QTextEdit QScrollBar:vertical { background: transparent; width: 8px; margin: 0; }"
            "QTextEdit QScrollBar::handle:vertical { background: rgba(35,38,41,100); min-height: 20px; border-radius: "
            "4px; }"
            "QTextEdit QScrollBar::handle:vertical:hover { background: rgba(35,38,41,200); }"
            "QTextEdit QScrollBar::add-line:vertical, QTextEdit QScrollBar::sub-line:vertical { height: 0; }"
            "QTextEdit QScrollBar::add-page:vertical, QTextEdit QScrollBar::sub-page:vertical { background: none; }")
            .arg(m_baseBgColor.name(), m_baseTextColor.name()));
}

EditorState NoteEditor::currentState() const
{
    EditorState state;
    state.bold           = selectionAllHave(isBold);
    state.italic         = selectionAllHave(isItalic);
    state.underline      = selectionAllHave(isUnderline);
    state.strike         = selectionAllHave(isStrike);
    state.fontSize       = document()->defaultFont().pointSize();
    state.hasSelection   = textCursor().hasSelection();
    state.undoAvailable  = document()->isUndoAvailable();
    state.redoAvailable  = document()->isRedoAvailable();
    state.pasteAvailable = !QApplication::clipboard()->text().isEmpty();

    return state;
}

void NoteEditor::filterStandardMenuActions(QMenu *menu, bool hasSelection)
{
    QList<QAction *> toRemove;
    for (QAction *action : menu->actions()) {
        if (action->isSeparator()) {
            toRemove.append(action);
            continue;
        }
        QString normalizedText = action->text().remove('&').toLower();
        if (normalizedText.contains("undo") || normalizedText.contains("redo")
            || (!hasSelection && !normalizedText.contains("paste") && !normalizedText.contains("select all")))
            toRemove.append(action);
    }
    for (auto *action : toRemove)
        menu->removeAction(action);

    for (QAction *action : menu->actions()) {
        if (action->isSeparator())
            continue;
        QString normalizedText = action->text().remove('&').toLower();
        if (normalizedText.contains("cut")) {
            action->setIcon(Icons::icon(Design::Icon::Cut));
            applyHotkeyToAction(action, Editor::Cut);
        }
        else if (normalizedText.contains("copy")) {
            action->setIcon(Icons::icon(Design::Icon::Copy));
            applyHotkeyToAction(action, Editor::Copy);
        }
        else if (normalizedText.contains("paste")) {
            action->setIcon(Icons::icon(Design::Icon::Paste));
            applyHotkeyToAction(action, Editor::Paste);
        }
        else if (normalizedText.contains("delete"))
            action->setIcon(Icons::icon(Design::Icon::Delete));
    }
}

void NoteEditor::applyHotkeyToAction(QAction *action, ActionId actionId)
{
    auto it = m_hotkeys.constFind(actionId);
    if (it != m_hotkeys.constEnd() && !it->isEmpty())
        action->setShortcut(*it);
}

void NoteEditor::buildNoteActionsMenu(QMenu *menu, const QPoint &globalPos)
{
    menu->addSeparator();
    auto *newAction
        = menu->addAction(Icons::icon(Design::Icon::New), "New note", this, &NoteEditor::createNoteRequested);
    applyHotkeyToAction(newAction, Notes::New);
    auto *deleteAction
        = menu->addAction(Icons::icon(Design::Icon::Delete), "Delete note", this, &NoteEditor::deleteNoteRequested);
    applyHotkeyToAction(deleteAction, Notes::Delete);

    menu->addSeparator();
    auto *undoAction = menu->addAction(Icons::icon(Design::Icon::Undo), "Undo", this, &QTextEdit::undo);
    undoAction->setEnabled(document()->isUndoAvailable());
    applyHotkeyToAction(undoAction, Editor::Undo);
    auto *redoAction = menu->addAction(Icons::icon(Design::Icon::Redo), "Redo", this, &QTextEdit::redo);
    redoAction->setEnabled(document()->isRedoAvailable());
    applyHotkeyToAction(redoAction, Editor::Redo);

    menu->addSeparator();
    auto *ontopAction = menu->addAction(Icons::icon(Design::Icon::Ontop),
                                        m_alwaysOnTop ? "Disable always on top" : "Enable always on top");
    ontopAction->setCheckable(true);
    ontopAction->setChecked(m_alwaysOnTop);
    applyHotkeyToAction(ontopAction, Notes::Ontop);
    connect(ontopAction, &QAction::triggered, this, [this]() { emit alwaysOnTopRequested(); });

    auto *toolbarAction
        = menu->addAction(Icons::icon(Design::Icon::Tool), m_toolbarVisible ? "Hide toolbar" : "Show toolbar");
    toolbarAction->setCheckable(true);
    toolbarAction->setChecked(m_toolbarVisible);
    applyHotkeyToAction(toolbarAction, Notes::ToolbarToggle);
    connect(toolbarAction, &QAction::triggered, this, [this]() { emit toolbarToggleRequested(); });

    auto *hideNoteAction = menu->addAction(Icons::icon(Design::Icon::HideNote), "Hide note");
    applyHotkeyToAction(hideNoteAction, Notes::Hide);
    connect(hideNoteAction, &QAction::triggered, this, [this]() { emit hideNoteRequested(); });

    auto *hideAllAction = menu->addAction(Icons::icon(Design::Icon::HideAllNotes), "Hide all notes");
    applyHotkeyToAction(hideAllAction, Notes::HideAll);
    connect(hideAllAction, &QAction::triggered, this, [this]() { emit hideAllNotesRequested(); });

    menu->addSeparator();
    auto *findAction = menu->addAction(Icons::icon(Design::Icon::Search), "Find", this, &NoteEditor::findRequested);
    applyHotkeyToAction(findAction, Editor::Find);

    menu->addSeparator();
    menu->addAction(Icons::icon(Design::Icon::Settings), "Note preferences...", this, [this, globalPos]() {
        emit noteSettingsRequested(globalPos);
    });
    menu->addAction(Icons::icon(Design::Icon::Settings), "Preferences...", this, &NoteEditor::preferencesRequested);
}

void NoteEditor::buildFormattingActionsMenu(QMenu *menu)
{
    menu->addSeparator();

    auto addToggle = [&](const QIcon &icon, const QString &name, auto predicate, auto setter) -> QAction * {
        auto *action = menu->addAction(icon, name);
        action->setCheckable(true);
        action->setChecked(selectionAllHave(predicate));
        connect(action, &QAction::triggered, this, [this, action, setter]() {
            QTextCharFormat format;
            setter(format, action->isChecked());
            mergeCurrentCharFormat(format);
            setFocus();
        });
        return action;
    };

    {
        auto *action = addToggle(Icons::icon(Design::Icon::Bold), "Bold", isBold, [](QTextCharFormat &format, bool on) {
            format.setFontWeight(on ? QFont::Bold : QFont::Normal);
        });
        applyHotkeyToAction(action, Editor::Bold);
    }
    {
        auto *action
            = addToggle(Icons::icon(Design::Icon::Italic), "Italic", isItalic, [](QTextCharFormat &format, bool on) {
                  format.setFontItalic(on);
              });
        applyHotkeyToAction(action, Editor::Italic);
    }
    {
        auto *action = addToggle(Icons::icon(Design::Icon::Underline),
                                 "Underline",
                                 isUnderline,
                                 [](QTextCharFormat &format, bool on) { format.setFontUnderline(on); });
        applyHotkeyToAction(action, Editor::Underline);
    }
    {
        auto *action = addToggle(Icons::icon(Design::Icon::Strikethrough),
                                 "Strikethrough",
                                 isStrike,
                                 [](QTextCharFormat &format, bool on) { format.setFontStrikeOut(on); });
        applyHotkeyToAction(action, Editor::Strikethrough);
    }

    menu->addSeparator();
    {
        auto *action = menu->addAction(
            Icons::icon(Design::Icon::ClearFormat), "Clear formatting", this, &NoteEditor::clearFormatting);
        applyHotkeyToAction(action, Editor::ClearFormat);
    }
}

bool NoteEditor::event(QEvent *event)
{
    if (event->type() == QEvent::ContextMenu) {
        contextMenuEvent(static_cast<QContextMenuEvent *>(event));
        return true;
    }
    return QTextEdit::event(event);
}

void NoteEditor::contextMenuEvent(QContextMenuEvent *event)
{
    qDebug() << "NE::contextMenuEvent called, locked=" << m_locked << "tempEdit=" << m_tempEdit;
    if (m_locked && !m_tempEdit)
        return;

    bool  hasSelection = textCursor().hasSelection();
    auto *menu         = createStandardContextMenu();

    filterStandardMenuActions(menu, hasSelection);

    if (!hasSelection)
        buildNoteActionsMenu(menu, event->globalPos());

    if (hasSelection && !m_locked)
        buildFormattingActionsMenu(menu);

    menu->exec(event->globalPos());
    delete menu;
    qDebug() << "NE: calling event->accept()";
    event->accept();
}

void NoteEditor::setAlwaysOnTop(bool on)
{
    m_alwaysOnTop = on;
}

void NoteEditor::setToolbarVisible(bool on)
{
    m_toolbarVisible = on;
}

void NoteEditor::setHotkey(ActionId actionId, const QKeySequence &seq)
{
    m_hotkeys[actionId] = seq;
}

void NoteEditor::mousePressEvent(QMouseEvent *event)
{
    QTextEdit::mousePressEvent(event);
}

void NoteEditor::mouseMoveEvent(QMouseEvent *event)
{
    if (m_locked && m_tempEdit && m_pressPos >= 0 && (event->buttons() & Qt::LeftButton)) {
        QTextCursor cursor(document());
        cursor.setPosition(m_pressPos);
        int endPos = cursorForPosition(event->pos()).position();
        cursor.setPosition(endPos, QTextCursor::KeepAnchor);
        setTextCursor(cursor);
        return;
    }

    QTextEdit::mouseMoveEvent(event);
}

void NoteEditor::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (m_locked && !m_tempEdit) {
        enterTempEdit();
        m_pressPos = cursorForPosition(event->pos()).position();
        QTextCursor cursor(document());
        cursor.setPosition(m_pressPos);
        setTextCursor(cursor);
        return;
    }
    QTextEdit::mouseDoubleClickEvent(event);
}

void NoteEditor::mouseReleaseEvent(QMouseEvent *event)
{
    QTextEdit::mouseReleaseEvent(event);
    if (m_locked && m_tempEdit)
        m_pressPos = -1;
}

void NoteEditor::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape && m_tempEdit) {
        exitTempEdit();
        return;
    }

    QKeySequence seq(event->key() | event->modifiers());

    // Intercept custom shortcuts for clipboard/history actions
    // to override QTextEdit's native handling
    for (auto it = m_hotkeys.constBegin(); it != m_hotkeys.constEnd(); ++it) {
        if (it.value() != seq)
            continue;
        const ActionId &id = it.key();
        if (id == Editor::Cut) {
            cut();
            return;
        }
        if (id == Editor::Copy) {
            copy();
            return;
        }
        if (id == Editor::Paste) {
            paste();
            return;
        }
        if (id == Editor::Undo) {
            undo();
            return;
        }
        if (id == Editor::Redo) {
            redo();
            return;
        }
        break;
    }

    // Block native QTextEdit shortcuts for cut/copy/paste/undo/redo
    // These are always handled by our keybinding system instead
    if (seq == QKeySequence(QKeySequence::Cut) || seq == QKeySequence(QKeySequence::Copy)
        || seq == QKeySequence(QKeySequence::Paste) || seq == QKeySequence(QKeySequence::Undo)
        || seq == QKeySequence(QKeySequence::Redo))
        return;

    QTextEdit::keyPressEvent(event);
}

void NoteEditor::focusOutEvent(QFocusEvent *event)
{
    if (m_tempEdit)
        exitTempEdit();
    QTextEdit::focusOutEvent(event);
}

void NoteEditor::enterTempEdit()
{
    m_tempEdit = true;
    setReadOnly(false);
    viewport()->setCursor(Qt::IBeamCursor);
}

void NoteEditor::exitTempEdit()
{
    m_tempEdit = false;
    setReadOnly(true);
    viewport()->setCursor(Qt::ArrowCursor);
    QTextCursor cursor = textCursor();
    cursor.clearSelection();
    setTextCursor(cursor);
}

void NoteEditor::moveCursorToEnd()
{
    QTextCursor cursor(document());
    cursor.movePosition(QTextCursor::End);
    setTextCursor(cursor);
}

void NoteEditor::emitState()
{
    emit stateChanged(currentState());
}
