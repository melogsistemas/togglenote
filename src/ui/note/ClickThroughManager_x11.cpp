#include "ClickThroughManager.h"
#include <QWidget>
#include <QGuiApplication>
#include <QDebug>
#include <X11/Xlib.h>
#include <X11/extensions/shape.h>

static bool isWayland()
{
    static const bool wayland = (QGuiApplication::platformName() == "wayland");
    return wayland;
}

static Display *qtDisplay()
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 2, 0)
    if (auto *x11App = qGuiApp->nativeInterface<QNativeInterface::QX11Application>())
        return x11App->display();
    return nullptr;
#else
    return QX11Info::display();
#endif
}

static bool isShapeAvailable()
{
    if (isWayland())
        return false;
    static bool available = []() {
        Display *dpy = qtDisplay();
        if (!dpy)
            return false;
        int eventBase, errorBase;
        return XShapeQueryExtension(dpy, &eventBase, &errorBase) != 0;
    }();
    return available;
}

void ClickThroughManager::setClickThrough(QWidget *window, bool enable)
{
    if (!window || !isShapeAvailable())
        return;

    Display *dpy = qtDisplay();
    Window win = static_cast<Window>(window->winId());

    if (enable) {
        Region region = XCreateRegion();
        XShapeCombineRegion(dpy, win, ShapeInput, 0, 0, region, ShapeSet);
        XDestroyRegion(region);
    }
    else {
        XShapeCombineMask(dpy, win, ShapeInput, 0, 0, None, ShapeSet);
    }
    XFlush(dpy);
}

bool ClickThroughManager::isAvailable()
{
    return isShapeAvailable();
}
