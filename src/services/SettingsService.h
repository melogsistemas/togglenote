#pragma once

#include <QObject>
#include "ISettingsProvider.h"

class ISettingsRepository;

class SettingsService : public QObject, public ISettingsProvider
{
    Q_OBJECT
public:
    explicit SettingsService(ISettingsRepository *repo, QObject *parent = nullptr);

    void save(const Settings &settings) override;

    const Settings &settings() const override
    {
        return m_settings;
    }

signals:
    void settingsChanged();

private:
    void load();

    ISettingsRepository *m_repo;
    Settings             m_settings;
};
