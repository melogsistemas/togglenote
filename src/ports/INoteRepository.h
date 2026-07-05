#pragma once

#include <QList>
#include <QString>
#include "Note.h"

class INoteRepository
{
public:
    virtual ~INoteRepository()                             = default;
    virtual QList<Note> getAll()                           = 0;
    virtual bool        saveOne(const Note &note)          = 0;
    virtual bool        removeOne(const QString &filename) = 0;
    virtual QString     nextFileName()                     = 0;
};
