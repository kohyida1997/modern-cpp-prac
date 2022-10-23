cmake_minimum_required(VERSION 3.5)

project(alignment_lib)

############################################################
# Create a library
############################################################

#Generate the shared library from the library sources
add_library(alignment_lib SHARED 
    src/Alignment.cpp
)
add_library(alignment::lib ALIAS alignment_lib)

target_include_directories(alignment_lib
    PUBLIC 
        ${PROJECT_SOURCE_DIR}/include
)

############################################################
# Create an executable
############################################################

# Add an executable with the above sources
add_executable(alignment_bin
    src/main.cpp
)

# link the new hello_library target with the hello_binary target
target_link_libraries(alignment_bin
    PRIVATE 
        alignment::lib
)