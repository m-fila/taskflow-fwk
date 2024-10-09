add_library(taskflow INTERFACE )
target_include_directories(taskflow INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/taskflow/>)