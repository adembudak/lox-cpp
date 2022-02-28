add_library(ast stmt.cpp expr.cpp)
target_link_libraries(ast PRIVATE Boost::boost)
target_include_directories(ast PRIVATE ${LOX_CPP_SOURCE_DIR}/include)
