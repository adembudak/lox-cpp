cmake_minimum_required(VERSION 3.22.0)

project(lox-cpp LANGUAGES CXX)

include(CTest)
include(CMakeDependentOption)

option(WITH_TESTS "Build tests" ON)
cmake_dependent_option(WITH_TESTS "" ON BUILD_TESTING OFF)

if(WITH_TESTS)
  enable_testing()
  find_package(Catch2 QUIET REQUIRED CONFIG)

  add_subdirectory(tests)
endif()

find_package(Boost QUIET REQUIRED CONFIG)

add_subdirectory(src)
add_subdirectory(cli)

