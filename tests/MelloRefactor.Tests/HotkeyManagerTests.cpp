// Copyright (c) 2026 Voltaire Toledo
// Licensed under the MIT License.
// Part of the Mello-Refactor project
// https://github.com/voltaire-toledo/Mello-Refactor

#include "pch.h"
#include "../../src/MelloRefactor.Core/include/HotkeyManager.h"
#include "../../src/MelloRefactor.Core/include/KeyModifiers.h"
#include <thread>
#include <chrono>

using namespace MelloRefactor::Core;

// Test fixture for HotkeyManager tests
class HotkeyManagerTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Setup code if needed
    }

    void TearDown() override {
        // Cleanup code if needed
    }
};

// Test that HotkeyManager can be created and destroyed
TEST_F(HotkeyManagerTest, ConstructorDestructor) {
    HotkeyManager* manager = new HotkeyManager();
    EXPECT_NE(manager, nullptr);
    EXPECT_FALSE(manager->IsRunning());
    delete manager;
}

// Test Start and Stop
TEST_F(HotkeyManagerTest, StartStop) {
    HotkeyManager manager;
    EXPECT_FALSE(manager.IsRunning());

    // Start may fail in non-interactive environments
    bool started = manager.Start();
    if (started) {
        EXPECT_TRUE(manager.IsRunning());

        manager.Stop();
        EXPECT_FALSE(manager.IsRunning());
    }
}

// Test hotkey registration
TEST_F(HotkeyManagerTest, RegisterHotkey) {
    HotkeyManager manager;

    // Start the manager first
    bool started = manager.Start();
    if (!started) {
        GTEST_SKIP() << "Could not start HotkeyManager in this environment";
    }

    bool callbackInvoked = false;
    HotkeyManager::HotkeyAction action(
        VK_F1,
        MOD_WIN,
        [&]() { callbackInvoked = true; },
        L"Test hotkey"
    );

    bool registered = manager.RegisterHotkey(action);
    EXPECT_TRUE(registered);
    EXPECT_EQ(manager.GetHotkeyCount(), 1);

    manager.Stop();
}

// Test hotkey conflict detection
TEST_F(HotkeyManagerTest, ConflictDetection) {
    HotkeyManager manager;

    bool started = manager.Start();
    if (!started) {
        GTEST_SKIP() << "Could not start HotkeyManager in this environment";
    }

    HotkeyManager::HotkeyAction action1(VK_F1, MOD_WIN, []() {}, L"Action 1");
    HotkeyManager::HotkeyAction action2(VK_F1, MOD_WIN, []() {}, L"Action 2");

    // Register first hotkey
    bool registered1 = manager.RegisterHotkey(action1);
    EXPECT_TRUE(registered1);

    // Check for conflict
    EXPECT_TRUE(manager.IsHotkeyConflict(VK_F1, MOD_WIN));

    // Try to register conflicting hotkey
    bool registered2 = manager.RegisterHotkey(action2);
    EXPECT_FALSE(registered2); // Should fail due to conflict

    manager.Stop();
}

// Test unregistering hotkeys
TEST_F(HotkeyManagerTest, UnregisterHotkey) {
    HotkeyManager manager;

    bool started = manager.Start();
    if (!started) {
        GTEST_SKIP() << "Could not start HotkeyManager in this environment";
    }

    HotkeyManager::HotkeyAction action(VK_F1, MOD_WIN, []() {}, L"Test");

    manager.RegisterHotkey(action);
    EXPECT_EQ(manager.GetHotkeyCount(), 1);

    bool unregistered = manager.UnregisterHotkey(VK_F1, MOD_WIN);
    EXPECT_TRUE(unregistered);
    EXPECT_EQ(manager.GetHotkeyCount(), 0);

    manager.Stop();
}

// Test clearing all hotkeys
TEST_F(HotkeyManagerTest, ClearAllHotkeys) {
    HotkeyManager manager;

    bool started = manager.Start();
    if (!started) {
        GTEST_SKIP() << "Could not start HotkeyManager in this environment";
    }

    // Register multiple hotkeys
    manager.RegisterHotkey(HotkeyManager::HotkeyAction(VK_F1, MOD_WIN, []() {}, L"F1"));
    manager.RegisterHotkey(HotkeyManager::HotkeyAction(VK_F2, MOD_CTRL, []() {}, L"F2"));
    manager.RegisterHotkey(HotkeyManager::HotkeyAction(VK_F3, MOD_SHIFT, []() {}, L"F3"));

    EXPECT_EQ(manager.GetHotkeyCount(), 3);

    manager.ClearAllHotkeys();
    EXPECT_EQ(manager.GetHotkeyCount(), 0);

    manager.Stop();
}

// Test multiple hotkey registration
TEST_F(HotkeyManagerTest, MultipleHotkeys) {
    HotkeyManager manager;

    bool started = manager.Start();
    if (!started) {
        GTEST_SKIP() << "Could not start HotkeyManager in this environment";
    }

    // Register multiple different hotkeys
    for (DWORD i = 0; i < 10; ++i) {
        HotkeyManager::HotkeyAction action(
            VK_F1 + i,
            MOD_WIN,
            []() {},
            L"Hotkey " + std::to_wstring(i)
        );
        bool registered = manager.RegisterHotkey(action);
        EXPECT_TRUE(registered);
    }

    EXPECT_EQ(manager.GetHotkeyCount(), 10);

    manager.Stop();
}

// Test GetRegisteredHotkeys
TEST_F(HotkeyManagerTest, GetRegisteredHotkeys) {
    HotkeyManager manager;

    bool started = manager.Start();
    if (!started) {
        GTEST_SKIP() << "Could not start HotkeyManager in this environment";
    }

    manager.RegisterHotkey(HotkeyManager::HotkeyAction(VK_F1, MOD_WIN, []() {}, L"F1"));
    manager.RegisterHotkey(HotkeyManager::HotkeyAction(VK_F2, MOD_CTRL, []() {}, L"F2"));

    auto hotkeys = manager.GetRegisteredHotkeys();
    EXPECT_EQ(hotkeys.size(), 2);

    manager.Stop();
}

// Test thread safety (basic test)
TEST_F(HotkeyManagerTest, ThreadSafety) {
    HotkeyManager manager;

    bool started = manager.Start();
    if (!started) {
        GTEST_SKIP() << "Could not start HotkeyManager in this environment";
    }

    // Register hotkeys from multiple threads
    std::vector<std::thread> threads;
    for (int i = 0; i < 5; ++i) {
        threads.emplace_back([&manager, i]() {
            HotkeyManager::HotkeyAction action(
                VK_F1 + i,
                MOD_WIN,
                []() {},
                L"Thread hotkey"
            );
            manager.RegisterHotkey(action);
            });
    }

    for (auto& thread : threads) {
        thread.join();
    }

    // All hotkeys should be registered
    EXPECT_EQ(manager.GetHotkeyCount(), 5);

    manager.Stop();
}
