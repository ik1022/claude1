/**
 * @file A31G123Uart.cpp
 * @brief UART implementation for ABOV A31G123
 */

#include "A31G123Uart.hpp"
#include "../common/A31G123Registers.hpp"
#include "../common/A31G123Config.hpp"

namespace libemb::port::abov_a31g123 {

// Global UART instances for interrupt handlers
static A31G123Uart* uart_instances[4] = {nullptr, nullptr, nullptr, nullptr};

A31G123Uart::A31G123Uart(uint8_t uart_port, uint32_t baudrate)
    : uart_port_(uart_port), baudrate_(baudrate), uart_base_(nullptr) {
    // Store instance for interrupt handler
    if (uart_port < 4) {
        uart_instances[uart_port] = this;
    }

    initialize();
}

void A31G123Uart::initialize() {
    // Get UART base address
    switch (uart_port_) {
        case 0:
            uart_base_ = reinterpret_cast<volatile uint32_t*>(UART0_BASE);
            break;
        case 1:
            uart_base_ = reinterpret_cast<volatile uint32_t*>(UART1_BASE);
            break;
        case 2:
            uart_base_ = reinterpret_cast<volatile uint32_t*>(UART2_BASE);
            break;
        case 3:
            uart_base_ = reinterpret_cast<volatile uint32_t*>(UART3_BASE);
            break;
        default:
            return;  // Invalid UART port
    }

    UART_Type* uart = reinterpret_cast<UART_Type*>(uart_base_);

    // Configure baud rate
    configure_baudrate();

    // Enable UART with TX and RX enabled
    uart->CTRL = UART_CTRL_TXEN | UART_CTRL_RXEN;

    // Enable RX interrupt
    uart->IE = 0x02;  // RXNE interrupt enable

    // Note: GPIO configuration (TX/RX pins) would be done here
    // Requires GPIO register access based on UART port
}

void A31G123Uart::configure_baudrate() {
    UART_Type* uart = reinterpret_cast<UART_Type*>(uart_base_);

    // Calculate baud rate divisor
    // Baud rate formula: BaudRate = APB_CLOCK / (2 * Divisor)
    // So: Divisor = APB_CLOCK / (2 * BaudRate)
    uint16_t divisor = calculate_uart_divisor(baudrate_, APB_CLOCK_FREQ);

    // Write to baud rate register
    uart->BAUD = divisor;
}

void A31G123Uart::putChar(uint8_t byte) {
    UART_Type* uart = reinterpret_cast<UART_Type*>(uart_base_);

    // Wait until TX is empty
    while (!(uart->STAT & UART_STAT_TXE)) {
        // Busy-wait (blocking)
    }

    // Write data to TX register
    uart->DATA = byte;
}

uint8_t A31G123Uart::getChar() {
    // Check if data is available in RX buffer
    if (rx_buffer_.empty()) {
        return 0;  // No data available
    }

    // Pop from buffer
    uint8_t byte = 0;
    rx_buffer_.pop(byte);
    return byte;
}

bool A31G123Uart::isDataAvailable() const {
    return !rx_buffer_.empty();
}

void A31G123Uart::write(const uint8_t* data, size_t length) {
    for (size_t i = 0; i < length; i++) {
        putChar(data[i]);
    }
}

size_t A31G123Uart::read(uint8_t* data, size_t maxLength) {
    size_t count = 0;
    while (count < maxLength && !rx_buffer_.empty()) {
        data[count] = getChar();
        count++;
    }
    return count;
}

/**
 * @brief UART interrupt handler
 *
 * Called when UART RX data is available.
 * Pushes received byte into RingBuffer.
 */
void uart_rx_handler(uint8_t uart_port) {
    if (uart_port >= 4) return;

    A31G123Uart* uart = uart_instances[uart_port];
    if (!uart) return;

    UART_Type* hw_uart = reinterpret_cast<UART_Type*>(uart->uart_base_);

    // Check if RX not empty
    if (hw_uart->STAT & UART_STAT_RXNE) {
        // Read byte from RX register
        uint8_t byte = (uint8_t)(hw_uart->DATA & 0xFF);

        // Push to RingBuffer (drops if full)
        uart->rx_buffer_.push(byte);
    }
}

/**
 * @brief UART0 interrupt handler
 *
 * Called on UART0 interrupt (RX data available).
 */
extern "C" __attribute__((weak)) void UART0_IRQHandler() {
    uart_rx_handler(0);
}

/**
 * @brief UART1 interrupt handler
 */
extern "C" __attribute__((weak)) void UART1_IRQHandler() {
    uart_rx_handler(1);
}

/**
 * @brief UART2 interrupt handler
 */
extern "C" __attribute__((weak)) void UART2_IRQHandler() {
    uart_rx_handler(2);
}

/**
 * @brief UART3 interrupt handler
 */
extern "C" __attribute__((weak)) void UART3_IRQHandler() {
    uart_rx_handler(3);
}

}  // namespace libemb::port::abov_a31g123
