#include "Application.h"
#include "NoteManager.h"
#include "INoteController.h"
#include "INoteRepository.h"
#include "INoteService.h"
#include "NoteFileRepository.h"
#include "NoteService.h"
#include "ThemeUtils.h"
#include "SettingsService.h"
#include "SettingsRepository.h"
#include "KeybindingService.h"
#include "ActionId.h"
#include "SettingsDialog.h"
#include "SettingsViewModel.h"
#include "OsSignalHandler.h"
#include "CurrentProcessManager.h"
#include "TrayIcon.h"
#include "GlobalHotkeyService.h"
#include "AlwaysOnTopManager.h"
#include "Paths.h"
#include "constants.h"

#include <QIcon>

Application::Application(int &argc, char **argv)
    : QApplication(argc, argv)
{
    setApplicationName(Constants::APP_NAME);
    setApplicationVersion(Constants::APP_VERSION);
    setWindowIcon(QIcon(QStringLiteral(":/logo.svg")));
    setQuitOnLastWindowClosed(false);
}

Application::~Application()
{
    if (!m_savedBeforeQuit && m_noteManager)
        m_noteManager->saveAll();
}

bool Application::init()
{
    setupSettings();
    ThemeUtils::loadTheme();
    setupOsSignalHandler();
    setupTrayIcon();

    if (setupCurrentProcessManager())
        return false;

    m_cliArgs = parseCommandLineArgs(arguments());
    if (m_cliArgs.quit || m_cliArgs.helpRequested)
        return false;

    m_noteRepo    = std::make_unique<NoteFileRepository>(m_settingsManager->settings().notesDir());
    m_noteService = std::make_unique<NoteService>(m_noteRepo.get());
    m_noteManager = new NoteManager(m_noteService.get(), m_settingsManager, m_keybindings, this);

    setupComponentConnections();

    m_globalHotkeyFilter = new GlobalHotkeyService(m_keybindings, this);
    installNativeEventFilter(m_globalHotkeyFilter);
    m_keybindings->setValidator([this](ActionId actionId, const QKeySequence &seq) {
        return m_globalHotkeyFilter->registerOne(actionId, seq);
    });
    connect(m_globalHotkeyFilter, &GlobalHotkeyService::triggered, this, [this](ActionId actionId) {
        if (actionId == Global::ToggleNotes)
            m_noteManager->toggleAll();
        else if (actionId == Global::QuickCapture)
            m_noteManager->createNewNote();
        else if (actionId == Global::GhostMode) {
            m_noteManager->toggleAllGhostMode();
            bool anyGhost = false;
            m_noteManager->forEachController(
                [&anyGhost](INoteController *ctrl) { anyGhost = anyGhost || ctrl->isGhostMode(); });
            m_tray->setGhostModeChecked(anyGhost);
        }
    });

    bool startHidden = m_settingsManager->settings().startHidden();

    m_noteManager->restoreNotes(!startHidden);

    if (m_noteManager->hasNoNotes() || m_settingsManager->settings().autostartNewNote())
        m_noteManager->createNewNote(!startHidden);

    if (startHidden)
        m_noteManager->setAllHidden(true);

    processCliArgs(m_cliArgs);
    return true;
}

void Application::setupSettings()
{
    m_settingsRepo    = std::make_unique<SettingsRepository>(Paths::configDir());
    m_settingsManager = new SettingsService(m_settingsRepo.get(), this);
    m_keybindings     = new KeybindingService(m_settingsManager, this);
}

void Application::setupOsSignalHandler()
{
    m_sigHandler = new OsSignalHandler(this);
    m_sigHandler->start();
}

bool Application::setupCurrentProcessManager()
{
    m_instance = new CurrentProcessManager(Constants::IPC::SERVER, this);
    if (m_instance->tryConnect(arguments()))
        return true;
    m_instance->startServer();
    return false;
}

void Application::setupTrayIcon()
{
    m_tray = new TrayIcon(this);
}

void Application::setupComponentConnections()
{
    connect(m_sigHandler, &OsSignalHandler::quitRequested, this, &Application ::saveAllAndQuit);

    connect(m_instance, &CurrentProcessManager::argsReceived, this, &Application ::processCliArgs);

    connect(m_noteManager, &NoteManager::preferencesRequested, this, &Application ::showPreferences);

    connect(m_tray, &TrayIcon::preferencesRequested, this, &Application ::showPreferences);
    connect(m_tray, &TrayIcon::quitRequested, this, &Application ::saveAllAndQuit);

    connect(m_tray, &TrayIcon::showAllRequested, m_noteManager, &NoteManager::showAll);
    connect(m_tray, &TrayIcon::hideAllRequested, m_noteManager, &NoteManager::hideAll);
    connect(m_tray, &TrayIcon::toggleRequested, m_noteManager, &NoteManager::toggleAll);

    connect(m_tray, &TrayIcon::ghostModeToggled, m_noteManager, &NoteManager::setAllGhostMode);

    connect(m_tray, &TrayIcon::newNoteRequested, this, [this]() { m_noteManager->createNewNote(); });

    connect(m_settingsManager, &SettingsService::settingsChanged, this, [this]() {
        const Settings &s = m_settingsManager->settings();
        m_keybindings->reload();
        m_noteManager->forEachController([&](INoteController *ctrl) { ctrl->applyGlobalSettings(s, false); });
    });

    connect(this, &QApplication::applicationStateChanged, this, [this](Qt::ApplicationState state) {
        if (state == Qt::ApplicationInactive)
            m_noteManager->forEachController([](INoteController *ctrl) { ctrl->saveNow(); });
    });
}

void Application::processCliArgs(const CommandLineArgs &args)
{
    if (args.quit)
        saveAllAndQuit();
    if (args.hide)
        m_noteManager->hideAll();
    if (args.show)
        m_noteManager->showAll();
    if (args.toggle)
        m_noteManager->toggleAll();
    if (args.createNote)
        m_noteManager->createNewNote();
    if (args.ghostMode) {
        m_noteManager->toggleAllGhostMode();
        bool anyGhost = false;
        m_noteManager->forEachController(
            [&anyGhost](INoteController *ctrl) { anyGhost = anyGhost || ctrl->isGhostMode(); });
        m_tray->setGhostModeChecked(anyGhost);
    }
}

void Application::saveAllAndQuit()
{
    m_noteManager->saveAll();
    m_savedBeforeQuit = true;
    quit();
}

void Application::showPreferences()
{
    if (m_settingsDialog) {
        m_settingsDialog->raise();
        m_settingsDialog->activateWindow();
        return;
    }

    auto *viewModel = new SettingsViewModel(m_settingsManager, m_keybindings, this);
    viewModel->load();
    m_settingsDialog = new SettingsDialog(viewModel);
    AlwaysOnTopManager::applyWindowFlags(m_settingsDialog, true);
    m_settingsDialog->setAttribute(Qt::WA_DeleteOnClose);

    connect(m_settingsDialog, &SettingsDialog::changed, this, [this](const Settings &s) {
        m_noteManager->forEachController([&](INoteController *ctrl) { ctrl->applyGlobalSettings(s, false); });
    });

    connect(m_settingsDialog, &QDialog::finished, this, [this, viewModel](int r) {
        m_settingsDialog = nullptr;
        if (r == QDialog::Rejected) {
            const Settings &s = viewModel->snapshot();
            m_noteManager->forEachController([&](INoteController *ctrl) { ctrl->applyGlobalSettings(s, true); });
        }
        else {
            const Settings &s = viewModel->settings();
            m_noteManager->forEachController([&](INoteController *ctrl) { ctrl->applyGlobalSettings(s, true); });
        }
    });

    m_noteManager->forEachController([&](INoteController *ctrl) { ctrl->setAutohide(false); });
    m_settingsDialog->show();
    m_settingsDialog->raise();
    m_settingsDialog->activateWindow();
}
