add_library(function function.cpp)
target_include_directories(function PRIVATE ${LOX_CPP_SOURCE_DIR}/include)

add_library(return return.cpp)
target_include_directories(return PRIVATE ${LOX_CPP_SOURCE_DIR}/include)

target_link_libraries(function PRIVATE return)
