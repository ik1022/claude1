# ABOV A31G123 Demo Applications
# Set of example programs demonstrating HAL usage

include(${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/LibembConfig.cmake)

# ============================================================================
# Demo 1: LED Blink (GPIO)
# ============================================================================

set(BLINK_SOURCES blink.cpp)

add_arm_executable(a31g123_blink ${BLINK_SOURCES})

target_include_directories(a31g123_blink PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(a31g123_blink
    PRIVATE
    libemb::hal
    libemb::port::abov_a31g123
)

# Linker script
target_link_options(a31g123_blink PRIVATE -T${ABOV_A31G123_LINKER_SCRIPT})

message(STATUS "✓ a31g123_blink example configured")

# ============================================================================
# Demo 2: UART Echo (Serial Communication)
# ============================================================================

set(UART_SOURCES uart_echo.cpp)

add_arm_executable(a31g123_uart_echo ${UART_SOURCES})

target_include_directories(a31g123_uart_echo PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(a31g123_uart_echo
    PRIVATE
    libemb::hal
    libemb::port::abov_a31g123
)

target_link_options(a31g123_uart_echo PRIVATE -T${ABOV_A31G123_LINKER_SCRIPT})

message(STATUS "✓ a31g123_uart_echo example configured")

# ============================================================================
# Demo 3: I2C Display (I2C Communication)
# ============================================================================

set(I2C_SOURCES i2c_display.cpp)

add_arm_executable(a31g123_i2c_display ${I2C_SOURCES})

target_include_directories(a31g123_i2c_display PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(a31g123_i2c_display
    PRIVATE
    libemb::hal
    libemb::driver
    libemb::port::abov_a31g123
)

target_link_options(a31g123_i2c_display PRIVATE -T${ABOV_A31G123_LINKER_SCRIPT})

message(STATUS "✓ a31g123_i2c_display example configured")
