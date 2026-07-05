#include "CustomColorDialog.h"

#include <QColorDialog>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QVBoxLayout>

CustomColorDialog::CustomColorDialog(const QColor &initial, QWidget *parent)
    : BaseDialog(QStringLiteral("Color"), parent, false)
{
    m_picker = new QColorDialog(initial, this);
    m_picker->setOption(QColorDialog::DontUseNativeDialog);
    m_picker->setWindowFlags(Qt::Widget);

    auto *btnBox = m_picker->findChild<QDialogButtonBox *>();
    if (btnBox)
        btnBox->hide();

    m_picker->setStyleSheet(QStringLiteral("QColorDialog { border: none; background: transparent; }"
                                           "QFrame { border: none; }"));

    contentLayout()->addWidget(m_picker);
    setupStandardButtons();
}

QColor CustomColorDialog::selectedColor() const
{
    return m_picker->currentColor();
}
