# Example 04: Sensor Integration

**Difficulty**: 🔴🔴⚪ (Advanced) | **Time**: 10 minutes | **Complexity**: Full System

## Overview

Demonstrates combining multiple libemb components into a cohesive system:

- **I2C Communication**: MockI2c simulates I2C bus
- **Driver Integration**: Ssd1306 OLED display driver
- **Data Buffering**: RingBuffer for sensor data logging
- **User Interface**: MockGpio for button input
- **System Design**: Multi-component architecture

## Concepts Learned

### 1. Component Composition

Building systems from independent, testable components:

```
┌─────────────────────────────────────┐
│   EnvironmentalMonitor (Orchestrator)│
├──────────────┬──────────────┬───────┤
│              │              │       │
▼              ▼              ▼       ▼
Ssd1306    RingBuffer   MockI2c  MockGpio
(Display)  (Buffer)     (Bus)    (Button)
```

### 2. Hardware Abstraction

All hardware accessed through interfaces:
- `IGpio` - Button input
- `II2c` - Display communication
- Mocks replace real hardware seamlessly

### 3. Data Flow

```
Sensor Data → RingBuffer → Display Updates → I2C Transmit
     ↓                           ↓                  ↓
  Simulate              Update OLED          MockI2c records
```

### 4. System Initialization

```cpp
// 1. Create mocks
auto i2c = std::make_unique<MockI2c>();
auto button = std::make_unique<MockGpio>();

// 2. Create monitor (inject dependencies)
auto monitor = EnvironmentalMonitor(i2c.get(), button.get());

// 3. Initialize system
monitor->initialize();

// 4. Operate
monitor->measure();
```

## Code Structure

### EnvironmentalMonitor Class

Main system component that orchestrates:

```cpp
class EnvironmentalMonitor {
    // Dependencies injected via constructor
    libemb::hal::II2c* i2c_;
    libemb::hal::IGpio* button_;

    // Owned components
    libemb::driver::Ssd1306 display_;
    libemb::util::RingBuffer<uint8_t, 100> data_buffer_;

public:
    // Initialize all sub-components
    bool initialize();

    // Simulate sensor reading
    SensorReading read_sensor();

    // Execute measurement cycle
    void measure();
};
```

### Key Design Patterns

**1. Dependency Injection**
```cpp
// Dependencies passed to constructor
EnvironmentalMonitor(libemb::hal::II2c* i2c, libemb::hal::IGpio* button)
    : i2c_(i2c), button_(button) { }
```

**2. Hardware Independence**
```cpp
// Works with real or mock hardware
libemb::hal::II2c* i2c_;  // Could be real STM32, mock, or other implementation
```

**3. Composition Over Inheritance**
```cpp
// Owns components, doesn't inherit from them
libemb::driver::Ssd1306 display_;
libemb::util::RingBuffer<uint8_t, 100> data_buffer_;
```

## Expected Output

```
╔════════════════════════════════════════════════╗
║     libemb Example 04: Sensor Integration     ║
║  Multi-Component Demonstration with Mocks     ║
╚════════════════════════════════════════════════╝

=== Environmental Monitor Initialization ===
✓ OLED Display initialized (I2C address: 0x3C)
✓ Button configured (GPIO input with pullup)
✓ Monitor ready to collect data

Collecting sensor data...

[ 1] Temp: 20°C, Pressure: 1003 Pa | Buffer: 3/100 bytes
[ 2] Temp: 21°C, Pressure: 1006 Pa | Buffer: 6/100 bytes
[ 3] Temp: 22°C, Pressure: 1009 Pa | Buffer: 9/100 bytes
[ 4] Temp: 23°C, Pressure: 1012 Pa | Buffer: 12/100 bytes
[ 5] Temp: 24°C, Pressure: 1015 Pa [BUTTON PRESSED] | Buffer: 15/100 bytes
...

=== Collection Complete ===
Samples collected: 10
Buffer usage: 30 bytes
I2C transactions: 15
Button events: 11

✓ All components working correctly!
```

## Building and Running

```bash
# Build
cd examples/04_sensor_integration
cmake -B build -DLIBEMB_BUILD_EXAMPLES=ON
cmake --build build

# Run
./build/examples/example_04_sensor_integration
```

## Key Takeaways

### ✅ What This Example Demonstrates

1. **Modularity**: Each component has single responsibility
2. **Testability**: All components replaceable with mocks
3. **Composition**: Building complex systems from simple parts
4. **Hardware Abstraction**: Code independent of actual hardware
5. **Dependency Injection**: Flexible component configuration
6. **Real-World Pattern**: Actual embedded systems use this approach

### ❌ What This Example Does NOT Do

- Does not include actual sensor drivers (BMP280, etc.)
- Does not perform real I2C communication
- Does not render actual graphics on OLED
- Does not handle real-time constraints

## Architecture Lessons

### From Simple to Complex

```
Example 1: Single Component
└── RingBuffer

Example 2: Component + Interface
├── RingBuffer
└── UART + MockUart

Example 3: Driver Integration
├── Ssd1306 Driver
├── MockI2c
└── Component Dependencies

Example 4: Full System ⭐
├── EnvironmentalMonitor (Orchestrator)
├── Ssd1306 (Display Driver)
├── RingBuffer (Data Storage)
├── MockI2c (Communication)
└── MockGpio (User Input)
```

## Next Steps

### To Extend This Example

1. **Add Real Sensor Driver**: Implement BMP280 temperature sensor
2. **Text Rendering**: Add simple ASCII text to OLED display
3. **Data Logging**: Log buffer contents to file
4. **Configuration**: Add settings menu via button input
5. **Real Hardware**: Replace mocks with actual I2C/GPIO

### To Learn More

- **ARCHITECTURE.md** - System design patterns
- **ADDING_DRIVERS.md** - How to add BMP280 sensor
- **TESTING_STRATEGY.md** - Testing complex systems
- **PERFORMANCE_GUIDE.md** - Optimization techniques

## Advanced Topics

### Error Handling Integration

```cpp
// Monitor could handle errors gracefully
bool success = monitor->initialize();
if (!success) {
    // Handle initialization error
    // Perhaps display error on OLED
    // Log to buffer
}
```

### State Machine Pattern

```cpp
enum class MonitorState {
    IDLE,
    MEASURING,
    DISPLAYING,
    LOGGING,
    ERROR
};

// Monitor transitions between states
```

### Event-Driven Design

```cpp
// Instead of polling button:
void on_button_pressed() {
    // React to user input
    // Toggle data logging
    // Change display mode
}
```

---

**Estimated Learning Time**: 10 minutes
**Difficulty Progression**: ⭐ ⭐ ⭐ ⭐ (Fourth in series)
