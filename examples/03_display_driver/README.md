# 03_display_driver - Ssd1306 OLED 드라이버 사용법

## 목표

이 예제는 libemb의 드라이버 아키텍처를 소개합니다:
- **HAL 추상화**: 하드웨어 독립적인 인터페이스
- **드라이버 통합**: 센서/디바이스 제어 로직
- **Mock을 통한 테스트**: 하드웨어 없이 PC에서 검증

## 아키텍처

```
Application Layer
    ↓
Ssd1306 Driver (디스플레이 제어 로직)
    ↓
HAL::II2c Interface (하드웨어 추상화)
    ↓
MockI2c (host) / Stm32f4I2c (embedded)
    ↓
실제 하드웨어 또는 시뮬레이션
```

## 주요 개념

### 하드웨어 독립성

같은 코드가 다양한 플랫폼에서 작동:

```cpp
// Host (Mock)
MockI2c i2c;
display.init(&i2c);

// STM32F4 (하드웨어)
Stm32f4I2c i2c;
display.init(&i2c);  // 동일한 코드!
```

### Ssd1306 드라이버

SSD1306은 I2C/SPI 기반 128×64 흑백 OLED 디스플레이:

```
┌─────────────────────┐
│                     │ ← 128 pixels
│  OLED Display       │
│                     │
└─────────────────────┘
64 pixels

드라이버 기능:
- 초기화 및 설정
- 픽셀 단위 그리기
- 프레임 버퍼 관리
- 디스플레이 업데이트
```

## 코드 실행

```bash
cmake --build build/host --target display_driver

# 예제 실행
./build/host/display_driver
```

## 예상 출력

```
=== Ssd1306 OLED Display Driver Example ===

[1] Initializing display...
    ✓ Display initialized successfully
    → I2C transactions: 35
    → First transaction to address: 0x3C

[2] Clearing display...
    ✓ Display cleared

[3] Drawing pattern...
    ✓ Pattern drawn (vertical lines at x = 0, 4, 8, ...)

[4] Drawing horizontal lines...
    ✓ Horizontal lines drawn

[5] Testing custom I2C address...
    ✓ Display initialized with address 0x3D

=== Display Statistics ===
Total I2C transactions: 70
Display dimensions: 128x64 pixels
Driver class: Ssd1306
Communication protocol: I2C
Frame buffer updates: 3

✓ Example completed successfully!
```

## 코드 구조

### 기본 사용법

```cpp
// 1. Mock I2C 생성 (또는 실제 하드웨어 I2C)
MockI2c i2c;

// 2. 드라이버 생성
Ssd1306 display;

// 3. 초기화
display.init(&i2c);  // 기본 주소 0x3C

// 4. 그리기
display.clear();                      // 화면 지우기
display.drawVLine(10, 0, 64, true);   // 수직선
display.drawHLine(0, 20, 128, true);  // 수평선

// 5. 업데이트
display.display();   // 프레임 버퍼를 디스플레이에 전송
```

### I2C 트랜잭션 검증

```cpp
// Mock의 트랜잭션 기록 확인
auto txns = i2c.getTransactions();
for (const auto& txn : txns) {
    printf("Address: 0x%02X, Bytes: %zu\n",
           txn.slaveAddr, txn.data.size());
}
```

## 학습 포인트

1. **드라이버 패턴**
   - 센서/디바이스 제어 로직
   - 프로토콜 추상화 (I2C, SPI)
   - 상태 관리

2. **HAL의 중요성**
   - 하드웨어 세부사항 숨김
   - 다양한 MCU 지원 용이
   - 테스트 용이 (Mock 사용)

3. **Mock을 통한 개발**
   ```cpp
   // PC에서 하드웨어 없이 개발/테스트
   MockI2c mock;
   display.init(&mock);

   // 나중에 실제 하드웨어로 교체
   Stm32f4I2c hw_i2c;
   display.init(&hw_i2c);
   ```

## 실제 하드웨어로 이식

이 예제를 STM32F4에 이식하려면:

1. **I2C 구현**
   ```cpp
   // port/stm32f4/hal/Stm32f4I2c.hpp
   class Stm32f4I2c : public libemb::hal::II2c {
       // HAL_I2C_* 함수 기반 구현
   };
   ```

2. **초기화**
   ```cpp
   Stm32f4I2c i2c(&hi2c1);  // STM32 HAL I2C 핸들
   Ssd1306 display;
   display.init(&i2c);
   ```

3. **빌드 및 플래시**
   ```bash
   cmake -B build/stm32 -DCMAKE_TOOLCHAIN_FILE=cmake/toolchains/arm-none-eabi.cmake
   cmake --build build/stm32
   # STLink로 플래시
   ```

## 다음 단계

- [드라이버 추가 가이드](../../docs/ADDING_DRIVERS.md) - 새로운 센서 드라이버 추가
- [MCU 포팅 가이드](../../docs/PORTING_MCU.md) - STM32에 이식
- [아키텍처](../../docs/ARCHITECTURE.md) - libemb 전체 구조 이해
