# libemb Architecture Guide

## 계층 구조

libemb는 하드웨어 독립적인 임베디드 라이브러리입니다. 다음과 같은 계층 구조를 따릅니다:

### Level 1: Public API (include/libemb/)

사용자가 직접 사용하는 인터페이스와 데이터 구조

- **hal/**: 하드웨어 추상화 계층 (순수 가상 인터페이스)
  - `IGpio.hpp`: GPIO 제어
  - `IUart.hpp`: 직렬 통신
  - `ISpi.hpp`: SPI 통신
  - `II2c.hpp`: I2C 통신

- **util/**: 재사용 가능한 데이터 구조 (헤더 온리)
  - `RingBuffer.hpp`: 고정 크기 순환 버퍼
  - `StaticVector.hpp`: 동적 할당 없는 벡터

- **driver/**: 장치 드라이버 인터페이스
  - `ssd1306/`: Ssd1306 OLED 디스플레이 드라이버

- **protocol/**: 통신 프로토콜 (향후 확장)
  - CAN, MQTT, Modbus 등

### Level 2: Implementation (src/)

공개 인터페이스의 구체적인 구현

- **driver/**: 드라이버 구현
  - `ssd1306/Ssd1306.cpp`: Ssd1306 구현

- **protocol/**: 프로토콜 구현

### Level 3: Ports (port/)

MCU별 HAL 구현

- **mock/**: 호스트 테스트용 Mock 구현
  - `hal/MockGpio.hpp`: GPIO 모의 구현
  - `hal/MockI2c.hpp`: I2C 모의 구현
  - `hal/MockUart.hpp`: UART 모의 구현

- **stm32f4/** (향후): STM32F4 구체적 구현
- **nrf52/** (향후): Nordic nRF52 구체적 구현

### Level 4: Tests (test/)

유닛 테스트 및 검증

- **unit/**: Google Test 기반 단위 테스트
  - `util/`: RingBuffer, StaticVector 테스트
  - `driver/`: 드라이버 테스트

### Level 5: Examples (examples/)

사용법 예시

- `01_basic_ringbuffer/`: 기본 데이터 구조
- `02_uart_ringbuffer/`: UART 통합
- `03_display_driver/`: I2C 드라이버 사용

## 의존성 관계

```
Application Layer
    ↓
┌─────────────────────────────────────┐
│   Public API (include/libemb/)      │
├─────────────────────────────────────┤
│ ┌──────────┐ ┌──────────┐ ┌───────┐ │
│ │   hal/   │ │  util/   │ │driver/│ │
│ └──────────┘ └──────────┘ └───────┘ │
└─────────────────────────────────────┘
         ↓          ↓         ↓
┌─────────────────────────────────────┐
│  Implementation (src/)               │
├─────────────────────────────────────┤
│         ┌─────────────────┐         │
│         │ src/driver/...  │         │
│         └─────────────────┘         │
└─────────────────────────────────────┘
         ↓
┌─────────────────────────────────────┐
│   Platform (port/)                  │
├─────────────────────────────────────┤
│ ┌────────────┐   ┌─────────────────┐│
│ │mock/       │   │stm32f4/...      ││
│ └────────────┘   └─────────────────┘│
└─────────────────────────────────────┘
         ↓
   Hardware Layer
```

## 모듈 간 의존성 관계

### hal/ (독립적)
- ✅ 다른 모듈에 의존하지 않음
- 순수 인터페이스만 정의
- 모든 곳에서 참조 가능

### util/ (독립적)
- ✅ hal/에 의존하지 않음
- 일반적인 데이터 구조
- 모든 곳에서 사용 가능

### driver/
- ❌ hal/에 **의존** (하드웨어 제어 필요)
- ❌ util/에 **의존** (버퍼 관리)
- ✅ 특정 MCU에 의존하지 않음
- 테스트: Mock hal/ 사용

### port/mock (테스트용)
- ❌ hal/을 **구현**
- ✅ 실제 하드웨어 제어 없음
- ✅ 호스트 환경에서만 사용

### port/stm32f4 (MCU 포팅)
- ❌ hal/을 **구현**
- ✅ STM32 하드웨어 제어
- ✅ 크로스컴파일 환경

## 설계 원칙

### 1. 하드웨어 독립성
드라이버와 애플리케이션은 구체적인 MCU에 의존하지 않습니다.
```cpp
// 호스트 환경
Ssd1306 display;
display.init(&mock_i2c);

// STM32 환경
Ssd1306 display;
display.init(&stm32_i2c);
// 같은 코드!
```

### 2. 정적 메모리 할당
동적 메모리 할당 금지:
```cpp
// ❌ 금지
std::vector<uint8_t> buffer;

// ✅ 추천
libemb::util::StaticVector<uint8_t, 256> buffer;
libemb::util::RingBuffer<uint8_t, 256> buffer;
```

### 3. 제로 오버헤드 추상화
헤더 온리 유틸리티는 인라인으로 컴파일되어 성능 손실 없음

### 4. 테스트 주도 설계
모든 드라이버는 Mock HAL로 PC에서 테스트 가능

## 빌드 시스템

### CMake 계층화

```
CMakeLists.txt (루트)
├── include/libemb/hal/CMakeLists.txt
├── include/libemb/util/CMakeLists.txt
├── src/driver/CMakeLists.txt
│   └── add_libemb_driver(ssd1306)
├── port/mock/CMakeLists.txt
├── test/unit/CMakeLists.txt
│   └── add_libemb_test(...)
└── examples/CMakeLists.txt
    └── add_libemb_example(...)
```

### 편리한 매크로 (cmake/LibembConfig.cmake)

```cmake
# 드라이버 추가
add_libemb_driver(ssd1306)

# 테스트 추가
add_libemb_test(test_Ssd1306 test_Ssd1306.cpp)

# 예제 추가
add_libemb_example(demo main.cpp libemb::driver)
```

## 확장 가이드

### 새 드라이버 추가
1. `include/libemb/driver/<name>/` - 헤더
2. `src/driver/<name>/` - 구현
3. `test/unit/driver/test_<name>.cpp` - 테스트
4. `src/driver/CMakeLists.txt`에 `add_libemb_driver(<name>)` 추가

상세: [ADDING_DRIVERS.md](./ADDING_DRIVERS.md)

### 새 MCU 포팅
1. `port/<mcu>/hal/` - HAL 구현
2. `cmake/toolchains/<mcu>.cmake` - 도구체인
3. 기존 드라이버는 변경 없음

상세: [PORTING_MCU.md](./PORTING_MCU.md)

## 성능 특성

| 특성 | 값 | 비고 |
|------|-----|------|
| **힙 할당** | 0 | 모든 메모리는 컴파일 타임에 할당 |
| **메모리 예측** | ✅ 정확 | 크기 변동 없음 |
| **실시간성** | ✅ 좋음 | 동적 할당 없음 |
| **추상화 비용** | 최소 | 헤더 온리 + 인라인 |
| **코드 크기** | 작음 | MCU 최적화 |

## 더 알아보기

- [예제](../examples/README.md) - 실제 사용 방법
- [드라이버 추가](./ADDING_DRIVERS.md) - 새 드라이버 작성
- [MCU 포팅](./PORTING_MCU.md) - 새 MCU 지원
