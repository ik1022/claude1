# Ssd1306 OLED Display Driver Example

add_libemb_example(display_driver main.cpp)

# Link the Ssd1306 driver implementation
target_link_libraries(display_driver PRIVATE libemb::driver::ssd1306)

# Add project root include path for port/mock access
target_include_directories(display_driver PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../..)
