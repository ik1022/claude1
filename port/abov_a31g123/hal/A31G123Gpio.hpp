/**
 * @file A31G123Gpio.hpp
 * @brief ABOV A31G123 GPIO HAL Implementation
 *
 * Provides GPIO abstraction for ABOV A31G123 MCU.
 * Implements libemb::hal::IGpio interface for digital input/output operations.
 */

#pragma once

#include "libemb/hal/IGpio.hpp"

namespace libemb::port::abov_a31g123 {

/**
 * @class A31G123Gpio
 * @brief GPIO implementation for ABOV A31G123
 *
 * Maps to physical GPIO pins on ABOV A31G123 MCU.
 * Supports modes: INPUT, OUTPUT, INPUT_PULLUP, INPUT_PULLDOWN, ANALOG
 *
 * @note Each GPIO instance manages a single pin.
 * @note Pin numbering: 0-31 per port (PA0-PA31, PB0-PB31, etc.)
 *
 * @example
 * @code
 * // Create GPIO for PA5 as output (LED pin)
 * A31G123Gpio led_pin(GPIOA_BASE, 5);
 * led_pin.setMode(GpioMode::OUTPUT);
 * led_pin.write(GpioState::HIGH);   // Turn on LED
 * @endcode
 */
class A31G123Gpio : public libemb::hal::IGpio {
public:
    /**
     * @brief Construct GPIO instance for a specific pin
     *
     * @param port Port base address (e.g., 0x40040000 for GPIOA)
     * @param pin  Pin number on port (0-31)
     *
     * @note Port addresses:
     *       - GPIOA: 0x40040000
     *       - GPIOB: 0x40044000
     *       - GPIOC: 0x40048000
     *       - GPIOD: 0x4004C000
     *       - GPIOE: 0x40050000
     */
    A31G123Gpio(volatile uint32_t* port_base, uint8_t pin);

    /**
     * @brief Set GPIO mode (input, output, analog, etc.)
     * @param mode Mode to configure
     *
     * Mode behaviors:
     * - INPUT:        Digital input, no pull resistor
     * - OUTPUT:       Push-pull output, ~25mA capability
     * - INPUT_PULLUP: Input with internal pull-up enabled
     * - INPUT_PULLDOWN: Input with internal pull-down enabled
     * - ANALOG:       ADC/comparator input, GPIO disabled
     */
    void setMode(libemb::hal::GpioMode mode) override;

    /**
     * @brief Write GPIO state (for output pins)
     * @param state HIGH or LOW
     *
     * @note Has no effect for input pins
     */
    void write(libemb::hal::GpioState state) override;

    /**
     * @brief Read GPIO state (for input pins)
     * @return Current pin state (HIGH or LOW)
     *
     * Reads actual pin level regardless of pin direction.
     */
    libemb::hal::GpioState read() const override;

    /**
     * @brief Toggle GPIO output state
     *
     * @note Only effective for output pins
     * @note Faster than read() + write()
     */
    void toggle() override;

    /**
     * @brief Get current GPIO mode
     * @return Current mode setting
     */
    libemb::hal::GpioMode getMode() const { return mode_; }

    /**
     * @brief Get GPIO port base address
     * @return Base address of GPIO port register
     */
    volatile uint32_t* getPort() const { return port_; }

    /**
     * @brief Get pin number on port
     * @return Pin number (0-31)
     */
    uint8_t getPin() const { return pin_; }

private:
    volatile uint32_t* port_;           ///< GPIO port base address
    uint8_t pin_;                       ///< Pin number (0-31)
    libemb::hal::GpioMode mode_;       ///< Current GPIO mode

    /**
     * @brief Get pin mask for register operations
     * @return Bit mask for pin (1 << pin_)
     */
    uint32_t pin_mask() const {
        return 1UL << pin_;
    }

    /**
     * @brief Set GPIO mode in hardware registers
     * @param mode GPIO mode to apply
     */
    void apply_mode(libemb::hal::GpioMode mode);

    /**
     * @brief Configure pull resistor (internal hardware feature)
     * @param pull_up true for pull-up, false for pull-down
     */
    void configure_pull(bool pull_up);

    /**
     * @brief Enable/disable analog mode
     * @param enable true to enable analog, false for digital
     */
    void set_analog_mode(bool enable);
};

}  // namespace libemb::port::abov_a31g123
