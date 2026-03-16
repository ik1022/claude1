/**
 * @file A31G123Registers.hpp
 * @brief ABOV A31G123 MCU Register Definitions
 *
 * Provides register structures and memory-mapped I/O definitions
 * for ABOV A31G123 Cortex-M0+ MCU.
 *
 * @note Based on ABOV A31G123 Datasheet v1.x
 * @note All addresses and register layouts are hardware-specific
 */

#pragma once

#include <cstdint>

namespace libemb::port::abov_a31g123 {

/* ============================================================================
   Memory Map
   ============================================================================ */

// Peripheral base addresses
constexpr uintptr_t GPIOA_BASE = 0x40040000UL;
constexpr uintptr_t GPIOB_BASE = 0x40044000UL;
constexpr uintptr_t GPIOC_BASE = 0x40048000UL;
constexpr uintptr_t GPIOD_BASE = 0x4004C000UL;
constexpr uintptr_t GPIOE_BASE = 0x40050000UL;

constexpr uintptr_t UART0_BASE = 0x40080000UL;
constexpr uintptr_t UART1_BASE = 0x40084000UL;
constexpr uintptr_t UART2_BASE = 0x40088000UL;
constexpr uintptr_t UART3_BASE = 0x4008C000UL;

constexpr uintptr_t I2C0_BASE = 0x40090000UL;
constexpr uintptr_t I2C1_BASE = 0x40094000UL;

constexpr uintptr_t SPI0_BASE = 0x40098000UL;
constexpr uintptr_t SPI1_BASE = 0x4009C000UL;

constexpr uintptr_t TIMER0_BASE = 0x400A0000UL;
constexpr uintptr_t TIMER1_BASE = 0x400A4000UL;

constexpr uintptr_t ADC_BASE = 0x400B0000UL;

constexpr uintptr_t RCC_BASE = 0x40010000UL;  // Reset and Clock Control
constexpr uintptr_t SYSCFG_BASE = 0x40014000UL;

/* ============================================================================
   GPIO Register Structure
   ============================================================================ */

/**
 * @brief GPIO Port Register Structure
 *
 * Each GPIO port (A, B, C, D, E) has the same register layout.
 */
struct GPIO_Type {
    volatile uint32_t CTRL;      ///< 0x00: Port mode control
    volatile uint32_t DOUT;      ///< 0x04: Data output
    volatile uint32_t DIN;       ///< 0x08: Data input (read-only)
    volatile uint32_t TOGGLE;    ///< 0x0C: Toggle output
    volatile uint32_t AFSEL;     ///< 0x10: Alternate function select
    volatile uint32_t PUPU;      ///< 0x14: Pull-up control
    volatile uint32_t PDPU;      ///< 0x18: Pull-down control
    volatile uint32_t ADCSEL;    ///< 0x1C: ADC input select
    volatile uint32_t RESERVED[8];
};

// GPIO mode bits in CTRL register
enum class GPIO_Mode : uint32_t {
    INPUT = 0x00,      ///< 00: Input mode
    OUTPUT = 0x01,     ///< 01: Output mode (push-pull)
    ANALOG = 0x02,     ///< 10: Analog input (ADC)
    RESERVED = 0x03    ///< 11: Reserved
};

/* ============================================================================
   UART Register Structure
   ============================================================================ */

/**
 * @brief UART Register Structure
 *
 * Supports up to 4 UART channels with identical register layout.
 */
struct UART_Type {
    volatile uint32_t DATA;      ///< 0x00: TX/RX data
    volatile uint32_t STAT;      ///< 0x04: Status
    volatile uint32_t CTRL;      ///< 0x08: Control
    volatile uint32_t BAUD;      ///< 0x0C: Baud rate divisor
    volatile uint32_t IE;        ///< 0x10: Interrupt enable
    volatile uint32_t RESERVED[3];
};

// UART Status bits
#define UART_STAT_TXE    0x01  ///< TX empty
#define UART_STAT_RXNE   0x02  ///< RX not empty
#define UART_STAT_TC     0x04  ///< TX complete
#define UART_STAT_FERR   0x08  ///< Framing error
#define UART_STAT_OERR   0x10  ///< Overrun error
#define UART_STAT_PERR   0x20  ///< Parity error

// UART Control bits
#define UART_CTRL_TXEN   0x01  ///< TX enable
#define UART_CTRL_RXEN   0x02  ///< RX enable
#define UART_CTRL_TXIE   0x04  ///< TX interrupt enable
#define UART_CTRL_RXIE   0x08  ///< RX interrupt enable
#define UART_CTRL_MODE8  0x00  ///< 8-bit data
#define UART_CTRL_MODE9  0x10  ///< 9-bit data
#define UART_CTRL_STOP1  0x00  ///< 1 stop bit
#define UART_CTRL_STOP2  0x20  ///< 2 stop bits

/* ============================================================================
   I2C Register Structure
   ============================================================================ */

/**
 * @brief I2C Register Structure
 *
 * I2C Master with clock stretching support.
 */
struct I2C_Type {
    volatile uint32_t CTRL;      ///< 0x00: Control
    volatile uint32_t STAT;      ///< 0x04: Status
    volatile uint32_t ADDR;      ///< 0x08: Slave address
    volatile uint32_t DATA;      ///< 0x0C: TX/RX data
    volatile uint32_t SCLL;      ///< 0x10: SCL low timing
    volatile uint32_t SCLH;      ///< 0x14: SCL high timing
    volatile uint32_t RESERVED[2];
};

// I2C Status bits
#define I2C_STAT_BUSY    0x01  ///< Bus busy
#define I2C_STAT_START   0x02  ///< START condition sent
#define I2C_STAT_STOP    0x04  ///< STOP condition sent
#define I2C_STAT_ACK     0x08  ///< ACK received
#define I2C_STAT_NACK    0x10  ///< NACK received
#define I2C_STAT_DONE    0x20  ///< Transfer complete

// I2C Control bits
#define I2C_CTRL_EN      0x01  ///< I2C enable
#define I2C_CTRL_START   0x02  ///< Generate START
#define I2C_CTRL_STOP    0x04  ///< Generate STOP
#define I2C_CTRL_ACK     0x08  ///< Send ACK
#define I2C_CTRL_NACK    0x10  ///< Send NACK
#define I2C_CTRL_IE      0x20  ///< Interrupt enable

/* ============================================================================
   SPI Register Structure
   ============================================================================ */

/**
 * @brief SPI Register Structure
 *
 * Full-duplex SPI with mode and clock control.
 */
struct SPI_Type {
    volatile uint32_t CTRL;      ///< 0x00: Control
    volatile uint32_t STAT;      ///< 0x04: Status
    volatile uint32_t DATA;      ///< 0x08: TX/RX data
    volatile uint32_t CLK;       ///< 0x0C: Clock divisor
    volatile uint32_t IE;        ///< 0x10: Interrupt enable
    volatile uint32_t RESERVED[3];
};

// SPI Status bits
#define SPI_STAT_TXE     0x01  ///< TX empty
#define SPI_STAT_RXNE    0x02  ///< RX not empty
#define SPI_STAT_BUSY    0x04  ///< SPI busy
#define SPI_STAT_DONE    0x08  ///< Transfer complete

// SPI Control bits
#define SPI_CTRL_EN      0x01  ///< SPI enable
#define SPI_CTRL_MASTER  0x02  ///< Master mode
#define SPI_CTRL_CPOL0   0x00  ///< Clock polarity low
#define SPI_CTRL_CPOL1   0x04  ///< Clock polarity high
#define SPI_CTRL_CPHA0   0x00  ///< Clock phase 0
#define SPI_CTRL_CPHA1   0x08  ///< Clock phase 1
#define SPI_CTRL_IE      0x10  ///< Interrupt enable

/* ============================================================================
   Reset and Clock Control (RCC)
   ============================================================================ */

/**
 * @brief RCC Register Structure
 */
struct RCC_Type {
    volatile uint32_t CR;        ///< 0x00: Clock control
    volatile uint32_t PLL;       ///< 0x04: PLL control
    volatile uint32_t CFGR;      ///< 0x08: Clock config
    volatile uint32_t APB;       ///< 0x0C: APB prescaler
    volatile uint32_t AHB;       ///< 0x10: AHB prescaler
    volatile uint32_t RESERVED[3];
};

// RCC Control bits
#define RCC_CR_HSION     0x01  ///< HSI oscillator on
#define RCC_CR_HSIRDY    0x02  ///< HSI ready
#define RCC_CR_HSEON     0x04  ///< HSE oscillator on
#define RCC_CR_HSERDY    0x08  ///< HSE ready
#define RCC_CR_PLLON     0x10  ///< PLL enable
#define RCC_CR_PLLRDY    0x20  ///< PLL ready

/* ============================================================================
   System Configuration (SYSCFG)
   ============================================================================ */

/**
 * @brief SYSCFG Register Structure
 */
struct SYSCFG_Type {
    volatile uint32_t MEMRMP;    ///< 0x00: Memory remap
    volatile uint32_t EXTINT;    ///< 0x04: External interrupt config
    volatile uint32_t RESERVED[2];
};

/* ============================================================================
   Helper Functions
   ============================================================================ */

/**
 * @brief Get GPIO port from base address
 */
inline volatile GPIO_Type* gpio_from_base(volatile uint32_t* base) {
    return reinterpret_cast<volatile GPIO_Type*>(base);
}

/**
 * @brief Get UART port from base address
 */
inline volatile UART_Type* uart_from_base(volatile uint32_t* base) {
    return reinterpret_cast<volatile UART_Type*>(base);
}

/**
 * @brief Get I2C port from base address
 */
inline volatile I2C_Type* i2c_from_base(volatile uint32_t* base) {
    return reinterpret_cast<volatile I2C_Type*>(base);
}

/**
 * @brief Get SPI port from base address
 */
inline volatile SPI_Type* spi_from_base(volatile uint32_t* base) {
    return reinterpret_cast<volatile SPI_Type*>(base);
}

/**
 * @brief Get RCC peripheral
 */
inline RCC_Type* get_rcc() {
    return reinterpret_cast<RCC_Type*>(RCC_BASE);
}

}  // namespace libemb::port::abov_a31g123
