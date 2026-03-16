# Mock HAL port - test implementations of HAL interfaces

add_library(libemb_port_mock INTERFACE)
add_library(libemb::port::mock ALIAS libemb_port_mock)

target_include_directories(libemb_port_mock
    INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/..
    ${CMAKE_CURRENT_SOURCE_DIR}/../..
)

target_link_libraries(libemb_port_mock
    INTERFACE
    libemb::hal
)

# Interface sources for IDE/documentation
target_sources(libemb_port_mock
    INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/MockGpio.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/MockUart.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/hal/MockI2c.hpp
)

# Set C++ standard
target_compile_features(libemb_port_mock INTERFACE cxx_std_17)
