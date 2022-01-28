add_library(scanner scanner.cpp)
target_link_libraries(scanner PRIVATE magic_enum::magic_enum)
target_include_directories(scanner PRIVATE ${PROJECT_SOURCE_DIR}/include)
