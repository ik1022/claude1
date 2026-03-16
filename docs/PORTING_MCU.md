# MCU 포팅 가이드

libemb를 새로운 MCU 플랫폼에 이식하는 방법을 설명합니다.

## 포팅 개요

libemb의 핵심 아이디어는 **하드웨어 추상화 계층 (HAL)**입니다. 새로운 MCU로 이식하려면:

1. HAL 인터페이스를 MCU에 맞게 **구현**
2. 나머지 드라이버는 **변경 없음**
3. 크로스컴파일 환경 **설정**

```
libemb 코드
    ↓
HAL 인터페이스 (변경 없음)
    ↓
STM32 구현 ← 이 부분만 변경!
    ↓
하드웨어
```

## 단계별 포팅 절차

### Step 1: 포트 디렉토리 구조 생성

```bash
# STM32F4 포팅의 경우
mkdir -p port/stm32f4/hal
mkdir -p port/stm32f4/common
mkdir -p port/stm32f4/startup
mkdir -p cmake/toolchains
```

**예상 구조**:
```
port/stm32f4/
├── hal/
│   ├── Stm32f4Gpio.hpp
│   ├── Stm32f4Gpio.cpp
│   ├── Stm32f4Uart.hpp
│   ├── Stm32f4Uart.cpp
│   ├── Stm32f4I2c.hpp
│   ├── Stm32f4I2c.cpp
│   ├── Stm32f4Spi.hpp
│   └── Stm32f4Spi.cpp
├── common/
│   ├── SystemClock.cpp
│   └── SystemClock.hpp
├── startup/
│   ├── startup_stm32f407xx.s
│   └── stm32f407xx_flash.ld
└── CMakeLists.txt
```

### Step 2: HAL 인터페이스 구현

각 HAL 인터페이스를 MCU에 맞게 구현합니다.

#### 2-1: GPIO 구현

```cpp
// port/stm32f4/hal/Stm32f4Gpio.hpp
#pragma once

#include "libemb/hal/IGpio.hpp"
#include "stm32f4xx.h"

namespace libemb::port::stm32f4 {

/**
 * @brief STM32F4 GPIO 구현
 */
class Stm32f4Gpio : public libemb::hal::IGpio {
public:
    /**
     * @brief GPIO 포트 초기화
     * @param port GPIO 포트 (GPIOA, GPIOB, ...)
     * @param pin 핀 번호 (0-15)
     */
    Stm32f4Gpio(GPIO_TypeDef* port, uint16_t pin);

    /// @brief 핀을 HIGH로 설정
    libemb::hal::GpioStatus setPin(uint8_t level) override;

    /// @brief 핀 상태 읽기
    libemb::hal::GpioStatus getPin(bool* level) override;

private:
    GPIO_TypeDef* port_;
    uint16_t pin_;
};

} // namespace libemb::port::stm32f4
```

```cpp
// port/stm32f4/hal/Stm32f4Gpio.cpp
#include "Stm32f4Gpio.hpp"

namespace libemb::port::stm32f4 {

Stm32f4Gpio::Stm32f4Gpio(GPIO_TypeDef* port, uint16_t pin)
    : port_(port), pin_(pin) {
}

libemb::hal::GpioStatus Stm32f4Gpio::setPin(uint8_t level) {
    if (level) {
        HAL_GPIO_WritePin(port_, pin_, GPIO_PIN_SET);
    } else {
        HAL_GPIO_WritePin(port_, pin_, GPIO_PIN_RESET);
    }
    return libemb::hal::GpioStatus::Ok;
}

libemb::hal::GpioStatus Stm32f4Gpio::getPin(bool* level) {
    if (level == nullptr) {
        return libemb::hal::GpioStatus::ParameterError;
    }
    *level = (HAL_GPIO_ReadPin(port_, pin_) == GPIO_PIN_SET);
    return libemb::hal::GpioStatus::Ok;
}

} // namespace libemb::port::stm32f4
```

#### 2-2: UART 구현

```cpp
// port/stm32f4/hal/Stm32f4Uart.hpp
#pragma once

#include "libemb/hal/IUart.hpp"
#include "stm32f4xx_hal.h"

namespace libemb::port::stm32f4 {

/**
 * @brief STM32F4 UART 구현
 */
class Stm32f4Uart : public libemb::hal::IUart {
public:
    /// UART 핸들로 초기화
    Stm32f4Uart(UART_HandleTypeDef* huart);

    libemb::hal::UartStatus transmit(const uint8_t* data,
                                      size_t len) override;

    libemb::hal::UartStatus receive(uint8_t* data,
                                     size_t* len) override;

private:
    UART_HandleTypeDef* huart_;
};

} // namespace libemb::port::stm32f4
```

#### 2-3: I2C 구현

```cpp
// port/stm32f4/hal/Stm32f4I2c.hpp
#pragma once

#include "libemb/hal/II2c.hpp"
#include "stm32f4xx_hal.h"

namespace libemb::port::stm32f4 {

/**
 * @brief STM32F4 I2C 구현
 */
class Stm32f4I2c : public libemb::hal::II2c {
public:
    /// I2C 핸들로 초기화
    Stm32f4I2c(I2C_HandleTypeDef* hi2c);

    libemb::hal::I2cStatus write(uint8_t slaveAddr,
                                  const uint8_t* data,
                                  size_t len) override;

    libemb::hal::I2cStatus read(uint8_t slaveAddr,
                                 uint8_t* data,
                                 size_t len) override;

private:
    I2C_HandleTypeDef* hi2c_;
};

} // namespace libemb::port::stm32f4
```

### Step 3: 크로스컴파일 도구체인 설정

```cmake
# cmake/toolchains/arm-none-eabi.cmake

# 도구 설정
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR ARM)

set(TOOLCHAIN_PREFIX arm-none-eabi)
set(CMAKE_C_COMPILER ${TOOLCHAIN_PREFIX}-gcc)
set(CMAKE_CXX_COMPILER ${TOOLCHAIN_PREFIX}-g++)
set(CMAKE_ASM_COMPILER ${TOOLCHAIN_PREFIX}-gcc)
set(CMAKE_OBJCOPY ${TOOLCHAIN_PREFIX}-objcopy)
set(CMAKE_OBJDUMP ${TOOLCHAIN_PREFIX}-objdump)
set(CMAKE_SIZE ${TOOLCHAIN_PREFIX}-size)

# 컴파일 플래그
set(COMMON_FLAGS
    -mcpu=cortex-m4
    -mthumb
    -mfpu=fpv4-sp-d16
    -mfloat-abi=hard
    -fdata-sections
    -ffunction-sections
    --specs=nosys.specs
)

set(CMAKE_C_FLAGS "${COMMON_FLAGS}" CACHE STRING "")
set(CMAKE_CXX_FLAGS "${COMMON_FLAGS}" CACHE STRING "")
set(CMAKE_ASM_FLAGS "${COMMON_FLAGS}" CACHE STRING "")

# 링크 플래그
set(CMAKE_EXE_LINKER_FLAGS
    "${COMMON_FLAGS} -Wl,--gc-sections"
    CACHE STRING "")

# 크로스컴파일 플래그 설정
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
```

### Step 4: CMakeLists.txt 작성

```cmake
# port/stm32f4/CMakeLists.txt

add_library(libemb_port_stm32f4 STATIC)
add_library(libemb::port::stm32f4 ALIAS libemb_port_stm32f4)

# 소스 파일 추가
target_sources(libemb_port_stm32f4
    PRIVATE
    hal/Stm32f4Gpio.cpp
    hal/Stm32f4Uart.cpp
    hal/Stm32f4I2c.cpp
    hal/Stm32f4Spi.cpp
    common/SystemClock.cpp
)

# include 경로
target_include_directories(libemb_port_stm32f4
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../..
)

# STM32 HAL 라이브러리 링크
target_link_libraries(libemb_port_stm32f4
    PUBLIC libemb::hal
    PRIVATE STM32F4_HAL  # STM32 공식 HAL
)

message(STATUS "STM32F4 port configured")
```

### Step 5: 루트 CMakeLists.txt 업데이트

```cmake
# CMakeLists.txt

# 새 옵션 추가
option(LIBEMB_TARGET_MCU "Target MCU platform" "")

# 크로스컴파일 설정
if(DEFINED LIBEMB_TARGET_MCU)
    if(LIBEMB_TARGET_MCU STREQUAL "stm32f4")
        set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/cmake/toolchains/arm-none-eabi.cmake)
        add_subdirectory(port/stm32f4)
    endif()
endif()

# 플랫폼별 조건부 빌드
if(NOT CMAKE_CROSSCOMPILING)
    # 호스트 전용
    add_subdirectory(port/mock)
else()
    # MCU 전용
    # tests 비활성화 (크로스컴파일 환경에서는 실행 불가)
endif()
```

### Step 6: 크로스컴파일 빌드

```bash
# STM32F4 빌드
cmake -B build/stm32f4 \
  -DLIBEMB_TARGET_MCU=stm32f4 \
  -DCMAKE_TOOLCHAIN_FILE=cmake/toolchains/arm-none-eabi.cmake

cmake --build build/stm32f4

# 바이너리 정보 확인
arm-none-eabi-size build/stm32f4/<firmware.elf>
arm-none-eabi-objdump -h build/stm32f4/<firmware.elf>
```

### Step 7: 애플리케이션 코드 작성

```cpp
// examples/stm32f4_demo/main.cpp

#include "libemb/driver/ssd1306/Ssd1306.hpp"
#include "port/stm32f4/hal/Stm32f4I2c.hpp"

// STM32 HAL 초기화 (자동 생성)
extern I2C_HandleTypeDef hi2c1;
extern UART_HandleTypeDef huart2;

using namespace libemb::driver;
using namespace libemb::port::stm32f4;

int main(void) {
    // HAL 초기화
    HAL_Init();
    SystemClock_Config();

    // libemb 포트 초기화
    Stm32f4I2c i2c(&hi2c1);

    // 드라이버 사용 (호스트와 동일!)
    Ssd1306 display;
    display.init(&i2c);

    // 루프
    while (1) {
        display.clear();
        display.drawVLine(10, 0, 64, true);
        display.display();

        HAL_Delay(1000);
    }

    return 0;
}
```

### Step 8: 펌웨어 플래시

```bash
# STLink를 사용한 플래시
st-flash write build/stm32f4/firmware.bin 0x08000000

# 또는 OpenOCD 사용
openocd -f interface/stlink.cfg \
        -f target/stm32f4x.cfg \
        -c "program build/stm32f4/firmware.elf verify reset exit"
```

## 체크리스트

- [ ] **디렉토리 생성**
  - [ ] `port/mcu/hal/` 생성
  - [ ] `cmake/toolchains/` 생성

- [ ] **HAL 구현**
  - [ ] `IGpio` 구현
  - [ ] `IUart` 구현
  - [ ] `ISpi` 구현
  - [ ] `II2c` 구현

- [ ] **크로스컴파일 설정**
  - [ ] 도구체인 파일 작성
  - [ ] 컴파일 플래그 설정
  - [ ] 링크 스크립트 포함

- [ ] **CMakeLists.txt**
  - [ ] 포트 CMakeLists.txt 작성
  - [ ] 루트 CMakeLists.txt 업데이트
  - [ ] 빌드 성공 확인

- [ ] **테스트**
  - [ ] 크로스컴파일 빌드 완료
  - [ ] 바이너리 크기 확인
  - [ ] 하드웨어 테스트

- [ ] **문서화**
  - [ ] README.md (포팅 가이드)
  - [ ] 빌드 지침
  - [ ] 추가 설정 (클록, 핀 등)

## 일반적인 문제 해결

### 문제 1: "arm-none-eabi-gcc를 찾을 수 없음"

```bash
# 도구체인 설치 (Ubuntu/Debian)
sudo apt-get install gcc-arm-none-eabi

# 또는 ARM 공식 배포판
# https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm
```

### 문제 2: 링크 에러

```
undefined reference to `HAL_I2C_Master_Transmit'
```

해결책:
- STM32 HAL 라이브러리가 링크되었는지 확인
- CMakeLists.txt에서 STM32_HAL 추가

### 문제 3: 메모리 부족

```
section `.text' will not fit in region `FLASH'
```

해결책:
- 최적화 플래그 추가: `-Os`, `-flto`
- 불필요한 기능 제거
- 링크 스크립트 확인

## 향후 포팅 예상

```
port/
├── stm32f4/          (2025년 Q1)
├── nrf52840/         (2025년 Q2)
├── esp32/            (2025년 Q3)
└── rp2040/           (향후)
```

## 더 알아보기

- [아키텍처](./ARCHITECTURE.md) - libemb 구조
- [드라이버 추가](./ADDING_DRIVERS.md) - 새 드라이버 개발
- [예제](../examples/) - 실제 사용 예
