cmake_minimum_required(VERSION 3.12)
cmake_policy(SET CMP0167 OLD) # find boost

project(
  WorkflowMockup
  VERSION 0.1.0
  LANGUAGES CXX)

find_package(Boost REQUIRED COMPONENTS graph program_options log)

find_package(Catch2 3 REQUIRED)
include(CTest)
include(Catch)

add_subdirectory(deps)

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
target_link_libraries(taskflow_demo PRIVATE Boost::program_options taskflow mockup Boost::log)

add_executable(mockup_tests tests/read_graph.test.cpp)
target_link_libraries(mockup_tests PRIVATE mockup Catch2::Catch2WithMain)

catch_discover_tests(mockup_tests)