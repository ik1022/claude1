# Protocol module - for future protocol implementations (CAN, MQTT, Modbus, etc.)

add_library(libemb_protocol INTERFACE)
add_library(libemb::protocol ALIAS libemb_protocol)

target_include_directories(libemb_protocol
    INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include
)

target_link_libraries(libemb_protocol
    INTERFACE
    libemb::hal
    libemb::util
)

target_compile_features(libemb_protocol INTERFACE cxx_std_17)

# Placeholder message
message(STATUS "libemb::protocol module available for future expansion")
