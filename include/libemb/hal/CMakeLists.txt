# HAL (Hardware Abstraction Layer) interface targets
# These are INTERFACE libraries - pure virtual interfaces with no implementation

add_library(libemb_hal INTERFACE)
add_library(libemb::hal ALIAS libemb_hal)

target_include_directories(libemb_hal
    INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)

# Interface sources for IDE/documentation
target_sources(libemb_hal
    INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/IGpio.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/IUart.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ISpi.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/II2c.hpp
)

# Set C++ standard
target_compile_features(libemb_hal INTERFACE cxx_std_17)
