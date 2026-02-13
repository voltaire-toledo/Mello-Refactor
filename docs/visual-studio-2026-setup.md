# Visual Studio 2026/2022 Setup Guide

## Prerequisites

### System Requirements
- **Operating System**: Windows 11 (22H2 or later)
- **RAM**: 16 GB or more recommended
- **Disk Space**: At least 30 GB free space for Visual Studio and SDKs

## Visual Studio Installation

### 1. Download Visual Studio
Download Visual Studio 2022 Community Edition (or 2026 if available) from:
https://visualstudio.microsoft.com/downloads/

### 2. Required Workloads

During installation, select the following workloads:

#### Desktop development with C++
- MSVC v143 - VS 2022 C++ x64/x86 build tools (Latest)
- C++ CMake tools for Windows
- C++ AddressSanitizer
- Windows 10 SDK (10.0.19041.0 or later)
- Windows 11 SDK (10.0.22621.0 or later)

#### Universal Windows Platform development
- Windows App SDK C++ Templates
- Windows App SDK (latest version)
- C++/WinRT templates and tools

### 3. Individual Components

In addition to the workloads, ensure these individual components are selected:

- **Compilers, build tools, and runtimes**
  - C++ 2022 Redistributable Update
  - MSVC v143 - VS 2022 C++ x64/x86 build tools (Latest)
  - C++ CMake tools for Windows

- **Development activities**
  - C++ core features
  - C++ profiling tools

- **SDKs, libraries, and frameworks**
  - Windows 10 SDK (10.0.19041.0)
  - Windows 11 SDK (10.0.22621.0)
  - C++/WinRT

- **Code tools**
  - Git for Windows
  - GitHub Extension for Visual Studio

### 4. Optional Components (Recommended)

- **Debugging and testing**
  - C++ AddressSanitizer
  - Test Adapter for Google Test
  - C++ profiling tools

- **Code analysis**
  - C++ Core Guidelines checkers

## Windows App SDK Installation

### Method 1: Via Visual Studio Installer (Recommended)
The Windows App SDK should be installed automatically with the UWP workload.

### Method 2: Via NuGet (Already Configured)
The project is configured to automatically download the Windows App SDK via NuGet package restore.

### Verify Installation
1. Open Visual Studio
2. Go to Tools > Get Tools and Features
3. Verify "Windows App SDK C++ Templates" is installed

## Post-Installation Setup

### 1. Enable Developer Mode
1. Open Windows Settings
2. Go to Privacy & Security > For developers
3. Toggle "Developer Mode" to On

### 2. Configure Visual Studio

#### Set C++ Language Standard
The project is already configured for C++20. To verify:
1. Right-click the project > Properties
2. Configuration Properties > C/C++ > Language
3. Ensure "C++ Language Standard" is set to "ISO C++20 Standard (/std:c++20)"

#### NuGet Package Manager
1. Tools > Options > NuGet Package Manager
2. Ensure "Allow NuGet to download missing packages" is checked
3. Ensure "Automatically check for missing packages during build" is checked

## Building the Project

### 1. Clone the Repository
```bash
git clone https://github.com/voltaire-toledo/Mello-Refactor.git
cd Mello-Refactor
```

### 2. Open the Solution
1. Double-click `MelloRefactor.sln` or
2. Open Visual Studio > File > Open > Project/Solution > Select `MelloRefactor.sln`

### 3. Restore NuGet Packages
1. Right-click the solution in Solution Explorer
2. Select "Restore NuGet Packages"
3. Wait for packages to download (this may take a few minutes)

Required packages:
- Microsoft.WindowsAppSDK (1.6.0)
- Microsoft.Windows.CppWinRT (2.0.240405.15)
- nlohmann.json (3.11.3)
- Microsoft.googletest.v140.windesktop.msvcstl.static.rt-dyn (1.14.0)

### 4. Select Build Configuration
1. In the toolbar, select "Debug" or "Release"
2. Select "x64" platform (only x64 is supported)

### 5. Build the Solution
- Press **F7** or
- Build > Build Solution or
- Right-click solution > Build Solution

Expected output:
```
Build started...
1>------ Build started: Project: MelloRefactor.Core, Configuration: Debug x64 ------
1>  pch.cpp
1>  KeyModifiers.cpp
1>  KeyboardHook.cpp
1>  HotkeyManager.cpp
1>  MelloRefactor.Core.vcxproj -> ...\bin\x64\Debug\MelloRefactor.Core.lib
2>------ Build started: Project: MelloRefactor, Configuration: Debug x64 ------
...
========== Build: 3 succeeded, 0 failed, 0 up-to-date, 0 skipped ==========
```

### 6. Run the Application
- Press **F5** (Debug mode) or **Ctrl+F5** (without debugging)
- The main window should appear

## Running Tests

### Using Test Explorer
1. Test > Test Explorer (or press Ctrl+E, T)
2. Click "Run All" to execute all tests
3. View test results in the Test Explorer window

### Using Command Line
```bash
vstest.console.exe bin\x64\Debug\MelloRefactor.Tests.dll
```

## Common Build Issues

### Issue: "Cannot open source file 'winrt/...'"
**Solution**: 
- Ensure C++/WinRT is installed
- Restore NuGet packages
- Clean and rebuild the solution

### Issue: "Windows App SDK not found"
**Solution**:
- Install Windows App SDK via Visual Studio Installer
- Verify NuGet package "Microsoft.WindowsAppSDK" is restored
- Check packages.config for correct version

### Issue: "LNK2019: unresolved external symbol"
**Solution**:
- Ensure MelloRefactor.Core is built before MelloRefactor
- Check project dependencies in solution properties
- Verify project references are correct

### Issue: "Google Test headers not found"
**Solution**:
- Restore NuGet packages for the test project
- Verify "Microsoft.googletest" package is installed
- Clean and rebuild the test project

### Issue: "Keyboard hook installation fails"
**Solution**:
- This is expected in non-interactive environments (like CI)
- Run the application with administrator privileges (if needed)
- Ensure Windows Security isn't blocking the hook

## Build Configurations

### Debug Configuration
- Optimizations: Disabled
- Runtime Library: Multi-threaded Debug DLL (/MDd)
- Generates debug symbols (.pdb files)
- AddressSanitizer: Available but not enabled by default

### Release Configuration
- Optimizations: Maximum (/O2)
- Runtime Library: Multi-threaded DLL (/MD)
- Whole Program Optimization: Enabled
- Generates optimized binaries

## Project Structure

```
MelloRefactor/
├── MelloRefactor.sln                 # Solution file
├── src/
│   ├── MelloRefactor/                # WinUI 3 Application
│   │   ├── MelloRefactor.vcxproj
│   │   ├── App.xaml / .h / .cpp
│   │   ├── MainWindow.xaml / .h / .cpp
│   │   ├── Package.appxmanifest
│   │   └── Assets/
│   └── MelloRefactor.Core/           # Core Library
│       ├── MelloRefactor.Core.vcxproj
│       ├── include/                  # Public headers
│       │   ├── KeyboardHook.h
│       │   ├── HotkeyManager.h
│       │   ├── KeyModifiers.h
│       │   └── HotkeyConfig.h
│       └── src/                      # Implementation
│           ├── KeyboardHook.cpp
│           ├── HotkeyManager.cpp
│           └── KeyModifiers.cpp
└── tests/
    └── MelloRefactor.Tests/          # Unit Tests
        ├── MelloRefactor.Tests.vcxproj
        ├── KeyboardHookTests.cpp
        └── HotkeyManagerTests.cpp
```

## Additional Resources

### Documentation
- [Windows App SDK Documentation](https://docs.microsoft.com/windows/apps/windows-app-sdk/)
- [C++/WinRT Documentation](https://docs.microsoft.com/windows/uwp/cpp-and-winrt-apis/)
- [WinUI 3 Gallery](https://github.com/microsoft/WinUI-Gallery)
- [PowerToys Source Code](https://github.com/microsoft/PowerToys) (reference implementation)

### Keyboard Hook Resources
- [SetWindowsHookEx Documentation](https://docs.microsoft.com/windows/win32/api/winuser/nf-winuser-setwindowshookexw)
- [Low-Level Keyboard Hook](https://docs.microsoft.com/windows/win32/winmsg/lowlevelkeyboardproc)

### C++20 Features Used
- Concepts
- Ranges
- Designated initializers
- constexpr improvements
- Three-way comparison operator (spaceship operator)

## Troubleshooting

### Clean and Rebuild
If you encounter persistent build errors:
1. Build > Clean Solution
2. Close Visual Studio
3. Delete `bin`, `obj`, and `.vs` folders
4. Reopen Visual Studio
5. Restore NuGet packages
6. Build > Rebuild Solution

### Reset Visual Studio Settings
If Visual Studio behaves unexpectedly:
1. Tools > Import and Export Settings
2. Reset all settings
3. Restart Visual Studio

## Next Steps

After successful setup:
1. Review the code in `src/MelloRefactor.Core/`
2. Understand the keyboard hook implementation
3. Run the unit tests to verify functionality
4. Experiment with the WinUI 3 application
5. Read the specification in `docs/specs/001-core-keyboard-hook.md`

## Support

For issues specific to this project:
- Check existing GitHub Issues
- Review the documentation in `docs/`
- Consult PowerToys implementation for reference patterns

For Visual Studio issues:
- [Visual Studio Documentation](https://docs.microsoft.com/visualstudio/)
- [Visual Studio Developer Community](https://developercommunity.visualstudio.com/)
