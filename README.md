# Taskflow-demonstrator

Demonstrator for event-processing application framework scheduler using [taskflow](https://taskflow.github.io/)

## Dependencies

The project has following dependencies:

- taskflow v4.0.0
- boost
    - program_options - used for the entry-point application
    - graph -  used when scheduling mockups workflows for reading workflow description stored in GraphML file
    - log - used for configurable printouts
- catch2 for testing only

## Getting started

Building:

```
cmake -S . -Bbuild
cmake --build build
```

Running demonstrator:

```
./build/taskflow_demo --threads 6 --slots 4 --event-count 4 --trace-chrome trace.json --dfg data/ATLAS/q449/df.graphml
```
