#!/bin/bash
set -e

ROOT=$(pwd)
DEPS_DIR="$ROOT/third_party_dependencies"
INSTALL_DIR="$DEPS_DIR/install"
mkdir -p "$INSTALL_DIR"

# Helper for cmake build
function build_cmake {
  NAME=$1
  SRC_DIR=$2
  shift 2
  ARGS=("$@")

  echo "========================================"
  echo "Building $NAME..."
  echo "Source: $SRC_DIR"
  echo "Args: ${ARGS[@]}"
  echo "========================================"
  
  if [ ! -d "$SRC_DIR" ]; then
      echo "Error: Directory $SRC_DIR does not exist."
      exit 1
  fi

  rm -rf "$SRC_DIR/build"
  mkdir -p "$SRC_DIR/build"
  cd "$SRC_DIR/build"
  # Standardize flags: Release, PIC, Static
  cmake .. \
      -DCMAKE_INSTALL_PREFIX="$INSTALL_DIR" \
      -DCMAKE_BUILD_TYPE=Release \
      -DCMAKE_POSITION_INDEPENDENT_CODE=ON \
      -DBUILD_SHARED_LIBS=OFF \
      "${ARGS[@]}"
      
  make -j4
  make install
  cd "$ROOT"
}

# 1. Fmt (Build first as it is simple)
build_cmake "Fmt" "$DEPS_DIR/fmt" -DFMT_TEST=OFF

# 2. Double Conversion
build_cmake "Double Conversion" "$DEPS_DIR/double-conversion"

# 3. Glog
# Disable gflags, tests, examples
build_cmake "Glog" "$DEPS_DIR/glog" -DWITH_GFLAGS=OFF -DBUILD_TESTING=OFF -DWITH_UNWIND=OFF

# 3.1 FastFloat
build_cmake "FastFloat" "$DEPS_DIR/fast_float" -DFASTFLOAT_TEST=OFF

# 3.2 LibEvent
build_cmake "LibEvent" "$DEPS_DIR/libevent" -DEVENT__DISABLE_OPENSSL=ON -DEVENT__DISABLE_REGRESS=ON -DEVENT__DISABLE_TESTS=ON



# 3.5 Boost
# Build required libraries for Folly
echo "Building Boost..."
BOOST_SRC="$DEPS_DIR/boost_1_83_0"
if [ -d "$BOOST_SRC" ]; then
    cd "$BOOST_SRC"
    if [ ! -f "./b2" ]; then
        ./bootstrap.sh --prefix="$INSTALL_DIR"
    fi
    ./b2 install \
        link=static \
        threading=multi \
        runtime-link=shared \
        variant=release \
        cxxflags="-fPIC -std=c++20" \
        --with-context \
        --with-filesystem \
        --with-program_options \
        --with-regex \
        --with-system \
        --with-thread \
        -j4
    cd "$ROOT"
else
    echo "Boost source not found at $BOOST_SRC"
    exit 1
fi

# 4. Folly
# Folly requires Boost, Glog, DoubleVerify, Fmt
echo "Building Folly..."
# Ensure folly-config.h is in place (it should be from download_deps.sh)
build_cmake "Folly" "$DEPS_DIR/folly" \
    -DCMAKE_PREFIX_PATH="$INSTALL_DIR" \
    -DBOOST_ROOT="$DEPS_DIR/boost_1_83_0" \
    -DBoost_INCLUDE_DIR="$DEPS_DIR/boost_1_83_0" \
    -DBoost_NO_SYSTEM_PATHS=ON \
    -DBOOST_LINK_STATIC=ON \
    -DCMAKE_CXX_STANDARD=20 \
    -DCMAKE_CXX_FLAGS="-DFOLLY_MOBILE=1 -DFOLLY_USE_LIBCPP=1 -DFOLLY_CFG_NO_COROUTINES=1 -DFOLLY_HAVE_CLOCK_GETTIME=1 -Wno-comma -Wno-shorten-64-to-32" \
    -DBUILD_TESTS=OFF \
    -DFOLLY_ASAN=OFF


echo "Core dependencies built successfully."
echo "Artifacts installed to $INSTALL_DIR"

# Yoga
echo "Building Yoga..."
NODE_MODULES_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/node_modules" && pwd)"
YOGA_DIR="$NODE_MODULES_DIR/react-native/ReactCommon/yoga"

# Manual CMake build for Yoga
if [ ! -d "$YOGA_DIR/build" ]; then
    mkdir -p "$YOGA_DIR/build"
fi
cd "$YOGA_DIR/build"
# Use -S . since we are building from YOGA_DIR/build referencing .. usually?
# No, YOGA_DIR is the source root (containing CMakeLists.txt).
# So -S $YOGA_DIR -B .
cmake -S "$YOGA_DIR" -B . \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_CXX_STANDARD=17 \
    -DCMAKE_POSITION_INDEPENDENT_CODE=ON \
    -DCMAKE_INSTALL_PREFIX="$DEPS_DIR/install"

cmake --build . --config Release -j$(nproc)

# Manual Install
echo "Installing Yoga..."
cp yoga/libyogacore.a "$DEPS_DIR/install/lib/"
mkdir -p "$DEPS_DIR/install/include/yoga"
cp "$YOGA_DIR/yoga/"*.h "$DEPS_DIR/install/include/yoga/"
