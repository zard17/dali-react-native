#!/bin/bash
# Package dali-react-native for transfer to Ubuntu VM
# Run this on Mac to create a tarball

set -e

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
PROJECT_DIR="$(dirname "$SCRIPT_DIR")"
OUTPUT_DIR="${1:-$HOME/Desktop}"
TARBALL="$OUTPUT_DIR/dali-react-native-ubuntu.tar.gz"

echo "Packaging dali-react-native for Ubuntu..."
echo "Project: $PROJECT_DIR"
echo "Output: $TARBALL"

cd "$PROJECT_DIR"

# Create tarball excluding unnecessary files
tar -czvf "$TARBALL" \
    --exclude='build' \
    --exclude='node_modules' \
    --exclude='.git' \
    --exclude='*.o' \
    --exclude='*.a' \
    --exclude='dali-rn-demo' \
    --exclude='native-benchmark*' \
    --exclude='CMakeCache.txt' \
    --exclude='CMakeFiles' \
    --exclude='Makefile' \
    --exclude='cmake_install.cmake' \
    -C "$(dirname "$PROJECT_DIR")" \
    "$(basename "$PROJECT_DIR")"

echo ""
echo "=== Package created: $TARBALL ==="
echo ""
echo "Transfer to Ubuntu VM and extract:"
echo "  scp $TARBALL ubuntu-vm:~/"
echo "  # or use UTM shared folder"
echo ""
echo "On Ubuntu VM:"
echo "  cd ~"
echo "  tar -xzvf dali-react-native-ubuntu.tar.gz"
echo "  mv dali-react-native ~/dali-react-native"
echo "  ~/dali-react-native/scripts/ubuntu-build.sh build-rn"
echo "  ~/dali-react-native/scripts/ubuntu-build.sh run"
