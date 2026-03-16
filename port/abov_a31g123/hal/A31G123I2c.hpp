/**
 * @file A31G123I2c.hpp
 * @brief ABOV A31G123 I2C HAL Implementation
 */

#pragma once

#include "libemb/hal/II2c.hpp"

namespace libemb::port::abov_a31g123 {

/**
 * @class A31G123I2c
 * @brief I2C Master implementation for ABOV A31G123
 *
 * Supports Standard Mode (100 kHz) and Fast Mode (400 kHz).
 * Master-only, blocking operations with timeout protection.
 */
class A31G123I2c : public libemb::hal::II2c {
public:
    /**
     * @brief Construct I2C instance
     * @param i2c_port I2C port number (0, 1, etc.)
     * @param clock_freq Clock frequency in Hz (100000 or 400000)
     */
    A31G123I2c(uint8_t i2c_port, uint32_t clock_freq = 100000);

    /**
     * @brief Write data to I2C slave
     * @param slaveAddr 7-bit slave address
     * @param data Data to transmit
     * @param length Number of bytes
     * @return Status of operation
     */
    libemb::hal::I2cStatus write(uint8_t slaveAddr, const uint8_t* data, size_t length) override;

    /**
     * @brief Read data from I2C slave
     * @param slaveAddr 7-bit slave address
     * @param data Buffer for received data
     * @param length Number of bytes to read
     * @return Status of operation
     */
    libemb::hal::I2cStatus read(uint8_t slaveAddr, uint8_t* data, size_t length) override;

    /**
     * @brief Write then read (for register-based slaves)
     * @param slaveAddr 7-bit slave address
     * @param writeData Register address or command
     * @param writeLength Number of bytes to write
     * @param readData Buffer for read data
     * @param readLength Number of bytes to read
     * @return Status of operation
     */
    libemb::hal::I2cStatus writeRead(uint8_t slaveAddr,
                                      const uint8_t* writeData, size_t writeLength,
                                      uint8_t* readData, size_t readLength) override;

private:
    uint8_t i2c_port_;
    uint32_t clock_freq_;
    volatile uint32_t* i2c_base_;

    void initialize();
    void configure_frequency();
    libemb::hal::I2cStatus wait_for_complete(uint32_t timeout);
};

}  // namespace libemb::port::abov_a31g123
