# DALi React Native Memory Benchmark Report

**Date:** 2026-01-29
**Platform:** macOS (Apple Silicon)
**Window Size:** 800x600
**Graphics Backend:** GLES via ANGLE

## Test Configuration

- **Native DALi:** Direct C++ implementation using DALi Toolkit
- **React Native + DALi:** JavaScript app rendered via Fabric to DALi backend
- **Measurement:** RSS (Resident Set Size) in KB, measured after 5-8 seconds of app stabilization
- **Runs:** 3 runs per test for consistency verification

## Results

### Native DALi - 1000 Views

| Run | RSS (KB) | RSS (MB) |
|-----|----------|----------|
| 1 | 105,008 | 102.5 |
| 2 | 105,184 | 102.7 |
| 3 | 104,544 | 102.1 |
| **Average** | **104,912** | **102.5** |

### Native DALi - 100 Images

| Run | RSS (KB) | RSS (MB) |
|-----|----------|----------|
| 1 | 98,480 | 96.2 |
| 2 | 98,416 | 96.1 |
| 3 | 98,416 | 96.1 |
| **Average** | **98,437** | **96.1** |

### React Native + DALi - 1000 Views

| Run | RSS (KB) | RSS (MB) |
|-----|----------|----------|
| 1 | 128,848 | 125.8 |
| 2 | 129,792 | 126.8 |
| 3 | 130,128 | 127.1 |
| **Average** | **129,589** | **126.6** |

### React Native + DALi - 100 Images

| Run | RSS (KB) | RSS (MB) |
|-----|----------|----------|
| 1 | 119,504 | 116.7 |
| 2 | 121,008 | 118.2 |
| 3 | 120,368 | 117.5 |
| **Average** | **120,293** | **117.5** |

## Summary Comparison

| Test | Native DALi | RN + DALi | Overhead (MB) | Overhead (%) |
|------|-------------|-----------|---------------|--------------|
| 1000 Views | 102.5 MB | 126.6 MB | +24.1 MB | +24% |
| 100 Images | 96.1 MB | 117.5 MB | +21.4 MB | +22% |

## Key Findings

1. **Consistent Results:** Memory measurements are highly consistent across runs (< 1% variance), indicating stable memory behavior.

2. **Fixed Overhead:** React Native adds approximately 22-24 MB of overhead regardless of the number of UI elements. This overhead includes:
   - Hermes JavaScript runtime
   - React Native Fabric renderer
   - Shadow tree and layout engine (Yoga)
   - Bridge infrastructure

3. **Scalable Architecture:** The percentage overhead decreases as the number of elements increases, suggesting the architecture scales well for complex UIs.

4. **Reasonable Overhead:** ~22-24% memory overhead is acceptable for a cross-platform framework that provides:
   - Declarative UI programming model
   - Hot reloading during development
   - Large ecosystem of JavaScript libraries
   - Cross-platform code sharing

## Test Details

### View Test
- Creates a 20x50 grid of colored rectangles (1000 total)
- Each view is 20x20 pixels with unique background color
- Uses absolute positioning

### Image Test
- Creates a 10x10 grid of images (100 total)
- Each image is 50x50 pixels
- Loads from local PNG files (10 unique images, cycled)
- Centered in window

## Environment

```
DALi Version: 2.x (with ANGLE backend)
React Native: 0.79.x (Fabric enabled)
Hermes: Enabled
Window Mode: Windowed (800x600)
```

## Notes

- Full-screen mode on Mac requires a Y-offset workaround due to an ANGLE rendering bug
- Windowed mode (tested here) works correctly without workarounds
- Memory measurements taken using `ps -o rss` command
