#include "Design.h"

namespace Design
{

QString svgPath(Icon id)
{
    switch (id) {
    case Icon::New:
        return QStringLiteral(":/icons/new.svg");
    case Icon::Delete:
        return QStringLiteral(":/icons/delete.svg");
    case Icon::Cut:
        return QStringLiteral(":/icons/cut.svg");
    case Icon::Copy:
        return QStringLiteral(":/icons/copy.svg");
    case Icon::Paste:
        return QStringLiteral(":/icons/paste.svg");
    case Icon::Undo:
        return QStringLiteral(":/icons/undo.svg");
    case Icon::Redo:
        return QStringLiteral(":/icons/redo.svg");
    case Icon::Search:
        return QStringLiteral(":/icons/search.svg");
    case Icon::Bold:
        return QStringLiteral(":/icons/bold.svg");
    case Icon::Italic:
        return QStringLiteral(":/icons/italic.svg");
    case Icon::Underline:
        return QStringLiteral(":/icons/underline.svg");
    case Icon::Strikethrough:
        return QStringLiteral(":/icons/strikethrough.svg");
    case Icon::ClearFormat:
        return QStringLiteral(":/icons/clearformat.svg");
    case Icon::Ontop:
        return QStringLiteral(":/icons/ontop.svg");
    case Icon::Pin:
        return QStringLiteral(":/icons/pin.svg");
    case Icon::Settings:
        return QStringLiteral(":/icons/settings.svg");
    case Icon::Lock:
        return QStringLiteral(":/icons/lock.svg");
    case Icon::Reset:
        return QStringLiteral(":/icons/reset.svg");
    case Icon::Plus:
        return QStringLiteral(":/icons/plus.svg");
    case Icon::Minus:
        return QStringLiteral(":/icons/minus.svg");
    case Icon::Close:
        return QStringLiteral(":/icons/close.svg");
    case Icon::Minimize:
        return QStringLiteral(":/icons/minimize.svg");
    case Icon::Maximize:
        return QStringLiteral(":/icons/maximize.svg");
    case Icon::Restore:
        return QStringLiteral(":/icons/restore.svg");
    case Icon::HideNote:
        return QStringLiteral(":/icons/hidenote.svg");
    case Icon::HideAllNotes:
        return QStringLiteral(":/icons/hideallnotes.svg");
    case Icon::Tool:
        return QStringLiteral(":/icons/tool.svg");
    case Icon::ZoomIn:
        return QStringLiteral(":/icons/zoomin.svg");
    case Icon::ZoomOut:
        return QStringLiteral(":/icons/zoomout.svg");
    case Icon::QuickNote:
        return QStringLiteral(":/icons/quicknote.svg");
    case Icon::HideShowNote:
        return QStringLiteral(":/icons/hideshownote.svg");
    case Icon::Backspace:
        return QStringLiteral(":/icons/backspace.svg");
    case Icon::Ghost:
        return QStringLiteral(":/icons/ghost.svg");
    }
    return {};
}

} // namespace Design
