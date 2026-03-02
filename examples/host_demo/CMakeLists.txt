# Host-side examples for libemb

add_executable(host_demo main.cpp)

target_link_libraries(host_demo
    PRIVATE
    libemb::util
    libemb::hal
)

target_include_directories(host_demo
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include
)

target_compile_features(host_demo PRIVATE cxx_std_17)
