cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

project(RuleOfFive)


add_library(widget_lib SHARED 
    src/Widget.cpp
)
add_library(widget::lib ALIAS widget_lib)

target_include_directories(widget_lib
    PUBLIC 
        ${PROJECT_SOURCE_DIR}/widget
)

add_executable(widget_bin
    src/main.cpp
)

target_link_libraries(widget_bin
    PRIVATE 
        widget_lib
)

