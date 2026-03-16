#!/bin/bash
# Apply Clang-Tidy auto-fixes locally
# Usage: bash scripts/apply_clang_tidy_fixes.sh [options]
#
# Options:
#   --dry-run      Show what would be fixed without modifying files
#   --apply        Apply all fixes (modifies files)
#   --safe-only    Apply only safe fixes (no naming changes)
#   --review       Interactive review mode
#   --help         Show this help message
#
# Examples:
#   bash scripts/apply_clang_tidy_fixes.sh --dry-run
#   bash scripts/apply_clang_tidy_fixes.sh --review
#   bash scripts/apply_clang_tidy_fixes.sh --apply

set -e

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Options
MODE="dry-run"
SAFE_ONLY=0
REVIEW_MODE=0
CHECKS="*,-fuchsia-*,-google-*,-llvm-*"

# Parse arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        --apply)
            MODE="apply"
            shift
            ;;
        --dry-run)
            MODE="dry-run"
            shift
            ;;
        --safe-only)
            SAFE_ONLY=1
            # Exclude risky fixes
            CHECKS="*,-fuchsia-*,-google-*,-llvm-*,-readability-identifier-naming,-modernize-use-trailing-return-type"
            shift
            ;;
        --review)
            REVIEW_MODE=1
            MODE="apply"
            shift
            ;;
        --help)
            grep "^#" "$0" | grep -v "^#!/bin/bash" | sed 's/^# *//'
            exit 0
            ;;
        *)
            echo "Unknown option: $1"
            echo "Use --help for usage information"
            exit 1
            ;;
    esac
done

echo -e "${BLUE}=== Clang-Tidy Auto-Fix Tool ===${NC}"
echo ""

# Check dependencies
check_tool() {
    if ! command -v "$1" &> /dev/null; then
        echo -e "${RED}❌ $1 not found${NC}"
        echo "   Install: sudo apt-get install $2"
        return 1
    fi
    return 0
}

echo "[1/5] Checking dependencies..."
tools_ok=true
check_tool clang-tidy clang-tools || tools_ok=false
check_tool cmake cmake || tools_ok=false
check_tool clang++ clang || tools_ok=false

if [ "$tools_ok" = false ]; then
    echo ""
    echo -e "${RED}Install missing tools and try again.${NC}"
    exit 1
fi
echo -e "${GREEN}✓${NC} All dependencies found"
echo ""

# Create backup
echo "[2/5] Creating backup..."
BACKUP_DIR=".clang_tidy_backup_$(date +%s)"
mkdir -p "$BACKUP_DIR"

# Only backup files that will be modified
for file in $(find include src -name "*.cpp" -o -name "*.hpp" -o -name "*.h"); do
    cp "$file" "$BACKUP_DIR/" 2>/dev/null || true
done

echo -e "${GREEN}✓${NC} Backup created: $BACKUP_DIR"
echo ""

# Configure and build with clang-tidy fixes
echo "[3/5] Analyzing code with Clang-Tidy..."
cmake -B build_fix \
    -DCMAKE_CXX_COMPILER=clang++ \
    -DCMAKE_CXX_CLANG_TIDY="clang-tidy;-checks=$CHECKS;--fix;--fix-errors" \
    -DLIBEMB_BUILD_TESTS=ON \
    -DLIBEMB_BUILD_BENCHMARKS=OFF \
    -DCMAKE_BUILD_TYPE=Release \
    2>&1 | tail -20

# Build to trigger fixes
cmake --build build_fix 2>&1 | grep -E "(warning|error|fixed)" || true

echo -e "${GREEN}✓${NC} Analysis complete"
echo ""

# Generate diff
echo "[4/5] Generating fix summary..."
mkdir -p .clang_tidy_report

if git diff --quiet; then
    echo -e "${GREEN}✓${NC} No changes detected"
    echo ""
    echo "Your code is already compliant with Clang-Tidy checks!"
    echo "Use --help for available options."

    # Cleanup
    rm -rf "$BACKUP_DIR" build_fix 2>/dev/null || true
    exit 0
fi

# Generate comprehensive report
git diff > .clang_tidy_report/all_fixes.patch
git diff --stat > .clang_tidy_report/summary.txt

# Count fixes by category
cat > analyze_fixes.py << 'PYTHON'
import re
import subprocess

# Get list of changed files
result = subprocess.run(['git', 'diff', '--name-only'], capture_output=True, text=True)
changed_files = result.stdout.strip().split('\n')

# Analyze each file
fixes_by_category = {}
total_changes = 0

for file in changed_files:
    if not file:
        continue

    # Get diff for file
    result = subprocess.run(['git', 'diff', file], capture_output=True, text=True)
    diff_lines = result.stdout.split('\n')

    for line in diff_lines:
        if line.startswith('@@'):
            total_changes += 1

# Parse brief summary
result = subprocess.run(['git', 'diff', '--stat'], capture_output=True, text=True)
summary = result.stdout

with open('.clang_tidy_report/fixes_analysis.txt', 'w') as f:
    f.write("=== Clang-Tidy Fixes Analysis ===\n\n")
    f.write(f"Total hunks changed: {total_changes}\n\n")
    f.write("File changes:\n")
    f.write(summary)

PYTHON

python3 analyze_fixes.py
rm analyze_fixes.py

echo -e "${GREEN}✓${NC} Report generated: .clang_tidy_report/"
echo ""

# Display summary
echo "=== Fix Summary ==="
echo ""
cat .clang_tidy_report/summary.txt
echo ""

# Show preview of changes
echo "=== Preview of Changes ==="
echo ""
git diff --unified=1 | head -100
echo ""

if [ $(git diff | wc -l) -gt 100 ]; then
    echo -e "${YELLOW}... (truncated, full patch in .clang_tidy_report/all_fixes.patch)${NC}"
    echo ""
fi

# Handle different modes
if [ "$REVIEW_MODE" = 1 ]; then
    echo -e "${BLUE}=== Interactive Review Mode ===${NC}"
    echo ""
    echo "Review the changes above. Apply them?"
    echo ""
    read -p "Apply fixes? [y/n] " -n 1 -r
    echo
    if [[ $REPLY =~ ^[Yy]$ ]]; then
        echo -e "${GREEN}Applying fixes...${NC}"
        # Fixes are already applied by clang-tidy, just need to clean up
        rm -rf "$BACKUP_DIR"
        echo -e "${GREEN}✓ Fixes applied!${NC}"
    else
        echo -e "${YELLOW}Restoring backup...${NC}"
        # Restore from backup
        for file in $(find "$BACKUP_DIR" -type f); do
            original="${file#$BACKUP_DIR/}"
            cp "$file" "$original"
        done
        rm -rf "$BACKUP_DIR"
        echo -e "${GREEN}✓ Original files restored${NC}"
    fi

elif [ "$MODE" = "apply" ]; then
    echo -e "${GREEN}✓ Fixes applied automatically${NC}"
    rm -rf "$BACKUP_DIR"

else
    # Dry-run mode
    echo -e "${YELLOW}[DRY-RUN MODE]${NC}"
    echo "No files were modified."
    echo ""
    echo "To apply fixes, run:"
    echo "  bash scripts/apply_clang_tidy_fixes.sh --apply"
    echo ""
    echo "To review interactively:"
    echo "  bash scripts/apply_clang_tidy_fixes.sh --review"
    echo ""

    # Restore original files in dry-run
    for file in $(find "$BACKUP_DIR" -type f); do
        original="${file#$BACKUP_DIR/}"
        cp "$file" "$original"
    done
    rm -rf "$BACKUP_DIR"
fi

# Final message
echo ""
echo "[5/5] Cleanup complete"
echo ""

if [ "$MODE" = "apply" ] || [ "$REVIEW_MODE" = 1 ]; then
    echo -e "${GREEN}=== Summary ===${NC}"
    echo ""
    echo "Fixed files:"
    git diff --name-only | sed 's/^/  /'
    echo ""
    echo "Next steps:"
    echo "  1. Review the changes (git diff)"
    echo "  2. Run tests to verify (cmake --build build && ctest --test-dir build)"
    echo "  3. Commit the changes"
    echo ""
else
    echo -e "${BLUE}=== Summary ===${NC}"
    echo ""
    echo "Potential fixes found in:"
    git diff --name-only | sed 's/^/  /'
    echo ""
    echo "Options:"
    echo "  bash scripts/apply_clang_tidy_fixes.sh --apply   # Apply all fixes"
    echo "  bash scripts/apply_clang_tidy_fixes.sh --review  # Interactive review"
    echo "  bash scripts/apply_clang_tidy_fixes.sh --safe-only  # Only safe fixes"
    echo ""
fi

# Cleanup build directory
rm -rf build_fix 2>/dev/null || true

echo -e "${GREEN}Done! ✓${NC}"
