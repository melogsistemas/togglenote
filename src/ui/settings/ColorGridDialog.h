#pragma once

#include "BaseDialog.h"
#include <QVector>
#include <array>

class QToolButton;
class QVBoxLayout;

class ColorGridDialog : public BaseDialog
{
    Q_OBJECT
public:
    explicit ColorGridDialog(const QColor          &current,
                             const QVector<QColor> &customColors = defaultCustomColors(),
                             const QString         &title        = QStringLiteral("Select color"),
                             QWidget               *parent       = nullptr);
    QColor selectedColor() const
    {
        return m_selected;
    }
    const QVector<QColor> &customColors() const
    {
        return m_customColors;
    }
    static QVector<QColor> defaultCustomColors();

    static constexpr int kPaletteCols      = 10;
    static constexpr int kPaletteRows      = 5;
    static constexpr int kCustomColorCount = 9;

signals:
    void colorPicked(const QColor &color);
    void customColorsChanged(const QVector<QColor> &colors);

private:
    void pickColor(const QColor &color);
    void pickCustom();
    void shiftCustomColor(const QColor &color);
    void refreshSelection();
    void buildPaletteGrid(QVBoxLayout *root);
    void buildCustomColorsRow(QVBoxLayout *root);
    void onCustomButtonClicked();

    QColor                 m_selected;
    QVector<QColor>        m_customColors;
    QVector<QToolButton *> m_paletteBtns;
    QVector<QToolButton *> m_customBtns;
};
