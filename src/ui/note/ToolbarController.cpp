#include "ToolbarController.h"
#include "ContextMenu.h"

#include "NoteEditor.h"
#include "SearchBar.h"
#include "Icons.h"
#include "Design.h"
#include "ActionDefinition.h"
#include "ISettingsProvider.h"
#include "Settings.h"

#include <QSet>
#include <QTextDocument>
#include <QTextCursor>


ToolbarController::ToolbarController(NoteEditor        *editor,
                                     SearchBar         *searchBar,
                                     ToolbarWidget     *toolbar,
                                     ISettingsProvider *settingsProvider,
                                     QObject           *parent)
    : QObject(parent)
    , m_toolbar(toolbar)
    , m_editor(editor)
    , m_searchBar(searchBar)
    , m_settingsProvider(settingsProvider)
{
    const Settings &s  = m_settingsProvider->settings();
    m_toolbarLayout    = s.toolbarLayout();
    m_toolbarLayoutVis = s.toolbarLayoutVisibility();
}

void ToolbarController::init()
{
    m_specs = buildToolbarSpec();
    m_toolbar->setButtons(m_specs);

    connect(m_editor, &NoteEditor::stateChanged, this, &ToolbarController::syncToolbarState);
    connect(m_toolbar, &ToolbarWidget::visibleButtonsChanged, this, &ToolbarController::saveLayoutVisibility);

    applyLayoutVisibility();
    syncToolbarState(m_editor->currentState());

    connect(m_searchBar, &SearchBar::searchRequested, this, &ToolbarController::onSearchRequested);
    connect(m_searchBar, &SearchBar::closed, this, &ToolbarController::onSearchClosed);
}

QList<QList<ToolbarButton>> ToolbarController::buildToolbarSpec()
{
    const QList<ActionDefinition> allDefs = allActionDefinitions();

    QHash<ActionId, const ActionDefinition *> defsById;
    for (const auto &def : allDefs)
        defsById.insert(def.id, &def);

    QHash<ActionId, std::function<void(bool)>> callbacks;
    callbacks[Notes::Hide]           = [this](bool) { emit hideNoteRequested(); };
    callbacks[Notes::HideAll]        = [this](bool) { emit hideAllNotesRequested(); };
    callbacks[Notes::New]            = [this](bool) { emit newNoteRequested(); };
    callbacks[Notes::Delete]         = [this](bool) { emit deleteNoteRequested(); };
    callbacks[Notes::Prefs]          = [this](bool) { emit preferencesRequested(); };
    callbacks[Editor::ZoomIn]        = [this](bool) { emit zoomInRequested(); };
    callbacks[Editor::ZoomOut]       = [this](bool) { emit zoomOutRequested(); };
    callbacks[Editor::Cut]           = [this](bool) { m_editor->cut(); };
    callbacks[Editor::Copy]          = [this](bool) { m_editor->copy(); };
    callbacks[Editor::Paste]         = [this](bool) { m_editor->paste(); };
    callbacks[Editor::Undo]          = [this](bool) { m_editor->undo(); };
    callbacks[Editor::Redo]          = [this](bool) { m_editor->redo(); };
    callbacks[Editor::Find]          = [this](bool) { toggleSearch(); };
    callbacks[Editor::Bold]          = [this](bool) { m_editor->toggleBold(); };
    callbacks[Editor::Italic]        = [this](bool) { m_editor->toggleItalic(); };
    callbacks[Editor::Underline]     = [this](bool) { m_editor->toggleUnderline(); };
    callbacks[Editor::Strikethrough] = [this](bool) { m_editor->toggleStrikethrough(); };
    callbacks[Editor::ClearFormat]   = [this](bool) { m_editor->clearFormatting(); };
    callbacks[Notes::Ontop]
        = [this](bool) { emit alwaysOnTopToggled(m_toolbar->isButtonChecked(buttonId(ButtonId::Ontop))); };
    callbacks[Global::GhostMode]
        = [this](bool) { emit ghostModeToggled(m_toolbar->isButtonChecked(buttonId(ButtonId::GhostMode))); };

    m_layoutItemIds.clear();
    m_widgetIdToActionId.clear();

    QList<QList<ToolbarButton>> spec;
    QList<ToolbarButton>        currentGroup;
    bool                        inRightSection = false;

    for (const auto &item : m_toolbarLayout) {
        if (item == QStringLiteral("|")) {
            if (!currentGroup.isEmpty()) {
                spec.append(currentGroup);
                currentGroup.clear();
            }
            continue;
        }

        if (item == QStringLiteral(">>")) {
            if (!currentGroup.isEmpty()) {
                spec.append(currentGroup);
                currentGroup.clear();
            }
            inRightSection = true;
            continue;
        }

        ActionId actionId = ActionId::fromString(item);
        auto     defIt    = defsById.constFind(actionId);
        if (defIt == defsById.constEnd())
            continue;
        const ActionDefinition *def = *defIt;

        if (def->btn == ButtonId::None)
            continue;
        auto cbIt = callbacks.constFind(def->id);
        if (cbIt == callbacks.constEnd())
            continue;

        m_layoutItemIds.insert(def->id);

        Design::Icon iconType = def->icon;

        ToolbarButton btn;
        btn.id = QString::number(static_cast<int>(def->btn));
        if (m_widgetIdToActionId.contains(btn.id))
            continue;
        btn.iconForSize  = [iconType](int size) { return Icons::icon(iconType, size); };
        btn.tooltip      = def->label;
        btn.checkable    = def->checkable;
        btn.rightAligned = inRightSection;
        btn.callback     = *cbIt;
        m_widgetIdToActionId.insert(btn.id, def->id);
        m_buttonIds[def->btn] = btn.id;

        currentGroup.append(btn);
    }

    if (!currentGroup.isEmpty())
        spec.append(currentGroup);

    return spec;
}

void ToolbarController::syncToolbarState(const EditorState &editorState)
{
    m_toolbar->setButtonChecked(buttonId(ButtonId::Bold), editorState.bold);
    m_toolbar->setButtonChecked(buttonId(ButtonId::Italic), editorState.italic);
    m_toolbar->setButtonChecked(buttonId(ButtonId::Underline), editorState.underline);
    m_toolbar->setButtonChecked(buttonId(ButtonId::Strike), editorState.strike);

    m_toolbar->setButtonEnabled(buttonId(ButtonId::Cut), editorState.hasSelection);
    m_toolbar->setButtonEnabled(buttonId(ButtonId::Copy), editorState.hasSelection);
    m_toolbar->setButtonEnabled(buttonId(ButtonId::Paste), editorState.pasteAvailable);
    m_toolbar->setButtonEnabled(buttonId(ButtonId::Undo), editorState.undoAvailable);
    m_toolbar->setButtonEnabled(buttonId(ButtonId::Redo), editorState.redoAvailable);
}

void ToolbarController::applyLayoutVisibility()
{
    const QSet<QString> &vis = m_toolbarLayoutVis;
    const QSignalBlocker blocker(m_toolbar);
    for (auto it = m_widgetIdToActionId.constBegin(); it != m_widgetIdToActionId.constEnd(); ++it)
        m_toolbar->setButtonVisible(it.key(), vis.contains(it.value().toString()));
}

QSet<ActionId> ToolbarController::layoutVisibility() const
{
    QSet<ActionId> result;
    for (auto it = m_widgetIdToActionId.constBegin(); it != m_widgetIdToActionId.constEnd(); ++it)
        if (m_toolbar->isButtonVisible(it.key()))
            result.insert(it.value());
    return result;
}

void ToolbarController::saveLayoutVisibility()
{
    Settings      settings = m_settingsProvider->settings();
    QSet<QString> vis;
    for (const auto &id : layoutVisibility())
        vis.insert(id.toString());
    settings.setToolbarLayoutVisibility(vis);
    m_settingsProvider->save(settings);
}

void ToolbarController::reloadToolbarLayout()
{
    const Settings &s  = m_settingsProvider->settings();
    m_toolbarLayout    = s.toolbarLayout();
    m_toolbarLayoutVis = s.toolbarLayoutVisibility();
    m_specs            = buildToolbarSpec();
    m_toolbar->setButtons(m_specs);
    applyLayoutVisibility();
    syncToolbarState(m_editor->currentState());
}

void ToolbarController::reloadToolbarLayout(const QStringList &layout, const QSet<QString> &vis)
{
    m_toolbarLayout    = layout;
    m_toolbarLayoutVis = vis;
    m_specs            = buildToolbarSpec();
    m_toolbar->setButtons(m_specs);
    applyLayoutVisibility();
    syncToolbarState(m_editor->currentState());
}

ContextMenu *ToolbarController::createContextMenu(QWidget *parent)
{
    auto *menu = new ContextMenu(parent);

    QHash<ActionId, Design::Icon> iconsById;
    for (const auto &def : allActionDefinitions())
        iconsById.insert(def.id, def.icon);

    bool firstGroup = true;
    for (const auto &group : m_specs) {
        if (group.isEmpty())
            continue;
        if (!firstGroup)
            menu->addSeparator();
        firstGroup = false;
        for (const auto &spec : group) {
            if (spec.id.isEmpty())
                continue;
            bool      visible = m_toolbar->isButtonVisible(spec.id);
            MenuEntry entry;
            entry.icon      = spec.iconForSize ? spec.iconForSize(16) : QIcon();
            entry.text      = spec.tooltip;
            entry.checkable = true;
            entry.checked   = visible;
            entry.onClick
                = [this, id = spec.id](bool) { m_toolbar->setButtonVisible(id, !m_toolbar->isButtonVisible(id)); };
            menu->addEntry(entry);
        }
    }
    return menu;
}

void ToolbarController::onSearchRequested(const QString &text, bool forward)
{
    if (text.isEmpty())
        return;
    QTextDocument::FindFlags flags;
    if (!forward)
        flags |= QTextDocument::FindBackward;
    if (!m_editor->find(text, flags)) {
        if (forward)
            m_editor->moveCursor(QTextCursor::Start);
        else
            m_editor->moveCursor(QTextCursor::End);
        m_editor->find(text, flags);
    }
}

void ToolbarController::onSearchClosed()
{
    m_searchBar->setVisible(false);
    m_toolbar->setButtonChecked(buttonId(ButtonId::Find), false);
    m_editor->setFocus();
    QTextCursor cursor = m_editor->textCursor();
    cursor.clearSelection();
    m_editor->setTextCursor(cursor);
}

void ToolbarController::toggleSearch()
{
    bool visible = !m_searchBar->isVisible();
    m_searchBar->setVisible(visible);
    m_toolbar->setButtonChecked(buttonId(ButtonId::Find), visible);
    if (visible)
        m_searchBar->focusInput();
    else
        m_editor->setFocus();
}
