# Testing Strategy for libemb

Comprehensive approach to testing embedded C++ components in libemb without requiring hardware.

## Overview

The libemb testing strategy uses a three-level pyramid:

```
         ┌─────────────────────┐
         │  System Integration │  (Actual Hardware)
         ├─────────────────────┤
         │  Integration Tests  │  (Mock Components)
         ├─────────────────────┤
         │    Unit Tests       │  (Isolated Components)
         └─────────────────────┘
```

### Testing Philosophy

- **Test-Driven Development (TDD)**: Write tests before implementation
- **Mock-Based Testing**: Use mocks to avoid hardware dependencies
- **Coverage Goals**: Maintain >80% code coverage
- **Fast Feedback**: Tests should complete in milliseconds
- **Deterministic**: Same test should always produce same result

---

## Unit Testing

Unit tests verify individual components in isolation using mocks.

### Framework: Google Test (v1.14.0)

Google Test provides:
- Test fixtures for setup/teardown
- Assertions with clear error messages
- Test discovery and execution
- Parameterized tests
- Matchers for complex assertions

### Unit Test Structure

```cpp
#include <gtest/gtest.h>
#include "libemb/util/RingBuffer.hpp"

// Test fixture for setup/cleanup
class RingBufferTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Called before each test
        buffer = std::make_unique<libemb::util::RingBuffer<uint8_t, 10>>();
    }

    void TearDown() override {
        // Called after each test
    }

    std::unique_ptr<libemb::util::RingBuffer<uint8_t, 10>> buffer;
};

// Basic test
TEST_F(RingBufferTest, PushAndPop) {
    // Arrange
    uint8_t value = 42;

    // Act
    bool pushed = buffer->push(value);
    uint8_t popped = 0;
    bool popped_success = buffer->pop(popped);

    // Assert
    ASSERT_TRUE(pushed);
    ASSERT_TRUE(popped_success);
    ASSERT_EQ(popped, value);
}

// Test expected behavior on error
TEST_F(RingBufferTest, FailsWhenFull) {
    // Fill buffer completely
    for (int i = 0; i < 10; i++) {
        buffer->push(i);
    }

    // Attempt to overfill
    ASSERT_FALSE(buffer->push(99));
}
```

### Unit Test Organization

```
test/unit/
├── test_RingBuffer.cpp         # RingBuffer tests
├── test_StaticVector.cpp       # StaticVector tests
├── test_Ssd1306.cpp            # OLED driver tests
├── test_Uart.cpp               # UART driver tests
├── test_I2c.cpp                # I2C driver tests
└── CMakeLists.txt              # Test configuration
```

### Running Unit Tests

```bash
# Build with tests
cmake -B build -DLIBEMB_BUILD_TESTS=ON
cmake --build build

# Run all tests
ctest --test-dir build --output-on-failure

# Run specific test
ctest --test-dir build -R test_RingBuffer --output-on-failure

# Run with verbose output
ctest --test-dir build -VV

# Run with coverage
cmake -B build -DLIBEMB_ENABLE_COVERAGE=ON
cmake --build build
ctest --test-dir build
lcov --directory build --capture --output-file build/coverage.info
```

---

## Integration Testing

Integration tests verify interactions between multiple components.

### What to Test

- Driver + HAL (via Mock)
- Utility + Driver
- Multiple HAL operations in sequence
- Error handling across components

### Example: Driver Integration Test

```cpp
TEST(Ssd1306Integration, InitAndDisplay) {
    // Setup: Create mock I2C and driver
    auto i2c = std::make_unique<MockI2c>();
    auto display = std::make_unique<Ssd1306>();

    // Initialize display
    ASSERT_TRUE(display->init(i2c.get(), 0x3C));

    // Draw some content
    display->clear();
    display->setPixel(10, 10, true);
    display->drawHLine(0, 0, 10, true);
    display->drawVLine(0, 0, 10, true);

    // Display (send to hardware mock)
    ASSERT_TRUE(display->display());

    // Verify I2C protocol was followed
    ASSERT_GT(i2c->transactions().size(), 0);

    // Verify init commands were sent
    bool found_init = false;
    for (const auto& tx : i2c->transactions()) {
        if (tx.type == MockI2c::Transaction::WRITE) {
            found_init = true;
            break;
        }
    }
    ASSERT_TRUE(found_init);
}
```

### Integration Test Organization

```
test/unit/
├── test_Ssd1306.cpp            # Ssd1306 + MockI2c
├── test_RingBuffer.cpp         # RingBuffer + Driver
└── fixtures/
    └── test_Fixtures.cpp       # Shared test helpers
```

---

## Mock-Based Testing

Mocks simulate hardware behavior, enabling testing without real hardware.

### When to Use Mocks

| Scenario | Solution |
|----------|----------|
| Test on PC | Use Mock |
| No hardware available | Use Mock |
| Need fast tests | Use Mock |
| Testing error handling | Use Mock (inject errors) |
| Testing normal flow | Use Mock |
| Validating on hardware | Real hardware |

### Mock Workflow

```
1. Create mock object
   ↓
2. Prepare test data (mocks can inject responses)
   ↓
3. Execute code under test
   ↓
4. Verify behavior (mocks record operations)
   ↓
5. Assert correct interactions
```

### Example: Error Injection

```cpp
TEST(Sensor, HandleI2cFailure) {
    auto i2c = std::make_unique<MockI2c>();
    auto sensor = std::make_unique<BMP280>();

    // Inject I2C error
    i2c->set_response_status(I2cStatus::TIMEOUT);

    // Code should handle gracefully
    bool result = sensor->read(i2c.get());

    // Should not crash and should report error
    ASSERT_FALSE(result);
}
```

---

## Coverage Metrics

### What is Code Coverage?

Code coverage measures what percentage of source code is executed by tests.

**Types:**
- **Line Coverage**: % of executable lines covered
- **Branch Coverage**: % of if/else paths covered
- **Function Coverage**: % of functions called

### Coverage Tools

**Linux (GCC/Clang)**:
- **gcov**: Compiler-built coverage tool
- **lcov**: Graphical coverage analysis
- **genhtml**: Generate HTML coverage reports

### Measuring Coverage

```bash
# Configure with coverage enabled
cmake -B build \
  -DLIBEMB_ENABLE_COVERAGE=ON \
  -DCMAKE_CXX_FLAGS="-fprofile-arcs -ftest-coverage"

# Build and run tests
cmake --build build
ctest --test-dir build

# Generate coverage report
cd build
lcov --directory . --capture --output-file coverage.info
lcov --remove coverage.info '/usr/*' '*/test/*' --output-file coverage.info
genhtml coverage.info --output-directory coverage-html

# View report
open coverage-html/index.html
```

### Coverage Goals

| Module | Target |
|--------|--------|
| `libemb::hal` | 90% |
| `libemb::util` | 95% |
| `libemb::driver` | 85% |
| Overall | 80% |

### Improving Coverage

1. **Identify uncovered code**: Check coverage HTML
2. **Write tests for gaps**: Test the missing scenarios
3. **Verify edge cases**: Test boundary conditions
4. **Test error paths**: Include error handling tests

**Do NOT:**
- ❌ Artificially inflate coverage with useless tests
- ❌ Test compiler-generated code
- ❌ Write tests just for coverage metrics
- ✅ Write meaningful tests for real code paths

---

## Test Patterns

### Pattern 1: Arrange-Act-Assert (AAA)

```cpp
TEST_F(MyTest, DoesWhatWeExpect) {
    // ARRANGE: Setup test data
    int input = 5;
    int expected = 10;

    // ACT: Execute code under test
    int result = function_under_test(input);

    // ASSERT: Verify behavior
    ASSERT_EQ(result, expected);
}
```

### Pattern 2: Parameterized Tests

```cpp
// Test same logic with different inputs
class RingBufferParamTest : public ::testing::TestWithParam<int> {};

TEST_P(RingBufferParamTest, PushPopSize) {
    auto buffer = std::make_unique<RingBuffer<int, 10>>();
    int size = GetParam();

    for (int i = 0; i < size; i++) {
        buffer->push(i);
    }

    ASSERT_EQ(buffer->size(), size);
}

INSTANTIATE_TEST_SUITE_P(
    SizesTest,
    RingBufferParamTest,
    ::testing::Values(1, 5, 10)  // Test with sizes 1, 5, 10
);
```

### Pattern 3: Fixture Reuse

```cpp
class DriverTest : public ::testing::Test {
protected:
    std::unique_ptr<MockI2c> i2c;
    std::unique_ptr<MyDriver> driver;

    void SetUp() override {
        i2c = std::make_unique<MockI2c>();
        driver = std::make_unique<MyDriver>();
    }
};

// All these tests reuse the same fixture
TEST_F(DriverTest, Initialize) { /* ... */ }
TEST_F(DriverTest, Read) { /* ... */ }
TEST_F(DriverTest, Write) { /* ... */ }
TEST_F(DriverTest, HandleError) { /* ... */ }
```

### Pattern 4: Death Tests

```cpp
// Test that code aborts/asserts on invalid input
TEST(MyTest, AssertsOnNullPointer) {
    ASSERT_DEATH(function_with_assert(nullptr), ".*");
}
```

---

## Real Hardware Testing

### When Hardware is Available

1. **Functional Validation**: Verify real hardware behavior
2. **Performance**: Measure actual timing
3. **Integration**: Test with actual sensors/devices
4. **System Validation**: End-to-end testing

### Hardware Test Strategy

```
PC (Unit Tests)
    ↓
PC (Integration Tests)
    ↓
Hardware (Functional Tests)
    ↓
Hardware (Integration Tests)
```

### Debugging on Hardware

**Setup**:
```bash
# STM32 debugging with GDB + STLink
arm-none-eabi-gdb build/firmware.elf

(gdb) target extended-remote :4242
(gdb) load
(gdb) break main
(gdb) continue
(gdb) step
(gdb) print variable_name
```

**Serial Monitor**:
```bash
# View UART output
minicom -D /dev/ttyUSB0 -b 115200

# Or using stm32cubeprogrammer (GUI)
```

---

## Continuous Integration Testing

### GitHub Actions CI

All tests automatically run on:
- Push to `main` or `develop`
- Pull requests
- Manual workflow dispatch

### CI Test Jobs

```yaml
jobs:
  unit-tests:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v4
      - name: Configure
        run: cmake -B build -DLIBEMB_BUILD_TESTS=ON
      - name: Build
        run: cmake --build build
      - name: Test
        run: ctest --test-dir build --output-on-failure
```

### Test Artifacts

- Test reports
- Coverage reports
- Build logs
- Benchmark results

---

## Best Practices

### ✅ DO:

- Write tests alongside code (TDD)
- Use descriptive test names
- Test happy path AND error cases
- Keep tests fast (mocks help)
- Isolate test dependencies
- Use fixtures for common setup
- Verify behavior, not implementation
- Write deterministic tests

### ❌ DON'T:

- Mock everything (use real implementations when simple)
- Write tests that depend on execution order
- Share state between tests
- Test compiler-generated code
- Write "tests" that always pass
- Use magic values in tests
- Test private implementation details

---

## Example: Complete Test Suite

```cpp
// test/unit/test_Ssd1306.cpp

#include <gtest/gtest.h>
#include "libemb/driver/Ssd1306.hpp"
#include "port/mock/MockI2c.hpp"

using libemb::driver::Ssd1306;
using libemb::port::mock::MockI2c;

class Ssd1306Test : public ::testing::Test {
protected:
    std::unique_ptr<MockI2c> i2c;
    std::unique_ptr<Ssd1306> display;

    void SetUp() override {
        i2c = std::make_unique<MockI2c>();
        display = std::make_unique<Ssd1306>();
    }
};

TEST_F(Ssd1306Test, InitializeSuccess) {
    ASSERT_TRUE(display->init(i2c.get(), 0x3C));
    ASSERT_GT(i2c->transactions().size(), 0);
}

TEST_F(Ssd1306Test, SetPixelUpdatesBuffer) {
    display->init(i2c.get(), 0x3C);
    i2c->clear_history();

    display->setPixel(10, 10, true);

    // Setting pixel doesn't transmit until display() is called
    ASSERT_EQ(i2c->transactions().size(), 0);
}

TEST_F(Ssd1306Test, DisplayTransmitsBuffer) {
    display->init(i2c.get(), 0x3C);
    display->setPixel(0, 0, true);
    i2c->clear_history();

    ASSERT_TRUE(display->display());

    // display() should transmit frame buffer
    ASSERT_GT(i2c->transactions().size(), 0);
}

TEST_F(Ssd1306Test, ClearRemovesAllPixels) {
    display->init(i2c.get(), 0x3C);
    display->setPixel(10, 10, true);
    display->clear();
    ASSERT_TRUE(display->display());

    // Verify buffer was cleared (implementation-specific)
    // This is a behavior test, not an implementation detail test
}
```

---

## Running the Full Test Suite

```bash
# Build with all features
cmake -B build \
  -DLIBEMB_BUILD_TESTS=ON \
  -DLIBEMB_BUILD_EXAMPLES=ON \
  -DLIBEMB_ENABLE_COVERAGE=ON \
  -DLIBEMB_ENABLE_CLANG_TIDY=ON \
  -DLIBEMB_STRICT_COMPILER_FLAGS=ON \
  -DCMAKE_BUILD_TYPE=Debug

# Compile
cmake --build build --parallel $(nproc)

# Run tests with verbose output
ctest --test-dir build --output-on-failure -VV

# Generate coverage report
lcov --directory build --capture --output-file build/coverage.info
genhtml build/coverage.info --output-directory build/coverage-html

# Run static analysis
bash scripts/run_static_analysis.sh
```

---

## Summary

The libemb testing strategy ensures code quality through:

1. **Unit Tests**: Fast, isolated component tests with mocks
2. **Integration Tests**: Multi-component interaction verification
3. **Coverage Metrics**: >80% code coverage requirement
4. **CI/CD**: Automated testing on every push/PR
5. **Real Hardware**: Final validation on actual hardware

This approach enables confident development without requiring hardware while preventing regressions.

---

## See Also

- ADDING_MOCKS.md - Mock implementation guide
- PERFORMANCE_GUIDE.md - Benchmarking strategy
- test/unit/ - Example tests
- .github/workflows/ - CI/CD configuration
