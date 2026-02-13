// Copyright (c) 2026 Voltaire Toledo
// Licensed under the MIT License.
// Part of the Mello-Refactor project
// https://github.com/voltaire-toledo/Mello-Refactor

#pragma once

#include <Windows.h>
#include <string>
#include <initializer_list>

namespace MelloRefactor::Core {
    // Modifier flags (combine with bitwise OR)
    // These flags match the Windows RegisterHotKey modifier conventions
    constexpr DWORD MOD_NONE = 0x0000;
    constexpr DWORD MOD_WIN = 0x0001;   // Windows key
    constexpr DWORD MOD_CTRL = 0x0002;  // Control key
    constexpr DWORD MOD_SHIFT = 0x0004; // Shift key
    constexpr DWORD MOD_ALT = 0x0008;   // Alt key

    // Helper function to combine multiple modifiers
    // Usage: CombineModifiers({MOD_WIN, MOD_CTRL})
    inline constexpr DWORD CombineModifiers(std::initializer_list<DWORD> mods) {
        DWORD result = MOD_NONE;
        for (DWORD mod : mods) {
            result |= mod;
        }
        return result;
    }

    // Convert modifier flags to human-readable string (e.g., "Win+Ctrl")
    std::wstring ModifiersToString(DWORD modifiers);

    // Convert virtual key code to human-readable string (e.g., "A", "F1", "Enter")
    std::wstring VirtualKeyToString(DWORD vkCode);

    // Combine modifiers and key into hotkey string (e.g., "Win+Ctrl+A")
    inline std::wstring HotkeyToString(DWORD modifiers, DWORD vkCode) {
        std::wstring result = ModifiersToString(modifiers);
        if (!result.empty()) {
            result += L"+";
        }
        result += VirtualKeyToString(vkCode);
        return result;
    }
}
