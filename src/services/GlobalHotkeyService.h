#pragma once

#include <QObject>
#include <QAbstractNativeEventFilter>
#include <QHash>
#include <QKeySequence>
#include "ActionId.h"

class KeybindingService;

/// Registers and dispatches global (system-wide) hotkeys.
/// Platform-specific: X11 (xcb), Windows (RegisterHotKey), macOS (Carbon).
/// Watches native events via QAbstractNativeEventFilter.
class GlobalHotkeyService : public QObject, public QAbstractNativeEventFilter
{
    Q_OBJECT
public:
    explicit GlobalHotkeyService(KeybindingService *keybindings, QObject *parent = nullptr);
    ~GlobalHotkeyService() override;

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    bool nativeEventFilter(const QByteArray &eventType, void *message, qintptr *result) override;
#else
    bool               nativeEventFilter(const QByteArray &eventType, void *message, long *result) override;
#endif

    QString registerOne(ActionId actionId, const QKeySequence &seq);

#if Q_OS_MACOS
    QHash<int, ActionId>        m_macRegisteredIds;
    static GlobalHotkeyService *s_macInstance;
#endif

signals:
    void triggered(ActionId actionId);

private:
    void registerAll();
    void unregisterAll();

    KeybindingService *m_keybindings;
    bool               m_isWayland = false;
#ifdef Q_OS_WIN
    QHash<int, ActionId> m_registeredIds;
    int                  m_nextId{1};
#elif defined(Q_OS_MACOS)
    QHash<int, void *> m_macRefs;
    int                m_macNextId{1};
#else
    struct X11Hotkey
    {
        unsigned char keycode;
        unsigned int  mods;
    };
    QHash<ActionId, X11Hotkey> m_x11Hotkeys;
#endif
};
