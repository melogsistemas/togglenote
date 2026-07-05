#include "ColorGridDialog.h"
#include "CustomColorDialog.h"
#include "Settings.h"
#include "Icons.h"
#include "Design.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QToolButton>
#include <QFrame>
#include <QPushButton>
#include <QVariant>

// GTK3-inspired palette (10 columns × 5 rows)
static constexpr std::array kPalette{
    std::array{"#99c1f1", "#62a0ea", "#3584e4", "#1c71d8", "#1a5fb4"},
    std::array{"#8ff0a4", "#57e389", "#33d17a", "#2ec27e", "#26a269"},
    std::array{"#f9f06b", "#f8e45c", "#f6d32d", "#f5c211", "#e5a50a"},
    std::array{"#ffbe6f", "#ffa348", "#ff7800", "#e66100", "#c64600"},
    std::array{"#f66151", "#ed333b", "#e01b24", "#c01c28", "#a51d2d"},
    std::array{"#dc8add", "#c061cb", "#9141ac", "#813d9c", "#613583"},
    std::array{"#fcccd8", "#f5a0b0", "#e87890", "#d05070", "#b83858"},
    std::array{"#cdab8f", "#b5835a", "#986a44", "#865e3c", "#63452c"},
    std::array{"#ffffff", "#f6f5f4", "#deddda", "#c0bfbc", "#9a9996"},
    std::array{"#77767b", "#5e5c64", "#3d3846", "#241f31", "#000000"},
};

QVector<QColor> ColorGridDialog::defaultCustomColors()
{
    return {
        QColor("#ffee99"),
        QColor("#ccffcc"),
        QColor("#ccccff"),
        QColor("#ffcccc"),
        QColor("#ffdd99"),
        QColor("#ddccff"),
        QColor("#ccffee"),
        QColor("#ffccbb"),
        QColor("#99ccff"),
    };
}

static QColor colorAt(int col, int row)
{
    return QColor(QLatin1String(kPalette[col][row]));
}

static QString swatchStyle(const QColor &c, bool selected)
{
    QString border = selected ? QStringLiteral("2px solid %1").arg(Design::Accent)
                              : QStringLiteral("1px solid %1").arg(Design::Border);

    return QStringLiteral("QToolButton { background-color: %1; border: %2; border-radius: 2px; }"
                          "QToolButton:hover { border: 2px solid white; }")
        .arg(c.name(), border);
}

static QToolButton *makeSwatch(const QColor &c, bool selected, QWidget *parent)
{
    auto *btn = new QToolButton(parent);
    btn->setFixedSize(44, 32);
    btn->setStyleSheet(swatchStyle(c, selected));
    return btn;
}

ColorGridDialog::ColorGridDialog(const QColor          &current,
                                 const QVector<QColor> &customColors,
                                 const QString         &title,
                                 QWidget               *parent)
    : BaseDialog(title, parent, false)
    , m_selected(current)
    , m_customColors(customColors)
{
    contentLayout()->setContentsMargins(8, 0, 8, 0);
    contentLayout()->setSpacing(6);

    buildPaletteGrid(contentLayout());
    buildCustomColorsRow(contentLayout());
    setupStandardButtons();

    setFixedSize(500, sizeHint().height());
}

void ColorGridDialog::buildPaletteGrid(QVBoxLayout *root)
{
    auto *grid = new QGridLayout;
    grid->setSpacing(2);
    for (int col = 0; col < kPaletteCols; ++col) {
        int gridColumn = (col >= 8) ? col + 1 : col;
        for (int row = 0; row < kPaletteRows; ++row) {
            QColor color = colorAt(col, row);
            auto  *btn   = makeSwatch(color, color == m_selected, this);
            connect(btn, &QToolButton::clicked, this, [this, color]() { pickColor(color); });
            grid->addWidget(btn, row, gridColumn);
            m_paletteBtns.append(btn);
        }
    }
    auto *spacer = new QWidget;
    spacer->setFixedWidth(10);
    grid->addWidget(spacer, 0, 8, kPaletteRows, 1);
    root->addLayout(grid);

    auto *sep = new QFrame;
    sep->setFixedHeight(1);
    sep->setStyleSheet(QStringLiteral("background: %1; border: none;").arg(Design::Border));
    root->addWidget(sep);
}

void ColorGridDialog::buildCustomColorsRow(QVBoxLayout *root)
{
    auto *row = new QHBoxLayout;
    row->setSpacing(2);
    for (int i = 0; i < kCustomColorCount; ++i) {
        QColor color = m_customColors[i];
        auto  *btn   = makeSwatch(color, color == m_selected, this);
        btn->setProperty("index", i);
        connect(btn, &QToolButton::clicked, this, &ColorGridDialog::onCustomButtonClicked);
        row->addWidget(btn);
        m_customBtns.append(btn);
    }

    auto *addBtn = new QToolButton;
    addBtn->setIcon(plusIcon());
    addBtn->setIconSize(QSize(IconLimits::kStepButton, IconLimits::kStepButton));
    addBtn->setFixedSize(44, 32);
    addBtn->setStyleSheet(QStringLiteral(
        "QToolButton { background: %1; color: %2; border: 1px dashed %3; border-radius: 2px; }"
        "QToolButton:hover { background: #555a5f; border: 1px solid %4; }")
                              .arg(Design::Border, Design::Foreground, Design::Separator, Design::Accent));
    connect(addBtn, &QToolButton::clicked, this, &ColorGridDialog::pickCustom);
    row->addWidget(addBtn);
    row->addStretch();

    root->addLayout(row);
}


void ColorGridDialog::onCustomButtonClicked()
{
    auto *btn = qobject_cast<QToolButton *>(sender());
    if (!btn)
        return;
    int index = btn->property("index").toInt();
    pickColor(m_customColors[index]);
}

void ColorGridDialog::refreshSelection()
{
    int idx = 0;
    for (int col = 0; col < kPaletteCols; ++col)
        for (int row = 0; row < kPaletteRows; ++row)
            m_paletteBtns[idx++]->setStyleSheet(swatchStyle(colorAt(col, row), colorAt(col, row) == m_selected));
    for (int i = 0; i < kCustomColorCount; ++i)
        m_customBtns[i]->setStyleSheet(swatchStyle(m_customColors[i], m_customColors[i] == m_selected));
}

void ColorGridDialog::pickColor(const QColor &color)
{
    m_selected = color;
    refreshSelection();
    emit colorPicked(color);
}

void ColorGridDialog::shiftCustomColor(const QColor &color)
{
    for (int i = 1; i < kCustomColorCount; ++i)
        m_customColors[i - 1] = m_customColors[i];
    m_customColors[kCustomColorCount - 1] = color;
    refreshSelection();
    emit customColorsChanged(m_customColors);
}

void ColorGridDialog::pickCustom()
{
    CustomColorDialog dlg(m_selected, this);
    if (dlg.exec() == QDialog::Accepted) {
        m_selected = dlg.selectedColor();
        shiftCustomColor(m_selected);
        emit colorPicked(m_selected);
    }
}
