#pragma once

#include <QList>
#include <QString>
#include "Note.h"

/// Business operations for note lifecycle management.
class INoteService
{
public:
    virtual ~INoteService() = default;
    /// Creates a new note with default content and returns it.
    virtual Note createNote() = 0;
    /// Deletes the note identified by @p filename.
    virtual void deleteNote(const QString &filename) = 0;
    /// Persists changes to an existing note.
    virtual void saveNote(const Note &note) = 0;
    /// Returns all notes currently on disk.
    virtual QList<Note> getAllNotes() = 0;
};
