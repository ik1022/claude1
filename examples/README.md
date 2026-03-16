# libemb Examples

libemb 라이브러리의 사용 방법을 단계별로 학습합니다.

## 예제 목록

### 01. RingBuffer & StaticVector 기본 사용

📁 [01_basic_ringbuffer](./01_basic_ringbuffer/)

**학습 목표**:
- 고정 크기 데이터 구조 (RingBuffer, StaticVector)
- 메모리 할당 없는 임베디드 데이터 구조
- FIFO 패턴 구현

**난이도**: ⭐ (초급)

**실행**:
```bash
cmake --build build/host --target basic_ringbuffer
./build/host/basic_ringbuffer
```

---

### 02. UART 통신 + RingBuffer

📁 [02_uart_ringbuffer](./02_uart_ringbuffer/)

**학습 목표**:
- 실제 하드웨어 인터페이스와 버퍼 통합
- Non-blocking I/O 패턴
- Mock을 통한 하드웨어 시뮬레이션

**난이도**: ⭐⭐ (중급)

**필수 선행 학습**: 01. RingBuffer 기본 사용

**실행**:
```bash
cmake --build build/host --target uart_ringbuffer_demo
./build/host/uart_ringbuffer_demo
```

---

### 03. Ssd1306 OLED 디스플레이 드라이버

📁 [03_display_driver](./03_display_driver/)

**학습 목표**:
- libemb 드라이버 아키텍처
- HAL 추상화의 실제 사용
- I2C 기반 센서/디바이스 제어
- Mock을 통한 하드웨어 독립적 개발

**난이도**: ⭐⭐⭐ (중상급)

**필수 선행 학습**: 01. RingBuffer, 02. UART 통신

**실행**:
```bash
cmake --build build/host --target display_driver
./build/host/display_driver
```

---

## 빌드 방법

### 모든 예제 빌드

```bash
# 테스트와 함께 빌드
cmake -B build/host -DLIBEMB_BUILD_EXAMPLES=ON -DLIBEMB_BUILD_TESTS=ON
cmake --build build/host

# 모든 예제 실행
./build/host/basic_ringbuffer
./build/host/uart_ringbuffer_demo
./build/host/display_driver
```

### 개별 예제 빌드

```bash
# 특정 예제만 빌드
cmake --build build/host --target display_driver

# 특정 예제만 실행
./build/host/display_driver
```

---

## 학습 경로

```
01. 기본 데이터 구조
    ↓
02. 하드웨어 인터페이스 (UART)
    ↓
03. 드라이버 아키텍처 (I2C 센서)
    ↓
[실제 MCU에 이식]
```

---

## 다음 단계

예제를 모두 학습한 후:

1. **새 드라이버 추가**: [ADDING_DRIVERS.md](../docs/ADDING_DRIVERS.md)
   - BMP280 기압 센서
   - MPU6050 IMU 센서
   - 기타 I2C/SPI 디바이스

2. **MCU 이식**: [PORTING_MCU.md](../docs/PORTING_MCU.md)
   - STM32F4
   - NXP Kinetis
   - Nordic nRF52

3. **아키텍처 이해**: [ARCHITECTURE.md](../docs/ARCHITECTURE.md)
   - libemb 전체 구조
   - 모듈 간 의존성
   - 확장 패턴

---

## 예제 실행 환경

| 환경 | 지원 | 설명 |
|------|------|------|
| **Linux/macOS** | ✅ | GCC/Clang + CMake |
| **Windows** | ✅ | MSVC 또는 MinGW |
| **Host PC** | ✅ | Mock HAL로 완전히 시뮬레이션 |
| **STM32** | 📋 | 이식 가이드 참고 |

---

## 문제 해결

### 빌드 실패

```bash
# 캐시 초기화 후 재빌드
rm -rf build/host
cmake -B build/host -DLIBEMB_BUILD_EXAMPLES=ON
cmake --build build/host
```

### 테스트 실패

```bash
# 상세 출력과 함께 테스트 실행
ctest --test-dir build/host --output-on-failure -VV
```

### AddressSanitizer로 메모리 검사

```bash
cmake -B build/asan -DLIBEMB_BUILD_EXAMPLES=ON -DLIBEMB_ENABLE_ASAN=ON
cmake --build build/asan
./build/asan/display_driver
```

---

## 문의 및 피드백

예제 개선에 대한 의견:
- 문서: 명확하지 않은 부분
- 코드: 더 나은 설명이 필요한 부분
- 추가: 다루었으면 하는 주제

[GitHub Issues](https://github.com/libemb/libemb/issues)에서 피드백 부탁드립니다.
