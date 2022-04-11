########################
# #### functions ##### #

function(add_lox_library tgt)
  cmake_parse_arguments(PARSE_ARGV 1 prefix "PRIVATE;PUBLIC;INTERFACE;ALIAS" "ALIAS_NAME" "SOURCES;LINK;INCLUDE_DIRS")

  if(prefix_ALIAS)
    add_library(${prefix_ALIAS_NAME} ALIAS ${tgt})
  elseif (prefix_INTERFACE)
    add_library(${tgt} INTERFACE)
    target_link_libraries(${tgt} INTERFACE ${prefix_LINK})
  else()
    set(link)
    if(prefix_PUBLIC)
      set(link "PUBLIC")
    else()
      set(link "PRIVATE")
    endif()

    add_library(${tgt} ${prefix_SOURCES})
    target_link_libraries(${tgt} ${link} ${prefix_LINK})
    target_include_directories(${tgt} ${link} ${prefix_INCLUDE_DIRS} ${LOX_CPP_INCLUDE_DIR})
  endif()
endfunction()

function(add_lox_executable tgt)
  cmake_parse_arguments(PARSE_ARGV 1 prefix "PRIVATE;PUBLIC" "" "SOURCES;LINK")

  set(link)
  if(prefix_PUBLIC)
    set(link "PUBLIC")
  else()
    set(link "PRIVATE")
  endif()

  add_executable(${tgt} ${prefix_SOURCES})
  target_link_libraries(${tgt} ${link} ${prefix_LINK})
  target_include_directories(${tgt} ${link} ${prefix_INCLUDE_DIRS} ${LOX_CPP_INCLUDE_DIR})
endfunction()

# #################### #
########################

cmake_minimum_required(VERSION 3.22.0)

project(lox-cpp LANGUAGES CXX)

include(CTest)
include(CMakeDependentOption)

option(WITH_TESTS "Build tests" ON)
option(WITH_UNIT_TESTS "" OFF)

cmake_dependent_option(WITH_TESTS "" ON BUILD_TESTING OFF)
cmake_dependent_option(WITH_UNIT_TESTS "" ON WITH_TESTS OFF)

set(LOX_CPP_SRC_DIR ${PROJECT_SOURCE_DIR}/src)
set(LOX_CPP_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)
set(LOX_CPP_TEST_DIR ${PROJECT_SOURCE_DIR}/tests)

find_package(Boost QUIET REQUIRED CONFIG)
find_package(fmt QUIET REQUIRED CONFIG)

add_lox_library(literal SOURCES ${LOX_CPP_SRC_DIR}/literal.cpp)
add_lox_library(ast SOURCES ${LOX_CPP_SRC_DIR}/ast/stmt.cpp ${LOX_CPP_SRC_DIR}/ast/expr.cpp LINK Boost::boost)
add_lox_library(astprinter SOURCES ${LOX_CPP_SRC_DIR}/astprinter/astprinter.cpp LINK literal Boost::boost)
add_lox_library(function SOURCES ${LOX_CPP_SRC_DIR}/callable/function/function.cpp LINK return)
add_lox_library(return SOURCES ${LOX_CPP_SRC_DIR}/callable/function/return.cpp)
add_lox_library(class SOURCES ${LOX_CPP_SRC_DIR}/callable/class/class.cpp LINK instance)
add_lox_library(instance SOURCES ${LOX_CPP_SRC_DIR}/callable/class/instance.cpp)
add_lox_library(callable SOURCES ${LOX_CPP_SRC_DIR}/callable/callable.cpp LINK function class)
add_lox_library(environment SOURCES ${LOX_CPP_SRC_DIR}/environment/environment.cpp)
add_lox_library(error SOURCES ${LOX_CPP_SRC_DIR}/error/error.cpp LINK fmt::fmt)
add_lox_library(interpreter SOURCES ${LOX_CPP_SRC_DIR}/interpreter/interpreter.cpp LINK literal ast fmt::fmt)
add_lox_library(parser SOURCES ${LOX_CPP_SRC_DIR}/parser/parser.cpp LINK ast)
add_lox_library(scanner SOURCES ${LOX_CPP_SRC_DIR}/scanner/scanner.cpp)
add_lox_library(resolver SOURCES ${LOX_CPP_SRC_DIR}/resolver/resolver.cpp LINK ast Boost::boost)
add_lox_library(lox INTERFACE LINK scanner parser interpreter environment callable  astprinter error resolver)
add_lox_library(lox ALIAS ALIAS_NAME lox::lox)

if(WITH_TESTS)
  enable_testing()
  
  if(WITH_UNIT_TESTS)
    find_package(Catch2 QUIET REQUIRED CONFIG)
    add_lox_executable(scanner_test PRIVATE SOURCES ${LOX_CPP_TEST_DIR}/unit/scanner_test.cpp 
            LINK lox::lox Catch2::Catch2 Catch2::Catch2WithMain)
    add_test(scanner.test scanner_test)
  endif()

  add_lox_executable(lox-cli PRIVATE SOURCES ${LOX_CPP_TEST_DIR}/cli/main.cpp LINK lox::lox)
  macro(run_cli_for folder)
    file(GLOB programs LIST_DIRECTORIES FALSE "${LOX_CPP_TEST_DIR}/cli/test/${folder}/*.lox")
    foreach(program IN LISTS programs)
      set(test_name ${program})
      add_test(NAME ${test_name} COMMAND lox-cli ${program} WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
    endforeach()
  endmacro()

  run_cli_for(comments)
  run_cli_for(print)
  run_cli_for(nil)
  run_cli_for(if)
  run_cli_for(function)
endif()

