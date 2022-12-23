cmake_minimum_required(VERSION 3.5)

project(trivia)

############################################################
# Create a library
############################################################

# Glob
file(GLOB_RECURSE SOURCES RELATIVE ${CMAKE_SOURCE_DIR} "src/*.cpp" "src/*.h")

# Remove main.cpp
list(REMOVE_ITEM SOURCES "src/main.cpp")

#Generate the shared library from the library sources
add_library(trivia_lib SHARED 
    ${SOURCES}
)
# add_library(alignment::lib ALIAS trivia_lib)

target_include_directories(trivia_lib
    PUBLIC 
        ${PROJECT_SOURCE_DIR}/include
)

############################################################
# Create an executable
############################################################

# Add an executable with the above sources
add_executable(trivia_bin
    src/main.cpp
)

# link the new hello_library target with the hello_binary target
target_link_libraries(trivia_bin
    PRIVATE 
        trivia_lib
)
