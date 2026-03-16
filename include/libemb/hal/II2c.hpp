#pragma once

#include <stdint.h>
#include <stddef.h>

namespace libemb::hal {

/**
 * @brief I2C 전송 상태 열거형
 *
 * I2C 통신 결과를 나타냅니다.
 */
enum class I2cStatus {
    OK = 0,       ///< 전송 성공
    NACK = 1,     ///< 슬레이브에서 응답 없음 (Not Acknowledged)
    TIMEOUT = 2,  ///< 통신 시간 초과
    ERROR = 3,    ///< 기타 전송 오류
};

/**
 * @defgroup hal Hardware Abstraction Layer
 * @brief MCU 독립적 하드웨어 인터페이스
 * @{
 */

/**
 * @brief Inter-Integrated Circuit (I2C) 통신을 위한 추상 인터페이스
 *
 * I2C(또는 TWI) 버스 인터페이스를 제어하기 위한 MCU 독립적 추상화입니다.
 *
 * **특징:**
 * - 슬레이브 주소: 7비트 (0x00-0x7F)
 * - 간단한 쓰기/읽기 연산
 * - 레지스터 기반 쓰기/읽기 (WriteRead)
 * - 상태 피드백 (OK, NACK, TIMEOUT, ERROR)
 *
 * **사용 사례:**
 * - OLED 디스플레이 제어 (SSD1306)
 * - 센서 인터페이스 (온도, 습도, 압력 등)
 * - EEPROM 메모리 접근
 * - 다양한 IC 주변장치 통신
 *
 * @section example 사용 예제:
 * @code
 * // MockI2c를 사용한 테스트
 * auto i2c = std::make_unique<libemb::port::mock::MockI2c>();
 *
 * // 데이터 쓰기
 * uint8_t cmdBuf[] = {0x81, 0xFF};  // 명령, 값
 * auto status = i2c->write(0x3C, cmdBuf, sizeof(cmdBuf));
 *
 * if (status == libemb::hal::I2cStatus::OK) {
 *     printf("Write successful\n");
 * } else {
 *     printf("Write failed: %d\n", static_cast<int>(status));
 * }
 *
 * // 데이터 읽기
 * uint8_t readBuf[2];
 * status = i2c->read(0x48, readBuf, sizeof(readBuf));
 *
 * // 레지스터 쓰기 후 읽기
 * uint8_t regAddr = 0x05;
 * uint8_t data[2];
 * status = i2c->writeRead(0x48, &regAddr, 1, data, 2);
 * @endcode
 *
 * @see libemb::driver::Ssd1306 (I2C 사용 예제)
 */
class II2c {
public:
    virtual ~II2c() = default;

    /**
     * @brief I2C 슬레이브 장치에 데이터를 씁니다
     * @param slaveAddr 슬레이브 장치 주소 (7비트 주소)
     * @param data 데이터 버퍼에 대한 포인터
     * @param length 쓸 바이트 수
     * @return I2C 전송 상태
     */
    virtual I2cStatus write(uint8_t slaveAddr, const uint8_t* data, size_t length) = 0;

    /**
     * @brief I2C 슬레이브 장치에서 데이터를 읽습니다
     * @param slaveAddr 슬레이브 장치 주소 (7비트 주소)
     * @param data 수신 버퍼에 대한 포인터
     * @param length 읽을 바이트 수
     * @return I2C 전송 상태
     */
    virtual I2cStatus read(uint8_t slaveAddr, uint8_t* data, size_t length) = 0;

    /**
     * @brief 레지스터에 데이터를 쓴 다음 응답을 읽습니다
     * @param slaveAddr 슬레이브 장치 주소
     * @param reg 쓸 레지스터 주소
     * @param rxData 수신 버퍼에 대한 포인터
     * @param rxLength 읽을 바이트 수
     * @return I2C 전송 상태
     */
    virtual I2cStatus writeRead(uint8_t slaveAddr, uint8_t reg, uint8_t* rxData, size_t rxLength) = 0;
};

} // namespace libemb::hal
