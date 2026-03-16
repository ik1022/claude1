# 새로운 드라이버 추가 가이드

libemb에 새로운 센서/디바이스 드라이버를 추가하는 방법을 단계별로 설명합니다.

## 드라이버 분류

### Type A: 간단한 드라이버 (GPIO, LED)
- 단순 핀 제어
- 상태 기반
- 초기화 최소

### Type B: 시리얼 드라이버 (I2C, SPI)
- I2C/SPI 기반 센서
- 복잡한 프로토콜
- Mock I2C/SPI로 테스트 가능

**예**: Ssd1306, BMP280, MPU6050

### Type C: 복잡한 드라이버
- 다단계 초기화
- 상태 머신
- 에러 복구 로직

## 단계별 추가 가이드

### Step 1: 폴더 구조 생성

```bash
mkdir -p include/libemb/driver/bmp280
mkdir -p src/driver/bmp280
mkdir -p test/unit/driver
```

예시: BMP280 기압 센서

### Step 2: 타입 정의 헤더 작성

```cpp
// include/libemb/driver/bmp280/Bmp280Types.hpp
#pragma once

#include <cstdint>

namespace libemb::driver {

/// BMP280 구성 설정
struct Bmp280Config {
    /// I2C 기본 주소
    static constexpr uint8_t I2C_ADDR_PRIMARY = 0x76;
    /// I2C 보조 주소
    static constexpr uint8_t I2C_ADDR_SECONDARY = 0x77;
};

/// BMP280 센서 데이터
struct Bmp280Data {
    /// 온도 (°C)
    float temperature;
    /// 기압 (hPa)
    float pressure;
    /// 고도 (m) - 선택사항
    float altitude;
};

/// BMP280 상태
enum class Bmp280Status {
    Ok = 0,
    InvalidAddress = 1,
    CommunicationError = 2,
    ParameterError = 3,
    NotInitialized = 4,
};

} // namespace libemb::driver
```

### Step 3: 드라이버 인터페이스 헤더

```cpp
// include/libemb/driver/bmp280/Bmp280.hpp
#pragma once

#include "libemb/hal/II2c.hpp"
#include "Bmp280Types.hpp"

namespace libemb::driver {

/**
 * @brief BMP280 기압 센서 드라이버
 *
 * Bosch BMP280 기압/온도 센서 제어
 *
 * 사용 예:
 * ```cpp
 * Bmp280 sensor;
 * sensor.init(&i2c, Bmp280Config::I2C_ADDR_PRIMARY);
 * Bmp280Data data;
 * sensor.read(&data);
 * ```
 */
class Bmp280 {
public:
    /// 기본 생성자
    Bmp280() = default;

    /// 복사 금지
    Bmp280(const Bmp280&) = delete;
    Bmp280& operator=(const Bmp280&) = delete;

    /**
     * @brief 센서 초기화
     * @param i2c I2C HAL 인터페이스
     * @param slaveAddr I2C 슬레이브 주소 (기본값: 0x76)
     * @return 초기화 성공 여부
     */
    bool init(libemb::hal::II2c* i2c,
              uint8_t slaveAddr = Bmp280Config::I2C_ADDR_PRIMARY);

    /**
     * @brief 센서에서 데이터 읽기
     * @param data 결과 저장소 (nullptr 불가)
     * @return 읽기 성공 여부
     */
    bool read(Bmp280Data* data);

    /**
     * @brief 센서 상태 확인
     * @return 현재 상태
     */
    Bmp280Status status() const;

    /**
     * @brief 센서 리셋
     * @return 리셋 성공 여부
     */
    bool reset();

private:
    /// I2C 인터페이스 (소유하지 않음)
    libemb::hal::II2c* i2c_ = nullptr;
    /// I2C 슬레이브 주소
    uint8_t slave_addr_ = Bmp280Config::I2C_ADDR_PRIMARY;
    /// 초기화 상태
    bool initialized_ = false;
    /// 캘리브레이션 데이터
    struct CalibrationData {
        uint16_t dig_T1;
        int16_t dig_T2, dig_T3;
        uint16_t dig_P1;
        int16_t dig_P2, dig_P3, dig_P4, dig_P5, dig_P6, dig_P7, dig_P8, dig_P9;
    } calib_;

    /// 캘리브레이션 데이터 읽기
    bool readCalibration();
    /// I2C 레지스터 읽기
    bool readRegister(uint8_t reg, uint8_t* data, size_t len);
    /// I2C 레지스터 쓰기
    bool writeRegister(uint8_t reg, uint8_t data);
};

} // namespace libemb::driver
```

### Step 4: 드라이버 구현

```cpp
// src/driver/bmp280/Bmp280.cpp
#include "libemb/driver/bmp280/Bmp280.hpp"

namespace libemb::driver {

bool Bmp280::init(libemb::hal::II2c* i2c, uint8_t slaveAddr) {
    if (i2c == nullptr) {
        return false;
    }

    i2c_ = i2c;
    slave_addr_ = slaveAddr;

    // 센서 ID 확인
    uint8_t chip_id = 0;
    if (!readRegister(0xD0, &chip_id, 1)) {
        return false;
    }

    // BMP280 칩 ID: 0x58
    if (chip_id != 0x58) {
        return false;
    }

    // 캘리브레이션 데이터 읽기
    if (!readCalibration()) {
        return false;
    }

    // 정상 모드로 설정 (ctrl_meas: 0x2F)
    if (!writeRegister(0xF4, 0x2F)) {
        return false;
    }

    initialized_ = true;
    return true;
}

bool Bmp280::read(Bmp280Data* data) {
    if (data == nullptr || !initialized_) {
        return false;
    }

    // 측정 데이터 읽기 (주소: 0xF7~0xF9)
    uint8_t raw_data[3];
    if (!readRegister(0xF7, raw_data, 3)) {
        return false;
    }

    // 온도 데이터 읽기 (주소: 0xFA~0xFC)
    uint8_t temp_data[3];
    if (!readRegister(0xFA, temp_data, 3)) {
        return false;
    }

    // ... 데이터 파싱 및 캘리브레이션 적용
    data->pressure = 1013.25f;  // 더미 값
    data->temperature = 25.0f;   // 더미 값

    return true;
}

bool Bmp280::readCalibration() {
    // ... 캘리브레이션 데이터 읽기 구현
    return true;
}

// ... 기타 함수 구현

} // namespace libemb::driver
```

### Step 5: 테스트 작성

```cpp
// test/unit/driver/test_Bmp280.cpp
#include <gtest/gtest.h>
#include "libemb/driver/bmp280/Bmp280.hpp"
#include "port/mock/hal/MockI2c.hpp"

using namespace libemb::driver;
using namespace libemb::port::mock;

class Bmp280Test : public ::testing::Test {
protected:
    MockI2c mock_i2c_;
    Bmp280 sensor_;
};

/// 센서 초기화 테스트
TEST_F(Bmp280Test, InitializationSuccess) {
    // Mock을 설정해서 칩 ID 반환
    mock_i2c_.setReadResponse(0xD0, 0x58);  // BMP280 ID

    EXPECT_TRUE(sensor_.init(&mock_i2c_));
    EXPECT_EQ(sensor_.status(), Bmp280Status::Ok);
}

/// 잘못된 I2C 포인터
TEST_F(Bmp280Test, InitializationWithNullI2c) {
    EXPECT_FALSE(sensor_.init(nullptr));
}

/// 잘못된 칩 ID
TEST_F(Bmp280Test, InitializationWithWrongChipId) {
    mock_i2c_.setReadResponse(0xD0, 0xFF);  // 틀린 ID

    EXPECT_FALSE(sensor_.init(&mock_i2c_));
}

/// 데이터 읽기
TEST_F(Bmp280Test, ReadData) {
    // 초기화
    mock_i2c_.setReadResponse(0xD0, 0x58);
    EXPECT_TRUE(sensor_.init(&mock_i2c_));

    // Mock 데이터 설정
    mock_i2c_.setReadResponse(0xF7, 0x50);  // 기압 MSB
    mock_i2c_.setReadResponse(0xF8, 0x00);  // 기압 LSB
    mock_i2c_.setReadResponse(0xF9, 0x00);  // 기압 XLSB

    // 데이터 읽기
    Bmp280Data data;
    EXPECT_TRUE(sensor_.read(&data));
    EXPECT_GT(data.pressure, 0);
}

/// I2C 통신 오류 처리
TEST_F(Bmp280Test, I2cCommunicationError) {
    mock_i2c_.setFailNextRead();

    EXPECT_FALSE(sensor_.init(&mock_i2c_));
}
```

### Step 6: CMakeLists.txt 등록

```cmake
# src/driver/CMakeLists.txt에 추가
add_libemb_driver(bmp280)

# test/unit/driver/CMakeLists.txt에 추가
add_libemb_test(test_Bmp280 test_Bmp280.cpp)
```

### Step 7: 빌드 및 테스트

```bash
# 빌드
cmake -B build/host -DLIBEMB_BUILD_TESTS=ON
cmake --build build/host

# 테스트 실행
ctest --test-dir build/host --output-on-failure

# 특정 테스트만
ctest --test-dir build/host -R test_Bmp280 --output-on-failure
```

### Step 8: 예제 작성 (선택사항)

```cpp
// examples/04_sensor_integration/main.cpp
#include "libemb/driver/bmp280/Bmp280.hpp"
#include "port/mock/hal/MockI2c.hpp"

int main() {
    using namespace libemb::driver;
    using namespace libemb::port::mock;

    MockI2c i2c;
    Bmp280 sensor;

    if (!sensor.init(&i2c)) {
        printf("Sensor initialization failed\n");
        return 1;
    }

    Bmp280Data data;
    for (int i = 0; i < 10; ++i) {
        if (sensor.read(&data)) {
            printf("Temperature: %.2f°C, Pressure: %.2f hPa\n",
                   data.temperature, data.pressure);
        }
    }

    return 0;
}
```

### Step 9: 문서화

각 드라이버는 다음 문서를 포함해야 합니다:
- **README.md**: 기본 사용법, 사양, 예제
- **Doxygen 주석**: API 문서
- **config.cmake**: 의존성 선언

```markdown
# BMP280 기압 센서 드라이버

## 개요
Bosch BMP280 기압/온도 센서 드라이버

## 특징
- I2C 인터페이스
- 기압, 온도 측정
- 낮은 전력 소비

## 사양
| 항목 | 값 |
|------|-----|
| I2C 주소 | 0x76, 0x77 |
| 기압 범위 | 300~1100 hPa |
| 온도 범위 | -40~85°C |
| 정밀도 | 1 hPa |

## 사용법
```cpp
Bmp280 sensor;
sensor.init(&i2c);
Bmp280Data data;
sensor.read(&data);
```
```

## 체크리스트

- [ ] **헤더 파일**
  - [ ] `Bmp280Types.hpp` - 구조체, enum
  - [ ] `Bmp280.hpp` - 클래스 선언
  - [ ] Doxygen 주석 완성

- [ ] **구현**
  - [ ] `Bmp280.cpp` - 구현 완료
  - [ ] 에러 처리
  - [ ] 메모리 효율성

- [ ] **테스트**
  - [ ] `test_Bmp280.cpp` - 최소 5개 테스트
  - [ ] Mock I2C 사용
  - [ ] 정상 케이스 + 에러 케이스

- [ ] **빌드 통합**
  - [ ] `src/driver/CMakeLists.txt`에 등록
  - [ ] `test/unit/driver/CMakeLists.txt`에 등록
  - [ ] 빌드 성공 확인
  - [ ] 모든 테스트 통과

- [ ] **예제** (선택사항)
  - [ ] 기본 사용 예제
  - [ ] README.md

- [ ] **문서화**
  - [ ] `include/libemb/driver/bmp280/config.cmake`
  - [ ] 사용 가이드
  - [ ] API 참고자료

## 일반적인 실수

### 1. 동적 메모리 사용
❌ 금지:
```cpp
std::vector<uint8_t> buffer;  // 동적 할당
malloc(size);
```

✅ 추천:
```cpp
libemb::util::StaticVector<uint8_t, 256> buffer;
uint8_t buffer[256];
```

### 2. 하드웨어 의존성
❌ 금지:
```cpp
// STM32 특화 코드
HAL_I2C_Master_Transmit(&hi2c1, ...);
```

✅ 추천:
```cpp
// HAL 인터페이스 사용
i2c_->write(addr, data, len);
```

### 3. 테스트 부재
❌ 금지:
```cpp
// 테스트 없음
// 실제 하드웨어에서만 테스트
```

✅ 추천:
```cpp
// Mock을 사용한 테스트
MockI2c mock;
sensor.init(&mock);
EXPECT_TRUE(...);
```

## 더 알아보기

- [아키텍처](./ARCHITECTURE.md) - libemb 전체 구조
- [MCU 포팅](./PORTING_MCU.md) - 새로운 MCU 지원
- [예제](../examples/) - 실제 사용 예
