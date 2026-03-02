#pragma once

#include <stdint.h>
#include <stddef.h>

namespace libemb::hal {

/**
 * @brief I2C 전송 상태 열거형
 */
enum class I2cStatus {
    OK = 0,
    NACK = 1,
    TIMEOUT = 2,
    ERROR = 3,
};

/**
 * @brief 단일 집적 회로(Inter-Integrated Circuit) 통신을 위한 추상 I2C 인터페이스
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
