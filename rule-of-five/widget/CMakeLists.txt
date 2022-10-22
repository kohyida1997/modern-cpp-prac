add_library(
    Widget
    "Widget.h"
    "Widget.cpp"
)

target_include_directories(Widget PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")