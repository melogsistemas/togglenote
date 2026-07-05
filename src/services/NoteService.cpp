#include "NoteService.h"
#include "INoteRepository.h"

NoteService::NoteService(INoteRepository *repo)
    : m_repo(repo)
{}

Note NoteService::createNote()
{
    Note note;
    note.setFilename(m_repo->nextFileName());
    return note;
}

void NoteService::deleteNote(const QString &filename)
{
    m_repo->removeOne(filename);
}

void NoteService::saveNote(const Note &note)
{
    m_repo->saveOne(note);
}

QList<Note> NoteService::getAllNotes()
{
    return m_repo->getAll();
}
