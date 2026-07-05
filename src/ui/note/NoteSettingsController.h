#pragma once

#include <QObject>
#include <functional>
#include "Note.h"
#include "Settings.h"

class NoteViewModel;
class NoteSettingsDialog;
class ColorRow;
class SpinRow;
class SliderSpinRow;
class FontFamilyRow;

class NoteSettingsController : public QObject
{
    Q_OBJECT
public:
    using CustomColorsCb = std::function<void(const QStringList &)>;

    explicit NoteSettingsController(NoteViewModel  *viewModel,
                                    QWidget        *parent,
                                    const Settings &globalSettings,
                                    CustomColorsCb  onCustomColorsChanged = nullptr);
    ~NoteSettingsController() override;

    void showAt(const QPoint &pos);

private:
    template<typename Row>
    void setupRow(Row                  *row,
                  std::function<void()> applyInitial,
                  bool                  hasCustom,
                  std::function<void()> connectChanged,
                  std::function<void()> resetSlot);

    void setupColorRow(ColorRow                           *row,
                       std::function<void(const QColor &)> onChosen,
                       const QColor                       &initial,
                       bool                                hasCustom,
                       std::function<void()>               resetSlot,
                       const QString                      &title = QString());

    void chooseFontColor();
    void chooseBgColor();

    void resetFontColor();
    void resetBgColor();
    void resetFontSize();
    void resetFontFamily();
    void resetOpacity();
    void resetIconSize();

    void showColorGrid(ColorRow                           *row,
                       std::function<void(const QColor &)> onSelected,
                       const QColor                       &currentColor,
                       const QString                      &title = QString());
    void revertChanges();

    NoteViewModel      *m_viewModel;
    Settings            m_globalSettings;
    QStringList         m_initialCustomColors;
    CustomColorsCb      m_onCustomColorsChanged;
    Note                m_initialData;
    NoteSettingsDialog *m_dialog;
};

// Template implementation — must be in the header
template<typename Row>
void NoteSettingsController::setupRow(Row                  *row,
                                      std::function<void()> applyInitial,
                                      bool                  hasCustom,
                                      std::function<void()> connectChanged,
                                      std::function<void()> resetSlot)
{
    applyInitial();
    row->setResetEnabled(hasCustom);
    connectChanged();
    connect(row, &Row::resetClicked, this, resetSlot);
}
