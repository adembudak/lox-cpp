add_library(ast ast/stmt.cpp ast/expr.cpp)
target_link_libraries(ast PRIVATE Boost::boost)
target_include_directories(ast PRIVATE ${LOX_CPP_SOURCE_DIR}/include)

add_library(astprinter astprinter/astprinter.cpp)
target_link_libraries(astprinter PRIVATE Boost::boost)
target_include_directories(astprinter PRIVATE ${LOX_CPP_SOURCE_DIR}/include)

add_library(function callable/function.cpp)
target_include_directories(function PRIVATE ${LOX_CPP_SOURCE_DIR}/include)

add_library(return callable/return.cpp)
target_include_directories(return PRIVATE ${LOX_CPP_SOURCE_DIR}/include)
target_link_libraries(function PRIVATE return)

add_library(environment environment/environment.cpp)
target_include_directories(environment PRIVATE ${LOX_CPP_SOURCE_DIR}/include)

add_library(error error/error.cpp)
target_include_directories(error PRIVATE ${LOX_CPP_SOURCE_DIR}/include)

add_library(interpreter interpreter/interpreter.cpp)
target_link_libraries(interpreter PRIVATE ast)
target_include_directories(interpreter PRIVATE ${LOX_CPP_SOURCE_DIR}/include)

add_library(parser parser/parser.cpp)
target_link_libraries(parser PRIVATE ast)
target_include_directories(parser PRIVATE ${LOX_CPP_SOURCE_DIR}/include)

add_library(scanner scanner/scanner.cpp)
target_include_directories(scanner PRIVATE ${LOX_CPP_SOURCE_DIR}/include)

add_library(lox INTERFACE)
target_link_libraries(lox INTERFACE scanner parser ast interpreter environment function astprinter error)
add_library(lox::lox ALIAS lox)

