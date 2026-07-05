#include "ClickThroughManager.h"
#include <QtGlobal>

#if !defined(Q_OS_WIN) && !defined(Q_OS_MACOS) && !defined(Q_OS_UNIX)

void ClickThroughManager::setClickThrough(QWidget *, bool)
{}

bool ClickThroughManager::isAvailable()
{
    return false;
}

#endif
