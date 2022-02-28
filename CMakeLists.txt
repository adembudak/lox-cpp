cmake_minimum_required(VERSION 3.22.0)

project(lox-cpp LANGUAGES CXX)

include(CTest)
include(CMakeDependentOption)

option(WITH_TESTS "Build tests" ON)
cmake_dependent_option(WITH_TESTS "" ON BUILD_TESTING OFF)

set(LOX_CPP_SOURCE_DIR ${PROJECT_SOURCE_DIR})
set(LOX_CPP_BINARY_DIR ${PROJECT_BINARY_DIR})

if(WITH_TESTS)
  enable_testing()
  find_package(Catch2 QUIET REQUIRED CONFIG)

  add_subdirectory(tests)
endif()

find_package(Boost QUIET REQUIRED CONFIG)

add_subdirectory(src)

