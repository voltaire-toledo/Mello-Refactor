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
- **Build System**: Visual Studio 2022 Community Edition
- **Configuration**: JSON/YAML
- **Architecture**: Modern Win32 with low-level keyboard hooks

## 📋 Features (Planned)

- [x] Project setup and architecture
- [ ] Low-level keyboard hook implementation
- [ ] JSON/YAML configuration loader
- [ ] WinUI 3 modern interface
- [ ] Action execution system
- [ ] System tray integration
- [ ] Settings management
- [ ] Hotkey conflict detection
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
   git clone https://github.com/voltaire-toledo/keyboard-hook-manager.git
   cd keyboard-hook-manager
   ```

2. Open `KeyboardHookManager.sln` in Visual Studio 2022

3. Build the solution (F7)

4. Run (F5)

## 📁 Project Structure

```
keyboard-hook-manager/
├── src/
│   ├── Core/              # Core keyboard hook logic
│   ├── Configuration/     # Config loading and parsing
│   ├── Actions/           # Action executors
│   ├── UI/                # WinUI 3 interface
│   └── Utils/             # Helper utilities
├── include/               # Public headers
├── tests/                 # Unit tests
├── docs/                  # Documentation
│   ├── specs/            # Feature specifications
│   ├── architecture/     # Architecture decisions
│   └── learning/         # Learning notes
├── config/               # Sample configurations
└── assets/               # Resources and icons
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
