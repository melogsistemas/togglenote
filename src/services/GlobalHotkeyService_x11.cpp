#include "GlobalHotkeyService.h"

#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <X11/XF86keysym.h>
#include <xcb/xcb.h>
#include <xcb/xproto.h>

extern const unsigned int specialModifiers[4] = {0, Mod2Mask, LockMask, Mod2Mask | LockMask};
extern const unsigned int validModsMask       = ShiftMask | ControlMask | Mod1Mask | Mod4Mask;

KeySym qtKeyToKeySym(Qt::Key key)
{
    if (key >= Qt::Key_A && key <= Qt::Key_Z)
        return static_cast<KeySym>(key);
    if (key >= Qt::Key_0 && key <= Qt::Key_9)
        return static_cast<KeySym>(key);
    if (key >= Qt::Key_F1 && key <= Qt::Key_F24)
        return XK_F1 + (key - Qt::Key_F1);

    switch (key) {
    case Qt::Key_Escape:
        return XK_Escape;
    case Qt::Key_Tab:
        return XK_Tab;
    case Qt::Key_Backspace:
        return XK_BackSpace;
    case Qt::Key_Return:
        return XK_Return;
    case Qt::Key_Enter:
        return XK_Return;
    case Qt::Key_Insert:
        return XK_Insert;
    case Qt::Key_Delete:
        return XK_Delete;
    case Qt::Key_Home:
        return XK_Home;
    case Qt::Key_End:
        return XK_End;
    case Qt::Key_PageUp:
        return XK_Page_Up;
    case Qt::Key_PageDown:
        return XK_Page_Down;
    case Qt::Key_Left:
        return XK_Left;
    case Qt::Key_Right:
        return XK_Right;
    case Qt::Key_Up:
        return XK_Up;
    case Qt::Key_Down:
        return XK_Down;
    case Qt::Key_Space:
        return XK_space;

    case Qt::Key_CapsLock:
        return XK_Caps_Lock;
    case Qt::Key_NumLock:
        return XK_Num_Lock;
    case Qt::Key_ScrollLock:
        return XK_Scroll_Lock;
    case Qt::Key_Menu:
        return XK_Menu;
    case Qt::Key_Help:
        return XK_Help;
    case Qt::Key_Print:
        return XK_Print;
    case Qt::Key_Pause:
        return XK_Pause;

    case Qt::Key_MediaNext:
        return XF86XK_AudioNext;
    case Qt::Key_MediaPrevious:
        return XF86XK_AudioPrev;
    case Qt::Key_MediaPlay:
        return XF86XK_AudioPlay;
    case Qt::Key_MediaStop:
        return XF86XK_AudioStop;
    case Qt::Key_VolumeDown:
        return XF86XK_AudioLowerVolume;
    case Qt::Key_VolumeUp:
        return XF86XK_AudioRaiseVolume;
    case Qt::Key_VolumeMute:
        return XF86XK_AudioMute;

    default:
        if (key <= 0xFFFF)
            return static_cast<KeySym>(key);
        return NoSymbol;
    }
}

unsigned int qtModsToXMods(Qt::KeyboardModifiers mods)
{
    unsigned int result = 0;
    if (mods & Qt::ShiftModifier)
        result |= ShiftMask;
    if (mods & Qt::ControlModifier)
        result |= ControlMask;
    if (mods & Qt::AltModifier)
        result |= Mod1Mask;
    if (mods & Qt::MetaModifier)
        result |= Mod4Mask;
    return result;
}
