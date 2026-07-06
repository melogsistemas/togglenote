#pragma once

#include "IKeybindingProvider.h"
#include "ActionDefinition.h"
#include "ActionId.h"

/// Read-write interface for modifying keybinding mappings.
class IKeybindingConfiguration : public virtual IKeybindingProvider
{
public:
    /// Assigns @p seq to @p actionId. Returns an error message, or empty on success.
    virtual QString setShortcut(ActionId actionId, const QKeySequence &seq) = 0;
    /// Resets @p actionId to its default shortcut.
    virtual void resetShortcut(ActionId actionId) = 0;
    /// Resets all shortcuts to their factory defaults.
    virtual void resetAllShortcuts() = 0;
    /// Returns metadata for every bindable action.
    virtual QList<ActionDefinition> allActions() const = 0;
    /// Returns metadata for a single action.
    virtual ActionDefinition actionDef(ActionId actionId) const = 0;
    /// Returns an error message if @p seq conflicts with another action, or empty if valid.
    virtual QString validateShortcut(ActionId actionId, const QKeySequence &seq) const = 0;
};
