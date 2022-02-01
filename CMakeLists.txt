cmake_minimum_required(VERSION 3.22.0)

project(lox-cpp LANGUAGES CXX)
include(CTest)

option(WITH_TESTS "Build tests" ON)

if(WITH_TESTS)
  enable_testing()
  find_package(magic_enum QUIET REQUIRED CONFIG)
  add_subdirectory(tests)
endif()

find_package(Boost QUIET REQUIRED CONFIG)

add_subdirectory(src)

add_executable(lox-cli main.cpp)
target_include_directories(lox-cli PRIVATE include)
target_link_libraries(lox-cli PRIVATE lox::lox)
