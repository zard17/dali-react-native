# Architecture: DALi React Native Renderer

This document outlines the architecture of the DALi React Native Renderer for the New Architecture (Fabric).

## High-Level Overview

The system bridges **React Native Fabric** (C++) with the **DALi Scene Graph**, replacing platform-specific UI layers (Android Views, iOS UIViews) with DALi Actors.

### System Diagram

```
┌─────────────────────────────────────────────────────────────────┐
│                     React Native Layer                          │
├─────────────────────────────────────────────────────────────────┤
│  JavaScript (App.js)                                            │
│       ↓                                                         │
│  React Runtime ──JSI──> Fabric UIManager (C++)                  │
│                              ↓                                  │
│                         Shadow Tree                             │
│                              ↓                                  │
│                      Yoga Layout Engine                         │
└─────────────────────────────────────────────────────────────────┘
                              ↓
┌─────────────────────────────────────────────────────────────────┐
│                    Bridge / Renderer Layer                      │
├─────────────────────────────────────────────────────────────────┤
│  Scheduler ──Commit──> DeviceInstanceManager                    │
│                              ↓                                  │
│                      DaliMountingManager                        │
│                              ↓                                  │
│                    Component Factory                            │
│              (DaliViewComponent, DaliTextComponent, etc.)       │
└─────────────────────────────────────────────────────────────────┘
                              ↓
┌─────────────────────────────────────────────────────────────────┐
│                       DALi Layer                                │
├─────────────────────────────────────────────────────────────────┤
│  DALi Actors (Control, TextLabel, ImageView)                    │
│       ↓                                                         │
│  DALi Window ──ANGLE──> OpenGL ES                               │
└─────────────────────────────────────────────────────────────────┘
```

## Key Components

### 1. DeviceInstanceManager

**Role**: Central controller for the React Native instance.

**Responsibilities**:
- Initializes the **JavaScriptCore** runtime via JSI
- Creates and manages the Fabric **Scheduler**
- Manages **SurfaceHandler** lifecycle
- Executes JavaScript bundles
- Pumps the event loop via DALi idle callbacks

### 2. DaliMountingManager

**Role**: Scene graph builder that translates Fabric mutations to DALi API calls.

**Responsibilities**:
- **ProcessMutation**: Handles Create, Delete, Update, Insert, Remove operations
- **Actor Registry**: Maps React tags to DALi Actors
- **Coordinate Mapping**: Converts React Native coordinates to DALi (TOP_LEFT origin)
- **Tree Management**: Maintains parent-child actor relationships

### 3. DALi Components (`src/components/`)

Component adapters mapping React primitives to DALi Toolkit Controls:

| React Component | DALi Control | Props Handled |
|-----------------|--------------|---------------|
| `<View>` | `Dali::Toolkit::Control` | backgroundColor, opacity, borderRadius |
| `<Text>` | `Dali::Toolkit::TextLabel` | text, fontSize, color, fontWeight |
| `<Image>` | `Dali::Toolkit::ImageView` | source (URI), resizeMode |

### 4. DaliTextLayoutManager

**Role**: Measures text for Yoga layout calculations.

Uses DALi's text rendering engine to compute text dimensions before layout, ensuring accurate text positioning.

### 5. JS Runtime (JavaScriptCore)

- **JavaScriptCore** - system-provided JSC on macOS
- Connected via **JSI** (JavaScript Interface)
- TurboModules for native module access
- RuntimeScheduler for async task execution

## Data Flow

### Render Pipeline

1. **JS Execution**: React executes, creating element tree
2. **Fabric Commit**: Element tree converted to C++ Shadow Tree
3. **Yoga Layout**: Computes (x, y, width, height) for all nodes
4. **Diffing**: Compares old/new trees, generates Mutations
5. **Mount Phase**: DaliMountingManager applies mutations to DALi scene

### Mutation Types

| Type | Action |
|------|--------|
| `Create` | `Dali::Actor::New()`, register in actor map |
| `Update` | Apply property changes (color, text, etc.) |
| `Insert` | Add actor to parent at index |
| `Remove` | Detach actor from parent |
| `Delete` | Destroy actor, remove from map |

## Coordinate System

DALi uses TOP_LEFT origin with direct coordinate mapping:

```
React Native Position (x, y) → DALi Position (x, y)
```

All actors use:
- `PARENT_ORIGIN: TOP_LEFT`
- `ANCHOR_POINT: TOP_LEFT`

## Threading Model

- **Main Thread**: All DALi API calls (rendering, actor manipulation)
- **JS Thread**: JSC execution (via RuntimeExecutor)
- **Synchronization**: Idle callbacks pump JS tasks on main thread

## Directory Structure

```
src/
├── DaliRenderer.cpp        # App entry, window setup
├── DaliMountingManager.cpp # Mutation processing
├── DeviceInstanceManager.cpp # JS runtime, scheduler
├── DaliTextLayoutManager.cpp # Text measurement
├── TurboModuleRegistry.cpp # Native modules
└── components/
    ├── DaliViewComponent.cpp
    ├── DaliTextComponent.cpp
    └── DaliImageComponent.cpp
```

## Memory Characteristics

| Component | Approximate Size |
|-----------|------------------|
| DALi Base | ~96-103 MB |
| JSC Runtime | ~15 MB |
| Fabric/Shadow Tree | ~5-10 MB |
| Per 1000 Views | ~6 MB additional |

Total RN overhead: ~22-24 MB over native DALi.
