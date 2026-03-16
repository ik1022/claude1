#pragma once

#include "libemb/hal/II2c.hpp"
#include <vector>

namespace libemb::port::mock {

/**
 * @brief 테스트용 Mock I2C 구현
 *
 * II2c 인터페이스의 Mock 구현으로, 모든 I2C 트랜잭션을 기록합니다.
 * 테스트에서 하드웨어 없이 I2C 기반 드라이버를 검증할 수 있습니다.
 */
class MockI2c : public libemb::hal::II2c {
public:
    /**
     * @brief I2C 트랜잭션 기록
     */
    struct I2cTransaction {
        uint8_t slaveAddr;
        std::vector<uint8_t> data;
        bool isWrite;
    };

    /**
     * @brief I2C 쓰기 작업 수행
     * @param slaveAddr 슬레이브 주소
     * @param data 전송할 데이터 포인터
     * @param length 데이터 길이
     * @return I2cStatus::ERROR (fail_next_write_가 true인 경우) 또는 I2cStatus::OK
     */
    libemb::hal::I2cStatus write(uint8_t slaveAddr, const uint8_t* data,
                                  size_t length) override {
        if (fail_next_write_) {
            fail_next_write_ = false;
            return libemb::hal::I2cStatus::ERROR;
        }
        I2cTransaction txn;
        txn.slaveAddr = slaveAddr;
        txn.isWrite = true;
        for (size_t i = 0; i < length; ++i) {
            txn.data.push_back(data[i]);
        }
        transactions_.push_back(txn);
        return libemb::hal::I2cStatus::OK;
    }

    /**
     * @brief I2C 읽기 작업 수행
     * @param slaveAddr 슬레이브 주소
     * @param data 수신 데이터 버퍼 포인터
     * @param length 수신할 데이터 길이
     * @return I2cStatus::ERROR (fail_next_read_가 true인 경우) 또는 I2cStatus::OK
     */
    libemb::hal::I2cStatus read(uint8_t slaveAddr, [[maybe_unused]] uint8_t* data,
                                [[maybe_unused]] size_t length) override {
        if (fail_next_read_) {
            fail_next_read_ = false;
            return libemb::hal::I2cStatus::ERROR;
        }
        I2cTransaction txn;
        txn.slaveAddr = slaveAddr;
        txn.isWrite = false;
        transactions_.push_back(txn);
        return libemb::hal::I2cStatus::OK;
    }

    /**
     * @brief I2C 쓰기 후 읽기 작업 수행
     * @param slaveAddr 슬레이브 주소
     * @param reg 레지스터 주소
     * @param rxData 수신 데이터 버퍼 포인터
     * @param rxLength 수신할 데이터 길이
     * @return I2cStatus::OK
     */
    libemb::hal::I2cStatus writeRead([[maybe_unused]] uint8_t slaveAddr,
                                      [[maybe_unused]] uint8_t reg,
                                      [[maybe_unused]] uint8_t* rxData,
                                      [[maybe_unused]] size_t rxLength) override {
        return libemb::hal::I2cStatus::OK;
    }

    // ========== 테스트 헬퍼 메서드 ==========

    /**
     * @brief 기록된 모든 트랜잭션 반환
     */
    const std::vector<I2cTransaction>& getTransactions() const {
        return transactions_;
    }

    /**
     * @brief 모든 트랜잭션 기록 삭제
     */
    void clearTransactions() {
        transactions_.clear();
    }

    /**
     * @brief 다음 쓰기 작업을 실패하도록 설정
     */
    void setFailNextWrite() {
        fail_next_write_ = true;
    }

    /**
     * @brief 다음 읽기 작업을 실패하도록 설정
     */
    void setFailNextRead() {
        fail_next_read_ = true;
    }

    /**
     * @brief 기록된 트랜잭션 개수 반환
     */
    size_t getTransactionCount() const {
        return transactions_.size();
    }

private:
    std::vector<I2cTransaction> transactions_;
    bool fail_next_write_ = false;
    bool fail_next_read_ = false;
};

} // namespace libemb::port::mock
