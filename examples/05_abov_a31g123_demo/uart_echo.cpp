/**
 * @file uart_echo.cpp
 * @brief UART Echo Server Example for ABOV A31G123
 *
 * Demonstrates UART serial communication by echoing received characters.
 *
 * Hardware:
 * - UART0 on PA9 (TX) and PA10 (RX)
 * - USB UART adapter or serial cable connected to computer
 *
 * Terminal setup:
 * - Baud rate: 115200
 * - Data bits: 8
 * - Stop bits: 1
 * - Parity: None
 * - Flow control: None
 *
 * Expected behavior:
 * - Type characters in terminal
 * - Each character is echoed back
 * - Special handling:
 *   * '\r' (Enter) → Echo as "\r\n" (CRLF)
 *   * '\b' (Backspace) → Echo as "\b \b"
 */

#include "port/abov_a31g123/hal/A31G123Uart.hpp"
#include "port/abov_a31g123/hal/A31G123Gpio.hpp"
#include "port/abov_a31g123/common/A31G123Config.hpp"

using namespace libemb::port::abov_a31g123;

/* ============================================================================
   Hardware Configuration
   ============================================================================ */

#define GPIOA_BASE  0x40040000UL

/**
 * @brief Initialize MCU
 */
void mcu_init() {
    // Initialize clock
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
 * @brief Print string via UART
 */
void uart_puts(A31G123Uart& uart, const char* str) {
    while (*str) {
        uart.putChar(*str);
        str++;
    }
}

/**
 * @brief Main UART echo server
 */
int main() {
    // Initialize MCU
    mcu_init();

    // Create UART0 instance (PA9=TX, PA10=RX, 115200 baud)
    A31G123Uart uart0(0, 115200);

    // Print welcome message
    uart_puts(uart0, "\r\n");
    uart_puts(uart0, "╔════════════════════════════════════════════════╗\r\n");
    uart_puts(uart0, "║   ABOV A31G123 UART Echo Server               ║\r\n");
    uart_puts(uart0, "║   Baud: 115200, Data: 8N1                    ║\r\n");
    uart_puts(uart0, "║   Type characters to echo...                  ║\r\n");
    uart_puts(uart0, "╚════════════════════════════════════════════════╝\r\n");
    uart_puts(uart0, "\r\n> ");

    // Main echo loop
    while (1) {
        // Check if data is available
        if (uart0.isDataAvailable()) {
            // Receive character
            uint8_t byte = uart0.getChar();

            // Handle special cases
            if (byte == '\r') {
                // Enter key: echo as CRLF and print prompt
                uart_puts(uart0, "\r\n> ");
            }
            else if (byte == '\b' || byte == 0x7F) {
                // Backspace: erase character on terminal
                uart_puts(uart0, "\b \b");
            }
            else if (byte >= 0x20 && byte < 0x7F) {
                // Printable ASCII: echo normally
                uart0.putChar(byte);
            }
            else if (byte == '\n') {
                // Ignore standalone newline (handle \r\n)
                uart0.putChar('\n');
            }
            else {
                // Control character: show as dot
                uart0.putChar('.');
            }
        }

        // Optionally add small delay to prevent busy-waiting
        delay_ms(1);
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
