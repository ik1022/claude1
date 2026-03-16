# ABOV A31G123 Troubleshooting Guide

Complete troubleshooting and diagnostic guide for ABOV A31G123 firmware development.

## Table of Contents

1. [Build Issues](#build-issues)
2. [Flash & Debug Issues](#flash--debug-issues)
3. [GPIO Issues](#gpio-issues)
4. [UART Issues](#uart-issues)
5. [I2C Issues](#i2c-issues)
6. [SPI Issues](#spi-issues)
7. [Memory Issues](#memory-issues)
8. [Clock Issues](#clock-issues)
9. [Diagnostic Tools](#diagnostic-tools)
10. [Common Error Messages](#common-error-messages)

---

## Build Issues

### Issue 1: CMake Configuration Error

**Error**: `CMake Error: CMAKE_TOOLCHAIN_FILE does not exist`

**Cause**: Toolchain file path is incorrect or missing

**Solution**:
```bash
# Verify toolchain file exists
ls -la cmake/toolchains/abov-a31g123.cmake

# Use absolute path if needed
cmake -B build \
  -DCMAKE_TOOLCHAIN_FILE=$(pwd)/cmake/toolchains/abov-a31g123.cmake
```

### Issue 2: Compiler Not Found

**Error**: `arm-none-eabi-gcc: command not found`

**Cause**: ARM toolchain not installed or not in PATH

**Solution**:
```bash
# Install toolchain (Ubuntu/Debian)
sudo apt install arm-none-eabi-gcc arm-none-eabi-gdb

# Verify installation
arm-none-eabi-gcc --version

# Add to PATH if needed
export PATH=$PATH:/path/to/arm-toolchain/bin
```

### Issue 3: Undefined Reference to `_sbrk`

**Error**: `undefined reference to '_sbrk'`

**Cause**: Newlib syscalls not provided for bare-metal build

**Solution**:
```cmake
# Add in CMakeLists.txt for firmware targets
target_link_options(${TARGET} PRIVATE --specs=nosys.specs)
```

### Issue 4: Linker Script Not Found

**Error**: `linker command failed with exit code 1`

**Cause**: Linker script path is incorrect

**Solution**:
```bash
# Verify linker script
ls -la port/abov_a31g123/linker/A31G123.ld

# Ensure CMakeLists.txt has correct path
grep -n "A31G123.ld" port/abov_a31g123/CMakeLists.txt
```

---

## Flash & Debug Issues

### Issue 1: Flash Programming Fails

**Symptom**: `Error: Flash programming failed`

**Cause**:
- Debugger connection lost
- Flash memory write error
- File format incorrect

**Diagnosis**:
```bash
# Verify ELF file is valid
file build/firmware.elf
# Expected: ELF 32-bit LSB executable, ARM, EABI5...

# Check HEX file format
head -5 build/firmware.hex
# Expected: :020000040000FA
#          :10000000[hex data]
```

**Solution**:
```bash
# Re-flash with explicit protocol
arm-none-eabi-gdb build/firmware.elf
(gdb) target extended-remote :4242
(gdb) set remote hardware-breakpoint-packet off
(gdb) load
(gdb) monitor reset halt
(gdb) continue
```

### Issue 2: Debugger Not Detected

**Symptom**: `Error: libusb_open() failed with LIBUSB_ERROR_ACCESS`

**Cause**:
- USB permissions issue
- ST-Link not recognized
- Driver not installed

**Solution**:
```bash
# Check USB device is connected
lsusb | grep -i "st-link\|stm32"

# Add udev rules (Linux)
sudo usermod -a -G plugdev $USER
# Then logout and login

# Or run with sudo
sudo openocd -f interface/stlink-v2.cfg -f target/stm32f0x.cfg
```

### Issue 3: Breakpoints Don't Work

**Symptom**: Breakpoint set but never hit

**Cause**:
- Optimization level too high (code removed)
- Incorrect address
- Instruction caching issue

**Solution**:
```bash
# Build with debug info
cmake -B build -DCMAKE_BUILD_TYPE=Debug

# Verify debug symbols
arm-none-eabi-objdump -t build/firmware.elf | grep main

# Enable SWO tracing for detailed inspection
(gdb) set remote Z-packet off
(gdb) monitor tpiu config internal uart off 72000000 1500000
```

---

## GPIO Issues

### Issue 1: GPIO Pin Not Toggling

**Symptom**: LED doesn't turn on/off

**Diagnosis**:
```cpp
// Test hardware connection
A31G123Gpio led(GPIOA_BASE, 5);
led.setMode(GpioMode::OUTPUT);
led.write(GpioState::HIGH);

// Use GDB to verify register
(gdb) print gpio->DOUT
# Should show bit 5 set (0x20)
```

**Possible Causes**:
1. **Wrong pin number**: Verify pinout in datasheet
2. **Pin configured for alternate function**: Check AFSEL register
3. **Pin is INPUT instead of OUTPUT**: Verify setMode() called
4. **Current limiting resistor missing**: LED needs resistor
5. **Pin not powered**: Verify VCC on GPIO port

**Solution**:
```cpp
// Debug version with register inspection
A31G123Gpio led(GPIOA_BASE, 5);
led.setMode(GpioMode::OUTPUT);

// Read register to verify
GPIO_Type* gpio = (GPIO_Type*)GPIOA_BASE;
assert((gpio->CTRL & 0x30) == 0x10);  // PA5 should be OUTPUT (0x01 in bits 10-11)

led.write(GpioState::HIGH);
assert(gpio->DOUT & 0x20);  // Bit 5 should be set
```

### Issue 2: GPIO Pull-up/Pull-down Not Working

**Symptom**: Input pin floats instead of being pulled

**Cause**: PUPU/PDPU register not properly configured

**Solution**:
```cpp
A31G123Gpio button(GPIOA_BASE, 10);
button.setMode(GpioMode::INPUT_PULLUP);

// Verify in GDB
(gdb) print gpio->PUPU
# Should show bit 10 set (0x400)
```

---

## UART Issues

### Issue 1: No UART Output

**Symptom**: Nothing appears in terminal

**Diagnosis**:
```bash
# 1. Check if data is being sent
minicom -D /dev/ttyUSB0 -b 115200 &
strace -e write arm-none-eabi-gdb build/firmware.elf  # Watch for output

# 2. Check UART registers
(gdb) print uart->STAT
# Should show TXE bit set (0x01) if ready

# 3. Verify baud rate calculation
# APB_CLOCK = 35 MHz
# Divisor = (35MHz / 115200) / 2 = 151
(gdb) print uart->BAUD
# Should be 151 (0x97)
```

**Possible Causes**:
1. **Wrong baud rate**: Terminal set to different rate
2. **Wrong UART port**: Using UART0 but connected to UART1
3. **TX pin not mapped**: GPIO not configured for alternate function
4. **UART not enabled**: CTRL register missing TXEN bit
5. **No clock to UART**: RCC not configured

**Solution**:
```cpp
// Verify UART initialization
A31G123Uart uart(0, 115200);

// Manual verification
UART_Type* hw_uart = (UART_Type*)UART0_BASE;
assert(hw_uart->CTRL & UART_CTRL_TXEN);  // TX should be enabled
assert(hw_uart->BAUD == 151);  // Correct divisor

uart.putChar('A');
assert(hw_uart->STAT & UART_STAT_TC);  // TC bit should set after transmission
```

### Issue 2: Garbled UART Output

**Symptom**: Unreadable characters instead of expected text

**Cause**:
- Baud rate mismatch
- Parity bit wrong
- Stop bits wrong
- Data bits wrong

**Typical Symptoms**:
```
Expected: "Hello"
Actual:   "☺ℌℓℓö"  ← Baud rate doubled
Actual:   "Hellö"   ← Stop bit issue
```

**Solution**:
```cpp
// Calculate baud rate error percentage
float error = (actual_baud - target_baud) / target_baud * 100;

// For 115200 @ 35MHz: error should be < 2%
// Divisor = 151, Actual = 35000000 / (2 * 151) = 115763 Hz (0.49% error) ✓

// If error > 2%, use lower baud rate:
// 9600:   Divisor = 1823, Error = 0.00% ✓
// 19200:  Divisor = 911, Error = 0.00% ✓
// 115200: Divisor = 151, Error = 0.49% (marginal)

if (error > 2.0f) {
    // Switch to slower baud rate
    uart.configure_baudrate(9600);  // More stable
}
```

### Issue 3: UART Receives Garbage

**Symptom**: Received data is corrupted or missing characters

**Cause**:
- RX buffer overflow
- Interrupt not firing
- GPIO RX pin not configured

**Diagnosis**:
```cpp
// Check RX buffer status
if (uart.isDataAvailable()) {
    uint8_t byte = uart.getChar();
    // Byte should be valid ASCII or expected value
}

// Verify interrupt is firing
// In GDB: Set breakpoint in UART0_IRQHandler
(gdb) break UART0_IRQHandler
(gdb) continue
# Send character from terminal
# Breakpoint should trigger
```

**Solution**:
```cpp
// Check for RX buffer overflow
A31G123Uart uart(0, 115200);

// RX should be non-blocking
size_t received = uart.read(buffer, 64);
if (received == 0) {
    // No data available (normal)
}

// If you lose data, increase RingBuffer size
// (but be aware of 32KB RAM constraint on A31G123)
```

---

## I2C Issues

### Issue 1: I2C Bus Hangs

**Symptom**: I2C operations timeout, bus never completes

**Cause**:
- SDA/SCL stuck LOW
- Slave not responding
- Clock stretching issue
- Pull-up resistors missing

**Diagnosis**:
```bash
# Use oscilloscope to check bus
# SCL should be ~ 100 kHz (period = 10 µs)
# SDA should toggle during data transmission

# Measure pull-up voltage
# With no data: SDA/SCL should be HIGH (3.3V)
# During transmission: Should toggle between LOW (0V) and HIGH (3.3V)

# Check with logic analyzer
# Expected I2C START: SDA goes LOW while SCL is HIGH
# Expected I2C STOP: SDA goes HIGH while SCL is HIGH
```

**Solution**:
```cpp
// Add timeout protection
A31G123I2c i2c(0, 100000);

// writeRead includes timeout handling
I2cStatus status = i2c.writeRead(0x3C, cmd, 1, data, 8);

// Check for timeout
if (status == I2cStatus::TIMEOUT) {
    // Bus is hung, attempt recovery
    // Could:
    // 1. Issue STOP condition
    // 2. Reset I2C peripheral
    // 3. Perform I2C bus recovery (9 clock pulses)

    // For now: Restart I2C
    i2c = A31G123I2c(0, 100000);  // Re-initialize
}
```

### Issue 2: SSD1306 Display Not Detected

**Symptom**: I2C initialization fails, LED blinks error pattern

**Cause**:
- Display not at expected address (0x3C)
- Pull-up resistors not present
- Display not powered (VCC not connected)
- SDA/SCL pins wrong

**Diagnosis**:
```bash
# Use I2C scanner to find devices
# This scans addresses 0x00-0x7F and reports which respond

# Manual check with oscilloscope:
# 1. Verify VCC = 3.3V on display
# 2. Verify GND is connected
# 3. Verify SDA/SCL toggle (not stuck)
# 4. Verify pull-up resistors (should be 10kΩ)
```

**Solution**:
```cpp
// Add I2C scanning function
void i2c_scan(A31G123I2c& i2c) {
    for (uint8_t addr = 0x08; addr < 0x78; addr++) {
        I2cStatus status = i2c.write(addr, nullptr, 0);
        if (status == I2cStatus::OK) {
            // Device found at this address
            uart.putChar('.');
            uart.write((uint8_t*)&addr, 1);  // Print address
        }
    }
}

// Call to find display
i2c_scan(i2c);  // Should find device at 0x3C
```

---

## SPI Issues

### Issue 1: SPI Data Incorrect

**Symptom**: Wrong data transmitted/received

**Cause**:
- Clock polarity (CPOL) wrong
- Clock phase (CPHA) wrong
- Clock frequency too high (corruption)
- CS timing incorrect

**Diagnosis**:
```bash
# Measure SPI signals with logic analyzer:
# CPOL=0: SCK idles LOW
# CPOL=1: SCK idles HIGH
# CPHA=0: Data captured on first edge
# CPHA=1: Data captured on second edge

# Most common devices use Mode 0 (CPOL=0, CPHA=0)
```

**Solution**:
```cpp
// Try each mode until data is correct
A31G123Spi::Mode modes[] = {
    A31G123Spi::Mode::MODE_0,  // CPOL=0, CPHA=0 (most common)
    A31G123Spi::Mode::MODE_1,
    A31G123Spi::Mode::MODE_2,
    A31G123Spi::Mode::MODE_3
};

for (auto mode : modes) {
    A31G123Spi spi(0, 1000000, mode);
    uint8_t tx[] = {0x0F, 0xF0};
    uint8_t rx[2];
    if (spi.exchange(tx, rx, 2)) {
        // Check if data looks correct
        if (rx[0] == expected_value) {
            // Found correct mode
            break;
        }
    }
}
```

---

## Memory Issues

### Issue 1: Stack Overflow

**Symptom**: Random crashes, memory corruption

**Cause**:
- Local arrays too large
- Deep function call stack
- Recursive functions
- Global buffer overflow

**Diagnosis**:
```bash
# Check stack usage
arm-none-eabi-nm -S build/firmware.elf | grep -E "^[0-9a-f]+ [0-9a-f]+ b "

# Use AddressSanitizer to detect on host
cmake -B build -DLIBEMB_ENABLE_ASAN=ON
```

**Solution**:
```cpp
// Avoid large stack allocations
// ❌ Bad: Large local array
void bad_function() {
    uint8_t large_buffer[4096];  // 4KB on stack!
}

// ✅ Good: Use static or global allocation
static uint8_t large_buffer[4096];  // Fixed, doesn't grow stack

// ✅ Better: Allocate at compile time
template<typename T, size_t N>
class StaticBuffer {
    uint8_t data[N];
};
```

### Issue 2: Heap Exhaustion

**Symptom**: `malloc()` returns NULL, out of memory

**Cause**:
- Memory leak (not freeing)
- Too many allocations
- Heap too small for application

**Solution**:
```cpp
// Avoid dynamic allocation in embedded code
// ❌ Bad
char* message = (char*)malloc(100);

// ✅ Good: Use static allocation
static char message[100];

// For strings:
// ❌ Bad
std::vector<uint8_t> buffer;
buffer.resize(256);

// ✅ Good: Use libemb utilities
libemb::util::StaticVector<uint8_t, 256> buffer;
```

---

## Clock Issues

### Issue 1: Wrong System Clock Frequency

**Symptom**:
- UART baud rates off
- Timing loops too fast/slow
- I2C clock incorrect

**Diagnosis**:
```cpp
// Verify configured clock
uint32_t clock = libemb::port::abov_a31g123::get_system_clock_freq();
// Should be 70,000,000 Hz (70 MHz)

// Calculate UART baud from register
// Divisor = 151 @ 35 MHz APB
// Actual = 35,000,000 / (2 * 151) = 115,763 Hz (not exactly 115,200)

// Calculate error
float error = (115763 - 115200) / 115200.0f * 100;
// Error = +0.49% (acceptable, < 5%)
```

**Solution**:
```cpp
// Ensure clock is initialized before peripherals
void main() {
    initialize_system_clock();  // MUST be first

    // Now peripherals have correct clock
    A31G123Uart uart(0, 115200);
    A31G123I2c i2c(0, 100000);
}
```

---

## Diagnostic Tools

### GDB Commands for Debugging

```gdb
# Read GPIO register
(gdb) print *(GPIO_Type*)0x40040000

# Read specific DOUT register (GPIO A)
(gdb) x/w 0x40040004

# Watch for register changes
(gdb) watch *(uint32_t*)0x40040004

# Dump memory as hex
(gdb) x/64bx $sp

# Show assembly with current instruction
(gdb) disassemble /m main

# Print variables with type
(gdb) print -pretty on &gpio
(gdb) print sizeof(GPIO_Type)
```

### Serial Monitor Diagnostics

```bash
# Verbose output to terminal
minicom -D /dev/ttyUSB0 -b 115200 -C logfile.txt

# Monitor raw bytes (hexdump)
xxd < /dev/ttyUSB0

# Test UART with loopback
# (Short TX and RX together on board)
echo -n "test" > /dev/ttyUSB0 && cat /dev/ttyUSB0
```

### Binary Analysis

```bash
# Size breakdown
arm-none-eabi-size build/firmware.elf
# Shows .text, .data, .bss sizes

# Symbols by size
arm-none-eabi-nm -S build/firmware.elf | sort -rn | head -20

# Memory map
arm-none-eabi-objdump -h build/firmware.elf

# Check for undefined references
arm-none-eabi-nm -u build/firmware.elf
```

---

## Common Error Messages

### Error: "HardFault_Handler invoked"

**Meaning**: CPU exception occurred

**Common Causes**:
- Stack overflow
- Unaligned memory access
- Undefined instruction
- Divide by zero

**Fix**: Add breakpoint in HardFault_Handler to diagnose

### Error: "undefined reference to `_exit`"

**Meaning**: Program termination function missing

**Fix**:
```cmake
target_link_options(target PRIVATE --specs=nosys.specs)
```

### Error: "section `.text' will not fit in region `FLASH'"

**Meaning**: Program too large for Flash

**Fix**:
- Enable LTO: `-flto`
- Remove debug symbols: `-DCMAKE_BUILD_TYPE=Release`
- Reduce features / driver complexity

### Error: "conflicting variable type specification"

**Meaning**: Register type mismatch

**Fix**: Ensure volatile uint32_t* casting is correct

---

## Support Resources

- **ABOV A31G123 Datasheet**: Official documentation
- **ARM Cortex-M0+ Reference**: Core instruction set
- **GNU Arm Embedded Toolchain**: Compiler docs
- **libemb Documentation**: API references
- **Community Forums**: ARM and embedded development communities

---

For additional support, refer to:
- `docs/PORTING_ABOV_A31G123.md` - Hardware setup
- `docs/ARCHITECTURE.md` - System design
- `docs/ADDING_DRIVERS.md` - Driver development
