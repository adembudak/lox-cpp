add_subdirectory(scanner)
add_subdirectory(ast)
add_subdirectory(parser)

add_library(lox INTERFACE)
target_link_libraries(lox INTERFACE scanner ast parser)
add_library(lox::lox ALIAS lox)

