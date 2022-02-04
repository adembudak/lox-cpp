add_subdirectory(scanner)
add_subdirectory(ast)
add_subdirectory(parser)
add_subdirectory(prettyprint)

add_library(lox INTERFACE)
target_link_libraries(lox INTERFACE scanner parser prettyprint)
add_library(lox::lox ALIAS lox)

