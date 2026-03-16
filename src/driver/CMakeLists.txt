# Driver implementations

add_library(libemb_driver STATIC)
add_library(libemb::driver ALIAS libemb_driver)

target_sources(libemb_driver
    PRIVATE
    ssd1306/Ssd1306.cpp
    PUBLIC
    FILE_SET HEADERS
    BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/../../include
    FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include/libemb/driver/ssd1306/Ssd1306.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include/libemb/driver/ssd1306/Ssd1306Types.hpp
)

target_include_directories(libemb_driver
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include
)

target_link_libraries(libemb_driver
    PUBLIC
    libemb::hal
    libemb::util
)

target_compile_features(libemb_driver PRIVATE cxx_std_17)
