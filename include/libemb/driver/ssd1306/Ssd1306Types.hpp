#pragma once

#include <stdint.h>
#include <stddef.h>

namespace libemb::driver {

/**
 * @brief SSD1306 디스플레이 타입 및 상수 정의
 */

// 디스플레이 물리 파라미터
struct Ssd1306Config {
    static constexpr uint8_t DISPLAY_WIDTH = 128;
    static constexpr uint8_t DISPLAY_HEIGHT = 64;
    static constexpr uint8_t PAGES = 8;  // 64 픽셀 / 페이지당 8 픽셀
    static constexpr size_t FRAME_BUFFER_SIZE = DISPLAY_WIDTH * PAGES;
    static constexpr uint8_t DEFAULT_I2C_ADDR = 0x3C;
    static constexpr uint8_t ALTERNATE_I2C_ADDR = 0x3D;
};

/**
 * @brief SSD1306 명령 정의
 */
struct Ssd1306Commands {
    // 디스플레이 제어 명령
    static constexpr uint8_t SET_CONTRAST = 0x81;
    static constexpr uint8_t SET_NORMAL_DISPLAY = 0xA6;
    static constexpr uint8_t SET_INVERSE_DISPLAY = 0xA7;
    static constexpr uint8_t DISPLAY_OFF = 0xAE;
    static constexpr uint8_t DISPLAY_ON = 0xAF;

    // 주소 지정 명령
    static constexpr uint8_t SET_ADDRESS_MODE = 0x20;
    static constexpr uint8_t SET_COLUMN_ADDRESS = 0x21;
    static constexpr uint8_t SET_PAGE_ADDRESS = 0x22;

    // 초기화 시퀀스용 상수
    static constexpr uint8_t DISPLAY_CLOCK_DIVIDER = 0xD5;
    static constexpr uint8_t MULTIPLEX_RATIO = 0xA8;
    static constexpr uint8_t DISPLAY_OFFSET = 0xD3;
    static constexpr uint8_t START_LINE = 0x40;
    static constexpr uint8_t CHARGE_PUMP = 0x8D;
    static constexpr uint8_t SEGMENT_REMAP = 0xA1;
    static constexpr uint8_t COM_SCAN_DIRECTION = 0xC8;
    static constexpr uint8_t COM_PIN_CONFIG = 0xDA;
    static constexpr uint8_t PRECHARGE_PERIOD = 0xD9;
    static constexpr uint8_t VCOMH_LEVEL = 0xDB;
    static constexpr uint8_t ENTIRE_DISPLAY = 0xA4;
};

} // namespace libemb::driver
