cmake_minimum_required(VERSION 3.5)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

project(allocator_lib)

############################################################
# Create a library
############################################################

#Generate the shared library from the library sources
add_library(allocator_lib SHARED 
    src/BasicAllocator.cpp
    src/BasicArena.cpp
)
add_library(basicallocator::lib ALIAS allocator_lib)

target_include_directories(allocator_lib
    PUBLIC 
        ${PROJECT_SOURCE_DIR}/include
)

############################################################
# Create an executable
############################################################

# Add an executable with the above sources
add_executable(allocator_bin
    src/main.cpp
)

target_link_libraries(allocator_bin
    PRIVATE 
        basicallocator::lib
)