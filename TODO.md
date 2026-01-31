# DALi React Native - TODO

## Resolved Issues

### Coordinate System - RESOLVED ✓
- **Issue**: Views not rendering correctly with TOP_LEFT origin
- **Root Cause**: Mac full-screen mode has ANGLE rendering bug (Y < 200 doesn't render)
- **Solution**: Use 800x600 windowed mode - TOP_LEFT works perfectly
- **Implementation**:
  - All components use `PARENT_ORIGIN::TOP_LEFT` and `ANCHOR_POINT::TOP_LEFT`
  - Direct coordinate mapping: RN (x,y) → DALi (x,y)
  - Window size detected dynamically from DALi

### Layout Constraints - RESOLVED ✓
- **Issue**: `flex: 1` containers had width=0
- **Fix**: Set `layoutConstraints.minimumSize` equal to window size
- **File**: `src/DeviceInstanceManager.cpp`

### View Flattening - UNDERSTOOD ✓
- React Native Fabric flattens Views without "important" properties
- Visual result is correct - positions are adjusted automatically
- Use `collapsable={false}` only if hierarchy debugging is needed

### Text Color - RESOLVED ✓
- Colors extracted correctly from `textAttributes.foregroundColor`
- Font size and weight also working

## Pending Features

### 1. Touch/Gesture Handling
**Priority**: High

**Required for**: Interactive apps, buttons, scrolling

**Implementation Plan**:
1. Connect DALi `TouchedSignal()` to actors in DaliMountingManager
2. Map touch events to React Native format:
   - `TouchEvent::GetState()` → onPressIn, onPressOut, onPress
   - `TouchEvent::GetPoint()` → touch coordinates
3. Dispatch events via `EventEmitter` from `mEventEmitterRegistry`

**Files to modify**:
- `src/DaliMountingManager.cpp` - Register touch callbacks
- `src/components/*.cpp` - Enable touch on controls

### 2. ScrollView Support
**Priority**: Medium

**Required for**: Lists, scrollable content

**Approach**: Map to `Dali::Toolkit::ScrollView`

### 3. Animation Support
**Priority**: Medium

**Approach**: Bridge React Native Animated API to DALi animations

### 4. Border Radius
**Priority**: Low

**Status**: Property exists but not fully implemented

### 5. Tizen Deployment
**Priority**: Future

**Tasks**:
- Test on actual Tizen device
- Package as TPK
- Handle Tizen-specific APIs

## Known Limitations

### Mac Full-Screen Mode
- ANGLE has a rendering bug where Y positions < ~200 don't render
- **Workaround**: Use windowed mode (800x600)
- This is Mac/ANGLE specific, should work fine on Tizen

### View Hierarchy
- Fabric view flattening means not all Views become DALi actors
- Views are only created if they have visual properties (backgroundColor, etc.)
- This is expected behavior, not a bug

## Working Features

- ✅ View components with backgroundColor
- ✅ Text components with font size, color, weight
- ✅ Image components with local file loading
- ✅ Flexbox layout (flex, flexDirection, justifyContent, alignItems)
- ✅ Absolute positioning
- ✅ Nested containers
- ✅ Dynamic window size detection
- ✅ Memory benchmarking (see `scripts/memory-benchmark.sh`)
- ✅ **Hermes JavaScript engine** (migrated from JavaScriptCore)

## Memory Profile

| Component | Memory |
|-----------|--------|
| Native DALi base | ~96-103 MB |
| React Native overhead | ~22-24 MB |
| Per 1000 Views | ~6 MB additional |

## Memory Optimization Ideas

### High Impact (Target: -10-15 MB)

#### 1. Switch to Hermes Engine - COMPLETED ✓
**Status**: Implemented (2026-01-29)
- Migrated from JavaScriptCore to Hermes
- Uses prebuilt hermesvm.framework from Maven Central
- Memory usage: ~118-119 MB (comparable to JSC)
- Future: Bytecode precompilation for additional performance
- **See**: `docs/hermes-migration-plan.md`

#### 2. Lazy Module Loading
**Estimated Savings**: 3-5 MB
- Don't load all TurboModules at startup
- Load modules on first use
- Unload unused modules after timeout
- **Files**: `src/TurboModuleRegistry.cpp`

#### 3. Strip Unused RN Code
**Estimated Savings**: 2-3 MB
- Tree-shake unused React Native modules from bundle
- Remove LogBox, dev tools in release builds
- Minimize polyfills
- **Files**: `metro.config.js`, build configuration

### Medium Impact (Target: -5-10 MB)

#### 4. Actor Pooling/Recycling
**Estimated Savings**: 2-4 MB for dynamic lists
- Reuse DALi actors instead of create/destroy
- Implement RecyclerView-style virtualization
- Keep pool of common actor types (View, Text, Image)
- **Files**: `src/DaliMountingManager.cpp`

#### 5. Image Memory Management
**Estimated Savings**: Variable (depends on image usage)
- Lazy load images (only when visible)
- Release textures for off-screen images
- Use compressed texture formats (ETC2, ASTC)
- Implement texture atlasing for small images
- **Files**: `src/components/DaliImageComponent.cpp`

#### 6. Shadow Tree Optimization
**Estimated Savings**: 1-3 MB
- Release shadow nodes for static content after mount
- More aggressive view flattening
- Don't create shadow nodes for text-only components
- **Files**: Fabric integration layer

### Low Impact / Long-term

#### 7. Text Rendering Optimization
- Cache text measurements (avoid re-measuring same text)
- Share TextLabel for identical static text
- Use bitmap fonts for UI text
- **Files**: `src/DaliTextLayoutManager.cpp`, `src/components/DaliTextComponent.cpp`

#### 8. Native Memory Profiling
- Profile C++ allocations with Instruments/Valgrind
- Identify memory leaks in actor lifecycle
- Optimize shared_ptr usage
- Ensure proper cleanup in unmount operations

#### 9. JS Bundle Optimization
- Code splitting for large apps
- Lazy load screens/features
- Use RAM bundles for faster loading

### Measurement Strategy

To validate optimizations:
```bash
# Run before/after memory benchmark
./scripts/memory-benchmark.sh 5

# Profile specific areas
instruments -t "Allocations" ./build/dali-rn-demo
```

### Priority Order

1. ~~**Hermes**~~ - COMPLETED ✓
2. **Lazy Module Loading** - Low risk, moderate savings
3. **Strip Unused Code** - Build-time only, no runtime risk
4. **Actor Pooling** - Needed for lists anyway, good ROI
5. **Image Management** - Important for image-heavy apps
