# Claude Code Instructions for dali-react-native

## Project Overview

DALi React Native integration - renders React Native apps using Samsung's DALi graphics toolkit via Fabric renderer.

## Project Structure

- `src/` - C++ source files for DALi React Native integration
  - `DaliMountingManager.cpp` - Processes Fabric mutations, creates DALi actors
  - `DaliRenderer.cpp` - Main renderer initialization
  - `components/` - DALi component implementations (View, Text, Image)
- `native-benchmark/` - Native C++ benchmark apps (view-test, image-test)
- `rn-benchmark/` - React Native benchmark apps
- `docs/` - Documentation and reports
- `assets/` - Image assets for testing

## Key Files

- `App.js` - Main React Native app entry point
- `bundle.js` - Bundled JS code (generated)
- `TODO.md` - Known issues and planned features

## Testing

Debug logs are written to `debug.log` in the project root.
