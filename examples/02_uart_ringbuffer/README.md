# 02_uart_ringbuffer - UART 통신과 RingBuffer 통합

## 목표

이 예제는 실제 직렬 통신에서 RingBuffer를 활용하는 방법을 보여줍니다:
- Non-blocking UART 인터페이스 구현
- RingBuffer를 사용한 버퍼링
- Mock UART로 테스트

## 아키텍처

```
Application
    ↓
NonBlockingUart (UART + RingBuffer)
    ├─ TX: 송신 버퍼
    └─ RX: 수신 버퍼
    ↓
Mock UART (하드웨어 시뮬레이션)
```

## 주요 구성 요소

### NonBlockingUart 클래스
```cpp
class NonBlockingUart {
private:
    libemb::util::RingBuffer<uint8_t, TX_SIZE> tx_buffer_;
    libemb::util::RingBuffer<uint8_t, RX_SIZE> rx_buffer_;
    MockUart uart_;

public:
    void send(const uint8_t* data, size_t len);
    bool receive(uint8_t* data, size_t* len);
};
```

**특징**:
- 송수신 분리 (독립적인 버퍼)
- Non-blocking I/O
- 고정 메모리 사용

## 코드 실행

```bash
cmake --build build/host

# 예제 실행
./build/host/uart_ringbuffer_demo
```

## 예상 출력

```
=== Non-Blocking UART Demo ===
Sending: "Hello UART"
TX Buffer usage: 10 bytes

Simulating data reception...
RX Buffer: "Received"
RX Buffer usage: 8 bytes

=== Ring Buffer Efficiency ===
Wrapping test: circular buffer works correctly
No memory allocation in runtime!
```

## 학습 포인트

1. **Real-world Pattern**
   - UART는 하드웨어 인터페이스
   - 버퍼링으로 데이터 흐름 조절
   - 인터럽트 기반 구현 가능 (Mock으로 시뮬레이션)

2. **Non-blocking I/O**
   ```cpp
   uart.send(data);      // 버퍼에 추가
   // ... 다른 작업 수행
   uart.processTx();     // 백그라운드 처리
   ```

3. **메모리 안정성**
   - 버퍼 오버플로우 방지
   - 예측 가능한 메모리

## 하드웨어 이식

이 코드를 실제 STM32에 이식하려면:

```cpp
// port/stm32f4/hal/Stm32f4Uart.hpp
class Stm32f4Uart : public libemb::hal::IUart {
    // STM32 HAL 기반 구현
};

// 사용 방식은 동일
NonBlockingUart<Stm32f4Uart> uart(&stm32_uart);
```

## 다음 단계

- [03. Display Driver](../03_display_driver/) - I2C 센서/디바이스 제어
- [MCU 포팅 가이드](../../docs/PORTING_MCU.md) - 실제 MCU 이식 방법
