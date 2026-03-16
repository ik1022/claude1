#!/bin/bash
# Local strict validation matching CI configuration
# Usage: bash scripts/local_strict_validation.sh

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"
BUILD_DIR="$PROJECT_ROOT/build/host_strict"

echo "=========================================="
echo "libemb Strict Validation Build"
echo "=========================================="
echo ""
echo "Configuration:"
echo "  Compiler: GCC"
echo "  Build Type: Debug"
echo "  Strict Flags: ON"
echo "  Extra Warnings: ON"
echo "  Coverage: ON"
echo "  ASAN: ON"
echo ""

# Clean previous build
if [ -d "$BUILD_DIR" ]; then
    echo "Cleaning previous build..."
    rm -rf "$BUILD_DIR"
fi

# Configure
echo "Configuring CMake..."
cmake -B "$BUILD_DIR" \
    -DCMAKE_CXX_COMPILER=g++ \
    -DLIBEMB_BUILD_TESTS=ON \
    -DLIBEMB_BUILD_EXAMPLES=ON \
    -DLIBEMB_STRICT_COMPILER_FLAGS=ON \
    -DLIBEMB_ENABLE_EXTRA_WARNINGS=ON \
    -DLIBEMB_ENABLE_COVERAGE=ON \
    -DLIBEMB_ENABLE_ASAN=ON \
    -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_CXX_FLAGS="-O0 -fprofile-arcs -ftest-coverage"

echo ""
echo "Building..."
cmake --build "$BUILD_DIR" --parallel $(nproc) --verbose

echo ""
echo "Running tests..."
ctest --test-dir "$BUILD_DIR" --output-on-failure --parallel $(nproc)

echo ""
echo "=========================================="
echo "✅ Strict Validation Passed!"
echo "=========================================="
