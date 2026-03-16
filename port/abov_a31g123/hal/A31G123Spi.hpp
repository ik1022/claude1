/**
 * @file A31G123Spi.hpp
 * @brief ABOV A31G123 SPI HAL Implementation
 */

#pragma once

#include "libemb/hal/ISpi.hpp"

namespace libemb::port::abov_a31g123 {

/**
 * @class A31G123Spi
 * @brief SPI Master implementation for ABOV A31G123
 *
 * Full-duplex SPI communication with configurable clock and phase.
 * Master-only, polling-based operation (no DMA).
 */
class A31G123Spi : public libemb::hal::ISpi {
public:
    /**
     * @brief SPI Mode definition
     *
     * CPOL=0, CPHA=0: Mode 0
     * CPOL=0, CPHA=1: Mode 1
     * CPOL=1, CPHA=0: Mode 2
     * CPOL=1, CPHA=1: Mode 3
     */
    enum class Mode : uint8_t {
        MODE_0 = 0,  ///< CPOL=0, CPHA=0
        MODE_1 = 1,  ///< CPOL=0, CPHA=1
        MODE_2 = 2,  ///< CPOL=1, CPHA=0
        MODE_3 = 3   ///< CPOL=1, CPHA=1
    };

    /**
     * @brief Construct SPI instance
     * @param spi_port SPI port number (0, 1, etc.)
     * @param clock_freq Clock frequency in Hz
     * @param mode SPI mode (0, 1, 2, or 3)
     */
    A31G123Spi(uint8_t spi_port, uint32_t clock_freq = 1000000, Mode mode = Mode::MODE_0);

    /**
     * @brief Exchange data over SPI (simultaneous read/write)
     * @param tx_data Data to transmit (or nullptr)
     * @param rx_data Buffer for received data (or nullptr)
     * @param length Number of bytes to exchange
     * @return true if successful, false on timeout
     */
    bool exchange(const uint8_t* tx_data, uint8_t* rx_data, size_t length);

    /**
     * @brief Transmit data only (dummy read)
     * @param data Data to transmit
     * @param length Number of bytes
     * @return true if successful
     */
    bool transmit_only(const uint8_t* data, size_t length);

    /**
     * @brief Receive data only (transmit zeros)
     * @param rx_data Buffer for received data
     * @param length Number of bytes
     * @return true if successful
     */
    bool receive_only(uint8_t* rx_data, size_t length);

private:
    uint8_t spi_port_;
    uint32_t clock_freq_;
    Mode mode_;
    volatile uint32_t* spi_base_;

    void initialize();
    void configure_frequency();
    void configure_mode();
};

}  // namespace libemb::port::abov_a31g123
