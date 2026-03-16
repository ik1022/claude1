# Mock Implementation Guide

Complete guide to creating Mock HAL implementations for testing in libemb.

## Overview

Mocks are test doubles that simulate hardware behavior without actual hardware, enabling unit testing of drivers and applications on the host PC.

### Why Mocks Matter

- **Test Hardware Independence**: Test drivers without physical hardware
- **Deterministic Behavior**: Control inputs/outputs for reliable tests
- **Fast Execution**: No I/O delays or timing variability
- **Cost Reduction**: Don't need hardware for every test scenario
- **Error Injection**: Simulate hardware failures and edge cases

### Mocks vs. Real Hardware

| Aspect | Mock | Real Hardware |
|--------|------|---------------|
| **Speed** | ~μs (microseconds) | ms (milliseconds) |
| **Determinism** | 100% predictable | Hardware dependent |
| **Cost** | Free | $$ to $$$ |
| **Complexity** | Simpler | Full behavior |
| **Testing** | Unit/integration | System integration |
| **Use Case** | Development/CI | Final validation |

---

## Architecture: Mock Pattern

### Basic Structure

```cpp
class MockInterface : public IHardware {
public:
    // Record behavior for verification
    std::vector<Transaction> transactions;

    // Controllable state
    DataBuffer input_buffer;
    DataBuffer output_buffer;

    // Methods from interface
    virtual bool write(const uint8_t* data, size_t len) override {
        // Record transaction
        transactions.push_back({WRITE, data, len});

        // Process (update state, populate output buffer)
        process_write(data, len);

        return true;
    }

    // Test helpers
    const Transaction& last_transaction() const {
        return transactions.back();
    }

    void clear_history() {
        transactions.clear();
    }
};
```

---

## MockI2c Implementation

The I2C mock simulates an Inter-Integrated Circuit bus.

### Header: `port/mock/MockI2c.hpp`

```cpp
#pragma once

#include "libemb/hal/II2c.hpp"
#include <vector>
#include <cstring>

namespace libemb::port::mock {

/**
 * @brief Mock I2C implementation for testing
 *
 * Records all transactions for verification.
 * Allows injection of read data and error responses.
 */
class MockI2c : public libemb::hal::II2c {
public:
    /// I2C transaction record
    struct Transaction {
        enum Type { WRITE, READ, WRITE_READ } type;
        uint8_t slave_addr;
        std::vector<uint8_t> data;
        size_t length;
        I2cStatus status;
    };

    virtual I2cStatus write(uint8_t slaveAddr, const uint8_t* data, size_t length) override {
        transactions_.push_back({Transaction::WRITE, slaveAddr,
                               std::vector<uint8_t>(data, data + length),
                               length, I2cStatus::OK});
        return response_status_;
    }

    virtual I2cStatus read(uint8_t slaveAddr, uint8_t* data, size_t length) override {
        // Copy prepared response data
        if (read_data_.size() >= length) {
            std::memcpy(data, read_data_.data(), length);
            read_data_.erase(read_data_.begin(), read_data_.begin() + length);
        }

        transactions_.push_back({Transaction::READ, slaveAddr, {}, length, I2cStatus::OK});
        return response_status_;
    }

    virtual I2cStatus writeRead(uint8_t slaveAddr, const uint8_t* writeData, size_t writeLength,
                                uint8_t* readData, size_t readLength) override {
        // Record write
        write(slaveAddr, writeData, writeLength);

        // Record read
        return read(slaveAddr, readData, readLength);
    }

    // Test helpers
    /// Get all recorded transactions
    const std::vector<Transaction>& transactions() const { return transactions_; }

    /// Get last transaction
    const Transaction& last_transaction() const { return transactions_.back(); }

    /// Clear transaction history
    void clear_history() { transactions_.clear(); }

    /// Set response data for next read
    void set_read_data(const std::vector<uint8_t>& data) { read_data_ = data; }

    /// Set response status for next operation
    void set_response_status(I2cStatus status) { response_status_ = status; }

    /// Verify that data was written to specific address
    bool verify_write(uint8_t addr, const uint8_t* expected, size_t len) const {
        for (const auto& tx : transactions_) {
            if (tx.type == Transaction::WRITE && tx.slave_addr == addr) {
                if (tx.data.size() == len) {
                    return std::equal(tx.data.begin(), tx.data.end(), expected);
                }
            }
        }
        return false;
    }

    /// Count transactions of specific type to address
    size_t count_writes_to(uint8_t addr) const {
        return std::count_if(transactions_.begin(), transactions_.end(),
                           [addr](const Transaction& tx) {
                               return tx.type == Transaction::WRITE && tx.slave_addr == addr;
                           });
    }

private:
    std::vector<Transaction> transactions_;
    std::vector<uint8_t> read_data_;
    I2cStatus response_status_ = I2cStatus::OK;
};

} // namespace libemb::port::mock
```

### Usage Example

```cpp
TEST(Ssd1306, InitializeViaI2c) {
    // Setup
    auto i2c = std::make_unique<MockI2c>();
    auto display = std::make_unique<Ssd1306>();

    // Set expected init sequence response
    i2c->set_read_data({0x3F});  // Status register

    // Execute
    bool success = display->init(i2c.get(), 0x3C);

    // Verify
    ASSERT_TRUE(success);
    ASSERT_GT(i2c->transactions().size(), 0);
    ASSERT_TRUE(i2c->verify_write(0x3C, init_commands, sizeof(init_commands)));
}
```

---

## MockUart Implementation

The UART mock simulates serial communication.

### Header: `port/mock/MockUart.hpp`

```cpp
#pragma once

#include "libemb/hal/IUart.hpp"
#include <queue>
#include <vector>

namespace libemb::port::mock {

/**
 * @brief Mock UART for testing serial communication
 */
class MockUart : public libemb::hal::IUart {
public:
    virtual void putChar(uint8_t byte) override {
        tx_buffer_.push_back(byte);
    }

    virtual uint8_t getChar() override {
        if (rx_buffer_.empty()) return 0;
        uint8_t byte = rx_buffer_.front();
        rx_buffer_.pop();
        return byte;
    }

    virtual bool isDataAvailable() const override {
        return !rx_buffer_.empty();
    }

    virtual void write(const uint8_t* data, size_t length) override {
        for (size_t i = 0; i < length; i++) {
            tx_buffer_.push_back(data[i]);
        }
    }

    virtual size_t read(uint8_t* data, size_t maxLength) override {
        size_t count = 0;
        while (count < maxLength && !rx_buffer_.empty()) {
            data[count++] = rx_buffer_.front();
            rx_buffer_.pop();
        }
        return count;
    }

    // Test helpers
    const std::vector<uint8_t>& tx_buffer() const { return tx_buffer_; }
    void clear_tx_buffer() { tx_buffer_.clear(); }

    void prepare_rx_data(const std::vector<uint8_t>& data) {
        for (uint8_t byte : data) {
            rx_buffer_.push(byte);
        }
    }

    std::string tx_as_string() const {
        return std::string(tx_buffer_.begin(), tx_buffer_.end());
    }

private:
    std::vector<uint8_t> tx_buffer_;
    std::queue<uint8_t> rx_buffer_;
};

} // namespace libemb::port::mock
```

### Usage Example

```cpp
TEST(RingBuffer, TransmitViaUart) {
    // Setup
    auto uart = std::make_unique<MockUart>();
    libemb::util::RingBuffer<uint8_t, 10> buffer;

    // Prepare test data
    const char* message = "Hello";
    uart->prepare_rx_data({'H', 'e', 'l', 'l', 'o'});

    // Execute: transmit 5 bytes
    uint8_t data[5];
    size_t received = uart->read(data, 5);

    // Verify
    ASSERT_EQ(received, 5);
    ASSERT_EQ(std::string((char*)data, 5), "Hello");
}
```

---

## MockGpio Implementation

The GPIO mock simulates digital input/output pins.

### Header: `port/mock/MockGpio.hpp`

```cpp
#pragma once

#include "libemb/hal/IGpio.hpp"
#include <vector>

namespace libemb::port::mock {

/**
 * @brief Mock GPIO pin for testing
 */
class MockGpio : public libemb::hal::IGpio {
public:
    struct PinEvent {
        enum Type { MODE_CHANGE, WRITE, TOGGLE } type;
        GpioMode mode;
        GpioState state;
        size_t timestamp;
    };

    virtual void setMode(GpioMode mode) override {
        mode_ = mode;
        history_.push_back({PinEvent::MODE_CHANGE, mode, GpioState::LOW,
                          history_.size()});
    }

    virtual void write(GpioState state) override {
        state_ = state;
        history_.push_back({PinEvent::WRITE, mode_, state, history_.size()});
    }

    virtual GpioState read() const override {
        return state_;
    }

    virtual void toggle() override {
        state_ = (state_ == GpioState::HIGH) ? GpioState::LOW : GpioState::HIGH;
        history_.push_back({PinEvent::TOGGLE, mode_, state_, history_.size()});
    }

    // Test helpers
    const std::vector<PinEvent>& history() const { return history_; }
    void clear_history() { history_.clear(); }

    size_t count_toggles() const {
        return std::count_if(history_.begin(), history_.end(),
                           [](const PinEvent& e) { return e.type == PinEvent::TOGGLE; });
    }

    GpioMode current_mode() const { return mode_; }
    GpioState current_state() const { return state_; }

private:
    GpioMode mode_ = GpioMode::INPUT;
    GpioState state_ = GpioState::LOW;
    std::vector<PinEvent> history_;
};

} // namespace libemb::port::mock
```

---

## Testing Patterns

### Pattern 1: Transaction Verification

Verify that specific commands were sent:

```cpp
TEST(Driver, SendCorrectInitSequence) {
    auto i2c = std::make_unique<MockI2c>();
    auto driver = std::make_unique<MyDriver>();

    // Initialize driver
    driver->init(i2c.get(), 0x3C);

    // Verify init command was sent
    ASSERT_TRUE(i2c->verify_write(0x3C, INIT_COMMAND, sizeof(INIT_COMMAND)));

    // Verify number of I2C operations
    ASSERT_EQ(i2c->transactions().size(), 3);
}
```

### Pattern 2: State Assertions

Verify mock state after operations:

```cpp
TEST(Uart, BufferFill) {
    auto uart = std::make_unique<MockUart>();

    // Send data
    uart->write((const uint8_t*)"TEST", 4);

    // Verify transmit buffer
    ASSERT_EQ(uart->tx_buffer().size(), 4);
    ASSERT_EQ(uart->tx_as_string(), "TEST");
}
```

### Pattern 3: Response Injection

Inject responses from hardware:

```cpp
TEST(Sensor, HandleReadError) {
    auto i2c = std::make_unique<MockI2c>();
    auto sensor = std::make_unique<BMP280>();

    // Simulate I2C error
    i2c->set_response_status(I2cStatus::NACK);
    i2c->set_read_data({});  // No data

    // Verify error handling
    bool result = sensor->read_temperature(i2c.get());
    ASSERT_FALSE(result);  // Should handle error gracefully
}
```

### Pattern 4: Behavioral Testing

Test behavior based on inputs/outputs:

```cpp
TEST(Display, UpdatesOnDataChange) {
    auto i2c = std::make_unique<MockI2c>();
    auto display = std::make_unique<Ssd1306>();

    display->init(i2c.get(), 0x3C);
    size_t initial_transactions = i2c->transactions().size();

    // Change pixel
    display->setPixel(10, 10, true);

    // Should not transmit until display() is called
    ASSERT_EQ(i2c->transactions().size(), initial_transactions);

    // Call display
    display->display();

    // Now should have new transactions
    ASSERT_GT(i2c->transactions().size(), initial_transactions);
}
```

---

## Best Practices

### 1. Keep Mocks Simple

Mock only what you need to test. Don't simulate perfect hardware behavior—simulate realistic scenarios.

```cpp
// ❌ Too complex
class RealI2cSimulation {
    // Simulate timing, power consumption, thermal effects...
};

// ✅ Simple and focused
class MockI2c {
    // Just record/verify transactions
};
```

### 2. Record Observable Behaviors

Record what the test can actually observe and verify:

```cpp
// ✅ Good: Records what was written
void write(...) {
    transactions_.push_back({WRITE, data, length});
}

// ❌ Bad: Stores internal state that test can't verify
void write(...) {
    internal_bus_state_ = calculate_complex_state(data);
}
```

### 3. Provide Verification Methods

Make assertions easy:

```cpp
// ✅ Good: Helper methods
bool verify_write(uint8_t addr, const uint8_t* data, size_t len);
size_t count_reads_from(uint8_t addr);
const Transaction& last_transaction();

// ❌ Bad: Force tests to manually search
const auto& tx = i2c->transactions()[i2c->transactions().size() - 1];
```

### 4. Support Error Injection

Allow tests to simulate hardware failures:

```cpp
// ✅ Good: Can simulate errors
i2c->set_response_status(I2cStatus::TIMEOUT);
i2c->set_response_status(I2cStatus::NACK);

// ❌ Bad: Always succeeds
// No way to test error handling
```

### 5. Document Assumptions

Clear documentation prevents misunderstandings:

```cpp
/**
 * @brief Mock I2C that records all transactions
 *
 * Assumptions:
 * - All writes succeed unless set_response_status() is called
 * - Read data must be prepared via set_read_data() before read()
 * - Transactions are recorded in order
 * - Buffer sizes are unlimited (no real hardware limits)
 */
class MockI2c : public II2c {
    // ...
};
```

---

## Common Pitfalls

### Pitfall 1: Mocks That Are Too Realistic

**Problem**: Mocks that replicate all hardware details slow down tests and are hard to maintain.

```cpp
// ❌ Too complex
class MockI2c {
    // Simulates timing, I2C bus states, arbitration...
    void simulate_bus_collision() { /* ... */ }
    void calculate_propagation_delay() { /* ... */ }
};

// ✅ Focused
class MockI2c {
    // Just track transactions
    std::vector<Transaction> transactions;
};
```

### Pitfall 2: Hard to Verify Behavior

**Problem**: Tests that are hard to write or read because mock doesn't provide helpers.

```cpp
// ❌ Hard to verify
ASSERT_EQ(i2c->get_transaction_history()[2].data[0], 0x81);

// ✅ Easy to verify
ASSERT_TRUE(i2c->verify_write(0x3C, {0x81, ...}));
```

### Pitfall 3: Mocks That Don't Match Interface

**Problem**: Mock doesn't behave like real hardware in important ways.

```cpp
// ❌ Doesn't match interface
class MockI2c {
    // Real I2c returns I2cStatus
    int write() { return 0; }  // WRONG TYPE!
};

// ✅ Matches interface exactly
class MockI2c : public II2c {
    I2cStatus write(...) override { return I2cStatus::OK; }
};
```

### Pitfall 4: Shared State Between Tests

**Problem**: Tests that pollute each other.

```cpp
// ❌ Shared state
MockI2c* global_i2c = nullptr;

TEST(Test1, ...) {
    global_i2c = new MockI2c();
    // ...
}  // Leak! global_i2c still exists

TEST(Test2, ...) {
    // global_i2c has leftover state from Test1
}

// ✅ Isolated state
TEST(Test1, ...) {
    auto i2c = std::make_unique<MockI2c>();
    // ...
}  // i2c destroyed, clean state

TEST(Test2, ...) {
    auto i2c = std::make_unique<MockI2c>();  // Fresh instance
    // ...
}
```

---

## Integration with Google Test

### Fixture Pattern

```cpp
class I2cDriverTest : public ::testing::Test {
protected:
    void SetUp() override {
        i2c_ = std::make_unique<MockI2c>();
        driver_ = std::make_unique<MyDriver>();
    }

    void TearDown() override {
        // Cleanup if needed
    }

    std::unique_ptr<MockI2c> i2c_;
    std::unique_ptr<MyDriver> driver_;
};

TEST_F(I2cDriverTest, InitializeDriver) {
    ASSERT_TRUE(driver_->init(i2c_.get()));
    ASSERT_GT(i2c_->transactions().size(), 0);
}

TEST_F(I2cDriverTest, HandleReadError) {
    i2c_->set_response_status(I2cStatus::TIMEOUT);
    ASSERT_FALSE(driver_->read(i2c_.get()));
}
```

---

## Summary

Mocks are essential for testable embedded software:

1. **Record** - Capture what hardware operations occurred
2. **Verify** - Assert correct interactions
3. **Control** - Inject responses and errors
4. **Isolate** - Test drivers without hardware

Keep mocks simple, focused, and well-documented. They're not meant to be perfect hardware simulators—they're meant to be fast, reliable test doubles.

---

## See Also

- `port/mock/` - Existing mock implementations
- `test/unit/` - Example tests using mocks
- TESTING_STRATEGY.md - Comprehensive testing guide
- PERFORMANCE_GUIDE.md - Benchmarking with mocks
