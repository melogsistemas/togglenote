#include "NoteController.h"
#include "NoteViewModel.h"
#include "NoteWindow.h"
#include "INoteService.h"
#include "ISettingsProvider.h"
#include "IKeybindingProvider.h"
#include "NoteSettingsController.h"


NoteController::NoteController(INoteService        *service,
                               ISettingsProvider   *settingsProvider,
                               IKeybindingProvider *keybindings,
                               QObject             *parent)
    : INoteController(parent)
    , m_service(service)
    , m_settingsProvider(settingsProvider)
    , m_keybindings(keybindings)
{}

void NoteController::init(const Note &data)
{
    m_viewModel = new NoteViewModel(data, m_service, m_settingsProvider->settings(), this);

    m_window = new NoteWindow(m_viewModel, m_settingsProvider, m_keybindings, nullptr);

    connect(m_window, &NoteWindow::deleteRequested, this, &NoteController::onDeleteRequested);
    connect(m_window, &NoteWindow::newNoteRequested, this, &NoteController::newNoteRequested);
    connect(m_window, &NoteWindow::preferencesRequested, this, &NoteController::preferencesRequested);
    connect(m_window, &NoteWindow::hideAllNotesRequested, this, &NoteController::hideAllNotesRequested);
    connect(m_window, &NoteWindow::showSettingsRequested, this, [this](NoteViewModel *, const QPoint &pos) {
        onShowSettings(pos);
    });
}

NoteController::~NoteController() = default;

void NoteController::saveNow()
{
    m_window->saveNow();
}

void NoteController::showWindow()
{
    if (m_window)
        m_window->show();
}

void NoteController::hideWindow()
{
    if (m_window)
        m_window->hide();
}

bool NoteController::isWindowVisible() const
{
    return m_window && m_window->isVisible();
}

QWidget *NoteController::noteWidget() const
{
    return m_window;
}

void NoteController::onDeleteRequested()
{
    QString filename = m_viewModel->filename();
    m_window->syncFromEditor();
    m_service->deleteNote(filename);
    m_window->deleteLater();
    emit noteDeleted(this);
}

void NoteController::onShowSettings(const QPoint &pos)
{
    auto *ctrl = new NoteSettingsController(
        m_viewModel, m_window, m_settingsProvider->settings(), [this](const QStringList &colors) {
            Settings settings = m_settingsProvider->settings();
            settings.setCustomColors(colors);
            m_settingsProvider->save(settings);
        });
    ctrl->showAt(pos);
}

void NoteController::applyGlobalSettings(const Settings &settings, bool restoreAutohide)
{
    m_window->applyGlobalSettings(settings, restoreAutohide);
}

void NoteController::setAutohide(bool on)
{
    m_window->setAutohide(on);
}

bool NoteController::isPinned() const
{
    return m_window && m_window->isPinned();
}

void NoteController::setPinSoloMode(bool on)
{
    if (m_window)
        m_window->setPinSoloMode(on);
}

void NoteController::setGhostMode(bool on)
{
    if (m_window)
        m_window->setGhostMode(on);
}

bool NoteController::isGhostMode() const
{
    return m_window && m_window->isGhostMode();
}
