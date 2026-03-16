# Driver module tests

add_libemb_test(test_Ssd1306 test_Ssd1306.cpp)
target_link_libraries(test_Ssd1306 PRIVATE libemb::test::fixtures)
target_include_directories(test_Ssd1306 PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../..)
