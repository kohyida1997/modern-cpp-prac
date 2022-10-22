cmake_minimum_required(VERSION 3.10)

project(RuleOfFive)

add_executable(Main main.cpp)
target_link_libraries(Main PUBLIC Widget)
add_subdirectory(widget)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)