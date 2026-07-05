#pragma once

#include "BaseDialog.h"

class QColorDialog;

class CustomColorDialog : public BaseDialog
{
    Q_OBJECT
public:
    explicit CustomColorDialog(const QColor &initial, QWidget *parent = nullptr);
    QColor selectedColor() const;

private:
    QColorDialog *m_picker;
};
