#!/bin/bash
WORKFLOW="$(dirname $0)/../data/ATLAS/q449/df.graphml"
REPEAT=2
AFFINITY=0
THREADS_SEQ="2 4"

THREADS_PER_SLOT=2
EVENTS_PER_SLOT=2

EXEC="$(dirname $0)/../build/taskflow_demo"

for threads in ${THREADS_SEQ}; do
    concurrent=$((threads/THREADS_PER_SLOT))
    total=$((concurrent*EVENTS_PER_SLOT))
    output_file="timing_${threads}.csv"
    echo "Starting measurements for ${threads} threads, ${total} total events," \
        "${concurrent} concurrent events"
    CMD=(
      numactl --cpunodebind="${AFFINITY}" --membind="${AFFINITY}" "${EXEC}" --dfg "${WORKFLOW}"
      --threads="${threads}" --slots="${concurrent}" --event-count="${total}" --disable-logging
      --trials="${REPEAT}" --save-timing="${output_file}"
    )
    CMD=${CMD[*]}
    echo "Running command: ${CMD}"
    $CMD
done
