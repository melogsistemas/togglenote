#pragma once

#include "INoteRepository.h"

class NoteFileRepository : public INoteRepository
{
public:
    explicit NoteFileRepository(const QString &notesDir);

    QList<Note> getAll() override;
    bool        saveOne(const Note &note) override;
    bool        removeOne(const QString &filename) override;
    QString     nextFileName() override;

private:
    QString m_notesDir;
};
