/**
 * @file A31G123Spi.cpp
 * @brief SPI implementation for ABOV A31G123
 */

#include "A31G123Spi.hpp"
#include "../common/A31G123Registers.hpp"
#include "../common/A31G123Config.hpp"

namespace libemb::port::abov_a31g123 {

A31G123Spi::A31G123Spi(uint8_t spi_port, uint32_t clock_freq, Mode mode)
    : spi_port_(spi_port), clock_freq_(clock_freq), mode_(mode), spi_base_(nullptr) {
    initialize();
}

void A31G123Spi::initialize() {
    // Get SPI base address
    switch (spi_port_) {
        case 0:
            spi_base_ = reinterpret_cast<volatile uint32_t*>(SPI0_BASE);
            break;
        case 1:
            spi_base_ = reinterpret_cast<volatile uint32_t*>(SPI1_BASE);
            break;
        default:
            return;  // Invalid SPI port
    }

    SPI_Type* spi = reinterpret_cast<SPI_Type*>(spi_base_);

    // Configure clock frequency
    configure_frequency();

    // Configure SPI mode (CPOL, CPHA)
    configure_mode();

    // Enable SPI in master mode
    spi->CTRL = SPI_CTRL_EN | SPI_CTRL_MASTER;

    // Note: GPIO configuration (MOSI, MISO, SCK, CS pins) would be done here
    // Requires GPIO register access and alternate function setup
}

void A31G123Spi::configure_frequency() {
    SPI_Type* spi = reinterpret_cast<SPI_Type*>(spi_base_);

    // Calculate clock divisor
    // SPI clock = SYSTEM_CLOCK / (2 * (CLK + 1))
    // So: CLK = (SYSTEM_CLOCK / (2 * SPI_clock)) - 1

    uint32_t divisor = calculate_spi_divisor(clock_freq_, SYSTEM_CLOCK_FREQ);

    // Ensure divisor is valid (power of 2)
    spi->CLK = divisor;
}

void A31G123Spi::configure_mode() {
    SPI_Type* spi = reinterpret_cast<SPI_Type*>(spi_base_);

    // Clear CPOL and CPHA bits
    uint32_t ctrl = spi->CTRL & ~(SPI_CTRL_CPOL1 | SPI_CTRL_CPHA1);

    // Set mode bits based on selected mode
    switch (mode_) {
        case Mode::MODE_0:
            // CPOL=0, CPHA=0 (no change needed)
            break;
        case Mode::MODE_1:
            // CPOL=0, CPHA=1
            ctrl |= SPI_CTRL_CPHA1;
            break;
        case Mode::MODE_2:
            // CPOL=1, CPHA=0
            ctrl |= SPI_CTRL_CPOL1;
            break;
        case Mode::MODE_3:
            // CPOL=1, CPHA=1
            ctrl |= SPI_CTRL_CPOL1 | SPI_CTRL_CPHA1;
            break;
    }

    spi->CTRL = ctrl;
}

bool A31G123Spi::exchange(const uint8_t* tx_data, uint8_t* rx_data, size_t length) {
    SPI_Type* spi = reinterpret_cast<SPI_Type*>(spi_base_);

    for (size_t i = 0; i < length; i++) {
        // Wait until TX empty
        uint32_t timeout = 1000000;
        while (!(spi->STAT & SPI_STAT_TXE) && timeout > 0) {
            timeout--;
        }
        if (timeout == 0) return false;  // Timeout

        // Transmit byte
        spi->DATA = tx_data ? tx_data[i] : 0x00;

        // Wait until RX not empty
        timeout = 1000000;
        while (!(spi->STAT & SPI_STAT_RXNE) && timeout > 0) {
            timeout--;
        }
        if (timeout == 0) return false;  // Timeout

        // Receive byte
        if (rx_data) {
            rx_data[i] = (uint8_t)(spi->DATA & 0xFF);
        } else {
            // Dummy read to clear RX buffer
            (void)(spi->DATA);
        }
    }

    return true;
}

bool A31G123Spi::transmit_only(const uint8_t* data, size_t length) {
    // Transmit data, ignore received data
    return exchange(data, nullptr, length);
}

bool A31G123Spi::receive_only(uint8_t* rx_data, size_t length) {
    // Receive data, transmit zeros
    return exchange(nullptr, rx_data, length);
}

}  // namespace libemb::port::abov_a31g123
