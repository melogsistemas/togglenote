#include "Application.h"

#include <QDir>
#include <QSettings>
#include <QFileInfo>
#include <QStandardPaths>

int main(int argc, char *argv[])
{
    if (qEnvironmentVariableIsSet("WAYLAND_DISPLAY")) {
        QString configDir;
#ifdef TN_DEBUG
        configDir = QFileInfo(QString::fromLocal8Bit(argv[0])).absolutePath() + QStringLiteral("/data/config");
#else
        configDir
            = QStandardPaths::writableLocation(QStandardPaths::GenericConfigLocation) + QStringLiteral("/togglenote");
#endif
        QSettings s(configDir + QStringLiteral("/settings.conf"), QSettings::IniFormat);
        if (s.value(QStringLiteral("General/forceXcbOnWayland"), true).toBool())
            qputenv("QT_QPA_PLATFORM", "xcb");
    }

    Application app(argc, argv);
    if (!app.init())
        return 0;
    return app.exec();
}
