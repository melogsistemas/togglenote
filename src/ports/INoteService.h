#pragma once

#include <QList>
#include <QString>
#include "Note.h"

class INoteService
{
public:
    virtual ~INoteService()                                 = default;
    virtual Note        createNote()                        = 0;
    virtual void        deleteNote(const QString &filename) = 0;
    virtual void        saveNote(const Note &note)          = 0;
    virtual QList<Note> getAllNotes()                       = 0;
};
