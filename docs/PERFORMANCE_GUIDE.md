# Performance Optimization Guide

Performance tuning and benchmarking strategies for libemb.

## Performance Characteristics

### RingBuffer Performance

| Operation | Complexity | Time |
|-----------|-----------|------|
| push() | O(1) | <1 μs |
| pop() | O(1) | <1 μs |
| peek() | O(1) | <1 μs |
| size() | O(1) | <1 μs |

**Why O(1)?** Fixed-size circular array with index arithmetic.

### StaticVector Performance

| Operation | Complexity | Time |
|-----------|-----------|------|
| push_back() | O(1) | <1 μs |
| pop_back() | O(1) | <1 μs |
| operator[] | O(1) | <1 μs |
| size() | O(1) | <1 μs |

**Why O(1)?** Stack-allocated array with size tracking.

### Driver Performance (I2C)

| Operation | Typical Time | Notes |
|-----------|-------------|-------|
| I2C Write | 100-500 μs | Depends on data length |
| I2C Read | 100-500 μs | Depends on data length |
| Ssd1306 Init | ~5 ms | Multiple I2C commands |
| Ssd1306 Display | ~10 ms | Full frame update |

---

## Optimization Levels

### Profile 1: Development (Default)

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Debug
```

**Compiler Flags**: `-g -O0`
- Full debugging information
- Slowest execution
- Easiest debugging
- Largest binary

**Use When**: Developing and debugging

### Profile 2: Optimized Host

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
```

**Compiler Flags**: `-O3 -NDEBUG`
- Maximum optimization
- Full inlining
- Fastest execution
- Larger binary

**Use When**: Running tests, benchmarks, demos

### Profile 3: Embedded (Balanced)

```bash
cmake -B build \
  -DCMAKE_TOOLCHAIN_FILE=cmake/toolchains/arm-none-eabi.cmake \
  -DCMAKE_BUILD_TYPE=Release
```

**Additional Flags**:
- `-flto` (Link-Time Optimization)
- `-ffunction-sections -fdata-sections`
- `-Wl,--gc-sections` (Remove unused sections)

**Result**: ~40% size reduction vs. -O3 alone

### Profile 4: Size Optimized

```bash
cmake -B build \
  -DCMAKE_BUILD_TYPE=MinSizeRel \
  -DCMAKE_TOOLCHAIN_FILE=cmake/toolchains/arm-none-eabi.cmake
```

**Compiler Flags**: `-Os`
- Optimizes for size, not speed
- Smallest binary
- Reasonable performance
- LTO still enabled

---

## Benchmarking

### Running Benchmarks

```bash
# Build with benchmark support
cmake -B build \
  -DLIBEMB_BUILD_BENCHMARKS=ON \
  -DCMAKE_BUILD_TYPE=Release

# Run benchmarks
./build/benchmark/benchmark_ringbuffer
./build/benchmark/benchmark_vector
```

### Example Benchmark Output

```
2026-03-16T10:30:45+00:00
Running ./build/benchmark/benchmark_ringbuffer
Run on (8 X 4400 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x4)
  L1 Instruction 32 KiB (x4)
  L2 Unified 256 KiB (x4)
  L3 Unified 8192 KiB (x1)

Benchmark                    Time           CPU    Iterations
─────────────────────────────────────────────────────────────
RingBuffer_Push            10.5 ns       10.5 ns     66.6M
RingBuffer_Pop             10.3 ns       10.3 ns     68.1M
RingBuffer_PushPop         21.1 ns       21.1 ns     33.2M
RingBuffer_PeekEmpty       2.45 ns      2.45 ns    285.6M
```

### Interpreting Results

- **Time**: Wall clock time per iteration
- **CPU**: CPU time per iteration
- **Iterations**: How many times the loop ran

**Analysis**:
- push/pop: <1 μs ✅ Excellent
- No allocation: ✅ Predictable
- Cache-friendly: ✅ Fast access

---

## Memory Profiling

### AddressSanitizer (ASAN)

Detects memory errors at runtime:

```bash
cmake -B build \
  -DLIBEMB_ENABLE_ASAN=ON \
  -DLIBEMB_BUILD_TESTS=ON

cmake --build build
ctest --test-dir build
```

**Detects**:
- Buffer overflows
- Use-after-free
- Memory leaks
- Heap corruption

### Valgrind (Linux Alternative)

```bash
valgrind --leak-check=full ./build/examples/host_demo
```

**Output**:
```
==12345== HEAP SUMMARY:
==12345==     in use at exit: 0 bytes in 0 blocks
==12345==   total heap alloc/dealloc: X blocks / Y blocks
==12345==     in use at exit: 0 bytes in 0 blocks
==12345== All heap blocks were freed
```

### Memory Usage Analysis

Static allocation (no dynamic allocation):

```cpp
// libemb::util::RingBuffer<uint8_t, 256>
// Stack memory: 256 bytes (fixed)
// Heap memory: 0 bytes (no allocation)
// Total: 256 bytes
```

**Benefit**: Predictable memory footprint

---

## Compiler Optimization Techniques

### Technique 1: Link-Time Optimization (LTO)

**What**: Compiler optimizes across translation units

```bash
# Enabled in arm-none-eabi.cmake
-flto
```

**Result**:
- More aggressive inlining
- Dead code elimination
- ~20-30% size reduction
- Build time increase

### Technique 2: Function Sections

**What**: Each function in separate linker section

```bash
# Enabled in arm-none-eabi.cmake
-ffunction-sections -fdata-sections
-Wl,--gc-sections  # Garbage collect unused sections
```

**Result**:
- Only used functions linked
- ~40% size reduction (embedded)

### Technique 3: Inline Optimization

**What**: Compiler inlines small functions

```cpp
// Template functions are automatically inline
template<typename T, size_t N>
class StaticVector {
    size_t size() const {  // Implicitly inline
        return size_;
    }
};

// Force inline for performance-critical code
inline void setPixel(uint8_t x, uint8_t y) {
    // Fast path: direct frame buffer manipulation
}
```

### Technique 4: constexpr Optimization

**What**: Evaluate at compile time

```cpp
// Constant expression
template<typename T, size_t N>
class RingBuffer {
    static constexpr size_t CAPACITY = N;  // Compile-time constant
};

// Compile-time calculation
constexpr uint16_t calculate_checksum(const uint8_t* data, size_t len) {
    // Evaluated at compile time if inputs are constexpr
}
```

---

## Real-Time Considerations

### Predictability is Critical

Embedded systems need predictable behavior:

**What to Avoid**:
- ❌ Dynamic allocation (unpredictable time)
- ❌ Complex algorithms (variable timing)
- ❌ Blocking I/O (variable latency)
- ❌ Interrupts without careful design

**libemb Approach**:
- ✅ Fixed-size buffers (constant time)
- ✅ Simple algorithms (predictable)
- ✅ Non-blocking mock I2C
- ✅ Designed for interrupt-safe usage

### Latency Analysis

RingBuffer push in interrupt context:

```cpp
// In ISR (Interrupt Service Routine)
void UART_RX_ISR() {
    uint8_t byte = get_uart_byte();
    rx_buffer.push(byte);  // < 1 μs - deterministic!
}
```

**Overhead**: < 1 μs per byte
**Impact**: Minimal jitter in timing-critical systems

---

## Optimization Checklist

### Before Optimizing

- [ ] Profile to identify bottlenecks
- [ ] Measure current performance
- [ ] Define performance goals
- [ ] Document baseline metrics

### Code Optimization

- [ ] Use O3/-Os appropriately
- [ ] Enable LTO for embedded
- [ ] Use function sections + GC
- [ ] Inline performance-critical code
- [ ] Use constexpr for constants

### Memory Optimization

- [ ] Audit for dynamic allocation
- [ ] Use stack-based buffers
- [ ] Verify no memory leaks (ASAN)
- [ ] Profile memory usage
- [ ] Reduce unnecessary copies

### Benchmark Before/After

```bash
# Baseline
./build/benchmark/benchmark_ringbuffer > baseline.txt

# After optimization
./build/benchmark/benchmark_ringbuffer > optimized.txt

# Compare
diff baseline.txt optimized.txt
```

---

## Performance Tips

### Tip 1: Leverage Inlining

Small functions are inlined by compiler:

```cpp
// Good: Will be inlined
bool empty() const {
    return size_ == 0;
}

// Better: Explicitly inline
inline bool empty() const {
    return size_ == 0;
}

// Best: Template (automatically inline)
template<typename T, size_t N>
class RingBuffer {
    bool empty() const {
        return size_ == 0;  // Inlined in every instantiation
    }
};
```

### Tip 2: Use References, Not Copies

```cpp
// ❌ Slow: Copies data
void process(libemb::util::StaticVector<uint8_t, 10> vec) {
    // ...
}

// ✅ Fast: References data
void process(const libemb::util::StaticVector<uint8_t, 10>& vec) {
    // ...
}
```

### Tip 3: Batch Operations

```cpp
// ❌ Slow: N I2C transactions
for (int i = 0; i < 10; i++) {
    i2c->write(addr, &data[i], 1);  // 10 transactions
}

// ✅ Fast: 1 I2C transaction
i2c->write(addr, data, 10);  // 1 transaction
```

### Tip 4: Cache-Friendly Data Layout

```cpp
// ❌ Scattered memory access
struct Point {
    uint8_t x;    // Byte 0
    uint32_t y;   // Byte 4 (padding)
    uint8_t z;    // Byte 8
};

// ✅ Compact memory layout
struct Point {
    uint8_t x;
    uint8_t y;
    uint8_t z;
};
```

---

## Common Optimization Myths

### Myth 1: "All Optimizations Reduce Readability"

**Reality**: -O3 compiler optimization doesn't reduce readability. Write clear code, let compiler optimize.

### Myth 2: "Inline Everything"

**Reality**: Compiler inlines appropriately. Excessive inline creates code bloat. Use sparingly.

### Myth 3: "Always Use O2 Instead of O3"

**Reality**: -O3 is appropriate for embedded code. It doesn't create unpredictability with proper design.

### Myth 4: "Templates are Always Slower"

**Reality**: Templates enable better inlining and compile-time evaluation. Often faster than runtime equivalents.

---

## Profiling Tools

### GCC Profiler (gprof)

```bash
# Compile with profiling
gcc -pg -O3 program.cpp -o program

# Run program
./program

# Analyze
gprof program gmon.out
```

### Linux perf

```bash
# Profile with performance counters
perf record ./program
perf report

# Shows:
# - Hotspots (most called functions)
# - Cache misses
# - Branch mispredictions
```

### Clang LLVM Profile

```bash
# Compile with instrumentation
clang++ -fprofile-instr-generate program.cpp -o program

# Run
./program

# Convert profile
llvm-profdata merge -o default.profdata default.profraw

# View results
llvm-cov show program -instr-profile=default.profdata
```

---

## Building Optimized for Target Hardware

### STM32F4 Example

```bash
cmake -B build/stm32 \
  -DCMAKE_TOOLCHAIN_FILE=cmake/toolchains/arm-none-eabi.cmake \
  -DCMAKE_BUILD_TYPE=Release \
  -DARM_CORTEX_M4=ON

cmake --build build/stm32

# Analyze resulting binary
arm-none-eabi-size build/stm32/firmware.elf
arm-none-eabi-nm -S build/stm32/firmware.elf | sort -k 2 -n
```

---

## Performance Regression Prevention

### Benchmark in CI/CD

```yaml
# .github/workflows/performance.yml
jobs:
  benchmarks:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v4
      - name: Build benchmarks
        run: |
          cmake -B build -DLIBEMB_BUILD_BENCHMARKS=ON -DCMAKE_BUILD_TYPE=Release
          cmake --build build
      - name: Run benchmarks
        run: ./build/benchmark/benchmark_all
      - name: Compare with baseline
        run: bash scripts/compare_benchmarks.sh
```

---

## Summary

libemb is optimized for embedded performance:

1. **O(1) Operations**: RingBuffer, StaticVector
2. **No Dynamic Allocation**: Predictable memory
3. **Compile-Time Optimization**: Templates, constexpr
4. **Hardware-Friendly**: LTO, section GC, inline
5. **Benchmarkable**: Measurable performance

Use the optimization profiles and tools to achieve your performance targets while maintaining code quality.

---

## See Also

- TESTING_STRATEGY.md - Performance testing
- ADDING_MOCKS.md - Mock setup for benchmarks
- cmake/toolchains/arm-none-eabi.cmake - Embedded optimizations
- scripts/run_benchmarks.sh - Benchmark runner
