# ABOV A31G123 MCU Port for libemb
# Provides HAL implementations for GPIO, UART, I2C, SPI peripherals

include(${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/LibembConfig.cmake)

# ============================================================================
# Port Library Definition
# ============================================================================

add_library(libemb_port_abov_a31g123 STATIC)
add_library(libemb::port::abov_a31g123 ALIAS libemb_port_abov_a31g123)

# ============================================================================
# Source Files
# ============================================================================

target_sources(libemb_port_abov_a31g123
    PRIVATE
    # HAL Implementations
    hal/A31G123Gpio.cpp
    hal/A31G123Uart.cpp
    hal/A31G123I2c.cpp
    hal/A31G123Spi.cpp

    # Common utilities
    common/SystemClock.cpp
)

# ============================================================================
# Include Directories
# ============================================================================

target_include_directories(libemb_port_abov_a31g123
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/..
    INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include
)

# ============================================================================
# Compiler Settings
# ============================================================================

# Set C++ standard for this target
set_target_properties(libemb_port_abov_a31g123 PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

# ============================================================================
# Link Dependencies
# ============================================================================

target_link_libraries(libemb_port_abov_a31g123
    PUBLIC
    libemb::hal          # Interface definitions
    libemb::util         # RingBuffer, StaticVector
    PRIVATE
    # Any vendor-specific libraries would go here
)

# ============================================================================
# Compiler Flags
# ============================================================================

# Platform-specific macro
target_compile_definitions(libemb_port_abov_a31g123
    PRIVATE
    PLATFORM_ABOV_A31G123=1
)

# ============================================================================
# Linker Script
# ============================================================================

# Linker script path (used by examples and firmware)
set(ABOV_A31G123_LINKER_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/linker/A31G123.ld"
    CACHE FILEPATH "ABOV A31G123 linker script path")

# ============================================================================
# Optional: Startup Code
# ============================================================================

# If custom startup code is provided:
# target_sources(libemb_port_abov_a31g123 PRIVATE common/Startup.s)

# Set ASM language if using Startup.s
# enable_language(ASM)

message(STATUS "✓ ABOV A31G123 port configured")
message(STATUS "  - HAL: GPIO, UART, I2C, SPI")
message(STATUS "  - Clock: 70 MHz (Cortex-M0+)")
message(STATUS "  - Memory: 128KB Flash, 32KB RAM")
message(STATUS "  - Linker: ${ABOV_A31G123_LINKER_SCRIPT}")
