# Memory & Performance Benchmark Report

**Date:** 2026-01-31
**Platform:** macOS (Apple Silicon)

## JS Engine Comparison

### Memory Usage (Main App - Flexbox Demo)

| JS Engine | Run 1 | Run 2 | Run 3 | Average |
|-----------|-------|-------|-------|---------|
| JavaScriptCore | 123.3 MB | 123.7 MB | 123.3 MB | **123.4 MB** |
| Hermes 0.14.0 (Release) | 129.2 MB | 129.6 MB | 129.9 MB | **129.5 MB** |

**Difference:** Hermes uses ~6.1 MB more than JSC (+5.0%)

### Hermes Debug vs Release

| Build | Binary Size | Memory Usage |
|-------|-------------|--------------|
| Debug | 13 MB | 130.8 MB |
| Release | 9.2 MB | 129.5 MB |

**Release saves:** 3.8 MB binary size, 1.3 MB runtime memory

### Startup Time (Time to Bundle Execution)

| Test | Run 1 | Run 2 | Run 3 | Average |
|------|-------|-------|-------|---------|
| Native DALi (1000 views) | 539 ms* | 192 ms | 192 ms | **192 ms** |
| React Native + JSC | 273 ms* | 238 ms | 243 ms | **240 ms** |
| React Native + Hermes | 273 ms* | 254 ms | 249 ms | **252 ms** |

*First run includes cold-start overhead (shader compilation, cache warming)

**Observations:**
- Native DALi is ~48-60 ms faster than RN (no JS overhead)
- JSC is ~12 ms faster than Hermes for startup
- After warm-up, all engines show consistent performance

## Summary

| Metric | JSC | Hermes (Release) | Difference |
|--------|-----|------------------|------------|
| Memory | 123.4 MB | 129.5 MB | +6.1 MB (+5%) |
| Startup | 240 ms | 252 ms | +12 ms (+5%) |
| Binary | System | 9.2 MB | N/A |

## Analysis

### Current Results

In these benchmarks, JSC outperforms Hermes slightly in both memory and startup time. This is because:

1. **JSC is system-provided** on macOS - no additional binary overhead
2. **Hermes prebuilt** may not be fully optimized for macOS desktop
3. **No bytecode precompilation** - Hermes parses JS source like JSC

### When Hermes Would Win

1. **Bytecode precompilation**: `hermesc -emit-binary bundle.js -out bundle.hbc`
   - Eliminates parsing time
   - Faster cold starts

2. **Mobile platforms**: Hermes binary is smaller than bundled JSC

3. **Long-running apps**: Hermes GC is more predictable

4. **Large bundles**: Hermes bytecode is smaller than minified JS

## Test Environment

- Window Size: 800x600
- React Native: 0.83.1
- Hermes: 0.14.0 (Release build from Maven Central)
- DALi: 2.0.0
- macOS: Apple Silicon
