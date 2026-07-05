#include "NoteWindow.h"
#include "NoteViewModel.h"
#include "NoteEditor.h"
#include "AutohideController.h"
#include "toolbar/ToolbarWidget.h"
#include "ToolbarController.h"
#include "toolbar/ContextMenu.h"
#include "SearchBar.h"
#include "LockButton.h"
#include "PinButton.h"
#include "ClickThroughManager.h"
#include "AlwaysOnTopManager.h"
#include "WindowGeometryHelper.h"
#include "Icons.h"
#include "Design.h"
#include "ActionDefinition.h"
#include "ISettingsProvider.h"
#include "IKeybindingProvider.h"
#include "FramelessWindow.h"

#include <QVBoxLayout>
#include <QResizeEvent>
#include <QGraphicsOpacityEffect>
#include <QGuiApplication>
#include <QTextDocument>
#include <QTextCursor>
#include <QMessageBox>
#include <QContextMenuEvent>
#include <QShortcut>
#include <QPropertyAnimation>
#include "GhostIndicator.h"
#include "Settings.h"

namespace
{
// Private Use Area char used as placeholder to preserve tabs through Qt markdown serialization
constexpr QChar kTabSentinel        = QChar(0xE000);
constexpr int   kOpacityAnimationMs = 350;
} // namespace

NoteWindow::NoteWindow(NoteViewModel       *viewModel,
                       ISettingsProvider   *settingsProvider,
                       IKeybindingProvider *keybindings,
                       QWidget             *parent)
    : QWidget(parent)
    , m_viewModel(viewModel)
    , m_settingsProvider(settingsProvider)
    , m_keybindings(keybindings)
{
    setupUI();
    setMouseTracking(true);

    if (QGuiApplication::platformName() != "xcb") {
        m_opacityEffect = new QGraphicsOpacityEffect(this);
        m_opacityEffect->setOpacity(1.0);
        setGraphicsEffect(m_opacityEffect);
    }

    initWindow();
}

NoteWindow::~NoteWindow()
{
    qDeleteAll(m_shortcuts);
    m_shortcuts.clear();

    if (m_toolbar)
        m_toolbar->removeEventFilter(this);

    if (m_editor && m_editor->viewport())
        m_editor->viewport()->removeEventFilter(this);
}

void NoteWindow::setupUI()
{
    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    m_searchBar = new SearchBar;
    m_searchBar->setVisible(false);
    mainLayout->addWidget(m_searchBar);

    m_editor = new NoteEditor;
    mainLayout->addWidget(m_editor, 1);

    m_toolbar = new ToolbarWidget({}, this);
    mainLayout->addWidget(m_toolbar);

    m_toolbarController = new ToolbarController(m_editor, m_searchBar, m_toolbar, m_settingsProvider, this);
    m_toolbarController->init();

    m_lockBtn        = new LockButton(this);
    m_pinBtn         = new PinButton(this);
    m_ghostIndicator = new GhostIndicator(this);
    m_ghostIndicator->setVisible(false);
}

void NoteWindow::initWindow()
{
    loadContent();
    setupFramelessWindow();

    m_autohideController = new AutohideController(this);
    m_autohideController->setTimeout(m_viewModel->globalDefaults().autohideTimeout());
    m_toolbar->setVisible(false);

    connect(m_autohideController, &AutohideController::toolbarShown, this, [this]() {
        if (m_viewModel->isLocked())
            return;
        m_toolbar->setVisible(true);
        m_toolbar->raise();
        m_editor->setToolbarVisible(true);
        m_lockBtn->reposition();
        m_pinBtn->reposition();
        m_ghostIndicator->reposition();
        m_toolbarController->syncToolbarState(m_editor->currentState());
    });
    connect(m_autohideController, &AutohideController::toolbarHidden, this, [this]() {
        m_toolbar->setVisible(false);
        if (m_searchBar)
            m_searchBar->setVisible(false);
        m_toolbar->setButtonChecked(m_toolbarController->buttonId(ButtonId::Find), false);
        m_editor->setToolbarVisible(false);
        m_lockBtn->reposition();
        m_pinBtn->reposition();
        m_ghostIndicator->reposition();
    });

    m_toolbar->installEventFilter(this);
    m_toolbar->setMouseTracking(true);
    m_editor->viewport()->installEventFilter(this);
    m_editor->viewport()->setMouseTracking(true);

    applyGlobalSettings(m_viewModel->globalDefaults(), true);

    if (m_viewModel->isLocked()) {
        m_lockBtn->setChecked(true);
        m_editor->setLocked(true);
        m_toolbar->setVisible(false);
    }

    if (m_viewModel->isPinned()) {
        m_pinBtn->setChecked(true);
    }

    if (m_viewModel->isAlwaysOnTop())
        m_toolbar->setButtonChecked(m_toolbarController->buttonId(ButtonId::Ontop), true);

    setupConnections();
    registerShortcuts();
    setupGeometry();

    installEventFilter(this);

    m_lockBtn->reposition();
    m_pinBtn->reposition();

    saveNow();
}

void NoteWindow::loadContent()
{
    m_editor->setMarkdown(m_viewModel->content());
    m_editor->moveCursorToEnd();
    restoreTabs();

    m_editor->setBaseFontFamily(m_viewModel->fontFamily());
    m_editor->setBaseFontSize(m_viewModel->fontSize());
    m_editor->setBaseTextColor(m_viewModel->textColor());
    m_editor->setBaseBgColor(m_viewModel->bgColor());
}

void NoteWindow::restoreTabs()
{
    const QSignalBlocker blocker(m_editor->document());
    QTextCursor          cursor(m_editor->document());
    cursor.beginEditBlock();
    QTextCursor findCursor = m_editor->document()->find(kTabSentinel);
    while (!findCursor.isNull()) {
        findCursor.insertText(QStringLiteral("\t"));
        findCursor = m_editor->document()->find(kTabSentinel, findCursor);
    }
    cursor.endEditBlock();
    m_editor->document()->setModified(false);
}

void NoteWindow::setupFramelessWindow()
{
    m_windowController = new FramelessWindow(this);
    m_windowController->addDragHandle(m_toolbar);
    m_windowController->addDragHandle(m_editor->viewport(), true);
    m_windowController->setDragPredicate([this]() { return m_viewModel->isLocked() && !m_editor->isInTempEdit(); });
}

void NoteWindow::applyStyle()
{
    m_editor->setBaseFontFamily(m_viewModel->fontFamily());
    m_editor->setBaseFontSize(m_viewModel->fontSize());
    m_editor->setBaseTextColor(m_viewModel->textColor());
    m_editor->setBaseBgColor(m_viewModel->bgColor());
    setStyleSheet(QStringLiteral("NoteWindow { background-color: %1; }").arg(m_viewModel->bgColor().name()));
    applyWindowOpacity();
}

void NoteWindow::setupConnections()
{
    // Toolbar actions
    connect(m_toolbarController, &ToolbarController::newNoteRequested, this, &NoteWindow::newNoteRequested);
    connect(m_toolbarController, &ToolbarController::deleteNoteRequested, this, &NoteWindow::deleteNote);
    connect(
        m_toolbarController, &ToolbarController::alwaysOnTopToggled, this, [this](bool on) { toggleAlwaysOnTop(on); });
    connect(m_toolbarController, &ToolbarController::ghostModeToggled, this, [this](bool on) { toggleGhostMode(on); });
    connect(m_toolbarController, &ToolbarController::preferencesRequested, this, &NoteWindow::preferencesRequested);
    connect(m_toolbarController, &ToolbarController::zoomInRequested, this, &NoteWindow::zoomIn);
    connect(m_toolbarController, &ToolbarController::zoomOutRequested, this, &NoteWindow::zoomOut);

    // Document changes → auto-save
    connect(m_editor->document(), &QTextDocument::contentsChanged, this, [this]() {
        m_viewModel->markDirty();
        m_viewModel->scheduleSave();
    });

    // Lock/pin buttons
    connect(m_lockBtn, &QToolButton::clicked, this, &NoteWindow::toggleLock);
    connect(m_pinBtn, &QToolButton::clicked, this, &NoteWindow::togglePin);

    // Search bar
    connect(
        m_searchBar, &SearchBar::searchRequested, this, [this](const QString &text, bool) { m_lastSearchText = text; });

    // Style changes from ViewModel
    connect(m_viewModel, &NoteViewModel::styleChanged, this, [this]() {
        applyStyle();
        if (m_toolbar)
            m_toolbar->setIconSize(m_viewModel->hasCustomToolbarIconSize()
                                       ? m_viewModel->iconSize().value_or(m_viewModel->defaultToolbarIconSize())
                                       : m_viewModel->defaultToolbarIconSize());
        m_lockBtn->updateIconSize(toolbarIconSize());
        m_pinBtn->updateIconSize(toolbarIconSize());
    });

    // Editor-context actions
    connect(m_editor, &NoteEditor::createNoteRequested, this, &NoteWindow::newNoteRequested);
    connect(m_editor, &NoteEditor::deleteNoteRequested, this, [this]() { deleteNote(); });
    connect(m_editor, &NoteEditor::alwaysOnTopRequested, this, [this]() { toggleAlwaysOnTop(); });
    connect(m_editor, &NoteEditor::noteSettingsRequested, this, [this](const QPoint &pos) { showSettings(pos); });
    connect(m_editor, &NoteEditor::preferencesRequested, this, &NoteWindow::preferencesRequested);
    connect(m_editor, &NoteEditor::findRequested, this, [this]() { m_toolbarController->toggleSearch(); });
    connect(m_editor, &NoteEditor::toolbarToggleRequested, this, &NoteWindow::toggleToolbarVisibility);

    // Hide/show
    connect(m_editor, &NoteEditor::hideNoteRequested, this, &NoteWindow::handleHideNote);
    connect(m_editor, &NoteEditor::hideAllNotesRequested, this, &NoteWindow::hideAllNotesRequested);
    connect(m_toolbarController, &ToolbarController::hideNoteRequested, this, &NoteWindow::handleHideNote);
    connect(m_toolbarController, &ToolbarController::hideAllNotesRequested, this, &NoteWindow::hideAllNotesRequested);

    m_editor->setToolbarVisible(m_autohideController->isToolbarVisible());
}

void NoteWindow::setupGeometry()
{
    setWindowFlags(windowFlags() | Qt::FramelessWindowHint);
    AlwaysOnTopManager::applyWindowFlags(this, m_viewModel->isAlwaysOnTop());

    if (QGuiApplication::platformName() != "xcb")
        setAttribute(Qt::WA_TranslucentBackground);
    else
        setAttribute(Qt::WA_TranslucentBackground, false);

    WindowGeometryHelper::applyPosition(this, m_viewModel->snapshot());
}

bool NoteWindow::eventFilter(QObject *obj, QEvent *event)
{
    switch (event->type()) {
    // PERF: Ver como optiimzar/simplificar esto
    case QEvent::Enter:
        handleAutohideEnter(obj);
        break;
    case QEvent::Leave:
        handleAutohideLeave(obj);
        break;
    case QEvent::WindowActivate:
        if (obj == this)
            handleWindowActivated();
        break;
    case QEvent::WindowDeactivate:
        if (obj == this)
            handleWindowDeactivated();
        break;
    case QEvent::ContextMenu:
        qDebug() << "NW::eventFilter ContextMenu obj=" << obj << "==this?" << (obj == this);
        if (obj == this)
            return handleContextMenuEvent(static_cast<QContextMenuEvent *>(event));
        break;
    case QEvent::Show:
        if (obj == this)
            handleShowEvent();
        break;
    default:
        break;
    }
    return QWidget::eventFilter(obj, event);
}

bool NoteWindow::handleAutohideEnter(QObject *obj)
{
    if ((obj == m_toolbar || obj == m_editor->viewport()) && !m_viewModel->isLocked()
        && m_autohideController->isAutohideEnabled() && !m_autohideController->isUserHidden()) {
        m_autohideController->stopHideTimer();
        m_autohideController->showToolbar();
    }
    return false;
}

bool NoteWindow::handleAutohideLeave(QObject *obj)
{
    if ((obj == m_toolbar || obj == m_editor->viewport()) && m_autohideController->isAutohideEnabled()
        && m_autohideController->isToolbarVisible() && !m_inContextMenu)
        m_autohideController->startHideTimer();
    return false;
}

bool NoteWindow::handleWindowActivated()
{
    raise();
    m_autohideController->stopHideTimer();
    if (m_autohideController->isAutohideEnabled()) {
        if (rect().contains(mapFromGlobal(QCursor::pos())) && !m_autohideController->isUserHidden())
            m_autohideController->showToolbar();
        else if (m_autohideController->isToolbarVisible())
            m_autohideController->hideToolbar();
    }
    return false;
}

bool NoteWindow::handleWindowDeactivated()
{
    if (m_autohideController->isAutohideEnabled() && m_autohideController->isToolbarVisible()
        && !rect().contains(mapFromGlobal(QCursor::pos())))
        m_autohideController->startHideTimer();
    saveNow();
    return false;
}

bool NoteWindow::handleContextMenuEvent(QContextMenuEvent *event)
{
    qDebug() << "NW::handleContextMenuEvent called, isLocked=" << m_viewModel->isLocked();
    if (m_viewModel->isLocked())
        return false;
    m_inContextMenu = true;
    auto *menu      = findChild<ContextMenu *>();
    if (menu)
        connect(menu, &ContextMenu::destroyed, this, [this]() { m_inContextMenu = false; });
    showContextMenu(event->globalPos());
    return true;
}

bool NoteWindow::handleShowEvent()
{
    applyWindowOpacity();

    if (m_ghostMode) {
        m_windowController->setTransparentForInput(false);
        setAttribute(Qt::WA_TransparentForMouseEvents, false);
        ClickThroughManager::setClickThrough(this, false);
        QTimer::singleShot(0, this, [this]() {
            if (m_ghostMode) {
                m_windowController->setTransparentForInput(true);
                setAttribute(Qt::WA_TransparentForMouseEvents, true);
                ClickThroughManager::setClickThrough(this, true);
            }
        });
    }

    if (m_autohideController->isAutohideEnabled() && m_autohideController->isToolbarVisible()
        && !rect().contains(mapFromGlobal(QCursor::pos())))
        m_autohideController->hideToolbar();
    return false;
}

void NoteWindow::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    m_lockBtn->reposition();
    m_pinBtn->reposition();
    m_ghostIndicator->reposition();
}

void NoteWindow::saveNow()
{
    syncFromEditor();
    m_viewModel->save();
}

void NoteWindow::syncFromEditor()
{
    QString content = m_editor->toMarkdown();
    content.replace(QChar('\t'), kTabSentinel);
    m_viewModel->updateContent(content);

    auto [xPct, yPct] = WindowGeometryHelper::capturePositionPercent(this);
    m_viewModel->updateGeometry(width(), height(), xPct, yPct);
}

void NoteWindow::applyGlobalSettings(const Settings &settings, bool restoreAutohide)
{
    m_currentSettings = settings;
    if (restoreAutohide)
        setAutohide(settings.autohideToolbar());
    setAutohideTimeout(settings.autohideTimeout());
    m_viewModel->setGlobalDefaults(settings);

    if (!m_viewModel->hasCustomToolbarIconSize())
        setToolbarIconSize(settings.iconSize());

    applyStyle();
    m_toolbarController->reloadToolbarLayout(settings.toolbarLayout(), settings.toolbarLayoutVisibility());
}

void NoteWindow::setAutohide(bool on)
{
    m_autohideController->setEnabled(on);
    if (on && !isActiveWindow() && m_autohideController->isToolbarVisible()) {
        if (!rect().contains(mapFromGlobal(QCursor::pos())))
            m_autohideController->startHideTimer();
    }
}

void NoteWindow::setAutohideTimeout(int ms)
{
    m_autohideController->setTimeout(ms);
}

void NoteWindow::setToolbarIconSize(int iconSize)
{
    if (m_toolbar)
        m_toolbar->setIconSize(iconSize);
    m_editor->setBaseBgColor(m_viewModel->bgColor());
    const int size = m_toolbar ? m_toolbar->iconSize() : iconSize;
    m_lockBtn->updateIconSize(size);
    m_pinBtn->updateIconSize(size);
    m_ghostIndicator->updateIconSize(size);
}

int NoteWindow::toolbarIconSize() const
{
    return m_toolbar ? m_toolbar->iconSize() : m_viewModel->defaultToolbarIconSize();
}

void NoteWindow::showContextMenu(const QPoint &globalPos)
{
    auto *menu = m_toolbarController->createContextMenu(this);

    menu->addSeparator();
    menu->addEntry({Icons::icon(Design::Icon::Settings),
                    QStringLiteral("Note preferences..."),
                    false,
                    false,
                    [this, globalPos](bool) { showSettings(globalPos); }});

    menu->popup(globalPos);
}

void NoteWindow::zoomIn()
{
    int newSize = qMin(m_viewModel->fontSize() + 1, FontSizeLimits::kMax);
    m_viewModel->setFontSize(newSize);
    m_viewModel->scheduleSave();
}

void NoteWindow::zoomOut()
{
    int newSize = qMax(m_viewModel->fontSize() - 1, FontSizeLimits::kMin);
    m_viewModel->setFontSize(newSize);
    m_viewModel->scheduleSave();
}

void NoteWindow::toggleToolbarVisibility()
{
    if (m_autohideController->isToolbarVisible()) {
        m_autohideController->setUserHidden(true);
        m_autohideController->hideToolbar();
    }
    else {
        m_autohideController->setUserHidden(false);
        m_autohideController->showToolbar();
    }
}

void NoteWindow::toggleAlwaysOnTop(std::optional<bool> on)
{
    auto btnId    = m_toolbarController->buttonId(ButtonId::Ontop);
    bool targetOn = on.value_or(!m_toolbar->isButtonChecked(btnId));
    m_toolbar->setButtonChecked(btnId, targetOn);

    m_viewModel->updateContent(m_editor->toMarkdown());
    AlwaysOnTopManager::applyWindowFlags(this, targetOn);
    m_editor->setAlwaysOnTop(targetOn);
}

void NoteWindow::setGhostMode(bool on)
{
    toggleGhostMode(on);
}

void NoteWindow::toggleGhostMode(std::optional<bool> on)
{
    auto btnId    = m_toolbarController->buttonId(ButtonId::GhostMode);
    bool targetOn = on.value_or(!m_toolbar->isButtonChecked(btnId));
    m_toolbar->setButtonChecked(btnId, targetOn);
    m_ghostMode = targetOn;
    m_windowController->setTransparentForInput(targetOn);
    setAttribute(Qt::WA_TransparentForMouseEvents, targetOn);
    ClickThroughManager::setClickThrough(this, targetOn);

    if (targetOn) {
        m_autohideWasEnabled = m_autohideController->isAutohideEnabled();
        m_autohideController->setEnabled(false);
        m_autohideController->hideToolbar();
        m_ghostIndicator->setVisible(true);
    }
    else {
        m_ghostIndicator->setVisible(false);
        m_autohideController->setEnabled(m_autohideWasEnabled);
    }
}

void NoteWindow::deleteNote()
{
    auto result = QMessageBox::question(this,
                                        QStringLiteral("Delete note"),
                                        QStringLiteral("Delete this note?"),
                                        QMessageBox::Yes | QMessageBox::No,
                                        QMessageBox::No);
    if (result == QMessageBox::Yes)
        emit deleteRequested(this);
}

void NoteWindow::toggleLock()
{
    bool locked = !m_viewModel->isLocked();
    m_editor->setLocked(locked);
    m_lockBtn->setChecked(locked);
    m_viewModel->setLocked(locked);
    m_autohideController->hideToolbar();
    if (!locked)
        m_autohideController->showToolbar();
}

void NoteWindow::togglePin()
{
    bool wasPinned = m_viewModel->isPinned();
    bool pinned    = !wasPinned;
    m_pinBtn->setChecked(pinned);
    m_viewModel->setPinned(pinned);
    m_viewModel->scheduleSave();
    if (wasPinned && m_pinSoloMode)
        setPinSoloMode(false);
    else
        applyWindowOpacity();
}

bool NoteWindow::isPinned() const
{
    return m_viewModel->isPinned();
}

void NoteWindow::handleHideNote()
{
    if (isPinned() && !m_pinSoloMode)
        setPinSoloMode(true);
    else
        hide();
}

void NoteWindow::setPinSoloMode(bool on)
{
    if (m_pinSoloMode == on)
        return;
    m_pinSoloMode = on;
    applyWindowOpacity();
}

void NoteWindow::enterEvent(QEnterEvent *event)
{
    QWidget::enterEvent(event);
    m_mouseHovered = true;
    if (m_pinSoloMode)
        applyWindowOpacity();
}

void NoteWindow::leaveEvent(QEvent *event)
{
    QWidget::leaveEvent(event);
    m_mouseHovered = false;
    if (m_pinSoloMode)
        applyWindowOpacity();
}

void NoteWindow::showSettings(const QPoint &pos)
{
    m_autohideController->stopHideTimer();
    m_autohideController->showToolbar();
    m_inContextMenu = true;
    emit showSettingsRequested(m_viewModel, pos);
}

qreal NoteWindow::animatedOpacity() const
{
    if (m_opacityEffect)
        return m_opacityEffect->opacity();
    return windowOpacity();
}

void NoteWindow::setAnimatedOpacity(qreal op)
{
    if (m_opacityEffect)
        m_opacityEffect->setOpacity(op);
    else
        setWindowOpacity(op);
}

void NoteWindow::applyWindowOpacity()
{
    if (!isVisible())
        return;
    qreal target = static_cast<qreal>(m_viewModel->opacity()) / 100.0;
    if (m_pinSoloMode) {
        int op = m_mouseHovered ? m_currentSettings.pinHoverOpacity() : m_currentSettings.pinIdleOpacity();
        target = static_cast<qreal>(op) / 100.0;
    }
    qreal current = animatedOpacity();
    if (qFuzzyCompare(current, target))
        return;
    if (m_pinSoloMode) {
        if (!m_opacityAnimation) {
            m_opacityAnimation = new QPropertyAnimation(this, "animatedOpacity", this);
            m_opacityAnimation->setDuration(kOpacityAnimationMs);
        }
        if (m_opacityAnimation->state() == QAbstractAnimation::Running
            && qFuzzyCompare(m_opacityAnimation->endValue().toReal(), target))
            return;
        m_opacityAnimation->stop();
        m_opacityAnimation->setEasingCurve(QEasingCurve::Linear);
        m_opacityAnimation->setStartValue(current);
        m_opacityAnimation->setEndValue(target);
        m_opacityAnimation->start();
    }
    else {
        if (m_opacityAnimation)
            m_opacityAnimation->stop();
        setAnimatedOpacity(target);
    }
}

void NoteWindow::registerShortcuts()
{
    qDeleteAll(m_shortcuts);
    m_shortcuts.clear();

    QHash<ActionId, std::function<void()>> callbacks;

    callbacks[Notes::Lock]          = [this]() { toggleLock(); };
    callbacks[Notes::Ontop]         = [this]() { toggleAlwaysOnTop(); };
    callbacks[Notes::ToolbarToggle] = [this]() { toggleToolbarVisibility(); };

    callbacks[Notes::New]     = [this]() { emit newNoteRequested(); };
    callbacks[Notes::Delete]  = [this]() { deleteNote(); };
    callbacks[Notes::Hide]    = [this]() { handleHideNote(); };
    callbacks[Notes::HideAll] = [this]() { emit hideAllNotesRequested(); };
    callbacks[Notes::Prefs]   = [this]() { emit preferencesRequested(); };

    callbacks[Editor::Cut]   = [this]() { m_editor->cut(); };
    callbacks[Editor::Copy]  = [this]() { m_editor->copy(); };
    callbacks[Editor::Paste] = [this]() { m_editor->paste(); };
    callbacks[Editor::Undo]  = [this]() { m_editor->undo(); };
    callbacks[Editor::Redo]  = [this]() { m_editor->redo(); };

    callbacks[Editor::Bold]          = [this]() { m_editor->toggleBold(); };
    callbacks[Editor::Italic]        = [this]() { m_editor->toggleItalic(); };
    callbacks[Editor::Underline]     = [this]() { m_editor->toggleUnderline(); };
    callbacks[Editor::Strikethrough] = [this]() { m_editor->toggleStrikethrough(); };
    callbacks[Editor::ClearFormat]   = [this]() { m_editor->clearFormatting(); };

    callbacks[Editor::Find]     = [this]() { m_toolbarController->toggleSearch(); };
    callbacks[Editor::FindNext] = [this]() {
        if (m_searchBar->isVisible() && !m_lastSearchText.isEmpty()) {
            m_editor->find(m_lastSearchText);
        }
        else {
            m_toolbarController->toggleSearch();
        }
    };
    callbacks[Editor::FindPrev] = [this]() {
        if (m_searchBar->isVisible() && !m_lastSearchText.isEmpty()) {
            m_editor->find(m_lastSearchText, QTextDocument::FindBackward);
        }
        else {
            m_toolbarController->toggleSearch();
        }
    };

    callbacks[Editor::ZoomIn]  = [this]() { zoomIn(); };
    callbacks[Editor::ZoomOut] = [this]() { zoomOut(); };

    for (const auto &def : allActionDefinitions()) {
        m_editor->setHotkey(def.id, m_keybindings->shortcut(def.id));
    }

    for (const auto &def : allActionDefinitions()) {
        auto it = callbacks.constFind(def.id);
        if (it == callbacks.constEnd())
            continue;
        QKeySequence seq = m_keybindings->shortcut(def.id);
        if (seq.isEmpty())
            continue;
        auto *sc = new QShortcut(seq, this);
        connect(sc, &QShortcut::activated, this, *it);
        m_shortcuts.append(sc);
    }

    m_keybindings->onBindingsChanged(this, [this]() { registerShortcuts(); });
}
