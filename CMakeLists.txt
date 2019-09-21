cmake_minimum_required(VERSION 3.14)
project(Metro)

set(CMAKE_CXX_STANDARD 17)

add_executable(Metro main.cpp Tile.cpp Tile.h Constants.h Directions.h Game.cpp Game.h)