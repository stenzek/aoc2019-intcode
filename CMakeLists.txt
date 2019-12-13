project(aoc2019-intcode CXX)
cmake_minimum_required(VERSION 3.13)

add_library(intcode intcode.h intcode.cpp scope_timer.cpp)
set_property(TARGET intcode PROPERTY CXX_STANDARD 17)

add_executable(day9 day9.cpp)
set_property(TARGET day9 PROPERTY CXX_STANDARD 17)
target_link_libraries(day9 intcode)

add_executable(day11 day11.cpp)
set_property(TARGET day11 PROPERTY CXX_STANDARD 17)
target_link_libraries(day11 intcode)

add_executable(day13 day13.cpp)
set_property(TARGET day13 PROPERTY CXX_STANDARD 17)
target_link_libraries(day13 intcode)

add_executable(day13-part2 day13-part2.cpp)
set_property(TARGET day13-part2 PROPERTY CXX_STANDARD 17)
target_link_libraries(day13-part2 intcode)
