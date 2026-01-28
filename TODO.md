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

### 2. Parent-child insertion hierarchy
- **Issue**: All children are inserted into root (tag 1) instead of their actual parent container
- **Example**: In logs, `Insert: Child=4 into Parent=1` even when child should be inserted into a container (tag 22)
- **Impact**: Z-ordering issues, parent containers may overlap children
- **Files to investigate**:
  - `src/DaliMountingManager.cpp` - `ProcessMutation()` for Insert operations
  - Check how parent tags are resolved during Insert mutations

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
- **Files to investigate**:
  - `src/DaliMountingManager.cpp` - Text/Paragraph handling
  - `src/components/DaliTextComponent.cpp`

### 5. Element positioning inconsistent
- **Issue**: Elements appear at different positions than specified in style
- **Example**: Color boxes specified at y=420 appear at y=50 on screen
- **Observation**: Works correctly in benchmark tests but fails in mixed-component apps
- **Files to investigate**:
  - Compare how benchmark apps work vs complex apps
  - Check if position is being overwritten somewhere

## Component Issues

### 6. Text color hardcoded to BLACK
- **Issue**: `DaliTextComponent::ApplyProps()` hardcodes text color to `Dali::Color::BLACK`
- **Location**: `src/components/DaliTextComponent.cpp:35`
- **Fix needed**: Extract text color from React Native's TextProps
- **Impact**: White/colored text on dark backgrounds is invisible

## Working Features (for reference)
- View components with backgroundColor ✓
- Image components ✓
- Absolute positioning (in simple cases) ✓
- Benchmark tests with 1000+ views ✓
- Benchmark tests with 100 images ✓
