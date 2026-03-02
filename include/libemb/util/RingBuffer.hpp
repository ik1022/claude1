#pragma once

#include <stdint.h>
#include <stddef.h>

namespace libemb::util {

/**
 * @brief 동적 할당이 없는 고정 크기 링 버퍼
 *
 * 단일 생산자/단일 소비자에 대해 스레드 안전합니다 (락 없음).
 * 메모리가 제한된 임베디드 시스템에 적합합니다.
 *
 * @tparam T 요소 타입
 * @tparam N 버퍼 용량
 */
template<typename T, size_t N>
class RingBuffer {
public:
    /// 저장할 수 있는 최대 요소 수
    static constexpr size_t CAPACITY = N;

    /// @brief 빈 링 버퍼를 초기화합니다
    RingBuffer() : head_(0), tail_(0), count_(0) {}

    /// @brief 뒤쪽에 요소를 추가합니다
    /// @return 성공하면 true, 버퍼가 가득 차면 false
    bool push(const T& value) {
        if (count_ >= CAPACITY) {
            return false;
        }
        buffer_[tail_] = value;
        tail_ = (tail_ + 1) % CAPACITY;
        count_++;
        return true;
    }

    /// @brief 앞쪽에서 요소를 제거합니다
    /// @param value 꺼낸 값을 저장할 참조
    /// @return 성공하면 true, 버퍼가 비어있으면 false
    bool pop(T& value) {
        if (count_ == 0) {
            return false;
        }
        value = buffer_[head_];
        head_ = (head_ + 1) % CAPACITY;
        count_--;
        return true;
    }

    /// @brief 요소를 제거하지 않고 앞쪽 요소를 봅니다
    /// @param value 미리 보기한 값을 저장할 참조
    /// @return 버퍼가 비어있지 않으면 true
    bool peek(T& value) const {
        if (count_ == 0) {
            return false;
        }
        value = buffer_[head_];
        return true;
    }

    /// @brief 현재 요소의 개수를 가져옵니다
    size_t size() const {
        return count_;
    }

    /// @brief 버퍼가 비어있는지 확인합니다
    bool empty() const {
        return count_ == 0;
    }

    /// @brief 버퍼가 가득 찼는지 확인합니다
    bool full() const {
        return count_ >= CAPACITY;
    }

    /// @brief 버퍼에서 모든 요소를 제거합니다
    void clear() {
        head_ = 0;
        tail_ = 0;
        count_ = 0;
    }

private:
    T buffer_[N];
    size_t head_;
    size_t tail_;
    size_t count_;
};

} // namespace libemb::util
