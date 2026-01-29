# DALi Mac Port Build Instructions

This guide outlines the steps to build the DALi stack (Core, Adaptor, Toolkit, and Demo) on macOS (Apple Silicon/ARM64).

## Prerequisites

1.  **Xcode Command Line Tools**: Ensure these are installed (`xcode-select --install`).
2.  **Homebrew**: Recommended for installing system dependencies like `glib` if needed.
3.  **VCPKG**: Used for managing dependencies (`unofficial-angle`, `curl`, `png`, etc.).
    *   Location assumed: `~/tizen/macos-dependencies` or `~/tizen/vcpkg`.
    *   In this guide, we assume: `~/tizen/vcpkg`.

## Directory Structure

This guide assumes the following directory structure:
```
~/tizen/
├── dali-env/           # Installation prefix
├── vcpkg/              # VCPKG dependency manager
├── dali-core/
├── dali-adaptor/
├── dali-toolkit/
└── dali-demo/
```

## 1. Environment Setup

Set up the necessary environment variables. You can add these to your shell profile or run them in your current session.

```bash
export DESKTOP_PREFIX=$HOME/tizen/dali-env
export VCPKG_FOLDER=$HOME/tizen/vcpkg
export PATH=$DESKTOP_PREFIX/bin:$PATH
export DYLD_LIBRARY_PATH=$DESKTOP_PREFIX/lib:$DYLD_LIBRARY_PATH
```

## 2. Dependencies (VCPKG)

Ensure installed dependencies are for the `arm64-osx` triplet.
```bash
cd $VCPKG_FOLDER
./vcpkg install unofficial-angle curl libpng freetype harfbuzz --triplet arm64-osx
```

## 3. Build DALi Core

```bash
cd ~/tizen/dali-core/build/tizen
cmake -DCMAKE_INSTALL_PREFIX=$DESKTOP_PREFIX \
      -DCMAKE_TOOLCHAIN_FILE=$VCPKG_FOLDER/scripts/buildsystems/vcpkg.cmake \
      -DINSTALL_CMAKE_MODULES=ON \
      .
make install -j8
```

## 4. Build DALi Adaptor

Adaptor requires the `MACOS` profile.

```bash
cd ~/tizen/dali-adaptor/build/tizen
cmake -DCMAKE_INSTALL_PREFIX=$DESKTOP_PREFIX \
      -DCMAKE_TOOLCHAIN_FILE=$VCPKG_FOLDER/scripts/buildsystems/vcpkg.cmake \
      -DINSTALL_CMAKE_MODULES=ON \
      -DENABLE_PROFILE=MACOS \
      -DPROFILE_LCASE=macos \
      .
make install -j8
```

## 5. Build DALi Toolkit

Toolkit requires explicit `arm64-osx` triplet in some configurations to find dependencies correctly.

```bash
cd ~/tizen/dali-toolkit/build/tizen
cmake -DCMAKE_INSTALL_PREFIX=$DESKTOP_PREFIX \
      -DCMAKE_TOOLCHAIN_FILE=$VCPKG_FOLDER/scripts/buildsystems/vcpkg.cmake \
      -DINSTALL_CMAKE_MODULES=ON \
      .
make install -j8
```

## 6. Build DALi Demo

The Demo build requires turning off internationalization to avoid `gettext`/`msgfmt` issues and specifying the VCPKG triplet and prefix path explicitly to ensure `unofficial-angle` is found.

```bash
cd ~/tizen/dali-demo/build/tizen
cmake -DCMAKE_INSTALL_PREFIX=$DESKTOP_PREFIX \
      -DCMAKE_TOOLCHAIN_FILE=$VCPKG_FOLDER/scripts/buildsystems/vcpkg.cmake \
      -DVCPKG_TARGET_TRIPLET=arm64-osx \
      -DINTERNATIONALIZATION=OFF \
      -DCMAKE_PREFIX_PATH="$VCPKG_FOLDER/installed/arm64-osx;$VCPKG_FOLDER/installed/arm64-osx/share/unofficial-angle" \
      .
make install -j8
```

## 7. Running the Demo

To run the examples, ensure `DYLD_LIBRARY_PATH` includes your installation lib directory.

```bash
export DYLD_LIBRARY_PATH=$DESKTOP_PREFIX/lib:$DYLD_LIBRARY_PATH
$DESKTOP_PREFIX/bin/hello-world.example
```

You should see the DALi application window utilizing the GLES backend (ANGLE).
