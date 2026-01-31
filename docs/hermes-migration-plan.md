# Hermes Migration - COMPLETED

## Summary

Successfully migrated from JavaScriptCore (JSC) to Hermes JavaScript engine.

**Status**: Completed
**Date**: 2026-01-29

## Changes Made

### 1. Downloaded Prebuilt Hermes from Maven Central

Building Hermes from source failed due to libatomic issues on macOS. Instead, we use the prebuilt Hermes from React Native's Maven repository.

- **Version**: 0.14.0 (matches React Native .hermesversion)
- **Source**: `https://repo1.maven.org/maven2/com/facebook/hermes/hermes-ios/0.14.0/`
- **Framework**: hermesvm.framework (universal binary: x86_64 + arm64)

### 2. Files Modified

| File | Change |
|------|--------|
| `scripts/build_hermes.sh` | Downloads prebuilt Hermes from Maven instead of building from source |
| `cmake/FindHermes.cmake` | Updated to find hermesvm.framework on macOS |
| `CMakeLists.txt` | Added `find_package(Hermes REQUIRED)` and framework linking |
| `src/DeviceInstanceManager.cpp` | Changed from `makeJSCRuntime()` to `makeHermesRuntime()` |

### 3. Code Changes

**DeviceInstanceManager.cpp**:
```cpp
// Before (JSC)
#include <jsc/JSCRuntime.h>
mRuntime = facebook::jsc::makeJSCRuntime();

// After (Hermes)
#include <hermes/hermes.h>
auto config = ::hermes::vm::RuntimeConfig::Builder()
    .withGCConfig(::hermes::vm::GCConfig::Builder()
        .withMaxHeapSize(32 << 20)  // 32MB max heap
        .build())
    .build();
mRuntime = facebook::hermes::makeHermesRuntime(config);
```

**CMakeLists.txt**:
```cmake
find_package(Hermes REQUIRED)

target_link_libraries(dali-rn-demo
  ...
  "-F${HERMES_ROOT}/lib"      # Framework search path
  "-framework hermesvm"       # Hermes JavaScript engine
)
```

## Running the App

```bash
# Set framework path and run
DYLD_FRAMEWORK_PATH="third_party_dependencies/hermes-install/lib:$DYLD_FRAMEWORK_PATH" ./build/dali-rn-demo
```

## Memory Results

| Configuration | Memory (RSS) |
|--------------|--------------|
| With JSC | ~118-127 MB |
| With Hermes | ~118-119 MB |

Memory usage is roughly equivalent. The main benefits of Hermes are:
- Faster startup (bytecode vs parsing)
- Better GC behavior over time
- Smaller binary size on mobile
- Future: Bytecode precompilation for additional benefits

## Future Improvements

### Bytecode Precompilation (Optional)

For additional performance benefits, compile JS to Hermes bytecode:

```bash
# Compile bundle to bytecode
hermesc -emit-binary -out bundle.hbc bundle.js
```

Then load the `.hbc` file instead of `.js` in DeviceInstanceManager.

## Verification

The app works correctly with Hermes:
- JS Runtime initializes: `"JS Runtime (Hermes) Initialized"`
- Bundle executes successfully
- All components render correctly (Views, Text, flexbox layout)
- Memory usage is consistent with JSC baseline

## Rollback

To revert to JSC:

1. In `DeviceInstanceManager.cpp`:
   - Change `#include <hermes/hermes.h>` back to `#include <jsc/JSCRuntime.h>`
   - Change `makeHermesRuntime()` back to `makeJSCRuntime()`

2. In `CMakeLists.txt`:
   - Remove `find_package(Hermes REQUIRED)`
   - Change `-framework hermesvm` back to `-framework JavaScriptCore`
   - Remove `-F${HERMES_ROOT}/lib`
