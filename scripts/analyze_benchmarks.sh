#!/bin/bash
# Analyze benchmark results and generate performance reports
#
# Usage:
#   bash scripts/analyze_benchmarks.sh [results_dir] [baseline_file]
#
# Arguments:
#   results_dir:   Directory containing benchmark results (default: benchmark_results)
#   baseline_file: JSON file with baseline results for comparison (optional)

set -e

RESULTS_DIR="${1:-benchmark_results}"
BASELINE_FILE="${2:-}"

# Color codes for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

echo "=== libemb Benchmark Analysis ==="
echo "Results directory: $RESULTS_DIR"
echo ""

if [ ! -d "$RESULTS_DIR" ]; then
    echo "❌ Results directory not found: $RESULTS_DIR"
    exit 1
fi

# Find the most recent benchmark results
latest_json=$(find "$RESULTS_DIR" -name "bench_*.json" -type f | sort | tail -1)

if [ -z "$latest_json" ]; then
    echo "❌ No benchmark results found in $RESULTS_DIR"
    exit 1
fi

echo "📊 Analyzing: $(basename $latest_json)"
echo ""

# Function to extract benchmark metrics from JSON
extract_metrics() {
    local json_file=$1
    local benchmark_name=$2

    if [ ! -f "$json_file" ]; then
        return
    fi

    # Extract benchmarks array and iterate through each benchmark
    grep '"name":' "$json_file" | while IFS= read -r line; do
        # Extract name
        name=$(echo "$line" | sed 's/.*"name": "\([^"]*\)".*/\1/')

        # Get the line number for this benchmark
        line_num=$(grep -n "\"name\": \"$name\"" "$json_file" | cut -d: -f1 | head -1)

        # Extract cpu_time and wall_time from surrounding lines
        cpu_time=$(sed -n "$((line_num)),+20p" "$json_file" | grep '"cpu_time"' | head -1 | sed 's/.*"cpu_time": \([^,]*\).*/\1/' | tr -d ' ')
        wall_time=$(sed -n "$((line_num)),+20p" "$json_file" | grep '"real_time"' | head -1 | sed 's/.*"real_time": \([^,]*\).*/\1/' | tr -d ' ')
        iterations=$(sed -n "$((line_num)),+20p" "$json_file" | grep '"iterations"' | head -1 | sed 's/.*"iterations": \([^,]*\).*/\1/' | tr -d ' ')

        if [ -n "$cpu_time" ] && [ "$cpu_time" != "0" ]; then
            printf "  %-50s | CPU: %10.2f ns | Iterations: %8s\n" "$name" "$cpu_time" "$iterations"
        fi
    done
}

# Extract and display benchmark data for each file
echo "📈 Performance Metrics:"
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"

for bench_file in "$RESULTS_DIR"/bench_*.json; do
    if [ -f "$bench_file" ]; then
        benchmark=$(basename "$bench_file" | sed 's/_[0-9]*.json//')
        echo ""
        echo "📌 $benchmark:"
        extract_metrics "$bench_file" "$benchmark"
    fi
done

echo ""
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"

# Compare against baseline if provided
if [ -n "$BASELINE_FILE" ] && [ -f "$BASELINE_FILE" ]; then
    echo ""
    echo "🔄 Comparison with Baseline:"
    echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"

    # Extract metrics from both files and compare
    echo "Note: Baseline comparison requires post-processing with external tools (jq, python)"
    echo "Baseline file: $BASELINE_FILE"
else
    echo ""
    echo "💡 Tip: Pass a baseline JSON file to compare results:"
    echo "   bash scripts/analyze_benchmarks.sh $RESULTS_DIR baseline_results.json"
fi

# Generate summary statistics
echo ""
echo "📋 Summary:"
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"

total_benchmarks=$(find "$RESULTS_DIR" -name "bench_*.json" -type f | wc -l)
total_tests=0

for bench_file in "$RESULTS_DIR"/bench_*.json; do
    if [ -f "$bench_file" ]; then
        count=$(grep -c '"name"' "$bench_file" 2>/dev/null || echo 0)
        total_tests=$((total_tests + count))
    fi
done

echo "Total benchmark files: $total_benchmarks"
echo "Total test cases: $total_tests"
echo "Latest run: $(ls -lh "$latest_json" | awk '{print $6, $7, $8}')"

# Save analysis to file
analysis_file="$RESULTS_DIR/analysis_$(date +%Y%m%d_%H%M%S).txt"
{
    echo "=== libemb Benchmark Analysis Report ==="
    echo "Generated: $(date)"
    echo ""
    echo "Results directory: $RESULTS_DIR"
    echo "Total files: $total_benchmarks"
    echo "Total test cases: $total_tests"
} > "$analysis_file"

echo ""
echo "✅ Analysis complete!"
echo "   Report: $analysis_file"
