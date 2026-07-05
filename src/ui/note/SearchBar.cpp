#include "SearchBar.h"
#include "Design.h"
#include <QHBoxLayout>
#include <QLineEdit>
#include <QToolButton>


SearchBar::SearchBar(QWidget *parent)
    : QWidget(parent)
{
    setStyleSheet(QStringLiteral("QWidget { background: %1; }"
                                 "QToolButton { color: %2; font-size: 12px; }")
                      .arg(Design::Background, Design::Foreground));

    auto *layout = new QHBoxLayout(this);
    layout->setContentsMargins(2, 2, 2, 2);
    layout->setSpacing(2);

    m_input = new QLineEdit;
    m_input->setPlaceholderText(QStringLiteral("Search..."));
    m_input->setClearButtonEnabled(true);
    layout->addWidget(m_input);

    m_downBtn = new QToolButton;
    m_downBtn->setText(QStringLiteral("\u25BC"));
    m_downBtn->setToolTip(QStringLiteral("Find next"));
    layout->addWidget(m_downBtn);

    m_upBtn = new QToolButton;
    m_upBtn->setText(QStringLiteral("\u25B2"));
    m_upBtn->setToolTip(QStringLiteral("Find previous"));
    layout->addWidget(m_upBtn);

    m_closeBtn = new QToolButton;
    m_closeBtn->setText(QStringLiteral("\u2715"));
    m_closeBtn->setToolTip(QStringLiteral("Close search"));
    layout->addWidget(m_closeBtn);

    connect(m_input, &QLineEdit::returnPressed, this, [this]() { emit searchRequested(m_input->text(), true); });
    connect(m_downBtn, &QToolButton::clicked, this, [this]() { emit searchRequested(m_input->text(), true); });
    connect(m_upBtn, &QToolButton::clicked, this, [this]() { emit searchRequested(m_input->text(), false); });
    connect(m_closeBtn, &QToolButton::clicked, this, &SearchBar::closed);
}

void SearchBar::focusInput()
{
    m_input->setFocus();
    m_input->selectAll();
}
