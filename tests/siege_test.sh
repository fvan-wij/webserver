#!/bin/bash

URL="http://localhost:9090"
DURATION="30s"

echo "Testing Siege for $DURATION"
siege -c 200 -t $DURATION $URL &
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

echo "Checking memory usage for the next 5 seconds"
# Initialize a counter
count=1

# While loop to execute 5 times
while [ $count -le 5 ]; do
	MEMORY_USAGE=$(ps -o rss= -p $(pidof app))
	echo "Memory usage: ${MEMORY_USAGE} KB"
	sleep 1
    # Increment the counter
    ((count++))
done
exit 1

