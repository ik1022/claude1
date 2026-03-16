# Example 05: ABOV A31G123 Demo Applications

**Difficulty**: 🔴🔴🔴 (Expert) | **Time**: 20 minutes | **Complexity**: Real Hardware

## Overview

Three working example applications demonstrating ABOV A31G123 MCU HAL usage:

1. **LED Blink** - GPIO output control
2. **UART Echo** - Serial communication
3. **I2C Display** - I2C master communication with SSD1306 OLED

These examples are designed to run on actual ABOV A31G123 hardware.

## Prerequisites

### Hardware

- ABOV A31G123 development board or custom circuit
- External 10 MHz crystal oscillator (or internal RC)
- USB UART adapter (for UART example)
- SSD1306 I2C OLED display (128×64) - optional for I2C example
- ABOV LINK debugger or STM32 ST-Link compatible (SWD interface)

### Software

```bash
# Cross-compiler toolchain
sudo apt install arm-none-eabi-gcc arm-none-eabi-gdb

# CMake
sudo apt install cmake

# Optional: Serial monitor
sudo apt install minicom
```

## Building

### Build for A31G123

```bash
cd libemb

# Configure with A31G123 toolchain
cmake -B build/a31g123 \
  -DCMAKE_TOOLCHAIN_FILE=cmake/toolchains/abov-a31g123.cmake \
  -DLIBEMB_BUILD_EXAMPLES=ON

# Build all examples
cmake --build build/a31g123 --target a31g123_blink
cmake --build build/a31g123 --target a31g123_uart_echo
cmake --build build/a31g123 --target a31g123_i2c_display
```

### Output Files

```
build/a31g123/examples/05_abov_a31g123_demo/
├── a31g123_blink.elf        (ELF with debug symbols)
├── a31g123_blink.hex        (Flash programming format)
├── a31g123_blink.bin        (Raw binary)
├── a31g123_uart_echo.*
└── a31g123_i2c_display.*
```

## Example 1: LED Blink

**File**: `blink.cpp`

Simplest example - toggles LED every 1 second.

### Hardware Setup

```
PA5 ──┬─── LED (+)
      │
     R1 (1kΩ resistor)
      │
     GND ─── LED (-)
```

### Running

```bash
# Flash with GDB
arm-none-eabi-gdb build/a31g123/examples/05_abov_a31g123_demo/a31g123_blink.elf
(gdb) target extended-remote :4242
(gdb) load
(gdb) run
```

### Expected Behavior

- LED on PA5 blinks at 1 Hz (500ms on, 500ms off)
- LED continues blinking indefinitely
- If blinking stops: MCU has crashed (investigate with GDB)

### Key Points

- Uses `delay_ms()` for timing (busy-wait, not accurate)
- Simple GPIO output control via `write()` and `toggle()`
- No interrupts, pure polling
- Minimal dependencies

### Improvements

- Replace `delay_ms()` with hardware timer
- Use systick for accurate timing
- Add button input to toggle LED

---

## Example 2: UART Echo

**File**: `uart_echo.cpp`

Demonstrates serial communication by echoing characters.

### Hardware Setup

```
         ┌─── USB UART Adapter ───── Computer (Terminal)
UART0    │
┌────┬───┤ PA9  (TX) ─── RX (white)
│    │   │ PA10 (RX) ─── TX (yellow)
│    │   │ GND ───────── GND (black)
└────┴───┘
```

### Terminal Configuration

```bash
# Using minicom
minicom -D /dev/ttyUSB0 -b 115200

# Or using screen
screen /dev/ttyUSB0 115200

# Key bindings:
# Ctrl+A Z → Help
# Ctrl+A X → Exit
```

### Running

```bash
# Flash firmware
arm-none-eabi-gdb build/a31g123/examples/05_abov_a31g123_demo/a31g123_uart_echo.elf
(gdb) target extended-remote :4242
(gdb) load
(gdb) run

# In separate terminal: connect with minicom
minicom -D /dev/ttyUSB0 -b 115200
```

### Expected Behavior

```
╔════════════════════════════════════════════════╗
║   ABOV A31G123 UART Echo Server               ║
║   Baud: 115200, Data: 8N1                    ║
║   Type characters to echo...                  ║
╚════════════════════════════════════════════════╝

> hello
hello
>
```

### Special Handling

- **Enter** → Echo as CRLF (`\r\n`) and show prompt
- **Backspace** → Erase on terminal
- **Control characters** → Echo as `.` (dot)
- **Printable ASCII** → Echo normally

### Key Points

- Non-blocking UART (polling for data)
- RingBuffer for RX buffering (256 bytes)
- APB clock: 35 MHz for UART timing
- Baud rate: 115200 (1.3% error at this freq)

### Improvements

- Add interrupt-driven RX
- Implement buffered command processing
- Add statistics (bytes received/transmitted)
- Support flow control (RTS/CTS)

---

## Example 3: I2C Display

**File**: `i2c_display.cpp`

Controls SSD1306 OLED display via I2C.

### Hardware Setup

```
         ┌─ 3.3V (via 10k pull-up)
I2C0     │
┌────┬───┤ PA6 (SDA) ─── SDA (blue)  ─┐
│    │   │                             │ SSD1306
│    │   │ PA7 (SCL) ─── SCL (yellow) ─┤ I2C OLED
│    │   │ GND ───────── GND (black) ──┤ 128×64
└────┴───┤ 3.3V ──────── VCC (red) ────┘
         │
         └─ 3.3V (via 10k pull-up)
```

### Running

```bash
# Flash firmware (requires hardware)
arm-none-eabi-gdb build/a31g123/examples/05_abov_a31g123_demo/a31g123_i2c_display.elf
(gdb) target extended-remote :4242
(gdb) load
(gdb) run
```

### Expected Behavior

1. **Startup** (1 second after reset)
   - OLED powers up
   - Display shows border pattern
   - Center square pattern appears

2. **Animation**
   - Horizontal line moves vertically
   - Updates 20 times per second
   - Continuous until power-off

3. **Error Indication**
   - If display not found: LED blinks at 10 Hz
   - I2C initialization failed
   - Check pull-up resistors and connections

### I2C Protocol Details

| Aspect | Value |
|--------|-------|
| Speed | 100 kHz (standard mode) |
| Slave Address | 0x3C |
| Data Format | 8N1 (8 bits, no parity, 1 stop) |
| Timing | Each init command ~100 µs, display update ~10 ms |

### Key Points

- I2C master implementation (blocking)
- SSD1306 driver integration
- Pixel-level graphics control
- Error handling with LED feedback

### Improvements

- Add text rendering
- Implement partial display updates
- Add button control for menu
- Multi-display I2C scanning

---

## Troubleshooting

### LED Blink Not Working

| Symptom | Cause | Solution |
|---------|-------|----------|
| LED never lights | Wrong pin | Check PA5 in schematic |
| LED always on | GPIO stuck HIGH | Verify output write() |
| LED doesn't toggle | Timing issue | Check delay_ms() accuracy |
| Program crashes | Stack overflow | Check reset handler |

### UART Echo Not Working

| Symptom | Cause | Solution |
|---------|-------|----------|
| No output | Wrong baud rate | Use 115200 exactly |
| Garbled text | Baud mismatch | Verify terminal settings |
| Can't receive | RX not connected | Check PA10 connection |
| TX but no RX | ISR not firing | Enable UART RX interrupt |

### I2C Display Not Working

| Symptom | Cause | Solution |
|---------|-------|----------|
| Display blank | Not initialized | Check init() return value |
| LED blinks | Display not found | Verify I2C address 0x3C |
| Partial display | Bus error | Add pull-up resistors (10k) |
| Slow updates | Wrong I2C frequency | Use 100 kHz default |

## Performance

### Memory Usage

| Example | Flash | RAM |
|---------|-------|-----|
| blink | ~2 KB | ~1 KB |
| uart_echo | ~4 KB | ~2 KB |
| i2c_display | ~6 KB | ~2 KB |

### Timing Accuracy

| Function | Accuracy | Notes |
|----------|----------|-------|
| `delay_ms()` | ±10% | Depends on compiler |
| UART baud | ±1.3% | 115200 baud |
| I2C clock | ±0.5% | 100 kHz mode |

## Next Steps

### Extend Blink Example

```cpp
// Add button input
A31G123Gpio button(GPIOA_BASE, 10);
button.setMode(GpioMode::INPUT_PULLUP);

if (button.read() == GpioState::LOW) {
    led.toggle();  // Toggle on press
}
```

### Extend UART Example

```cpp
// Add UART interrupt
extern "C" void UART0_IRQHandler() {
    uint8_t byte = read_uart_rx_reg();
    rx_buffer.push(byte);
}
```

### Extend I2C Example

```cpp
// Add text rendering
display.drawText(0, 0, "Hello");
display.drawText(0, 8, "ABOV A31G123");
display.display();
```

## Resources

- **Datasheet**: ABOV A31G123 MCU datasheet
- **Register Map**: `port/abov_a31g123/doc/REGISTER_MAPPING.md` (future)
- **HAL API**: `include/libemb/hal/` interfaces
- **Porting Guide**: `docs/PORTING_ABOV_A31G123.md`

## See Also

- Example 01: RingBuffer (data structures)
- Example 02: UART (software UART)
- Example 03: Display (SSD1306 driver)
- Example 04: Sensor Integration (multi-component)

---

**Note**: These examples assume a functional A31G123 board with proper power, oscillator, and connections. Hardware errors will manifest as locked-up MCU or silent failures.

**Debugging Tip**: Use GDB breakpoints to step through execution:
```gdb
(gdb) break main
(gdb) break blink.cpp:42
(gdb) continue
(gdb) step
```
