cmake_minimum_required(VERSION 3.5)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

OPTION(DEBUG "Overload New and Delete Operators" OFF)
IF(DEBUG)
    # Overloading New and Delte will cause Valgrind to break
    add_definitions(-DOVERLOAD_NEW_DELETE=1)
ENDIF(DEBUG)

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

# Add compile definitions

# Add an executable with the above sources
add_executable(main_bin
    src/main.cpp
    src/Utils.h
)

target_link_libraries(main_bin
    PUBLIC 
        small_buffer::lib
)
