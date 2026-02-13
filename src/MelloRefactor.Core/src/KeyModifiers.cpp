// Copyright (c) 2026 Voltaire Toledo
// Licensed under the MIT License.
// Part of the Mello-Refactor project
// https://github.com/voltaire-toledo/Mello-Refactor

#include "pch.h"
#include "../include/KeyModifiers.h"
#include <unordered_map>

namespace MelloRefactor::Core {
    // Convert modifier flags to human-readable string
    std::wstring ModifiersToString(DWORD modifiers) {
        std::wstring result;

        if (modifiers & MOD_WIN) {
            if (!result.empty()) result += L"+";
            result += L"Win";
        }
        if (modifiers & MOD_CTRL) {
            if (!result.empty()) result += L"+";
            result += L"Ctrl";
        }
        if (modifiers & MOD_SHIFT) {
            if (!result.empty()) result += L"+";
            result += L"Shift";
        }
        if (modifiers & MOD_ALT) {
            if (!result.empty()) result += L"+";
            result += L"Alt";
        }

        return result;
    }

    // Convert virtual key code to human-readable string
    std::wstring VirtualKeyToString(DWORD vkCode) {
        // Common key names map
        static const std::unordered_map<DWORD, std::wstring> keyNames = {
            {VK_BACK, L"Backspace"},
            {VK_TAB, L"Tab"},
            {VK_RETURN, L"Enter"},
            {VK_PAUSE, L"Pause"},
            {VK_CAPITAL, L"Caps Lock"},
            {VK_ESCAPE, L"Esc"},
            {VK_SPACE, L"Space"},
            {VK_PRIOR, L"Page Up"},
            {VK_NEXT, L"Page Down"},
            {VK_END, L"End"},
            {VK_HOME, L"Home"},
            {VK_LEFT, L"Left"},
            {VK_UP, L"Up"},
            {VK_RIGHT, L"Right"},
            {VK_DOWN, L"Down"},
            {VK_SNAPSHOT, L"Print Screen"},
            {VK_INSERT, L"Insert"},
            {VK_DELETE, L"Delete"},
            {VK_NUMPAD0, L"Numpad 0"},
            {VK_NUMPAD1, L"Numpad 1"},
            {VK_NUMPAD2, L"Numpad 2"},
            {VK_NUMPAD3, L"Numpad 3"},
            {VK_NUMPAD4, L"Numpad 4"},
            {VK_NUMPAD5, L"Numpad 5"},
            {VK_NUMPAD6, L"Numpad 6"},
            {VK_NUMPAD7, L"Numpad 7"},
            {VK_NUMPAD8, L"Numpad 8"},
            {VK_NUMPAD9, L"Numpad 9"},
            {VK_MULTIPLY, L"Numpad *"},
            {VK_ADD, L"Numpad +"},
            {VK_SUBTRACT, L"Numpad -"},
            {VK_DECIMAL, L"Numpad ."},
            {VK_DIVIDE, L"Numpad /"},
            {VK_F1, L"F1"}, {VK_F2, L"F2"}, {VK_F3, L"F3"}, {VK_F4, L"F4"},
            {VK_F5, L"F5"}, {VK_F6, L"F6"}, {VK_F7, L"F7"}, {VK_F8, L"F8"},
            {VK_F9, L"F9"}, {VK_F10, L"F10"}, {VK_F11, L"F11"}, {VK_F12, L"F12"},
            {VK_NUMLOCK, L"Num Lock"},
            {VK_SCROLL, L"Scroll Lock"},
        };

        // Check if we have a special name for this key
        auto it = keyNames.find(vkCode);
        if (it != keyNames.end()) {
            return it->second;
        }

        // For alphanumeric keys (A-Z, 0-9), convert directly
        if ((vkCode >= 'A' && vkCode <= 'Z') || (vkCode >= '0' && vkCode <= '9')) {
            return std::wstring(1, static_cast<wchar_t>(vkCode));
        }

        // For other keys, show the VK code
        return L"VK_" + std::to_wstring(vkCode);
    }
}
