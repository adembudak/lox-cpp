add_executable(lox-cli main.cpp)
target_link_libraries(lox-cli PRIVATE lox::lox)
target_include_directories(lox-cli PRIVATE ${PROJECT_SOURCE_DIR}/include)

macro(run_cli_for folder)
  file(GLOB programs LIST_DIRECTORIES FALSE "test/${folder}/*.lox")
  foreach(program IN LISTS programs)
    set(test_name ${program})
    add_test(NAME ${test_name} COMMAND lox-cli ${program} WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
  endforeach()
endmacro()

run_cli_for(comments)
run_cli_for(print)

