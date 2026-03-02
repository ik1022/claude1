#pragma once

#include "libemb/hal/II2c.hpp"
#include "libemb/util/StaticVector.hpp"
#include <stdint.h>
#include <stddef.h>

namespace libemb::driver {

/**
 * @brief SSD1306 OLED 디스플레이 드라이버
 *
 * HAL 추상화를 사용하는 단색 128x64 I2C OLED 디스플레이 드라이버.
 * 동적 할당 없음 - 임베디드 시스템에 적합합니다.
 */
class Ssd1306 {
public:
    // 디스플레이 치수
    static constexpr uint8_t DISPLAY_WIDTH = 128;
    static constexpr uint8_t DISPLAY_HEIGHT = 64;
    static constexpr uint8_t PAGES = 8;  // 64 픽셀 / 페이지당 8 픽셀
    static constexpr size_t FRAME_BUFFER_SIZE = DISPLAY_WIDTH * PAGES;

    // I2C 슬레이브 주소 (하드웨어에 따라 0x3C 또는 0x3D)
    static constexpr uint8_t DEFAULT_I2C_ADDR = 0x3C;

    // SSD1306 명령 정의
    static constexpr uint8_t CMD_SET_CONTRAST = 0x81;
    static constexpr uint8_t CMD_SET_NORMAL_DISPLAY = 0xA6;
    static constexpr uint8_t CMD_SET_INVERSE_DISPLAY = 0xA7;
    static constexpr uint8_t CMD_DISPLAY_OFF = 0xAE;
    static constexpr uint8_t CMD_DISPLAY_ON = 0xAF;
    static constexpr uint8_t CMD_SET_ADDRESS_MODE = 0x20;
    static constexpr uint8_t CMD_SET_COLUMN_ADDRESS = 0x21;
    static constexpr uint8_t CMD_SET_PAGE_ADDRESS = 0x22;

    /**
     * @brief OLED 디스플레이를 초기화합니다
     * @param i2c I2C HAL 인터페이스에 대한 포인터
     * @param slaveAddr I2C 슬레이브 주소 (기본값 0x3C)
     * @return 초기화 성공 시 true
     */
    bool init(libemb::hal::II2c* i2c, uint8_t slaveAddr = DEFAULT_I2C_ADDR);

    /**
     * @brief 프레임 버퍼를 디스플레이에 씁니다
     * @return 전송 성공 시 true
     */
    bool display();

    /**
     * @brief 프레임 버퍼에 픽셀을 설정합니다
     * @param x 수평 위치 (0-127)
     * @param y 수직 위치 (0-63)
     * @param on 픽셀을 설정하면 true, 지우면 false
     */
    void setPixel(uint8_t x, uint8_t y, bool on);

    /**
     * @brief 전체 디스플레이를 지웁니다
     */
    void clear();

    /**
     * @brief 수평선을 그립니다
     * @param x 시작 x 위치
     * @param y y 위치
     * @param width 선의 너비 (픽셀 단위)
     * @param on 그리면 true, 지우면 false
     */
    void drawHLine(uint8_t x, uint8_t y, uint8_t width, bool on = true);

    /**
     * @brief 수직선을 그립니다
     * @param x x 위치
     * @param y 시작 y 위치
     * @param height 선의 높이 (픽셀 단위)
     * @param on 그리면 true, 지우면 false
     */
    void drawVLine(uint8_t x, uint8_t y, uint8_t height, bool on = true);

    /**
     * @brief 직사각형을 그립니다
     * @param x 왼쪽 상단 x 위치
     * @param y 왼쪽 상단 y 위치
     * @param width 직사각형의 너비
     * @param height 직사각형의 높이
     * @param filled 채워진 직사각형이면 true, 윤곽이면 false
     * @param on 그리면 true, 지우면 false
     */
    void drawRect(uint8_t x, uint8_t y, uint8_t width, uint8_t height,
                  bool filled = false, bool on = true);

    /**
     * @brief 디스플레이 명암을 설정합니다
     * @param contrast 명암 수준 (0-255)
     * @return 명령 전송 성공 시 true
     */
    bool setContrast(uint8_t contrast);

    /**
     * @brief 디스플레이 반전을 활성화/비활성화합니다
     * @param inverted 반전 색상이면 true
     * @return 명령 전송 성공 시 true
     */
    bool setInverted(bool inverted);

    /**
     * @brief 디스플레이를 켜기/끄기합니다
     * @param on 켜기면 true, 끄기면 false
     * @return 명령 전송 성공 시 true
     */
    bool setDisplayOn(bool on);

    /**
     * @brief 프레임 버퍼 포인터를 가져옵니다 (직접 접근 용)
     * @return 프레임 버퍼 데이터에 대한 포인터
     */
    uint8_t* getFrameBuffer() {
        return frame_buffer_.data();
    }

    /**
     * @brief 프레임 버퍼 크기를 가져옵니다
     * @return 프레임 버퍼 크기 (바이트 단위)
     */
    size_t getFrameBufferSize() const {
        return FRAME_BUFFER_SIZE;
    }

private:
    /// 프레임 버퍼: 128 픽셀 너비 x 64 픽셀 높이 = 128 x 8 바이트
    libemb::util::StaticVector<uint8_t, FRAME_BUFFER_SIZE> frame_buffer_;

    /// I2C HAL 인터페이스에 대한 포인터
    libemb::hal::II2c* i2c_ = nullptr;

    /// I2C 슬레이브 주소
    uint8_t slave_addr_ = DEFAULT_I2C_ADDR;

    /**
     * @brief 디스플레이에 명령 바이트를 보냅니다
     * @param cmd 명령 바이트
     * @return 성공 시 true
     */
    bool sendCommand(uint8_t cmd);

    /**
     * @brief 여러 명령 바이트를 보냅니다
     * @param cmds 명령 배열에 대한 포인터
     * @param count 명령 개수
     * @return 성공 시 true
     */
    bool sendCommands(const uint8_t* cmds, size_t count);
};

} // namespace libemb::driver
