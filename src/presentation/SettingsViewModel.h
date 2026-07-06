#pragma once

#include <QObject>
#include <QHash>
#include "Settings.h"
#include "ActionId.h"

class ISettingsProvider;
class IKeybindingConfiguration;

/// ViewModel for the global settings dialog.
/// Owns a mutable working copy of Settings + a snapshot for revert.
/// Handles accept/reject/preview semantics — persist only on accept().
class SettingsViewModel : public QObject
{
    Q_OBJECT
public:
    explicit SettingsViewModel(ISettingsProvider        *provider,
                               IKeybindingConfiguration *keybindings,
                               QObject                  *parent = nullptr);

    const Settings &settings() const
    {
        return m_settings;
    }
    const Settings &snapshot() const
    {
        return m_snapshot;
    }
    const QHash<ActionId, QKeySequence> &localKeybindings() const
    {
        return m_localKeybindings;
    }
    IKeybindingConfiguration *keybindingConfiguration() const
    {
        return m_keybindings;
    }
    QHash<ActionId, QKeySequence> *mutableLocalKeybindings()
    {
        return &m_localKeybindings;
    }

    void load();
    void setSettings(const Settings &s);
    void setLocalKeybindings(const QHash<ActionId, QKeySequence> &kb);
    void accept();
    void reject();

signals:
    void changed(const Settings &settings);

private:
    ISettingsProvider            *m_provider;
    IKeybindingConfiguration     *m_keybindings;
    Settings                      m_settings;
    Settings                      m_snapshot;
    QHash<ActionId, QKeySequence> m_localKeybindings;
};
