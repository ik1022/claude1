/**
 * @file i2c_display.cpp
 * @brief I2C Display Example for ABOV A31G123
 *
 * Demonstrates I2C communication by controlling an SSD1306 OLED display.
 *
 * Hardware:
 * - I2C0 on PA6 (SDA) and PA7 (SCL)
 * - SSD1306 OLED display (128×64 pixels)
 * - I2C slave address: 0x3C (typical for SSD1306)
 * - Pull-up resistors: 10k on SDA/SCL (usually on module)
 *
 * Expected behavior:
 * - Initialize I2C and display
 * - Draw patterns on OLED
 * - Update display periodically
 */

#include "port/abov_a31g123/hal/A31G123I2c.hpp"
#include "port/abov_a31g123/hal/A31G123Gpio.hpp"
#include "port/abov_a31g123/driver/Ssd1306.hpp"
#include "port/abov_a31g123/common/A31G123Config.hpp"

using namespace libemb::port::abov_a31g123;
using namespace libemb::driver;

/* ============================================================================
   Hardware Configuration
   ============================================================================ */

#define GPIOA_BASE  0x40040000UL

// I2C slave address (SSD1306 OLED)
constexpr uint8_t DISPLAY_ADDRESS = 0x3C;

/**
 * @brief Initialize MCU
 */
void mcu_init() {
    // initialize_system_clock();  // TODO: Implement
}

/**
 * @brief Delay function
 */
void delay_ms(uint32_t ms) {
    const uint32_t cycles_per_ms = (SYSTEM_CLOCK_FREQ / 1000) / 3;
    for (uint32_t i = 0; i < ms; i++) {
        for (volatile uint32_t j = 0; j < cycles_per_ms; j++) {
            __asm__("nop");
        }
    }
}

/**
 * @brief Main I2C display example
 */
int main() {
    // Initialize MCU
    mcu_init();

    // Create I2C0 instance (PA6=SDA, PA7=SCL, 100 kHz)
    A31G123I2c i2c(0, 100000);

    // Create SSD1306 display instance
    Ssd1306 display;

    // Initialize display via I2C
    if (!display.init(&i2c, DISPLAY_ADDRESS)) {
        // Display initialization failed - flash LED pattern to indicate error
        A31G123Gpio led(reinterpret_cast<volatile uint32_t*>(GPIOA_BASE), 5);
        led.setMode(libemb::hal::GpioMode::OUTPUT);

        while (1) {
            led.write(libemb::hal::GpioState::HIGH);
            delay_ms(100);
            led.write(libemb::hal::GpioState::LOW);
            delay_ms(100);
        }
        return -1;  // Never reached
    }

    // Clear display
    display.clear();
    display.display();

    // Draw welcome pattern
    // Top border
    for (uint8_t x = 0; x < 128; x++) {
        display.setPixel(x, 0, true);
        display.setPixel(x, 7, true);
    }

    // Left border
    for (uint8_t y = 0; y < 64; y++) {
        display.setPixel(0, y, true);
        display.setPixel(7, y, true);
    }

    // Right border
    for (uint8_t y = 0; y < 64; y++) {
        display.setPixel(120, y, true);
        display.setPixel(127, y, true);
    }

    // Center pattern (expanding square)
    for (uint8_t i = 0; i < 20; i++) {
        display.setPixel(64 - i, 32 - i, true);
        display.setPixel(64 + i, 32 - i, true);
        display.setPixel(64 - i, 32 + i, true);
        display.setPixel(64 + i, 32 + i, true);
    }

    // Update display
    display.display();

    // Animation loop
    uint16_t frame = 0;
    while (1) {
        // Update animation pattern
        frame++;

        // Draw moving line
        uint8_t y = (frame / 4) % 64;
        display.drawHLine(8, y, 112, true);

        // Update display
        display.display();

        // Delay for animation speed
        delay_ms(50);

        // Clear animation line before next frame
        display.drawHLine(8, y, 112, false);
    }

    return 0;  // Never reached
}

/* ============================================================================
   Minimal Exception Handlers
   ============================================================================ */

extern "C" void Default_Handler() {
    while (1) {
        __asm__("wfi");
    }
}

extern "C" __attribute__((weak)) void Reset_Handler() {
    main();
    while (1);
}
