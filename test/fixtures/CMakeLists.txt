# Test fixtures library - Mock HAL implementations and test helpers

add_library(libemb_test_fixtures INTERFACE)
add_library(libemb::test::fixtures ALIAS libemb_test_fixtures)

target_include_directories(libemb_test_fixtures
    INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/../..)

# Mock HAL depends on HAL interfaces
target_link_libraries(libemb_test_fixtures
    INTERFACE libemb::hal)
