#include "SliderSpinRow.h"
#include "SettingsRowUtils.h"

#include <QHBoxLayout>
#include <QSpinBox>
#include <QSlider>

using namespace SettingsRowUtils;

SliderSpinRow::SliderSpinRow(QWidget *parent)
    : ResettableRow(parent)
{
    auto *lay = new QHBoxLayout(this);
    lay->setContentsMargins(0, 0, 0, 0);

    m_slider = new QSlider(Qt::Horizontal);
    lay->addWidget(m_slider, 1);

    m_spin = new QSpinBox;
    m_spin->setFixedWidth(64);
    configureSpinbox(m_spin);

    lay->addWidget(m_spin);

    addStepButtons(m_spin, this, lay);

    lay->addWidget(resetButton());

    auto sync = [this](int value) {
        if (m_syncing)
            return;
        m_syncing = true;
        m_slider->setValue(value);
        m_spin->setValue(value);
        m_syncing = false;
        emit valueChanged(value);
    };
    connect(m_slider, &QSlider::valueChanged, this, sync);
    connect(m_spin, QOverload<int>::of(&QSpinBox::valueChanged), this, sync);
}

void SliderSpinRow::setRange(int min, int max)
{
    m_slider->setRange(min, max);
    m_spin->setRange(min, max);
}

void SliderSpinRow::setSuffix(const QString &suffix)
{
    m_spin->setSuffix(suffix);
}

void SliderSpinRow::setValue(int value)
{
    m_slider->setValue(value);
    m_spin->setValue(value);
}

int SliderSpinRow::value() const
{
    return m_spin->value();
}
