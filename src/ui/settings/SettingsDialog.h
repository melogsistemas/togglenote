#pragma once

#include "BaseDialog.h"
#include "Settings.h"
#include "ColorRow.h"
#include "SpinRow.h"
#include "SliderSpinRow.h"
#include "FontFamilyRow.h"
#include "ActionId.h"

#include <QListWidget>

class QCheckBox;
class QSpinBox;
class QTabWidget;
class ISettingsProvider;
class IKeybindingConfiguration;
class KeybindingsPage;

class SettingsDialog : public BaseDialog
{
    Q_OBJECT
public:
    explicit SettingsDialog(ISettingsProvider        *provider,
                            IKeybindingConfiguration *keybindings,
                            QWidget                  *parent = nullptr);

    Settings settings() const;
    void     accept() override;

signals:
    void changed(const Settings &s);

private:
    void setupUI();
    void setupAppearanceTab();
    void setupToolbarTab();
    void setupStartupTab();
    void setupShortcutsTab();
    void emitChanged();
    void buildToolbarLayoutList();
    void saveToolbarLayout();
    void notifyToolbarLayoutChanged();

    ISettingsProvider        *m_provider;
    IKeybindingConfiguration *m_keybindings;
    ColorRow                 *m_bgColorRow{nullptr};
    ColorRow                 *m_fontColorRow{nullptr};
    FontFamilyRow            *m_fontFamilyRow{nullptr};
    SpinRow                  *m_fontSizeRow{nullptr};
    SpinRow                  *m_iconSizeRow{nullptr};
    SliderSpinRow            *m_opacityRow{nullptr};
    SliderSpinRow            *m_pinHoverOpacityRow{nullptr};
    SliderSpinRow            *m_pinIdleOpacityRow{nullptr};
    QCheckBox                *m_autostartCheck{nullptr};
    QCheckBox                *m_startHiddenCheck{nullptr};
    QCheckBox                *m_defaultOnTopCheck{nullptr};
    QCheckBox                *m_forceXcbCheck{nullptr};
    QCheckBox                *m_autohideCheck{nullptr};
    QSpinBox                 *m_timeoutSpin{nullptr};
    QColor                    m_selectedColor;
    QColor                    m_selectedFontColor;
    int                       m_selectedOpacity{100};
    QStringList               m_customColors;
    QTabWidget                *m_tabWidget{nullptr};
    KeybindingsPage           *m_keybindingsPage{nullptr};
    QListWidget               *m_toolbarLayoutList{nullptr};
    QStringList                m_toolbarLayout;
    QSet<QString>              m_toolbarLayoutVis;
    QHash<ActionId, QKeySequence> m_localKeybindings;
};
