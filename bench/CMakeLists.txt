add_library(bench
    src/timer.cpp
    src/histogram.cpp
)

target_include_directories(bench PUBLIC include)

add_executable(bench_tests
    tests/histogram_test.cpp
)

target_link_libraries(bench_tests
    PRIVATE
        bench
        gtest_main
)

include(GoogleTest)
gtest_discover_tests(bench_tests)