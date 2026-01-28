# Hermes Migration Plan

## Objective

Replace JavaScriptCore (JSC) with Hermes to reduce memory footprint.

**Expected Savings**: 5-10 MB (JSC ~15 MB → Hermes ~5-8 MB)

## Current State

### JSC Integration Points

1. **DeviceInstanceManager.cpp:7**
   ```cpp
   #include <jsc/JSCRuntime.h>
   ```

2. **DeviceInstanceManager.cpp:50**
   ```cpp
   mRuntime = facebook::jsc::makeJSCRuntime();
   ```

3. **CMakeLists.txt:65**
   ```cmake
   "-framework JavaScriptCore"
   ```

4. **third_party_dependencies/build_react_native/CMakeLists.txt:115-119**
   ```cmake
   add_library(jsc_runtime OBJECT "${REACT_COMMON_DIR}/jsc/JSCRuntime.cpp")
   ```

## Hermes Requirements

### 1. Build Hermes Library

Hermes needs to be built from source for macOS. The `hermes-engine` npm package only includes:
- `hermesc` compiler (for bytecode compilation)
- Android prebuilt libraries
- Headers (in android/include)

**Option A: Build from React Native's bundled Hermes**
```bash
# Clone hermes from React Native's pinned version
cd third_party_dependencies
git clone https://github.com/facebook/hermes.git
cd hermes
git checkout <version-matching-rn-0.79>

# Build for macOS
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --target libhermes
```

**Option B: Use Homebrew (if available)**
```bash
brew install hermes
```

### 2. Required Hermes Libraries

After building, we need:
- `libhermes.a` or `libhermes.dylib` - Main runtime
- `libjsi.a` - JSI interface (may be bundled)
- Headers from `hermes/include/`

### 3. Header Files Needed

```
hermes/
├── hermes.h              # Main API (makeHermesRuntime)
├── Public/
│   ├── RuntimeConfig.h   # Runtime configuration
│   └── GCConfig.h        # GC settings
└── VM/
    └── ...               # Internal (not needed directly)
```

## Implementation Steps

### Phase 1: Build Hermes (Day 1-2)

1. **Add Hermes build script**
   - Create `scripts/build_hermes.sh`
   - Clone Hermes repository
   - Configure CMake with appropriate flags
   - Build static library for macOS (arm64 + x86_64)

2. **Configure CMake integration**
   - Add `cmake/FindHermes.cmake` module
   - Set up include paths
   - Link against libhermes

### Phase 2: Code Changes (Day 2-3)

1. **Update DeviceInstanceManager.cpp**
   ```cpp
   // Before
   #include <jsc/JSCRuntime.h>
   mRuntime = facebook::jsc::makeJSCRuntime();

   // After
   #include <hermes/hermes.h>
   auto config = ::hermes::vm::RuntimeConfig::Builder()
       .withGCConfig(::hermes::vm::GCConfig::Builder()
           .withMaxHeapSize(32 << 20)  // 32MB max heap
           .build())
       .build();
   mRuntime = facebook::hermes::makeHermesRuntime(config);
   ```

2. **Update CMakeLists.txt**
   ```cmake
   # Remove
   # "-framework JavaScriptCore"

   # Add
   find_package(Hermes REQUIRED)
   target_link_libraries(dali-rn-demo ... ${HERMES_LIBRARIES})
   target_include_directories(dali-rn-demo ... ${HERMES_INCLUDE_DIRS})
   ```

3. **Update third_party_dependencies build**
   - Remove `jsc_runtime` target
   - Add `hermes_runtime` target (or link prebuilt)

### Phase 3: Bytecode Compilation (Optional, Day 3-4)

For additional performance/size benefits:

1. **Compile JS to Hermes bytecode**
   ```bash
   hermesc -emit-binary -out bundle.hbc bundle.js
   ```

2. **Update bundle loading in DeviceInstanceManager**
   - Detect `.hbc` files
   - Use `evaluateJavaScript` with bytecode buffer

### Phase 4: Testing & Benchmarking (Day 4-5)

1. **Functional testing**
   - Run existing demo app
   - Verify all components render correctly
   - Test text, images, flexbox layout

2. **Memory benchmarking**
   ```bash
   ./scripts/memory-benchmark.sh 5
   ```
   Compare with JSC baseline.

3. **Performance testing**
   - Measure startup time
   - Measure JS execution time

## File Changes Summary

| File | Change |
|------|--------|
| `scripts/build_hermes.sh` | NEW - Build script |
| `cmake/FindHermes.cmake` | NEW - CMake find module |
| `CMakeLists.txt` | Modify - Link Hermes |
| `third_party_dependencies/build_react_native/CMakeLists.txt` | Modify - Remove JSC |
| `src/DeviceInstanceManager.cpp` | Modify - Use HermesRuntime |
| `src/DeviceInstanceManager.h` | Modify - Include changes |

## Risks & Mitigations

### Risk 1: Hermes build complexity
- **Mitigation**: Start with prebuilt if available, fall back to source build
- **Mitigation**: Document exact build steps

### Risk 2: API differences between JSC and Hermes
- **Mitigation**: Both implement JSI, so core API is identical
- **Mitigation**: Test thoroughly after migration

### Risk 3: Hermes-specific bugs
- **Mitigation**: Use stable Hermes version matching React Native
- **Mitigation**: Keep JSC as fallback (compile-time flag)

## Rollback Plan

Keep JSC support via compile flag:
```cmake
option(USE_HERMES "Use Hermes instead of JSC" ON)

if(USE_HERMES)
  # Hermes configuration
else()
  # JSC configuration
endif()
```

## Success Criteria

1. ✅ All existing tests pass
2. ✅ Memory usage reduced by ≥5 MB
3. ✅ No regression in startup time
4. ✅ Flexbox demo renders correctly
5. ✅ Benchmark script works with Hermes

## References

- [Hermes GitHub](https://github.com/facebook/hermes)
- [Hermes Documentation](https://hermesengine.dev/)
- [React Native Hermes Integration](https://reactnative.dev/docs/hermes)
- [Building Hermes from Source](https://github.com/facebook/hermes/blob/main/doc/BuildingAndRunning.md)
