cmake_minimum_required(VERSION 3.25)
include(FetchContent)

# Tell the user what's happening.
message(STATUS "Building taskflow as part of local build.")

# Declare where to get taskflow from.
set(WORKFLOWMOCKUP_TASKFLOW_SOURCE
    "URL;https://github.com/taskflow/taskflow/archive/refs/tags/v4.0.0.tar.gz;URL_MD5;7b96dfe83bafe8749365b944d44c0ab7"
    CACHE STRING "Source for taskflow, when built as part of this project")

mark_as_advanced(WORKFLOWMOCKUP_TASKFLOW_SOURCE)
FetchContent_Declare(taskflow SYSTEM ${WORKFLOWMOCKUP_TASKFLOW_SOURCE})

# Turn off build tests, examples, and docs.
set(TF_BUILD_BENCHMARKS OFF)
set(TF_BUILD_PROFILER OFF)
set(TF_BUILD_CUDA OFF)
set(TF_BUILD_TESTS OFF)
set(TF_BUILD_EXAMPLES OFF)
set(TF_BUILD_MODULES OFF)

# Get it into the current directory.
FetchContent_MakeAvailable(taskflow)
