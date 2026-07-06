#pragma once

class QWidget;

/// Platform-specific utility for ghost mode (click-through windows).
/// X11: _NET_WM_STATE, Windows: WS_EX_TRANSPARENT, macOS: NSOpaque.
class ClickThroughManager
{
public:
    /// Enables or disables click-through on @p window.
    static void setClickThrough(QWidget *window, bool enable);
    /// Returns true if the platform supports click-through.
    static bool isAvailable();
};
