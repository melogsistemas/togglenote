#pragma once

#include <QObject>
#include <QList>
#include <functional>
#include "Note.h"

class INoteController;
class INoteService;
class ISettingsProvider;
class IKeybindingProvider;

class NoteManager : public QObject
{
    Q_OBJECT
public:
    NoteManager(INoteService        *noteService,
                ISettingsProvider   *settingsProvider,
                IKeybindingProvider *keybindings,
                QObject             *parent = nullptr);
    ~NoteManager() override;

    void createNewNote(bool show = true);
    void restoreNotes(bool show = true);
    void saveAll();
    void showAll();
    void hideAll();
    void setAllHidden(bool on);
    void toggleAll();
    void setAllGhostMode(bool on);
    void toggleAllGhostMode();
    bool hasNoNotes() const;
    void forEachController(std::function<void(INoteController *)> func);

signals:
    void preferencesRequested();

private:
    void             onNoteDeleted(INoteController *controller);
    INoteController *createController(const Note &data, bool show = true);

    ISettingsProvider       *m_settingsProvider;
    IKeybindingProvider     *m_keybindings;
    INoteService            *m_noteService;
    QList<INoteController *> m_noteControllers;
    bool                     m_allHidden{false};
    int                      m_cascadeIndex{0};
};
