#pragma once

#include "Settings.h"

/// In-memory settings cache with persist-on-save semantics.
/// Implementations (e.g. SettingsService) cache Settings and emit signals on save.
class ISettingsProvider
{
public:
    virtual ~ISettingsProvider() = default;
    /// Returns the current in-memory settings snapshot.
    virtual const Settings &settings() const = 0;
    /// Replaces the in-memory cache and persists to disk.
    virtual void save(const Settings &settings) = 0;
};
