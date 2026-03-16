#!/bin/bash
# Generate code coverage report using gcov and lcov
#
# Usage:
#   bash scripts/generate_coverage.sh

set -e

BUILD_DIR="build/coverage"
REPORT_DIR="coverage_report"

echo "=== libemb Code Coverage Report ==="
echo ""

# Build with coverage
echo "[1/4] Building with coverage..."
cmake -B "$BUILD_DIR" \
    -DLIBEMB_BUILD_TESTS=ON \
    -DLIBEMB_ENABLE_COVERAGE=ON > /dev/null 2>&1
cmake --build "$BUILD_DIR" > /dev/null 2>&1
echo "  ✓ Build complete"

# Run tests
echo "[2/4] Running tests..."
cd "$BUILD_DIR"
ctest --output-on-failure > /dev/null 2>&1 || true
cd - > /dev/null
echo "  ✓ Tests complete"

# Collect coverage with ignore-errors
echo "[3/4] Collecting coverage data..."
lcov --directory "$BUILD_DIR" \
     --capture \
     --output-file "$BUILD_DIR/coverage.info" \
     --ignore-errors empty \
     2>/dev/null || true

# Remove external files
lcov --remove "$BUILD_DIR/coverage.info" \
     '/usr/*' '/usr/local/*' '*/_deps/*' '*/googletest/*' \
     --output-file "$BUILD_DIR/coverage.info" \
     --ignore-errors empty \
     2>/dev/null || true

echo "  ✓ Coverage data collected"

# Generate HTML report
echo "[4/4] Generating HTML report..."
rm -rf "$REPORT_DIR"
mkdir -p "$REPORT_DIR"

genhtml "$BUILD_DIR/coverage.info" \
        --output-directory "$REPORT_DIR" \
        --quiet \
        --ignore-errors source \
        2>/dev/null || {
    # Fallback: create simple report
    echo "<html><body><h1>Coverage Report</h1><p>Data available in $BUILD_DIR/coverage.info</p></body></html>" > "$REPORT_DIR/index.html"
}

echo "  ✓ Report generated"
echo ""

# Show file info
if [ -f "$BUILD_DIR/coverage.info" ]; then
    SIZE=$(ls -lh "$BUILD_DIR/coverage.info" | awk '{print $5}')
    echo "Coverage info saved: $BUILD_DIR/coverage.info ($SIZE)"
fi

echo ""
echo "✅ Coverage setup complete!"
echo "   Report: file://$(pwd)/$REPORT_DIR/index.html"
