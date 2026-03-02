#pragma once

#include "libemb/hal/IGpio.hpp"
#include <vector>

namespace libemb::port::mock {

/**
 * @brief 테스트용 Mock GPIO 구현
 *
 * 단위 테스트에서 검증하기 위해 모든 핀 상태 변화를 기록합니다.
 */
class MockGpio : public libemb::hal::IGpio {
public:
    /// GPIO 상태 변화 기록
    struct WriteRecord {
        libemb::hal::GpioState state;
        size_t timestamp;  // 작업 시퀀스 번호
    };

    MockGpio() : current_state_(libemb::hal::GpioState::LOW),
                 mode_(libemb::hal::GpioMode::INPUT),
                 write_count_(0) {}

    void setMode(libemb::hal::GpioMode mode) override {
        mode_ = mode;
    }

    void write(libemb::hal::GpioState state) override {
        current_state_ = state;
        write_records_.push_back({state, write_count_++});
    }

    libemb::hal::GpioState read() const override {
        return current_state_;
    }

    void toggle() override {
        current_state_ = (current_state_ == libemb::hal::GpioState::LOW)
                            ? libemb::hal::GpioState::HIGH
                            : libemb::hal::GpioState::LOW;
        write_records_.push_back({current_state_, write_count_++});
    }

    // 테스트 헬퍼 메서드

    /// 기록된 모든 상태 변화를 가져옵니다
    const std::vector<WriteRecord>& getWriteRecords() const {
        return write_records_;
    }

    /// 쓰기 작업 횟수를 가져옵니다
    size_t getWriteCount() const {
        return write_count_;
    }

    /// 모든 기록을 지웁니다
    void clearRecords() {
        write_records_.clear();
        write_count_ = 0;
    }

    /// 현재 모드를 가져옵니다
    libemb::hal::GpioMode getMode() const {
        return mode_;
    }

    /// 현재 상태를 가져옵니다
    libemb::hal::GpioState getState() const {
        return current_state_;
    }

private:
    libemb::hal::GpioState current_state_;
    libemb::hal::GpioMode mode_;
    std::vector<WriteRecord> write_records_;
    size_t write_count_;
};

} // namespace libemb::port::mock
