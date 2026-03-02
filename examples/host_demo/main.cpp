#include <iostream>
#include "libemb/util/RingBuffer.hpp"
#include "libemb/util/StaticVector.hpp"

using namespace libemb::util;

/**
 * @brief libemb 유틸리티의 호스트 측 데모
 *
 * 이 예제는 호스트 PC에서 RingBuffer와 StaticVector를 보여주며,
 * MCU 하드웨어 없이 핵심 데이터 구조를 시연합니다.
 */

int main() {
    std::cout << "=== libemb 임베디드 라이브러리 - 호스트 데모 ===" << std::endl;
    std::cout << std::endl;

    // 데모 1: RingBuffer
    {
        std::cout << "1. RingBuffer 데모" << std::endl;
        std::cout << "   용량이 8인 링 버퍼 생성 중..." << std::endl;

        RingBuffer<uint8_t, 8> buffer;

        // 요소 추가
        std::cout << "   값 추가: 10, 20, 30, 40" << std::endl;
        buffer.push(10);
        buffer.push(20);
        buffer.push(30);
        buffer.push(40);

        std::cout << "   버퍼 크기: " << buffer.size() << std::endl;
        std::cout << "   버퍼 가득 찬 여부: " << (buffer.full() ? "예" : "아니오") << std::endl;

        // 요소 제거
        std::cout << "   요소 제거: ";
        uint8_t value;
        while (buffer.pop(value)) {
            std::cout << (int)value << " ";
        }
        std::cout << std::endl;
        std::cout << "   버퍼 비어있음: " << (buffer.empty() ? "예" : "아니오") << std::endl;
        std::cout << std::endl;
    }

    // 데모 2: StaticVector
    {
        std::cout << "2. StaticVector 데모" << std::endl;
        std::cout << "   용량이 16인 정적 벡터 생성 중..." << std::endl;

        StaticVector<int, 16> vec;

        // 숫자로 벡터 채우기
        std::cout << "   값 추가: 1, 2, 3, 4, 5" << std::endl;
        for (int i = 1; i <= 5; ++i) {
            vec.push_back(i);
        }

        std::cout << "   벡터 크기: " << vec.size() << std::endl;
        std::cout << "   벡터 용량: " << vec.capacity() << std::endl;
        std::cout << "   첫 번째 요소: " << vec.front() << std::endl;
        std::cout << "   마지막 요소: " << vec.back() << std::endl;

        // 반복 및 출력
        std::cout << "   내용: ";
        for (size_t i = 0; i < vec.size(); ++i) {
            std::cout << vec[i] << " ";
        }
        std::cout << std::endl;

        // 경계 검사로 접근 시도
        try {
            std::cout << "   유효한 인덱스 [2] 접근: " << vec.at(2) << std::endl;
            std::cout << "   유효하지 않은 인덱스 [100] 접근..." << std::endl;
            (void)vec.at(100);
        } catch (const std::out_of_range& e) {
            std::cout << "   예외 발생: " << e.what() << std::endl;
        }
        std::cout << std::endl;
    }

    // 데모 3: RingBuffer 순환
    {
        std::cout << "3. RingBuffer 순환 데모" << std::endl;
        std::cout << "   용량이 4인 작은 링 버퍼 생성 중..." << std::endl;

        RingBuffer<int, 4> rb;

        // 채우기
        std::cout << "   값 추가: 100, 200, 300, 400" << std::endl;
        rb.push(100);
        rb.push(200);
        rb.push(300);
        rb.push(400);

        // 2개 제거
        int val;
        rb.pop(val);
        rb.pop(val);

        // 2개 더 추가 (순환)
        std::cout << "   2개 요소 제거 후 추가: 500, 600" << std::endl;
        rb.push(500);
        rb.push(600);

        // 내용 표시
        std::cout << "   최종 내용: ";
        while (rb.pop(val)) {
            std::cout << val << " ";
        }
        std::cout << std::endl;
        std::cout << std::endl;
    }

    // 데모 4: 다른 데이터 타입
    {
        std::cout << "4. 다양한 타입의 템플릿" << std::endl;

        // Float 벡터
        StaticVector<float, 8> floats;
        floats.push_back(3.14f);
        floats.push_back(2.71f);
        std::cout << "   Float 벡터 크기: " << floats.size() << std::endl;

        // Char 링 버퍼 (문자열 저장)
        RingBuffer<char, 32> chars;
        const char* msg = "Hello";
        for (const char* p = msg; *p; ++p) {
            chars.push(*p);
        }
        std::cout << "   Char 링 버퍼 (문자 큐) 크기: " << chars.size()
                  << " 글자" << std::endl;
        std::cout << std::endl;
    }

    std::cout << "=== 데모 완료 ===" << std::endl;
    std::cout << "이 유틸리티는 다음 특징을 가지는 임베디드 시스템용으로 설계됨:" << std::endl;
    std::cout << "  - 동적 메모리 할당 없음" << std::endl;
    std::cout << "  - 컴파일 타임 용량 경계" << std::endl;
    std::cout << "  - 캐시 친화적인 스택 할당" << std::endl;

    return 0;
}
