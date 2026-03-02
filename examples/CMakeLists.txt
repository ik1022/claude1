# Example applications demonstrating libemb library

add_subdirectory(host_demo)

# UART RingBuffer example only for host builds (uses STL)
if(NOT CMAKE_CROSSCOMPILING)
    add_subdirectory(uart_ringbuffer)
endif()
