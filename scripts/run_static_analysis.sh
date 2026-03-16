#!/bin/bash
# Run static analysis tools locally
# Usage: bash scripts/run_static_analysis.sh [output_dir]

set -e

OUTPUT_DIR="${1:-.static_analysis}"

mkdir -p "$OUTPUT_DIR"

echo "=== libemb Static Analysis ==="
echo "Output directory: $OUTPUT_DIR"
echo ""

# Check for required tools
check_tool() {
    if ! command -v "$1" &> /dev/null; then
        echo "❌ $1 not found. Install it first:"
        echo "   sudo apt-get install $2"
        return 1
    fi
    echo "✓ $1 found"
    return 0
}

echo "[1/3] Checking dependencies..."
tools_ok=true
check_tool cppcheck cppcheck || tools_ok=false
check_tool clang-tidy clang-tools || tools_ok=false

if [ "$tools_ok" = false ]; then
    echo ""
    echo "Install missing tools and try again."
    exit 1
fi
echo ""

# Run CPPCheck
echo "[2/3] Running CPPCheck..."
mkdir -p "$OUTPUT_DIR/cppcheck"

cppcheck \
    --enable=all \
    --suppress=missingIncludeSystem \
    --suppress=unusedFunction \
    --suppress=unmatchedSuppression \
    --std=c++17 \
    --platform=unix64 \
    --inline-suppr \
    --xml \
    --xml-version=2 \
    --output-file="$OUTPUT_DIR/cppcheck/cppcheck.xml" \
    include/ src/ port/ benchmark/ test/ 2>&1 || true

# Human-readable cppcheck report
cppcheck \
    --enable=all \
    --suppress=missingIncludeSystem \
    --suppress=unusedFunction \
    --suppress=unmatchedSuppression \
    --std=c++17 \
    --platform=unix64 \
    --inline-suppr \
    include/ src/ port/ benchmark/ test/ \
    2>&1 | tee "$OUTPUT_DIR/cppcheck/cppcheck.txt" || true

echo "✓ CPPCheck completed"
echo ""

# Run Clang-Tidy
echo "[3/3] Running Clang-Tidy..."
mkdir -p "$OUTPUT_DIR/clang-tidy"

if [ ! -d "build" ]; then
    echo "Building with clang++ for tidy analysis..."
    cmake -B build \
        -DCMAKE_CXX_COMPILER=clang++ \
        -DCMAKE_CXX_CLANG_TIDY="clang-tidy;-checks=*,-fuchsia-*,-google-*,-llvm-*" \
        -DLIBEMB_BUILD_TESTS=ON \
        -DLIBEMB_BUILD_BENCHMARKS=OFF \
        -DCMAKE_BUILD_TYPE=Release
fi

cmake --build build 2>&1 | tee "$OUTPUT_DIR/clang-tidy/clang-tidy.log" || true

echo "✓ Clang-Tidy completed"
echo ""

# Generate summary
echo "[Summary]"
echo "========================================="

# CPPCheck statistics
echo ""
echo "CPPCheck Results:"
if [ -f "$OUTPUT_DIR/cppcheck/cppcheck.xml" ]; then
    error_count=$(grep -c '<error severity="error"' "$OUTPUT_DIR/cppcheck/cppcheck.xml" 2>/dev/null || echo 0)
    warning_count=$(grep -c '<error severity="warning"' "$OUTPUT_DIR/cppcheck/cppcheck.xml" 2>/dev/null || echo 0)
    style_count=$(grep -c '<error severity="style"' "$OUTPUT_DIR/cppcheck/cppcheck.xml" 2>/dev/null || echo 0)

    echo "  Errors: $error_count"
    echo "  Warnings: $warning_count"
    echo "  Style issues: $style_count"
else
    echo "  (No results)"
fi

# Clang-Tidy statistics
echo ""
echo "Clang-Tidy Results:"
if [ -f "$OUTPUT_DIR/clang-tidy/clang-tidy.log" ]; then
    warning_count=$(grep -c "warning:" "$OUTPUT_DIR/clang-tidy/clang-tidy.log" 2>/dev/null || echo 0)
    error_count=$(grep -c "error:" "$OUTPUT_DIR/clang-tidy/clang-tidy.log" 2>/dev/null || echo 0)

    echo "  Warnings: $warning_count"
    echo "  Errors: $error_count"

    # Show first 10 warnings
    echo ""
    echo "  Top warnings:"
    grep "warning:" "$OUTPUT_DIR/clang-tidy/clang-tidy.log" | head -10 | sed 's/^/    /'
else
    echo "  (No results)"
fi

echo ""
echo "========================================="
echo "✅ Static analysis complete!"
echo "   Results: $OUTPUT_DIR/"
echo ""
echo "Next steps:"
echo "  1. Review results in $OUTPUT_DIR/"
echo "  2. Fix critical issues (errors)"
echo "  3. Consider fixing warnings"
echo "  4. Apply clang-tidy fixes: clang-tidy --fix <file>"
