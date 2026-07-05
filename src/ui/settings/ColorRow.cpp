#include "ColorRow.h"
#include "ColorGridDialog.h"
#include "SettingsRowUtils.h"

#include <QHBoxLayout>
#include <QPushButton>

ColorRow::ColorRow(QWidget *parent)
    : ResettableRow(parent)
{
    auto *lay = new QHBoxLayout(this);
    lay->setContentsMargins(0, 0, 0, 0);

    m_colorBtn = new QPushButton;
    m_colorBtn->setFixedSize(60, 30);
    lay->addWidget(m_colorBtn);

    auto *choose = new QPushButton(QStringLiteral("Choose..."));
    lay->addWidget(choose);

    lay->addWidget(resetButton());
    lay->addStretch();

    connect(choose, &QPushButton::clicked, this, &ColorRow::chooseClicked);
}

void ColorRow::setColor(const QColor &color)
{
    m_color = color;
    m_colorBtn->setStyleSheet(QStringLiteral("background-color: %1; border: 1px solid gray;").arg(color.name()));
}

QColor ColorRow::color() const
{
    return m_color;
}

ColorGridResult ColorRow::openGrid(const QColor                       &currentColor,
                                   const QStringList                  &customColors,
                                   QWidget                            *parent,
                                   const QString                      &title,
                                   std::function<void(const QColor &)> onPreview)
{
    QVector<QColor> init;
    for (const QString &colorString : customColors)
        init.append(QColor(colorString));
    if (init.size() != ColorGridDialog::kCustomColorCount)
        init = ColorGridDialog::defaultCustomColors();

    QColor      initialColor        = currentColor;
    QStringList updatedCustomColors = customColors;

    ColorGridDialog dialog(initialColor, init, title.isEmpty() ? QStringLiteral("Select color") : title, parent);

    if (onPreview) {
        QObject::connect(&dialog, &ColorGridDialog::colorPicked, &dialog, [&dialog, onPreview](const QColor &color) {
            onPreview(color);
        });
    }

    QObject::connect(
        &dialog, &ColorGridDialog::customColorsChanged, &dialog, [&updatedCustomColors](const QVector<QColor> &colors) {
            QStringList colorNames;
            colorNames.reserve(colors.size());
            for (const QColor &color : colors)
                colorNames.append(color.name());
            updatedCustomColors = colorNames;
        });

    if (dialog.exec() == QDialog::Accepted) {
        return {dialog.selectedColor(), updatedCustomColors, true};
    }
    else {
        return {initialColor, updatedCustomColors, false};
    }
}
