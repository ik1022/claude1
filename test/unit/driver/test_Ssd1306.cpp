#include <gtest/gtest.h>
#include "libemb/driver/ssd1306/Ssd1306.hpp"
#include "port/mock/hal/MockI2c.hpp"

using namespace libemb::driver;
using namespace libemb::port::mock;

class Ssd1306Test : public ::testing::Test {
protected:
    MockI2c mock_i2c_;
    Ssd1306 display;
};

TEST_F(Ssd1306Test, InitializationSuccess) {
    // 초기화 성공 테스트
    EXPECT_TRUE(display.init(&mock_i2c_));

    // 초기화 명령이 전송되었는지 확인
    EXPECT_GT(mock_i2c_.getTransactionCount(), 0);

    // 사용된 슬레이브 주소 확인 (기본값 0x3C)
    const auto& txns = mock_i2c_.getTransactions();
    EXPECT_EQ(txns[0].slaveAddr, 0x3C);
}

TEST_F(Ssd1306Test, InitializationWithCustomAddress) {
    // 사용자 정의 주소로 초기화 테스트
    uint8_t custom_addr = 0x3D;
    EXPECT_TRUE(display.init(&mock_i2c_, custom_addr));

    const auto& txns = mock_i2c_.getTransactions();
    EXPECT_EQ(txns[0].slaveAddr, custom_addr);
}

TEST_F(Ssd1306Test, InitializationFailsWithNullI2c) {
    // null I2C로 초기화 실패 테스트
    EXPECT_FALSE(display.init(nullptr));
}

TEST_F(Ssd1306Test, ClearDisplay) {
    // 디스플레이 초기화 테스트
    EXPECT_TRUE(display.init(&mock_i2c_));
    mock_i2c_.clearTransactions();

    display.clear();
    uint8_t* fb = display.getFrameBuffer();
    EXPECT_EQ(display.getFrameBufferSize(), 1024);  // 128 * 8

    // 모든 바이트가 0인지 확인
    for (size_t i = 0; i < display.getFrameBufferSize(); ++i) {
        EXPECT_EQ(fb[i], 0x00);
    }
}

TEST_F(Ssd1306Test, SetPixel) {
    // 픽셀 설정 테스트
    display.init(&mock_i2c_);
    display.clear();

    // 픽셀 설정
    display.setPixel(0, 0, true);

    uint8_t* fb = display.getFrameBuffer();
    EXPECT_EQ(fb[0], 0x01);  // 첫 비트 설정됨

    // 같은 바이트에서 다른 픽셀 설정
    display.setPixel(0, 2, true);
    EXPECT_EQ(fb[0], 0x05);  // 비트 0과 2 설정됨

    // 픽셀 제거
    display.setPixel(0, 0, false);
    EXPECT_EQ(fb[0], 0x04);  // 비트 2만 설정됨
}

TEST_F(Ssd1306Test, SetPixelBoundaryCheck) {
    // 경계 외의 픽셀 설정 테스트
    display.init(&mock_i2c_);
    display.clear();

    // 경계 외의 픽셀 설정 시도 - 충돌하지 않아야 함
    display.setPixel(128, 64, true);
    display.setPixel(200, 100, true);
    display.setPixel(255, 255, true);

    // 충돌이 없고 프레임 버퍼가 변하지 않았는지 확인
    uint8_t* fb = display.getFrameBuffer();
    EXPECT_EQ(fb[0], 0x00);
}

TEST_F(Ssd1306Test, DrawHorizontalLine) {
    // 수평선 그리기 테스트
    display.init(&mock_i2c_);
    display.clear();

    // y=0, x=0, width=8에서 수평선 그리기
    display.drawHLine(0, 0, 8, true);

    uint8_t* fb = display.getFrameBuffer();
    // 처음 8개 바이트가 0x01로 설정되어야 함 (행 0의 모든 픽셀)
    for (int i = 0; i < 8; ++i) {
        EXPECT_EQ(fb[i], 0x01);
    }
}

TEST_F(Ssd1306Test, DrawVerticalLine) {
    // 수직선 그리기 테스트
    display.init(&mock_i2c_);
    display.clear();

    // x=0, y=0, height=8에서 수직선 그리기
    display.drawVLine(0, 0, 8, true);

    uint8_t* fb = display.getFrameBuffer();
    // 첫 바이트의 모든 8비트가 설정되어야 함
    EXPECT_EQ(fb[0], 0xFF);
}

TEST_F(Ssd1306Test, DrawRectangleOutline) {
    // 직사각형 윤곽 그리기 테스트
    display.init(&mock_i2c_);
    display.clear();

    // (0,0)에서 4x4 직사각형 윤곽 그리기
    display.drawRect(0, 0, 4, 4, false, true);

    uint8_t* fb = display.getFrameBuffer();
    // 첫 바이트는 상단 및 좌측 가장자리에 대한 비트가 설정되어야 함
    EXPECT_NE(fb[0], 0x00);
}

TEST_F(Ssd1306Test, DrawFilledRectangle) {
    // 채워진 직사각형 그리기 테스트
    display.init(&mock_i2c_);
    display.clear();

    // (0,0)에서 8x8 채워진 직사각형 그리기
    display.drawRect(0, 0, 8, 8, true, true);

    uint8_t* fb = display.getFrameBuffer();
    // 첫 바이트는 완전히 설정되어야 함
    EXPECT_EQ(fb[0], 0xFF);
    // 두 번째 바이트 (x=1)도 완전히 설정되어야 함
    EXPECT_EQ(fb[1], 0xFF);
}

TEST_F(Ssd1306Test, DisplaySendsFBData) {
    // 디스플레이에 프레임 버퍼 데이터 전송 테스트
    EXPECT_TRUE(display.init(&mock_i2c_));
    display.clear();
    mock_i2c_.clearTransactions();

    // 뭔가 그리고 디스플레이에 전송
    display.setPixel(64, 32, true);
    EXPECT_TRUE(display.display());

    // 열 명령, 페이지 명령 및 데이터를 전송해야 함
    EXPECT_GT(mock_i2c_.getTransactionCount(), 2);
}

TEST_F(Ssd1306Test, DisplayFailsBeforeInit) {
    // 초기화 전 디스플레이 실패 테스트
    // 초기화하지 않음
    EXPECT_FALSE(display.display());
}

TEST_F(Ssd1306Test, SetContrastCommand) {
    // 명암 설정 명령 테스트
    EXPECT_TRUE(display.init(&mock_i2c_));
    mock_i2c_.clearTransactions();

    EXPECT_TRUE(display.setContrast(128));

    const auto& txns = mock_i2c_.getTransactions();
    // 첫 번째 트랜잭션은 제어 바이트 (0x00), 두 번째는 명령 데이터
    EXPECT_GE(txns.size(), 2);
    EXPECT_EQ(txns[1].data[0], 0x81);  // 명암 명령
    EXPECT_EQ(txns[1].data[1], 128);   // 명암 값
}

TEST_F(Ssd1306Test, SetInvertedCommand) {
    // 반전 명령 테스트
    EXPECT_TRUE(display.init(&mock_i2c_));
    mock_i2c_.clearTransactions();

    EXPECT_TRUE(display.setInverted(true));

    const auto& txns = mock_i2c_.getTransactions();
    EXPECT_GE(txns.size(), 2);
    EXPECT_EQ(txns[1].data[0], 0xA7);  // 반전 디스플레이 명령
}

TEST_F(Ssd1306Test, SetDisplayOnOff) {
    // 디스플레이 켜기/끄기 테스트
    EXPECT_TRUE(display.init(&mock_i2c_));
    mock_i2c_.clearTransactions();

    EXPECT_TRUE(display.setDisplayOn(false));

    const auto& txns = mock_i2c_.getTransactions();
    EXPECT_GE(txns.size(), 2);
    EXPECT_EQ(txns[1].data[0], 0xAE);  // 디스플레이 OFF 명령

    mock_i2c_.clearTransactions();

    EXPECT_TRUE(display.setDisplayOn(true));

    const auto& txns2 = mock_i2c_.getTransactions();
    EXPECT_GE(txns2.size(), 2);
    EXPECT_EQ(txns2[1].data[0], 0xAF);  // 디스플레이 ON 명령
}
