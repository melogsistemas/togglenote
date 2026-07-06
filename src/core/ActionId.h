#pragma once

#include <QString>
#include <QHash>

/// Strongly-typed integer wrapper for action identifiers.
/// Namespace-scoped constants define all available actions:
///   Editor::   (0–14) editing commands
///   Notes::    (100–107) note-level operations
///   Global::   (200–202) application-wide hotkeys
struct ActionId
{
    int value;

    bool operator==(const ActionId &o) const
    {
        return value == o.value;
    }
    bool operator!=(const ActionId &o) const
    {
        return value != o.value;
    }
    bool operator<(const ActionId &o) const
    {
        return value < o.value;
    }

    /// Serializes the action ID to its dotted-string key (e.g. "editor.bold").
    QString toString() const;
    /// Deserializes a dotted-string key back to an ActionId.
    static ActionId fromString(const QString &str);
};

namespace Editor
{
inline constexpr ActionId Cut{0};
inline constexpr ActionId Copy{1};
inline constexpr ActionId Paste{2};
inline constexpr ActionId Undo{3};
inline constexpr ActionId Redo{4};
inline constexpr ActionId Bold{5};
inline constexpr ActionId Italic{6};
inline constexpr ActionId Underline{7};
inline constexpr ActionId Strikethrough{8};
inline constexpr ActionId ClearFormat{9};
inline constexpr ActionId Find{10};
inline constexpr ActionId FindNext{11};
inline constexpr ActionId FindPrev{12};
inline constexpr ActionId ZoomIn{13};
inline constexpr ActionId ZoomOut{14};
} // namespace Editor

namespace Notes
{
inline constexpr ActionId New{100};
inline constexpr ActionId Delete{101};
inline constexpr ActionId Lock{102};
inline constexpr ActionId Ontop{103};
inline constexpr ActionId ToolbarToggle{104};
inline constexpr ActionId Hide{105};
inline constexpr ActionId HideAll{106};
inline constexpr ActionId Prefs{107};
} // namespace Notes

namespace Global
{
inline constexpr ActionId ToggleNotes{200};
inline constexpr ActionId QuickCapture{201};
inline constexpr ActionId GhostMode{202};
} // namespace Global

inline uint qHash(const ActionId &id, uint seed = 0)
{
    return ::qHash(id.value, seed);
}
