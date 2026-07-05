#include "FramelessWindow.h"

#include <QWidget>
#include <QToolButton>
#include <QSizeGrip>
#include <QMouseEvent>
#include <QGuiApplication>
#include <QScreen>
#include <QApplication>
#include <QAbstractScrollArea>
#include <QWindow>
#include "ToolbarWidget.h"


FramelessWindow::FramelessWindow(QWidget *targetWindow)
    : QObject(targetWindow)
    , m_window(targetWindow)
{
    installFilterRecursive(m_window);
    QApplication::instance()->installEventFilter(this);
}

FramelessWindow::~FramelessWindow()
{
    QApplication::instance()->removeEventFilter(this);
}

void FramelessWindow::addDragHandle(QWidget *handle, bool dragOnMove)
{
    if (!handle || m_dragHandles.contains(handle))
        return;

    m_dragHandles.insert(handle);
    if (dragOnMove)
        m_moveDragHandles.insert(handle);
    installFilterRecursive(handle);
    handle->setMouseTracking(true);
}

void FramelessWindow::removeDragHandle(QWidget *handle)
{
    if (!handle)
        return;
    m_dragHandles.remove(handle);
    m_moveDragHandles.remove(handle);
    handle->removeEventFilter(this);
}

void FramelessWindow::installFilterRecursive(QWidget *widget)
{
    widget->installEventFilter(this);
    const auto children = widget->findChildren<QWidget *>(QString(), Qt::FindChildrenRecursively);
    for (auto *child : children)
        child->installEventFilter(this);
}

bool FramelessWindow::isInteractiveWidget(const QWidget *widget)
{
    return qobject_cast<const QToolButton *>(widget) || qobject_cast<const QSizeGrip *>(widget);
}

void FramelessWindow::setResizeMargin(int px)
{
    m_resizeMargin = qMax(0, px);
}

void FramelessWindow::setMinimumSize(QSize size)
{
    m_minimumSize = size;
}

void FramelessWindow::setResizeEnabled(bool enabled)
{
    m_resizeEnabled = enabled;
}

void FramelessWindow::setDragPredicate(DragPredicate pred)
{
    m_dragPredicate = std::move(pred);
}

void FramelessWindow::setTransparentForInput(bool on)
{
    m_transparentForInput = on;
}

bool FramelessWindow::eventFilter(QObject *obj, QEvent *event)
{
    if (!isOurWidget(obj))
        return QObject::eventFilter(obj, event);

    if (m_transparentForInput || m_window->testAttribute(Qt::WA_TransparentForMouseEvents)) {
        switch (event->type()) {
        case QEvent::MouseButtonPress:
        case QEvent::MouseButtonRelease:
        case QEvent::MouseMove:
        case QEvent::MouseButtonDblClick:
        case QEvent::ContextMenu:
        case QEvent::Wheel:
            return false;
        default:
            break;
        }
    }

    switch (event->type()) {
    case QEvent::MouseButtonPress: {
        m_rightClickPressed = false;
        if (m_windowDragActive) {
            m_windowDragActive = false;
            QApplication::restoreOverrideCursor();
        }
        auto *me = static_cast<QMouseEvent *>(event);

        if (me->button() == Qt::LeftButton) {
            if (isInteractiveWidget(qobject_cast<QWidget *>(obj)))
                break;
            if (isDragHandle(obj) || obj == m_window) {
                QPoint wpos  = toWindowPos(obj, me->pos());
                int    edges = edgeAtPos(wpos);
                if (edges != kEdgeNone && m_resizeEnabled) {
                    if (auto *win = m_window->windowHandle())
                        win->startSystemResize(static_cast<Qt::Edges>(edges));
                    return true;
                }
                bool wantDrag = false;
                if (isDragHandle(obj) && !isMoveDragHandle(obj)) {
                    wantDrag = true;
                }
                else if (obj == m_window || isMoveDragHandle(obj)) {
                    wantDrag = !m_dragPredicate || m_dragPredicate();
                }
                if (wantDrag) {
                    m_windowDragActive = true;
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
                    m_dragStartGlobalPos = me->globalPosition().toPoint();
#else
                    m_dragStartGlobalPos = me->globalPos();
#endif
                    m_dragStartWindowPos = m_window->pos();
                    QApplication::setOverrideCursor(Qt::ClosedHandCursor);
                    return true;
                }
            }
        }

        if (me->button() == Qt::RightButton && (me->modifiers() & Qt::AltModifier)) {
            if (auto *win = m_window->windowHandle())
                win->startSystemResize(Qt::BottomEdge | Qt::RightEdge);
            return true;
        }

        if (me->button() == Qt::RightButton) {
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
            m_rightClickPressPos = me->globalPosition().toPoint();
#else
            m_rightClickPressPos = me->globalPos();
#endif
            m_rightClickPressed = true;
        }
        break;
    }

    case QEvent::MouseMove: {
        auto *me = static_cast<QMouseEvent *>(event);

        if (m_windowDragActive) {
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
            QPoint delta = me->globalPosition().toPoint() - m_dragStartGlobalPos;
#else
            QPoint delta = me->globalPos() - m_dragStartGlobalPos;
#endif
            m_window->move(m_dragStartWindowPos + delta);
            return true;
        }

        if (m_rightClickPressed && (me->buttons() & Qt::RightButton)) {
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
            QPoint delta = me->globalPosition().toPoint() - m_rightClickPressPos;
#else
            QPoint delta = me->globalPos() - m_rightClickPressPos;
#endif
            if (delta.manhattanLength() > 5) {
                m_rightClickPressed = false;
                m_windowDragActive  = true;
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
                m_dragStartGlobalPos = me->globalPosition().toPoint();
#else
                m_dragStartGlobalPos = me->globalPos();
#endif
                m_dragStartWindowPos = m_window->pos();
                QApplication::setOverrideCursor(Qt::ClosedHandCursor);
                return true;
            }
        }

        if (!m_windowDragActive && m_resizeEnabled) {
            QPoint          wpos  = toWindowPos(obj, me->pos());
            int             edges = edgeAtPos(wpos);
            Qt::CursorShape shape = cursorForEdges(edges);
            m_window->setCursor(shape);
        }
        break;
    }

    case QEvent::MouseButtonRelease: {
        auto *me = static_cast<QMouseEvent *>(event);

        if (m_windowDragActive) {
            m_windowDragActive = false;
            QApplication::restoreOverrideCursor();
            return true;
        }

        if (me->button() == Qt::RightButton && m_rightClickPressed) {
            m_rightClickPressed = false;
            QWidget *target     = qobject_cast<QWidget *>(obj);
            if (!target)
                break;
            if (auto *scrollArea = qobject_cast<QAbstractScrollArea *>(target->parentWidget()))
                if (scrollArea->viewport() == target)
                    target = scrollArea;
            for (QWidget *p = target; p; p = p->parentWidget())
                if (qobject_cast<ToolbarWidget *>(p)) {
                    target = m_window;
                    break;
                }
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
            QPoint globalPos = me->globalPosition().toPoint();
#else
            QPoint globalPos = me->globalPos();
#endif
            QContextMenuEvent ce(QContextMenuEvent::Mouse, target->mapFromGlobal(globalPos), globalPos);
            m_manualContextMenu = true;
            QApplication::sendEvent(target, &ce);
            m_manualContextMenu = false;
            return true;
        }

        m_rightClickPressed = false;
        break;
    }

    case QEvent::ContextMenu:
        return !m_manualContextMenu;

    default:
        break;
    }
    return QObject::eventFilter(obj, event);
}

int FramelessWindow::edgeAtPos(const QPoint &pos) const
{
    int   edges      = kEdgeNone;
    QSize windowSize = m_window->size();

    if (pos.x() <= m_resizeMargin)
        edges |= kEdgeLeft;
    if (pos.x() >= windowSize.width() - m_resizeMargin)
        edges |= kEdgeRight;

    if (pos.y() <= m_resizeMargin)
        edges |= kEdgeTop;
    if (pos.y() >= windowSize.height() - m_resizeMargin)
        edges |= kEdgeBottom;

    return edges;
}

Qt::CursorShape FramelessWindow::cursorForEdges(int edges) const
{
    switch (edges) {
    case kEdgeLeft:
    case kEdgeRight:
        return Qt::SizeHorCursor;

    case kEdgeTop:
    case kEdgeBottom:
        return Qt::SizeVerCursor;

    case kEdgeTop | kEdgeLeft:
    case kEdgeBottom | kEdgeRight:
        return Qt::SizeFDiagCursor;

    case kEdgeTop | kEdgeRight:
    case kEdgeBottom | kEdgeLeft:
        return Qt::SizeBDiagCursor;

    default:
        return Qt::ArrowCursor;
    }
}

bool FramelessWindow::isDragHandle(QObject *obj) const
{
    auto *widget = qobject_cast<QWidget *>(obj);
    if (!widget)
        return false;
    for (auto *handle : m_dragHandles) {
        if (widget == handle || handle->isAncestorOf(widget))
            return true;
    }
    return false;
}

bool FramelessWindow::isMoveDragHandle(QObject *obj) const
{
    auto *widget = qobject_cast<QWidget *>(obj);
    if (!widget)
        return false;
    for (auto *handle : m_moveDragHandles) {
        if (widget == handle || handle->isAncestorOf(widget))
            return true;
    }
    return false;
}

bool FramelessWindow::isOurWidget(QObject *obj) const
{
    if (obj == m_window)
        return true;
    auto *w = qobject_cast<QWidget *>(obj);
    return w && m_window->isAncestorOf(w);
}

QPoint FramelessWindow::toWindowPos(QObject *obj, const QPoint &localPos) const
{
    if (obj == m_window)
        return localPos;
    if (auto *widget = qobject_cast<QWidget *>(obj))
        return widget->mapTo(m_window, localPos);
    return localPos;
}
