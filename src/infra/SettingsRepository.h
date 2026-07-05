#pragma once

#include "ISettingsRepository.h"

class SettingsRepository : public ISettingsRepository
{
public:
    explicit SettingsRepository(const QString &configDir);

    Settings getSettings() override;
    bool     save(const Settings &settings) override;

private:
    static bool migrateToolbarLayout(Settings &settings);
    QString     m_configDir;
};
