project(aoc2019-intcode CXX)
cmake_minimum_required(VERSION 3.13)

add_library(intcode intcode.h intcode.cpp scope_timer.cpp)
set_property(TARGET intcode PROPERTY CXX_STANDARD 17)

add_executable(day9 day9.cpp)
set_property(TARGET day9 PROPERTY CXX_STANDARD 17)
target_link_libraries(day9 intcode)
