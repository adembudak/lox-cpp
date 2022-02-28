add_library(prettyprint prettyprint.cpp)
target_link_libraries(prettyprint PRIVATE ast Boost::boost)
target_include_directories(prettyprint PRIVATE ${LOX_CPP_SOURCE_DIR}/include)
