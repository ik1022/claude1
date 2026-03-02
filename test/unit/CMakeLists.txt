# Unit tests using Google Test

# Helper macro to create test executables
macro(add_libemb_test TEST_NAME TEST_FILE)
    add_executable(${TEST_NAME} ${TEST_FILE})

    target_link_libraries(${TEST_NAME}
        PRIVATE
        gtest
        gtest_main
        libemb::hal
        libemb::util
        libemb::driver
        libemb::port::mock
    )

    target_include_directories(${TEST_NAME}
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../../include
    )

    target_compile_features(${TEST_NAME} PRIVATE cxx_std_17)

    # Register test with CTest
    add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
endmacro()

# Register test files
add_libemb_test(test_RingBuffer test_RingBuffer.cpp)
add_libemb_test(test_Ssd1306 test_Ssd1306.cpp)
