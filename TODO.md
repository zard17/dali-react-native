# DALi React Native - TODO

## Layout Issues

### Flexbox centering not working (justifyContent/alignItems)
- **Issue**: `justifyContent: 'center'` and `alignItems: 'center'` don't properly center child elements
- **Expected**: Child views should be centered within parent when using flexbox centering
- **Actual**: Children appear at top-left instead of center
- **Workaround**: Using explicit position calculation with `Dimensions.get('window')`
- **Root cause**: Likely a mapping issue between Yoga layout output and DALi coordinate system
- **Files to investigate**:
  - `src/DaliMountingManager.cpp` - `UpdateLayout()` function
  - Check how `layoutMetrics.frame` is being applied to DALi actors
  - Verify Yoga is calculating correct positions for centered layouts
