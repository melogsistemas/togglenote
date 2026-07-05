#pragma once

#include <QObject>
#include <QSet>
#include <QPoint>
#include <QRect>
#include <QSize>
#include <functional>

class QWidget;

class FramelessWindow : public QObject
{
    Q_OBJECT
public:
    explicit FramelessWindow(QWidget *targetWindow);
    ~FramelessWindow() override;

    using DragPredicate = std::function<bool()>;

    void addDragHandle(QWidget *handle, bool dragOnMove = false);
    void removeDragHandle(QWidget *handle);
    void setResizeMargin(int px);
    int  resizeMargin() const
    {
        return m_resizeMargin;
    }
    void setMinimumSize(QSize size);
    void setResizeEnabled(bool enabled);
    bool isResizeEnabled() const
    {
        return m_resizeEnabled;
    }
    void setDragPredicate(DragPredicate pred);
    void setTransparentForInput(bool on);

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;

private:
    enum Edge
    {
        kEdgeNone   = 0,
        kEdgeLeft   = 1,
        kEdgeRight  = 2,
        kEdgeTop    = 4,
        kEdgeBottom = 8,
    };

    void            installFilterRecursive(QWidget *widget);
    static bool     isInteractiveWidget(const QWidget *widget);
    int             edgeAtPos(const QPoint &pos) const;
    Qt::CursorShape cursorForEdges(int edges) const;
    bool            isDragHandle(QObject *obj) const;
    bool            isMoveDragHandle(QObject *obj) const;
    QPoint          toWindowPos(QObject *obj, const QPoint &localPos) const;
    bool            isOurWidget(QObject *obj) const;

    QWidget        *m_window;
    QSet<QWidget *> m_dragHandles;
    QSet<QWidget *> m_moveDragHandles;
    int             m_resizeMargin{6};
    QSize           m_minimumSize{100, 80};
    bool            m_resizeEnabled{true};
    bool            m_rightClickPressed{false};
    QPoint          m_rightClickPressPos;
    bool            m_windowDragActive{false};
    QPoint          m_dragStartGlobalPos;
    QPoint          m_dragStartWindowPos;
    bool            m_manualContextMenu{false};
    DragPredicate   m_dragPredicate;
    bool            m_transparentForInput{false};
};
