#include "Paths.h"

#include <QCoreApplication>
#include <QStandardPaths>

namespace Paths
{

QString configDir()
{
#ifdef TN_DEBUG
    return QCoreApplication::applicationDirPath() + QStringLiteral("/data/config");
#else
    return QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
#endif
}

} // namespace Paths
