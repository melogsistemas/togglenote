#pragma once

#include "Settings.h"

class ISettingsRepository
{
public:
    virtual ~ISettingsRepository()                  = default;
    virtual Settings getSettings()                  = 0;
    virtual bool     save(const Settings &settings) = 0;
};
