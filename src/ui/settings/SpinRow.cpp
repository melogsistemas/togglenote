#include "SpinRow.h"
#include "SettingsRowUtils.h"

#include <QHBoxLayout>
#include <QSpinBox>

using namespace SettingsRowUtils;

SpinRow::SpinRow(QWidget *parent)
    : ResettableRow(parent)
{
    auto *lay = new QHBoxLayout(this);
    lay->setContentsMargins(0, 0, 0, 0);

    m_spin = new QSpinBox;
    configureSpinbox(m_spin);

    lay->addWidget(m_spin);

    addStepButtons(m_spin, this, lay);

    lay->addWidget(resetButton());

    connect(m_spin, QOverload<int>::of(&QSpinBox::valueChanged), this, &SpinRow::valueChanged);
}

void SpinRow::setRange(int min, int max)
{
    m_spin->setRange(min, max);
}
void SpinRow::setSuffix(const QString &suffix)
{
    m_spin->setSuffix(suffix);
}
void SpinRow::setValue(int value)
{
    m_spin->setValue(value);
}
int SpinRow::value() const
{
    return m_spin->value();
}
