# CMake module for comprehensive build validation
# Supports strict compiler flags, multi-compiler validation, and code quality checks
#
# Usage:
#   include(Validation)
#   libemb_validate_compiler()
#   libemb_enable_compiler_validation()

# Compiler detection and validation
function(libemb_detect_compiler)
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
        set(LIBEMB_COMPILER_IS_GCC TRUE PARENT_SCOPE)
        message(STATUS "Detected GCC: ${CMAKE_CXX_COMPILER_VERSION}")

        # Check minimum version
        if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS "9.0")
            message(WARNING "GCC version ${CMAKE_CXX_COMPILER_VERSION} is older than recommended 9.0")
        endif()

    elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        set(LIBEMB_COMPILER_IS_CLANG TRUE PARENT_SCOPE)
        message(STATUS "Detected Clang: ${CMAKE_CXX_COMPILER_VERSION}")

        # Check minimum version
        if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS "12.0")
            message(WARNING "Clang version ${CMAKE_CXX_COMPILER_VERSION} is older than recommended 12.0")
        endif()

    elseif(MSVC)
        set(LIBEMB_COMPILER_IS_MSVC TRUE PARENT_SCOPE)
        message(STATUS "Detected MSVC: ${CMAKE_CXX_COMPILER_VERSION}")

    else()
        message(WARNING "Unknown compiler: ${CMAKE_CXX_COMPILER_ID}")
    endif()
endfunction()


# Apply strict compiler flags based on detected compiler
function(libemb_apply_strict_flags TARGET SCOPE)
    if(LIBEMB_COMPILER_IS_GCC)
        target_compile_options(${TARGET} ${SCOPE}
            # Treat warnings as errors
            -Werror

            # Standard warnings
            -Wall
            -Wextra
            -Wpedantic

            # Type safety
            -Wconversion
            -Wsign-conversion
            -Wdouble-promotion

            # Initialization and shadowing
            -Wuninitialized
            -Winit-self
            -Wshadow
            -Wshadow=local

            # Function and variable issues
            -Wwrite-strings
            -Wstrict-aliasing=2
            -Wcast-qual
            -Wcast-align=strict
            -Wuseless-cast

            # Format and logic
            -Wformat=2
            -Wlogical-op
            -Wduplicated-cond
            -Wduplicated-branches

            # Null pointer and floating point
            -Wnull-dereference
            -Wfloat-equal

            # Other important checks
            -Warray-bounds=2
            -Wattributes
            -Wno-deprecated-declarations
        )

    elseif(LIBEMB_COMPILER_IS_CLANG)
        target_compile_options(${TARGET} ${SCOPE}
            # Treat warnings as errors
            -Werror

            # Most warnings (with reasonable exclusions)
            -Weverything
            -Wno-c++98-compat
            -Wno-c++98-compat-pedantic
            -Wno-padded
            -Wno-weak-vtables
            -Wno-exit-time-destructors
            -Wno-global-constructors
            -Wno-reserved-identifier
            -Wno-disabled-macro-expansion

            # Additional documentation checks
            -Wdocumentation
            -Wdocumentation-unknown-command

            # Explicitly enable important checks
            -Wdouble-promotion
            -Wunused-exception-parameter
        )

    elseif(LIBEMB_COMPILER_IS_MSVC)
        target_compile_options(${TARGET} ${SCOPE}
            /WX              # Treat warnings as errors
            /W4              # Warning level 4
            /permissive-     # Strict standard conformance

            # Specific warning levels for MSVC
            /w14062          # Switch without default
            /w14263          # Member function hiding
            /w14287          # Unsigned/negative const compare
            /w14296          # Always true/false
            /w14311          # Pointer truncation
            /w14545          # Function call incomplete
            /w14546          # Function call incomplete arg
            /w14547          # Operator without effect
            /w14549          # Operator precedence
            /w14555          # Operator precedence
            /w14905          # Wide string to LPSTR
            /w14906          # String literal cast to LPWSTR
        )
    endif()
endfunction()


# Apply extra warning flags for enhanced validation
function(libemb_apply_extra_warnings TARGET SCOPE)
    if(LIBEMB_COMPILER_IS_GCC)
        target_compile_options(${TARGET} ${SCOPE}
            -Wstrict-overflow=5
            -Wformat-signedness
            -Wimplicit-fallthrough=5
            -Wunused-parameter
            -Wunused-variable
        )

    elseif(LIBEMB_COMPILER_IS_CLANG)
        target_compile_options(${TARGET} ${SCOPE}
            -Wunused-parameter
            -Wunused-variable
            -Wunused-exception-parameter
        )

    elseif(LIBEMB_COMPILER_IS_MSVC)
        target_compile_options(${TARGET} ${SCOPE}
            /w14189          # Local variable unused
            /w14100          # Unreferenced formal parameter
        )
    endif()
endfunction()


# Main validation function: enable compiler validation globally
function(libemb_enable_compiler_validation)
    message(STATUS "Enabling compiler validation...")

    # Detect compiler
    libemb_detect_compiler()

    # Check if strict flags should be enabled
    if(LIBEMB_STRICT_COMPILER_FLAGS)
        message(STATUS "Strict compiler flags: ENABLED")
        message(STATUS "  All warnings will be treated as errors")

        # Apply globally to all targets
        if(LIBEMB_COMPILER_IS_GCC OR LIBEMB_COMPILER_IS_CLANG)
            add_compile_options(-Werror)
        elseif(LIBEMB_COMPILER_IS_MSVC)
            add_compile_options(/WX)
        endif()

    else()
        message(STATUS "Strict compiler flags: DISABLED (warnings only)")
    endif()

    # Apply extra warnings if requested
    if(LIBEMB_ENABLE_EXTRA_WARNINGS)
        message(STATUS "Extra warnings: ENABLED")
        if(LIBEMB_COMPILER_IS_GCC OR LIBEMB_COMPILER_IS_CLANG)
            add_compile_options(-Wconversion -Wshadow -Wdouble-promotion)
        endif()
    else()
        message(STATUS "Extra warnings: DISABLED")
    endif()

    # Print compiler info
    message(STATUS "Compiler: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
    message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
endfunction()


# Validate a specific target's compilation
function(libemb_validate_target TARGET)
    # Check target exists
    if(NOT TARGET ${TARGET})
        message(FATAL_ERROR "Target '${TARGET}' does not exist")
    endif()

    # Get target type
    get_target_property(TARGET_TYPE ${TARGET} TYPE)
    message(STATUS "Validating ${TARGET_TYPE} target: ${TARGET}")

    # Verify target has public headers if it's a library
    if(TARGET_TYPE STREQUAL "INTERFACE_LIBRARY" OR TARGET_TYPE STREQUAL "STATIC_LIBRARY")
        get_target_property(INCLUDE_DIRS ${TARGET} INTERFACE_INCLUDE_DIRECTORIES)
        if(NOT INCLUDE_DIRS)
            message(WARNING "Target ${TARGET} has no public include directories")
        endif()
    endif()
endfunction()


# Print validation summary
function(libemb_print_validation_summary)
    message(STATUS "")
    message(STATUS "========== Validation Configuration ==========")
    message(STATUS "Compiler: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
    message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
    message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")
    message(STATUS "")
    message(STATUS "Validation Options:")
    message(STATUS "  Strict Flags: ${LIBEMB_STRICT_COMPILER_FLAGS}")
    message(STATUS "  Extra Warnings: ${LIBEMB_ENABLE_EXTRA_WARNINGS}")
    message(STATUS "  Clang-Tidy: ${LIBEMB_ENABLE_CLANG_TIDY}")
    message(STATUS "  Coverage: ${LIBEMB_ENABLE_COVERAGE}")
    message(STATUS "  ASAN: ${LIBEMB_ENABLE_ASAN}")
    message(STATUS "  UBSAN: ${LIBEMB_ENABLE_UBSAN}")
    message(STATUS "")
    message(STATUS "Build Components:")
    message(STATUS "  Tests: ${LIBEMB_BUILD_TESTS}")
    message(STATUS "  Examples: ${LIBEMB_BUILD_EXAMPLES}")
    message(STATUS "  Documentation: ${LIBEMB_BUILD_DOCS}")
    message(STATUS "  Benchmarks: ${LIBEMB_BUILD_BENCHMARKS}")
    message(STATUS "")
    message(STATUS "============================================")
endfunction()
