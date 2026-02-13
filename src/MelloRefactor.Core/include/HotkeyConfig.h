// Copyright (c) 2026 Voltaire Toledo
// Licensed under the MIT License.
// Part of the Mello-Refactor project
// https://github.com/voltaire-toledo/Mello-Refactor

#pragma once

#include <Windows.h>
#include <string>
#include <functional>

namespace MelloRefactor::Core {
    // Configuration structure for a single hotkey
    // This represents a hotkey binding with its associated action
    struct HotkeyConfig {
        DWORD vkCode;                         // Virtual key code (e.g., VK_A, VK_F1)
        DWORD modifiers;                      // Modifier flags (MOD_WIN, MOD_CTRL, etc.)
        std::function<void()> action;         // Action to execute when hotkey is pressed
        std::wstring description;             // Human-readable description of the action
        bool enabled;                         // Whether this hotkey is currently active

        HotkeyConfig()
            : vkCode(0)
            , modifiers(0)
            , action(nullptr)
            , description(L"")
            , enabled(true)
        {
        }

        HotkeyConfig(DWORD vk, DWORD mods, std::function<void()> act, const std::wstring& desc)
            : vkCode(vk)
            , modifiers(mods)
            , action(std::move(act))
            , description(desc)
            , enabled(true)
        {
        }

        // Generate unique identifier for this hotkey combination
        // Used for conflict detection and map storage
        uint64_t GetUniqueId() const {
            return (static_cast<uint64_t>(modifiers) << 32) | vkCode;
        }
    };
}
