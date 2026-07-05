#pragma once

#include <QIcon>
#include "Design.h"

class Icons
{
public:
    static QIcon   icon(Design::Icon id, int size = 16);
    static QPixmap renderSvg(const QString &path, const QColor &color, int size);
};

inline QIcon resetIcon()
{
    return Icons::icon(Design::Icon::Reset);
}
inline QIcon plusIcon()
{
    return Icons::icon(Design::Icon::Plus);
}
inline QIcon minusIcon()
{
    return Icons::icon(Design::Icon::Minus);
}
