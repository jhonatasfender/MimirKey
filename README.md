# MimirKey (C++20 + Qt)

Minimal scaffold for a desktop app to analyze keyboard keymaps, built with CMake, C++20 and Qt (6/7). MimirKey is inspired by Mímir, the Norse figure of wisdom. Follows SOLID and Clean Architecture rules defined in `.cursor/rules`.

## Requirements

- CMake 3.21+
- C++20 toolchain (GCC 12+/Clang 15+)
- Qt 6.6+ or Qt 7 (Quick and Qml modules)
- Linux/macOS/Windows (tested on Linux)

On Ubuntu/Debian (example):
```bash
sudo apt update
sudo apt install -y build-essential cmake qt6-base-dev qt6-declarative-dev
```

## Build

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
```

## Run

```bash
./build/keymaps
```

If Qt is not found automatically, set `CMAKE_PREFIX_PATH` to your Qt installation, e.g.:
```bash
cmake -S . -B build -DCMAKE_PREFIX_PATH=$HOME/Qt/6.7.2/gcc_64
```

## Project Layout

- `src/domain/` domain types (to be implemented)
- `src/application/` use cases (e.g., `LoadKeymap`)
- `src/infrastructure/` repositories/adapters (e.g., filesystem)
- `src/interface/` Qt UI (QML, view models)

## Rules (QML best practices)

- Componentize UI into small, reusable QML files in `src/interface/qml/components/`.
- Keep UI in QML and business logic in C++; expose data via properties/signals.
- Prefer composition over inheritance; parameterize with properties; document public API.
- Naming/style: ids camelCase; files/components PascalCase; order in files: id → properties → signals → functions → visual props → children → states → transitions.
- Layout: use Layouts/anchors (do not mix both on the same item); prefer responsive sizing over absolute values.
- Performance: avoid heavy JS; defer offscreen content with `Loader`; reuse components; limit timers and animations; cache images.
- Resources: bundle assets via `qt_add_qml_module`; prefer SVG; avoid runtime paths.
- Logging: centralize logs; capture key UI events with timestamp and metadata.

More details in `.cursor/rules/`.

## Notes

- Current repository returns demo layers; parsing of keymap files will be added next.
- See `.cursor/rules` for architecture, style and review checklists.
