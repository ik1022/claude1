#!/bin/bash
# Quick validation - fast check before committing
# Usage: bash scripts/quick_validation.sh

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"
BUILD_DIR="$PROJECT_ROOT/build/quick"

echo "=========================================="
echo "libemb Quick Validation"
echo "=========================================="
echo ""
echo "Configuration:"
echo "  Compiler: Default (GCC/Clang)"
echo "  Build Type: Release"
echo "  Strict Flags: ON"
echo "  Tests: ON"
echo ""

# Clean previous build
if [ -d "$BUILD_DIR" ]; then
    echo "Cleaning previous build..."
    rm -rf "$BUILD_DIR"
fi

# Configure
echo "Configuring CMake..."
cmake -B "$BUILD_DIR" \
    -DLIBEMB_BUILD_TESTS=ON \
    -DLIBEMB_STRICT_COMPILER_FLAGS=ON \
    -DCMAKE_BUILD_TYPE=Release

echo ""
echo "Building..."
cmake --build "$BUILD_DIR" --parallel $(nproc)

echo ""
echo "Running tests..."
ctest --test-dir "$BUILD_DIR" --output-on-failure -j $(nproc)

echo ""
echo "=========================================="
echo "✅ Quick Validation Passed!"
echo "=========================================="
echo ""
echo "Next: Push your changes or run full validation:"
echo "  bash scripts/local_strict_validation.sh"
