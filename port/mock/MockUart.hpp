#pragma once

#include "libemb/hal/IUart.hpp"
#include <queue>
#include <vector>

namespace libemb::port::mock {

/**
 * @brief 테스트용 Mock UART 구현
 *
 * 구성 가능한 읽기 큐가 있는 UART 인터페이스를 시뮬레이션합니다.
 * 검증을 위해 모든 전송 데이터를 기록합니다.
 */
class MockUart : public libemb::hal::IUart {
public:
    MockUart() = default;

    void putChar(uint8_t byte) override {
        tx_buffer_.push_back(byte);
    }

    uint8_t getChar() override {
        if (rx_queue_.empty()) {
            return 0;
        }
        uint8_t byte = rx_queue_.front();
        rx_queue_.pop();
        return byte;
    }

    bool isDataAvailable() const override {
        return !rx_queue_.empty();
    }

    void write(const uint8_t* data, size_t length) override {
        for (size_t i = 0; i < length; ++i) {
            tx_buffer_.push_back(data[i]);
        }
    }

    size_t read(uint8_t* data, size_t maxLength) override {
        size_t count = 0;
        while (count < maxLength && !rx_queue_.empty()) {
            data[count++] = rx_queue_.front();
            rx_queue_.pop();
        }
        return count;
    }

    // 테스트 헬퍼 메서드

    /// 수신할 데이터를 큐에 추가합니다
    void queueRxData(const uint8_t* data, size_t length) {
        for (size_t i = 0; i < length; ++i) {
            rx_queue_.push(data[i]);
        }
    }

    /// 모든 전송 데이터를 가져옵니다
    const std::vector<uint8_t>& getTxBuffer() const {
        return tx_buffer_;
    }

    /// 전송 데이터를 문자열로 가져옵니다 (ASCII 가정)
    std::string getTxString() const {
        return std::string(tx_buffer_.begin(), tx_buffer_.end());
    }

    /// 전송 버퍼를 지웁니다
    void clearTxBuffer() {
        tx_buffer_.clear();
    }

    /// 수신 큐를 지웁니다
    void clearRxQueue() {
        while (!rx_queue_.empty()) {
            rx_queue_.pop();
        }
    }

    /// 수신 큐의 크기를 가져옵니다
    size_t getRxQueueSize() const {
        return rx_queue_.size();
    }

private:
    std::vector<uint8_t> tx_buffer_;
    std::queue<uint8_t> rx_queue_;
};

} // namespace libemb::port::mock
