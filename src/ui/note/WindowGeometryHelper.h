#pragma once

#include <QPair>

class QWidget;
class Note;

/// Utility for saving/restoring window positions as screen percentages.
/// Enables correct multi-monitor positioning across sessions.
class WindowGeometryHelper
{
public:
    /// Restores @p window position from Note's xPct/yPct on the current screen.
    static void applyPosition(QWidget *window, const Note &data);
    /// Captures the current position as screen percentage pair (xPct, yPct).
    static QPair<double, double> capturePositionPercent(QWidget *window);
};
