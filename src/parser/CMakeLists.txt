add_library(parser parser.cpp)
target_link_libraries(parser PRIVATE ast)
target_include_directories(parser PRIVATE ${LOX_CPP_SOURCE_DIR}/include)
