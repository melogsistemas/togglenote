#pragma once

#include "INoteController.h"

class NoteViewModel;
class NoteWindow;
class INoteService;
class ISettingsProvider;
class IKeybindingProvider;
struct Note;

/// INoteController implementation. Constructs NoteViewModel + NoteWindow,
/// bridges window signals to higher-level signals for NoteManager.
class NoteController : public INoteController
{
    Q_OBJECT
public:
    NoteController(INoteService        *service,
                   ISettingsProvider   *settingsProvider,
                   IKeybindingProvider *keybindings,
                   QObject             *parent = nullptr);
    ~NoteController() override;

    void init(const Note &data);

    NoteWindow *window() const
    {
        return m_window;
    }
    NoteViewModel *viewModel() const
    {
        return m_viewModel;
    }

    void     saveNow() override;
    void     showWindow() override;
    void     hideWindow() override;
    bool     isWindowVisible() const override;
    QWidget *noteWidget() const override;
    void     applyGlobalSettings(const Settings &settings, bool restoreAutohide) override;
    void     setAutohide(bool on) override;
    bool     isPinned() const override;
    void     setPinSoloMode(bool on) override;
    void     setGhostMode(bool on) override;
    bool     isGhostMode() const override;

private slots:
    void onDeleteRequested();
    void onShowSettings(const QPoint &pos);

private:
    INoteService        *m_service;
    ISettingsProvider   *m_settingsProvider;
    IKeybindingProvider *m_keybindings;
    NoteViewModel       *m_viewModel{nullptr};
    NoteWindow          *m_window{nullptr};
};
