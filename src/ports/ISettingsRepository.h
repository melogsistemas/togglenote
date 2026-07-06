#pragma once

#include "Settings.h"

/// Persistence contract for application settings (INI file).
class ISettingsRepository
{
public:
    virtual ~ISettingsRepository() = default;
    /// Loads settings from persistent storage.
    virtual Settings getSettings() = 0;
    /// Writes settings to persistent storage. Returns false on failure.
    virtual bool save(const Settings &settings) = 0;
};
