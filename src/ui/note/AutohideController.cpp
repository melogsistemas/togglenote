#include "AutohideController.h"
#include <QCursor>
#include <QPoint>
#include <QRect>
#include <QWidget>

AutohideController::AutohideController(QObject *parent)
    : QObject(parent)
{
    m_hideTimer = new QTimer(this);
    m_hideTimer->setSingleShot(true);
    connect(m_hideTimer, &QTimer::timeout, this, [this]() {
        auto *w = qobject_cast<QWidget *>(this->parent());
        if (!w || !w->rect().contains(w->mapFromGlobal(QCursor::pos())))
            hideToolbar();
    });
}

void AutohideController::setEnabled(bool on)
{
    m_autohide = on;
    if (!on) {
        showToolbar();
        m_hideTimer->stop();
    }
}

void AutohideController::setTimeout(int ms)
{
    m_hideTimer->setInterval(ms);
}

void AutohideController::showToolbar()
{
    if (m_toolbarVisible)
        return;
    m_toolbarVisible = true;
    emit toolbarShown();
}

void AutohideController::hideToolbar()
{
    if (!m_toolbarVisible)
        return;
    m_toolbarVisible = false;
    m_hideTimer->stop();
    emit toolbarHidden();
}

void AutohideController::startHideTimer()
{
    m_hideTimer->start();
}

void AutohideController::stopHideTimer()
{
    m_hideTimer->stop();
}
