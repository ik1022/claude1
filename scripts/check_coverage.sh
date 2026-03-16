#!/bin/bash
# Check if code coverage meets minimum threshold
#
# Usage:
#   bash scripts/check_coverage.sh [threshold]
#
# Arguments:
#   threshold: Minimum coverage percentage (default: 70)

THRESHOLD=${1:-70}
BUILD_DIR="build/coverage"
COVERAGE_FILE="$BUILD_DIR/coverage.info"

echo "=== Code Coverage Threshold Check ==="
echo "Threshold: ${THRESHOLD}%"
echo ""

# Check if coverage file exists
if [ ! -f "$COVERAGE_FILE" ]; then
    echo "⚠ Coverage file not found: $COVERAGE_FILE"
    echo "Run: bash scripts/generate_coverage.sh"
    exit 1
fi

# Extract coverage percentage
COVERAGE=$(lcov --list "$COVERAGE_FILE" 2>/dev/null | grep TOTAL | tail -1 | awk '{print $NF}' | sed 's/%//')

# Handle case when coverage data is empty
if [ -z "$COVERAGE" ] || [ "$COVERAGE" = "" ]; then
    COVERAGE=0
fi

echo "Current Coverage: ${COVERAGE}%"
echo ""

# Check threshold
if [ -z "$COVERAGE" ] || [ "$COVERAGE" -eq 0 ]; then
    echo "⚠ No coverage data available"
    exit 1
elif (( $(echo "$COVERAGE >= $THRESHOLD" | bc -l) )); then
    echo "✅ Coverage meets threshold! (${COVERAGE}% >= ${THRESHOLD}%)"
    exit 0
else
    echo "❌ Coverage below threshold! (${COVERAGE}% < ${THRESHOLD}%)"
    echo "   Required: ${THRESHOLD}%"
    echo "   Current: ${COVERAGE}%"
    echo "   Missing: $(echo "$THRESHOLD - $COVERAGE" | bc)%"
    exit 1
fi
