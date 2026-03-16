/**
 * @file A31G123Uart.cpp
 * @brief UART implementation for ABOV A31G123
 */

#include "A31G123Uart.hpp"

namespace libemb::port::abov_a31g123 {

A31G123Uart::A31G123Uart(uint8_t uart_port, uint32_t baudrate)
    : uart_port_(uart_port), baudrate_(baudrate), uart_base_(nullptr) {
    initialize();
}

void A31G123Uart::initialize() {
    // TODO: Configure UART peripheral
    // - Set GPIO pins for TX/RX
    // - Configure baud rate
    // - Enable interrupts for RX
}

void A31G123Uart::configure_baudrate() {
    // TODO: Calculate and set baud rate divisor
}

void A31G123Uart::putChar(uint8_t byte) {
    // TODO: Transmit single byte
    (void)byte;
}

uint8_t A31G123Uart::getChar() {
    // TODO: Receive single byte from buffer
    return 0;
}

bool A31G123Uart::isDataAvailable() const {
    return !rx_buffer_.empty();
}

void A31G123Uart::write(const uint8_t* data, size_t length) {
    // TODO: Transmit multiple bytes
    (void)data;
    (void)length;
}

size_t A31G123Uart::read(uint8_t* data, size_t maxLength) {
    // TODO: Receive multiple bytes
    size_t count = 0;
    while (count < maxLength && !rx_buffer_.empty()) {
        rx_buffer_.pop(data[count]);
        count++;
    }
    return count;
}

}  // namespace libemb::port::abov_a31g123
