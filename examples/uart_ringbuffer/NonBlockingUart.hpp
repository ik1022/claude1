#pragma once

#include "libemb/hal/IUart.hpp"
#include "libemb/util/RingBuffer.hpp"

namespace libemb::example {

/**
 * @brief TX/RX 버퍼링을 위해 RingBuffer를 사용하는 비블로킹 UART 드라이버
 *
 * 이 드라이버는 인터럽트 핸들러와 메인 애플리케이션을 분리합니다:
 * - RX: 인터럽트가 RX 버퍼에 데이터를 저장하고, 메인 루프가 처리합니다
 * - TX: 메인 루프가 TX 버퍼에 데이터를 큐에 추가하고, 인터럽트가 전송합니다
 *
 * @tparam RxSize 수신 버퍼 크기 (일반적으로 전이중의 경우 256)
 * @tparam TxSize 전송 버퍼 크기 (일반적으로 출력의 경우 128)
 */
template<size_t RxSize = 256, size_t TxSize = 128>
class NonBlockingUart {
public:
    /// RX/TX 상태 코드
    enum class Status {
        OK,           ///< 작업 성공
        TX_FULL,      ///< TX 버퍼 가득 참, 큐 실패
        RX_EMPTY,     ///< RX 버퍼 비어있음, 사용 가능한 데이터 없음
    };

    /**
     * @brief HAL 인터페이스로 드라이버를 초기화합니다
     * @param uart 기본 UART HAL 구현에 대한 포인터
     */
    void init(libemb::hal::IUart* uart) {
        uart_ = uart;
        rx_buffer_.clear();
        tx_buffer_.clear();
    }

    /**
     * @brief 전송을 위해 데이터를 큐에 추가합니다 (비블로킹)
     *
     * TX 링 버퍼에 바이트를 추가합니다. 인터럽트 핸들러가
     * 버퍼된 데이터를 점진적으로 전송합니다.
     *
     * @param data 전송할 데이터에 대한 포인터
     * @param length 큐에 추가할 바이트 수
     * @return 모든 바이트가 큐에 추가되면 Status::OK, 버퍼 오버플로우 시 Status::TX_FULL
     */
    Status write(const uint8_t* data, size_t length) {
        for (size_t i = 0; i < length; ++i) {
            if (!tx_buffer_.push(data[i])) {
                return Status::TX_FULL;  // 버퍼 가득 참
            }
        }
        return Status::OK;
    }

    /**
     * @brief 단일 바이트를 전송 큐에 추가합니다
     * @param byte 전송할 바이트
     * @return Status::OK 또는 Status::TX_FULL
     */
    Status writeByte(uint8_t byte) {
        return tx_buffer_.push(byte) ? Status::OK : Status::TX_FULL;
    }

    /**
     * @brief 수신 데이터를 읽습니다 (비블로킹)
     *
     * 인터럽트 핸들러에 의해 수신 및 버퍼된 바이트를 검색합니다.
     *
     * @param data 출력 버퍼에 대한 포인터
     * @param maxLength 읽을 최대 바이트 수
     * @return 실제로 읽은 바이트 수 (버퍼가 비어있으면 0)
     */
    size_t read(uint8_t* data, size_t maxLength) {
        size_t count = 0;
        while (count < maxLength && rx_buffer_.pop(data[count])) {
            count++;
        }
        return count;
    }

    /**
     * @brief 사용 가능한 경우 단일 바이트를 읽습니다
     * @param byte 바이트를 저장할 참조
     * @return 바이트가 사용 가능하고 읽었으면 true, 버퍼가 비어있으면 false
     */
    bool readByte(uint8_t& byte) {
        return rx_buffer_.pop(byte);
    }

    /**
     * @brief 제거하지 않고 수신 데이터를 미리봅니다
     * @param byte 미리본 바이트를 저장할 참조
     * @return 데이터가 사용 가능하면 true, 버퍼가 비어있으면 false
     */
    bool peekByte(uint8_t& byte) const {
        return rx_buffer_.peek(byte);
    }

    /**
     * @brief RX 버퍼에 사용 가능한 데이터가 있는지 확인합니다
     * @return 데이터가 사용 가능하면 true
     */
    bool isDataAvailable() const {
        return !rx_buffer_.empty();
    }

    /**
     * @brief TX 버퍼가 가득 찼는지 확인합니다
     * @return TX 버퍼가 가득 차면 true
     */
    bool isTxFull() const {
        return tx_buffer_.full();
    }

    /**
     * @brief 현재 RX 버퍼 채우기 수준을 가져옵니다
     * @return RX 버퍼의 바이트 수
     */
    size_t getRxBufferLevel() const {
        return rx_buffer_.size();
    }

    /**
     * @brief 현재 TX 버퍼 채우기 수준을 가져옵니다
     * @return TX 버퍼의 바이트 수
     */
    size_t getTxBufferLevel() const {
        return tx_buffer_.size();
    }

    // ====== 인터럽트 핸들러 (UART ISR에서 호출) ======

    /**
     * @brief UART RX 인터럽트 핸들러 - RX ISR에서 호출
     *
     * UART에서 한 바이트를 수신하여 RX 링 버퍼에 저장합니다.
     * UART RX 인터럽트 핸들러에서 호출하세요.
     *
     * 예제 (STM32):
     * @code
     * void UART1_IRQHandler(void) {
     *     if (LL_USART_IsActiveFlag_RXNE(UART1)) {
     *         uint8_t byte = LL_USART_ReceiveData8(UART1);
     *         uart_driver.onRxInterrupt(byte);
     *     }
     * }
     * @endcode
     *
     * @param byte UART에서 수신한 바이트
     */
    void onRxInterrupt(uint8_t byte) {
        // RX 버퍼에 저장합니다 (가득 차면 버림)
        rx_buffer_.push(byte);
    }

    /**
     * @brief UART TX 인터럽트 핸들러 - TX ISR에서 호출
     *
     * TX 링 버퍼에서 다음 바이트를 UART로 전송합니다.
     * UART TX 빈 인터럽트 핸들러에서 호출하세요.
     *
     * 예제 (STM32):
     * @code
     * void UART1_IRQHandler(void) {
     *     if (LL_USART_IsActiveFlag_TXE(UART1)) {
     *         uart_driver.onTxInterrupt();
     *     }
     * }
     * @endcode
     *
     * @return 보낼 더 많은 데이터가 있으면 true, TX 버퍼가 비어있으면 false
     */
    bool onTxInterrupt() {
        uint8_t byte;
        if (tx_buffer_.pop(byte)) {
            uart_->putChar(byte);
            return true;  // 버퍼에 더 많은 데이터
        }
        // TX 버퍼 비어있음 - ISR에서 TX 인터럽트 비활성화
        return false;
    }

private:
    libemb::hal::IUart* uart_ = nullptr;
    libemb::util::RingBuffer<uint8_t, RxSize> rx_buffer_;
    libemb::util::RingBuffer<uint8_t, TxSize> tx_buffer_;
};

}  // namespace libemb::example
