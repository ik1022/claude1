# Example applications demonstrating libemb library

include(${CMAKE_CURRENT_SOURCE_DIR}/../cmake/LibembConfig.cmake)

# Basic RingBuffer and StaticVector usage
add_subdirectory(01_basic_ringbuffer)

# UART integration with RingBuffer (host builds only - uses STL)
if(NOT CMAKE_CROSSCOMPILING)
    add_subdirectory(02_uart_ringbuffer)
endif()

# Ssd1306 OLED display driver example (host builds only - uses Mock I2C)
if(NOT CMAKE_CROSSCOMPILING)
    add_subdirectory(03_display_driver)
endif()

# Sensor integration example - Multi-component system (host builds only)
if(NOT CMAKE_CROSSCOMPILING)
    add_subdirectory(04_sensor_integration)
endif()
