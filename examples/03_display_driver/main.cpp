/**
 * @file main.cpp
 * @brief Ssd1306 OLED Display Driver Example
 *
 * Demonstrates:
 * - HAL abstraction (using Mock I2C)
 * - Driver initialization and usage
 * - Pixel rendering and frame buffer operations
 */

#include <cstdio>
#include "libemb/driver/ssd1306/Ssd1306.hpp"
#include "port/mock/hal/MockI2c.hpp"

using namespace libemb::driver;
using namespace libemb::port::mock;

int main() {
    printf("=== Ssd1306 OLED Display Driver Example ===\n\n");

    // Create Mock I2C interface (simulates hardware)
    MockI2c mock_i2c;

    // Create Ssd1306 display controller
    Ssd1306 display;

    // Initialize the display with default I2C address
    printf("[1] Initializing display...\n");
    if (!display.init(&mock_i2c)) {
        printf("ERROR: Failed to initialize display!\n");
        return 1;
    }
    printf("    ✓ Display initialized successfully\n");

    // Check I2C transactions
    auto txns = mock_i2c.getTransactions();
    printf("    → I2C transactions: %zu\n", txns.size());
    if (!txns.empty()) {
        printf("    → First transaction to address: 0x%02X\n", txns[0].slaveAddr);
    }

    // Clear the display (black background)
    printf("\n[2] Clearing display...\n");
    display.clear();
    if (!display.display()) {
        printf("ERROR: Failed to update display!\n");
        return 1;
    }
    printf("    ✓ Display cleared\n");

    // Draw vertical lines (pattern)
    printf("\n[3] Drawing pattern...\n");
    for (int x = 0; x < 128; x += 4) {
        display.drawVLine(x, 0, 64, true);  // Vertical lines
    }
    printf("    ✓ Pattern drawn (vertical lines at x = 0, 4, 8, ...)\n");

    // Update display with new frame
    if (!display.display()) {
        printf("ERROR: Failed to update display!\n");
        return 1;
    }

    // Draw horizontal lines
    printf("\n[4] Drawing horizontal lines...\n");
    for (int y = 0; y < 64; y += 8) {
        display.drawHLine(0, y, 128, true);  // Horizontal lines
    }

    // Update display
    if (!display.display()) {
        printf("ERROR: Failed to update display!\n");
        return 1;
    }
    printf("    ✓ Horizontal lines drawn\n");

    // Test with custom I2C address
    printf("\n[5] Testing custom I2C address...\n");
    MockI2c mock_i2c_alt;
    Ssd1306 display_alt;
    uint8_t custom_addr = 0x3D;

    if (!display_alt.init(&mock_i2c_alt, custom_addr)) {
        printf("ERROR: Failed to initialize with custom address!\n");
        return 1;
    }
    printf("    ✓ Display initialized with address 0x%02X\n", custom_addr);

    // Show statistics
    printf("\n=== Display Statistics ===\n");
    txns = mock_i2c.getTransactions();
    printf("Total I2C transactions: %zu\n", txns.size());
    printf("Display dimensions: 128x64 pixels\n");
    printf("Driver class: Ssd1306\n");
    printf("Communication protocol: I2C\n");
    printf("Frame buffer updates: 3\n");

    printf("\n✓ Example completed successfully!\n");

    return 0;
}
