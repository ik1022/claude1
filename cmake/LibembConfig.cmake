# LibembConfig.cmake - libemb build system configuration
# Provides reusable macros for drivers, tests, examples, and ports

#
# libemb_set_common_flags(TARGET SCOPE)
#
# Apply common compiler flags to a target
#
# Arguments:
#   TARGET: Target name (e.g., libemb_driver_ssd1306)
#   SCOPE:  PUBLIC, PRIVATE, or INTERFACE
#
macro(libemb_set_common_flags TARGET SCOPE)
    # Compiler-specific warning flags
    if(MSVC)
        target_compile_options(${TARGET} ${SCOPE}
            /W4 /WX)
    else()
        target_compile_options(${TARGET} ${SCOPE}
            -Wall -Wextra -Wpedantic -Werror
            -Wno-unused-parameter)  # Needed for mock implementations
    endif()

    # Optimization flags for cross-compilation
    if(CMAKE_CROSSCOMPILING)
        target_compile_options(${TARGET} ${SCOPE}
            -flto                    # Link-time optimization
            -ffunction-sections      # Each function in separate section
            -fdata-sections)         # Each data item in separate section
        target_link_options(${TARGET} ${SCOPE}
            -flto
            -Wl,--gc-sections)       # Remove unused sections
    endif()

    # AddressSanitizer for host builds
    if(LIBEMB_ENABLE_ASAN AND NOT CMAKE_CROSSCOMPILING)
        target_compile_options(${TARGET} ${SCOPE}
            -fsanitize=address -fno-omit-frame-pointer)
        target_link_options(${TARGET} ${SCOPE}
            -fsanitize=address)
    endif()
endmacro()


#
# add_libemb_driver(DRIVER_NAME)
#
# Create and register a driver library
# Automatically collects source files from src/driver/<name>/
# and headers from include/libemb/driver/<name>/
#
# Arguments:
#   DRIVER_NAME: Name of the driver (e.g., ssd1306, bmp280)
#
# Creates:
#   - libemb_driver_<name> (STATIC library)
#   - libemb::driver::<name> (alias)
#
macro(add_libemb_driver DRIVER_NAME)
    # Validate source directory exists
    set(DRIVER_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/${DRIVER_NAME}")
    set(DRIVER_INC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../include/libemb/driver/${DRIVER_NAME}")

    if(NOT EXISTS "${DRIVER_SRC_DIR}")
        message(FATAL_ERROR "Driver source directory not found: ${DRIVER_SRC_DIR}")
    endif()

    if(NOT EXISTS "${DRIVER_INC_DIR}")
        message(FATAL_ERROR "Driver include directory not found: ${DRIVER_INC_DIR}")
    endif()

    # Target name
    set(DRIVER_LIB_NAME "libemb_driver_${DRIVER_NAME}")

    # Create static library
    add_library(${DRIVER_LIB_NAME} STATIC)
    add_library(libemb::driver::${DRIVER_NAME} ALIAS ${DRIVER_LIB_NAME})

    # Auto-collect source and header files
    file(GLOB DRIVER_SOURCES "${DRIVER_SRC_DIR}/*.cpp")
    file(GLOB DRIVER_HEADERS "${DRIVER_INC_DIR}/*.hpp")

    if(NOT DRIVER_SOURCES)
        message(FATAL_ERROR "No .cpp files found in ${DRIVER_SRC_DIR}")
    endif()

    # Register sources and headers
    target_sources(${DRIVER_LIB_NAME}
        PRIVATE ${DRIVER_SOURCES}
        PUBLIC FILE_SET HEADERS
        BASE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/../../include"
        FILES ${DRIVER_HEADERS})

    # Include paths
    target_include_directories(${DRIVER_LIB_NAME}
        PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/../../include")

    # Apply common flags
    libemb_set_common_flags(${DRIVER_LIB_NAME} PUBLIC)
    target_compile_features(${DRIVER_LIB_NAME} PUBLIC cxx_std_17)

    # Base dependencies (all drivers depend on HAL and Util)
    target_link_libraries(${DRIVER_LIB_NAME}
        PUBLIC libemb::hal libemb::util)

    # Register in global property
    get_property(ALL_DRIVERS GLOBAL PROPERTY LIBEMB_REGISTERED_DRIVERS)
    if(NOT ALL_DRIVERS)
        set(ALL_DRIVERS)
    endif()
    list(APPEND ALL_DRIVERS "libemb::driver::${DRIVER_NAME}")
    set_property(GLOBAL PROPERTY LIBEMB_REGISTERED_DRIVERS "${ALL_DRIVERS}")

    message(STATUS "Registered driver: libemb::driver::${DRIVER_NAME}")
endmacro()


#
# add_libemb_test(TEST_NAME TEST_FILE [EXTRA_DEPS...])
#
# Create a test executable and register with CTest
#
# Arguments:
#   TEST_NAME:   Test executable name (e.g., test_RingBuffer)
#   TEST_FILE:   Main test source file
#   EXTRA_DEPS:  Additional libraries to link (optional)
#
macro(add_libemb_test TEST_NAME TEST_FILE)
    # Create test executable
    add_executable(${TEST_NAME} ${TEST_FILE})

    # Link Google Test and core libraries
    target_link_libraries(${TEST_NAME}
        PRIVATE
        gtest
        gtest_main
        libemb::hal
        libemb::util
        libemb::driver
        libemb::port::mock)

    # Link test fixtures if available
    if(TARGET libemb::test::fixtures)
        target_link_libraries(${TEST_NAME} PRIVATE libemb::test::fixtures)
    endif()

    # Link any additional dependencies passed as arguments
    if(ARGC GREATER 2)
        target_link_libraries(${TEST_NAME} PRIVATE ${ARGN})
    endif()

    # Include paths (set when called from test/unit/*, paths resolve from there)
    # Handles both public API (include/) and internal structures (port/, test/)
    target_include_directories(${TEST_NAME}
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../../include
        ${CMAKE_CURRENT_SOURCE_DIR}/../..)

    # Apply common flags
    libemb_set_common_flags(${TEST_NAME} PRIVATE)
    target_compile_features(${TEST_NAME} PRIVATE cxx_std_17)

    # Register with CTest
    add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
    set_tests_properties(${TEST_NAME} PROPERTIES
        FAIL_REGULAR_EXPRESSION "FAILED"
        TIMEOUT 10)

    # Register in global property
    get_property(ALL_TESTS GLOBAL PROPERTY LIBEMB_REGISTERED_TESTS)
    if(NOT ALL_TESTS)
        set(ALL_TESTS)
    endif()
    list(APPEND ALL_TESTS "${TEST_NAME}")
    set_property(GLOBAL PROPERTY LIBEMB_REGISTERED_TESTS "${ALL_TESTS}")

    message(STATUS "Registered test: ${TEST_NAME}")
endmacro()


#
# add_libemb_example(EXAMPLE_NAME MAIN_FILE [EXTRA_DEPS...])
#
# Create an example executable
#
# Arguments:
#   EXAMPLE_NAME: Example executable name
#   MAIN_FILE:    Main source file
#   EXTRA_DEPS:   Additional libraries to link (optional)
#
macro(add_libemb_example EXAMPLE_NAME MAIN_FILE)
    # Create executable
    add_executable(${EXAMPLE_NAME} ${MAIN_FILE})

    # Include paths
    # When called from examples/<name>/CMakeLists.txt:
    # ${CMAKE_CURRENT_SOURCE_DIR} = examples/<name>
    # ../../include = project root's include directory
    target_include_directories(${EXAMPLE_NAME}
        PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../include)

    # Apply common flags
    libemb_set_common_flags(${EXAMPLE_NAME} PRIVATE)
    target_compile_features(${EXAMPLE_NAME} PRIVATE cxx_std_17)

    # Base dependencies
    target_link_libraries(${EXAMPLE_NAME} PRIVATE libemb::util libemb::hal)

    # Link additional dependencies if provided
    if(ARGC GREATER 2)
        target_link_libraries(${EXAMPLE_NAME} PRIVATE ${ARGN})
    endif()

    message(STATUS "Registered example: ${EXAMPLE_NAME}")
endmacro()


#
# add_libemb_port(PORT_NAME)
#
# Register an MCU port (for future use)
#
# Arguments:
#   PORT_NAME: Name of the port (e.g., stm32f4, nrf52)
#
macro(add_libemb_port PORT_NAME)
    set(PORT_LIB_NAME "libemb_port_${PORT_NAME}")
    add_library(${PORT_LIB_NAME} INTERFACE)
    add_library(libemb::port::${PORT_NAME} ALIAS ${PORT_LIB_NAME})

    message(STATUS "Registered port: libemb::port::${PORT_NAME}")
endmacro()


#
# print_libemb_summary()
#
# Print a summary of the libemb build configuration
#
function(print_libemb_summary)
    get_property(ALL_DRIVERS GLOBAL PROPERTY LIBEMB_REGISTERED_DRIVERS)
    get_property(ALL_TESTS GLOBAL PROPERTY LIBEMB_REGISTERED_TESTS)

    message(STATUS "")
    message(STATUS "======== libemb Build Summary ========")
    message(STATUS "Version: ${PROJECT_VERSION}")
    message(STATUS "CXX Standard: ${CMAKE_CXX_STANDARD}")
    message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")
    message(STATUS "Cross-compiling: ${CMAKE_CROSSCOMPILING}")
    message(STATUS "AddressSanitizer: ${LIBEMB_ENABLE_ASAN}")
    message(STATUS "")

    if(ALL_DRIVERS)
        message(STATUS "Registered Drivers (${CMAKE_VERSION}):")
        foreach(driver ${ALL_DRIVERS})
            message(STATUS "  ✓ ${driver}")
        endforeach()
    endif()

    if(ALL_TESTS)
        message(STATUS "Registered Tests:")
        foreach(test ${ALL_TESTS})
            message(STATUS "  ✓ ${test}")
        endforeach()
    endif()

    message(STATUS "")
    message(STATUS "======================================")
    message(STATUS "")
endfunction()
