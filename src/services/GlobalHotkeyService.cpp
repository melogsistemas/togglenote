#include "GlobalHotkeyService.h"
#include "KeybindingService.h"
#include "ActionDefinition.h"
#include <QGuiApplication>
#include <QDebug>

#ifdef Q_OS_WIN
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#elif defined(Q_OS_MACOS)
#include <Carbon/Carbon.h>
#else
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <xcb/xcb.h>
#include <xcb/xproto.h>

static Display *x11Display()
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 2, 0)
    if (auto *x11App = qGuiApp->nativeInterface<QNativeInterface::QX11Application>())
        return x11App->display();
    return nullptr;
#else
    return QX11Info::display();
#endif
}
#endif

#ifdef Q_OS_WIN
UINT toNativeKey(Qt::Key keycode);
UINT toNativeModifiers(Qt::KeyboardModifiers mods);
#elif defined(Q_OS_MACOS)
OSStatus macHotkeyHandler(EventHandlerCallRef, EventRef, void *);
quint32  macKeycode(Qt::Key key, bool &ok);
quint32  macModifiers(Qt::KeyboardModifiers mods, bool &ok);
#else
extern const unsigned int specialModifiers[4];
extern const unsigned int validModsMask;
unsigned int              qtModsToXMods(Qt::KeyboardModifiers mods);
KeySym                    qtKeyToKeySym(Qt::Key key);
#endif

GlobalHotkeyService::GlobalHotkeyService(KeybindingService *keybindings, QObject *parent)
    : QObject(parent)
    , m_keybindings(keybindings)
{
#if !defined(Q_OS_WIN) && !defined(Q_OS_MACOS)
    m_isWayland = (QGuiApplication::platformName() == "wayland");
    if (m_isWayland) {
        qWarning("ToggleNote: Wayland detected — global hotkeys and ghost mode (click-through) are not supported. "
                 "Run under X11/XWayland for full functionality.");
    }
#endif

#if defined(Q_OS_MACOS)
    s_macInstance = this;

    EventTypeSpec eventSpec;
    eventSpec.eventClass = kEventClassKeyboard;
    eventSpec.eventKind  = kEventHotKeyPressed;
    InstallApplicationEventHandler(&macHotkeyHandler, 1, &eventSpec, NULL, NULL);
#endif

    registerAll();
    connect(m_keybindings, &KeybindingService::bindingsChanged, this, &GlobalHotkeyService::registerAll);
}

GlobalHotkeyService::~GlobalHotkeyService()
{
    unregisterAll();
#if defined(Q_OS_MACOS)
    s_macInstance = nullptr;
#endif
}

void GlobalHotkeyService::registerAll()
{
    unregisterAll();
#if !defined(Q_OS_WIN) && !defined(Q_OS_MACOS)
    if (m_isWayland)
        return;
#endif
    const auto actions = m_keybindings->allActions();
    for (const auto &def : actions) {
        if (def.id.value < 200)
            continue;
        QKeySequence seq = m_keybindings->shortcut(def.id);
        if (seq.isEmpty())
            continue;
        QString err = registerOne(def.id, seq);
        if (!err.isEmpty())
            qWarning("GlobalHotkeyService: %s: %s", qPrintable(def.id.toString()), qPrintable(err));
    }
}

void GlobalHotkeyService::unregisterAll()
{
#ifdef Q_OS_WIN
    for (auto it = m_registeredIds.begin(); it != m_registeredIds.end(); ++it)
        UnregisterHotKey(NULL, it.key());
    m_registeredIds.clear();
    m_nextId = 1;

#elif defined(Q_OS_MACOS)
    for (auto it = m_macRefs.constBegin(); it != m_macRefs.constEnd(); ++it) {
        EventHotKeyRef ref = static_cast<EventHotKeyRef>(it.value());
        UnregisterEventHotKey(ref);
    }
    m_macRegisteredIds.clear();
    m_macRefs.clear();
    m_macNextId   = 1;

#else
    if (m_isWayland) {
        m_x11Hotkeys.clear();
        return;
    }
    Display *display = x11Display();
    if (!display)
        return;
    Window root = DefaultRootWindow(display);
    for (auto it = m_x11Hotkeys.constBegin(); it != m_x11Hotkeys.constEnd(); ++it) {
        for (unsigned int spec : specialModifiers)
            XUngrabKey(display, it.value().keycode, it.value().mods | spec, root);
    }
    XFlush(display);
    m_x11Hotkeys.clear();
#endif
}

QString GlobalHotkeyService::registerOne(ActionId actionId, const QKeySequence &seq)
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    auto combo = seq[0];
    int  code  = combo.toCombined();
#else
    int     code  = seq[0];
#endif
    Qt::Key               key  = static_cast<Qt::Key>(code & ~Qt::KeyboardModifierMask);
    Qt::KeyboardModifiers mods = static_cast<Qt::KeyboardModifiers>(code & Qt::KeyboardModifierMask);

#ifdef Q_OS_WIN
    UINT vk = toNativeKey(key);
    if (!vk)
        return QStringLiteral("This key cannot be mapped to a Windows virtual key.");

    UINT nativeMods = toNativeModifiers(mods);
    int  id         = m_nextId++;
    if (RegisterHotKey(NULL, id, nativeMods, vk)) {
        m_registeredIds.insert(id, actionId);
        return {};
    }

    DWORD err = GetLastError();
    if (err == ERROR_HOTKEY_ALREADY_REGISTERED)
        return QStringLiteral("This shortcut is already in use by another application.");
    return QStringLiteral("Failed to register global shortcut (error %1).").arg(err);

#elif defined(Q_OS_MACOS)
    bool    ok    = false;
    quint32 kcode = macKeycode(key, ok);
    if (!ok || kcode == 0)
        return QStringLiteral("This key cannot be mapped to a macOS keycode.");

    quint32 modsNative = macModifiers(mods, ok);
    if (!ok)
        return QStringLiteral("This key cannot be mapped to a macOS modifier.");

    int           uniqueId = m_macNextId++;
    EventHotKeyID hkeyID;
    hkeyID.signature = uniqueId;
    hkeyID.id        = 0;

    EventHotKeyRef hotKeyRef = nullptr;
    OSStatus       status = RegisterEventHotKey(kcode, modsNative, hkeyID, GetApplicationEventTarget(), 0, &hotKeyRef);
    if (status != noErr) {
        if (status == eventHotKeyExistsErr)
            return QStringLiteral("This shortcut is already in use by another application.");
        return QStringLiteral("Failed to register global shortcut (error %1).").arg(status);
    }

    m_macRegisteredIds.insert(uniqueId, actionId);
    m_macRefs.insert(uniqueId, reinterpret_cast<void *>(hotKeyRef));
    return {};

#else
    if (m_isWayland) {
        return {};
    }
    Display *display = x11Display();
    if (!display) {
        return QStringLiteral("X11 display not available.");
    }

    KeySym sym = qtKeyToKeySym(key);
    if (sym == NoSymbol) {
        return QStringLiteral("This key cannot be mapped to an X11 keysym.");
    }

    KeyCode keycode = XKeysymToKeycode(display, sym);
    if (!keycode) {
        return QStringLiteral("No X11 keycode found for this key.");
    }

    unsigned int xmods = qtModsToXMods(mods);
    Window       root  = DefaultRootWindow(display);

    for (unsigned int spec : specialModifiers)
        XGrabKey(display, keycode, xmods | spec, root, True, GrabModeAsync, GrabModeAsync);
    XSync(display, False);

    m_x11Hotkeys.insert(actionId, {keycode, xmods});
    return {};
#endif
}

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
bool GlobalHotkeyService::nativeEventFilter(const QByteArray &eventType, void *message, qintptr *result)
#else
bool GlobalHotkeyService::nativeEventFilter(const QByteArray &eventType, void *message, long *result)
#endif
{
    Q_UNUSED(result);
#ifdef Q_OS_WIN
    if (eventType == "windows_generic_MSG" || eventType == "windows_dispatcher_MSG") {
        auto *msg = static_cast<MSG *>(message);
        if (msg->message == WM_HOTKEY) {
            int  id = static_cast<int>(msg->wParam);
            auto it = m_registeredIds.constFind(id);
            if (it != m_registeredIds.constEnd()) {
                emit triggered(it.value());
                return true;
            }
        }
    }
    return false;

#elif defined(Q_OS_MACOS)
    Q_UNUSED(eventType);
    Q_UNUSED(message);
    return false;

#else
    if (m_isWayland)
        return false;
    if (eventType == "xcb_generic_event_t") {
        auto *event = static_cast<xcb_generic_event_t *>(message);
        if ((event->response_type & ~0x80) == XCB_KEY_PRESS) {
            auto *kp = reinterpret_cast<xcb_key_press_event_t *>(event);

            for (auto it = m_x11Hotkeys.constBegin(); it != m_x11Hotkeys.constEnd(); ++it) {
                auto match = it.value().mods == (kp->state & validModsMask);
                if (it.value().keycode == kp->detail && match) {
                    emit triggered(it.key());
                    return true;
                }
            }
        }
    }
    return false;
#endif
}
