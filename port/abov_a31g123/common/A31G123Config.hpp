/**
 * @file A31G123Config.hpp
 * @brief ABOV A31G123 Configuration Constants
 *
 * Default configuration values for ABOV A31G123 MCU.
 * These can be overridden by project-specific CMake options.
 */

#pragma once

namespace libemb::port::abov_a31g123 {

/* ============================================================================
   Clock Configuration
   ============================================================================ */

/**
 * @brief Main oscillator frequency (Hz)
 *
 * Typical values:
 * - 10 MHz (external crystal, common)
 * - 4 MHz (internal RC)
 */
constexpr uint32_t EXTERNAL_OSC_FREQ = 10000000UL;  // 10 MHz

/**
 * @brief PLL target output frequency (Hz)
 *
 * Common configurations:
 * - 70 MHz (full speed, 10MHz XTAL × 7)
 * - 60 MHz (power saving)
 * - 30 MHz (half speed)
 */
constexpr uint32_t PLL_OUTPUT_FREQ = 70000000UL;  // 70 MHz

/**
 * @brief System clock frequency (Hz)
 *
 * After clock dividers, the actual system clock.
 */
constexpr uint32_t SYSTEM_CLOCK_FREQ = 70000000UL;  // 70 MHz

/**
 * @brief AHB clock frequency (Hz)
 *
 * Advanced High-speed Bus frequency (may differ from system clock).
 */
constexpr uint32_t AHB_CLOCK_FREQ = 70000000UL;  // 70 MHz

/**
 * @brief APB clock frequency (Hz)
 *
 * Advanced Peripheral Bus frequency.
 */
constexpr uint32_t APB_CLOCK_FREQ = 35000000UL;  // 35 MHz (divided by 2)

/* ============================================================================
   UART Configuration
   ============================================================================ */

/**
 * @brief Default UART baud rate (bits per second)
 */
constexpr uint32_t DEFAULT_UART_BAUDRATE = 115200;

/**
 * @brief UART data bits (7 or 8)
 */
constexpr uint8_t DEFAULT_UART_DATA_BITS = 8;

/**
 * @brief UART stop bits (1 or 2)
 */
constexpr uint8_t DEFAULT_UART_STOP_BITS = 1;

/**
 * @brief UART parity ('N' = None, 'E' = Even, 'O' = Odd)
 */
constexpr char DEFAULT_UART_PARITY = 'N';

/* ============================================================================
   I2C Configuration
   ============================================================================ */

/**
 * @brief Default I2C clock frequency (Hz)
 *
 * Standard modes:
 * - 100000 (Standard mode, 100 kHz)
 * - 400000 (Fast mode, 400 kHz)
 */
constexpr uint32_t DEFAULT_I2C_CLOCK = 100000UL;  // 100 kHz

/**
 * @brief I2C timeout (system clock cycles)
 *
 * Prevents bus hangup if slave doesn't respond.
 */
constexpr uint32_t I2C_TIMEOUT_CYCLES = 1000000UL;  // ~14 ms at 70 MHz

/* ============================================================================
   SPI Configuration
   ============================================================================ */

/**
 * @brief Default SPI clock frequency (Hz)
 *
 * Must be <= SYSTEM_CLOCK_FREQ / 2.
 */
constexpr uint32_t DEFAULT_SPI_CLOCK = 1000000UL;  // 1 MHz

/**
 * @brief Default SPI mode (0, 1, 2, or 3)
 *
 * Mode 0: CPOL=0, CPHA=0
 * Mode 1: CPOL=0, CPHA=1
 * Mode 2: CPOL=1, CPHA=0
 * Mode 3: CPOL=1, CPHA=1
 */
constexpr uint8_t DEFAULT_SPI_MODE = 0;

/* ============================================================================
   Memory Configuration
   ============================================================================ */

/**
 * @brief Total Flash memory (bytes)
 */
constexpr uint32_t FLASH_SIZE = 128 * 1024;  // 128 KB

/**
 * @brief Total RAM size (bytes)
 */
constexpr uint32_t RAM_SIZE = 32 * 1024;  // 32 KB

/**
 * @brief EEPROM size (bytes) - varies by variant
 */
constexpr uint32_t EEPROM_SIZE = 4 * 1024;  // 4 KB (typical)

/* ============================================================================
   Interrupt Configuration
   ============================================================================ */

/**
 * @brief Number of interrupt vectors in vector table
 *
 * ARM Cortex-M0+: 16 core exceptions + device-specific
 * A31G123: 16 + 32 = 48 total
 */
constexpr uint32_t NUM_INTERRUPTS = 48;

/* ============================================================================
   Debug Configuration
   ============================================================================ */

/**
 * @brief Enable SWD (Serial Wire Debug) interface
 *
 * If true, PA13 (SWDIO) and PA14 (SWDCK) are reserved for debugger.
 */
constexpr bool ENABLE_SWD = true;

/**
 * @brief Enable SWO (Serial Wire Output) for profiling
 */
constexpr bool ENABLE_SWO = false;

/* ============================================================================
   Utility Functions
   ============================================================================ */

/**
 * @brief Calculate UART baud rate divisor for given baud and clock
 * @param baud_rate Target baud rate (e.g., 115200)
 * @param clock Peripheral clock frequency (Hz)
 * @return Divisor for UART baud rate register
 *
 * @note For ABOV A31G123, UART clock is APB_CLOCK_FREQ (35 MHz)
 */
constexpr uint16_t calculate_uart_divisor(uint32_t baud_rate, uint32_t clock) {
    return (clock / baud_rate + 1) / 2;  // Simplified calculation
}

/**
 * @brief Calculate I2C SCL frequency divisor
 * @param i2c_freq Desired I2C frequency (Hz, 100000 or 400000)
 * @param clock I2C clock frequency (Hz)
 * @return Divisor for I2C timing registers
 */
constexpr uint8_t calculate_i2c_divisor(uint32_t i2c_freq, uint32_t clock) {
    return clock / (i2c_freq * 2);
}

/**
 * @brief Calculate SPI clock divisor
 * @param spi_freq Desired SPI frequency (Hz)
 * @param clock System clock frequency (Hz)
 * @return Divisor for SPI clock register (usually power of 2)
 */
constexpr uint8_t calculate_spi_divisor(uint32_t spi_freq, uint32_t clock) {
    uint8_t div = 1;
    while ((clock / div) > spi_freq && div < 256) {
        div *= 2;
    }
    return div;
}

}  // namespace libemb::port::abov_a31g123
