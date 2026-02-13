// Copyright (c) 2026 Voltaire Toledo
// Licensed under the MIT License.
// Part of the Mello-Refactor project
// https://github.com/voltaire-toledo/Mello-Refactor

#pragma once

#include "KeyboardHook.h"
#include "HotkeyConfig.h"
#include <unordered_map>
#include <mutex>
#include <memory>
#include <vector>

namespace MelloRefactor::Core {
    // High-level hotkey management system
    // 
    // This class provides a convenient interface for managing multiple hotkeys
    // with conflict detection and lifecycle management. It wraps the lower-level
    // KeyboardHook class and provides thread-safe registration.
    //
    // Features:
    // - Automatic conflict detection (prevents duplicate hotkey registrations)
    // - Thread-safe hotkey registration/unregistration
    // - Lifecycle management (Start/Stop)
    // - Action-based interface (register actions instead of raw callbacks)
    //
    // Usage:
    //   HotkeyManager manager;
    //   manager.Start();
    //   
    //   HotkeyManager::HotkeyAction action;
    //   action.vkCode = VK_F1;
    //   action.modifiers = MOD_WIN;
    //   action.action = []() { MessageBox(NULL, L"F1 pressed", L"Info", MB_OK); };
    //   action.description = L"Show help";
    //   
    //   manager.RegisterHotkey(action);
    class HotkeyManager {
    public:
        // Structure for registering a hotkey with its associated action
        struct HotkeyAction {
            DWORD vkCode;                     // Virtual key code
            DWORD modifiers;                  // Modifier flags
            std::function<void()> action;     // Action to execute
            std::wstring description;         // Human-readable description

            HotkeyAction()
                : vkCode(0)
                , modifiers(0)
                , action(nullptr)
                , description(L"")
            {
            }

            HotkeyAction(DWORD vk, DWORD mods, std::function<void()> act, const std::wstring& desc = L"")
                : vkCode(vk)
                , modifiers(mods)
                , action(std::move(act))
                , description(desc)
            {
            }
        };

        HotkeyManager();
        ~HotkeyManager();

        // Prevent copying (manager contains unique hook instance)
        HotkeyManager(const HotkeyManager&) = delete;
        HotkeyManager& operator=(const HotkeyManager&) = delete;

        // Start the hotkey manager (installs the keyboard hook)
        // Must be called from a thread with a message loop
        // Returns true on success, false if hook installation failed
        bool Start();

        // Stop the hotkey manager (uninstalls the keyboard hook)
        // Automatically called by destructor
        void Stop();

        // Check if the manager is currently running
        bool IsRunning() const;

        // Register a hotkey action
        // Returns false if:
        // - The hotkey combination is already registered (conflict)
        // - The action is null
        // - The manager is not running
        bool RegisterHotkey(const HotkeyAction& hotkeyAction);

        // Unregister a previously registered hotkey
        // Returns false if the hotkey was not found
        bool UnregisterHotkey(DWORD vkCode, DWORD modifiers);

        // Clear all registered hotkeys
        void ClearAllHotkeys();

        // Check if a hotkey combination would conflict with existing registrations
        // Returns true if the combination is already registered
        bool IsHotkeyConflict(DWORD vkCode, DWORD modifiers) const;

        // Get all registered hotkeys
        // Returns a copy of the registered actions (thread-safe)
        std::vector<HotkeyAction> GetRegisteredHotkeys() const;

        // Get count of registered hotkeys
        size_t GetHotkeyCount() const;

    private:
        // Generate unique key for hotkey identification
        uint64_t MakeHotkeyKey(DWORD vkCode, DWORD modifiers) const;

        // Keyboard hook instance
        std::unique_ptr<KeyboardHook> m_hook;

        // Map of registered hotkey actions
        // Key: Combined vkCode and modifiers
        // Value: Action to execute
        std::unordered_map<uint64_t, HotkeyAction> m_registeredHotkeys;

        // Mutex for thread-safe access to m_registeredHotkeys
        mutable std::mutex m_mutex;

        // Running state flag
        bool m_isRunning;
    };
}
