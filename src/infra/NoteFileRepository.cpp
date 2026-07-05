#include "NoteFileRepository.h"

#include <QDebug>
#include <QDir>
#include <QFile>


namespace
{
const QString MARKDOWN_FILE_PATTERN = QStringLiteral("*.md");
}

NoteFileRepository::NoteFileRepository(const QString &notesDir)
    : m_notesDir(notesDir)
{
    QDir().mkpath(m_notesDir);
}

QList<Note> NoteFileRepository::getAll()
{
    QList<Note> notes;
    QDir        dir(m_notesDir);

    for (const QString &fileName : dir.entryList(QStringList(MARKDOWN_FILE_PATTERN), QDir::Files)) {
        QFile file(dir.filePath(fileName));
        if (!file.open(QIODevice::ReadOnly)) {
            qWarning() << "Failed to read note file:" << file.fileName() << file.errorString();
            continue;
        }
        Note props = Note::fromMarkdown(QString::fromUtf8(file.readAll()));
        props.setFilename(fileName);
        if (props.id().isEmpty())
            props.setId(QStringLiteral("orphan-") + fileName);
        notes.append(props);
    }
    return notes;
}

bool NoteFileRepository::saveOne(const Note &note)
{
    QDir  dir(m_notesDir);
    QFile file(dir.filePath(note.filename()));
    if (file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        file.write(note.toMarkdown().toUtf8());
        return true;
    }
    qWarning() << "Failed to save note:" << note.filename() << file.errorString();
    return false;
}

QString NoteFileRepository::nextFileName()
{
    QDir dir(m_notesDir);
    int  index = 1;
    while (true) {
        QString name = (index == 1) ? QStringLiteral("note.md") : QString(QStringLiteral("note %1.md")).arg(index);
        if (!dir.exists(name))
            return name;
        index++;
    }
}

bool NoteFileRepository::removeOne(const QString &filename)
{
    if (filename.isEmpty())
        return false;
    return QFile::remove(QDir(m_notesDir).filePath(filename));
}
