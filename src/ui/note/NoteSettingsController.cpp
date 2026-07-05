#include "NoteSettingsController.h"
#include "NoteSettingsDialog.h"
#include "NoteViewModel.h"
#include "SettingsRowUtils.h"
#include "ColorRow.h"
#include "SpinRow.h"
#include "SliderSpinRow.h"
#include "FontFamilyRow.h"

#include <QPushButton>
#include <QFont>

NoteSettingsController::NoteSettingsController(NoteViewModel  *viewModel,
                                               QWidget        *parent,
                                               const Settings &globalSettings,
                                               CustomColorsCb  onCustomColorsChanged)
    : QObject(viewModel)
    , m_viewModel(viewModel)
    , m_globalSettings(globalSettings)
    , m_onCustomColorsChanged(onCustomColorsChanged)
    , m_initialData(viewModel->snapshot())
    , m_initialCustomColors(globalSettings.customColors())
{
    m_dialog = new NoteSettingsDialog(parent);

    // Color rows
    setupColorRow(
        m_dialog->bgColorRow(),
        [this](const QColor &color) { m_viewModel->setBgColor(color); },
        m_viewModel->bgColor(),
        m_initialData.bgColor().has_value(),
        [this]() { resetBgColor(); },
        QStringLiteral("Note color"));

    setupColorRow(
        m_dialog->fontColorRow(),
        [this](const QColor &color) { m_viewModel->setTextColor(color); },
        m_viewModel->textColor(),
        m_initialData.textColor().has_value(),
        [this]() { resetFontColor(); },
        QStringLiteral("Font color"));

    // Font family
    setupRow(
        m_dialog->fontFamilyRow(),
        [this]() { m_dialog->fontFamilyRow()->setCurrentFont(QFont(m_viewModel->fontFamily())); },
        m_initialData.fontFamily().has_value(),
        [this]() {
            connect(m_dialog->fontFamilyRow(), &FontFamilyRow::fontChanged, this, [this](const QFont &font) {
                m_viewModel->setFontFamily(font.family());
                m_dialog->fontFamilyRow()->setResetEnabled(true);
            });
        },
        [this]() { resetFontFamily(); });

    // Font size
    setupRow(
        m_dialog->fontSizeRow(),
        [this]() { m_dialog->fontSizeRow()->setValue(m_viewModel->fontSize()); },
        m_initialData.fontSize().has_value(),
        [this]() {
            connect(m_dialog->fontSizeRow(), &SpinRow::valueChanged, this, [this](int value) {
                m_viewModel->setFontSize(value);
                m_dialog->fontSizeRow()->setResetEnabled(true);
            });
        },
        [this]() { resetFontSize(); });

    // Icon size
    setupRow(
        m_dialog->iconSizeRow(),
        [this]() {
            m_dialog->iconSizeRow()->setValue(m_initialData.iconSize().value_or(m_viewModel->defaultToolbarIconSize()));
        },
        m_initialData.iconSize().has_value(),
        [this]() {
            connect(m_dialog->iconSizeRow(), &SpinRow::valueChanged, this, [this](int value) {
                m_viewModel->setIconSize(value);
                m_dialog->iconSizeRow()->setResetEnabled(true);
            });
        },
        [this]() { resetIconSize(); });

    // Opacity
    setupRow(
        m_dialog->opacityRow(),
        [this]() { m_dialog->opacityRow()->setValue(m_viewModel->opacity()); },
        m_initialData.opacity().has_value(),
        [this]() {
            connect(m_dialog->opacityRow(), &SliderSpinRow::valueChanged, this, [this](int value) {
                m_viewModel->setOpacity(value);
                m_dialog->opacityRow()->setResetEnabled(true);
            });
        },
        [this]() { resetOpacity(); });

    // OK / Cancel
    connect(m_dialog->okButton(), &QPushButton::clicked, this, [this]() {
        if (m_onCustomColorsChanged)
            m_onCustomColorsChanged(m_globalSettings.customColors());
        m_dialog->accept();
    });
    connect(m_dialog->cancelButton(), &QPushButton::clicked, this, [this]() {
        revertChanges();
        m_dialog->reject();
    });
}

NoteSettingsController::~NoteSettingsController() = default;

void NoteSettingsController::showAt(const QPoint &pos)
{
    m_dialog->move(pos);
    m_dialog->show();
    m_dialog->raise();
}

void NoteSettingsController::setupColorRow(ColorRow                           *row,
                                           std::function<void(const QColor &)> onChosen,
                                           const QColor                       &initial,
                                           bool                                hasCustom,
                                           std::function<void()>               resetSlot,
                                           const QString                      &title)
{
    row->setColor(initial);
    row->setResetEnabled(hasCustom);
    connect(row, &ColorRow::chooseClicked, this, [this, row, onChosen, title]() {
        showColorGrid(row, onChosen, row->color(), title);
    });
    connect(row, &ColorRow::resetClicked, this, resetSlot);
}

void NoteSettingsController::showColorGrid(ColorRow                           *row,
                                           std::function<void(const QColor &)> onSelected,
                                           const QColor                       &currentColor,
                                           const QString                      &title)
{
    bool origResetEnabled = row->isResetEnabled();

    auto result = ColorRow::openGrid(
        currentColor, m_globalSettings.customColors(), m_dialog, title, [row, onSelected](const QColor &color) {
            row->setColor(color);
            onSelected(color);
        });

    m_globalSettings.setCustomColors(result.customColors);

    if (result.accepted) {
        onSelected(result.selectedColor);
        row->setColor(result.selectedColor);
        row->setResetEnabled(true);
    }
    else {
        onSelected(currentColor);
        row->setColor(currentColor);
        row->setResetEnabled(origResetEnabled);
    }
}

void NoteSettingsController::resetFontColor()
{
    m_viewModel->setTextColor(std::nullopt);
    m_dialog->fontColorRow()->setColor(m_viewModel->textColor());
    m_dialog->fontColorRow()->setResetEnabled(false);
}

void NoteSettingsController::resetBgColor()
{
    m_viewModel->setBgColor(std::nullopt);
    m_dialog->bgColorRow()->setColor(m_viewModel->bgColor());
    m_dialog->bgColorRow()->setResetEnabled(false);
}

void NoteSettingsController::resetFontSize()
{
    m_viewModel->setFontSize(std::nullopt);
    m_dialog->fontSizeRow()->blockSignals(true);
    m_dialog->fontSizeRow()->setValue(m_viewModel->fontSize());
    m_dialog->fontSizeRow()->blockSignals(false);
    m_dialog->fontSizeRow()->setResetEnabled(false);
}

void NoteSettingsController::resetFontFamily()
{
    m_viewModel->setFontFamily(std::nullopt);
    m_dialog->fontFamilyRow()->blockSignals(true);
    m_dialog->fontFamilyRow()->setCurrentFont(QFont(m_viewModel->fontFamily()));
    m_dialog->fontFamilyRow()->blockSignals(false);
    m_dialog->fontFamilyRow()->setResetEnabled(false);
}

void NoteSettingsController::resetOpacity()
{
    m_viewModel->setOpacity(std::nullopt);
    m_dialog->opacityRow()->blockSignals(true);
    m_dialog->opacityRow()->setValue(m_viewModel->opacity());
    m_dialog->opacityRow()->blockSignals(false);
    m_dialog->opacityRow()->setResetEnabled(false);
}

void NoteSettingsController::resetIconSize()
{
    m_viewModel->setIconSize(std::nullopt);
    m_dialog->iconSizeRow()->blockSignals(true);
    m_dialog->iconSizeRow()->setValue(m_viewModel->defaultToolbarIconSize());
    m_dialog->iconSizeRow()->blockSignals(false);
    m_dialog->iconSizeRow()->setResetEnabled(false);
}

void NoteSettingsController::revertChanges()
{
    m_viewModel->setFontSize(m_initialData.fontSize());
    m_viewModel->setFontFamily(m_initialData.fontFamily());
    m_viewModel->setTextColor(m_initialData.textColor());
    m_viewModel->setBgColor(m_initialData.bgColor());
    m_viewModel->setOpacity(m_initialData.opacity());
    m_viewModel->setIconSize(m_initialData.iconSize());
    m_globalSettings.setCustomColors(m_initialCustomColors);
}
