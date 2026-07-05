#pragma once

#include <QtGlobal>
#include <functional>

enum class ButtonId
{
    None,
    New,
    Delete,
    Cut,
    Copy,
    Paste,
    Undo,
    Redo,
    Find,
    Bold,
    Italic,
    Underline,
    Strike,
    ClearFormat,
    Ontop,
    Prefs,
    HideNote,
    HideAllNotes,
    ZoomIn,
    ZoomOut,
    GhostMode,
};

inline uint qHash(ButtonId key, uint seed = 0) noexcept
{
    return std::hash<int>{}(static_cast<int>(key)) ^ (seed * 0x9e3779b9u);
}
