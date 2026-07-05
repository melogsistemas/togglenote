#include "TitleBar.h"
#include "Icons.h"
#include "Design.h"

#include <QHBoxLayout>
#include <QToolButton>
#include <QLabel>
#include <QEvent>
#include <QMouseEvent>

TitleBar::TitleBar(QWidget *parent)
    : QWidget(parent)
{
    setFixedHeight(32);

    auto *layout = new QHBoxLayout(this);
    layout->setContentsMargins(8, 0, 4, 0);
    layout->setSpacing(2);

    m_titleLabel = new QLabel;
    m_titleLabel->setStyleSheet(QStringLiteral("color: %1; font-size: 12px;").arg(Design::Foreground));
    layout->addWidget(m_titleLabel, 1);

    layout->addStretch();

    m_minimizeBtn = new QToolButton;
    m_minimizeBtn->setIcon(Icons::icon(Design::Icon::Minimize, 14));
    m_minimizeBtn->setToolTip(QStringLiteral("Minimize"));
    m_minimizeBtn->setFixedSize(32, 24);
    layout->addWidget(m_minimizeBtn);

    m_maximizeBtn = new QToolButton;
    m_maximizeBtn->setIcon(Icons::icon(Design::Icon::Maximize, 14));
    m_maximizeBtn->setToolTip(QStringLiteral("Maximize"));
    m_maximizeBtn->setFixedSize(32, 24);
    layout->addWidget(m_maximizeBtn);

    m_closeBtn = new QToolButton;
    m_closeBtn->setIcon(Icons::icon(Design::Icon::Close, 14));
    m_closeBtn->setToolTip(QStringLiteral("Close"));
    m_closeBtn->setFixedSize(32, 24);
    layout->addWidget(m_closeBtn);

    connect(m_closeBtn, &QToolButton::clicked, this, &TitleBar::closeRequested);
    connect(m_minimizeBtn, &QToolButton::clicked, this, &TitleBar::minimizeRequested);
    connect(m_maximizeBtn, &QToolButton::clicked, this, &TitleBar::maximizeRequested);

    setStyleSheet(QStringLiteral("TitleBar { background: #1c1e20; border-bottom: 1px solid %1; }"
                                 "QToolButton { border: none; border-radius: 2px; padding: 2px; }"
                                 "QToolButton:hover { background: %2; }")
                      .arg(Design::Border, Design::Panel));
}

void TitleBar::setTitle(const QString &title)
{
    m_titleLabel->setText(title);
}

void TitleBar::updateMaximizeIcon()
{
    auto *w         = window();
    bool  maximized = w->isMaximized() || w->isFullScreen();
    m_maximizeBtn->setIcon(Icons::icon(maximized ? Design::Icon::Restore : Design::Icon::Maximize, 14));
    m_maximizeBtn->setToolTip(maximized ? QStringLiteral("Restore") : QStringLiteral("Maximize"));
}

void TitleBar::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::WindowStateChange)
        updateMaximizeIcon();
    QWidget::changeEvent(event);
}

void TitleBar::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
        emit maximizeRequested();
    QWidget::mouseDoubleClickEvent(event);
}
