#pragma once

#include <QKeySequence>
#include <QObject>
#include <functional>
#include "ActionId.h"

class IKeybindingProvider
{
public:
    virtual ~IKeybindingProvider()                                                                       = default;
    virtual QKeySequence            shortcut(ActionId actionId) const                                    = 0;
    virtual QMetaObject::Connection onBindingsChanged(QObject *receiver, std::function<void()> callback) = 0;
};
