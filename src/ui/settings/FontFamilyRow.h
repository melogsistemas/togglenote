#pragma once

#include "ResettableRow.h"

class QFontComboBox;

class FontFamilyRow : public ResettableRow
{
    Q_OBJECT
public:
    explicit FontFamilyRow(QWidget *parent = nullptr);
    void  setCurrentFont(const QFont &font);
    QFont currentFont() const;
signals:
    void fontChanged(const QFont &);

private:
    QFontComboBox *m_combo = nullptr;
};
