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
option(LIBEMB_BUILD_BENCHMARKS "Build performance benchmarks" OFF)
option(LIBEMB_BUILD_DOCS "Generate Doxygen documentation" OFF)
option(LIBEMB_ENABLE_ASAN "Enable AddressSanitizer for host builds" OFF)
option(LIBEMB_ENABLE_UBSAN "Enable UndefinedBehaviorSanitizer" OFF)
option(LIBEMB_ENABLE_COVERAGE "Enable code coverage reporting (requires GCC/Clang)" OFF)
option(LIBEMB_ENABLE_CLANG_TIDY "Enable clang-tidy static analysis" OFF)

# Validation options (Phase 3)
option(LIBEMB_STRICT_COMPILER_FLAGS "Treat all warnings as errors" OFF)
option(LIBEMB_ENABLE_EXTRA_WARNINGS "Enable extended compiler warnings" OFF)
option(LIBEMB_COVERAGE_THRESHOLD "Minimum required code coverage percentage" 80)

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

# UndefinedBehaviorSanitizer configuration (Phase 3)
if(LIBEMB_ENABLE_UBSAN)
    if(NOT CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
        message(FATAL_ERROR "UndefinedBehaviorSanitizer requires Clang or GCC")
    endif()
    add_compile_options(-fsanitize=undefined)
    add_link_options(-fsanitize=undefined)
    message(STATUS "UndefinedBehaviorSanitizer enabled")
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

# Clang-Tidy configuration
if(LIBEMB_ENABLE_CLANG_TIDY)
    if(NOT CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        message(WARNING "Clang-Tidy works best with Clang compiler")
    endif()
    if(CMAKE_CROSSCOMPILING)
        message(WARNING "Clang-Tidy may not work well with cross-compilation")
    endif()

    find_program(CLANG_TIDY_EXE clang-tidy)
    if(CLANG_TIDY_EXE)
        set(CMAKE_CXX_CLANG_TIDY "${CLANG_TIDY_EXE};-checks=*,-fuchsia-*,-google-*,-llvm-*")
        message(STATUS "Clang-Tidy enabled: ${CLANG_TIDY_EXE}")
    else()
        message(WARNING "clang-tidy not found - analysis disabled")
    endif()
endif()

# Compiler-specific flags
if(MSVC)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Werror)
endif()

# Include CMake modules
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(Validation)
include(Doxygen)

# Initialize validation framework
libemb_detect_compiler()
libemb_enable_compiler_validation()

# Main library components
add_subdirectory(include/libemb/hal)
add_subdirectory(include/libemb/util)
add_subdirectory(src/driver)
add_subdirectory(src/protocol)

# Mock implementations (host only - uses STL)
if(NOT CMAKE_CROSSCOMPILING)
    add_subdirectory(port/mock)
endif()

# MCU-Specific Ports
# ABOV A31G123 (Phase 4) - Only for cross-compilation
if(CMAKE_CROSSCOMPILING)
    add_subdirectory(port/abov_a31g123)
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

# Benchmarks
if(LIBEMB_BUILD_BENCHMARKS)
    if(CMAKE_CROSSCOMPILING)
        message(FATAL_ERROR "Benchmarks cannot be built for cross-compilation")
    endif()
    add_subdirectory(benchmark)
endif()

# Documentation with Doxygen
if(LIBEMB_BUILD_DOCS)
    libemb_add_doxygen_docs(
        PROJECT_NAME "libemb"
        PROJECT_VERSION "${PROJECT_VERSION}"
        OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/docs"
    )
endif()

# Print configuration summary (Phase 3 enhanced)
libemb_print_validation_summary()
