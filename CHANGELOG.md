# Changelog

## 1.0.0-dev — Unreleased

### Notes
- Always-on-top notes that float above other windows
- Pin notes — survive Hide All with configurable solo-mode opacity
- Lock notes with double-click temporary edit
- Per-note background color, font, opacity, and icon size
- Right-click drag to move, edge drag to resize
- Multi-monitor position preservation (screen percentage)
- Auto-save (500ms debounced, immediate on focus loss)

### Editor
- Rich text: bold, italic, underline, strikethrough, clear formatting
- Find & replace with next/previous navigation
- Per-note zoom in/out
- Markdown storage with tab preservation

### Toolbar
- Auto-hide with configurable delay (100–10000ms)
- Fully customizable layout — drag-to-reorder, add separators, show/hide buttons
- Configurable icon size (12–64px)
- Context menu with quick access to all actions

### Shortcuts
- 26 remappable keyboard shortcuts
- Conflict detection for shortcut assignments
- Global hotkeys: Ctrl+Alt+N (toggle all), Ctrl+Shift+Space (new note)

### Settings
- Default appearance: background color, font, opacity
- Full shortcut editor with record/clear/reset per action
- Toolbar layout editor with drag-and-drop
- Auto-create note on start, start hidden, default always-on-top

### System
- System tray with show/hide/toggle/new/preferences/quit
- Single-instance enforcement with IPC
- CLI: `--new`, `--show`, `--hide`, `--toggle`, `--quit`
- Cross-platform: Linux (X11), Windows 7–11, macOS 10.15+

### Fixes in this build
- Always-on-top manager no longer forces notes visible on startup
- Ghost mode survives hide/show cycles (click-through preserved)
- Settings dialog Cancel properly reverts all changes including shortcuts
- Color picker Cancel reverts preview changes
- Toolbar context menu respects disabled-item styling
- Global hotkey registration uses Qt's own X11 display (no stale X11 connections)
- ClickThroughManager uses Qt's X11 display instead of separate XOpenDisplay
- macOS: tray icon click only toggles on DoubleClick (not Trigger)
- macOS: CMake `MACOSX` → `APPLE` guard fix, `RUNTIME` → `BUNDLE DESTINATION`
- macOS: post-install `macdeployqt` + `codesign` for signed .app bundles
- X11: ghost mode no longer broken after hide/show cycles
- Toolbar: default layout redesigned (core actions visible, shortcut-only hidden)
- Metainfo: screenshot path fixed to match actual directory
