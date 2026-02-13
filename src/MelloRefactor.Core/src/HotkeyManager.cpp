// Copyright (c) 2026 Voltaire Toledo
// Licensed under the MIT License.
// Part of the Mello-Refactor project
// https://github.com/voltaire-toledo/Mello-Refactor

#include "pch.h"
#include "../include/HotkeyManager.h"
#include <algorithm>

namespace MelloRefactor::Core {
    HotkeyManager::HotkeyManager()
        : m_hook(nullptr)
        , m_isRunning(false)
    {
    }

    HotkeyManager::~HotkeyManager() {
        Stop();
    }

    bool HotkeyManager::Start() {
        std::lock_guard<std::mutex> lock(m_mutex);

        if (m_isRunning) {
            return true; // Already running
        }

        // Create and install the keyboard hook
        m_hook = std::make_unique<KeyboardHook>();
        if (!m_hook->Install()) {
            m_hook.reset();
            return false;
        }

        m_isRunning = true;
        return true;
    }

    void HotkeyManager::Stop() {
        std::lock_guard<std::mutex> lock(m_mutex);

        if (!m_isRunning) {
            return; // Already stopped
        }

        // Clear all hotkeys and destroy the hook
        m_registeredHotkeys.clear();
        m_hook.reset();
        m_isRunning = false;
    }

    bool HotkeyManager::IsRunning() const {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_isRunning;
    }

    bool HotkeyManager::RegisterHotkey(const HotkeyAction& hotkeyAction) {
        std::lock_guard<std::mutex> lock(m_mutex);

        if (!m_isRunning || !m_hook) {
            return false; // Manager not running
        }

        if (!hotkeyAction.action) {
            return false; // Invalid action
        }

        // Check for conflicts
        uint64_t key = MakeHotkeyKey(hotkeyAction.vkCode, hotkeyAction.modifiers);
        if (m_registeredHotkeys.find(key) != m_registeredHotkeys.end()) {
            return false; // Hotkey already registered
        }

        // Register with the keyboard hook
        // We wrap the action in a lambda that ignores the vkCode and modifiers parameters
        m_hook->RegisterHotkey(
            hotkeyAction.vkCode,
            hotkeyAction.modifiers,
            [action = hotkeyAction.action](DWORD, DWORD) {
                if (action) {
                    action();
                }
            }
        );

        // Store the action
        m_registeredHotkeys[key] = hotkeyAction;
        return true;
    }

    bool HotkeyManager::UnregisterHotkey(DWORD vkCode, DWORD modifiers) {
        std::lock_guard<std::mutex> lock(m_mutex);

        if (!m_isRunning || !m_hook) {
            return false;
        }

        uint64_t key = MakeHotkeyKey(vkCode, modifiers);
        auto it = m_registeredHotkeys.find(key);
        if (it == m_registeredHotkeys.end()) {
            return false; // Hotkey not found
        }

        // Unregister from the keyboard hook
        m_hook->UnregisterHotkey(vkCode, modifiers);

        // Remove from our map
        m_registeredHotkeys.erase(it);
        return true;
    }

    void HotkeyManager::ClearAllHotkeys() {
        std::lock_guard<std::mutex> lock(m_mutex);

        if (m_hook) {
            m_hook->ClearAllHotkeys();
        }
        m_registeredHotkeys.clear();
    }

    bool HotkeyManager::IsHotkeyConflict(DWORD vkCode, DWORD modifiers) const {
        std::lock_guard<std::mutex> lock(m_mutex);

        uint64_t key = MakeHotkeyKey(vkCode, modifiers);
        return m_registeredHotkeys.find(key) != m_registeredHotkeys.end();
    }

    std::vector<HotkeyManager::HotkeyAction> HotkeyManager::GetRegisteredHotkeys() const {
        std::lock_guard<std::mutex> lock(m_mutex);

        std::vector<HotkeyAction> result;
        result.reserve(m_registeredHotkeys.size());

        for (const auto& pair : m_registeredHotkeys) {
            result.push_back(pair.second);
        }

        return result;
    }

    size_t HotkeyManager::GetHotkeyCount() const {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_registeredHotkeys.size();
    }

    uint64_t HotkeyManager::MakeHotkeyKey(DWORD vkCode, DWORD modifiers) const {
        // Combine modifiers (high 32 bits) and vkCode (low 32 bits) into a single uint64_t
        return (static_cast<uint64_t>(modifiers) << 32) | vkCode;
    }
}
