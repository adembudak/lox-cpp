add_executable(lox-cli main.cpp)
target_link_libraries(lox-cli PRIVATE lox::lox)
target_include_directories(lox-cli PRIVATE ${PROJECT_SOURCE_DIR}/include)

