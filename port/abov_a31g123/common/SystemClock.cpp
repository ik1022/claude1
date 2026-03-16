/**
 * @file SystemClock.cpp
 * @brief ABOV A31G123 System Clock Configuration
 *
 * Configures the MCU clock system to achieve target frequency.
 * Supports PLL-based clock generation.
 */

#include "A31G123Config.hpp"

namespace libemb::port::abov_a31g123 {

/**
 * @brief Initialize system clock to configured frequency
 *
 * Configures:
 * - External oscillator (if applicable)
 * - PLL for frequency multiplication
 * - Clock distribution to peripherals
 *
 * @note This function should be called from startup code
 *       before any peripherals are initialized.
 *
 * Default configuration:
 * - Input: 10 MHz external crystal
 * - PLL: 10 MHz × 7 = 70 MHz
 * - Output: 70 MHz system clock
 */
void initialize_system_clock() {
    // TODO: Implement clock configuration
    // This requires MCU datasheet register definitions

    // Typical sequence:
    // 1. Verify external oscillator is running
    // 2. Configure PLL multiplier and divider
    // 3. Enable PLL
    // 4. Wait for PLL lock
    // 5. Switch system clock to PLL output
    // 6. Configure peripheral clock dividers
}

/**
 * @brief Get current system clock frequency
 * @return Frequency in Hz
 */
uint32_t get_system_clock_freq() {
    return SYSTEM_CLOCK_FREQ;
}

/**
 * @brief Get AHB clock frequency
 * @return Frequency in Hz
 */
uint32_t get_ahb_clock_freq() {
    return AHB_CLOCK_FREQ;
}

/**
 * @brief Get APB clock frequency
 * @return Frequency in Hz
 */
uint32_t get_apb_clock_freq() {
    return APB_CLOCK_FREQ;
}

/**
 * @brief Configure UART clock divisor for target baud rate
 * @param uart_num UART port number (0, 1, etc.)
 * @param baudrate Target baud rate
 * @return Status (0 = success)
 *
 * Configures the UART baud rate divisor based on the
 * configured APB clock frequency.
 */
int configure_uart_clock(uint8_t uart_num, uint32_t baudrate) {
    if (uart_num > 3) {
        return -1;  // Invalid UART port
    }

    // Calculate divisor for baud rate
    uint16_t divisor = calculate_uart_divisor(baudrate, APB_CLOCK_FREQ);

    // TODO: Write divisor to UART baud rate register
    // This requires access to MCU hardware registers

    return 0;  // Success
}

/**
 * @brief Configure I2C clock divisor for target SCL frequency
 * @param i2c_num I2C port number
 * @param freq_hz Target I2C frequency (100000 or 400000 Hz)
 * @return Status (0 = success)
 */
int configure_i2c_clock(uint8_t i2c_num, uint32_t freq_hz) {
    if (i2c_num > 1) {
        return -1;  // Invalid I2C port
    }

    // Validate frequency
    if (freq_hz != 100000 && freq_hz != 400000) {
        return -2;  // Unsupported frequency
    }

    // Calculate divisor
    uint8_t divisor = calculate_i2c_divisor(freq_hz, AHB_CLOCK_FREQ);

    // TODO: Write divisor to I2C timing register

    return 0;  // Success
}

/**
 * @brief Configure SPI clock divisor
 * @param spi_num SPI port number
 * @param freq_hz Target SPI clock frequency
 * @return Status (0 = success)
 */
int configure_spi_clock(uint8_t spi_num, uint32_t freq_hz) {
    if (spi_num > 1) {
        return -1;  // Invalid SPI port
    }

    // Ensure frequency doesn't exceed half system clock
    if (freq_hz > SYSTEM_CLOCK_FREQ / 2) {
        return -2;  // Frequency too high
    }

    // Calculate divisor (usually power of 2)
    uint8_t divisor = calculate_spi_divisor(freq_hz, SYSTEM_CLOCK_FREQ);

    // TODO: Write divisor to SPI clock register

    return 0;  // Success
}

/**
 * @brief Get actual baud rate achieved for given divisor
 * @param divisor UART divisor value
 * @param clock_freq UART clock frequency
 * @return Actual baud rate
 */
uint32_t get_actual_baudrate(uint16_t divisor, uint32_t clock_freq) {
    if (divisor == 0) return 0;
    return clock_freq / (divisor * 2);
}

/**
 * @brief Calculate error percentage between target and actual baud rate
 * @param target Target baud rate
 * @param actual Actual baud rate
 * @return Error percentage (0-100%)
 */
float calculate_baudrate_error(uint32_t target, uint32_t actual) {
    if (target == 0) return 0.0f;
    float diff = actual > target ? (actual - target) : (target - actual);
    return (diff * 100.0f) / target;
}

}  // namespace libemb::port::abov_a31g123
