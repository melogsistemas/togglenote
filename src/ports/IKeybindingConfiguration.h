#pragma once

#include "IKeybindingProvider.h"
#include "ActionDefinition.h"
#include "ActionId.h"

class IKeybindingConfiguration : public virtual IKeybindingProvider
{
public:
    virtual QString                 setShortcut(ActionId actionId, const QKeySequence &seq) = 0;
    virtual void                    resetShortcut(ActionId actionId)                        = 0;
    virtual void                    resetAllShortcuts()                                     = 0;
    virtual QList<ActionDefinition> allActions() const                                      = 0;
    virtual ActionDefinition        actionDef(ActionId actionId) const                      = 0;
    virtual QString                 validateShortcut(ActionId actionId, const QKeySequence &seq) const = 0;
};
