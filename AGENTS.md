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

## Naming & code style

- `#pragma once` exclusively in all headers (no `#ifndef` guards)
- `m_` prefix for all member variables (e.g. `m_repo`, `m_settings`)
- `k` prefix for static/global constants (e.g. `kMin`, `kMax`, `kDefaultSize`)
- PascalCase for namespaces (`Constants`, `Defaults`, `Editor`, `Notes`, `Global`, `SettingsRowUtils`)
- PascalCase file names matching class name (e.g. `NoteWindow.h` contains `class NoteWindow`)
- `explicit` on all single-argument constructors
- `override` on every overridden virtual method
- `nullptr` instead of `NULL` or `0`
- Braced initialization `Type var{value}` for members and local variables
- Simple getters/setters defined inline in headers

## Architecture & layering

- **Clean Architecture + MVVM**: dependency flow points strictly inward — `ui/` → `presentation/` → `services/` → `ports/` ← `infra/` → `core/`. Lower layers never depend on higher layers.
- **Interface segregation**: `ports/` contains small, focused pure-virtual contracts (7 interfaces with 5–15 methods each).
- **Service-Repository pattern**: `XxxService` depends on `IXxxRepository` interface; repository implementations depend only on `core/`.
- **Platform-specific files**: use `_x11.cpp`, `_win.cpp`, `_mac.cpp`/`.mm` suffixes selected via CMake generator expressions. Use `#ifdef` within a single file only for small differences.
- **ViewModels** extend `QObject` (never `QWidget`), emit signals for view binding.
- **BaseDialog** provides `TitleBar` + `FramelessWindow` + OK/Cancel; settings pages add rows via `contentLayout()`.
- **Row widgets** (`ColorRow`, `SpinRow`, `SliderSpinRow`, `FontFamilyRow`) extend `ResettableRow` which provides a reset button with `resetClicked()` signal.
- **Thin subclasses**: `LockButton`, `PinButton`, `GhostIndicator` are minimal `CornerButton` subclasses that only configure their parent.

## Signals & slots

- Use `signals:`, `public slots:`, `private slots:` access specifiers (not `Q_SIGNALS`/`Q_SLOTS` macros)
- Lambda connections preferred for complex wiring (e.g. settings propagation, global hotkey dispatch in `Application.cpp`)
- Signal chaining: controllers convert widget signals into higher-level semantic signals (e.g. `NoteController` converts `NoteWindow::deleteRequested` into `noteDeleted(INoteController*)`)

## CMake conventions

- List sources explicitly in `SOURCES` and `HEADERS` variables (never `file(GLOB ...)`). Headers are listed so they appear in IDE project files.
- Guard platform-specific `find_package` with conditionals (e.g. X11 only on Linux)
- Use `QT_VERSION_CHECK` in code for Qt5/Qt6 compatibility branches (used in `NoteEditor`, `NoteWindow`, `GlobalHotkeyService`)
- `TN_DEBUG` preprocessor define set in Debug builds, used for debug IPC server name
- LTO enabled via `check_ipo_supported`
- `compile_commands.json` symlinked to build dir for clangd

## Error handling

- `qWarning()` for error logging (no custom logging framework)
- No exceptions — zero `try`/`catch` in the codebase. Errors signaled via return values (bool) or logged.
- `std::optional` for "value not set" (distinguishes unset per-note overrides from global defaults)

## SVG icons

- Icons rendered at runtime with colorization via `QSvgRenderer` in `Icons::renderSvg()`
- `ToolbarButton` uses `std::function<QIcon(int)>` icon factory for size-dependent rendering

## Build system & resources

- `CMAKE_AUTOMOC` handles all moc generation (no manual `#include "moc_...cpp"`)
- Property-based QSS selectors: `QPushButton[role="reset"]`, `QPushButton[role="step"]` in `theme.qss`

## Versioning

- Single version source: `APP_VERSION` in `src/app/constants.h`
- Changelog (`CHANGELOG.md`) maintained as unreleased until a release tag
