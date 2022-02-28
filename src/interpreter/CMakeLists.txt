add_library(interpreter interpreter.cpp)
target_link_libraries(interpreter PRIVATE ast)
target_include_directories(interpreter PRIVATE ${LOX_CPP_SOURCE_DIR}/include)
