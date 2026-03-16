# Driver implementations

include(${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/LibembConfig.cmake)

# Register individual drivers
# add_libemb_driver() automatically:
# - Collects .cpp from src/driver/<name>/
# - Collects .hpp from include/libemb/driver/<name>/
# - Links HAL and Util dependencies
# - Applies common compiler flags
add_libemb_driver(ssd1306)

# Future drivers can be added as:
# add_libemb_driver(bmp280)
# add_libemb_driver(mpu6050)

# Create aggregate library linking all drivers
add_library(libemb_driver INTERFACE)
add_library(libemb::driver ALIAS libemb_driver)

target_link_libraries(libemb_driver INTERFACE
    libemb::driver::ssd1306
    # libemb::driver::bmp280 (will be added as drivers are added)
    # libemb::driver::mpu6050
)
