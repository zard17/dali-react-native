# DALi React Native - TODO

## Layout Issues

### 1. Flexbox centering not working (justifyContent/alignItems)
- **Issue**: `justifyContent: 'center'` and `alignItems: 'center'` don't properly center child elements
- **Expected**: Child views should be centered within parent when using flexbox centering
- **Actual**: Children appear at top-left instead of center
- **Workaround**: Using explicit position calculation with `Dimensions.get('window')`
- **Root cause**: Likely a mapping issue between Yoga layout output and DALi coordinate system
- **Files to investigate**:
  - `src/DaliMountingManager.cpp` - `UpdateLayout()` function
  - Check how `layoutMetrics.frame` is being applied to DALi actors
  - Verify Yoga is calculating correct positions for centered layouts

### 2. Parent-child insertion hierarchy - INVESTIGATION IN PROGRESS
- **Issue**: All children are inserted into root (tag 1) instead of their actual parent container
- **Observed cause**: React Native Fabric's **view flattening optimization**
  - Fabric automatically flattens Views that don't have "important" properties
  - Views with only backgroundColor, position styles are considered collapsible
- **Workaround**: Use `collapsable={false}` on View components that must maintain hierarchy
- **Verified**: With `collapsable={false}`, Insert mutations show correct parent tags

#### Open Question: Why does view flattening cause visual issues?
- In theory, view flattening should be **transparent** - same visual result, better performance
- Yoga should recalculate child positions to be absolute (relative to new parent)
- **Need to investigate**: Compare layout metrics WITH vs WITHOUT `collapsable={false}`
  - WITH collapsable={false}: Text at (100, 80) relative to parent at (50, 50) → visual (150, 130)
  - WITHOUT: If positions aren't adjusted, Text would appear at (100, 80) instead of (150, 130)
- **Hypothesis**: Either Yoga isn't adjusting positions, or our DALi implementation needs to handle this
- **Next step**: Run app without collapsable, capture layout metrics, compare positions

### 3. Container with `flex: 1` has width=0
- **Issue**: When using `flex: 1` style, the container gets `width: 0` in layout metrics
- **Expected**: Container should expand to fill parent
- **Actual**: Container has height but 0 width
- **Workaround**: Use explicit `width` and `height` instead of `flex: 1`
- **Files to investigate**:
  - Check Yoga layout constraints being passed to root surface
  - Verify surface size is propagated correctly

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

### 6. Text color hardcoded to BLACK
- **Issue**: `DaliTextComponent::ApplyProps()` hardcodes text color to `Dali::Color::BLACK`
- **Location**: `src/components/DaliTextComponent.cpp:35`
- **Fix needed**: Extract text color from React Native's TextProps
- **Impact**: White/colored text on dark backgrounds is invisible

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
