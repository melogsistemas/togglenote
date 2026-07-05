#include "NoteSettingsDialog.h"
#include "Settings.h"

#include <QVBoxLayout>
#include <QFormLayout>

#include <QSpinBox>
#include <QSlider>
#include <QLabel>
#include <QFontComboBox>
#include <QPalette>

NoteSettingsDialog::NoteSettingsDialog(QWidget *parent)
    : BaseDialog(QStringLiteral("Note preferences"), parent, false, true, true)
{
    setupUI();
}

void NoteSettingsDialog::setupUI()
{
    contentLayout()->setContentsMargins(12, 12, 12, 12);
    contentLayout()->setSpacing(8);

    auto *form = new QFormLayout;
    form->setSpacing(6);
    form->setLabelAlignment(Qt::AlignRight);

    m_bgColorRow = new ColorRow(this);
    form->addRow(QStringLiteral("Bg color:"), m_bgColorRow);

    m_fontColorRow = new ColorRow(this);
    form->addRow(QStringLiteral("Font color:"), m_fontColorRow);

    m_fontFamilyRow = new FontFamilyRow(this);
    form->addRow(QStringLiteral("Font family:"), m_fontFamilyRow);

    m_fontSizeRow = new SpinRow(this);
    m_fontSizeRow->setRange(FontSizeLimits::kMin, FontSizeLimits::kMax);
    m_fontSizeRow->setSuffix(QStringLiteral(" pt"));
    form->addRow(QStringLiteral("Font size:"), m_fontSizeRow);

    m_iconSizeRow = new SpinRow(this);
    m_iconSizeRow->setRange(IconLimits::kMin, IconLimits::kMax);
    m_iconSizeRow->setSuffix(QStringLiteral(" px"));
    form->addRow(QStringLiteral("Icon size:"), m_iconSizeRow);

    m_opacityRow = new SliderSpinRow(this);
    m_opacityRow->setRange(10, 100);
    m_opacityRow->setSuffix(QStringLiteral("%"));
    form->addRow(QStringLiteral("Opacity:"), m_opacityRow);

    contentLayout()->addLayout(form);
    contentLayout()->addStretch();

    setupStandardButtons();

    setFixedSize(400, sizeHint().height() + 20);
}
