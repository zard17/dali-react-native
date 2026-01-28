# DALi React Native - TODO

## Layout Issues

### 1. Rendering threshold issue - WORKAROUND APPLIED (DALi Bug)
- **Issue**: With `PARENT_ORIGIN::TOP_LEFT`, fewer than ~400 actors don't render
- **Root cause**: **DALi rendering bug** - confirmed to affect official DALi demos too!
  - The `flex-container.example` demo (8 items with TOP_LEFT) also fails to render items

#### Workaround Applied (2026-01-29)
Changed all components to use `PARENT_ORIGIN::CENTER` and `ANCHOR_POINT::CENTER`:
- `src/components/DaliViewComponent.cpp`
- `src/components/DaliTextComponent.cpp`
- `src/components/DaliImageComponent.cpp`

**Result**: Views now render correctly with any number of actors (tested 100 views ✓)

**Known limitation**: Views appear offset from top-left
- With CENTER origin, position (0,0) is relative to parent's center
- Position conversion to match RN top-left coordinates causes rendering failures
  (negative positions also fail to render)
- **Current behavior**: Views render but positioned relative to center

**TODO**:
1. Report DALi bug to Samsung DALi team
2. Once fixed, implement proper RN→DALi coordinate conversion

### 2. Parent-child insertion hierarchy - RESOLVED ✓
- **Issue**: All children are inserted into root (tag 1) instead of their actual parent container
- **Observed cause**: React Native Fabric's **view flattening optimization**
  - Fabric automatically flattens Views that don't have "important" properties
  - Views with only layout properties are considered collapsible
- **Workaround**: Use `collapsable={false}` on View components that must maintain hierarchy

#### Investigation Results (2026-01-29)
Compared layout metrics WITH vs WITHOUT `collapsable={false}`:

| Metric | WITH collapsable={false} | WITHOUT |
|--------|--------------------------|---------|
| Text position | x=100, y=80 (relative) | x=150, y=130 (absolute) |
| Text insert parent | Parent=6 (inner View) | Parent=1 (root) |
| Inner View insert | Parent=8 (outer View) | Parent=1 (root) |

**Key Findings:**
1. **Fabric DOES adjust positions correctly** - Text position changes from (100,80) to (150,130)
2. **Views with backgroundColor ARE created** (FormsView trait) - only hierarchy is flattened
3. **Visual result is identical** - absolute positioning compensates for flattened hierarchy

**Conclusion:** View flattening works as designed. The `collapsable={false}` workaround is not required for visual correctness, but may help with debugging by preserving hierarchy.

**Technical Details:**
- Position adjustment happens in `sliceChildShadowNodeViewPairs.cpp`:
  ```cpp
  shadowView.layoutMetrics.frame.origin += layoutOffset;
  ```
- `FormsView` trait prevents view deletion when `backgroundColor` has alpha > 0

### 3. Container with `flex: 1` has width=0 - RESOLVED ✓
- **Issue**: When using `flex: 1` style, the container gets `width: 0` in layout metrics
- **Root cause**: `layoutConstraints.minimumSize` was set to `{0, 0}`, allowing Yoga to calculate 0 width

#### Fix Applied (2026-01-29)
**File**: `src/DeviceInstanceManager.cpp` lines 140-141

```cpp
// Before (broken):
layoutConstraints.minimumSize = Size{0, 0};
layoutConstraints.maximumSize = Size{1920, 1080};

// After (fixed):
layoutConstraints.minimumSize = Size{1920, 1080};  // Force exact size
layoutConstraints.maximumSize = Size{1920, 1080};
```

**Result**: Root now correctly has `w=1920 h=1080` instead of `w=0 h=1080`

### 4. Only last Text element renders (when multiple texts)
- **Issue**: When multiple Text components are in the tree, only the last one is visible
- **Observation**: Text at y=430 shows, but texts at y=40, y=95 don't show
- **Possible cause**: Text actors may be overwriting each other or z-ordering issue
- **Related to**: Issue #2 - view flattening may cause all texts to be siblings at root level
- **Try**: Use `collapsable={false}` on parent containers to maintain proper z-ordering
- **Files to investigate**:
  - `src/DaliMountingManager.cpp` - Text/Paragraph handling
  - `src/components/DaliTextComponent.cpp`

### 5. Element positioning inconsistent
- **Issue**: Elements appear at different positions than specified in style
- **Example**: Color boxes specified at y=420 appear at y=50 on screen
- **Observation**: Works correctly in benchmark tests but fails in mixed-component apps
- **Related to**: Issue #2 - view flattening causes position to be relative to root, not parent
- **Try**: Use `collapsable={false}` on parent containers
- **Files to investigate**:
  - Compare how benchmark apps work vs complex apps
  - Check if position is being overwritten somewhere

## Component Issues

### 6. Text color hardcoded to BLACK - RESOLVED ✓
- **Issue**: Text color was hardcoded to `Dali::Color::BLACK`
- **Fix**: Removed hardcoded BLACK, now extracts color from React Native's `textAttributes.foregroundColor`
- **Files modified**:
  - `src/DaliMountingManager.cpp` - Color extraction with proper fallback and logging
  - `src/components/DaliTextComponent.cpp` - Removed legacy hardcoded color
- **Verified**: White text (`color: 'white'`) now renders as `rgba(1, 1, 1, 1)`

## Feature Implementation

### 7. Touch/Gesture handling
- **Status**: Not implemented
- **Required for**: Button presses, scrolling, gestures, interactive apps
- **Implementation approach**:
  - Connect DALi touch signals to React Native event system
  - Map DALi `TouchEvent` to React Native touch events (onPress, onPressIn, onPressOut, onLongPress)
  - Implement hit testing to determine which actor received the touch
  - Route events through Fabric's EventEmitter system
- **Files to create/modify**:
  - `src/DaliMountingManager.cpp` - Register touch callbacks on actors
  - Create event dispatcher to send touch events to JS
  - Handle `Pressable`, `TouchableOpacity`, `Button` components
- **DALi APIs to use**:
  - `Actor::TouchedSignal()` - Connect to touch events
  - `TouchEvent::GetPoint()` - Get touch coordinates
  - `TouchEvent::GetState()` - Get touch state (DOWN, UP, MOTION, etc.)
- **React Native side**:
  - Events dispatched via `EventEmitter` stored in `mEventEmitterRegistry`
  - Need to implement `dispatchEvent()` in mounting manager

## Working Features (for reference)
- View components with backgroundColor ✓
- Image components ✓
- Absolute positioning (in simple cases) ✓
- Benchmark tests with 1000+ views ✓
- Benchmark tests with 100 images ✓
