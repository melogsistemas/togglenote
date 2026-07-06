#pragma once

#include <QKeySequence>
#include <QObject>
#include <functional>
#include "ActionId.h"

/// Read-only query interface for keybinding resolution.
class IKeybindingProvider
{
public:
    virtual ~IKeybindingProvider() = default;
    /// Returns the QKeySequence mapped to @p actionId (possibly empty).
    virtual QKeySequence shortcut(ActionId actionId) const = 0;
    /// Registers a callback invoked when bindings change. Returns the QMetaObject::Connection.
    virtual QMetaObject::Connection onBindingsChanged(QObject *receiver, std::function<void()> callback) = 0;
};
