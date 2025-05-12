# Taskflow-demonstrator

Demonstrator for event-processing application framework scheduler using [taskflow](https://taskflow.github.io/)

## Dependencies

The project has following dependencies:

- taskflow
- boost
    - program_options - used for the entry-point application
    - graph -  used when scheduling mockups workflows for reading workflow description stored in GraphML file
## Getting started

Building:

```
git clone
cd taskflow-fwk
git submodule update --init --recursive
mkdir build && cd build
cmake ..
make
```

Running demonstrator:

```
./taskflow_demo --threads 6 --slots 4 --event-count 4 --logs-trace trace.json --dfg ../data/ATLAS/q449/df.graphml
```
