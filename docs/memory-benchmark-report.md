# Memory & Performance Benchmark Report

**Date:** 2026-01-29
**Platform:** macOS (Apple Silicon)

## JS Engine Comparison

### Memory Usage (Main App - Flexbox Demo)

| JS Engine | Run 1 | Run 2 | Run 3 | Average |
|-----------|-------|-------|-------|---------|
| JavaScriptCore | 124.1 MB | 123.1 MB | 122.5 MB | **123.2 MB** |
| Hermes 0.14.0 | 131.6 MB | 130.3 MB | 130.4 MB | **130.8 MB** |

**Difference:** Hermes uses ~7.6 MB more than JSC (+6.2%)

### Startup Time (Time to Bundle Execution)

| Test | Run 1 | Run 2 | Run 3 | Average |
|------|-------|-------|-------|---------|
| Native DALi (1000 views) | 221 ms | 197 ms | 193 ms | **204 ms** |
| React Native + JSC | 256 ms | 248 ms | 239 ms | **248 ms** |
| React Native + Hermes | 255 ms | 240 ms | 273 ms | **256 ms** |

**Observations:**
- Native DALi is ~44 ms faster than RN (no JS parsing overhead)
- JSC and Hermes have similar startup times (~8 ms difference)
- RN overhead: ~44-52 ms for JS runtime initialization + bundle parsing

## Benchmark Tests (with Hermes)

### View Test (100 Views)

| Metric | Native DALi | RN + Hermes | Overhead |
|--------|-------------|-------------|----------|
| Memory | 102 MB | 135 MB | +32 MB (+31%) |

### Image Test (100 Images)

| Metric | Native DALi | RN + Hermes | Overhead |
|--------|-------------|-------------|----------|
| Memory | 96 MB | 123 MB | +27 MB (+28%) |

## Analysis

### Why JSC Uses Less Memory in This Test

The prebuilt Hermes framework from Maven Central may include:
- Debug symbols (debug build variant)
- Additional runtime features not stripped

For production, building Hermes from source with release optimizations would likely show different results.

### Hermes Benefits (Not Shown in These Tests)

1. **Bytecode Precompilation**: Compile JS to `.hbc` bytecode for faster cold starts
2. **Better GC**: More predictable memory behavior in long-running apps
3. **Smaller Binary**: On mobile platforms, Hermes binary is typically smaller
4. **Debugging**: Better source maps and debugging experience

### Recommendations

- For **memory-critical** applications: Consider JSC or optimize Hermes build
- For **startup-critical** applications: Use Hermes with bytecode precompilation
- For **long-running** applications: Hermes GC provides more stable memory over time

## Test Environment

- Window Size: 800x600
- React Native: 0.83.1
- Hermes: 0.14.0 (prebuilt from Maven Central)
- DALi: 2.0.0
