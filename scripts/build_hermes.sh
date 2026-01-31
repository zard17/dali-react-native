#!/bin/bash
#
# Download and install prebuilt Hermes for macOS
# Downloads from React Native's Maven repository
#

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_DIR="$(dirname "$SCRIPT_DIR")"
DEPS_DIR="$PROJECT_DIR/third_party_dependencies"
INSTALL_DIR="$DEPS_DIR/hermes-install"
DOWNLOAD_DIR="$DEPS_DIR/hermes-prebuilt"

# Hermes version matching React Native
# Check .hermesversion file for the pinned version
HERMES_VERSION="0.14.0"

# Maven URL for prebuilt Hermes iOS (includes macOS framework)
MAVEN_URL="https://repo1.maven.org/maven2/com/facebook/hermes/hermes-ios"
TARBALL_URL="${MAVEN_URL}/${HERMES_VERSION}/hermes-ios-${HERMES_VERSION}-hermes-ios-debug.tar.gz"

echo "==================================="
echo "  Installing Hermes for macOS"
echo "==================================="
echo ""
echo "Project dir: $PROJECT_DIR"
echo "Install dir: $INSTALL_DIR"
echo "Hermes version: $HERMES_VERSION"
echo ""

# Step 1: Create directories
mkdir -p "$DOWNLOAD_DIR"
mkdir -p "$INSTALL_DIR"

# Step 2: Download tarball if not exists
TARBALL_PATH="$DOWNLOAD_DIR/hermes-ios-${HERMES_VERSION}.tar.gz"
if [ ! -f "$TARBALL_PATH" ]; then
    echo ">>> Downloading Hermes ${HERMES_VERSION} from Maven..."
    curl -L "$TARBALL_URL" -o "$TARBALL_PATH"
else
    echo ">>> Using cached tarball: $TARBALL_PATH"
fi

# Step 3: Extract tarball
echo ""
echo ">>> Extracting tarball..."
cd "$DOWNLOAD_DIR"
rm -rf destroot  # Clean previous extraction
tar -xzf "$TARBALL_PATH"

# Step 4: Install to hermes-install directory
echo ""
echo ">>> Installing Hermes..."
rm -rf "$INSTALL_DIR"
mkdir -p "$INSTALL_DIR/lib" "$INSTALL_DIR/bin" "$INSTALL_DIR/include"

# Copy headers
cp -R destroot/include/* "$INSTALL_DIR/include/"

# Copy macOS framework
cp -R destroot/Library/Frameworks/macosx/hermesvm.framework "$INSTALL_DIR/lib/"

# Copy hermesc compiler
cp destroot/bin/hermesc "$INSTALL_DIR/bin/"
chmod +x "$INSTALL_DIR/bin/hermesc"

# Step 5: Verify installation
echo ""
echo ">>> Verifying installation..."

if [ -d "$INSTALL_DIR/lib/hermesvm.framework" ]; then
    echo "✓ hermesvm.framework found"
else
    echo "✗ hermesvm.framework not found!"
    exit 1
fi

if [ -f "$INSTALL_DIR/include/hermes/hermes.h" ]; then
    echo "✓ Hermes headers found"
else
    echo "✗ Hermes headers not found!"
    exit 1
fi

if [ -f "$INSTALL_DIR/bin/hermesc" ]; then
    echo "✓ hermesc compiler found"
else
    echo "✗ hermesc compiler not found!"
    exit 1
fi

# Show framework info
echo ""
echo ">>> Framework info:"
file "$INSTALL_DIR/lib/hermesvm.framework/hermesvm"

echo ""
echo "==================================="
echo "  Hermes installation complete!"
echo "==================================="
echo ""
echo "Framework: $INSTALL_DIR/lib/hermesvm.framework"
echo "Headers: $INSTALL_DIR/include/"
echo "Compiler: $INSTALL_DIR/bin/hermesc"
echo ""
echo "To use in CMake:"
echo "  find_package(Hermes REQUIRED)"
echo "  target_link_libraries(\${TARGET} Hermes::hermes)"
echo ""
