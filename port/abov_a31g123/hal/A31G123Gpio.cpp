/**
 * @file A31G123Gpio.cpp
 * @brief GPIO implementation for ABOV A31G123
 */

#include "A31G123Gpio.hpp"
#include "../common/A31G123Registers.hpp"

namespace libemb::port::abov_a31g123 {

A31G123Gpio::A31G123Gpio(volatile uint32_t* port_base, uint8_t pin)
    : port_(port_base), pin_(pin), mode_(libemb::hal::GpioMode::INPUT) {
    // Default to input mode on initialization
    setMode(libemb::hal::GpioMode::INPUT);
}

void A31G123Gpio::setMode(libemb::hal::GpioMode mode) {
    mode_ = mode;
    apply_mode(mode);
}

void A31G123Gpio::write(libemb::hal::GpioState state) {
    // Cast to GPIO structure
    GPIO_Type* gpio = gpio_from_base(port_);

    // Write to DOUT register
    if (state == libemb::hal::GpioState::HIGH) {
        // Set bit
        gpio->DOUT |= pin_mask();
    } else {
        // Clear bit
        gpio->DOUT &= ~pin_mask();
    }
}

libemb::hal::GpioState A31G123Gpio::read() const {
    // Cast to GPIO structure
    GPIO_Type* gpio = gpio_from_base(port_);

    // Read from DIN register (input data)
    if (gpio->DIN & pin_mask()) {
        return libemb::hal::GpioState::HIGH;
    } else {
        return libemb::hal::GpioState::LOW;
    }
}

void A31G123Gpio::toggle() {
    // Cast to GPIO structure
    GPIO_Type* gpio = gpio_from_base(port_);

    // Write to TOGGLE register (XORs the bit)
    gpio->TOGGLE = pin_mask();
}

void A31G123Gpio::apply_mode(libemb::hal::GpioMode mode) {
    GPIO_Type* gpio = gpio_from_base(port_);

    // Clear existing mode bits for this pin (2 bits per pin)
    uint32_t shift = pin_ * 2;
    uint32_t mask = 0x03UL << shift;
    gpio->CTRL = (gpio->CTRL & ~mask) | ((uint32_t)mode << shift);

    // Configure pull resistors based on mode
    switch (mode) {
        case libemb::hal::GpioMode::INPUT:
            // Disable both pull-up and pull-down
            gpio->PUPU &= ~pin_mask();
            gpio->PDPU &= ~pin_mask();
            break;

        case libemb::hal::GpioMode::OUTPUT:
            // Disable pull resistors for output
            gpio->PUPU &= ~pin_mask();
            gpio->PDPU &= ~pin_mask();
            break;

        case libemb::hal::GpioMode::INPUT_PULLUP:
            // Enable pull-up
            gpio->PUPU |= pin_mask();
            gpio->PDPU &= ~pin_mask();
            break;

        case libemb::hal::GpioMode::INPUT_PULLDOWN:
            // Enable pull-down
            gpio->PUPU &= ~pin_mask();
            gpio->PDPU |= pin_mask();
            break;

        case libemb::hal::GpioMode::ANALOG:
            // Set analog mode
            set_analog_mode(true);
            break;
    }
}

void A31G123Gpio::configure_pull(bool pull_up) {
    GPIO_Type* gpio = gpio_from_base(port_);

    if (pull_up) {
        // Enable pull-up
        gpio->PUPU |= pin_mask();
        // Ensure pull-down is disabled
        gpio->PDPU &= ~pin_mask();
    } else {
        // Enable pull-down
        gpio->PDPU |= pin_mask();
        // Ensure pull-up is disabled
        gpio->PUPU &= ~pin_mask();
    }
}

void A31G123Gpio::set_analog_mode(bool enable) {
    GPIO_Type* gpio = gpio_from_base(port_);

    if (enable) {
        // Enable analog mode in ADCSEL
        gpio->ADCSEL |= pin_mask();
        // Set mode to ANALOG (0x02)
        uint32_t shift = pin_ * 2;
        uint32_t mask = 0x03UL << shift;
        gpio->CTRL = (gpio->CTRL & ~mask) | (0x02UL << shift);
    } else {
        // Disable analog mode
        gpio->ADCSEL &= ~pin_mask();
    }
}

}  // namespace libemb::port::abov_a31g123
