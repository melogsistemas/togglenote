#include "Settings.h"

#include <QCoreApplication>
#include <QStandardPaths>

namespace Defaults
{

QString notesDir()
{
#ifdef TN_DEBUG
    return QCoreApplication::applicationDirPath() + QStringLiteral("/data/notes");
#else
    return QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + QStringLiteral("/togglenotes");
#endif
}

} // namespace Defaults
