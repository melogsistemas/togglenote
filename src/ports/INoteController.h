#pragma once

#include <QObject>
#include "Settings.h"

class QWidget;

/// Controller interface for a single note instance.
/// Wires a NoteViewModel to a NoteWindow and bridges user actions to the manager.
class INoteController : public QObject
{
    Q_OBJECT
public:
    using QObject::QObject;
    ~INoteController() override = default;

    /// Immediately persists the note content to disk.
    virtual void saveNow() = 0;
    /// Shows the note window (de-iconifies, raises).
    virtual void showWindow() = 0;
    /// Hides the note window (iconifies).
    virtual void hideWindow() = 0;
    /// Returns true if the note window is currently visible (not iconified).
    virtual bool isWindowVisible() const = 0;
    /// Returns the note's QWidget for use in layout/parenting.
    virtual QWidget *noteWidget() const = 0;
    /// Propagates global setting changes to this note. If @p restoreAutohide is true,
    /// the toolbar auto-hide timer is restarted (used on settings dialog reject).
    virtual void applyGlobalSettings(const Settings &settings, bool restoreAutohide) = 0;
    /// Enables or disables toolbar auto-hide for this note.
    virtual void setAutohide(bool on) = 0;
    /// Returns true if the note is pinned (survives hide-all).
    virtual bool isPinned() const = 0;
    /// When solo mode is on, pinned notes get full opacity while others dim.
    virtual void setPinSoloMode(bool on) = 0;
    /// Toggles ghost mode (click-through, non-interactive).
    virtual void setGhostMode(bool on) = 0;
    /// Returns true if ghost mode is active.
    virtual bool isGhostMode() const = 0;

signals:
    void newNoteRequested();
    void preferencesRequested();
    /// Emitted when the user requests this note be deleted.
    void noteDeleted(INoteController *controller);
    void hideAllNotesRequested();
};
