#include "ClickThroughManager.h"
#include <QWidget>
#include <QWindow>

#ifdef Q_OS_MACOS
#import <AppKit/AppKit.h>

void ClickThroughManager::setClickThrough(QWidget *window, bool enable)
{
    if (!window)
        return;

    QWindow *qWin = window->windowHandle();
    if (!qWin)
        return;

    NSView *nsView = reinterpret_cast<NSView *>(qWin->winId());
    if (!nsView)
        return;

    NSWindow *nswindow = [nsView window];
    if (!nswindow)
        return;

    [nswindow setIgnoresMouseEvents:enable ? YES : NO];
}

bool ClickThroughManager::isAvailable()
{
    return true;
}

#else

void ClickThroughManager::setClickThrough(QWidget *, bool) {}

bool ClickThroughManager::isAvailable()
{
    return false;
}

#endif
