cmake_minimum_required(VERSION 3.14)
project(Metro)

set(CMAKE_CXX_STANDARD 17)
configure_file(cmake_variables.h.in ${PROJECT_BINARY_DIR}/cmake_variables.h @ONLY)
include_directories(Metro ${CMAKE_CURRENT_BINARY_DIR})

find_package (Threads)

add_executable(Metro main.cpp Tile.cpp Tile.h Directions.h Analyzer.cpp Analyzer.h SpecPermutations.h Game.cpp Solver.cpp)
target_link_libraries (Metro ${CMAKE_THREAD_LIBS_INIT})