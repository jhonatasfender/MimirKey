#!/usr/bin/env bash
set -euo pipefail
PROJECT_DIR="$(cd "$(dirname "$0")/.." && pwd)"

echo "==> Formatting QML"
QMLFORMAT_BIN="${HOME}/Qt/6.6.0/gcc_64/bin/qmlformat"
if [[ ! -x "$QMLFORMAT_BIN" ]]; then
  QMLFORMAT_BIN="$(command -v qmlformat || true)"
fi
if [[ -n "$QMLFORMAT_BIN" && -x "$QMLFORMAT_BIN" ]]; then
  find "$PROJECT_DIR/src/interface/qml" -type f -name '*.qml' -print0 | xargs -0 -n1 "$QMLFORMAT_BIN" -i || true
else
  echo "qmlformat not found; skipping QML formatting" >&2
fi

echo "==> Formatting C/C++"
find "$PROJECT_DIR/src" -type f \( -name '*.c' -o -name '*.cc' -o -name '*.cpp' -o -name '*.cxx' -o -name '*.h' -o -name '*.hh' -o -name '*.hpp' -o -name '*.hxx' \) -print0 | xargs -0 -r clang-format -i

echo "==> Formatting CMake"
if command -v cmake-format >/dev/null 2>&1; then
  
  find "$PROJECT_DIR" \
    -path "$PROJECT_DIR/build" -prune -o \
    -path "$PROJECT_DIR/cmake-build-*" -prune -o \
    -type f \( -name 'CMakeLists.txt' -o -name '*.cmake' \) -print0 \
    | xargs -0 -r cmake-format -i
else
  echo "cmake-format not found; skipping CMake formatting" >&2
fi

echo "Done."
