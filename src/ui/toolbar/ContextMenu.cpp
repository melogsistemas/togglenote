#include "ContextMenu.h"
#include "Design.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QEvent>
#include <QStyle>
#include <QPainter>
#include <QScreen>
#include <QGuiApplication>


class CheckWidget : public QWidget
{
public:
    explicit CheckWidget(QWidget *parent)
        : QWidget(parent)
    {
        setFixedSize(14, 14);
        setAttribute(Qt::WA_TransparentForMouseEvents);
    }

    void setChecked(bool checked)
    {
        if (checked != m_checked) {
            m_checked = checked;
            update();
        }
    }

protected:
    void paintEvent(QPaintEvent *) override
    {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);
        if (m_checked) {
            painter.setBrush(QColor(Design::Accent));
            painter.setPen(QColor(Design::Accent));
            painter.drawRoundedRect(QRectF(0.5, 0.5, 13, 13), 2, 2);
            painter.setPen(Qt::white);
            QFont font = painter.font();
            font.setPixelSize(11);
            painter.setFont(font);
            painter.drawText(rect(), Qt::AlignCenter, QStringLiteral("\u2713"));
        }
        else {
            painter.setBrush(QColor(Design::Background));
            painter.setPen(QColor(Design::Border));
            painter.drawRoundedRect(QRectF(0.5, 0.5, 13, 13), 2, 2);
        }
    }

private:
    bool m_checked = false;
};

ContextMenu::ContextMenu(QWidget *parent)
    : QFrame(parent, Qt::Popup | Qt::FramelessWindowHint)
{
    setAttribute(Qt::WA_DeleteOnClose);
    setMouseTracking(true);

    m_layout = new QVBoxLayout(this);
    m_layout->setContentsMargins(0, 4, 0, 4);
    m_layout->setSpacing(0);

    setStyleSheet(QStringLiteral("ContextMenu { background: %1; border: 1px solid %2; }"
                                 "ContextMenu > QWidget[hover=\"true\"] { background: %3; }")
                      .arg(Design::Panel, Design::Border, Design::Accent));
}

int ContextMenu::addEntry(const MenuEntry &entry)
{
    int index = m_entries.size();
    m_entries.append({entry, false});

    auto *row = createRow(index);
    m_layout->addWidget(row);

    return index;
}

void ContextMenu::addSeparator()
{
    m_entries.append({{}, true});
    auto *sep = createSeparator();
    m_layout->addWidget(sep);
}

QWidget *ContextMenu::createRow(int index)
{
    auto       &entryData = m_entries[index];
    const auto &entry     = entryData.entry;

    auto *row = new QWidget;
    row->setFixedHeight(26);
    row->setMouseTracking(true);
    row->installEventFilter(this);

    auto *horizontalLayout = new QHBoxLayout(row);
    horizontalLayout->setContentsMargins(8, 0, 8, 0);
    horizontalLayout->setSpacing(6);

    if (entry.checkable) {
        auto *checkWidget = new CheckWidget(this);
        checkWidget->setChecked(entry.checked);
        entryData.checkbox = checkWidget;
        horizontalLayout->addWidget(checkWidget);
    }

    auto *iconLabel = new QLabel;
    iconLabel->setPixmap(entry.icon.pixmap(16, 16));
    iconLabel->setFixedSize(16, 16);
    iconLabel->setAttribute(Qt::WA_TransparentForMouseEvents);
    horizontalLayout->addWidget(iconLabel);

    auto *textLabel = new QLabel(entry.text);
    textLabel->setStyleSheet(QStringLiteral("color: %1; background: transparent;").arg(Design::Foreground));
    textLabel->setAttribute(Qt::WA_TransparentForMouseEvents);
    horizontalLayout->addWidget(textLabel);
    horizontalLayout->addStretch();

    return row;
}

QWidget *ContextMenu::createSeparator()
{
    auto *sep = new QWidget;
    sep->setFixedHeight(1);
    sep->setStyleSheet(QStringLiteral("background: %1;").arg(Design::Border));
    sep->setProperty("separator", true);
    return sep;
}

int ContextMenu::rowIndex(QWidget *row) const
{
    for (int i = 0; i < m_layout->count(); ++i) {
        if (m_layout->itemAt(i)->widget() == row)
            return i;
    }
    return -1;
}

void ContextMenu::activate(int index)
{
    if (index < 0 || index >= m_entries.size())
        return;
    auto &entryData = m_entries[index];
    if (entryData.isSeparator)
        return;

    auto &entry = entryData.entry;
    if (entry.checkable) {
        entry.checked = !entry.checked;
        if (auto *cb = static_cast<CheckWidget *>(entryData.checkbox))
            cb->setChecked(entry.checked);
        if (entry.onClick)
            entry.onClick(entry.checked);
    }
    else {
        if (entry.onClick)
            entry.onClick(false);
        close();
    }
}

void ContextMenu::popup(const QPoint &pos)
{
    adjustSize();
    QPoint adjustedPos = pos;
    if (auto *screen = QGuiApplication::screenAt(adjustedPos)) {
        QRect geo = screen->availableGeometry();
        if (adjustedPos.x() + width() > geo.right())
            adjustedPos.setX(adjustedPos.x() - width());
        if (adjustedPos.y() + height() > geo.bottom())
            adjustedPos.setY(adjustedPos.y() - height());
        if (adjustedPos.x() < geo.left())
            adjustedPos.setX(geo.left());
        if (adjustedPos.y() < geo.top())
            adjustedPos.setY(geo.top());
    }
    move(adjustedPos);
    show();
}

bool ContextMenu::eventFilter(QObject *obj, QEvent *event)
{
    switch (event->type()) {
    case QEvent::MouseMove: {
        auto *widget = qobject_cast<QWidget *>(obj);
        while (widget && widget->parentWidget() != this)
            widget = widget->parentWidget();
        setHover(widget);
        return false;
    }
    case QEvent::MouseButtonRelease: {
        auto *widget = qobject_cast<QWidget *>(obj);
        while (widget && widget->parentWidget() != this)
            widget = widget->parentWidget();
        int idx = rowIndex(widget);
        if (idx >= 0)
            activate(idx);
        return true;
    }
    default:
        return QFrame::eventFilter(obj, event);
    }
}

void ContextMenu::setHover(QWidget *row)
{
    for (int i = 0; i < m_layout->count(); ++i) {
        auto *widget = m_layout->itemAt(i)->widget();
        if (!widget || widget->property("separator").toBool())
            continue;
        bool was = widget->property("hover").toBool();
        bool now = (widget == row);
        if (was != now) {
            Design::refreshStyleProperty(widget, "hover", now);
        }
    }
}

void ContextMenu::clearHover()
{
    for (int i = 0; i < m_layout->count(); ++i) {
        auto *widget = m_layout->itemAt(i)->widget();
        if (!widget || widget->property("separator").toBool())
            continue;
        if (widget->property("hover").toBool()) {
            Design::refreshStyleProperty(widget, "hover", false);
        }
    }
}

void ContextMenu::mouseMoveEvent(QMouseEvent *event)
{
    QWidget *child = childAt(event->pos());
    if (!child || child == this) {
        clearHover();
        return;
    }
    while (child && child->parentWidget() != this)
        child = child->parentWidget();
    setHover(child);
}

void ContextMenu::mouseReleaseEvent(QMouseEvent *event)
{
    QWidget *child = childAt(event->pos());
    if (!child || child == this)
        return;
    while (child && child->parentWidget() != this)
        child = child->parentWidget();
    int idx = rowIndex(child);
    if (idx >= 0)
        activate(idx);
}

void ContextMenu::leaveEvent(QEvent *)
{
    clearHover();
}

void ContextMenu::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape)
        close();
    QFrame::keyPressEvent(event);
}
