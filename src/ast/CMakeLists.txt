add_library(ast ast.cpp)
target_link_libraries(ast PRIVATE Boost::boost)
target_include_directories(ast PRIVATE ${PROJECT_SOURCE_DIR}/include)
