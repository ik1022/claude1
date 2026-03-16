#!/bin/bash
# Run all performance benchmarks and collect results
#
# Usage:
#   bash scripts/run_benchmarks.sh [output_dir]
#
# Arguments:
#   output_dir: Directory to store benchmark results (default: benchmark_results)

set -e

OUTPUT_DIR=${1:-benchmark_results}
BUILD_DIR="build/benchmark"
TIMESTAMP=$(date +%Y%m%d_%H%M%S)

echo "=== libemb Performance Benchmarks ==="
echo "Output directory: $OUTPUT_DIR"
echo ""

# Create output directory
mkdir -p "$OUTPUT_DIR"

# Configure and build benchmarks if needed
if [ ! -d "$BUILD_DIR" ]; then
    echo "[1/4] Configuring benchmarks..."
    cmake -B "$BUILD_DIR" -DLIBEMB_BUILD_BENCHMARKS=ON > /dev/null 2>&1
fi

echo "[2/4] Building benchmarks..."
cmake --build "$BUILD_DIR" > /dev/null 2>&1

echo "[3/4] Running benchmarks..."

# Arrays to store benchmark names and paths
benchmarks=(
    "bench_ringbuffer"
    "bench_staticvector"
    "bench_driver"
)

# Run each benchmark and save results
for bench in "${benchmarks[@]}"; do
    bench_path="$BUILD_DIR/benchmark/$bench"

    if [ ! -f "$bench_path" ]; then
        echo "⚠ Benchmark not found: $bench_path"
        continue
    fi

    output_file="$OUTPUT_DIR/${bench}_${TIMESTAMP}.json"
    csv_file="$OUTPUT_DIR/${bench}_${TIMESTAMP}.csv"

    echo "  Running: $bench"

    # Run benchmark and save JSON output
    "$bench_path" \
        --benchmark_format=json \
        --benchmark_out="$output_file" \
        --benchmark_out_format=json \
        > /dev/null 2>&1 || true

    # Also run with CSV format (if supported)
    "$bench_path" \
        --benchmark_format=csv \
        > "$csv_file" 2>&1 || true

    # Count results
    if [ -f "$output_file" ]; then
        count=$(grep -c '"name"' "$output_file" 2>/dev/null || echo 0)
        echo "    ✓ Saved: $output_file ($count tests)"
    fi
done

echo ""
echo "[4/4] Generating summary..."

# Create summary file
summary_file="$OUTPUT_DIR/summary_${TIMESTAMP}.txt"
{
    echo "=== libemb Benchmark Summary ==="
    echo "Timestamp: $(date)"
    echo ""
    echo "Benchmarks Run:"
    for bench in "${benchmarks[@]}"; do
        if [ -f "$OUTPUT_DIR/${bench}_${TIMESTAMP}.json" ]; then
            count=$(grep -c '"name"' "$OUTPUT_DIR/${bench}_${TIMESTAMP}.json" 2>/dev/null || echo 0)
            echo "  ✓ $bench: $count test cases"
        fi
    done
    echo ""
    echo "Output Files:"
    ls -lh "$OUTPUT_DIR"/${*}_"${TIMESTAMP}".* 2>/dev/null | awk '{print "  " $9 " (" $5 ")"}' || true
} > "$summary_file"

cat "$summary_file"

echo ""
echo "✅ Benchmarks completed!"
echo "   Results: $OUTPUT_DIR/"
echo "   Summary: $summary_file"
