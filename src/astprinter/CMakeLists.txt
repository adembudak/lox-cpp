add_library(astprinter astprinter.cpp)
target_link_libraries(astprinter PRIVATE Boost::boost)
target_include_directories(astprinter PRIVATE ${LOX_CPP_SOURCE_DIR}/include)
