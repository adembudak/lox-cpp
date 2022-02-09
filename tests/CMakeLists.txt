add_executable(scanner_test scanner_test.cpp)

target_link_libraries(scanner_test PRIVATE lox::lox magic_enum::magic_enum)
target_include_directories(scanner_test PRIVATE ${PROJECT_SOURCE_DIR}/include)
add_test(
  NAME scanner.test
  COMMAND scanner_test scanner.in
  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/tests)

add_executable(astprinter astprinter.cpp) 
target_link_libraries(astprinter PRIVATE lox::lox)
target_include_directories(astprinter PRIVATE ${PROJECT_SOURCE_DIR}/include)

