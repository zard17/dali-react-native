#!/bin/bash
#
# DALi React Native Memory Benchmark Script
# Runs memory tests for native DALi and React Native + DALi
#
# Usage: ./scripts/memory-benchmark.sh [runs]
#   runs: Number of runs per test (default: 3)
#

set -e

# Configuration
PROJECT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BUILD_DIR="$PROJECT_DIR/build"
RUNS=${1:-3}
STABILIZE_TIME=5  # seconds to wait for app to stabilize
CLEANUP_TIME=2    # seconds between runs

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
BLUE='\033[0;34m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Arrays to store results
declare -a NATIVE_VIEW_RESULTS
declare -a NATIVE_IMAGE_RESULTS
declare -a RN_VIEW_RESULTS
declare -a RN_IMAGE_RESULTS

echo -e "${BLUE}=======================================${NC}"
echo -e "${BLUE}  DALi React Native Memory Benchmark${NC}"
echo -e "${BLUE}=======================================${NC}"
echo ""
echo -e "Project: $PROJECT_DIR"
echo -e "Runs per test: $RUNS"
echo -e "Stabilize time: ${STABILIZE_TIME}s"
echo ""

# Helper function to kill processes
cleanup() {
    pkill -f native-benchmark-views 2>/dev/null || true
    pkill -f native-benchmark-images 2>/dev/null || true
    pkill -f dali-rn-demo 2>/dev/null || true
}

# Helper function to get RSS of a process
get_rss() {
    local process_name=$1
    ps -o rss= -p $(pgrep -f "$process_name" 2>/dev/null) 2>/dev/null | tr -d ' '
}

# Helper function to calculate average
calc_average() {
    local arr=("$@")
    local sum=0
    local count=${#arr[@]}
    for val in "${arr[@]}"; do
        sum=$((sum + val))
    done
    echo $((sum / count))
}

# Cleanup any existing processes
cleanup

# Check if build exists
if [ ! -f "$BUILD_DIR/native-benchmark-views" ] || [ ! -f "$BUILD_DIR/native-benchmark-images" ] || [ ! -f "$BUILD_DIR/dali-rn-demo" ]; then
    echo -e "${RED}Error: Build not found. Please run 'make' in the build directory first.${NC}"
    exit 1
fi

cd "$BUILD_DIR"

# ============================================
# Native View Benchmark
# ============================================
echo -e "${YELLOW}=== Native View Test (1000 views) ===${NC}"
for i in $(seq 1 $RUNS); do
    ./native-benchmark-views 2>/dev/null &
    sleep $STABILIZE_TIME
    RSS=$(get_rss native-benchmark-views)
    if [ -n "$RSS" ]; then
        NATIVE_VIEW_RESULTS+=($RSS)
        echo -e "  Run $i: ${GREEN}$RSS KB${NC}"
    else
        echo -e "  Run $i: ${RED}Failed to get RSS${NC}"
    fi
    pkill -f native-benchmark-views 2>/dev/null || true
    sleep $CLEANUP_TIME
done
echo ""

# ============================================
# Native Image Benchmark
# ============================================
echo -e "${YELLOW}=== Native Image Test (100 images) ===${NC}"
for i in $(seq 1 $RUNS); do
    ./native-benchmark-images 2>/dev/null &
    sleep $STABILIZE_TIME
    RSS=$(get_rss native-benchmark-images)
    if [ -n "$RSS" ]; then
        NATIVE_IMAGE_RESULTS+=($RSS)
        echo -e "  Run $i: ${GREEN}$RSS KB${NC}"
    else
        echo -e "  Run $i: ${RED}Failed to get RSS${NC}"
    fi
    pkill -f native-benchmark-images 2>/dev/null || true
    sleep $CLEANUP_TIME
done
echo ""

# ============================================
# RN View Benchmark
# ============================================
echo -e "${YELLOW}=== RN View Test (1000 views) ===${NC}"
echo -e "  Bundling..."
cd "$PROJECT_DIR"
npx react-native bundle --platform ios --dev false \
    --entry-file rn-benchmark/view-test/index.js \
    --bundle-output build/bundle.js \
    --assets-dest build/assets 2>/dev/null

cd "$BUILD_DIR"
for i in $(seq 1 $RUNS); do
    ./dali-rn-demo 2>/dev/null &
    sleep $((STABILIZE_TIME + 3))  # RN needs more time to initialize
    RSS=$(get_rss dali-rn-demo)
    if [ -n "$RSS" ]; then
        RN_VIEW_RESULTS+=($RSS)
        echo -e "  Run $i: ${GREEN}$RSS KB${NC}"
    else
        echo -e "  Run $i: ${RED}Failed to get RSS${NC}"
    fi
    pkill -f dali-rn-demo 2>/dev/null || true
    sleep $CLEANUP_TIME
done
echo ""

# ============================================
# RN Image Benchmark
# ============================================
echo -e "${YELLOW}=== RN Image Test (100 images) ===${NC}"
echo -e "  Bundling..."
cd "$PROJECT_DIR"
npx react-native bundle --platform ios --dev false \
    --entry-file rn-benchmark/image-test/index.js \
    --bundle-output build/bundle.js \
    --assets-dest build/assets 2>/dev/null

cd "$BUILD_DIR"
for i in $(seq 1 $RUNS); do
    ./dali-rn-demo 2>/dev/null &
    sleep $((STABILIZE_TIME + 3))
    RSS=$(get_rss dali-rn-demo)
    if [ -n "$RSS" ]; then
        RN_IMAGE_RESULTS+=($RSS)
        echo -e "  Run $i: ${GREEN}$RSS KB${NC}"
    else
        echo -e "  Run $i: ${RED}Failed to get RSS${NC}"
    fi
    pkill -f dali-rn-demo 2>/dev/null || true
    sleep $CLEANUP_TIME
done
echo ""

# ============================================
# Calculate Averages and Display Summary
# ============================================
NATIVE_VIEW_AVG=$(calc_average "${NATIVE_VIEW_RESULTS[@]}")
NATIVE_IMAGE_AVG=$(calc_average "${NATIVE_IMAGE_RESULTS[@]}")
RN_VIEW_AVG=$(calc_average "${RN_VIEW_RESULTS[@]}")
RN_IMAGE_AVG=$(calc_average "${RN_IMAGE_RESULTS[@]}")

VIEW_OVERHEAD=$((RN_VIEW_AVG - NATIVE_VIEW_AVG))
IMAGE_OVERHEAD=$((RN_IMAGE_AVG - NATIVE_IMAGE_AVG))
VIEW_OVERHEAD_PCT=$((VIEW_OVERHEAD * 100 / NATIVE_VIEW_AVG))
IMAGE_OVERHEAD_PCT=$((IMAGE_OVERHEAD * 100 / NATIVE_IMAGE_AVG))

echo -e "${BLUE}=======================================${NC}"
echo -e "${BLUE}           SUMMARY RESULTS${NC}"
echo -e "${BLUE}=======================================${NC}"
echo ""
echo -e "${YELLOW}View Test (1000 views):${NC}"
echo -e "  Native DALi:    $(printf "%'d" $NATIVE_VIEW_AVG) KB (~$((NATIVE_VIEW_AVG / 1024)) MB)"
echo -e "  RN + DALi:      $(printf "%'d" $RN_VIEW_AVG) KB (~$((RN_VIEW_AVG / 1024)) MB)"
echo -e "  Overhead:       ${GREEN}+$(printf "%'d" $VIEW_OVERHEAD) KB (+${VIEW_OVERHEAD_PCT}%)${NC}"
echo ""
echo -e "${YELLOW}Image Test (100 images):${NC}"
echo -e "  Native DALi:    $(printf "%'d" $NATIVE_IMAGE_AVG) KB (~$((NATIVE_IMAGE_AVG / 1024)) MB)"
echo -e "  RN + DALi:      $(printf "%'d" $RN_IMAGE_AVG) KB (~$((RN_IMAGE_AVG / 1024)) MB)"
echo -e "  Overhead:       ${GREEN}+$(printf "%'d" $IMAGE_OVERHEAD) KB (+${IMAGE_OVERHEAD_PCT}%)${NC}"
echo ""
echo -e "${BLUE}=======================================${NC}"

# ============================================
# Save Results to File
# ============================================
REPORT_FILE="$PROJECT_DIR/docs/memory-benchmark-$(date +%Y%m%d-%H%M%S).md"
cat > "$REPORT_FILE" << EOF
# Memory Benchmark Results

**Date:** $(date '+%Y-%m-%d %H:%M:%S')
**Runs:** $RUNS per test

## Raw Results

### Native View (1000 views)
$(for i in "${!NATIVE_VIEW_RESULTS[@]}"; do echo "- Run $((i+1)): ${NATIVE_VIEW_RESULTS[$i]} KB"; done)
- **Average: $NATIVE_VIEW_AVG KB**

### Native Image (100 images)
$(for i in "${!NATIVE_IMAGE_RESULTS[@]}"; do echo "- Run $((i+1)): ${NATIVE_IMAGE_RESULTS[$i]} KB"; done)
- **Average: $NATIVE_IMAGE_AVG KB**

### RN View (1000 views)
$(for i in "${!RN_VIEW_RESULTS[@]}"; do echo "- Run $((i+1)): ${RN_VIEW_RESULTS[$i]} KB"; done)
- **Average: $RN_VIEW_AVG KB**

### RN Image (100 images)
$(for i in "${!RN_IMAGE_RESULTS[@]}"; do echo "- Run $((i+1)): ${RN_IMAGE_RESULTS[$i]} KB"; done)
- **Average: $RN_IMAGE_AVG KB**

## Summary

| Test | Native DALi | RN + DALi | Overhead |
|------|-------------|-----------|----------|
| 1000 Views | $((NATIVE_VIEW_AVG / 1024)) MB | $((RN_VIEW_AVG / 1024)) MB | +$((VIEW_OVERHEAD / 1024)) MB (+${VIEW_OVERHEAD_PCT}%) |
| 100 Images | $((NATIVE_IMAGE_AVG / 1024)) MB | $((RN_IMAGE_AVG / 1024)) MB | +$((IMAGE_OVERHEAD / 1024)) MB (+${IMAGE_OVERHEAD_PCT}%) |
EOF

echo -e "Results saved to: ${GREEN}$REPORT_FILE${NC}"
echo ""

# Cleanup
cleanup
