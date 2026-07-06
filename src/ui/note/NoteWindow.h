#pragma once

#include <QWidget>
#include <QTimer>
#include <QList>
#include <optional>
#include "Settings.h"

class QShortcut;
class QContextMenuEvent;
class QPropertyAnimation;
class QGraphicsOpacityEffect;

class NoteViewModel;
class NoteEditor;
class AutohideController;
class IToolbarController;
class SearchBar;
class LockButton;
class PinButton;
class GhostIndicator;
class ISettingsProvider;
class IKeybindingProvider;
class ToolbarWidget;
class FramelessWindow;
class ContextMenu;

/// The main note widget. Composes editor, toolbar, search bar, corner buttons,
/// frameless window manager, and autohide controller.
class NoteWindow : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(qreal animatedOpacity READ animatedOpacity WRITE setAnimatedOpacity)
public:
    explicit NoteWindow(NoteViewModel       *viewModel,
                        ISettingsProvider   *settingsProvider,
                        IKeybindingProvider *keybindings,
                        QWidget             *parent = nullptr);
    ~NoteWindow() override;

    void saveNow();
    void syncFromEditor();
    void applyGlobalSettings(const Settings &settings, bool restoreAutohide);
    void setAutohide(bool on);
    void setAutohideTimeout(int ms);
    void setToolbarIconSize(int iconSize);
    int  toolbarIconSize() const;
    void registerShortcuts();
    bool isPinned() const;
    void setPinSoloMode(bool on);
    void setGhostMode(bool on);
    bool isGhostMode() const
    {
        return m_ghostMode;
    }

    qreal animatedOpacity() const;
    void  setAnimatedOpacity(qreal op);

signals:
    void deleteRequested(NoteWindow *window);
    void preferencesRequested();
    void newNoteRequested();
    void showSettingsRequested(NoteViewModel *viewModel, const QPoint &pos);
    void hideAllNotesRequested();

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    void enterEvent(QEnterEvent *event) override;
#else
    void enterEvent(QEvent *event) override;
#endif
    void leaveEvent(QEvent *event) override;

private:
    void setupUI();
    void initWindow();
    void loadContent();
    void restoreTabs();
    void setupFramelessWindow();
    void setupConnections();
    void setupGeometry();

    void applyStyle();
    void showContextMenu(const QPoint &globalPos);
    void handleHideNote();

    void zoomIn();
    void zoomOut();
    void toggleToolbarVisibility();
    void toggleAlwaysOnTop(std::optional<bool> on = std::nullopt);
    void toggleGhostMode(std::optional<bool> on = std::nullopt);
    void deleteNote();
    void toggleLock();
    void togglePin();
    void showSettings(const QPoint &pos);
    void applyWindowOpacity();

    bool handleAutohideEnter(QObject *obj);
    bool handleAutohideLeave(QObject *obj);
    bool handleWindowActivated();
    bool handleWindowDeactivated();
    bool handleContextMenuEvent(QContextMenuEvent *event);
    bool handleShowEvent();

    NoteViewModel       *m_viewModel;
    ISettingsProvider   *m_settingsProvider;
    IKeybindingProvider *m_keybindings;
    NoteEditor          *m_editor{nullptr};
    SearchBar           *m_searchBar{nullptr};
    ToolbarWidget       *m_toolbar{nullptr};
    IToolbarController  *m_toolbarController{nullptr};
    FramelessWindow     *m_windowController{nullptr};
    LockButton          *m_lockBtn{nullptr};
    PinButton           *m_pinBtn{nullptr};
    GhostIndicator      *m_ghostIndicator{nullptr};

    AutohideController     *m_autohideController{nullptr};
    QGraphicsOpacityEffect *m_opacityEffect{nullptr};
    QPropertyAnimation     *m_opacityAnimation{nullptr};
    QList<QShortcut *>      m_shortcuts;
    QString                 m_lastSearchText;
    bool                    m_ghostMode{false};
    bool                    m_autohideWasEnabled{true};
    bool                    m_pinSoloMode{false};
    bool                    m_mouseHovered{false};
    bool                    m_inContextMenu{false};
    qreal                   m_savedOpacity{1.0};
    qreal                   m_ghostOpacityOverride{-1.0};
    Settings                m_currentSettings;
};
