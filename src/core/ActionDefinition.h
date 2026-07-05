#pragma once

#include <QString>
#include <QKeySequence>
#include <QList>

#include "ActionId.h"
#include "ButtonId.h"
#include "Icon.h"

struct ActionDefinition
{
    ActionId     id;
    QString      key;
    QString      label;
    QString      category;
    QKeySequence defaultShortcut;
    Design::Icon icon         = static_cast<Design::Icon>(-1);
    ButtonId     btn          = ButtonId::None;
    bool         checkable    = false;
    bool         rightAligned = false;
    int          toolbarGroup = -1;
    int          sortOrder    = 0;
};

QList<ActionDefinition> allActionDefinitions();
