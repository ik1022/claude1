/**
 * @file main.cpp
 * @brief Example 04: Sensor Integration
 *
 * Demonstrates combining multiple libemb components:
 * - I2C communication (MockI2c)
 * - OLED display (Ssd1306 driver)
 * - Data buffering (RingBuffer)
 * - User interface (MockGpio button)
 *
 * This example simulates an environmental monitoring system that reads
 * sensor data, displays it on an OLED, and logs data in a circular buffer.
 */

#include <iostream>
#include <iomanip>
#include <memory>

#include "libemb/util/RingBuffer.hpp"
#include "libemb/driver/Ssd1306.hpp"
#include "port/mock/MockI2c.hpp"
#include "port/mock/MockGpio.hpp"

using std::cout;
using std::endl;

// Simulated sensor data
struct SensorReading {
    uint8_t temp;      // Temperature in Celsius
    uint16_t pressure; // Pressure in Pa (scaled)

    SensorReading() : temp(0), pressure(0) {}
    SensorReading(uint8_t t, uint16_t p) : temp(t), pressure(p) {}
};

/**
 * @class EnvironmentalMonitor
 * @brief Integrates multiple libemb components for sensor monitoring
 */
class EnvironmentalMonitor {
public:
    EnvironmentalMonitor(libemb::hal::II2c* i2c, libemb::hal::IGpio* button)
        : i2c_(i2c), button_(button), sample_count_(0) {
        // Initialize data buffer
        data_buffer_.clear();
    }

    /**
     * @brief Initialize the monitor
     * @return true if initialization successful
     */
    bool initialize() {
        cout << "=== Environmental Monitor Initialization ===" << endl;

        // Initialize display
        if (!display_.init(i2c_, 0x3C)) {
            cout << "ERROR: Failed to initialize display" << endl;
            return false;
        }

        cout << "✓ OLED Display initialized (I2C address: 0x3C)" << endl;

        // Clear display
        display_.clear();
        display_.display();

        // Configure button
        button_->setMode(libemb::hal::GpioMode::INPUT_PULLUP);
        cout << "✓ Button configured (GPIO input with pullup)" << endl;

        cout << "✓ Monitor ready to collect data" << endl << endl;
        return true;
    }

    /**
     * @brief Simulate reading sensor data
     * @return Sensor reading with simulated temperature and pressure
     */
    SensorReading read_sensor() {
        // Simulate temperature (20-30°C)
        uint8_t temp = 20 + (sample_count_ % 11);

        // Simulate pressure (1000-1030 Pa, scaled)
        uint16_t pressure = 1000 + (sample_count_ * 3) % 30;

        return SensorReading(temp, pressure);
    }

    /**
     * @brief Process one measurement cycle
     */
    void measure() {
        sample_count_++;

        // Read simulated sensor
        SensorReading reading = read_sensor();

        // Store in buffer
        if (!data_buffer_.full()) {
            data_buffer_.push(reading.temp);
            data_buffer_.push((uint8_t)(reading.pressure >> 8));
            data_buffer_.push((uint8_t)(reading.pressure & 0xFF));
        }

        // Display results
        cout << "[" << std::setw(2) << (int)sample_count_ << "] ";
        cout << "Temp: " << (int)reading.temp << "°C, ";
        cout << "Pressure: " << (int)reading.pressure << " Pa";

        // Check button (display state)
        if (button_->read() == libemb::hal::GpioState::LOW) {
            cout << " [BUTTON PRESSED]";
        }

        cout << " | ";
        cout << "Buffer: " << data_buffer_.size() << "/"
             << (int)libemb::util::RingBuffer<uint8_t, 100>::CAPACITY << " bytes";

        cout << endl;

        // Update display every 3 samples
        if (sample_count_ % 3 == 0) {
            update_display(reading);
        }
    }

    /**
     * @brief Get number of samples collected
     */
    size_t sample_count() const { return sample_count_; }

    /**
     * @brief Get buffer fill level
     */
    size_t buffer_size() const { return data_buffer_.size(); }

private:
    libemb::hal::II2c* i2c_;
    libemb::hal::IGpio* button_;
    libemb::driver::Ssd1306 display_;
    libemb::util::RingBuffer<uint8_t, 100> data_buffer_;
    size_t sample_count_;

    /**
     * @brief Update OLED display with current sensor data
     */
    void update_display(const SensorReading& reading) {
        // Clear display
        display_.clear();

        // Draw border
        display_.drawHLine(0, 0, 128, true);
        display_.drawHLine(0, 63, 128, true);
        display_.drawVLine(0, 0, 64, true);
        display_.drawVLine(127, 0, 64, true);

        // In a real scenario, we would draw text and data here
        // For now, show activity with pixels
        uint8_t x = (sample_count_ * 10) % 120;
        for (uint8_t i = 0; i < 5; i++) {
            display_.setPixel(x + i, 10 + i, true);
        }

        // Send to display
        display_.display();
    }
};

/**
 * @brief Main example demonstrating sensor integration
 */
int main() {
    cout << "╔════════════════════════════════════════════════╗" << endl;
    cout << "║     libemb Example 04: Sensor Integration     ║" << endl;
    cout << "║  Multi-Component Demonstration with Mocks     ║" << endl;
    cout << "╚════════════════════════════════════════════════╝" << endl << endl;

    // Create mock objects
    auto i2c = std::make_unique<libemb::port::mock::MockI2c>();
    auto button = std::make_unique<libemb::port::mock::MockGpio>();

    // Create monitor
    auto monitor = std::make_unique<EnvironmentalMonitor>(i2c.get(), button.get());

    // Initialize
    if (!monitor->initialize()) {
        cout << "Failed to initialize monitor" << endl;
        return 1;
    }

    // Simulate 10 measurement cycles
    cout << "Collecting sensor data..." << endl << endl;
    for (int i = 0; i < 10; i++) {
        monitor->measure();

        // Simulate button press on cycle 5
        if (i == 4) {
            button->write(libemb::hal::GpioState::LOW);
        } else {
            button->write(libemb::hal::GpioState::HIGH);
        }
    }

    // Summary
    cout << endl << "=== Collection Complete ===" << endl;
    cout << "Samples collected: " << monitor->sample_count() << endl;
    cout << "Buffer usage: " << monitor->buffer_size() << " bytes" << endl;
    cout << "I2C transactions: " << i2c->transactions().size() << endl;
    cout << "Button events: " << button->history().size() << endl;

    // Verify results
    assert(monitor->sample_count() == 10);
    assert(monitor->buffer_size() > 0);
    assert(i2c->transactions().size() > 0);

    cout << endl << "✓ All components working correctly!" << endl;

    return 0;
}
