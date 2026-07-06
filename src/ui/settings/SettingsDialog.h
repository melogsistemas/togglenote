#pragma once

#include "BaseDialog.h"
#include "Settings.h"
#include "ColorRow.h"
#include "SpinRow.h"
#include "SliderSpinRow.h"
#include "FontFamilyRow.h"

#include <QListWidget>

class QCheckBox;
class QSpinBox;
class QTabWidget;
class SettingsViewModel;
class KeybindingsPage;

/// Main preferences dialog with 4 tabs (Appearance, Toolbar, Startup, Shortcuts).
/// Owns UI state temporarily, delegates accept/revert/preview to SettingsViewModel.
class SettingsDialog : public BaseDialog
{
    Q_OBJECT
public:
    explicit SettingsDialog(SettingsViewModel *viewModel, QWidget *parent = nullptr);

    void accept() override;

signals:
    void changed(const Settings &s);

private:
    void     setupUI();
    void     setupAppearanceTab();
    void     setupToolbarTab();
    void     setupStartupTab();
    void     setupShortcutsTab();
    void     emitChanged();
    Settings settings() const;
    void     buildToolbarLayoutList();
    void     saveToolbarLayout();
    void     notifyToolbarLayoutChanged();

    SettingsViewModel *m_viewModel;
    ColorRow          *m_bgColorRow{nullptr};
    ColorRow          *m_fontColorRow{nullptr};
    FontFamilyRow     *m_fontFamilyRow{nullptr};
    SpinRow           *m_fontSizeRow{nullptr};
    SpinRow           *m_iconSizeRow{nullptr};
    SliderSpinRow     *m_opacityRow{nullptr};
    SliderSpinRow     *m_pinHoverOpacityRow{nullptr};
    SliderSpinRow     *m_pinIdleOpacityRow{nullptr};
    QCheckBox         *m_autostartCheck{nullptr};
    QCheckBox         *m_startHiddenCheck{nullptr};
    QCheckBox         *m_defaultOnTopCheck{nullptr};
    QCheckBox         *m_forceXcbCheck{nullptr};
    QCheckBox         *m_autohideCheck{nullptr};
    QSpinBox          *m_timeoutSpin{nullptr};
    QColor             m_selectedColor;
    QColor             m_selectedFontColor;
    int                m_selectedOpacity{100};
    QStringList        m_customColors;
    QTabWidget        *m_tabWidget{nullptr};
    KeybindingsPage   *m_keybindingsPage{nullptr};
    QListWidget       *m_toolbarLayoutList{nullptr};
    QStringList        m_toolbarLayout;
    QSet<QString>      m_toolbarLayoutVis;
};
