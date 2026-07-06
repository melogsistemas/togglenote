#pragma once

#include <QList>
#include <QString>
#include "Note.h"

/// Persistence contract for note storage (flat-file markdown).
class INoteRepository
{
public:
    virtual ~INoteRepository() = default;
    /// Returns all saved notes.
    virtual QList<Note> getAll() = 0;
    /// Persists a note (insert or update). Returns false on failure.
    virtual bool saveOne(const Note &note) = 0;
    /// Deletes the note identified by @p filename. Returns false on failure.
    virtual bool removeOne(const QString &filename) = 0;
    /// Returns an unused filename for a new note.
    virtual QString nextFileName() = 0;
};
