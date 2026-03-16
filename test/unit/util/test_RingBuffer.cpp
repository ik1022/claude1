#include <gtest/gtest.h>
#include "libemb/util/RingBuffer.hpp"

using namespace libemb::util;

class RingBufferUint8Test : public ::testing::Test {
protected:
    RingBuffer<uint8_t, 8> buffer;
};

TEST_F(RingBufferUint8Test, InitiallyEmpty) {
    // 초기에 비어있어야 합니다
    EXPECT_TRUE(buffer.empty());
    EXPECT_EQ(buffer.size(), 0);
    EXPECT_FALSE(buffer.full());
}

TEST_F(RingBufferUint8Test, PushAndPop) {
    // 추가 및 제거 테스트
    EXPECT_TRUE(buffer.push(42));
    EXPECT_EQ(buffer.size(), 1);
    EXPECT_FALSE(buffer.empty());

    uint8_t value;
    EXPECT_TRUE(buffer.pop(value));
    EXPECT_EQ(value, 42);
    EXPECT_TRUE(buffer.empty());
}

TEST_F(RingBufferUint8Test, PopFromEmpty) {
    // 비어있는 버퍼에서 제거 시도
    uint8_t value;
    EXPECT_FALSE(buffer.pop(value));
}

TEST_F(RingBufferUint8Test, FillAndEmpty) {
    // 버퍼를 가득 찰 때까지 채우기
    for (int i = 0; i < 8; ++i) {
        EXPECT_TRUE(buffer.push(i));
    }
    EXPECT_TRUE(buffer.full());
    EXPECT_EQ(buffer.size(), 8);

    // 가득 찼을 때 추가 시도
    EXPECT_FALSE(buffer.push(99));

    // 모든 요소 제거
    uint8_t value;
    for (int i = 0; i < 8; ++i) {
        EXPECT_TRUE(buffer.pop(value));
        EXPECT_EQ(value, i);
    }
    EXPECT_TRUE(buffer.empty());
}

TEST_F(RingBufferUint8Test, Peek) {
    // 미리보기 기능 테스트
    buffer.push(42);
    buffer.push(99);

    uint8_t value;
    EXPECT_TRUE(buffer.peek(value));
    EXPECT_EQ(value, 42);

    // 미리보기는 요소를 제거하면 안 됨
    EXPECT_EQ(buffer.size(), 2);
    EXPECT_TRUE(buffer.peek(value));
    EXPECT_EQ(value, 42);
}

TEST_F(RingBufferUint8Test, PeekEmpty) {
    // 빈 버퍼에서 미리보기 시도
    uint8_t value;
    EXPECT_FALSE(buffer.peek(value));
}

TEST_F(RingBufferUint8Test, Clear) {
    // 버퍼 정리 테스트
    for (int i = 0; i < 5; ++i) {
        buffer.push(i);
    }
    EXPECT_EQ(buffer.size(), 5);

    buffer.clear();
    EXPECT_TRUE(buffer.empty());
    EXPECT_EQ(buffer.size(), 0);
}

TEST_F(RingBufferUint8Test, WrapAround) {
    // 순환 테스트
    // 버퍼 채우기
    for (int i = 0; i < 8; ++i) {
        buffer.push(i);
    }

    // 처음 4개 요소 제거
    uint8_t value;
    for (int i = 0; i < 4; ++i) {
        buffer.pop(value);
    }

    // 4개 더 추가 (순환)
    for (int i = 8; i < 12; ++i) {
        EXPECT_TRUE(buffer.push(i));
    }

    // 내용 검증
    for (int i = 4; i < 12; ++i) {
        EXPECT_TRUE(buffer.pop(value));
        EXPECT_EQ(value, i);
    }
}

class RingBufferIntTest : public ::testing::Test {
protected:
    RingBuffer<int, 4> buffer;
};

TEST_F(RingBufferIntTest, SignedIntegers) {
    // 부호 있는 정수 테스트
    EXPECT_TRUE(buffer.push(-10));
    EXPECT_TRUE(buffer.push(100));
    EXPECT_TRUE(buffer.push(-999));

    int value;
    EXPECT_TRUE(buffer.pop(value));
    EXPECT_EQ(value, -10);
    EXPECT_TRUE(buffer.pop(value));
    EXPECT_EQ(value, 100);
    EXPECT_TRUE(buffer.pop(value));
    EXPECT_EQ(value, -999);
}
