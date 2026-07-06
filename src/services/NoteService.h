#pragma once

#include "INoteService.h"

class INoteRepository;

/// INoteService implementation delegating to INoteRepository.
class NoteService : public INoteService
{
public:
    explicit NoteService(INoteRepository *repo);

    Note        createNote() override;
    void        deleteNote(const QString &filename) override;
    void        saveNote(const Note &note) override;
    QList<Note> getAllNotes() override;

private:
    INoteRepository *m_repo;
};
