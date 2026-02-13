# Mello-Refactor

A modern Windows 11 native application for managing global keyboard shortcuts and automating Windows actions.

## 🎯 Project Goals

- Learn modern Windows development with C++/WinRT and WinUI 3
- Master spec-driven development with GitHub Copilot
- Create a production-ready keyboard automation tool
- Foundation for refactoring Mello-Workspace into a native Windows app

## 🛠️ Tech Stack

- **Language**: C++20, C++/WinRT
- **UI Framework**: WinUI 3 (Windows App SDK)
- **Build System**: Visual Studio 2022/2026 Community Edition
- **Configuration**: JSON/YAML
- **Architecture**: Modern Win32 with low-level keyboard hooks

## 📋 Features (Planned)

- [x] Project setup and architecture
- [x] Low-level keyboard hook implementation (Spec 001)
- [x] WinUI 3 modern interface (basic)
- [x] Hotkey conflict detection
- [ ] JSON/YAML configuration loader
- [ ] Action execution system
- [ ] System tray integration
- [ ] Settings management
- [ ] Auto-startup support
- [ ] Import/Export configurations

## 🚀 Getting Started

### Prerequisites

- Windows 11 (22H2 or later)
- Visual Studio 2022 Community Edition with:
  - Desktop development with C++
  - Universal Windows Platform development
  - Windows App SDK (WinUI 3)
  - C++/WinRT tools

### Building

1. Clone the repository:
   ```bash
   git clone https://github.com/voltaire-toledo/Mello-Refactor.git
   cd Mello-Refactor
   ```

2. Open `MelloRefactor.sln` in Visual Studio 2022

3. Restore NuGet packages (right-click solution > Restore NuGet Packages)

4. Build the solution (F7)

5. Run (F5)

For detailed setup instructions, see [docs/visual-studio-2026-setup.md](docs/visual-studio-2026-setup.md)

## 📁 Project Structure

```
Mello-Refactor/
├── MelloRefactor.sln                    # Visual Studio solution
├── src/
│   ├── MelloRefactor/                   # WinUI 3 Application
│   │   ├── App.xaml / .h / .cpp
│   │   ├── MainWindow.xaml / .h / .cpp
│   │   ├── Package.appxmanifest
│   │   └── Assets/                      # App icons and resources
│   └── MelloRefactor.Core/              # Core Library (Static .lib)
│       ├── include/                     # Public headers
│       │   ├── KeyboardHook.h           # Low-level keyboard hook
│       │   ├── HotkeyManager.h          # High-level hotkey manager
│       │   ├── KeyModifiers.h           # Modifier key constants
│       │   └── HotkeyConfig.h           # Configuration structures
│       └── src/                         # Implementation files
├── tests/
│   └── MelloRefactor.Tests/             # Google Test unit tests
│       ├── KeyboardHookTests.cpp
│       └── HotkeyManagerTests.cpp
├── docs/                                # Documentation
│   ├── specs/                           # Feature specifications
│   │   └── 001-core-keyboard-hook.md    # Keyboard hook spec
│   ├── visual-studio-2026-setup.md      # Setup guide
│   └── copilot-workflow.md              # Development workflow
└── config/                              # Sample configurations
    └── example-config.json
```

## 📖 Learning Path

This project serves as a learning platform for:

1. **Windows Development**
   - Win32 API and low-level hooks
   - C++/WinRT for modern Windows programming
   - WinUI 3 for beautiful UIs

2. **Modern C++**
   - C++20 features
   - RAII and smart pointers
   - Async/await patterns

3. **Development Practices**
   - Spec-driven development
   - GitHub Copilot workflows
   - Test-driven development
   - Clean architecture

## 🤝 Copilot Collaboration

This project is built in collaboration with GitHub Copilot. See [docs/copilot-workflow.md](docs/copilot-workflow.md) for our development process.

## 📝 License

MIT License - See [LICENSE](LICENSE) for details

## 🎓 Credits

Inspired by Microsoft PowerToys architecture and patterns.
