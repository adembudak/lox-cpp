add_subdirectory(scanner)
add_subdirectory(ast)
add_subdirectory(parser)
add_subdirectory(interpreter)
add_subdirectory(environment)
add_subdirectory(callable)

add_library(lox INTERFACE)
target_link_libraries(lox INTERFACE scanner parser ast interpreter environment function)
add_library(lox::lox ALIAS lox)

