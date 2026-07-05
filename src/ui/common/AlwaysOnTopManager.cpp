#include "AlwaysOnTopManager.h"
#include <QWidget>

#ifdef Q_OS_WIN
#include <windows.h>
#endif

void AlwaysOnTopManager::applyWindowFlags(QWidget *window, bool onTop)
{
    bool wasVisible = window->isVisible();
    bool wasActive  = false;
    if (wasVisible) {
        wasActive = window->isActiveWindow();
        window->hide();
    }

    if (onTop)
        window->setWindowFlags(window->windowFlags() | Qt::WindowStaysOnTopHint);
    else
        window->setWindowFlags(window->windowFlags() & ~Qt::WindowStaysOnTopHint);

#ifdef Q_OS_WIN
    {
        HWND hwnd = (HWND) window->winId();
        SetWindowPos(hwnd, onTop ? HWND_TOPMOST : HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
    }
#endif

    if (!wasVisible)
        return;

    window->show();
    if (wasActive) {
        window->raise();
        window->activateWindow();
    }
}
