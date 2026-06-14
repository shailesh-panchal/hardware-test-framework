cmake_minimum_required(VERSION 3.16)

project(
    HardwareValidationFramework
    VERSION 1.0.0
    LANGUAGES C
)

# ---------------------------------------------------------------------------
# C Standard
# ---------------------------------------------------------------------------

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

# ---------------------------------------------------------------------------
# Build Type
# ---------------------------------------------------------------------------

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

# ---------------------------------------------------------------------------
# Compiler Warnings
# ---------------------------------------------------------------------------

if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(
        -Wall
        -Wextra
        -Wpedantic
    )
endif()

# ---------------------------------------------------------------------------
# Output Directories
# ---------------------------------------------------------------------------

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# ---------------------------------------------------------------------------
# Include Directories
# ---------------------------------------------------------------------------

include_directories(
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/src
    ${PROJECT_SOURCE_DIR}/src/config/
    ${PROJECT_SOURCE_DIR}/src/utils
    ${PROJECT_SOURCE_DIR}/third-party/cjson-1.7.19
)

# ---------------------------------------------------------------------------
# Source Files
# ---------------------------------------------------------------------------

file(GLOB_RECURSE APP_SOURCES
    src/*.c
    third-party/cjson-1.7.19/*.c
)

# ---------------------------------------------------------------------------
# Executable
# ---------------------------------------------------------------------------

add_executable(validator
    ${APP_SOURCES}
)