#pragma once

#include <stdint.h>
#include <stddef.h>

namespace libemb::hal {

/**
 * @brief 직렬 통신을 위한 추상 UART 인터페이스
 *
 * UART(Universal Asynchronous Receiver-Transmitter) 장치를 제어하기 위한
 * MCU 독립적인 인터페이스입니다.
 *
 * 이 인터페이스는 다음을 제공합니다:
 * - 단일 바이트 송수신 (@ref putChar, @ref getChar)
 * - 대량 데이터 송수신 (@ref write, @ref read)
 * - 데이터 가용성 확인 (@ref isDataAvailable)
 *
 * @note 블로킹/논블로킹 동작은 구현에 따라 달라집니다.
 *
 * @section usage 사용 예:
 * @code
 * // Mock 구현으로 테스트
 * auto uart = std::make_unique<MockUart>();
 *
 * // 단일 바이트 전송
 * uart->putChar('H');
 * uart->putChar('i');
 *
 * // 데이터 확인
 * if (uart->isDataAvailable()) {
 *     uint8_t byte = uart->getChar();
 * }
 *
 * // 대량 데이터 전송
 * uint8_t buffer[] = {'H', 'e', 'l', 'l', 'o'};
 * uart->write(buffer, sizeof(buffer));
 * @endcode
 */
class IUart {
public:
    virtual ~IUart() = default;

    /**
     * @brief UART으로 단일 바이트를 전송합니다
     * @param byte 전송할 바이트
     */
    virtual void putChar(uint8_t byte) = 0;

    /**
     * @brief UART에서 단일 바이트를 수신합니다
     * @return 수신한 바이트, 또는 사용 가능한 데이터가 없으면 0
     */
    virtual uint8_t getChar() = 0;

    /**
     * @brief 읽을 수 있는 데이터가 있는지 확인합니다
     * @return 최소 1바이트 이상 사용 가능하면 true
     */
    virtual bool isDataAvailable() const = 0;

    /**
     * @brief 여러 바이트를 전송합니다
     * @param data 데이터 버퍼에 대한 포인터
     * @param length 전송할 바이트 수
     */
    virtual void write(const uint8_t* data, size_t length) = 0;

    /**
     * @brief 여러 바이트를 수신합니다
     * @param data 수신 버퍼에 대한 포인터
     * @param maxLength 수신할 최대 바이트 수
     * @return 실제로 수신한 바이트 수
     */
    virtual size_t read(uint8_t* data, size_t maxLength) = 0;
};

} // namespace libemb::hal
