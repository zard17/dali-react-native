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

## Memory Profile

| Component | Memory |
|-----------|--------|
| Native DALi base | ~96-103 MB |
| React Native overhead | ~22-24 MB |
| Per 1000 Views | ~6 MB additional |
