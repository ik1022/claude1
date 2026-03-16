/**
 * @file A31G123Uart.hpp
 * @brief ABOV A31G123 UART HAL Implementation
 */

#pragma once

#include "libemb/hal/IUart.hpp"
#include "libemb/util/RingBuffer.hpp"

namespace libemb::port::abov_a31g123 {

/**
 * @class A31G123Uart
 * @brief UART implementation for ABOV A31G123
 *
 * Non-blocking serial communication with internal buffering.
 * Uses interrupt-driven receive with RingBuffer.
 */
class A31G123Uart : public libemb::hal::IUart {
public:
    /**
     * @brief Construct UART instance
     * @param uart_port UART port number (0, 1, 2, etc.)
     * @param baudrate Baud rate (default 115200)
     */
    A31G123Uart(uint8_t uart_port, uint32_t baudrate = 115200);

    /**
     * @brief Transmit single byte
     * @param byte Byte to transmit
     */
    void putChar(uint8_t byte) override;

    /**
     * @brief Receive single byte
     * @return Received byte, or 0 if no data available
     */
    uint8_t getChar() override;

    /**
     * @brief Check if data is available
     * @return true if bytes available in receive buffer
     */
    bool isDataAvailable() const override;

    /**
     * @brief Transmit multiple bytes
     * @param data Buffer of bytes to transmit
     * @param length Number of bytes to transmit
     */
    void write(const uint8_t* data, size_t length) override;

    /**
     * @brief Receive multiple bytes
     * @param data Buffer to store received bytes
     * @param maxLength Maximum number of bytes to receive
     * @return Number of bytes actually received
     */
    size_t read(uint8_t* data, size_t maxLength) override;

private:
    uint8_t uart_port_;
    uint32_t baudrate_;
    volatile uint32_t* uart_base_;
    libemb::util::RingBuffer<uint8_t, 256> rx_buffer_;

    void initialize();
    void configure_baudrate();
};

}  // namespace libemb::port::abov_a31g123
