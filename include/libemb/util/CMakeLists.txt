# Utility module - header-only library with data structures and algorithms

add_library(libemb_util INTERFACE)
add_library(libemb::util ALIAS libemb_util)

target_include_directories(libemb_util
    INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)

# Interface sources for IDE/documentation
target_sources(libemb_util
    INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/RingBuffer.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/StaticVector.hpp
)

# Set C++ standard
target_compile_features(libemb_util INTERFACE cxx_std_17)
