# CLAUDE.md

**libemb** 저장소에서 Claude Code를 사용할 때의 지침.

## 프로젝트 개요

**libemb**는 MCU 기반 시스템을 위해 설계된 현대적인 임베디드 C++ 펌웨어 라이브러리입니다.

- **언어**: C++17
- **빌드 시스템**: CMake 3.20+
- **표준 라이브러리**: RTOS 의존성 없음
- **목적**: 재사용 가능한 HAL 추상화, 데이터 구조, 다중 MCU용 드라이버

## 아키텍처

### 핵심 모듈

| 모듈 | 타입 | 목적 |
|--------|------|---------|
| `libemb::hal` | INTERFACE | 하드웨어 추상화 계층 - 순수 가상 인터페이스 |
| `libemb::util` | INTERFACE (헤더 온리) | 데이터 구조: RingBuffer, StaticVector |
| `libemb::driver` | STATIC 라이브러리 | 장치 드라이버: Ssd1306 OLED 디스플레이 |
| `libemb::port::mock` | INTERFACE | 단위 테스트용 Mock HAL 구현 |
| `libemb::protocol` | INTERFACE | 향후 프로토콜 구현 (CAN, MQTT 등) |

### 설계 철학

1. **동적 할당 금지**: 모든 버퍼와 컨테이너는 고정 크기 사용
2. **하드웨어 독립성**: 드라이버는 HAL 인터페이스에만 의존, MCU 세부사항 배제
3. **헤더 온리 유틸리티**: 템플릿과 간단한 데이터 구조는 인라인 컴파일
4. **테스트 우선 설계**: Mock 구현으로 하드웨어 없이 PC에서 테스트 가능
5. **최소한의 런타임**: 예외 없음, 임베디드 빌드에서 RTTI 비활성화

## 디렉토리 구조

```
.
├── CMakeLists.txt                 # 루트 CMake 설정
├── CLAUDE.md                       # 이 파일
├── cmake/
│   └── toolchains/
│       └── arm-none-eabi.cmake    # ARM Cortex-M 크로스컴파일 툴체인
├── include/libemb/
│   ├── hal/                       # HAL 인터페이스 (IGpio, IUart, ISpi, II2c)
│   ├── util/                      # 유틸리티 (RingBuffer, StaticVector)
│   ├── driver/                    # 장치 드라이버 헤더 (Ssd1306)
│   └── protocol/                  # 프로토콜 인터페이스 (향후)
├── src/
│   ├── driver/                    # 드라이버 구현
│   └── protocol/                  # 프로토콜 구현
├── port/
│   └── mock/                      # 테스트용 Mock HAL 구현
├── test/
│   └── unit/                      # Google Test 단위 테스트
├── examples/
│   └── host_demo/                 # 호스트 PC 데모 앱
└── .gitignore
```

## 빌드

### 사전 요구사항

- **호스트 (Linux/macOS)**: GCC/Clang, CMake 3.20+
- **크로스컴파일 (STM32 등)**: `arm-none-eabi-gcc` 툴체인

### 테스트와 함께 호스트 빌드

```bash
# 모든 테스트와 예제 빌드
cmake -B build/host -DLIBEMB_BUILD_TESTS=ON -DLIBEMB_BUILD_EXAMPLES=ON
cmake --build build/host

# 테스트 실행
ctest --test-dir build/host --output-on-failure

# 데모 실행
./build/host/examples/host_demo
```

### AddressSanitizer 포함 빌드 (메모리 안전성 검사)

```bash
cmake -B build/host_asan -DLIBEMB_BUILD_TESTS=ON -DLIBEMB_ENABLE_ASAN=ON
cmake --build build/host_asan
ctest --test-dir build/host_asan --output-on-failure
```

### 최소 빌드 (테스트 및 예제 제외)

```bash
cmake -B build/minimal
cmake --build build/minimal
```

### ARM 크로스컴파일 (STM32F4 등)

```bash
cmake -B build/stm32 \
  -DCMAKE_TOOLCHAIN_FILE=cmake/toolchains/arm-none-eabi.cmake \
  -DLIBEMB_BUILD_TESTS=OFF

cmake --build build/stm32

# 바이너리 크기 분석
arm-none-eabi-size build/stm32/<firmware.elf>
```

## 코딩 규칙

### C++ 표준
- C++17 기능 사용 (std::optional, std::variant, 구조화된 바인딩 허용)
- `const`, `constexpr`, move semantics 선호
- 임베디드 코드에서 예외 금지; 에러 코드 또는 assert 사용

### 이름 지정
- **네임스페이스**: `libemb::hal`, `libemb::driver` 등
- **클래스**: PascalCase (예: `RingBuffer`, `Ssd1306`)
- **함수**: camelCase (예: `setPixel`, `getState`)
- **상수**: UPPER_CASE (예: `DISPLAY_WIDTH = 128`)
- **Private 멤버**: 뒤에 언더스코어가 붙는 snake_case (예: `i2c_`, `frame_buffer_`)

### 인터페이스
- 순수 가상 인터페이스는 `I`로 시작 (예: `IGpio`, `IUart`)
- 기본 소멸자 `virtual ~IClass() = default;`

### 메모리 관리
- **`new`/`delete` 금지**: 스택 할당 또는 `std::array` 사용
- **`malloc`/`free` 금지**: HAL 구현에서도 사용 금지
- **정적 크기 지정**: 버퍼 크기에 컴파일 타임 상수 사용

### 문서화
- 공개 API에 Doxygen 스타일 주석
- `@brief`, `@param`, `@return` 태그 사용
- 명확하지 않은 로직에만 코드 주석 작성

## 테스트 전략

### 단위 테스트
- **프레임워크**: Google Test (v1.14.0)
- **위치**: `test/unit/`
- **Mock HAL**: `port/mock/MockGpio.hpp`, `port/mock/MockUart.hpp`
- **테스트된 드라이버**: MockI2c를 사용한 Ssd1306
- **테스트된 유틸리티**: RingBuffer, StaticVector

### 테스트 실행
```bash
# 모든 테스트
ctest --test-dir build/host --output-on-failure

# 특정 테스트
ctest --test-dir build/host -R test_RingBuffer --output-on-failure

# 상세 출력
ctest --test-dir build/host -VV
```

### 새로운 테스트 작성
1. `test/unit/test_YourModule.cpp` 생성
2. `<gtest/gtest.h>` 및 모듈 헤더 포함
3. setup/teardown을 위해 fixture 클래스 사용
4. `test/unit/CMakeLists.txt`에 `add_libemb_test()`로 등록

## 새로운 드라이버 추가

예제: 새로운 I2C 센서 드라이버 추가

1. **헤더 생성**: `include/libemb/driver/MySensor.hpp`
   - `libemb::hal::II2c`에만 의존 (특정 MCU 제외)
   - 버퍼에 `libemb::util::StaticVector` 사용

2. **구현 생성**: `src/driver/MySensor.cpp`

3. **CMakeLists.txt 항목 추가**: `src/driver/CMakeLists.txt` 업데이트
   - 소스 파일을 `target_sources()`에 추가

4. **테스트 생성**: `test/unit/test_MySensor.cpp`
   - 테스트 파일에서 `MockI2c` 사용
   - 하드웨어와 독립적으로 드라이버 로직 테스트

## 새로운 MCU로 포팅

### 단계
1. MCU 포트 디렉토리 생성: `port/stm32f4/`
2. HAL 인터페이스 구현 (GPIO, UART, SPI, I2C)
3. 조건부 컴파일을 위해 `CMakeLists.txt` 업데이트
4. 툴체인을 MCU 특화 컴파일러 플래그로 설정

### 예제 (STM32F4)
```cpp
// port/stm32f4/Stm32f4Gpio.hpp
#include "libemb/hal/IGpio.hpp"
#include "stm32f4xx.h"  // ST HAL

class Stm32f4Gpio : public libemb::hal::IGpio {
    // MCU 특화 구현
};
```

## 성능 고려사항

- **힙 할당 없음**: 실시간 예측 가능성, 단편화 없음
- **스택 전용 설계**: 컴파일 타임에 고정된 메모리 사용량
- **인라인 템플릿**: 유틸리티의 제로 코스트 추상화
- **ARM 컴파일 플래그**: 툴체인에서 LTO, section GC 활성화
