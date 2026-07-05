# Conventions

## Settings: modify in memory, persist only on accept

All settings modifications in dialogs (e.g., `SettingsDialog`) must update local member variables only. Persistence to `ISettingsProvider` must happen exclusively in `accept()`, never while the dialog is open.

**Correct:**
```cpp
void SettingsDialog::saveToolbarLayout()
{
    m_toolbarLayout.clear();
    for (...) { m_toolbarLayout.append(id); }
}
void SettingsDialog::accept()
{
    m_provider->save(settings());
    QDialog::accept();
}
Settings SettingsDialog::settings() const
{
    result.setToolbarLayout(m_toolbarLayout);
    return result;
}
```

**Wrong** (persists before accept, so Cancel can't revert):
```cpp
void SettingsDialog::saveToolbarLayout()
{
    result.setToolbarLayout(layout);
    m_provider->save(result);  // NO — persists immediately
}
```

### Real-time preview pattern

To show changes in real-time (while the dialog is open):
1. Update the local member variable
2. Emit `changed(settings())` — this signal triggers `applyGlobalSettings(s, false)` which updates the UI without persisting
3. On Accept: `m_provider->save(settings())` persists everything at once
4. On Reject: the original settings snapshot is restored via `applyGlobalSettings(snapshot, true)`

### Toolbar layout

`ToolbarController::reloadToolbarLayout()` has two overloads:
- `reloadToolbarLayout()` — reads from provider (used on init)
- `reloadToolbarLayout(const QStringList &layout, const QSet<QString> &vis)` — uses the passed layout directly, without persisting (used for real-time preview)

`buildToolbarSpec()` uses `m_toolbarLayout` member, not the provider. Layout is always stored locally and only written to the provider on explicit save.

## Context menus: application-wide consistency

All context menus (`QMenu`) must share the same styling via the global `theme.qss`.

### Disabled items
Always add `QMenu::item:disabled { color: #62686e; }` to the global theme.

### Avoid local setStyleSheet on ancestors
Local `setStyleSheet()` on parent widgets creates a stylesheet scope that can break `QMenu` styling for popups within that subtree. Use `QPalette` instead when possible, or move the rule to the global QSS.

## Right-click + drag (FramelessWindow)

Right-click triggers a drag if the mouse moves >5px (`ClosedHandCursor`). If no drag, a manual `QContextMenuEvent` is sent to the target widget. Widgets that don't handle context menus (e.g., `QToolButton`, children of `ToolbarWidget`) forward the event to `m_window` (the `NoteWindow`), which shows the toolbar context menu.
