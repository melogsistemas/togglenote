#pragma once

#include "Settings.h"

class ISettingsProvider
{
public:
    virtual ~ISettingsProvider()                           = default;
    virtual const Settings &settings() const               = 0;
    virtual void            save(const Settings &settings) = 0;
};
