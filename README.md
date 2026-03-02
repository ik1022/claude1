# libemb - 임베디드 C++ 펌웨어 라이브러리

**libemb**는 MCU 기반 임베디드 시스템을 위한 현대적인 C++17 펌웨어 라이브러리입니다. 하드웨어 추상화, 재사용 가능한 드라이버, 메모리 효율적인 데이터 구조를 제공합니다.

## 📋 목차

- [주요 특징](#주요-특징)
- [시스템 요구사항](#시스템-요구사항)
- [설치 및 빌드](#설치-및-빌드)
- [빠른 시작](#빠른-시작)
- [프로젝트 구조](#프로젝트-구조)
- [모듈 설명](#모듈-설명)
- [사용 예제](#사용-예제)
- [테스트 실행](#테스트-실행)
- [MCU 포팅](#mcu-포팅)
- [FAQ](#faq)

---

## 주요 특징

✨ **C++17 현대식 설계**
- STL 호환 템플릿 (std::optional, std::array)
- constexpr, move semantics 활용
- 예외 및 RTTI 제거 가능 (임베디드 최적화)

🎯 **하드웨어 무관 설계**
- 순수 가상 인터페이스 (IGpio, IUart, ISpi, II2c)
- 드라이버는 HAL에만 의존
- MCU 독립적인 로직 재사용

💾 **메모리 효율성**
- 동적 할당 없음 (전체 프로젝트)
- 컴파일 타임 크기 결정
- 스택 기반 할당으로 예측 가능한 메모리 사용

🧪 **테스트 가능한 구조**
- Mock HAL 구현으로 PC에서 단위 테스트
- Google Test 프레임워크 통합
- 실제 하드웨어 없이 드라이버 검증

🔧 **크로스 플랫폼 빌드**
- CMake 3.20+ 기반 (표준화된 빌드)
- ARM Cortex-M 크로스 컴파일 완전 지원 (bare-metal, 라이브러리 불필요)
- AddressSanitizer로 메모리 안전성 검사
- 최소 바이너리 크기 (driver: 1.4KB text)

---

## 시스템 요구사항

### 호스트 빌드 (테스트/개발용)

**macOS / Linux:**
```bash
# Homebrew (macOS)
brew install cmake gcc clang

# Ubuntu/Debian
sudo apt-get install cmake build-essential
```

**Windows (WSL2 권장):**
```bash
# WSL2에서 Linux 명령어 사용 가능
wsl --install Ubuntu
sudo apt-get install cmake build-essential
```

**필수 버전:**
- CMake: 3.20 이상
- C++ 컴파일러: GCC 9+ 또는 Clang 10+
- Python 3.6+ (테스트 실행용)

### MCU 크로스 컴파일용

```bash
# ARM Cortex-M 툴체인 설치 (예: STM32)
brew install arm-none-eabi-gcc  # macOS

# Ubuntu/Debian
sudo apt-get install gcc-arm-none-eabi
```

---

## 설치 및 빌드

### 1️⃣ 저장소 클론

```bash
git clone <repository-url> libemb
cd libemb
```

### 2️⃣ 호스트 빌드 (테스트 포함)

```bash
# 빌드 디렉토리 생성 및 CMake 설정
cmake -B build/host \
  -DCMAKE_BUILD_TYPE=Release \
  -DLIBEMB_BUILD_TESTS=ON \
  -DLIBEMB_BUILD_EXAMPLES=ON

# 빌드 실행
cmake --build build/host

# 결과 확인
ls -lh build/host/test/unit/
```

### 3️⃣ 메모리 안전성 검사 포함 빌드

```bash
cmake -B build/host_asan \
  -DCMAKE_BUILD_TYPE=Debug \
  -DLIBEMB_BUILD_TESTS=ON \
  -DLIBEMB_ENABLE_ASAN=ON

cmake --build build/host_asan
```

### 4️⃣ ARM Cortex-M 크로스 컴파일

ARM bare-metal 환경에서 빌드합니다. libemb는 표준 C 라이브러리 없이도 컴파일됩니다:

```bash
cmake -B build/stm32 \
  -DCMAKE_TOOLCHAIN_FILE=cmake/toolchains/arm-none-eabi.cmake \
  -DCMAKE_BUILD_TYPE=MinSizeRel \
  -DLIBEMB_BUILD_TESTS=OFF

cmake --build build/stm32

# 바이너리 크기 확인
arm-none-eabi-size build/stm32/src/driver/liblibemb_driver.a
```

**특징:**
- ✅ C++ 표준 라이브러리 불필요 (bare-metal 헤더 사용)
- ✅ 작은 바이너리 크기 (driver: 1.4KB text)
- ✅ 예외/RTTI 제거로 최소 오버헤드
- ✅ ARM Cortex-M4 (STM32F4, nRF52 등) 호환

---

## 빠른 시작

### 예제 1: RingBuffer 사용

```cpp
#include "libemb/util/RingBuffer.hpp"

using namespace libemb::util;

int main() {
    // 8개 요소를 저장할 수 있는 링 버퍼 생성
    RingBuffer<uint8_t, 8> buffer;

    // 데이터 저장
    buffer.push(42);
    buffer.push(99);

    // 데이터 꺼내기
    uint8_t value;
    while (buffer.pop(value)) {
        printf("Value: %d\n", value);
    }

    return 0;
}
```

### 예제 2: StaticVector 사용

```cpp
#include "libemb/util/StaticVector.hpp"

using namespace libemb::util;

int main() {
    // 최대 16개 요소를 저장할 수 있는 정적 벡터
    StaticVector<int, 16> vector;

    // 요소 추가
    vector.push_back(10);
    vector.push_back(20);
    vector.push_back(30);

    // 반복문으로 순회
    for (size_t i = 0; i < vector.size(); ++i) {
        printf("Element[%zu] = %d\n", i, vector[i]);
    }

    return 0;
}
```

### 예제 3: HAL 인터페이스 구현

```cpp
#include "libemb/hal/IGpio.hpp"

// MCU 특화 GPIO 구현
class Stm32Gpio : public libemb::hal::IGpio {
private:
    uint32_t pin_;
    GPIO_TypeDef* port_;

public:
    Stm32Gpio(GPIO_TypeDef* port, uint32_t pin)
        : pin_(pin), port_(port) {}

    void setMode(GpioMode mode) override {
        // STM32 레지스터 설정
    }

    void write(GpioState state) override {
        if (state == GpioState::HIGH) {
            HAL_GPIO_WritePin(port_, pin_, GPIO_PIN_SET);
        } else {
            HAL_GPIO_WritePin(port_, pin_, GPIO_PIN_RESET);
        }
    }

    GpioState read() const override {
        GPIO_PinState pin_state = HAL_GPIO_ReadPin(port_, pin_);
        return (pin_state == GPIO_PIN_SET)
            ? GpioState::HIGH
            : GpioState::LOW;
    }

    void toggle() override {
        HAL_GPIO_TogglePin(port_, pin_);
    }
};
```

### 예제 4: 드라이버와 HAL 연결

```cpp
#include "libemb/driver/Ssd1306.hpp"
#include "libemb/hal/II2c.hpp"

// I2C 인터페이스 구현 (STM32 예시)
class Stm32I2c : public libemb::hal::II2c {
    // 구현...
};

int main() {
    // I2C 및 OLED 초기화
    Stm32I2c i2c;
    libemb::driver::Ssd1306 display;

    if (!display.init(&i2c, 0x3C)) {
        return -1;
    }

    // 화면 지우기
    display.clear();

    // 사각형 그리기
    display.drawRect(10, 10, 50, 30, true);

    // 화면에 반영
    display.display();

    return 0;
}
```

---

## 프로젝트 구조

```
libemb/
├── CMakeLists.txt                  # 루트 빌드 설정
├── CLAUDE.md                       # 개발자 가이드
├── README.md                       # 이 파일 (사용 설명서)
│
├── cmake/
│   └── toolchains/
│       └── arm-none-eabi.cmake     # ARM Cortex-M 크로스 컴파일 설정
│
├── include/libemb/
│   ├── hal/                        # 하드웨어 추상화 계층 (인터페이스)
│   │   ├── IGpio.hpp               # GPIO 인터페이스
│   │   ├── IUart.hpp               # UART 인터페이스
│   │   ├── ISpi.hpp                # SPI 인터페이스
│   │   ├── II2c.hpp                # I2C 인터페이스
│   │   └── CMakeLists.txt
│   │
│   ├── util/                       # 유틸리티 (헤더 온리)
│   │   ├── RingBuffer.hpp          # 링 버퍼
│   │   ├── StaticVector.hpp        # 정적 벡터
│   │   └── CMakeLists.txt
│   │
│   ├── driver/                     # 드라이버
│   │   ├── Ssd1306.hpp             # OLED 디스플레이 드라이버
│   │   └── (향후 추가 가능)
│   │
│   ├── protocol/                   # 프로토콜 (향후 CAN, MQTT 등)
│   │
│   └── bare-metal/                 # Bare-metal 크로스컴파일 헤더
│       ├── stdint.h                # 정수 타입 정의
│       ├── stddef.h                # 표준 정의 (size_t, NULL)
│       └── assert.h                # 단언 매크로
│
├── src/
│   ├── driver/
│   │   ├── Ssd1306.cpp             # OLED 드라이버 구현
│   │   └── CMakeLists.txt
│   │
│   └── protocol/
│       └── CMakeLists.txt
│
├── port/                           # MCU 특화 포트
│   └── mock/                       # 테스트용 Mock 구현
│       ├── MockGpio.hpp
│       ├── MockUart.hpp
│       └── CMakeLists.txt
│
├── test/
│   ├── CMakeLists.txt              # Google Test 설정
│   └── unit/
│       ├── test_RingBuffer.cpp     # 링 버퍼 단위 테스트
│       ├── test_Ssd1306.cpp        # OLED 드라이버 테스트
│       └── CMakeLists.txt
│
└── examples/
    ├── CMakeLists.txt
    ├── host_demo/
    │   ├── main.cpp                # 호스트 데모 애플리케이션
    │   └── CMakeLists.txt
    └── uart_ringbuffer/            # Non-blocking UART 예제
        ├── main.cpp                # UART 예제 코드
        ├── NonBlockingUart.hpp      # Non-blocking UART 드라이버
        └── CMakeLists.txt
```

---

## 모듈 설명

### 🔌 HAL (Hardware Abstraction Layer)

임베디드 시스템의 핵심 하드웨어 인터페이스를 정의합니다.

**모듈 위치:** `include/libemb/hal/`

**포함 인터페이스:**
- `IGpio` - GPIO 핀 제어 (READ, WRITE, TOGGLE)
- `IUart` - UART 직렬 통신
- `ISpi` - SPI 동기 직렬 인터페이스
- `II2c` - I2C 동기 직렬 인터페이스

**특징:**
- 순수 가상 클래스 (구현 없음)
- MCU 독립적
- 여러 MCU에서 동일 인터페이스로 호환

**CMake 타겟:** `libemb::hal`

```cpp
// 드라이버는 이런 식으로 HAL 사용
class MyDriver {
    libemb::hal::IGpio* gpio_;

    void initialize(libemb::hal::IGpio* gpio) {
        gpio_ = gpio;  // MCU 특화 구현 주입
        gpio_->setMode(libemb::hal::GpioMode::OUTPUT);
    }
};
```

### 📦 Utility (유틸리티)

동적 할당 없이 작동하는 고효율 데이터 구조입니다.

**모듈 위치:** `include/libemb/util/`

#### RingBuffer - 링 버퍼

FIFO(First-In-First-Out) 원형 버퍼로, 고정된 크기로 할당됩니다.

```cpp
#include "libemb/util/RingBuffer.hpp"

libemb::util::RingBuffer<uint8_t, 256> rxBuffer;

// 수신 인터럽트 핸들러
void uart_interrupt() {
    uint8_t data = UART_READ();
    rxBuffer.push(data);  // 원형 버퍼에 추가
}

// 메인 루프
void main_loop() {
    uint8_t byte;
    if (rxBuffer.pop(byte)) {
        process_command(byte);
    }
}
```

**주요 메서드:**
| 메서드 | 설명 | 반환값 |
|--------|------|--------|
| `push(value)` | 끝에 요소 추가 | bool (성공/실패) |
| `pop(value)` | 앞에서 요소 제거 | bool |
| `peek(value)` | 앞 요소 조회 (제거 없음) | bool |
| `size()` | 현재 요소 수 | size_t |
| `full()` | 버퍼 가득 참 여부 | bool |
| `empty()` | 버퍼 비어있음 여부 | bool |
| `clear()` | 모든 요소 제거 | void |

**성능:**
- 시간복잡도: O(1) for all operations
- 공간: 고정 (할당 시점 결정)
- 싱글 프로듀서/컨슈머: 락 불필요

#### StaticVector - 정적 벡터

std::vector와 유사하지만 스택 기반 고정 크기입니다.

```cpp
#include "libemb/util/StaticVector.hpp"

libemb::util::StaticVector<sensor_reading_t, 100> readings;

// 센서 데이터 수집
void collect_data() {
    sensor_reading_t data = read_sensor();
    if (!readings.full()) {
        readings.push_back(data);
    }
}

// 데이터 처리
void process_readings() {
    for (size_t i = 0; i < readings.size(); ++i) {
        process(readings[i]);
    }
}
```

**주요 메서드:**
| 메서드 | 설명 |
|--------|------|
| `push_back(value)` | 끝에 요소 추가 |
| `pop_back()` | 마지막 요소 제거 |
| `at(index)` | 경계 검사 포함 조회 |
| `operator[](index)` | 경계 검사 없음 조회 |
| `front() / back()` | 첫/마지막 요소 |
| `size() / capacity()` | 크기/용량 |
| `clear()` | 모든 요소 제거 |
| `data()` | 포인터 반환 |

**템플릿 파라미터:**
```cpp
StaticVector<T, N>
// T: 요소 타입 (int, float, struct 등)
// N: 최대 용량 (컴파일 타임 상수)
```

**CMake 타겟:** `libemb::util`

### 🎮 Driver (드라이버)

구체적인 장치 제어를 위한 라이브러리입니다.

**모듈 위치:** `include/libemb/driver/` and `src/driver/`

#### Ssd1306 - OLED 디스플레이

128x64 모노크롬 I2C OLED 디스플레이 제어 드라이버입니다.

```cpp
#include "libemb/driver/Ssd1306.hpp"

// I2C 구현이 있다고 가정
MyI2c i2c;
libemb::driver::Ssd1306 display;

// 초기화
if (!display.init(&i2c, 0x3C)) {
    // 초기화 실패
    return;
}

// 화면 지우기
display.clear();

// 픽셀 설정
display.setPixel(64, 32, true);  // (64, 32) 위치에 픽셀 켜기

// 도형 그리기
display.drawHLine(0, 0, 128, true);      // 수평선
display.drawVLine(64, 0, 64, true);      // 수직선
display.drawRect(10, 10, 50, 30, true);  // 채워진 사각형

// 화면에 반영
display.display();

// 명령 전송
display.setContrast(200);              // 명도 조정
display.setInverted(true);             // 색상 반전
display.setDisplayOn(true);            // 화면 켜기
```

**주요 메서드:**
| 메서드 | 설명 |
|--------|------|
| `init(i2c, addr)` | I2C와 주소로 초기화 |
| `display()` | 프레임버퍼를 디스플레이에 전송 |
| `setPixel(x, y, on)` | 픽셀 설정/해제 |
| `clear()` | 화면 지우기 |
| `drawHLine(x, y, w, on)` | 수평선 그리기 |
| `drawVLine(x, y, h, on)` | 수직선 그리기 |
| `drawRect(x, y, w, h, filled, on)` | 사각형 그리기 |
| `setContrast(val)` | 명도 설정 (0-255) |
| `setInverted(on)` | 색상 반전 |
| `setDisplayOn(on)` | 화면 켜기/끄기 |

**CMake 타겟:** `libemb::driver`

### 🧪 Mock (테스트 구현)

실제 하드웨어 없이 테스트 가능한 Mock HAL 구현입니다.

**모듈 위치:** `port/mock/`

**포함 Mock:**
- `MockGpio` - GPIO 상태 변경 기록
- `MockUart` - TX/RX 데이터 큐 시뮬레이션
- `MockI2c` - 트랜잭션 기록 (테스트 코드에 구현)

**사용 예제:**

```cpp
#include "libemb/port/mock/MockGpio.hpp"
#include <gtest/gtest.h>

TEST(MyDriverTest, GpioToggle) {
    libemb::port::mock::MockGpio gpio;
    MyDriver driver;
    driver.init(&gpio);

    // 드라이버 테스트
    driver.toggle_led();

    // Mock에서 기록된 동작 검증
    const auto& writes = gpio.getWriteRecords();
    EXPECT_EQ(writes.size(), 1);
    EXPECT_EQ(writes[0].state, libemb::hal::GpioState::HIGH);
}
```

**CMake 타겟:** `libemb::port::mock`

### 🔄 Protocol (프로토콜)

향후 통신 프로토콜 구현을 위한 자리입니다 (CAN, MQTT, Modbus 등).

**CMake 타겟:** `libemb::protocol`

### 🖥️ Bare-Metal Headers (ARM 크로스컴파일용)

ARM bare-metal 환경에서 표준 C 라이브러리 없이 컴파일할 수 있도록 지원합니다.

**모듈 위치:** `include/libemb/bare-metal/`

**포함 헤더:**
- `stdint.h` - 고정 크기 정수 타입 (uint8_t, int32_t, size_t 등)
- `stddef.h` - 표준 정의 (size_t, ptrdiff_t, NULL)
- `assert.h` - 단언 매크로 (임베디드용 최소화 버전)

**특징:**
- ✅ C 라이브러리 없이도 컴파일 가능
- ✅ ARM `-nostdinc` 플래그와 호환
- ✅ libemb 크로스컴파일 자동 사용
- ✅ 사용자 정의 가능 (NDEBUG 등)

**사용:**
```bash
# 자동으로 포함됩니다 (CMake 설정에서)
cmake -B build/stm32 -DCMAKE_TOOLCHAIN_FILE=cmake/toolchains/arm-none-eabi.cmake
```

---

## 사용 예제

### 예제 1: Non-Blocking UART with RingBuffer

Non-blocking 방식의 UART 통신 예제입니다. RingBuffer를 활용해 인터럽트와 메인 루프를 분리합니다:

```bash
# 빌드
cmake -B build/host -DLIBEMB_BUILD_EXAMPLES=ON
cmake --build build/host

# 실행
./build/host/examples/uart_ringbuffer/uart_ringbuffer_demo
```

**예제 내용:**
- ✅ Non-blocking TX: 데이터를 TX 버퍼에 큐잉, 인터럽트가 전송
- ✅ Non-blocking RX: 인터럽트가 RX 버퍼에 저장, 메인 루프가 처리
- ✅ 버퍼 오버플로우 처리
- ✅ 버퍼 레벨 모니터링

**주요 특징:**
- 메인 루프가 UART 대기 없음
- 버스트 데이터도 효율적 처리
- 예측 가능한 메모리 사용 (동적 할당 없음)
- 실시간 시스템에 적합

### 예제 2: 호스트 데모 실행

```bash
# 빌드
cmake -B build/host -DLIBEMB_BUILD_EXAMPLES=ON
cmake --build build/host

# 실행
./build/host/examples/host_demo/host_demo
```

출력:
```
=== libemb Embedded Library - Host Demo ===

1. RingBuffer Demo
   Creating a ring buffer with capacity 8...
   Pushing values: 10, 20, 30, 40
   Buffer size: 4
   ...
```

### 예제 2: UART 드라이버 구현

```cpp
#include "libemb/hal/IUart.hpp"

class Stm32Uart : public libemb::hal::IUart {
private:
    UART_HandleTypeDef* uart_;
    libemb::util::RingBuffer<uint8_t, 256> rx_buffer_;

public:
    Stm32Uart(UART_HandleTypeDef* uart) : uart_(uart) {}

    void putChar(uint8_t byte) override {
        HAL_UART_Transmit(uart_, &byte, 1, HAL_MAX_DELAY);
    }

    uint8_t getChar() override {
        uint8_t byte;
        rx_buffer_.pop(byte) ? byte : 0;
        return byte;
    }

    bool isDataAvailable() const override {
        return !rx_buffer_.empty();
    }

    // ... 기타 메서드 구현

    // 인터럽트 핸들러에서 호출
    void on_data_received(uint8_t byte) {
        rx_buffer_.push(byte);
    }
};
```

### 예제 3: 통합 예제 (OLED + 온습도 센서)

```cpp
#include "libemb/driver/Ssd1306.hpp"
#include "libemb/hal/IGpio.hpp"
#include "libemb/util/StaticVector.hpp"

class EnvironmentMonitor {
private:
    libemb::driver::Ssd1306 display_;
    libemb::hal::II2c* i2c_;
    libemb::util::StaticVector<float, 100> temps_;
    libemb::util::StaticVector<float, 100> humidities_;

public:
    void initialize(libemb::hal::II2c* i2c) {
        i2c_ = i2c;
        display_.init(i2c);
        display_.clear();
    }

    void update_reading(float temp, float humidity) {
        // 데이터 저장
        if (!temps_.full()) temps_.push_back(temp);
        if (!humidities_.full()) humidities_.push_back(humidity);

        // 화면에 표시
        display_.clear();

        char buffer[32];
        sprintf(buffer, "Temp: %.1f°C", temp);
        // 텍스트 그리기 함수 (구현 필요)

        display_.display();
    }

    void show_history() {
        // 그래프 표시 (픽셀 기반)
        for (size_t i = 0; i < temps_.size(); ++i) {
            float normalized = temps_[i] / 50.0f * 64;  // 스케일 조정
            int y = static_cast<int>(normalized);
            if (y >= 0 && y < 64) {
                display_.setPixel(i % 128, 63 - y, true);
            }
        }
        display_.display();
    }
};
```

---

## 테스트 실행

### 1️⃣ 모든 테스트 실행

```bash
cmake --build build/host
ctest --test-dir build/host --output-on-failure
```

**예상 결과:**
```
Test project /Users/.../build/host
    Start 1: test_RingBuffer
1/2 Test #1: test_RingBuffer ..................   Passed    0.00 sec
    Start 2: test_Ssd1306
2/2 Test #2: test_Ssd1306 .....................   Passed    0.46 sec

100% tests passed, 0 tests failed out of 2
```

### 2️⃣ 특정 테스트만 실행

```bash
# RingBuffer 테스트만
ctest --test-dir build/host -R test_RingBuffer --output-on-failure

# 상세 출력
ctest --test-dir build/host -VV
```

### 3️⃣ 테스트 작성하기

새로운 테스트 파일을 만들어 추가하세요:

```cpp
// test/unit/test_MyModule.cpp
#include <gtest/gtest.h>
#include "libemb/util/RingBuffer.hpp"

TEST(MyModuleTest, BasicOperation) {
    libemb::util::RingBuffer<int, 10> buffer;

    EXPECT_TRUE(buffer.empty());
    EXPECT_TRUE(buffer.push(42));
    EXPECT_FALSE(buffer.empty());

    int value;
    EXPECT_TRUE(buffer.pop(value));
    EXPECT_EQ(value, 42);
}
```

그 후 `test/unit/CMakeLists.txt`에 추가:
```cmake
add_libemb_test(test_MyModule test_MyModule.cpp)
```

---

## MCU 포팅

### 새로운 MCU에 libemb 포팅하기

#### 1단계: MCU 포트 디렉토리 생성

```bash
mkdir -p port/stm32f4/
touch port/stm32f4/CMakeLists.txt
```

#### 2단계: HAL 인터페이스 구현

```cpp
// port/stm32f4/Stm32f4Gpio.hpp
#pragma once

#include "libemb/hal/IGpio.hpp"
#include "stm32f4xx.h"  // ST HAL

namespace libemb::port::stm32f4 {

class Stm32f4Gpio : public libemb::hal::IGpio {
private:
    GPIO_TypeDef* port_;
    uint16_t pin_;

public:
    Stm32f4Gpio(GPIO_TypeDef* port, uint16_t pin)
        : port_(port), pin_(pin) {}

    void setMode(GpioMode mode) override {
        GPIO_InitTypeDef gpio_init = {};
        gpio_init.Pin = pin_;

        switch (mode) {
            case GpioMode::INPUT:
                gpio_init.Mode = GPIO_MODE_INPUT;
                break;
            case GpioMode::OUTPUT:
                gpio_init.Mode = GPIO_MODE_OUTPUT_PP;
                break;
            case GpioMode::INPUT_PULLUP:
                gpio_init.Mode = GPIO_MODE_INPUT;
                gpio_init.Pull = GPIO_PULLUP;
                break;
            // ... 기타 모드
        }

        HAL_GPIO_Init(port_, &gpio_init);
    }

    void write(GpioState state) override {
        GPIO_PinState pin_state = (state == GpioState::HIGH)
            ? GPIO_PIN_SET
            : GPIO_PIN_RESET;
        HAL_GPIO_WritePin(port_, pin_, pin_state);
    }

    GpioState read() const override {
        GPIO_PinState pin_state = HAL_GPIO_ReadPin(port_, pin_);
        return (pin_state == GPIO_PIN_SET)
            ? GpioState::HIGH
            : GpioState::LOW;
    }

    void toggle() override {
        HAL_GPIO_TogglePin(port_, pin_);
    }
};

} // namespace libemb::port::stm32f4
```

#### 3단계: CMakeLists.txt 설정

```cmake
# port/stm32f4/CMakeLists.txt
add_library(libemb_port_stm32f4 INTERFACE)
add_library(libemb::port::stm32f4 ALIAS libemb_port_stm32f4)

target_include_directories(libemb_port_stm32f4
    INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/../..
)

target_link_libraries(libemb_port_stm32f4
    INTERFACE
    libemb::hal
    stm32f4_hal  # 외부 라이브러리 (예시)
)

target_compile_features(libemb_port_stm32f4 INTERFACE cxx_std_17)
```

#### 4단계: 메인 CMakeLists.txt에 조건부 포함

```cmake
# 루트 CMakeLists.txt
if(LIBEMB_PORT STREQUAL "stm32f4")
    add_subdirectory(port/stm32f4)
endif()
```

#### 5단계: 빌드

```bash
cmake -B build/stm32f4 \
  -DCMAKE_TOOLCHAIN_FILE=cmake/toolchains/arm-none-eabi.cmake \
  -DLIBEMB_PORT=stm32f4

cmake --build build/stm32f4
```

---

## FAQ

### Q1: 동적 메모리 할당을 해야 하는데요?

**A:** libemb의 철학은 예측 가능한 임베디드 시스템입니다. 대신:

- StaticVector의 크기를 더 크게 설정
- 여러 개의 작은 버퍼 사용
- 필요시 예외적으로 malloc 사용 (권장하지 않음)

### Q2: 특정 MCU가 지원되지 않는데요?

**A:** 포팅은 간단합니다!

1. HAL 인터페이스 구현
2. 새로운 포트 디렉토리 생성
3. CMakeLists.txt에 추가

### Q3: 예외를 사용할 수 있나요?

**A:** 사용 가능하지만 권장하지 않습니다.

- 임베디드 시스템에서는 보통 disabled
- 크기 및 성능 오버헤드 있음
- 에러 코드나 Optional 사용 권장

### Q4: RTOS를 지원하나요?

**A:** 현재 RTOS 가정이 없습니다.

- 단순 bare-metal 설계
- RTOS 위에서도 작동 가능
- RTOS 특화 기능이 필요하면 래퍼 추가

### Q5: 성능은 어떤가요?

**A:** 매우 효율적입니다:

- 헤더 온리 템플릿: 인라인 최적화
- 링 버퍼: O(1) 모든 연산
- 스택 할당: 예측 가능한 타이밍

### Q6: 다중 스레드를 지원하나요?

**A:** 현재 기본 지원 없습니다:

- 싱글 스레드 구현 가정
- Mutex 등은 MCU 포트에서 제공
- 필요시 스레드-세이프 래퍼 추가

### Q7: 디버깅은 어떻게 하나요?

**A:** 여러 방법:

```bash
# 호스트에서 GDB 사용
gdb ./build/host/test/unit/test_RingBuffer

# AddressSanitizer로 메모리 버그 감지
cmake -B build/debug -DLIBEMB_ENABLE_ASAN=ON -DCMAKE_BUILD_TYPE=Debug

# 자세한 로그 출력
ctest --test-dir build/host -VV
```

---

## 문제 해결

### 문제: CMake 설정 실패

```
CMake Error at CMakeLists.txt:1 (project):
  The CMAKE_CXX_COMPILER:
    /usr/bin/c++
  is not a full path and was not found in the PATH.
```

**해결:** 컴파일러 설치

```bash
# macOS
brew install llvm

# Ubuntu
sudo apt-get install build-essential
```

### 문제: 테스트 컴파일 실패

```
error: undefined reference to `gtest_main'
```

**해결:** CMake 재설정

```bash
rm -rf build/
cmake -B build/host -DLIBEMB_BUILD_TESTS=ON
cmake --build build/host
```

### 문제: ARM 크로스 컴파일 실패

```
arm-none-eabi-g++: command not found
```

**해결:** ARM 툴체인 설치

```bash
# macOS
brew install arm-none-eabi-gcc

# Ubuntu
sudo apt-get install gcc-arm-none-eabi
```

### 문제: AddressSanitizer 에러

```
ERROR: AddressSanitizer requires Clang or GCC
```

**해결:** Clang 사용

```bash
cmake -B build/asan \
  -DCMAKE_CXX_COMPILER=clang++ \
  -DLIBEMB_ENABLE_ASAN=ON
```

### 문제: ARM 크로스 컴파일 시 표준 라이브러리 헤더 찾기 실패

```
error: stdint.h: No such file or directory
error: assert.h: No such file or directory
```

**원인:** ARM bare-metal 도구체인은 C 표준 라이브러리를 포함하지 않습니다.

**해결:** libemb의 bare-metal 헤더가 자동으로 제공됩니다.

```bash
# 최신 버전으로 업데이트하면 자동 해결됩니다
git pull

# 재빌드
rm -rf build/stm32
cmake -B build/stm32 \
  -DCMAKE_TOOLCHAIN_FILE=cmake/toolchains/arm-none-eabi.cmake \
  -DLIBEMB_BUILD_TESTS=OFF

cmake --build build/stm32
```

**상세 정보:**
- libemb의 코드는 C++ 표준 라이브러리 대신 bare-metal 헤더 사용
- `<array>` 제거 → C 스타일 배열 사용
- `<stdint.h>` → `include/libemb/bare-metal/stdint.h`
- `-nostdinc` 플래그로 시스템 라이브러리 제외

### 문제: ARM 바이너리 크기 확인

```bash
# 생성된 라이브러리 확인
ls -lh build/stm32/src/driver/liblibemb_driver.a

# 상세 크기 정보 (ARM 도구 필요)
arm-none-eabi-size build/stm32/src/driver/liblibemb_driver.a
```

**출력 예시:**
```
text    data    bss     dec     hex    filename
1441       0      0    1441    5a1    Ssd1306.cpp.obj
```

---

## 기여하기

버그 리포트, 기능 요청, PR은 항상 환영합니다!

1. 현재 상태 확인: `git status`
2. 새 브랜치 생성: `git checkout -b feature/my-feature`
3. 테스트 작성: `test/unit/test_my_feature.cpp`
4. 빌드 확인: `cmake --build build/host`
5. 테스트 통과: `ctest --test-dir build/host`
6. PR 제출

---

## 라이센스

[라이센스 정보 추가]

---

## 참고 문서

- **개발자 가이드:** [CLAUDE.md](CLAUDE.md)
- **CMake 문서:** https://cmake.org/documentation/
- **Google Test:** https://github.com/google/googletest
- **C++17 표준:** https://en.cppreference.com/w/cpp/17

---

## 버전 정보

- **현재 버전:** 0.2.0
- **릴리즈 날짜:** 2026-03-02
- **상태:** 안정적 (ARM 크로스컴파일 지원)

**주요 업데이트 (v0.2.0):**
- ✨ ARM Cortex-M bare-metal 크로스컴파일 지원
- ✨ Bare-metal 헤더 세트 추가 (stdint.h, stddef.h, assert.h)
- ✨ C++ 표준 라이브러리 의존성 제거
- 🔧 StaticVector와 RingBuffer를 C 스타일 배열 기반으로 리팩토링
- 📚 ARM 크로스컴파일 문서 확대

---

**마지막 업데이트:** 2026-03-02

질문이나 피드백은 이슈를 생성해주세요!
