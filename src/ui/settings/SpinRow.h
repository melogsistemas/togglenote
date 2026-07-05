#pragma once

#include "ResettableRow.h"

class QSpinBox;

class SpinRow : public ResettableRow
{
    Q_OBJECT
public:
    explicit SpinRow(QWidget *parent = nullptr);
    void setRange(int min, int max);
    void setSuffix(const QString &suffix);
    void setValue(int value);
    int  value() const;
signals:
    void valueChanged(int);

private:
    QSpinBox *m_spin = nullptr;
};
