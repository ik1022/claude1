# libemb Build Validation Guide

Complete guide to libemb's multi-compiler validation framework (Phase 3).

## Overview

Phase 3 introduces comprehensive build validation across multiple compilers, static analysis enforcement, and code coverage tracking to ensure code quality and portability.

### What is Being Validated?

| Component | Tool | Status |
|-----------|------|--------|
| **Compiler Compatibility** | GCC, Clang, MSVC | ✅ Enforced in CI |
| **Strict Compilation** | -Werror, strict flags | ✅ Enforced in CI |
| **Static Analysis** | Clang-Tidy, CPPCheck | ✅ Enforced in CI |
| **Memory Safety** | AddressSanitizer | ✅ GCC builds in CI |
| **Code Coverage** | lcov/gcov | ✅ Enforced >80% |
| **Cross-Compilation** | ARM (STM32F4) | ✅ Build only, no tests |

---

## Compiler Support

### GCC (Linux) - Primary
- **Minimum Version**: 9.0
- **Flags**: `-Wall -Wextra -Wpedantic -Werror -Wconversion -Wshadow ...`
- **Special Features**: AddressSanitizer (-fsanitize=address)
- **Status**: ✅ Required to pass
- **CI Environment**: ubuntu-latest

### Clang (Linux) - Static Analysis
- **Minimum Version**: 12.0
- **Flags**: `-Weverything` (with reasonable exclusions)
- **Special Features**: Clang-Tidy integration
- **Status**: ✅ Required to pass
- **CI Environment**: ubuntu-latest
- **Additional Checks**: Clang-Tidy with strict enforcement

### MSVC (Windows) - Compatibility
- **Minimum Version**: Visual Studio 2019 (v142)
- **Flags**: `/W4 /WX /permissive-`
- **Special Features**: Standard conformance checking
- **Status**: ✅ Required to pass
- **CI Environment**: windows-latest

### ARM Cross-Compile (Embedded)
- **Toolchain**: arm-none-eabi-gcc
- **Target**: STM32F4 (Cortex-M4)
- **Flags**: Strict + LTO + Section GC
- **Status**: Build validation only (no execution)
- **CI Environment**: ubuntu-latest

---

## Strict Compilation

### What Does "Strict" Mean?

Strict compilation treats **all warnings as errors** and enables additional checks:

```bash
# GCC/Clang strict flags
-Werror                 # Treat warnings as errors
-Wall -Wextra -Wpedantic # Standard warnings
-Wconversion            # Implicit conversion checks
-Wshadow                # Variable shadowing
-Wformat=2              # Format string safety
-Wduplicated-cond       # Duplicated conditions
-Wnull-dereference      # Null pointer checks
```

### Local Development

To match CI validation locally:

```bash
# Quick validation
cmake -B build \
  -DLIBEMB_BUILD_TESTS=ON \
  -DLIBEMB_STRICT_COMPILER_FLAGS=ON

cmake --build build --parallel $(nproc)
ctest --test-dir build --output-on-failure
```

### CMake Options

```cmake
LIBEMB_STRICT_COMPILER_FLAGS    # Treat warnings as errors
LIBEMB_ENABLE_EXTRA_WARNINGS    # Enable extended checks
LIBEMB_ENABLE_CLANG_TIDY        # Clang-Tidy integration
LIBEMB_ENABLE_ASAN              # AddressSanitizer
LIBEMB_ENABLE_UBSAN             # UndefinedBehaviorSanitizer
LIBEMB_ENABLE_COVERAGE          # Code coverage
```

---

## Static Analysis

### Clang-Tidy

Automated static analysis with automatic fix suggestions.

**Configuration**: `.clang-tidy`

**Run locally**:
```bash
cmake -B build \
  -DLIBEMB_ENABLE_CLANG_TIDY=ON \
  -DCMAKE_CXX_COMPILER=clang++

cmake --build build
```

**What is checked?**
- Naming conventions (Classes: CamelCase, private members: snake_case_)
- Performance issues (const correctness, unnecessary copies)
- Modernization (C++17 idioms)
- Readability (function complexity, braces)
- Security (buffer overflows, format strings)

**Important**: Clang-Tidy warnings are treated as errors in CI.

### CPPCheck

Supplementary static analysis for additional checks.

**Run locally**:
```bash
cppcheck --enable=all \
  -i build -i test \
  include/ src/ port/ examples/
```

**What is checked?**
- Memory leaks
- Out-of-bounds access
- Uninitialized variables
- Logic errors
- Style issues

---

## Code Coverage

### Coverage Threshold

- **Overall**: ≥ 80% (mandatory)
- **Critical modules**: ≥ 90%
- **New code**: ≥ 85%

### Local Coverage Check

```bash
cmake -B build \
  -DLIBEMB_BUILD_TESTS=ON \
  -DLIBEMB_ENABLE_COVERAGE=ON \
  -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_CXX_FLAGS="-O0 -fprofile-arcs -ftest-coverage"

cmake --build build
ctest --test-dir build

# Generate HTML report
cd build
lcov --directory . --capture --output-file coverage.info
genhtml coverage.info --output-directory coverage-html
# Open coverage-html/index.html
```

### Coverage by Module

Coverage is tracked for:
- `include/libemb/hal/` - HAL interfaces
- `include/libemb/util/` - Utilities
- `src/driver/` - Drivers
- `port/mock/` - Mock implementations

**Note**: Tests and examples are excluded from coverage metrics.

---

## CI/CD Workflow

### Triggered By

- Push to `main`, `develop`, or `feature/*` branches
- Pull requests to `main` or `develop`
- Manual workflow dispatch

### Workflow Stages

1. **Multi-Compiler Validation** (parallel)
   - GCC validation (strict + ASAN)
   - Clang validation (strict + Clang-Tidy)
   - MSVC validation (strict + conformance)
   - ARM cross-compilation

2. **Static Analysis** (parallel)
   - Clang-Tidy enforcement
   - CPPCheck enforcement

3. **Coverage Analysis**
   - Generate coverage report
   - Enforce threshold (>80%)
   - Compare with main branch
   - Upload to Codecov

### Status Checks

All of the following must pass:
- ✅ `multi-compiler-validation`
- ✅ `static-analysis-enforcement`
- ✅ `coverage-enforcement`

### Failure Handling

If a workflow fails:

1. **Compilation Error**
   - Check compiler output in workflow logs
   - Fix the compilation error locally
   - Push corrected code

2. **Strict Warning** (-Werror)
   - Address the warning locally
   - Most warnings have fixes in CI logs
   - Use `libemb_apply_strict_validation()` to test

3. **Clang-Tidy Warning**
   - Review `.clang-tidy` config
   - Run clang-tidy locally
   - Apply suggested fixes

4. **Coverage Below Threshold**
   - Add tests for uncovered code
   - Check coverage report in artifacts
   - View coverage HTML in artifact

---

## Validation Profiles

### Profile 1: Development (Default)

```bash
cmake -B build \
  -DLIBEMB_BUILD_TESTS=ON \
  -DCMAKE_BUILD_TYPE=Debug
```

**Features**: Basic warnings, no strict flags
**Time**: ~1 minute
**Recommended For**: Local development

### Profile 2: Pre-Commit

```bash
cmake -B build \
  -DLIBEMB_BUILD_TESTS=ON \
  -DLIBEMB_BUILD_EXAMPLES=ON \
  -DLIBEMB_STRICT_COMPILER_FLAGS=ON \
  -DCMAKE_BUILD_TYPE=Release
```

**Features**: Strict flags, all warnings as errors
**Time**: ~2 minutes
**Recommended For**: Before pushing to PR

### Profile 3: Full CI Validation

```bash
cmake -B build \
  -DLIBEMB_BUILD_TESTS=ON \
  -DLIBEMB_BUILD_EXAMPLES=ON \
  -DLIBEMB_STRICT_COMPILER_FLAGS=ON \
  -DLIBEMB_ENABLE_EXTRA_WARNINGS=ON \
  -DLIBEMB_ENABLE_COVERAGE=ON \
  -DLIBEMB_ENABLE_CLANG_TIDY=ON \
  -DLIBEMB_ENABLE_ASAN=ON \
  -DCMAKE_BUILD_TYPE=Debug
```

**Features**: All checks enabled, complete validation
**Time**: ~5 minutes
**Recommended For**: CI/CD pipeline, final validation

### Profile 4: ARM Embedded

```bash
cmake -B build \
  -DCMAKE_TOOLCHAIN_FILE=cmake/toolchains/arm-none-eabi.cmake \
  -DLIBEMB_STRICT_COMPILER_FLAGS=ON \
  -DCMAKE_BUILD_TYPE=Release
```

**Features**: Cross-compilation, LTO, embedded optimizations
**Time**: ~2 minutes
**Recommended For**: Embedded builds only

### Profile 5: Memory Safety (ASAN+UBSAN)

```bash
cmake -B build \
  -DLIBEMB_BUILD_TESTS=ON \
  -DLIBEMB_ENABLE_ASAN=ON \
  -DLIBEMB_ENABLE_UBSAN=ON \
  -DCMAKE_BUILD_TYPE=Debug
```

**Features**: AddressSanitizer + UndefinedBehaviorSanitizer
**Time**: ~3 minutes (slower due to instrumentation)
**Recommended For**: Memory safety verification

---

## Helper Scripts

### Local Strict Validation

```bash
bash scripts/local_strict_validation.sh
```

Builds with:
- GCC compiler
- Strict flags enabled
- Extra warnings enabled
- Coverage enabled
- ASAN enabled
- All tests

### Quick Validation

```bash
bash scripts/quick_validation.sh
```

Fast validation (~1 minute):
- Strict flags only
- Tests only
- No coverage/ASAN overhead

---

## Common Issues

### Issue: Compilation Error with Strict Flags

**Error**: `error: unused variable 'x' [-Werror=unused-variable]`

**Solution**: Remove unused variable or use it.

```cpp
// Before
int unused_var = 42;

// After
[[maybe_unused]] int debug_var = 42;  // If intentionally unused
// OR
int used_var = 42;
printf("%d\n", used_var);  // Actually use it
```

### Issue: Clang-Tidy Warning on Naming

**Error**: `warning: invalid case style for private member 'myVar_'`

**Solution**: Follow naming convention in `.clang-tidy`

```cpp
// Before
int myVar_;  // Should be lower_case

// After
int my_var_;  // Correct for private member
```

### Issue: Coverage Below Threshold

**Error**: `Coverage 75% is below threshold 80%`

**Solution**: Add tests for uncovered code.

1. Download coverage HTML from CI artifacts
2. Identify uncovered lines
3. Write tests to cover those lines
4. Re-run coverage

### Issue: AddressSanitizer Error

**Error**: `==12345==ERROR: AddressSanitizer: heap-buffer-overflow`

**Solution**: Fix the memory safety issue.

```cpp
// Common issue: buffer overflow
char buffer[10];
strcpy(buffer, "very long string");  // ❌ Overflow!

// Fix: Use safe function
strncpy(buffer, "safe string", sizeof(buffer) - 1);  // ✅
buffer[sizeof(buffer) - 1] = '\0';
```

---

## Performance Considerations

### Build Time Impact

| Feature | Time Overhead |
|---------|---------|
| Strict flags | +5-10% |
| Clang-Tidy | +20-30% |
| Coverage | +50-100% |
| ASAN | +30-50% |
| UBSAN | +20-30% |

### Optimization for CI

- Use parallel builds: `cmake --build . --parallel $(nproc)`
- Enable caching in `.github/workflows/*.yml`
- Run different checks in parallel
- Skip non-essential checks on feature branches

---

## Related Documentation

- **CLAUDE.md**: Project overview and philosophy
- **CMakeLists.txt**: Build system configuration
- **cmake/Validation.cmake**: Validation framework implementation
- **.clang-tidy**: Static analysis rules
- **.github/workflows/**: CI/CD pipeline definitions

---

## Next Steps

1. **Before Opening PR**
   - Run Profile 2 (Pre-Commit) locally
   - Ensure all tests pass
   - Check coverage is above 80%

2. **After Opening PR**
   - Monitor CI checks
   - Review Clang-Tidy suggestions
   - Check coverage report

3. **Before Merging**
   - All CI checks must pass
   - Coverage must not decrease
   - Code review approval required

---

## Questions?

For issues with validation:
1. Check the relevant CI workflow logs
2. Run the same configuration locally
3. Refer to the "Common Issues" section
4. Open an issue with build logs attached
