#pragma once

#include <stdint.h>
#include <stddef.h>

namespace libemb::hal {

/**
 * @brief 직렬 주변장치 통신을 위한 추상 SPI 인터페이스
 */
class ISpi {
public:
    virtual ~ISpi() = default;

    /**
     * @brief SPI를 통해 단일 바이트를 전송합니다
     * @param byte 전송할 바이트
     * @return 전송 중에 수신한 바이트
     */
    virtual uint8_t transmit(uint8_t byte) = 0;

    /**
     * @brief 데이터를 동시에 전송하고 수신합니다
     * @param txData 전송 버퍼에 대한 포인터
     * @param rxData 수신 버퍼에 대한 포인터
     * @param length 교환할 바이트 수
     */
    virtual void exchange(const uint8_t* txData, uint8_t* rxData, size_t length) = 0;

    /**
     * @brief 데이터만 전송합니다 (수신 데이터 무시)
     * @param data 데이터 버퍼에 대한 포인터
     * @param length 전송할 바이트 수
     */
    virtual void transmitOnly(const uint8_t* data, size_t length) = 0;

    /**
     * @brief 데이터만 수신합니다 (0을 전송)
     * @param data 수신 버퍼에 대한 포인터
     * @param length 수신할 바이트 수
     */
    virtual void receiveOnly(uint8_t* data, size_t length) = 0;
};

} // namespace libemb::hal
