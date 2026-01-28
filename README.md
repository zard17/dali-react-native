# DALi React Native Renderer (New Architecture)

A Proof of Concept renderer for **React Native's New Architecture (Fabric)** using **Samsung DALi** as the graphics backend, running on macOS for development.

This project demonstrates bridging React Native's C++ Fabric core with DALi's Scene Graph, using **JavaScriptCore** as the JavaScript runtime and mapping React components to DALi Actors.

![DALi React Native Demo](screenshot.png)

## Features

- **React Native Fabric Integration**: Uses the C++ core of React Native (JSI, Fabric, Renderer)
- **JavaScriptCore Runtime**: System JSC on macOS, bridged via JSI
- **DALi Scene Graph Mapping**: Maps React primitives (`<View>`, `<Text>`, `<Image>`) to DALi Controls
- **Flexbox Layout**: Full Yoga layout engine support with correct positioning
- **Text Rendering**: Extracts text content and styling from Fabric's ParagraphState
- **Image Support**: Local file and URI-based image loading

## Prerequisites

- **macOS** (Apple Silicon or Intel)
- **Xcode Command Line Tools**
- **CMake** (3.14+)
- **Node.js & npm**
- **DALi Environment**: DALi libraries with ANGLE backend for macOS

## Project Structure

```
dali-react-native/
├── src/                    # C++ source code
│   ├── DaliRenderer.*      # Entry point, window management
│   ├── DaliMountingManager.*  # Fabric mutation handler
│   ├── DeviceInstanceManager.*  # JS runtime, scheduler lifecycle
│   └── components/         # DALi component implementations
├── native-benchmark/       # Pure C++ DALi benchmarks
├── rn-benchmark/          # React Native benchmarks
├── scripts/               # Build and test scripts
├── docs/                  # Documentation
└── index.js               # React Native app entry point
```

## Setup and Build

1. **Install npm dependencies**:
   ```bash
   npm install
   ```

2. **Build C++ Dependencies**:
   ```bash
   ./build_deps.sh
   ```

3. **Build the Application**:
   ```bash
   mkdir build && cd build
   cmake ..
   make -j8
   ```

4. **Bundle the JavaScript**:
   ```bash
   npx react-native bundle --platform ios --dev false \
       --entry-file index.js --bundle-output build/bundle.js
   ```

## Running

```bash
cd build
./dali-rn-demo
```

An 800x600 window will appear displaying a flexbox layout demo with:
- Header bar
- Colored boxes with space-between layout
- Flex items with different weights
- Nested flex containers
- Footer bar

## Architecture

See [docs/ARCHITECTURE.md](docs/ARCHITECTURE.md) for detailed documentation.

```
JavaScript (React) → Fabric (C++) → DaliMountingManager → DALi Actors
                         ↓
                   Yoga Layout
```

1. **JavaScriptCore** executes the JavaScript bundle
2. **Fabric** reconciles the component tree and generates mutations
3. **DaliMountingManager** processes mutations (Create, Insert, Update, Delete)
4. **DALi Actors** are created and rendered via ANGLE/OpenGL ES

## Memory Benchmark

React Native overhead compared to pure C++ DALi (800x600 window):

| Test | Native DALi | RN + DALi | Overhead |
|------|-------------|-----------|----------|
| 1000 Views | 103 MB | 127 MB | +24 MB (+23%) |
| 100 Images | 96 MB | 117 MB | +21 MB (+22%) |

The ~22-24 MB overhead includes JSC runtime, Fabric renderer, and shadow tree.

Run benchmarks:
```bash
./scripts/memory-benchmark.sh
```

See [docs/memory-benchmark-report.md](docs/memory-benchmark-report.md) for detailed analysis.

## Current Status

- ✅ JavaScriptCore runtime integration
- ✅ Fabric mutation processing
- ✅ View, Text, Image components
- ✅ Flexbox layout (Yoga)
- ✅ Background colors and text styling
- ✅ Local image loading
- ⬜ Touch/gesture handling
- ⬜ ScrollView support
- ⬜ Tizen deployment

## License

This project is for demonstration purposes.
