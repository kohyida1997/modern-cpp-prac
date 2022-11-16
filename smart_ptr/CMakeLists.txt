cmake_minimum_required(VERSION 3.5)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

project(smart_lib)

############################################################
# Create a library
############################################################

#Generate the shared library from the library sources
add_library(smart_lib INTERFACE)
add_library(smart_ptr::lib ALIAS smart_lib)

target_include_directories(smart_lib
    INTERFACE 
        ${PROJECT_SOURCE_DIR}/include
)

############################################################
# Create an executable
############################################################

# Add an executable with the above sources
add_executable(smart_ptr_bin
    src/main.cpp
)

target_link_libraries(smart_ptr_bin
    PUBLIC 
        smart_ptr::lib
)