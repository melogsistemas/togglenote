#include "ClickThroughManager.h"
#include <QWidget>

#ifdef Q_OS_WIN
#include <windows.h>

void ClickThroughManager::setClickThrough(QWidget *window, bool enable)
{
    if (!window)
        return;

    HWND hwnd = reinterpret_cast<HWND>(window->winId());
    if (!hwnd)
        return;

    LONG_PTR exStyle = GetWindowLongPtrW(hwnd, GWL_EXSTYLE);
    int      alpha   = qBound(0, static_cast<int>(window->windowOpacity() * 255), 255);

    if (enable) {
        exStyle |= WS_EX_LAYERED | WS_EX_TRANSPARENT;
        // Cap at 254 — the Desktop Window Manager treats alpha=255 as "fully
        // opaque" and refuses to route mouse events through WS_EX_TRANSPARENT.
        // Without this cap, ghost mode silently fails when window opacity is
        // at the default 100% (alpha=255), even though WS_EX_TRANSPARENT is set.
        alpha = qMin(alpha, 254);
    }
    else {
        exStyle &= ~WS_EX_TRANSPARENT;
    }

    SetWindowLongPtrW(hwnd, GWL_EXSTYLE, exStyle);
    SetLayeredWindowAttributes(hwnd, 0, static_cast<BYTE>(alpha), LWA_ALPHA);
}

bool ClickThroughManager::isAvailable()
{
    return true;
}

#else

void ClickThroughManager::setClickThrough(QWidget *, bool)
{}

bool ClickThroughManager::isAvailable()
{
    return false;
}

#endif
