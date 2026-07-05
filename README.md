# ToggleNote

Lightweight sticky notes that stay where you need them.

Free, open source, and privacy-respecting. Built with C++17 and Qt (6 on Linux, 5.15 on Windows/macOS).

![License](https://img.shields.io/badge/license-MIT-blue)
![Platform](https://img.shields.io/badge/platform-Linux%20%7C%20Windows%20%7C%20macOS-lightgrey)
![Qt](https://img.shields.io/badge/Qt-6%20%7C%205.15-green)

## Features

### Notes
- **Always on top** — notes stay visible over your other windows
- **Pin notes** — pin important notes so they survive "Hide All", with configurable solo-mode opacity
- **Lock notes** — prevent accidental edits; double-click for temporary edit mode
- **Ghost mode** — notes become click-through and non-interactive for passive reference
- **Per-note settings** — each note has its own background color, font, opacity slider (10–100%), and icon size
- **Drag & resize** — right-click drag from anywhere, or left-click drag from toolbar; resize from any edge; **Alt+right-click** to resize from bottom-right corner
- **Multi-monitor** — position saved as screen percentage, re-snaps correctly across monitors
- **Auto-save** — debounced 500ms save on edit, immediate save on focus loss

### Editor
- **Rich text** — bold, italic, underline, strikethrough, clear formatting
- **Find & replace** — in-note search with next/previous navigation (F3 / Shift+F3)
- **Zoom** — increase/decrease font size per note
- **Markdown storage** — notes saved as markdown files with tab preservation

### Toolbar
- **Auto-hide toolbar** — appears on hover, configurable hide delay (100–10000ms)
- **Configurable layout** — drag-to-reorder buttons, add/remove separators, show/hide individual buttons
- **Context menu** — right-click for quick access to all actions
- **Configurable icon size** — 12–64px toolbar icons

### Shortcuts
- **Fully remappable** — all 26 keyboard shortcuts customizable from Preferences
- **Conflict detection** — prevents assigning the same shortcut to two actions
- **Global hotkeys** — system-wide Ctrl+Alt+N (toggle all) and Ctrl+Shift+Space (new note)

### Settings
- **Appearance** — default background color, font color, family, size, opacity
- **Toolbar** — auto-hide toggle, hide delay, layout editor with drag-and-drop
- **Startup** — auto-create note, start hidden, default always-on-top
- **Shortcuts** — full shortcut editor with record, clear, and reset

### System
- **System tray** — create, show/hide, toggle all, preferences, quit
- **Single instance** — only one instance runs; second instance forwards CLI args via IPC
- **Command-line** — `--new`, `--show`, `--hide`, `--toggle`, `--ghost`, `--quit`
- **Cross-platform** — Linux (X11), Windows 7–11, macOS hotkey support

## Build from source

**Requirements:**

- CMake 3.16+
- C++17 compiler
- Linux: Qt 6.5+ (Widgets, Network, Svg), libx11-dev, libxcb1-dev, libxext-dev
- Windows/macOS: Qt 5.15 (Widgets, Network, Svg)
- macOS (Homebrew): `brew install qt@5` then `cmake -B build -DCMAKE_PREFIX_PATH=$(brew --prefix qt@5)`

```bash
git clone https://github.com/melogsistemas/togglenote.git
cd togglenote
cmake -B build
cmake --build build --parallel
```

### Install

```bash
sudo cmake --install build
```

### Uninstall

```bash
sudo cmake --build build --target uninstall
```

## Usage

```bash
# Start normally (opens a new note if none exist)
togglenote

# Show all hidden notes
togglenote --show

# Hide all notes
togglenote --hide

# Toggle visibility of all notes
togglenote --toggle

# Open a new note
togglenote --new

# Toggle ghost mode for all notes
togglenote --ghost

# Quit the application
togglenote --quit

# Show version
togglenote --version

# Show help
togglenote --help
```

## Keyboard Shortcuts

| Action | Default Shortcut |
|--------|------------------|
| New Note | Ctrl+N |
| Delete Note | Ctrl+Shift+Delete |
| Toggle Lock | Ctrl+L |
| Always on Top | Ctrl+Shift+T |
| Toggle Toolbar | Ctrl+Shift+H |
| Cut | Ctrl+X |
| Copy | Ctrl+C |
| Paste | Ctrl+V |
| Undo | Ctrl+Z |
| Redo | Ctrl+Shift+Z |
| Bold | Ctrl+B |
| Italic | Ctrl+I |
| Underline | Ctrl+U |
| Strikethrough | Ctrl+Shift+S |
| Clear Formatting | Ctrl+Space |
| Find | Ctrl+F |
| Find Next / Previous | F3 / Shift+F3 |
| Zoom In / Out | Ctrl++ / Ctrl+- |
| Ghost Mode | Ctrl+Alt+G |
| Show/Hide All Notes (global) | Ctrl+Alt+N |
| Quick Note (global) | Ctrl+Shift+Space |

All shortcuts are fully remappable from Preferences > Shortcuts.

## Architecture

ToggleNote follows **Clean Architecture + MVVM** principles:

```
src/
  app/          ← Composition root, CLI, tray, IPC
  core/         ← Domain (Note, Settings, ActionDef)
  ports/        ← Interfaces (contracts)
  infra/        ← File-based implementations (settings, notes)
  services/     ← KeybindingService, GlobalHotkeyFilter
  presentation/ ← NoteViewModel
  design/       ← Design tokens (colors, icons, theme)
  ui/
    note/       ← NoteWindow, NoteEditor, ToolbarController, SearchBar
    settings/   ← SettingsDialog, KeybindingsPage, ColorGrid
    common/     ← FramelessWindow, Icons, TrayIcon, BaseDialog
    toolbar/    ← ToolbarWidget, ContextMenu
```

## Privacy & Performance

- **~3–5 MB RAM** per idle note — lightweight native C++ app, no Electron bloat
- **No telemetry** — zero tracking, zero analytics, zero internet calls
- **No accounts** — no signup, no login, no cloud dependency
- **Offline first** — 100% functional without internet
- **Flat files** — each note is a plain `.md` file on your disk. Open them in any text editor. No databases, no proprietary formats.

## Contributing

1. Fork the repository
2. Create a feature branch
3. Commit your changes
4. Push to the branch
5. Open a Pull Request

Please follow the existing code style and keep commits focused.

## License

MIT License — see [LICENSE](LICENSE) for details.

## Links

- [Report a bug](https://github.com/melogsistemas/togglenote/issues)
