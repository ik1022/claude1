# Performance benchmarks for libemb

include(FetchContent)

# Fetch Google Benchmark
FetchContent_Declare(
    benchmark
    GIT_REPOSITORY https://github.com/google/benchmark.git
    GIT_TAG v1.8.3
    GIT_SHALLOW TRUE
    TIMEOUT 30
)

set(BENCHMARK_DOWNLOAD_DEPENDENCIES ON CACHE BOOL "" FORCE)
set(BENCHMARK_ENABLE_GTEST_TESTS OFF CACHE BOOL "" FORCE)
set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(benchmark)

# Benchmark 1: RingBuffer performance
add_executable(bench_ringbuffer bench_ringbuffer.cpp)
target_link_libraries(bench_ringbuffer
    PRIVATE
    benchmark::benchmark
    libemb::util
    libemb::hal
)
target_include_directories(bench_ringbuffer PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../include)

# Benchmark 2: StaticVector performance
add_executable(bench_staticvector bench_staticvector.cpp)
target_link_libraries(bench_staticvector
    PRIVATE
    benchmark::benchmark
    libemb::util
)
target_include_directories(bench_staticvector PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../include)

# Benchmark 3: Driver performance
add_executable(bench_driver bench_driver.cpp)
target_link_libraries(bench_driver
    PRIVATE
    benchmark::benchmark
    libemb::driver
    libemb::port::mock
)
target_include_directories(bench_driver PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../include)

# Set common flags
foreach(target bench_ringbuffer bench_staticvector bench_driver)
    target_compile_features(${target} PRIVATE cxx_std_17)
    target_compile_options(${target} PRIVATE -O3 -march=native)
endforeach()

message(STATUS "Benchmarks configured")
