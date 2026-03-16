# Unit tests using Google Test

include(${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/LibembConfig.cmake)

# Add test subdirectories
add_subdirectory(../fixtures fixtures)
add_subdirectory(util)
add_subdirectory(driver)
