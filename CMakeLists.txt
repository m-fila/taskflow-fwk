cmake_minimum_required(VERSION 3.12)
cmake_policy(SET CMP0167 OLD) # find boost

project(
  WorkflowMockup
  VERSION 0.1.0
  LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20 CACHE STRING "")
set(CMAKE_CXX_EXTENSIONS FALSE CACHE BOOL "Disable C++ extensions" )
set(CMAKE_CXX_STANDARD_REQUIRED ON)
if(CMAKE_CXX_STANDARD LESS 20)
  message(FATAL_ERROR "Unsupported C++ standard: ${CMAKE_CXX_STANDARD}. Must be at least C++20.")
endif()

find_package(Boost REQUIRED COMPONENTS graph program_options log)

option(USE_SYSTEM_LIBS "Use system libraries by default" FALSE)

option(BUILD_TASKFLOW "Build the taskflow sources included in this project" TRUE)

if (USE_SYSTEM_LIBS)
  set(USE_SYSTEM_TASKFLOW_DEFAULT ON)
else()
  set(USE_SYSTEM_TASKFLOW_DEFAULT OFF)
endif()
option(USE_SYSTEM_TASKFLOW
  "Pick up an existing installation of taskflow from the build environment"
  ${USE_SYSTEM_TASKFLOW_DEFAULT})
unset(USE_SYSTEM_TASKFLOW_DEFAULT)

if(BUILD_TASKFLOW)
  if(USE_SYSTEM_TASKFLOW)
    find_package(taskflow 4.0.0 REQUIRED)
  else()
    add_subdirectory(extern/taskflow)
  endif()
endif()

set(sources
    src/graph_representation.cpp
    src/read_graph.cpp
    src/cpu_cruncher.cpp
)

add_library(mockup SHARED ${sources})
target_include_directories(
    mockup PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/>
                  $<INSTALL_INTERFACE:include/>)

target_link_libraries(mockup PUBLIC Boost::graph)

add_executable(taskflow_demo bin/taskflow_demo.cpp)
target_link_libraries(taskflow_demo PRIVATE Boost::program_options Taskflow::Taskflow mockup Boost::log)


include(CTest)
if(BUILD_TESTING)
  enable_testing()
  add_subdirectory(tests)
endif()

