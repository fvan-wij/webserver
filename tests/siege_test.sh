#!/bin/bash

URL="http://localhost:9090"
DURATION="15s"

echo "Testing Siege for $DURATION"
siege -b -t $DURATION $URL &
SIEGE_PID=$!

# Function to monitor memory usage
monitor_memory_usage() {
    while kill -0 $SIEGE_PID 2> /dev/null; do
        # Print memory usage for the Siege process
		MEMORY_USAGE=$(ps -o rss= -p $(pidof app))
        echo "Memory usage: ${MEMORY_USAGE} KB"
        # Wait 2 seconds before the next check
        sleep 2
    done
}

# Run the memory monitor in the background
monitor_memory_usage &

# Wait for Siege to complete
wait $SIEGE_PID
echo "Siege test completed for $DURATION"
