// Copyright (c) 2026 Voltaire Toledo
// Licensed under the MIT License.
// Part of the Mello-Refactor project
// https://github.com/voltaire-toledo/Mello-Refactor

#include "pch.h"
#include "../include/KeyboardHook.h"

namespace MelloRefactor::Core {
    // Static instance pointer initialization
    KeyboardHook* KeyboardHook::s_instance = nullptr;

    KeyboardHook::KeyboardHook()
        : m_hookHandle(nullptr)
        , m_winKeyDown(false)
        , m_ctrlKeyDown(false)
        , m_shiftKeyDown(false)
        , m_altKeyDown(false)
    {
        // Only one instance allowed due to static callback requirement
        if (s_instance != nullptr) {
            throw std::runtime_error("Only one KeyboardHook instance is allowed");
        }
        s_instance = this;
    }

    KeyboardHook::~KeyboardHook() {
        Uninstall();
        s_instance = nullptr;
    }

    bool KeyboardHook::Install() {
        if (m_hookHandle != nullptr) {
            return true; // Already installed
        }

        // Install low-level keyboard hook
        // WH_KEYBOARD_LL: Installs a hook procedure that monitors low-level keyboard input events
        // This hook is called before the system sends the input to the target window
        // The hook procedure must be in a DLL or in the same process as the window that receives the input
        // NULL: Hook procedure is associated with all existing threads in the same desktop as the calling thread
        m_hookHandle = SetWindowsHookEx(
            WH_KEYBOARD_LL,
            LowLevelKeyboardProc,
            nullptr, // hMod - NULL for hooks that are not in a DLL
            0        // dwThreadId - 0 for all threads in the same desktop
        );

        if (m_hookHandle == nullptr) {
            // Hook installation failed
            DWORD error = GetLastError();
            // In a production app, log this error: error code in 'error'
            return false;
        }

        return true;
    }

    bool KeyboardHook::Uninstall() {
        if (m_hookHandle == nullptr) {
            return true; // Already uninstalled
        }

        // Remove the hook
        BOOL result = UnhookWindowsHookEx(m_hookHandle);
        m_hookHandle = nullptr;

        // Reset modifier state
        m_winKeyDown = false;
        m_ctrlKeyDown = false;
        m_shiftKeyDown = false;
        m_altKeyDown = false;

        return result != FALSE;
    }

    bool KeyboardHook::IsInstalled() const {
        return m_hookHandle != nullptr;
    }

    void KeyboardHook::RegisterHotkey(DWORD vkCode, DWORD modifiers, HotkeyCallback callback) {
        if (!callback) {
            return; // Don't register null callbacks
        }

        uint64_t key = MakeHotkeyKey(vkCode, modifiers);
        m_hotkeyCallbacks[key] = std::move(callback);
    }

    void KeyboardHook::UnregisterHotkey(DWORD vkCode, DWORD modifiers) {
        uint64_t key = MakeHotkeyKey(vkCode, modifiers);
        m_hotkeyCallbacks.erase(key);
    }

    void KeyboardHook::ClearAllHotkeys() {
        m_hotkeyCallbacks.clear();
    }

    // Static callback function for Windows hook
    LRESULT CALLBACK KeyboardHook::LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
        if (s_instance != nullptr) {
            return s_instance->HandleKeyboardMessage(nCode, wParam, lParam);
        }
        return CallNextHookEx(nullptr, nCode, wParam, lParam);
    }

    // Instance method that handles keyboard events
    LRESULT KeyboardHook::HandleKeyboardMessage(int nCode, WPARAM wParam, LPARAM lParam) {
        // nCode < 0: must call CallNextHookEx without further processing
        // HC_ACTION: wParam and lParam contain information about a keyboard message
        if (nCode == HC_ACTION) {
            // lParam points to a KBDLLHOOKSTRUCT structure
            KBDLLHOOKSTRUCT* pKeyboard = reinterpret_cast<KBDLLHOOKSTRUCT*>(lParam);

            // wParam: keyboard message (WM_KEYDOWN, WM_KEYUP, WM_SYSKEYDOWN, WM_SYSKEYUP)
            bool isKeyDown = (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN);
            DWORD vkCode = pKeyboard->vkCode;

            // Update modifier key state
            UpdateModifierState(vkCode, isKeyDown);

            // Check for hotkey match on key down events
            if (isKeyDown && !IsModifierKey(vkCode)) {
                DWORD currentModifiers = GetCurrentModifiers();
                uint64_t key = MakeHotkeyKey(vkCode, currentModifiers);

                auto it = m_hotkeyCallbacks.find(key);
                if (it != m_hotkeyCallbacks.end() && it->second) {
                    // Execute the callback
                    try {
                        it->second(vkCode, currentModifiers);
                    }
                    catch (...) {
                        // Prevent exceptions from propagating to Windows hook system
                        // In production, log this exception
                    }
                }
            }
        }

        // Call the next hook in the chain
        // This is important to allow other applications to process keyboard events
        return CallNextHookEx(m_hookHandle, nCode, wParam, lParam);
    }

    void KeyboardHook::UpdateModifierState(DWORD vkCode, bool isKeyDown) {
        // Track modifier key states
        // Note: We check for both left and right variants of modifier keys
        switch (vkCode) {
        case VK_LWIN:
        case VK_RWIN:
            m_winKeyDown = isKeyDown;
            break;
        case VK_LCONTROL:
        case VK_RCONTROL:
        case VK_CONTROL:
            m_ctrlKeyDown = isKeyDown;
            break;
        case VK_LSHIFT:
        case VK_RSHIFT:
        case VK_SHIFT:
            m_shiftKeyDown = isKeyDown;
            break;
        case VK_LMENU:
        case VK_RMENU:
        case VK_MENU:
            m_altKeyDown = isKeyDown;
            break;
        }
    }

    bool KeyboardHook::IsModifierKey(DWORD vkCode) const {
        // Check if the key is a modifier key
        return vkCode == VK_LWIN || vkCode == VK_RWIN ||
            vkCode == VK_LCONTROL || vkCode == VK_RCONTROL || vkCode == VK_CONTROL ||
            vkCode == VK_LSHIFT || vkCode == VK_RSHIFT || vkCode == VK_SHIFT ||
            vkCode == VK_LMENU || vkCode == VK_RMENU || vkCode == VK_MENU;
    }

    DWORD KeyboardHook::GetCurrentModifiers() const {
        DWORD modifiers = 0;
        if (m_winKeyDown) modifiers |= MOD_WIN;
        if (m_ctrlKeyDown) modifiers |= MOD_CTRL;
        if (m_shiftKeyDown) modifiers |= MOD_SHIFT;
        if (m_altKeyDown) modifiers |= MOD_ALT;
        return modifiers;
    }

    uint64_t KeyboardHook::MakeHotkeyKey(DWORD vkCode, DWORD modifiers) const {
        // Combine modifiers (high 32 bits) and vkCode (low 32 bits) into a single uint64_t
        return (static_cast<uint64_t>(modifiers) << 32) | vkCode;
    }
}
