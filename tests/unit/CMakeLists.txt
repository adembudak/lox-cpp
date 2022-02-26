
add_executable(scanner_test scanner_test.cpp)
target_link_libraries(scanner_test PRIVATE lox::lox Catch2::Catch2 Catch2::Catch2WithMain)
target_include_directories(scanner_test PRIVATE ${PROJECT_SOURCE_DIR}/include)

add_test(scanner.test scanner_test)

