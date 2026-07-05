#pragma once

#include "ResettableRow.h"

class QSlider;
class QSpinBox;

class SliderSpinRow : public ResettableRow
{
    Q_OBJECT
public:
    explicit SliderSpinRow(QWidget *parent = nullptr);
    void setRange(int min, int max);
    void setSuffix(const QString &suffix);
    void setValue(int value);
    int  value() const;
signals:
    void valueChanged(int);

private:
    QSlider  *m_slider  = nullptr;
    QSpinBox *m_spin    = nullptr;
    bool      m_syncing = false;
};
