# 01_basic_ringbuffer - RingBuffer & StaticVector 기본 사용법

## 목표

이 예제는 libemb 유틸리티 라이브러리의 핵심 데이터 구조를 소개합니다:
- **`libemb::util::RingBuffer`** - FIFO 순환 버퍼 (고정 크기)
- **`libemb::util::StaticVector`** - 동적 배열과 유사하지만 메모리 할당 없음

## 핵심 개념

### RingBuffer (순환 버퍼)
```
고정 크기 메모리에서 FIFO 패턴 구현
┌─────┬─────┬─────┬─────┐
│  ?  │  ?  │  ?  │  ?  │  ← 크기 4
└─────┴─────┴─────┴─────┘
 ▲
 head/tail
```

**장점**:
- 예측 가능한 메모리 사용량
- O(1) push/pop 성능
- 임베디드 시스템에 최적

### StaticVector (고정 크기 벡터)
```
배열 같은 인터페이스, 동적 할당 없음
┌─────┬─────┬─────┬─────┐
│  1  │  2  │  3  │  -  │  ← 사용 3/4
└─────┴─────┴─────┴─────┘
```

## 코드 실행

```bash
cd /home/user/claude1
cmake --build build/host

# 예제 실행
./build/host/basic_ringbuffer
```

## 예상 출력

```
=== RingBuffer Example ===
Pushing values: 10, 20, 30, 40
Queue content: [10, 20, 30, 40]
Popping: 10
Queue content: [20, 30, 40]

=== StaticVector Example ===
Vector content: [100, 200, 300]
Vector size: 3 / capacity: 10
```

## 학습 포인트

1. **고정 크기의 중요성**
   - 임베디드 시스템: 힙 메모리 없음
   - 예측 가능한 성능 (리얼타임)

2. **메모리 효율성**
   - 스택 할당 (RAII)
   - 단편화 없음

3. **사용 패턴**
   ```cpp
   libemb::util::RingBuffer<uint8_t, 8> buffer;
   buffer.push(42);
   uint8_t value;
   buffer.pop(value);
   ```

## 다음 단계

- [02. UART + RingBuffer](../02_uart_ringbuffer/) - 실제 직렬 통신에서 사용
- [03. Display Driver](../03_display_driver/) - HAL과 드라이버 통합
