#pragma once

#include <stddef.h>
#include <assert.h>

namespace libemb::util {

/**
 * @brief 정적 할당을 사용하는 고정 크기 벡터
 *
 * std::vector를 임베디드 환경에 맞게 구현한 것입니다.
 * 동적 메모리 할당이 없으므로 예측 가능한 성능을 보장합니다.
 *
 * **특징:**
 * - 표준 벡터와 유사한 인터페이스 (push_back, pop_back, operator[])
 * - 컴파일 타임 크기 정의 (템플릿 매개변수)
 * - 메모리 단편화 없음 (스택 할당)
 * - 범위 기반 for 루프 지원
 *
 * **제약:**
 * - 크기는 컴파일 타임에 정의됨 (변경 불가)
 * - 선형 시간의 삽입/삭제 (중간 위치)
 *
 * @tparam T 요소 타입 (기본 타입, POD 타입 권장)
 * @tparam N 최대 용량 (컴파일 타임 상수)
 *
 * @section example 예제:
 * @code
 * // 최대 10개 요소를 보관할 벡터
 * libemb::util::StaticVector<int, 10> vec;
 *
 * // 요소 추가
 * for (int i = 0; i < 5; i++) {
 *     vec.push_back(i * 10);
 * }
 *
 * // 요소 접근
 * for (size_t i = 0; i < vec.size(); i++) {
 *     printf("vec[%zu] = %d\n", i, vec[i]);
 * }
 *
 * // 범위 기반 for 루프
 * for (int val : vec) {
 *     printf("Value: %d\n", val);
 * }
 *
 * // 상태 확인
 * printf("Size: %zu, Capacity: %zu\n", vec.size(), vec.capacity());
 * @endcode
 */
template<typename T, size_t N>
class StaticVector {
public:
    /// 최대 용량
    static constexpr size_t CAPACITY = N;

    using value_type = T;
    using reference = T&;
    using const_reference = const T&;
    using pointer = T*;
    using const_pointer = const T*;
    using size_type = size_t;

    /// @brief 빈 벡터를 초기화합니다
    StaticVector() : size_(0) {}

    /// @brief 현재 요소의 개수를 가져옵니다
    size_type size() const {
        return size_;
    }

    /// @brief 벡터가 비어있는지 확인합니다
    bool empty() const {
        return size_ == 0;
    }

    /// @brief 벡터가 가득 찼는지 확인합니다
    bool full() const {
        return size_ >= CAPACITY;
    }

    /// @brief 최대 용량을 가져옵니다
    size_type capacity() const {
        return CAPACITY;
    }

    /// @brief 인덱스로 요소에 접근합니다 (경계 검사 포함)
    reference at(size_type index) {
        assert(index < size_);
        return buffer_[index];
    }

    /// @brief 인덱스로 요소에 접근합니다 (경계 검사 포함, const)
    const_reference at(size_type index) const {
        assert(index < size_);
        return buffer_[index];
    }

    /// @brief 인덱스로 요소에 접근합니다 (검사 없음)
    reference operator[](size_type index) {
        return buffer_[index];
    }

    /// @brief 인덱스로 요소에 접근합니다 (검사 없음, const)
    const_reference operator[](size_type index) const {
        return buffer_[index];
    }

    /// @brief 첫 번째 요소를 가져옵니다
    reference front() {
        return buffer_[0];
    }

    /// @brief 첫 번째 요소를 가져옵니다 (const)
    const_reference front() const {
        return buffer_[0];
    }

    /// @brief 마지막 요소를 가져옵니다
    reference back() {
        return buffer_[size_ - 1];
    }

    /// @brief 마지막 요소를 가져옵니다 (const)
    const_reference back() const {
        return buffer_[size_ - 1];
    }

    /// @brief 끝에 요소를 추가합니다
    /// @return 성공하면 true, 가득 차면 false
    bool push_back(const T& value) {
        if (size_ >= CAPACITY) {
            return false;
        }
        buffer_[size_] = value;
        size_++;
        return true;
    }

    /// @brief 마지막 요소를 제거합니다
    void pop_back() {
        if (size_ > 0) {
            size_--;
        }
    }

    /// @brief 모든 요소를 제거합니다
    void clear() {
        size_ = 0;
    }

    /// @brief 기본 데이터에 대한 포인터를 가져옵니다
    pointer data() {
        return buffer_;
    }

    /// @brief 기본 데이터에 대한 포인터를 가져옵니다 (const)
    const_pointer data() const {
        return buffer_;
    }

    /// @brief 정방향 반복자
    pointer begin() {
        return buffer_;
    }

    /// @brief 정방향 반복자 (const)
    const_pointer begin() const {
        return buffer_;
    }

    /// @brief 끝 반복자
    pointer end() {
        return buffer_ + size_;
    }

    /// @brief 끝 반복자 (const)
    const_pointer end() const {
        return buffer_ + size_;
    }

private:
    T buffer_[N];
    size_type size_;
};

} // namespace libemb::util
