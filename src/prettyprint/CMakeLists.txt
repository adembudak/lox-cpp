add_library(prettyprint prettyprint.cpp)
target_link_libraries(prettyprint PRIVATE ast Boost::boost)
target_include_directories(prettyprint PRIVATE ${PROJECT_SOURCE_DIR}/include)
