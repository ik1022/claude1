#include "libemb/driver/Ssd1306.hpp"

namespace libemb::driver {

bool Ssd1306::init(libemb::hal::II2c* i2c, uint8_t slaveAddr) {
    if (i2c == nullptr) {
        return false;
    }

    i2c_ = i2c;
    slave_addr_ = slaveAddr;

    // 프레임 버퍼 초기화
    clear();

    // SSD1306 초기화 시퀀스
    const uint8_t init_cmds[] = {
        0xAE,        // 디스플레이 OFF
        0xD5, 0x80,  // 디스플레이 클록 분주비 설정
        0xA8, 0x3F,  // 멀티플렉스 비율 설정 (1/64 duty)
        0xD3, 0x00,  // 디스플레이 오프셋 설정
        0x40,        // 시작 라인 주소 설정
        0x8D, 0x14,  // 충전 펌프 활성화
        0x20, 0x00,  // 메모리 주소 지정 모드 설정 (수평)
        0xA1,        // 세그먼트 재매핑 설정 (열 127 = SEG0)
        0xC8,        // COM 출력 스캔 방향 설정
        0xDA, 0x12,  // COM 핀 하드웨어 구성 설정
        0x81, 0x7F,  // 명암 제어 설정
        0xD9, 0xF1,  // 사전 충전 기간 설정
        0xDB, 0x40,  // Vcomh 선택 해제 레벨 설정
        0xA4,        // 전체 디스플레이 ON 비활성화
        0xA6,        // 일반 디스플레이 설정 (반전 아님)
        0xAF,        // 디스플레이 ON
    };

    // 초기화 명령 전송
    if (!sendCommands(init_cmds, sizeof(init_cmds))) {
        return false;
    }

    return true;
}

bool Ssd1306::display() {
    if (i2c_ == nullptr) {
        return false;
    }

    // 열 주소 범위 설정 (0-127)
    const uint8_t col_cmds[] = {
        CMD_SET_COLUMN_ADDRESS, 0x00, 0x7F
    };
    if (!sendCommands(col_cmds, sizeof(col_cmds))) {
        return false;
    }

    // 페이지 주소 범위 설정 (0-7)
    const uint8_t page_cmds[] = {
        CMD_SET_PAGE_ADDRESS, 0x00, 0x07
    };
    if (!sendCommands(page_cmds, sizeof(page_cmds))) {
        return false;
    }

    // 프레임 버퍼 데이터 전송
    // 데이터 바이트: 0x40 (데이터 모드 표시자) + 프레임 버퍼
    uint8_t data_header = 0x40;
    auto status = i2c_->write(slave_addr_, &data_header, 1);
    if (status != libemb::hal::I2cStatus::OK) {
        return false;
    }

    status = i2c_->write(slave_addr_, frame_buffer_.data(), frame_buffer_.size());
    return status == libemb::hal::I2cStatus::OK;
}

void Ssd1306::setPixel(uint8_t x, uint8_t y, bool on) {
    if (x >= DISPLAY_WIDTH || y >= DISPLAY_HEIGHT) {
        return;
    }

    uint8_t page = y / 8;
    uint8_t bit = y % 8;
    size_t index = static_cast<size_t>(page) * DISPLAY_WIDTH + x;

    if (index >= FRAME_BUFFER_SIZE) {
        return;
    }

    uint8_t byte_val = frame_buffer_.data()[index];
    if (on) {
        byte_val |= (1 << bit);
    } else {
        byte_val &= ~(1 << bit);
    }
    frame_buffer_.data()[index] = byte_val;
}

void Ssd1306::clear() {
    frame_buffer_.clear();
    for (size_t i = 0; i < FRAME_BUFFER_SIZE; ++i) {
        frame_buffer_.push_back(0x00);
    }
}

void Ssd1306::drawHLine(uint8_t x, uint8_t y, uint8_t width, bool on) {
    uint8_t end_x = (x + width > DISPLAY_WIDTH) ? DISPLAY_WIDTH : (x + width);
    for (uint8_t i = x; i < end_x; ++i) {
        setPixel(i, y, on);
    }
}

void Ssd1306::drawVLine(uint8_t x, uint8_t y, uint8_t height, bool on) {
    uint8_t end_y = (y + height > DISPLAY_HEIGHT) ? DISPLAY_HEIGHT : (y + height);
    for (uint8_t i = y; i < end_y; ++i) {
        setPixel(x, i, on);
    }
}

void Ssd1306::drawRect(uint8_t x, uint8_t y, uint8_t width, uint8_t height,
                       bool filled, bool on) {
    if (filled) {
        for (uint8_t j = y; j < y + height && j < DISPLAY_HEIGHT; ++j) {
            drawHLine(x, j, width, on);
        }
    } else {
        drawHLine(x, y, width, on);                           // 상단
        drawHLine(x, y + height - 1, width, on);              // 하단
        drawVLine(x, y, height, on);                          // 왼쪽
        drawVLine(x + width - 1, y, height, on);              // 오른쪽
    }
}

bool Ssd1306::setContrast(uint8_t contrast) {
    const uint8_t cmds[] = {CMD_SET_CONTRAST, contrast};
    return sendCommands(cmds, sizeof(cmds));
}

bool Ssd1306::setInverted(bool inverted) {
    uint8_t cmd = inverted ? CMD_SET_INVERSE_DISPLAY : CMD_SET_NORMAL_DISPLAY;
    return sendCommand(cmd);
}

bool Ssd1306::setDisplayOn(bool on) {
    uint8_t cmd = on ? CMD_DISPLAY_ON : CMD_DISPLAY_OFF;
    return sendCommand(cmd);
}

bool Ssd1306::sendCommand(uint8_t cmd) {
    return sendCommands(&cmd, 1);
}

bool Ssd1306::sendCommands(const uint8_t* cmds, size_t count) {
    if (i2c_ == nullptr || cmds == nullptr) {
        return false;
    }

    // 명령 모드: 0x00는 제어 바이트를 나타냅니다 (명령 뒤에 따라옴)
    uint8_t control_byte = 0x00;
    auto status = i2c_->write(slave_addr_, &control_byte, 1);
    if (status != libemb::hal::I2cStatus::OK) {
        return false;
    }

    status = i2c_->write(slave_addr_, cmds, count);
    return status == libemb::hal::I2cStatus::OK;
}

} // namespace libemb::driver
