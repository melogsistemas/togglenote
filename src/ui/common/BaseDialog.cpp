#include "BaseDialog.h"
#include "TitleBar.h"
#include "FramelessWindow.h"

#include <QHBoxLayout>
#include <QPushButton>
#include <QVBoxLayout>

BaseDialog::BaseDialog(const QString &title, QWidget *parent, bool resizeEnabled, bool staysOnTop, bool deleteOnClose)
    : QDialog(parent)
{
    setWindowTitle(title);

    Qt::WindowFlags flags = windowFlags() | Qt::FramelessWindowHint;
    if (staysOnTop)
        flags |= Qt::WindowStaysOnTopHint;
    setWindowFlags(flags);

    if (deleteOnClose)
        setAttribute(Qt::WA_DeleteOnClose);

    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    m_titleBar = new TitleBar(this);
    m_titleBar->setTitle(title);
    mainLayout->addWidget(m_titleBar);

    m_windowController = new FramelessWindow(this);
    m_windowController->addDragHandle(m_titleBar);
    m_windowController->setResizeEnabled(resizeEnabled);

    connect(m_titleBar, &TitleBar::closeRequested, this, &QDialog::reject);
    connect(m_titleBar, &TitleBar::minimizeRequested, this, &QDialog::showMinimized);
    connect(m_titleBar, &TitleBar::maximizeRequested, this, [this]() {
        if (isMaximized())
            showNormal();
        else
            showMaximized();
    });

    m_contentLayout = new QVBoxLayout;
    m_contentLayout->setContentsMargins(6, 6, 6, 6);
    m_contentLayout->setSpacing(0);
    mainLayout->addLayout(m_contentLayout);
}

void BaseDialog::setupStandardButtons()
{
    auto *layout = new QHBoxLayout;
    layout->setContentsMargins(0, 0, 0, 6);
    layout->setSpacing(6);
    m_okBtn     = new QPushButton(QStringLiteral("OK"));
    m_cancelBtn = new QPushButton(QStringLiteral("Cancel"));
    layout->addStretch();
    layout->addWidget(m_okBtn);
    layout->addWidget(m_cancelBtn);
    contentLayout()->addLayout(layout);

    connect(m_okBtn, &QPushButton::clicked, this, &QDialog::accept);
    connect(m_cancelBtn, &QPushButton::clicked, this, &QDialog::reject);
}

void BaseDialog::setResizeEnabled(bool enabled)
{
    m_windowController->setResizeEnabled(enabled);
}
