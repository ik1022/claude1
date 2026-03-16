# libemb Documentation

libemb 임베디드 라이브러리 공식 문서입니다.

## 📚 문서 목차

### 🏗️ 아키텍처 및 설계

[**ARCHITECTURE.md**](./ARCHITECTURE.md)
- libemb 전체 계층 구조
- 모듈 간 의존성 관계
- 설계 원칙 및 철학
- 빌드 시스템 개요

**대상**: 라이브러리를 이해하고 싶은 개발자

---

### 🚀 개발자 가이드

#### 새 드라이버 추가하기

[**ADDING_DRIVERS.md**](./ADDING_DRIVERS.md)
- 드라이버 분류 (GPIO, I2C, SPI 등)
- 단계별 개발 가이드
- 헤더 작성법
- 구현 예시 (BMP280 센서)
- 테스트 작성
- 체크리스트

**대상**: 새로운 센서/디바이스 드라이버를 개발하는 개발자

**예시 드라이버**:
- BMP280 기압 센서
- MPU6050 IMU 센서
- ACS712 전류 센서

---

#### 새 MCU로 포팅하기

[**PORTING_MCU.md**](./PORTING_MCU.md)
- 포팅 개요 및 원리
- 단계별 포팅 절차
- HAL 인터페이스 구현
- 크로스컴파일 설정
- CMake 도구체인
- 펌웨어 빌드 및 플래시
- 디버깅 팁

**대상**: 새로운 MCU 플랫폼에 libemb를 이식하려는 개발자

**예상 포팅 대상**:
- STM32F4/F7 (ARM Cortex-M4)
- Nordic nRF52 (ARM Cortex-M4)
- ESP32 (Xtensa)
- RP2040 (ARM Cortex-M0+)

---

### 🧪 테스트 및 검증

#### Mock을 통한 단위 테스트

[**ADDING_MOCKS.md**](./ADDING_MOCKS.md)
- Mock의 목적과 사용법
- MockI2c, MockUart, MockGpio 구현
- 드라이버 테스트 패턴
- 하드웨어 없이 PC에서 개발

**대상**: 드라이버를 테스트하는 개발자

---

#### 종합 테스트 전략

[**TESTING_STRATEGY.md**](./TESTING_STRATEGY.md)
- 단위 테스트 작성 가이드
- Google Test 프레임워크 사용법
- 통합 테스트 설계
- 코드 커버리지 측정 및 목표 설정
- CI/CD 테스트 자동화
- 테스트 패턴 및 best practices

**대상**: 테스트 전략을 수립하고 품질을 높이는 개발자

---

#### 성능 최적화 가이드

[**PERFORMANCE_GUIDE.md**](./PERFORMANCE_GUIDE.md)
- 각 컴포넌트의 성능 특성
- 컴파일러 최적화 레벨
- 벤치마킹 방법론
- 메모리 프로파일링 (ASAN, Valgrind)
- 임베디드 최적화 기법
- 실시간 성능 고려사항

**대상**: 성능 병목을 식별하고 최적화하는 개발자

---

### 🔧 ABOV A31G123 MCU 포팅

[**PORTING_ABOV_A31G123.md**](./PORTING_ABOV_A31G123.md)
- ABOV A31G123 보드 사양 및 핀 맵
- 클록 설정 및 계산
- 크로스컴파일 설정
- 하드웨어 초기화
- 기본 예제 (LED, UART)

**대상**: ABOV A31G123 MCU에서 libemb를 사용하는 개발자

---

### 📺 실제 하드웨어 테스트 가이드

#### LED Blink (GPIO 테스트)

[**HARDWARE_TEST_GUIDE_LED_BLINK.md**](./HARDWARE_TEST_GUIDE_LED_BLINK.md)
- GPIO 제어 기본 테스트
- 하드웨어 연결 및 배선도
- CMake 빌드 및 플래시 절차
- OpenOCD, ST-Link Utility, GDB 사용법
- 오실로스코프 측정 및 검증
- 성능 벤치마킹

**난이도**: 초급 (가장 간단한 하드웨어 검증)

---

#### UART Echo (시리얼 통신 테스트)

[**HARDWARE_TEST_GUIDE_UART_ECHO.md**](./HARDWARE_TEST_GUIDE_UART_ECHO.md)
- UART 포트 맵핑
- USB-UART 변환기 연결
- 터미널 프로그램 설정 (minicom, PuTTY, screen)
- 시리얼 통신 검증
- 성능 측정 (처리량, 정확성)
- 프로토콜 분석 및 디버깅

**난이도**: 중급 (상호작용 통신)

---

#### I2C Display (센서 통신 테스트)

[**HARDWARE_TEST_GUIDE_I2C_DISPLAY.md**](./HARDWARE_TEST_GUIDE_I2C_DISPLAY.md)
- SSD1306 OLED 디스플레이 사양
- I2C 풀-업 저항 설정
- I2C 신호 검증 (오실로스코프)
- SSD1306 초기화 및 제어
- 픽셀 데이터 전송
- 프레임 레이트 측정 (10-40 FPS)
- 디스플레이 콘트래스트 조정

**난이도**: 고급 (복잡한 프로토콜, 버퍼 관리)

---

### 🐛 문제 해결 가이드

[**TROUBLESHOOTING_ABOV_A31G123.md**](./TROUBLESHOOTING_ABOV_A31G123.md)
- 빌드 문제 진단 (10가지)
- 플래시 & 디버그 이슈
- GPIO, UART, I2C, SPI 별 문제 해결
- 메모리 및 클록 문제
- GDB 디버깅 명령어
- 바이너리 분석 도구

**대상**: 문제 발생시 참고하는 레퍼런스

---

### 📊 프로젝트 완료 보고서

[**PHASE_4_COMPLETION_SUMMARY.md**](./PHASE_4_COMPLETION_SUMMARY.md)
- ABOV A31G123 포팅 전체 요약
- 주간별 성과 분석
- 코드 품질 메트릭
- 성능 특성 및 벤치마크
- 미래 개선 사항 (Phase 5 로드맵)
- 프로젝트 통계

**대상**: 프로젝트 현황 및 성과를 보고 싶은 사람

---

## 🎓 학습 경로

### 입문자 (호스트 PC에서 학습)

1. [프로젝트 README](../README.md) - libemb 개요
2. [ARCHITECTURE.md](./ARCHITECTURE.md) - 시스템 설계 이해
3. [예제 01: RingBuffer](../examples/01_basic_ringbuffer/) - 기본 데이터 구조
4. [예제 02: UART](../examples/02_uart_ringbuffer/) - 하드웨어 인터페이스
5. [예제 03: Display Driver](../examples/03_display_driver/) - 드라이버 사용
6. [예제 04: Sensor Integration](../examples/04_sensor_integration/) - 다중 컴포넌트 시스템

### 입문자 (ABOV A31G123 보드 사용자)

1. [PORTING_ABOV_A31G123.md](./PORTING_ABOV_A31G123.md) - 보드 및 환경 설정
2. [HARDWARE_TEST_GUIDE_LED_BLINK.md](./HARDWARE_TEST_GUIDE_LED_BLINK.md) - GPIO 기본 테스트
3. [HARDWARE_TEST_GUIDE_UART_ECHO.md](./HARDWARE_TEST_GUIDE_UART_ECHO.md) - 시리얼 통신
4. [HARDWARE_TEST_GUIDE_I2C_DISPLAY.md](./HARDWARE_TEST_GUIDE_I2C_DISPLAY.md) - I2C 센서 제어
5. [TROUBLESHOOTING_ABOV_A31G123.md](./TROUBLESHOOTING_ABOV_A31G123.md) - 문제 해결

### 중급자 (드라이버 개발 & 테스트)

1. [ARCHITECTURE.md](./ARCHITECTURE.md) - 전체 구조 이해
2. [TESTING_STRATEGY.md](./TESTING_STRATEGY.md) - 테스트 전략 학습
3. [ADDING_MOCKS.md](./ADDING_MOCKS.md) - Mock 구현 이해
4. [예제 코드](../examples/) - 실제 드라이버 분석
5. [ADDING_DRIVERS.md](./ADDING_DRIVERS.md) - 새 드라이버 작성
6. 새 드라이버 구현 및 테스트

### 고급자 (MCU 포팅)

1. [ARCHITECTURE.md](./ARCHITECTURE.md) - 아키텍처 깊이 있게 학습
2. [PORTING_MCU.md](./PORTING_MCU.md) - 포팅 절차 숙지
3. STM32/nRF52 등의 MCU 포팅
4. 포팅된 플랫폼에서 드라이버 검증

---

## 📋 빌드 및 테스트

### 호스트 환경 (Linux/macOS)

```bash
# 기본 빌드 (테스트 및 예제 포함)
cmake -B build/host -DLIBEMB_BUILD_TESTS=ON -DLIBEMB_BUILD_EXAMPLES=ON
cmake --build build/host

# 모든 테스트 실행
ctest --test-dir build/host --output-on-failure

# 모든 예제 실행
./build/host/examples/01_basic_ringbuffer/basic_ringbuffer
./build/host/examples/02_uart_ringbuffer/uart_ringbuffer_demo
./build/host/examples/03_display_driver/display_driver
```

### AddressSanitizer (메모리 안전성 검사)

```bash
cmake -B build/asan \
  -DLIBEMB_BUILD_TESTS=ON \
  -DLIBEMB_ENABLE_ASAN=ON

cmake --build build/asan
ctest --test-dir build/asan --output-on-failure
```

### 크로스컴파일 (ABOV A31G123)

```bash
# ABOV A31G123 MCU 전용 빌드
cmake -B build/a31g123 \
  -DCMAKE_TOOLCHAIN_FILE=cmake/toolchains/abov-a31g123.cmake \
  -DLIBEMB_BUILD_EXAMPLES=ON \
  -DCMAKE_BUILD_TYPE=Release

cmake --build build/a31g123

# 예제 빌드 (LED Blink)
cmake --build build/a31g123 --target a31g123_blink

# 바이너리 분석
arm-none-eabi-size build/a31g123/examples/05_abov_a31g123_demo/a31g123_blink.elf
```

더 자세한 내용: [PORTING_ABOV_A31G123.md](./PORTING_ABOV_A31G123.md)

### 크로스컴파일 (STM32F4 또는 기타 ARM MCU)

```bash
cmake -B build/stm32f4 \
  -DCMAKE_TOOLCHAIN_FILE=cmake/toolchains/arm-none-eabi.cmake \
  -DLIBEMB_BUILD_TESTS=OFF

cmake --build build/stm32f4

# 바이너리 분석
arm-none-eabi-size build/stm32f4/<firmware.elf>
```

---

## 🔗 API 참조

### HAL 인터페이스

| 인터페이스 | 용도 | 파일 |
|-----------|------|------|
| `IGpio` | GPIO 제어 | `include/libemb/hal/IGpio.hpp` |
| `IUart` | 직렬 통신 | `include/libemb/hal/IUart.hpp` |
| `ISpi` | SPI 통신 | `include/libemb/hal/ISpi.hpp` |
| `II2c` | I2C 통신 | `include/libemb/hal/II2c.hpp` |

### 유틸리티

| 클래스 | 용도 | 파일 |
|--------|------|------|
| `RingBuffer<T, N>` | 고정 크기 순환 버퍼 | `include/libemb/util/RingBuffer.hpp` |
| `StaticVector<T, N>` | 동적 할당 없는 벡터 | `include/libemb/util/StaticVector.hpp` |

### 드라이버

| 드라이버 | 장치 | 프로토콜 | 파일 |
|---------|------|---------|------|
| `Ssd1306` | OLED 디스플레이 | I2C | `include/libemb/driver/ssd1306/` |

---

## 🛠️ 빌드 도구

### 요구사항

**호스트 개발**:
- GCC 또는 Clang
- CMake 3.20+
- Google Test (자동 다운로드)

**크로스컴파일**:
- arm-none-eabi-gcc
- CMake 3.20+

### 설치

```bash
# Ubuntu/Debian
sudo apt-get install cmake g++ gcc-arm-none-eabi

# macOS (Homebrew)
brew install cmake llvm arm-none-eabi-gcc

# Windows
# CMake: https://cmake.org/download/
# MinGW: https://www.mingw-w64.org/
# ARM GCC: https://developer.arm.com/
```

---

## 📊 프로젝트 통계

| 항목 | 값 |
|------|-----|
| **언어** | C++17 |
| **총 코드 라인 수** | ~9,500 lines |
| **라이브러리 코드** | ~3,000 lines |
| **드라이버 구현** | ~1,500 lines (GPIO, UART, I2C, SPI) |
| **예제 애플리케이션** | ~420 lines (5개 예제) |
| **문서** | ~4,000 lines (12개 가이드) |
| **동적 할당** | 0 (금지) |
| **테스트 커버리지** | >80% |
| **최소 바이너리 크기** | <20 KB (LED Blink) |
| **최대 바이너리 크기** | ~8 KB (전체 드라이버) |
| **컴파일 경고** | 0 (-Wall -Wextra -Wpedantic) |
| **지원 MCU** | ABOV A31G123 (Cortex-M0+) |
| **지원 인터페이스** | GPIO, UART, I2C, SPI |

---

## 🤝 기여 가이드

### 버그 보고

[GitHub Issues](https://github.com/libemb/libemb/issues)에서:
1. 라벨 선택: `bug`, `feature`, `documentation`
2. 상세 설명 (재현 단계, 예상/실제 동작)
3. 환경 정보 (OS, 컴파일러 버전, CMake 버전)

### 풀 리퀘스트

1. Fork 및 feature 브랜치 생성
2. 변경 사항 commit (의미 있는 메시지)
3. 테스트 추가 (>80% 커버리지)
4. 코드 리뷰 요청

**코드 스타일**:
- C++17 표준 준수
- 함수/클래스 이름: camelCase
- 상수: UPPER_CASE
- Private 멤버: snake_case_

---

## 📞 기술 지원

### FAQ

**Q: ABOV A31G123에서 어떻게 시작하나요?**
A: [PORTING_ABOV_A31G123.md](./PORTING_ABOV_A31G123.md)를 읽고, [HARDWARE_TEST_GUIDE_LED_BLINK.md](./HARDWARE_TEST_GUIDE_LED_BLINK.md)부터 시작하세요.

**Q: 실제 보드가 없으면 어떻게 하나요?**
A: 호스트 PC에서 Mock HAL을 사용해 테스트할 수 있습니다. [TESTING_STRATEGY.md](./TESTING_STRATEGY.md) 참고.

**Q: 메모리가 부족합니다 (32KB RAM)**
A: `StaticVector` 크기를 줄이거나, 필요한 드라이버만 링크하세요. 또는 더 큰 MCU로 업그레이드하세요.

**Q: I2C 통신이 작동하지 않습니다**
A: [HARDWARE_TEST_GUIDE_I2C_DISPLAY.md](./HARDWARE_TEST_GUIDE_I2C_DISPLAY.md)의 "풀-업 저항" 섹션과 [TROUBLESHOOTING_ABOV_A31G123.md](./TROUBLESHOOTING_ABOV_A31G123.md)의 I2C 문제 해결을 확인하세요.

**Q: UART 보드율이 틀렸습니다**
A: [TROUBLESHOOTING_ABOV_A31G123.md](./TROUBLESHOOTING_ABOV_A31G123.md)에서 "UART Issues"의 보드율 계산 방법을 참고하세요.

**Q: STM32 외에 다른 MCU도 지원하나요?**
A: ABOV A31G123은 완전히 지원됩니다. 다른 MCU는 [PORTING_MCU.md](./PORTING_MCU.md)를 따라 포팅할 수 있습니다.

### 연락처

- 📧 이메일: [support@libemb.dev](mailto:support@libemb.dev)
- 🐛 이슈 추적: [GitHub Issues](https://github.com/libemb/libemb/issues)
- 💬 토론: [GitHub Discussions](https://github.com/libemb/libemb/discussions)

---

## 📄 라이선스

이 프로젝트는 MIT 라이선스를 따릅니다. 자세한 내용은 [LICENSE](../LICENSE) 파일을 참고하세요.

---

## 🗺️ 로드맵

### ✅ Phase 4 완료 (2026년 3월)
- [x] ABOV A31G123 MCU 완전 포팅
  - [x] GPIO, UART, I2C, SPI 구현
  - [x] 3개 예제 애플리케이션
  - [x] 포괄적 문서 (12개 가이드)
  - [x] 하드웨어 테스트 가이드
  - [x] 문제 해결 가이드
- [x] CMake 빌드 시스템 최적화
- [x] 완전한 API 문서화

### 📋 Phase 5 계획 (2026년 Q2)

**MCU 포팅**
- [ ] Mindmotion MM32G0001 (8KB RAM, 저비용 버전)
- [ ] STM32F4xx (고성능, 1MB Flash)
- [ ] Nordic nRF52840 (Bluetooth, 고급 기능)

**드라이버 확장**
- [ ] BMP280 기압 센서
- [ ] MPU6050 IMU (6축 가속도계)
- [ ] ADS1115 ADC
- [ ] PWM 타이머 인터페이스
- [ ] ADC 변환 인터페이스

**개발 인프라**
- [ ] GitHub Actions CI/CD
- [ ] 자동 테스트 및 빌드 검증
- [ ] 코드 커버리지 리포팅
- [ ] 성능 벤치마크 자동화

### 🎯 Phase 6 비전 (2026년 Q3+)

**고급 기능**
- [ ] DMA 지원 (고속 데이터 전송)
- [ ] RTC / WDT 드라이버
- [ ] 저전력 모드 관리
- [ ] Bootloader 구현

**생태계 확장**
- [ ] Arduino 호환성 레이어
- [ ] PlatformIO 통합
- [ ] ESPressif ESP32 지원
- [ ] RISC-V 마이크로컨트롤러

---

**마지막 업데이트**: 2026년 3월 16일

[메인 README로 돌아가기](../README.md)
