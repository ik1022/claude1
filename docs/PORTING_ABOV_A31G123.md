# ABOV A31G123 MCU Porting Guide

Complete guide to building libemb firmware for the ABOV A31G123 MCU.

## Quick Start

### Build for A31G123

```bash
cmake -B build/a31g123 \
  -DCMAKE_TOOLCHAIN_FILE=cmake/toolchains/abov-a31g123.cmake \
  -DLIBEMB_BUILD_TESTS=OFF

cmake --build build/a31g123
```

### Output Files

After building, you'll have:
- `firmware.elf` - ELF executable (debug symbols)
- `firmware.hex` - Hex file (for flash programming)
- `firmware.bin` - Binary file (for bootloader)

### Flash to Board

```bash
# Using ABOV LINK or ST-Link compatible debugger
arm-none-eabi-gdb firmware.elf
(gdb) target extended-remote :4242
(gdb) load
(gdb) run
```

---

## Hardware Specifications

### ABOV A31G123

| Feature | Specification |
|---------|---------------|
| **Core** | ARM Cortex-M0+ |
| **Clock** | 70 MHz max |
| **Flash** | 128 KB (typical) |
| **RAM** | 32 KB |
| **EEPROM** | 4 KB (typical) |
| **Peripherals** | GPIO, UART, SPI, I2C, Timer, ADC, RTC, WDT |
| **Packages** | LQFP100, LQFP64, BGA |
| **Vendor** | ABOV Semiconductor (Korea) |

### MCU Variants

Different A31G123 variants may have different memory:
- A31G123: 128 KB Flash, 32 KB RAM (standard)
- A31G121: 64 KB Flash, 16 KB RAM
- A31G124: 256 KB Flash, 64 KB RAM (if available)

Check your datasheet for exact configuration.

---

## Clock Configuration

### Default Configuration

libemb configures the clock as follows:

**Input**: 10 MHz external crystal oscillator
**PLL**: 10 MHz × 7 = 70 MHz
**Output**: 70 MHz system clock

**Clock Dividers**:
- AHB: 70 MHz (no division)
- APB: 35 MHz (÷2)

### Custom Clock Configuration

To use different clock settings, modify `port/abov_a31g123/common/A31G123Config.hpp`:

```cpp
constexpr uint32_t EXTERNAL_OSC_FREQ = 4000000UL;   // Change to 4 MHz
constexpr uint32_t PLL_OUTPUT_FREQ = 60000000UL;    // 4 MHz × 15
constexpr uint32_t SYSTEM_CLOCK_FREQ = 60000000UL;  // 60 MHz
```

### Internal RC Oscillator

If you don't have an external oscillator:

```cpp
// Use internal RC clock (typically 4 MHz, ±2% accuracy)
constexpr uint32_t EXTERNAL_OSC_FREQ = 4000000UL;
// PLL disabled
constexpr uint32_t SYSTEM_CLOCK_FREQ = 4000000UL;
```

⚠️ **Note**: Internal RC has poor accuracy; UART baud rates may be off.

---

## GPIO Configuration

### Pin Naming Convention

GPIO pins are named by port and number:
- **PA0 - PA31**: Port A, pins 0-31
- **PB0 - PB31**: Port B, pins 0-31
- **PC0 - PC31**: Port C, pins 0-31
- **PD0 - PD31**: Port D, pins 0-31
- **PE0 - PE31**: Port E, pins 0-31

### GPIO Modes

```cpp
#include "port/abov_a31g123/hal/A31G123Gpio.hpp"

// Create GPIO for PA5
auto gpio = A31G123Gpio(GPIOA_BASE, 5);

// Configure as output (LED)
gpio.setMode(GpioMode::OUTPUT);
gpio.write(GpioState::HIGH);    // LED on
gpio.write(GpioState::LOW);     // LED off
gpio.toggle();                   // Toggle

// Configure as input (Button)
gpio.setMode(GpioMode::INPUT_PULLUP);
if (gpio.read() == GpioState::LOW) {
    // Button pressed
}
```

### Special GPIO Pins

**SWD Debug Pins** (reserved):
- PA13 (SWDIO) - Serial Wire Data In/Out
- PA14 (SWDCK) - Serial Wire Clock
- Not available for general GPIO use

⚠️ If you disable SWD debugging, these pins become available.

---

## UART Configuration

### Supported UART Ports

| UART | TX Pin | RX Pin | APB Clock |
|------|--------|--------|-----------|
| UART0 | PA9 | PA10 | 35 MHz |
| UART1 | PB0 | PB1 | 35 MHz |
| UART2 | PC7 | PC8 | 35 MHz |
| UART3 | PD10 | PD11 | 35 MHz |

### Standard Baud Rates

```cpp
#include "port/abov_a31g123/hal/A31G123Uart.hpp"

// Create UART0 at 115200 baud
auto uart = A31G123Uart(0, 115200);

// Transmit
uart.putChar('A');
uart.write((const uint8_t*)"Hello", 5);

// Receive
if (uart.isDataAvailable()) {
    uint8_t byte = uart.getChar();
}
```

### Baud Rate Accuracy

Baud rates depend on APB clock accuracy. With 10 MHz crystal:

| Baud | Divisor | Actual | Error |
|------|---------|--------|-------|
| 9600 | 182 | 9589 | -0.1% |
| 19200 | 91 | 19186 | -0.1% |
| 115200 | 15 | 116,667 | +1.3% |

⚠️ Note: 115200 baud has ~1.3% error. For critical applications, use 9600 or 19200.

### Default Settings

- Data bits: 8
- Stop bits: 1
- Parity: None
- Hardware flow control: Not supported

---

## I2C Configuration

### Supported I2C Ports

| I2C | SDA Pin | SCL Pin | Clock |
|-----|---------|---------|-------|
| I2C0 | PA6 | PA7 | 100 kHz default |
| I2C1 | PB8 | PB9 | 100 kHz default |

### Standard Modes

```cpp
#include "port/abov_a31g123/hal/A31G123I2c.hpp"

// Standard Mode (100 kHz)
auto i2c = A31G123I2c(0, 100000);

// Fast Mode (400 kHz)
auto i2c = A31G123I2c(0, 400000);

// Write to slave
uint8_t data[] = {0x81, 0x00, 0x00};
I2cStatus status = i2c.write(0x3C, data, 3);

// Read from slave
uint8_t buffer[8];
status = i2c.read(0x3C, buffer, 8);

// Write then read (for register-based devices)
uint8_t reg = 0x00;  // Register address
uint8_t value[2];
status = i2c.writeRead(0x3C, &reg, 1, value, 2);
```

### I2C Clock Frequency

With 70 MHz AHB clock:
- 100 kHz: Divisor = 350
- 400 kHz: Divisor = 87

---

## SPI Configuration

### Supported SPI Ports

| SPI | MOSI | MISO | SCK | CS |
|-----|------|------|-----|-----|
| SPI0 | PA12 | PA11 | PA8 | PA4 |
| SPI1 | PB3 | PB2 | PB1 | PB0 |

### SPI Modes

```cpp
#include "port/abov_a31g123/hal/A31G123Spi.hpp"

// Mode 0 (CPOL=0, CPHA=0) - most common
auto spi = A31G123Spi(0, 1000000, A31G123Spi::Mode::MODE_0);

// Exchange data (full-duplex)
uint8_t tx[] = {0x01, 0x02, 0x03};
uint8_t rx[3];
spi.exchange(tx, rx, 3);

// Transmit only
spi.transmit_only(tx, 3);

// Receive only (transmit zeros)
spi.receive_only(rx, 3);
```

### SPI Clock Frequency

With 70 MHz system clock, maximum SPI clock is 35 MHz:

| Freq | Divisor |
|------|---------|
| 1 MHz | 70 |
| 5 MHz | 14 |
| 10 MHz | 7 |
| 35 MHz | 2 |

---

## Memory Layout

### Flash Memory

```
0x00000000 ┌─────────────────────┐
           │   Vector Table      │  (256 bytes)
0x00000100 ├─────────────────────┤
           │   Code (.text)      │
           │   Read-only data    │  (≈110 KB)
           │   (.rodata)         │
0x0001E800 ├─────────────────────┤
           │   Reserved/Free     │
0x00020000 ├─────────────────────┤
           │   EEPROM data       │  (4 KB, optional)
0x00021000 ├─────────────────────┤
           │   End of Flash      │
```

### RAM Memory

```
0x20000000 ┌─────────────────────┐
           │   .data section     │  (initialized)
0x20000100 ├─────────────────────┤
           │   .bss section      │  (uninitialized)
0x20002000 ├─────────────────────┤
           │   Heap (4 KB)       │  (grows ↑)
0x20003000 ├─────────────────────┤
           │                     │
           │   Stack (2 KB)      │  (grows ↓)
           │                     │
0x20008000 └─────────────────────┘
           (end of RAM)
```

### Linker Script

The provided linker script (`port/abov_a31g123/linker/A31G123.ld`) handles memory layout.

To customize:
```ld
/* Increase heap */
. += 8K;  /* Was 4K */

/* Decrease stack */
. += 1K;  /* Was 2K */
```

---

## Toolchain Configuration

### Prerequisites

```bash
# Install ARM Cortex-M cross-compiler
sudo apt install arm-none-eabi-gcc arm-none-eabi-gdb

# Verify installation
arm-none-eabi-gcc --version
arm-none-eabi-gdb --version
```

### Compiler Flags for M0+

The included toolchain (`cmake/toolchains/abov-a31g123.cmake`) sets:

```
-mcpu=cortex-m0plus
-mthumb
-mno-unaligned-access    (M0+ requires alignment)
-fno-exceptions
-fno-rtti
```

### Optimization Levels

```bash
# Debug build (slow, full symbols)
cmake -B build -DCMAKE_BUILD_TYPE=Debug

# Release build (fast, optimized)
cmake -B build -DCMAKE_BUILD_TYPE=Release

# Size-optimized (smallest binary)
cmake -B build -DCMAKE_BUILD_TYPE=MinSizeRel
```

---

## Known Issues & Workarounds

### Issue 1: Unaligned Access on M0+

**Problem**: Cortex-M0+ doesn't support unaligned memory access.

**Symptom**: HardFault exception when accessing misaligned data.

**Workaround**: Use `__attribute__((aligned(4)))` for data structures:

```cpp
struct __attribute__((aligned(4))) SensorData {
    uint8_t status;
    uint16_t temperature;
    uint16_t humidity;
};
```

### Issue 2: UART Baud Rate Inaccuracy

**Problem**: 115200 baud has ~1.3% error with 35 MHz clock.

**Symptom**: Garbled characters in UART output.

**Workaround**: Use lower baud rates (9600, 19200) or add error correction.

### Issue 3: I2C Bus Hang

**Problem**: If I2C slave doesn't release SDA, master hangs.

**Symptom**: I2C operations timeout.

**Workaround**: The implementation includes timeout detection. Check `wait_for_complete()` return value.

---

## Debugging

### Using GDB

```bash
# Start GDB with ELF file
arm-none-eabi-gdb build/a31g123/firmware.elf

# Connect to debugger (OpenOCD default port)
(gdb) target extended-remote :4242

# Load firmware
(gdb) load

# Run program
(gdb) run

# Set breakpoint
(gdb) break main

# Step through code
(gdb) step
(gdb) next

# Print variables
(gdb) print variable_name
(gdb) print /x variable_name  (hex format)
```

### Using Serial Monitor for Debugging

```bash
# Ubuntu/Linux
minicom -D /dev/ttyUSB0 -b 115200

# macOS
screen /dev/tty.usbserial-* 115200

# Windows (using PuTTY or others)
# Select COMx, 115200 baud, 8N1
```

### Analyzing Binary Size

```bash
# See size breakdown
arm-none-eabi-size build/a31g123/firmware.elf

# Output:
#    text    data     bss     dec     hex filename
#   12345    1024    2048   15417    3c09 firmware.elf

# Detailed breakdown
arm-none-eabi-nm -S build/a31g123/firmware.elf | sort -rn | head -20
```

---

## Performance Characteristics

### Memory Usage (Typical)

| Component | Typical Size |
|-----------|-------------|
| libemb::hal | ~1 KB |
| libemb::util | ~2 KB |
| GPIO driver | ~500 B |
| UART driver | ~2 KB |
| I2C driver | ~3 KB |
| **Total HAL** | ~8 KB |

### Clock Speed

- **System Clock**: 70 MHz
- **AHB Clock**: 70 MHz
- **APB Clock**: 35 MHz

### Typical Power Consumption

- **Normal operation** (70 MHz): ~30-50 mA
- **Idle** (WFI): ~1-5 mA
- **Sleep**: <100 µA
- **Standby**: <10 µA

---

## Next Steps

### For Board Bringup

1. Verify power supply (3.3V typical)
2. Check external oscillator (10 MHz)
3. Load bootloader or firmware
4. Test GPIO (LED blink)
5. Test UART (serial echo)
6. Test I2C (sensor communication)

### For Application Development

1. Review example code in `examples/05_abov_a31g123_demo/`
2. Start with GPIO and UART
3. Add I2C/SPI as needed
4. Integrate with drivers (Ssd1306, etc.)
5. Run unit tests with mocks before hardware testing

### For Custom Drivers

Follow the pattern in `include/libemb/hal/` and `src/driver/`:
1. Define interface (header only)
2. Implement for A31G123
3. Write unit tests
4. Test on hardware

---

## Useful References

- [ABOV A31G123 Datasheet](https://abov.co.kr/)
- [ARM Cortex-M0+ Reference](https://developer.arm.com/ip-products/processors/cortex-m/cortex-m0-plus)
- [CMake Cross-Compilation Guide](https://cmake.org/cmake/help/latest/manual/cmake-toolchains.7.html)
- [libemb Documentation](../docs/README.md)

---

## See Also

- ARCHITECTURE.md - Overall system design
- PORTING_MCU.md - Generic MCU porting guide
- ADDING_DRIVERS.md - How to add drivers
- examples/ - Working example code
