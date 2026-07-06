#pragma once

#include <QString>
#include <QKeySequence>
#include <QList>

#include "ActionId.h"
#include "ButtonId.h"
#include "Icon.h"

/// Metadata for a single bindable action.
/// Includes display label, category, default shortcut, icon, and toolbar placement hints.
struct ActionDefinition
{
    ActionId     id;
    QString      key; ///< Dotted-string key (e.g. "editor.bold").
    QString      label; ///< Human-readable name for settings UI.
    QString      category; ///< Group name (e.g. "Editor", "Notes").
    QKeySequence defaultShortcut;
    Design::Icon icon         = static_cast<Design::Icon>(-1);
    ButtonId     btn          = ButtonId::None; ///< Toolbar button enum, if applicable.
    bool         checkable    = false;
    bool         rightAligned = false; ///< Place on the right side of the toolbar.
    int          toolbarGroup = -1; ///< Ordering group for toolbar layout.
    int          sortOrder    = 0; ///< Within-group sort order.
};

/// Returns metadata for every bindable action in the application.
QList<ActionDefinition> allActionDefinitions();
