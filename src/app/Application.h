#pragma once

#include <QApplication>
#include <memory>
#include "Settings.h"
#include "CommandLineArgs.h"

class NoteManager;
class SettingsDialog;
class SettingsService;
class KeybindingService;
class NoteService;
class INoteRepository;
class INoteService;
class ISettingsRepository;
class OsSignalHandler;
class CurrentProcessManager;
class TrayIcon;
class GlobalHotkeyService;

class Application : public QApplication
{
    Q_OBJECT
public:
    Application(int &argc, char **argv);
    ~Application() override;

    bool init();

public slots:
    void showPreferences();

private:
    void processCliArgs(const CommandLineArgs &actions);
    void saveAllAndQuit();

    void setupSettings();
    void setupOsSignalHandler();
    bool setupCurrentProcessManager();
    void setupTrayIcon();
    void setupComponentConnections();

    std::unique_ptr<ISettingsRepository> m_settingsRepo;
    std::unique_ptr<INoteRepository>     m_noteRepo;
    std::unique_ptr<INoteService>        m_noteService;
    SettingsService                     *m_settingsManager{nullptr};
    KeybindingService                   *m_keybindings{nullptr};
    SettingsDialog                      *m_settingsDialog{nullptr};
    NoteManager                         *m_noteManager{nullptr};
    OsSignalHandler                     *m_sigHandler{nullptr};
    CurrentProcessManager               *m_instance{nullptr};
    TrayIcon                            *m_tray{nullptr};

    GlobalHotkeyService *m_globalHotkeyFilter{nullptr};

    CommandLineArgs m_cliArgs;

    bool m_savedBeforeQuit{false};
};
