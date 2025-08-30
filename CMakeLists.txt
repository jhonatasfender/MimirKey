cmake_minimum_required(VERSION 3.21)

project(keymaps LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if(CMAKE_BUILD_TYPE STREQUAL "")
  set(CMAKE_BUILD_TYPE
      Release
      CACHE STRING "Build type" FORCE)
endif()

if(MSVC)
  add_compile_options(/W4)
else()
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Cross-platform Qt discovery (Qt6 preferred, fallback to Qt7) You can still
# override via -DQt6_DIR=... or -DCMAKE_PREFIX_PATH=...
if(NOT CMAKE_PREFIX_PATH)
  list(
    APPEND
    CMAKE_PREFIX_PATH
    "/usr/lib/x86_64-linux-gnu"
    "/usr/lib/x86_64-linux-gnu/cmake"
    "/opt/homebrew/opt/qt@6/lib/cmake"
    "/usr/local/opt/qt/lib/cmake"
    "/opt/local/lib/cmake")
endif()

set(_qt6_hints ' "$ENV{Qt6_DIR}" "$ENV{QT_DIR}/lib/cmake/Qt6"
               "/usr/lib/x86_64-linux-gnu/cmake/Qt6")
set(_qt7_hints "$ENV{Qt7_DIR}" "$ENV{QT_DIR}/lib/cmake/Qt7")

find_package(
  Qt6
  COMPONENTS Quick Qml Svg HINTS ${_qt6_hints}
  QUIET)
if(Qt6_FOUND)
  set(QT_VERSION_MAJOR 6)
else()
  find_package(
    Qt7
    COMPONENTS Quick Qml Svg HINTS ${_qt7_hints}
    QUIET)
  if(Qt7_FOUND)
    set(QT_VERSION_MAJOR 7)
  else()
    message(
      FATAL_ERROR
        "Could not find Qt (Qt6 or Qt7).\n"
        "Provide -DQt6_DIR=/path/to/Qt6Config.cmake directory or set CMAKE_PREFIX_PATH to your Qt install prefix."
    )
  endif()
endif()

set(PROJECT_SOURCES
    src/interface/main.cpp
    src/interface/viewmodels/KeymapViewModel.cpp
    src/application/usecases/LoadKeymap.cpp
    src/infrastructure/fs/FileKeymapRepository.cpp
    src/application/usecases/CopyText.hpp
    src/infrastructure/fs/QtClipboardRepository.hpp)

add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})

target_include_directories(${PROJECT_NAME}
                           PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

target_link_libraries(
  ${PROJECT_NAME} PRIVATE Qt${QT_VERSION_MAJOR}::Quick
                          Qt${QT_VERSION_MAJOR}::Qml Qt${QT_VERSION_MAJOR}::Svg)

# Apply Qt project defaults if available
if(COMMAND qt_standard_project_setup)
  qt_standard_project_setup()
endif()

# QML module/resources
if(COMMAND qt_policy)
  qt_policy(SET QTP0001 NEW) # default resource prefix for QML modules
endif()
qt_add_qml_module(
  ${PROJECT_NAME}
  URI
  Keymaps
  VERSION
  1.0
  QML_FILES
  src/interface/qml/Main.qml
  src/interface/qml/components/TopBar.qml
  src/interface/qml/components/KeymapView.qml
  src/interface/qml/components/LogPanel.qml
  RESOURCES
  src/interface/qml/assets/keymap.svg)

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)

# Convenience target to format sources
add_custom_target(
  format
  COMMAND ${CMAKE_COMMAND} -E env HOME=$ENV{HOME} bash
          ${CMAKE_CURRENT_SOURCE_DIR}/scripts/format.sh
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  COMMENT "Running code formatters (clang-format, qmlformat)")
