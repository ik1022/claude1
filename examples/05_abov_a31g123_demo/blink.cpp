/**
 * @file blink.cpp
 * @brief LED Blink Example for ABOV A31G123
 *
 * Demonstrates basic GPIO output by toggling an LED at 1 Hz.
 *
 * Hardware:
 * - LED connected to PA5 (with current-limiting resistor)
 * - External 10 MHz crystal oscillator (configured for 70 MHz PLL)
 *
 * Expected behavior:
 * - LED toggles every 1 second
 * - Infinite loop (software crash = LED stops blinking)
 */

#include "port/abov_a31g123/hal/A31G123Gpio.hpp"
#include "port/abov_a31g123/common/A31G123Config.hpp"

using namespace libemb::port::abov_a31g123;

/* ============================================================================
   Hardware Configuration
   ============================================================================ */

// GPIO port base addresses (MCU-specific)
#define GPIOA_BASE  0x40040000UL
#define GPIOB_BASE  0x40044000UL
#define GPIOC_BASE  0x40048000UL

/**
 * @brief Delay function using simple loop
 *
 * Busy-waits for specified milliseconds.
 * Accuracy depends on compiler optimization and CPU frequency.
 *
 * @param ms Milliseconds to delay
 */
void delay_ms(uint32_t ms) {
    // Approximate cycles per millisecond at 70 MHz
    // 70,000,000 cycles/sec ÷ 1,000 ms = 70,000 cycles/ms
    // Each loop iteration: ~3 cycles (depends on optimization)
    const uint32_t cycles_per_ms = (SYSTEM_CLOCK_FREQ / 1000) / 3;

    for (uint32_t i = 0; i < ms; i++) {
        for (volatile uint32_t j = 0; j < cycles_per_ms; j++) {
            __asm__("nop");  // No operation (prevent optimization)
        }
    }
}

/**
 * @brief Initialize MCU (placeholder)
 *
 * In production, this would:
 * - Configure PLL and system clock
 * - Set up peripheral clocks
 * - Initialize flash wait states
 */
void mcu_init() {
    // Call system clock initialization
    // initialize_system_clock();  // TODO: Implement after register definitions
}

/**
 * @brief Main program entry point
 *
 * Blinks LED on PA5 indefinitely.
 */
int main() {
    // Initialize MCU system clock
    mcu_init();

    // Create GPIO instance for PA5 (LED pin)
    // Port base: 0x40040000 (GPIOA)
    // Pin: 5
    A31G123Gpio led_gpio(reinterpret_cast<volatile uint32_t*>(GPIOA_BASE), 5);

    // Configure PA5 as output (push-pull)
    led_gpio.setMode(libemb::hal::GpioMode::OUTPUT);

    // Main loop: blink LED
    while (1) {
        // Turn LED on (write HIGH)
        led_gpio.write(libemb::hal::GpioState::HIGH);

        // Wait 500 ms
        delay_ms(500);

        // Turn LED off (write LOW)
        led_gpio.write(libemb::hal::GpioState::LOW);

        // Wait 500 ms
        delay_ms(500);

        // Result: 1 second period LED blink
    }

    return 0;  // Never reached
}

/* ============================================================================
   Cortex-M0+ Reset Handler (Minimal)
   ============================================================================ */

/**
 * @brief Default interrupt handler (weak symbol)
 *
 * Called for any unhandled interrupt.
 * Prevents system crash if interrupt is not properly handled.
 */
extern "C" void Default_Handler() {
    // Hang forever (infinite loop indicates error)
    while (1) {
        __asm__("wfi");  // Wait for interrupt (power saving)
    }
}

/**
 * @brief Reset handler stub (weak symbol)
 *
 * Called at startup. Normally defined in startup code.
 * This minimal version avoids linker errors.
 */
extern "C" __attribute__((weak)) void Reset_Handler() {
    // Call main()
    main();

    // If main() returns, hang
    while (1);
}
