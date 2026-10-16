cmake_minimum_required(VERSION 3.15)
project(zx81ide VERSION 0.1.1 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Source files
set(SOURCES
    src/main.cpp
    src/cli/cli_parser.cpp
    src/cli/cli_app.cpp
    src/ui_tui/tui_app.cpp
)

# Executable target
add_executable(zx81ide ${SOURCES})

# Target include directories
target_include_directories(zx81ide PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Compiler options
if(MSVC)
    target_compile_options(zx81ide PRIVATE /W4 /permissive-)
else()
    target_compile_options(zx81ide PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Installation rules for dist/ packaging
install(TARGETS zx81ide DESTINATION bin)
install(FILES 
    ${CMAKE_CURRENT_SOURCE_DIR}/README.md
    ${CMAKE_CURRENT_SOURCE_DIR}/MANUAL.md
    ${CMAKE_CURRENT_SOURCE_DIR}/SPEC.md
    ${CMAKE_CURRENT_SOURCE_DIR}/CHANGELOG.md
    ${CMAKE_CURRENT_SOURCE_DIR}/RELEASE.md
    ${CMAKE_CURRENT_SOURCE_DIR}/LICENSE
    DESTINATION .
    OPTIONAL
)
