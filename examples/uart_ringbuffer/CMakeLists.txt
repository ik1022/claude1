add_executable(uart_ringbuffer_demo main.cpp)

# Include libemb headers
target_include_directories(uart_ringbuffer_demo
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(uart_ringbuffer_demo
    PRIVATE
    libemb::util
    libemb::hal
)

target_compile_features(uart_ringbuffer_demo PRIVATE cxx_std_17)
