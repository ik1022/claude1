cmake_minimum_required(VERSION 3.20)

project(libemb
    LANGUAGES CXX
    VERSION 0.1.0
    DESCRIPTION "Embedded C++ Firmware Library - Reusable HAL, Drivers, and Utilities"
)

# C++17 standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Build options
option(LIBEMB_BUILD_TESTS "Build unit tests" OFF)
option(LIBEMB_BUILD_EXAMPLES "Build example applications" OFF)
option(LIBEMB_ENABLE_ASAN "Enable AddressSanitizer for host builds" OFF)
option(LIBEMB_ENABLE_COVERAGE "Enable code coverage reporting (requires GCC/Clang)" OFF)

# Detect cross-compilation
if(CMAKE_CROSSCOMPILING)
    set(LIBEMB_BUILD_TESTS OFF CACHE BOOL "Disable tests for cross-compilation" FORCE)
    message(STATUS "Cross-compilation detected - tests disabled")
endif()

# AddressSanitizer configuration
if(LIBEMB_ENABLE_ASAN)
    if(NOT CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
        message(FATAL_ERROR "AddressSanitizer requires Clang or GCC")
    endif()
    add_compile_options(-fsanitize=address -fno-omit-frame-pointer)
    add_link_options(-fsanitize=address)
    message(STATUS "AddressSanitizer enabled")
endif()

# Code coverage configuration
if(LIBEMB_ENABLE_COVERAGE)
    if(NOT CMAKE_CXX_COMPILER_ID MATCHES "GNU")
        message(FATAL_ERROR "Code coverage requires GCC (not Clang)")
    endif()
    if(CMAKE_CROSSCOMPILING)
        message(FATAL_ERROR "Code coverage not supported for cross-compilation")
    endif()

    add_compile_options(-fprofile-arcs -ftest-coverage)
    add_link_options(--coverage)
    message(STATUS "Code coverage enabled")
endif()

# Compiler-specific flags
if(MSVC)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Werror)
endif()

# Include CMake modules
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# Main library components
add_subdirectory(include/libemb/hal)
add_subdirectory(include/libemb/util)
add_subdirectory(src/driver)
add_subdirectory(src/protocol)

# Mock implementations (host only - uses STL)
if(NOT CMAKE_CROSSCOMPILING)
    add_subdirectory(port/mock)
endif()

# Tests
if(LIBEMB_BUILD_TESTS)
    enable_testing()
    add_subdirectory(test)
endif()

# Examples
if(LIBEMB_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

# Print configuration summary
message(STATUS "======== libemb Configuration ========")
message(STATUS "Build tests: ${LIBEMB_BUILD_TESTS}")
message(STATUS "Build examples: ${LIBEMB_BUILD_EXAMPLES}")
message(STATUS "Enable ASAN: ${LIBEMB_ENABLE_ASAN}")
message(STATUS "Cross-compiling: ${CMAKE_CROSSCOMPILING}")
message(STATUS "CXX Standard: ${CMAKE_CXX_STANDARD}")
