// Copyright (c) 2026 Voltaire Toledo
// Licensed under the MIT License.
// Part of the Mello-Refactor project
// https://github.com/voltaire-toledo/Mello-Refactor

#pragma once

#include <Windows.h>
#include <functional>
#include <unordered_map>
#include <memory>

namespace MelloRefactor::Core {
    // Low-level keyboard hook implementation
    // 
    // This class provides a Windows low-level keyboard hook (WH_KEYBOARD_LL) that intercepts
    // keyboard events system-wide. It tracks modifier key states and triggers callbacks
    // when registered hotkey combinations are pressed.
    //
    // Why WH_KEYBOARD_LL instead of WH_KEYBOARD:
    // - WH_KEYBOARD_LL works system-wide across all applications
    // - Receives events before they are processed by the target window
    // - Can prevent default behavior by not calling CallNextHookEx
    // - More suitable for global hotkey detection
    // - WH_KEYBOARD only hooks into the current thread's message queue
    //
    // Thread Safety:
    // - The hook callback runs on the thread that installed it
    // - Must call SetWindowsHookEx from a thread with a message loop
    // - Callback registration is not thread-safe by default (caller must synchronize)
    //
    // RAII Pattern:
    // - Constructor initializes state
    // - Destructor automatically uninstalls hook to prevent resource leaks
    // - No manual cleanup needed when object goes out of scope
    class KeyboardHook {
    public:
        KeyboardHook();
        ~KeyboardHook();

        // Prevent copying (hooks are not copyable due to global state)
        KeyboardHook(const KeyboardHook&) = delete;
        KeyboardHook& operator=(const KeyboardHook&) = delete;

        // Install the low-level keyboard hook
        // Must be called from a thread with a message loop (e.g., UI thread)
        // Returns true on success, false if hook installation failed
        bool Install();

        // Uninstall the keyboard hook
        // Automatically called by destructor
        // Returns true on success, false if unhook failed
        bool Uninstall();

        // Check if the hook is currently installed
        bool IsInstalled() const;

        // Callback function type for hotkey events
        // Parameters:
        //   vkCode: Virtual key code of the pressed key
        //   modifiers: Current modifier state (MOD_WIN, MOD_CTRL, etc.)
        using HotkeyCallback = std::function<void(DWORD vkCode, DWORD modifiers)>;

        // Register a callback for a specific hotkey combination
        // The callback will be invoked when the hotkey is pressed
        // Multiple callbacks can be registered for different hotkeys
        void RegisterHotkey(DWORD vkCode, DWORD modifiers, HotkeyCallback callback);

        // Unregister a previously registered hotkey
        // Does nothing if the hotkey was not registered
        void UnregisterHotkey(DWORD vkCode, DWORD modifiers);

        // Clear all registered hotkeys
        void ClearAllHotkeys();

    private:
        // Static callback required by SetWindowsHookEx
        // Windows API requires a static function, so we use s_instance to access the object
        static LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);

        // Instance method that handles the actual keyboard event processing
        LRESULT HandleKeyboardMessage(int nCode, WPARAM wParam, LPARAM lParam);

        // Update the current modifier key state based on keyboard events
        void UpdateModifierState(DWORD vkCode, bool isKeyDown);

        // Check if a key is a modifier key
        bool IsModifierKey(DWORD vkCode) const;

        // Convert modifier key state to MOD_* flags
        DWORD GetCurrentModifiers() const;

        // Generate unique key for hotkey map
        uint64_t MakeHotkeyKey(DWORD vkCode, DWORD modifiers) const;

        // Hook handle from SetWindowsHookEx
        // Must be unhooked with UnhookWindowsHookEx
        HHOOK m_hookHandle;

        // Map of registered hotkey callbacks
        // Key: Combined vkCode and modifiers (uint64_t)
        // Value: Callback function to invoke
        std::unordered_map<uint64_t, HotkeyCallback> m_hotkeyCallbacks;

        // Current state of modifier keys
        // Tracked separately because modifier keys don't always come as combinations
        bool m_winKeyDown;
        bool m_ctrlKeyDown;
        bool m_shiftKeyDown;
        bool m_altKeyDown;

        // Static instance pointer for the callback
        // Only one instance of KeyboardHook should exist at a time
        // This is a limitation of the Windows hook architecture
        static KeyboardHook* s_instance;
    };
}
