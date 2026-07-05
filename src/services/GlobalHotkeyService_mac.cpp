#include "GlobalHotkeyService.h"

#include <Carbon/Carbon.h>

OSStatus macHotkeyHandler(EventHandlerCallRef nextHandler, EventRef event, void *data)
{
    Q_UNUSED(nextHandler);
    Q_UNUSED(data);

    if (GetEventClass(event) == kEventClassKeyboard && GetEventKind(event) == kEventHotKeyPressed) {
        EventHotKeyID hkeyID;
        GetEventParameter(
            event, kEventParamDirectObject, typeEventHotKeyID, NULL, sizeof(EventHotKeyID), NULL, &hkeyID);
        if (GlobalHotkeyService::s_macInstance) {
            QString actionId = GlobalHotkeyService::s_macInstance->m_macRegisteredIds.value(hkeyID.signature);
            if (!actionId.isEmpty())
                emit GlobalHotkeyService::s_macInstance->triggered(actionId);
        }
    }
    return noErr;
}

quint32 macKeycode(Qt::Key key, bool &ok)
{
    ok = true;
    switch (key) {
    case Qt::Key_Return:
        return kVK_Return;
    case Qt::Key_Enter:
        return kVK_ANSI_KeypadEnter;
    case Qt::Key_Tab:
        return kVK_Tab;
    case Qt::Key_Space:
        return kVK_Space;
    case Qt::Key_Backspace:
        return kVK_Delete;
    case Qt::Key_Escape:
        return kVK_Escape;
    case Qt::Key_CapsLock:
        return kVK_CapsLock;
    case Qt::Key_Option:
        return kVK_Option;
    case Qt::Key_Help:
        return kVK_Help;
    case Qt::Key_Home:
        return kVK_Home;
    case Qt::Key_PageUp:
        return kVK_PageUp;
    case Qt::Key_Delete:
        return kVK_ForwardDelete;
    case Qt::Key_End:
        return kVK_End;
    case Qt::Key_PageDown:
        return kVK_PageDown;
    case Qt::Key_Left:
        return kVK_LeftArrow;
    case Qt::Key_Right:
        return kVK_RightArrow;
    case Qt::Key_Down:
        return kVK_DownArrow;
    case Qt::Key_Up:
        return kVK_UpArrow;

    case Qt::Key_F1:
        return kVK_F1;
    case Qt::Key_F2:
        return kVK_F2;
    case Qt::Key_F3:
        return kVK_F3;
    case Qt::Key_F4:
        return kVK_F4;
    case Qt::Key_F5:
        return kVK_F5;
    case Qt::Key_F6:
        return kVK_F6;
    case Qt::Key_F7:
        return kVK_F7;
    case Qt::Key_F8:
        return kVK_F8;
    case Qt::Key_F9:
        return kVK_F9;
    case Qt::Key_F10:
        return kVK_F10;
    case Qt::Key_F11:
        return kVK_F11;
    case Qt::Key_F12:
        return kVK_F12;
    case Qt::Key_F13:
        return kVK_F13;
    case Qt::Key_F14:
        return kVK_F14;
    case Qt::Key_F15:
        return kVK_F15;
    case Qt::Key_F16:
        return kVK_F16;
    case Qt::Key_F17:
        return kVK_F17;
    case Qt::Key_F18:
        return kVK_F18;
    case Qt::Key_F19:
        return kVK_F19;
    case Qt::Key_F20:
        return kVK_F20;

    default:
        ok = false;
        break;
    }

    if (ok)
        return 0;

    UTF16Char ch = static_cast<UTF16Char>(key);
    if (key >= Qt::Key_A && key <= Qt::Key_Z)
        ch = static_cast<UTF16Char>('a' + (key - Qt::Key_A));
    else if (key >= Qt::Key_0 && key <= Qt::Key_9)
        ch = static_cast<UTF16Char>('0' + (key - Qt::Key_0));

    TISInputSourceRef currentKeyboard = TISCopyCurrentASCIICapableKeyboardLayoutInputSource();
    if (!currentKeyboard)
        return 0;

    CFDataRef currentLayoutData
        = (CFDataRef) TISGetInputSourceProperty(currentKeyboard, kTISPropertyUnicodeKeyLayoutData);
    CFRelease(currentKeyboard);
    if (!currentLayoutData)
        return 0;

    UCKeyboardLayout     *header = (UCKeyboardLayout *) CFDataGetBytePtr(currentLayoutData);
    UCKeyboardTypeHeader *table  = header->keyboardTypeList;
    uint8_t              *data   = (uint8_t *) header;

    for (quint32 i = 0; i < header->keyboardTypeCount; i++) {
        UCKeyStateRecordsIndex *stateRec = nullptr;
        if (table[i].keyStateRecordsIndexOffset != 0) {
            stateRec = reinterpret_cast<UCKeyStateRecordsIndex *>(data + table[i].keyStateRecordsIndexOffset);
            if (stateRec->keyStateRecordsIndexFormat != kUCKeyStateRecordsIndexFormat)
                stateRec = nullptr;
        }

        UCKeyToCharTableIndex *charTable
            = reinterpret_cast<UCKeyToCharTableIndex *>(data + table[i].keyToCharTableIndexOffset);
        if (charTable->keyToCharTableIndexFormat != kUCKeyToCharTableIndexFormat)
            continue;

        for (quint32 j = 0; j < charTable->keyToCharTableCount; j++) {
            UCKeyOutput *keyToChar = reinterpret_cast<UCKeyOutput *>(data + charTable->keyToCharTableOffsets[j]);
            for (quint32 k = 0; k < charTable->keyToCharTableSize; k++) {
                if (keyToChar[k] & kUCKeyOutputTestForIndexMask) {
                    long idx = keyToChar[k] & kUCKeyOutputGetIndexMask;
                    if (stateRec && idx < stateRec->keyStateRecordCount) {
                        UCKeyStateRecord *rec
                            = reinterpret_cast<UCKeyStateRecord *>(data + stateRec->keyStateRecordOffsets[idx]);
                        if (rec->stateZeroCharData == ch) {
                            ok = true;
                            return k;
                        }
                    }
                }
                else if (!(keyToChar[k] & kUCKeyOutputSequenceIndexMask) && keyToChar[k] < 0xFFFE) {
                    if (keyToChar[k] == ch) {
                        ok = true;
                        return k;
                    }
                }
            }
        }
    }
    return 0;
}

quint32 macModifiers(Qt::KeyboardModifiers mods, bool &ok)
{
    quint32 result = 0;
    if (mods & Qt::ShiftModifier)
        result |= shiftKey;
    if (mods & Qt::ControlModifier)
        result |= cmdKey;
    if (mods & Qt::AltModifier)
        result |= optionKey;
    if (mods & Qt::MetaModifier)
        result |= controlKey;
    if (mods & Qt::KeypadModifier)
        result |= kEventKeyModifierNumLockMask;
    ok = true;
    return result;
}
