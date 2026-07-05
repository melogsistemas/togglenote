#include "FontFamilyRow.h"
#include "SettingsRowUtils.h"

#include <QHBoxLayout>
#include <QPushButton>
#include <QFontComboBox>

FontFamilyRow::FontFamilyRow(QWidget *parent)
    : ResettableRow(parent)
{
    auto *lay = new QHBoxLayout(this);
    lay->setContentsMargins(0, 0, 0, 0);

    m_combo = new QFontComboBox;
    lay->addWidget(m_combo, 1);

    lay->addWidget(resetButton());

    connect(m_combo, &QFontComboBox::currentFontChanged, this, &FontFamilyRow::fontChanged);
}

void FontFamilyRow::setCurrentFont(const QFont &font)
{
    m_combo->setCurrentFont(font);
}
QFont FontFamilyRow::currentFont() const
{
    return m_combo->currentFont();
}
