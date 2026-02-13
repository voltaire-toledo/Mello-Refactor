# Specification 001: Core Keyboard Hook

## Overview
This specification defines the implementation of the low-level keyboard hook system that forms the foundation of the Mello-Refactor application. The system provides global hotkey detection using Windows API hooks.

## Goals
1. Implement a robust, system-wide keyboard hook
2. Track modifier key states (Win, Ctrl, Shift, Alt)
3. Support multiple hotkey registrations with conflict detection
4. Provide thread-safe callback mechanism
5. Ensure proper resource cleanup (RAII pattern)
6. Follow PowerToys architectural patterns

## Architecture

### Component Overview

```
┌─────────────────────────────────────────────────┐
│           WinUI 3 Application                   │
│                                                 │
│  ┌─────────────────────────────────────────┐   │
│  │       HotkeyManager                     │   │
│  │  - High-level hotkey management         │   │
│  │  - Conflict detection                   │   │
│  │  - Thread-safe registration             │   │
│  └─────────────┬───────────────────────────┘   │
│                │                                 │
│                │ Uses                            │
│                ▼                                 │
│  ┌─────────────────────────────────────────┐   │
│  │       KeyboardHook                      │   │
│  │  - Low-level WH_KEYBOARD_LL hook       │   │
│  │  - Modifier state tracking              │   │
│  │  - Callback dispatch                    │   │
│  └─────────────────────────────────────────┘   │
│                                                 │
└─────────────────┬───────────────────────────────┘
                  │
                  │ SetWindowsHookEx
                  ▼
            ┌─────────────┐
            │ Windows API │
            │   Hooks     │
            └─────────────┘
```

### Class Hierarchy

#### KeyboardHook (Low-Level)
- **Responsibility**: Direct interaction with Windows keyboard hook API
- **Namespace**: `MelloRefactor::Core`
- **Thread Model**: Single-instance, must be called from thread with message loop
- **Key Features**:
  - Install/uninstall `WH_KEYBOARD_LL` hook
  - Track modifier key states independently
  - Dispatch callbacks for registered hotkey combinations
  - Automatic cleanup via RAII

#### HotkeyManager (High-Level)
- **Responsibility**: Manage multiple hotkeys with convenience features
- **Namespace**: `MelloRefactor::Core`
- **Thread Model**: Thread-safe via mutex protection
- **Key Features**:
  - Action-based hotkey registration
  - Conflict detection
  - Lifecycle management (Start/Stop)
  - Query registered hotkeys

#### KeyModifiers (Constants & Utilities)
- **Responsibility**: Define modifier key constants and utility functions
- **Namespace**: `MelloRefactor::Core`
- **Key Features**:
  - Standard modifier constants (MOD_WIN, MOD_CTRL, etc.)
  - String conversion utilities
  - Modifier combination helpers

## Technical Details

### Windows Hook Type: WH_KEYBOARD_LL

**Why WH_KEYBOARD_LL instead of WH_KEYBOARD?**

1. **System-Wide Coverage**
   - `WH_KEYBOARD_LL` works across all applications
   - `WH_KEYBOARD` only hooks the current thread

2. **Event Priority**
   - Low-level hooks receive events before target windows
   - Allows prevention of default behavior if needed

3. **Compatibility**
   - Doesn't require DLL injection
   - Can be installed from any process

4. **Limitations**
   - Must not block (keep processing fast)
   - Cannot call many USER32 functions from callback
   - Only one instance recommended per process

### Modifier Key Tracking

Modifier keys are tracked separately because:
1. They can be pressed independently or in combination
2. Virtual key codes differ for left/right variants (e.g., VK_LSHIFT vs VK_RSHIFT)
3. State must persist across multiple keyboard events

**Tracked Keys**:
- Win: `VK_LWIN`, `VK_RWIN`
- Ctrl: `VK_LCONTROL`, `VK_RCONTROL`, `VK_CONTROL`
- Shift: `VK_LSHIFT`, `VK_RSHIFT`, `VK_SHIFT`
- Alt: `VK_LMENU`, `VK_RMENU`, `VK_MENU`

**State Variables**:
```cpp
bool m_winKeyDown;
bool m_ctrlKeyDown;
bool m_shiftKeyDown;
bool m_altKeyDown;
```

### Hotkey Identification

Hotkeys are uniquely identified by combining modifier flags and virtual key code:

```cpp
uint64_t MakeHotkeyKey(DWORD vkCode, DWORD modifiers) {
    return (static_cast<uint64_t>(modifiers) << 32) | vkCode;
}
```

This allows:
- Fast lookup in `std::unordered_map`
- Guaranteed uniqueness for each combination
- Simple conflict detection

### Thread Safety

#### KeyboardHook
- **Not thread-safe** by design
- Single instance enforced via static pointer
- Must be created and used from same thread
- Hook callback executes on the thread that installed it

#### HotkeyManager
- **Thread-safe** for all public methods
- Uses `std::mutex` to protect shared state
- Allows registration from any thread
- Hook operations delegated to underlying KeyboardHook (same thread)

### Resource Management (RAII)

Both classes follow RAII principles:

```cpp
{
    KeyboardHook hook;
    hook.Install();
    // Hook automatically uninstalled when 'hook' goes out of scope
}
```

**Destructor Guarantees**:
- Hook is uninstalled via `UnhookWindowsHookEx`
- Static instance pointer is cleared
- No memory leaks from callback map
- Safe even if Install() was never called

## API Reference

### KeyboardHook Class

#### Constructor
```cpp
KeyboardHook();
```
- Initializes member variables
- Sets static instance pointer
- Throws `std::runtime_error` if instance already exists

#### Destructor
```cpp
~KeyboardHook();
```
- Calls `Uninstall()` if needed
- Clears static instance pointer

#### Install
```cpp
bool Install();
```
- **Returns**: `true` on success, `false` on failure
- **Thread**: Must be called from thread with message loop
- **Effect**: Installs `WH_KEYBOARD_LL` hook
- **Idempotent**: Safe to call multiple times

#### Uninstall
```cpp
bool Uninstall();
```
- **Returns**: `true` on success, `false` on failure
- **Effect**: Removes hook and resets modifier state
- **Idempotent**: Safe to call multiple times

#### RegisterHotkey
```cpp
void RegisterHotkey(DWORD vkCode, DWORD modifiers, HotkeyCallback callback);
```
- **Parameters**:
  - `vkCode`: Virtual key code (e.g., `VK_F1`, `'A'`)
  - `modifiers`: Modifier flags (e.g., `MOD_WIN | MOD_CTRL`)
  - `callback`: Function to call when hotkey is pressed
- **Effect**: Registers or replaces callback for this combination
- **Thread Safety**: Not thread-safe (caller must synchronize)

#### UnregisterHotkey
```cpp
void UnregisterHotkey(DWORD vkCode, DWORD modifiers);
```
- **Effect**: Removes callback for this combination
- **No-op**: If combination not registered

### HotkeyManager Class

#### Start
```cpp
bool Start();
```
- **Returns**: `true` on success, `false` on failure
- **Effect**: Creates and installs keyboard hook
- **Thread**: Must be called from thread with message loop
- **Thread Safety**: Thread-safe

#### Stop
```cpp
void Stop();
```
- **Effect**: Uninstalls hook and clears all registrations
- **Thread Safety**: Thread-safe

#### RegisterHotkey
```cpp
bool RegisterHotkey(const HotkeyAction& hotkeyAction);
```
- **Returns**: `false` if conflict or manager not running
- **Effect**: Registers action for hotkey combination
- **Thread Safety**: Thread-safe

#### IsHotkeyConflict
```cpp
bool IsHotkeyConflict(DWORD vkCode, DWORD modifiers) const;
```
- **Returns**: `true` if combination already registered
- **Thread Safety**: Thread-safe

### KeyModifiers Namespace

#### Constants
```cpp
constexpr DWORD MOD_NONE = 0x0000;
constexpr DWORD MOD_WIN = 0x0001;
constexpr DWORD MOD_CTRL = 0x0002;
constexpr DWORD MOD_SHIFT = 0x0004;
constexpr DWORD MOD_ALT = 0x0008;
```

#### Utility Functions
```cpp
std::wstring ModifiersToString(DWORD modifiers);
std::wstring VirtualKeyToString(DWORD vkCode);
std::wstring HotkeyToString(DWORD modifiers, DWORD vkCode);
```

## Usage Examples

### Basic Hook Usage
```cpp
#include "KeyboardHook.h"
#include "KeyModifiers.h"

using namespace MelloRefactor::Core;

// Create and install hook
KeyboardHook hook;
hook.Install();

// Register Win+F1 hotkey
hook.RegisterHotkey(VK_F1, MOD_WIN, [](DWORD vk, DWORD mods) {
    MessageBox(NULL, L"F1 pressed!", L"Info", MB_OK);
});

// Message loop required...
MSG msg;
while (GetMessage(&msg, NULL, 0, 0)) {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
}

// Hook automatically uninstalled when 'hook' goes out of scope
```

### HotkeyManager Usage
```cpp
#include "HotkeyManager.h"
#include "KeyModifiers.h"

using namespace MelloRefactor::Core;

HotkeyManager manager;
manager.Start();

// Register multiple hotkeys
HotkeyManager::HotkeyAction action1;
action1.vkCode = VK_F1;
action1.modifiers = MOD_WIN;
action1.action = []() { LaunchHelp(); };
action1.description = L"Launch Help";

HotkeyManager::HotkeyAction action2;
action2.vkCode = 'N';
action2.modifiers = MOD_WIN | MOD_CTRL;
action2.action = []() { CreateNewDocument(); };
action2.description = L"New Document";

manager.RegisterHotkey(action1);
manager.RegisterHotkey(action2);

// Check for conflicts
if (manager.IsHotkeyConflict(VK_F1, MOD_WIN)) {
    // Handle conflict
}

// Manager automatically stops when destroyed
```

## Implementation Notes

### Error Handling
- Methods return `bool` for success/failure (avoid exceptions in callbacks)
- Callback exceptions are caught and suppressed (logged in production)
- `GetLastError()` can be checked after `Install()` failure

### Performance Considerations
1. **Keep Callbacks Fast**
   - Hook callbacks must return quickly
   - Defer heavy work to another thread
   - Don't call blocking operations

2. **Map Lookup Efficiency**
   - `std::unordered_map` provides O(1) lookup
   - uint64_t key avoids string comparisons
   - Small number of hotkeys expected (< 100)

3. **Memory Usage**
   - One static instance of KeyboardHook
   - Callback storage: `~24 bytes` per registered hotkey
   - Minimal overhead for modifier tracking

### Security Considerations
1. **Hook Installation**
   - Requires sufficient privileges
   - May be blocked by security software
   - User should be notified if installation fails

2. **Callback Isolation**
   - Exceptions cannot propagate to Windows hook system
   - Invalid callbacks are ignored (null check)

3. **Resource Limits**
   - No hard limit on registered hotkeys
   - Consider limiting to prevent abuse

## Testing Strategy

### Unit Tests (Google Test)

#### KeyboardHook Tests
- ✅ Constructor/Destructor lifecycle
- ✅ Single instance enforcement
- ✅ Register/Unregister hotkeys
- ✅ Clear all hotkeys
- ✅ Modifier combinations
- ⚠️ Install/Uninstall (may fail in CI without GUI)

#### HotkeyManager Tests
- ✅ Start/Stop lifecycle
- ✅ Hotkey registration
- ✅ Conflict detection
- ✅ Unregister hotkeys
- ✅ Thread safety (basic)
- ✅ Multiple hotkey management

### Integration Tests (Manual)
1. Install hook in WinUI 3 app
2. Register Win+F1 hotkey
3. Press Win+F1 → verify callback invoked
4. Register conflicting hotkey → verify rejection
5. Unregister hotkey → verify no longer active
6. Close app → verify hook cleanup

### Edge Cases
- Register same hotkey twice (should replace)
- Unregister non-existent hotkey (no-op)
- Install hook without message loop (fails gracefully)
- Rapid key presses (state tracking accuracy)
- Left vs right modifier keys (both tracked)

## Future Enhancements

1. **Hotkey Serialization**
   - Save/load hotkey configurations to JSON
   - Import/export settings

2. **Advanced Modifiers**
   - Support for extended keys
   - CapsLock, NumLock as modifiers

3. **Visual Feedback**
   - On-screen display when hotkey pressed
   - Hotkey recording mode

4. **Conflict Resolution**
   - Automatic conflict detection with system hotkeys
   - Priority levels for conflicting hotkeys

5. **Statistics**
   - Track hotkey usage frequency
   - Performance metrics for callback execution

## References

### PowerToys Implementation
- **File**: `microsoft/PowerToys/src/runner/centralized_kb_hook.cpp`
- **Pattern**: Similar low-level hook with modifier tracking
- **Differences**: PowerToys uses C++/CLI, we use pure C++20

### Windows API Documentation
- [SetWindowsHookEx](https://docs.microsoft.com/windows/win32/api/winuser/nf-winuser-setwindowshookexw)
- [LowLevelKeyboardProc](https://docs.microsoft.com/windows/win32/winmsg/lowlevelkeyboardproc)
- [KBDLLHOOKSTRUCT](https://docs.microsoft.com/windows/win32/api/winuser/ns-winuser-kbdllhookstruct)
- [Virtual-Key Codes](https://docs.microsoft.com/windows/win32/inputdev/virtual-key-codes)

### C++20 Features Used
- `constexpr` for compile-time constants
- `std::function` for type-erased callbacks
- `std::unordered_map` for efficient lookup
- Smart pointers (`std::unique_ptr`)
- Range-based for loops
- `std::initializer_list` for modifier combination

## Acceptance Criteria

✅ KeyboardHook can be installed and uninstalled  
✅ Modifier keys are tracked accurately  
✅ Hotkeys can be registered and unregistered  
✅ Callbacks are invoked when hotkeys are pressed  
✅ RAII pattern ensures cleanup  
✅ Single instance enforcement works  
✅ HotkeyManager provides thread-safe interface  
✅ Conflict detection prevents duplicate registrations  
✅ Unit tests cover core functionality  
✅ Code follows PowerToys patterns  
✅ Documentation is comprehensive  
✅ All files have copyright headers  

## Version History

- **v1.0** (2026-02-13): Initial specification and implementation
