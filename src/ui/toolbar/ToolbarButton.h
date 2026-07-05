#pragma once

#include <QIcon>
#include <QString>
#include <functional>
#include "ButtonId.h"

struct ToolbarButton
{
    QString                   id;
    std::function<QIcon(int)> iconForSize;
    QString                   tooltip;
    std::function<void(bool)> callback;
    bool                      checkable    = false;
    bool                      rightAligned = false;
};
