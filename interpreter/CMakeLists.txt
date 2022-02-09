add_executable(lox-cli main.cpp)
target_include_directories(lox-cli PRIVATE include)
target_link_libraries(lox-cli PRIVATE lox::lox)
