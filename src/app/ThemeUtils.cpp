#include "ThemeUtils.h"
#include "constants.h"

#include <QApplication>
#include <QFile>

void ThemeUtils::loadTheme()
{
    QFile f(Constants::Paths::THEME);
    if (f.open(QIODevice::ReadOnly))
        qApp->setStyleSheet(f.readAll());
}
