add_library(parser parser.cpp)
target_link_libraries(parser PUBLIC ast)
target_include_directories(parser PRIVATE ${PROJECT_SOURCE_DIR}/include)
