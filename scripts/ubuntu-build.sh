#!/bin/bash
# Ubuntu Build Script for DALi + React Native
# Run on fresh Ubuntu 22.04 LTS

set -e

echo "=== DALi React Native Ubuntu Build Script ==="
echo ""

# Configuration
DALI_PREFIX="$HOME/dali-env"
BUILD_DIR="$HOME/dali-build"
JOBS=$(nproc)

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

log_info() { echo -e "${GREEN}[INFO]${NC} $1"; }
log_warn() { echo -e "${YELLOW}[WARN]${NC} $1"; }
log_error() { echo -e "${RED}[ERROR]${NC} $1"; }

# Step 1: Install system dependencies
install_dependencies() {
    log_info "Installing system dependencies..."

    sudo apt update
    sudo apt install -y \
        git \
        cmake \
        build-essential \
        ninja-build \
        pkg-config \
        libgles2-mesa-dev \
        libegl1-mesa-dev \
        libgbm-dev \
        libdrm-dev \
        libx11-dev \
        libxcursor-dev \
        libxi-dev \
        libxrandr-dev \
        libxinerama-dev \
        libxkbcommon-dev \
        libwayland-dev \
        wayland-protocols \
        libpng-dev \
        libjpeg-dev \
        libgif-dev \
        libturbojpeg0-dev \
        libfreetype6-dev \
        libfontconfig1-dev \
        libharfbuzz-dev \
        libcurl4-openssl-dev \
        libssl-dev \
        libwebp-dev \
        libhyphen-dev \
        libtiff-dev \
        libexif-dev \
        libcairo2-dev \
        libpango1.0-dev \
        libgdk-pixbuf2.0-dev \
        libatspi2.0-dev \
        libdbus-1-dev \
        libvulkan-dev \
        gettext \
        curl \
        nodejs \
        npm

    log_info "Dependencies installed successfully"
}

# Step 2: Create directories
setup_directories() {
    log_info "Setting up directories..."

    mkdir -p "$DALI_PREFIX"/{lib,include,share,bin}
    mkdir -p "$BUILD_DIR"

    log_info "Directories created"
}

# Step 3: Clone DALi repositories
clone_repos() {
    log_info "Cloning DALi repositories..."

    cd "$BUILD_DIR"

    if [ ! -d "dali-core" ]; then
        git clone https://github.com/puru666/dali-core.git
    else
        log_warn "dali-core already exists, skipping clone"
    fi

    if [ ! -d "dali-adaptor" ]; then
        git clone https://github.com/puru666/dali-adaptor.git
    else
        log_warn "dali-adaptor already exists, skipping clone"
    fi

    if [ ! -d "dali-toolkit" ]; then
        git clone https://github.com/puru666/dali-toolkit.git
    else
        log_warn "dali-toolkit already exists, skipping clone"
    fi

    log_info "Repositories cloned"
}

# Step 4: Build dali-core
build_dali_core() {
    log_info "Building dali-core..."

    cd "$BUILD_DIR/dali-core"

    mkdir -p build && cd build

    cmake .. \
        -DCMAKE_INSTALL_PREFIX="$DALI_PREFIX" \
        -DCMAKE_BUILD_TYPE=Release \
        -DENABLE_PKG_CONFIGURE=ON \
        -DENABLE_COVERAGE=OFF

    make -j$JOBS
    make install

    log_info "dali-core built successfully"
}

# Step 5: Build dali-adaptor
build_dali_adaptor() {
    log_info "Building dali-adaptor..."

    cd "$BUILD_DIR/dali-adaptor"

    mkdir -p build && cd build

    export PKG_CONFIG_PATH="$DALI_PREFIX/lib/pkgconfig:$PKG_CONFIG_PATH"

    cmake .. \
        -DCMAKE_INSTALL_PREFIX="$DALI_PREFIX" \
        -DCMAKE_BUILD_TYPE=Release \
        -DENABLE_PKG_CONFIGURE=ON \
        -DENABLE_PROFILE=UBUNTU \
        -DENABLE_GLES_VERSION=30 \
        -DENABLE_COVERAGE=OFF \
        -DENABLE_TRACE=OFF

    make -j$JOBS
    make install

    log_info "dali-adaptor built successfully"
}

# Step 6: Build dali-toolkit
build_dali_toolkit() {
    log_info "Building dali-toolkit..."

    cd "$BUILD_DIR/dali-toolkit"

    mkdir -p build && cd build

    export PKG_CONFIG_PATH="$DALI_PREFIX/lib/pkgconfig:$PKG_CONFIG_PATH"

    cmake .. \
        -DCMAKE_INSTALL_PREFIX="$DALI_PREFIX" \
        -DCMAKE_BUILD_TYPE=Release \
        -DENABLE_PKG_CONFIGURE=ON \
        -DENABLE_COVERAGE=OFF

    make -j$JOBS
    make install

    log_info "dali-toolkit built successfully"
}

# Step 7: Setup environment
setup_environment() {
    log_info "Setting up environment..."

    ENV_SCRIPT="$DALI_PREFIX/dali-env.sh"

    cat > "$ENV_SCRIPT" << EOF
# DALi Environment Setup
export DALI_PREFIX="$DALI_PREFIX"
export PATH="\$DALI_PREFIX/bin:\$PATH"
export LD_LIBRARY_PATH="\$DALI_PREFIX/lib:\$LD_LIBRARY_PATH"
export PKG_CONFIG_PATH="\$DALI_PREFIX/lib/pkgconfig:\$PKG_CONFIG_PATH"

# DALi runtime settings
export DALI_APPLICATION_PACKAGE="\$DALI_PREFIX/share/dali"
export DALI_IMAGE_DIR="\$DALI_PREFIX/share/dali/images"
export DALI_SOUND_DIR="\$DALI_PREFIX/share/dali/sounds"
export DALI_STYLE_DIR="\$DALI_PREFIX/share/dali/styles"
export DALI_STYLE_IMAGE_DIR="\$DALI_PREFIX/share/dali/styles/images"

echo "DALi environment loaded from \$DALI_PREFIX"
EOF

    # Add to bashrc if not already present
    if ! grep -q "dali-env.sh" "$HOME/.bashrc"; then
        echo "" >> "$HOME/.bashrc"
        echo "# DALi Environment" >> "$HOME/.bashrc"
        echo "source $ENV_SCRIPT" >> "$HOME/.bashrc"
    fi

    log_info "Environment setup complete"
    log_info "Run: source $ENV_SCRIPT"
}

# Step 8: Build dali-react-native
build_dali_react_native() {
    log_info "Building dali-react-native..."

    # Check if project exists
    RN_DIR="$HOME/dali-react-native"
    if [ ! -d "$RN_DIR" ]; then
        log_warn "dali-react-native not found at $RN_DIR"
        log_info "Please copy dali-react-native project to $RN_DIR"
        log_info "Then run: $0 build-rn"
        return 1
    fi

    cd "$RN_DIR"

    # Install npm dependencies
    npm install

    # Bundle JS
    npx react-native bundle --platform ios --dev false --entry-file index.js --bundle-output build/bundle.js

    # Build native
    mkdir -p build && cd build

    export PKG_CONFIG_PATH="$DALI_PREFIX/lib/pkgconfig:$PKG_CONFIG_PATH"

    cmake .. \
        -DCMAKE_BUILD_TYPE=Debug \
        -DCMAKE_PREFIX_PATH="$DALI_PREFIX"

    make -j$JOBS

    # Copy bundle to build
    cp ../bundle.js . 2>/dev/null || true

    log_info "dali-react-native built successfully"
}

# Step 9: Run test
run_test() {
    log_info "Running dali-react-native test..."

    source "$DALI_PREFIX/dali-env.sh"

    RN_DIR="$HOME/dali-react-native/build"
    if [ ! -f "$RN_DIR/dali-rn-demo" ]; then
        log_error "dali-rn-demo not found. Build first with: $0 build-rn"
        return 1
    fi

    cd "$RN_DIR"
    ./dali-rn-demo
}

# Main execution
case "${1:-all}" in
    deps)
        install_dependencies
        ;;
    setup)
        setup_directories
        ;;
    clone)
        clone_repos
        ;;
    dali-core)
        build_dali_core
        ;;
    dali-adaptor)
        build_dali_adaptor
        ;;
    dali-toolkit)
        build_dali_toolkit
        ;;
    dali)
        build_dali_core
        build_dali_adaptor
        build_dali_toolkit
        ;;
    env)
        setup_environment
        ;;
    build-rn)
        build_dali_react_native
        ;;
    run)
        run_test
        ;;
    all)
        install_dependencies
        setup_directories
        clone_repos
        build_dali_core
        build_dali_adaptor
        build_dali_toolkit
        setup_environment
        log_info ""
        log_info "=== DALi build complete! ==="
        log_info ""
        log_info "Next steps:"
        log_info "1. Copy dali-react-native project to ~/dali-react-native"
        log_info "2. Run: source ~/dali-env/dali-env.sh"
        log_info "3. Run: $0 build-rn"
        log_info "4. Run: $0 run"
        ;;
    *)
        echo "Usage: $0 {all|deps|setup|clone|dali|dali-core|dali-adaptor|dali-toolkit|env|build-rn|run}"
        echo ""
        echo "Commands:"
        echo "  all         - Full build (default)"
        echo "  deps        - Install system dependencies only"
        echo "  setup       - Create directories only"
        echo "  clone       - Clone DALi repos only"
        echo "  dali        - Build all DALi components"
        echo "  dali-core   - Build dali-core only"
        echo "  dali-adaptor - Build dali-adaptor only"
        echo "  dali-toolkit - Build dali-toolkit only"
        echo "  env         - Setup environment variables"
        echo "  build-rn    - Build dali-react-native"
        echo "  run         - Run dali-rn-demo"
        exit 1
        ;;
esac

log_info "Done!"
