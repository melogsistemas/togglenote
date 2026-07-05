#include "WindowGeometryHelper.h"
#include "Note.h"

#include <QWidget>
#include <QScreen>
#include <QGuiApplication>
#include <QRect>
#include <QCursor>

void WindowGeometryHelper::applyPosition(QWidget *window, const Note &data)
{
    if (data.width() > 0 && data.height() > 0)
        window->resize(data.width(), data.height());

    QScreen *screen = QGuiApplication::screenAt(QCursor::pos());
    if (!screen)
        screen = QGuiApplication::primaryScreen();

    if (data.xPct() >= 0.0 && data.yPct() >= 0.0 && screen) {
        QRect geo  = screen->availableGeometry();
        int   posX = qBound(
            geo.left(), static_cast<int>(geo.left() + data.xPct() * geo.width()), geo.right() - window->width());
        int posY = qBound(
            geo.top(), static_cast<int>(geo.top() + data.yPct() * geo.height()), geo.bottom() - window->height());
        window->move(posX, posY);
    }
    else if (screen) {
        QRect geo = screen->availableGeometry();
        window->move(geo.center() - QRect(0, 0, data.width(), data.height()).center());
    }
}

QPair<double, double> WindowGeometryHelper::capturePositionPercent(QWidget *window)
{
    QScreen *screen = QGuiApplication::screenAt(window->frameGeometry().center());
    double   xPct = -1, yPct = -1;
    if (screen) {
        QRect geo = screen->availableGeometry();
        xPct      = qBound(0.0, static_cast<double>(window->pos().x() - geo.left()) / geo.width(), 1.0);
        yPct      = qBound(0.0, static_cast<double>(window->pos().y() - geo.top()) / geo.height(), 1.0);
    }
    return {xPct, yPct};
}
