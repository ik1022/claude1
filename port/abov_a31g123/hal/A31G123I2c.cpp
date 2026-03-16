/**
 * @file A31G123I2c.cpp
 * @brief I2C implementation for ABOV A31G123
 */

#include "A31G123I2c.hpp"
#include "../common/A31G123Registers.hpp"
#include "../common/A31G123Config.hpp"

namespace libemb::port::abov_a31g123 {

A31G123I2c::A31G123I2c(uint8_t i2c_port, uint32_t clock_freq)
    : i2c_port_(i2c_port), clock_freq_(clock_freq), i2c_base_(nullptr) {
    initialize();
}

void A31G123I2c::initialize() {
    // Get I2C base address
    switch (i2c_port_) {
        case 0:
            i2c_base_ = reinterpret_cast<volatile uint32_t*>(I2C0_BASE);
            break;
        case 1:
            i2c_base_ = reinterpret_cast<volatile uint32_t*>(I2C1_BASE);
            break;
        default:
            return;  // Invalid I2C port
    }

    I2C_Type* i2c = reinterpret_cast<I2C_Type*>(i2c_base_);

    // Configure clock frequency
    configure_frequency();

    // Enable I2C interface
    i2c->CTRL = I2C_CTRL_EN;

    // Note: GPIO configuration (SDA/SCL pins) would be done here
    // Requires GPIO register access and alternate function setup
}

void A31G123I2c::configure_frequency() {
    I2C_Type* i2c = reinterpret_cast<I2C_Type*>(i2c_base_);

    // Calculate timing for I2C clock
    // For simplicity, use standard timing:
    // 100 kHz: SCLL = SCLH = AHB_CLOCK / (2 * 100kHz)
    // 400 kHz: SCLL = SCLH = AHB_CLOCK / (2 * 400kHz)

    uint32_t divisor = AHB_CLOCK_FREQ / (2 * clock_freq_);

    // Set SCL timing (symmetric: equal low and high time)
    i2c->SCLL = divisor;
    i2c->SCLH = divisor;
}

libemb::hal::I2cStatus A31G123I2c::write(uint8_t slaveAddr, const uint8_t* data, size_t length) {
    I2C_Type* i2c = reinterpret_cast<I2C_Type*>(i2c_base_);

    // Check if bus is busy
    if (i2c->STAT & I2C_STAT_BUSY) {
        return libemb::hal::I2cStatus::BUSY;
    }

    // Generate START condition
    i2c->CTRL |= I2C_CTRL_START;
    libemb::hal::I2cStatus status = wait_for_complete(I2C_TIMEOUT_CYCLES);
    if (status != libemb::hal::I2cStatus::OK) {
        return status;
    }

    // Send slave address with WRITE bit (0)
    i2c->ADDR = (slaveAddr << 1) | 0;  // Write operation
    status = wait_for_complete(I2C_TIMEOUT_CYCLES);
    if (status != libemb::hal::I2cStatus::OK) {
        return status;
    }

    // Transmit data bytes
    for (size_t i = 0; i < length; i++) {
        i2c->DATA = data[i];
        status = wait_for_complete(I2C_TIMEOUT_CYCLES);
        if (status != libemb::hal::I2cStatus::OK) {
            break;
        }
    }

    // Generate STOP condition
    i2c->CTRL |= I2C_CTRL_STOP;
    return status;
}

libemb::hal::I2cStatus A31G123I2c::read(uint8_t slaveAddr, uint8_t* data, size_t length) {
    I2C_Type* i2c = reinterpret_cast<I2C_Type*>(i2c_base_);

    // Check if bus is busy
    if (i2c->STAT & I2C_STAT_BUSY) {
        return libemb::hal::I2cStatus::BUSY;
    }

    // Generate START condition
    i2c->CTRL |= I2C_CTRL_START;
    libemb::hal::I2cStatus status = wait_for_complete(I2C_TIMEOUT_CYCLES);
    if (status != libemb::hal::I2cStatus::OK) {
        return status;
    }

    // Send slave address with READ bit (1)
    i2c->ADDR = (slaveAddr << 1) | 1;  // Read operation
    status = wait_for_complete(I2C_TIMEOUT_CYCLES);
    if (status != libemb::hal::I2cStatus::OK) {
        return status;
    }

    // Receive data bytes
    for (size_t i = 0; i < length; i++) {
        // Send ACK for all bytes except the last one
        if (i < length - 1) {
            i2c->CTRL |= I2C_CTRL_ACK;
        } else {
            i2c->CTRL |= I2C_CTRL_NACK;
        }

        status = wait_for_complete(I2C_TIMEOUT_CYCLES);
        if (status != libemb::hal::I2cStatus::OK) {
            break;
        }

        // Read data
        data[i] = (uint8_t)(i2c->DATA & 0xFF);
    }

    // Generate STOP condition
    i2c->CTRL |= I2C_CTRL_STOP;
    return status;
}

libemb::hal::I2cStatus A31G123I2c::writeRead(uint8_t slaveAddr,
                                              const uint8_t* writeData, size_t writeLength,
                                              uint8_t* readData, size_t readLength) {
    // Perform write operation
    libemb::hal::I2cStatus status = write(slaveAddr, writeData, writeLength);
    if (status != libemb::hal::I2cStatus::OK) {
        return status;
    }

    // Perform read operation
    return read(slaveAddr, readData, readLength);
}

libemb::hal::I2cStatus A31G123I2c::wait_for_complete(uint32_t timeout) {
    I2C_Type* i2c = reinterpret_cast<I2C_Type*>(i2c_base_);

    // Wait for DONE flag or timeout
    while (timeout > 0) {
        if (i2c->STAT & I2C_STAT_DONE) {
            // Check for error conditions
            if (i2c->STAT & I2C_STAT_NACK) {
                return libemb::hal::I2cStatus::NACK;
            }
            return libemb::hal::I2cStatus::OK;
        }

        timeout--;
    }

    // Timeout occurred
    return libemb::hal::I2cStatus::TIMEOUT;
}

}  // namespace libemb::port::abov_a31g123
