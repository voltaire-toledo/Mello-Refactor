// Copyright (c) 2026 Voltaire Toledo
// Licensed under the MIT License.
// Part of the Mello-Refactor project
// https://github.com/voltaire-toledo/Mello-Refactor

#include "pch.h"
#include "../../src/MelloRefactor.Core/include/KeyboardHook.h"
#include "../../src/MelloRefactor.Core/include/KeyModifiers.h"

using namespace MelloRefactor::Core;

// Test fixture for KeyboardHook tests
class KeyboardHookTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Setup code if needed
    }

    void TearDown() override {
        // Cleanup code if needed
    }
};

// Test that a KeyboardHook can be created and destroyed
TEST_F(KeyboardHookTest, ConstructorDestructor) {
    KeyboardHook* hook = new KeyboardHook();
    EXPECT_NE(hook, nullptr);
    EXPECT_FALSE(hook->IsInstalled());
    delete hook;
}

// Test that only one instance can exist at a time
TEST_F(KeyboardHookTest, SingleInstanceOnly) {
    KeyboardHook hook1;
    EXPECT_THROW({
        KeyboardHook hook2;
        }, std::runtime_error);
}

// Test hotkey registration
TEST_F(KeyboardHookTest, RegisterHotkey) {
    KeyboardHook hook;
    bool callbackInvoked = false;

    hook.RegisterHotkey(VK_F1, MOD_WIN, [&](DWORD vk, DWORD mods) {
        callbackInvoked = true;
        EXPECT_EQ(vk, VK_F1);
        EXPECT_EQ(mods, MOD_WIN);
        });

    // Note: We can't actually trigger the hotkey in a unit test without simulating keyboard input
    // This test just verifies registration doesn't crash
    EXPECT_FALSE(callbackInvoked); // Callback won't be invoked in this test
}

// Test hotkey unregistration
TEST_F(KeyboardHookTest, UnregisterHotkey) {
    KeyboardHook hook;
    bool callbackInvoked = false;

    hook.RegisterHotkey(VK_F1, MOD_WIN, [&](DWORD, DWORD) {
        callbackInvoked = true;
        });

    hook.UnregisterHotkey(VK_F1, MOD_WIN);

    // Callback should not be invoked after unregistration
    EXPECT_FALSE(callbackInvoked);
}

// Test clearing all hotkeys
TEST_F(KeyboardHookTest, ClearAllHotkeys) {
    KeyboardHook hook;

    hook.RegisterHotkey(VK_F1, MOD_WIN, [](DWORD, DWORD) {});
    hook.RegisterHotkey(VK_F2, MOD_CTRL, [](DWORD, DWORD) {});
    hook.RegisterHotkey(VK_F3, MOD_SHIFT, [](DWORD, DWORD) {});

    hook.ClearAllHotkeys();

    // All hotkeys should be cleared
    // We can't verify this directly, but we can verify no crash occurs
    SUCCEED();
}

// Test modifier key combinations
TEST_F(KeyboardHookTest, ModifierCombinations) {
    KeyboardHook hook;

    // Test various modifier combinations
    DWORD mods1 = CombineModifiers({ MOD_WIN, MOD_CTRL });
    DWORD mods2 = CombineModifiers({ MOD_WIN, MOD_SHIFT, MOD_ALT });
    DWORD mods3 = CombineModifiers({ MOD_CTRL, MOD_SHIFT });

    hook.RegisterHotkey(VK_A, mods1, [](DWORD, DWORD) {});
    hook.RegisterHotkey(VK_B, mods2, [](DWORD, DWORD) {});
    hook.RegisterHotkey(VK_C, mods3, [](DWORD, DWORD) {});

    SUCCEED();
}

// Test that Install/Uninstall work correctly
// Note: This test may fail in non-interactive environments (like CI)
// because hook installation requires a message loop
TEST_F(KeyboardHookTest, InstallUninstall) {
    KeyboardHook hook;

    // Try to install the hook
    // This may fail in CI environments without a GUI session
    bool installed = hook.Install();

    if (installed) {
        EXPECT_TRUE(hook.IsInstalled());

        bool uninstalled = hook.Uninstall();
        EXPECT_TRUE(uninstalled);
        EXPECT_FALSE(hook.IsInstalled());
    }
    else {
        // In CI environments, we just verify the methods don't crash
        EXPECT_FALSE(hook.IsInstalled());
    }
}
