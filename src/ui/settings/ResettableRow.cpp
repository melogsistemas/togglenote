#include "ResettableRow.h"
#include "SettingsRowUtils.h"

#include <QPushButton>

ResettableRow::ResettableRow(QWidget *parent)
    : QWidget(parent)
{
    m_resetBtn = SettingsRowUtils::makeResetButton(this);
    connect(m_resetBtn, &QPushButton::clicked, this, &ResettableRow::resetClicked);
}

void ResettableRow::setResetVisible(bool visible)
{
    m_resetBtn->setVisible(visible);
}
void ResettableRow::setResetEnabled(bool enabled)
{
    m_resetBtn->setEnabled(enabled);
}
bool ResettableRow::isResetEnabled() const
{
    return m_resetBtn->isEnabled();
}
