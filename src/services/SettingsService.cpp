#include "SettingsService.h"
#include "ISettingsRepository.h"

SettingsService::SettingsService(ISettingsRepository *repo, QObject *parent)
    : QObject(parent)
    , m_repo(repo)
{
    load();
}

void SettingsService::load()
{
    m_settings = m_repo->getSettings();
}

void SettingsService::save(const Settings &settings)
{
    m_settings = settings;
    m_repo->save(m_settings);
    emit settingsChanged();
}
