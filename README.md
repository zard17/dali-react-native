# DALi React Native Renderer (New Architecture)

This project is a Proof of Concept (PoC) renderer implementation for **React Native's New Architecture (Fabric)** on **Tizen/DALi**, running on macOS for development.

It demonstrates how to bridge React Native's C++ core (Fabric) with the DALi Scene Graph, utilizing **JavaScriptCore** as the JS runtime and mapping React components to DALi Actors.

## Features

-   **React Native Fabric Integration**: Uses the C++ core of React Native (JSI, Fabric, Renderer).
-   **DALi Scene Graph Mapping**: Maps React view primitives (`<View>`, `<Text>`, `<Image>`) to DALi `Control`, `TextLabel`, and `ImageView`.
-   **JavaScriptCore Runtime**: Integrated with the system's `JavaScriptCore` framework via a custom `JSCRuntime` adapter (built from React Native sources).
-   **Manual Dependency Build**: Includes scripts to build necessary dependencies (`Folly`, `Glog`, `Yoga`, etc.) cleanly on macOS.
-   **Simulated Execution**: Currently simulates the Fabric mutation pipeline (`simulatedJSExecution`) to demonstrate the renderer capablities without a full Metro bundler connection.

## Prerequisites

-   **macOS** (Project is currently configured for macOS build environment).
-   **Xcode Command Line Tools** (Clang/LLVM).
-   **CMake** (3.14+).
-   **Node.js & npm**.
-   **DALi Environment**: You need the DALi libraries installed or available in your PKG_CONFIG_PATH. (Assumes a local Tizen/DALi env setup).

## Project Structure

-   `src/`: C++ Source code for the Renderer.
    -   `DaliRenderer`: Entry point, initializes the bridge.
    -   `DaliMountingManager`: Handles Shadow Tree mutations and updates the DALi scene graph.
    -   `DeviceInstanceManager`: Manages the JS Runtime (JSC) and Fabric Scheduler.
    -   `components/`: DALi-specific component implementations.
-   `third_party_dependencies/`: Directory where manual dependencies are built.
-   `node_modules/`: React Native sources.
-   `cmake/`: CMake helper modules.

## Setup and Build

1.  **Install npm dependencies**:
    ```bash
    npm install
    ```

2.  **Build C++ Dependencies**:
    This project requires several static libraries (Folly, Glog, Yoga, etc.). Run the helper script to build them:
    ```bash
    ./build_deps.sh
    ```
    *Note: This may take some time as it compiles Folly and Boost parts.*

3.  **Build the Demo Application**:
    ```bash
    mkdir build
    cd build
    cmake ..
    make -j4
    ```

## Running the Demo

After building, run the executable:

```bash
./build/dali-rn-demo
```

**Expected Output**:
A window should appear displaying:
-   **Cyan Background** (Root View).
-   **Magenta Square** (Child View).
-   **"Color Test Running"** (Text Label).

The terminal logs will show the initialization of the JS Runtime and the processing of Mock Mutations.

## Current Status & Limitations

-   **JS Bundle**: While the JS Runtime is initialized, the project currently *simulates* the resulting Fabric mutations in C++ (`ProcessMockMutation`) rather than fully parsing a JS bundle via Metro. This allows isolating and verifying the Renderer logic.
-   **Platform**: Initial port focus is macOS.
-   **Components**: Only basic `View`, `Text`, and `Image` are mapped.
