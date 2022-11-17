cmake_minimum_required(VERSION 3.5)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

project(small_buffer)

############################################################
# Create a library
############################################################

#Generate the shared library from the library sources
add_library(small_buffer INTERFACE)
add_library(small_buffer::lib ALIAS small_buffer)

target_include_directories(small_buffer
    INTERFACE 
        ${PROJECT_SOURCE_DIR}/include
)

############################################################
# Create an executable
############################################################

# Add an executable with the above sources
add_executable(main_bin
    src/main.cpp
)

target_link_libraries(main_bin
    PUBLIC 
        small_buffer::lib
)
