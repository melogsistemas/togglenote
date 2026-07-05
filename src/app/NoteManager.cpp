#include "NoteManager.h"
#include "INoteController.h"
#include "NoteController.h"
#include "NoteWindow.h"
#include "INoteService.h"
#include "ISettingsProvider.h"
#include "IKeybindingProvider.h"

namespace
{
constexpr double kCascadeStep   = 0.04;
constexpr double kCascadeOffset = 0.34;
constexpr int    kCascadeMax    = 16;
} // namespace

NoteManager::NoteManager(INoteService        *noteService,
                         ISettingsProvider   *settingsProvider,
                         IKeybindingProvider *keybindings,
                         QObject             *parent)
    : QObject(parent)
    , m_settingsProvider(settingsProvider)
    , m_keybindings(keybindings)
    , m_noteService(noteService)
{}

NoteManager::~NoteManager() = default;

void NoteManager::createNewNote(bool show)
{
    Note data = m_noteService->createNote();
    data.setAlwaysOnTop(m_settingsProvider->settings().defaultOnTop());

    double offset = m_cascadeIndex * kCascadeStep;
    data.setXPct(kCascadeOffset + offset);
    data.setYPct(kCascadeOffset + offset);
    m_cascadeIndex = (m_cascadeIndex + 1) % kCascadeMax;

    createController(data, show);
}

void NoteManager::restoreNotes(bool show)
{
    QList<Note> notesData = m_noteService->getAllNotes();
    for (const Note &data : notesData)
        createController(data, show);
}

void NoteManager::saveAll()
{
    for (INoteController *ctrl : m_noteControllers)
        ctrl->saveNow();
}

void NoteManager::hideAll()
{
    for (INoteController *ctrl : m_noteControllers) {
        if (!ctrl->isPinned())
            ctrl->hideWindow();
        else
            ctrl->setPinSoloMode(true);
    }
    m_allHidden = true;
}

void NoteManager::showAll()
{
    for (INoteController *ctrl : m_noteControllers) {
        ctrl->setPinSoloMode(false);
        ctrl->showWindow();
    }
    m_allHidden = false;
}

void NoteManager::setAllHidden(bool on)
{
    m_allHidden = on;
}

void NoteManager::toggleAll()
{
    if (m_allHidden)
        showAll();
    else
        hideAll();
}

void NoteManager::setAllGhostMode(bool on)
{
    for (INoteController *ctrl : m_noteControllers)
        ctrl->setGhostMode(on);
}

void NoteManager::toggleAllGhostMode()
{
    bool anyGhost = false;
    for (INoteController *ctrl : m_noteControllers) {
        if (ctrl->isGhostMode()) {
            anyGhost = true;
            break;
        }
    }
    setAllGhostMode(!anyGhost);
}

bool NoteManager::hasNoNotes() const
{
    return m_noteControllers.isEmpty();
}

void NoteManager::forEachController(std::function<void(INoteController *)> func)
{
    for (INoteController *ctrl : m_noteControllers)
        func(ctrl);
}

void NoteManager::onNoteDeleted(INoteController *controller)
{
    m_noteControllers.removeOne(controller);
    controller->deleteLater();
}

INoteController *NoteManager::createController(const Note &data, bool show)
{
    auto *controller = new NoteController(m_noteService, m_settingsProvider, m_keybindings, this);
    controller->init(data);
    m_noteControllers.append(controller);

    connect(controller, &NoteController::newNoteRequested, this, [this]() { createNewNote(); });
    connect(controller, &NoteController::preferencesRequested, this, &NoteManager::preferencesRequested);
    connect(controller, &NoteController::noteDeleted, this, &NoteManager::onNoteDeleted);
    connect(controller, &NoteController::hideAllNotesRequested, this, &NoteManager::hideAll);

    if (show)
        controller->showWindow();

    return controller;
}
