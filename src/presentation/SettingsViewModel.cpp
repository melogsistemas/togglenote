#include "SettingsViewModel.h"
#include "ISettingsProvider.h"
#include "IKeybindingConfiguration.h"

SettingsViewModel::SettingsViewModel(ISettingsProvider        *provider,
                                     IKeybindingConfiguration *keybindings,
                                     QObject                  *parent)
    : QObject(parent)
    , m_provider(provider)
    , m_keybindings(keybindings)
{}

void SettingsViewModel::load()
{
    m_settings = m_provider->settings();
    m_snapshot = m_settings;

    m_localKeybindings.clear();
    const auto &kb = m_settings.keybindings();
    for (auto it = kb.constBegin(); it != kb.constEnd(); ++it)
        m_localKeybindings[ActionId::fromString(it.key())] = it.value();
}

void SettingsViewModel::setSettings(const Settings &s)
{
    m_settings = s;
    emit changed(m_settings);
}

void SettingsViewModel::setLocalKeybindings(const QHash<ActionId, QKeySequence> &kb)
{
    m_localKeybindings = kb;
    QHash<QString, QKeySequence> serialized;
    for (auto it = kb.constBegin(); it != kb.constEnd(); ++it)
        serialized[it.key().toString()] = it.value();
    m_settings.setKeybindings(serialized);
    emit changed(m_settings);
}

void SettingsViewModel::accept()
{
    QHash<QString, QKeySequence> serialized;
    for (auto it = m_localKeybindings.constBegin(); it != m_localKeybindings.constEnd(); ++it)
        serialized[it.key().toString()] = it.value();
    m_settings.setKeybindings(serialized);

    m_provider->save(m_settings);
    m_snapshot = m_settings;
}

void SettingsViewModel::reject()
{
    m_settings = m_snapshot;
}
