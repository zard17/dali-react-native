#!/bin/bash
# Memory Measurement Script for DALi Apps
# Measures memory usage of a process over time

APP_PATH="$1"
APP_NAME="$(basename $APP_PATH)"
DURATION=${2:-10}  # Default 10 seconds
INTERVAL=1  # Sample every 1 second

echo "=== Memory Measurement: $APP_NAME ==="
echo "Duration: ${DURATION}s, Interval: ${INTERVAL}s"
echo ""

# Start the app in background
$APP_PATH &
PID=$!

if [ -z "$PID" ]; then
    echo "Failed to start app"
    exit 1
fi

echo "Started $APP_NAME (PID: $PID)"
echo ""

# Wait for app to initialize
sleep 2

# Collect memory samples
declare -a RSS_SAMPLES
declare -a VSZ_SAMPLES

echo "Collecting memory samples..."
for ((i=0; i<$DURATION; i++)); do
    if ! kill -0 $PID 2>/dev/null; then
        echo "Process terminated early"
        break
    fi
    
    # Get memory info using ps (RSS in KB, VSZ in KB)
    MEM_INFO=$(ps -o rss=,vsz= -p $PID 2>/dev/null)
    if [ -n "$MEM_INFO" ]; then
        RSS=$(echo $MEM_INFO | awk '{print $1}')
        VSZ=$(echo $MEM_INFO | awk '{print $2}')
        RSS_SAMPLES+=($RSS)
        VSZ_SAMPLES+=($VSZ)
        echo "  Sample $((i+1)): RSS=${RSS}KB, VSZ=${VSZ}KB"
    fi
    sleep $INTERVAL
done

# Kill the app
kill $PID 2>/dev/null
wait $PID 2>/dev/null

echo ""
echo "=== Memory Summary for $APP_NAME ==="

# Calculate statistics
if [ ${#RSS_SAMPLES[@]} -gt 0 ]; then
    # Get final sample (stabilized memory)
    FINAL_RSS=${RSS_SAMPLES[-1]}
    FINAL_VSZ=${VSZ_SAMPLES[-1]}
    
    # Convert to MB
    FINAL_RSS_MB=$(echo "scale=2; $FINAL_RSS / 1024" | bc)
    FINAL_VSZ_MB=$(echo "scale=2; $FINAL_VSZ / 1024" | bc)
    
    echo "Final RSS (Physical Memory): ${FINAL_RSS}KB (${FINAL_RSS_MB}MB)"
    echo "Final VSZ (Virtual Memory):  ${FINAL_VSZ}KB (${FINAL_VSZ_MB}MB)"
    
    # Get detailed memory breakdown using vmmap (macOS)
    echo ""
    echo "=== Detailed Memory Breakdown ==="
    vmmap $PID 2>/dev/null | grep -E "^(TOTAL|MALLOC|TEXT|DATA|LINKEDIT|__DATA|__TEXT)" | head -20 || true
else
    echo "No memory samples collected"
fi

echo ""
echo "Measurement complete."
