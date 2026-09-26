add_library(book
    src/order_book.cpp
)

target_include_directories(book PUBLIC include)

add_executable(book_tests
    tests/order_book_test.cpp
)

target_link_libraries(book_tests
    PRIVATE
        book
        gtest_main
)

include(GoogleTest)
gtest_discover_tests(book_tests)