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

## 🎓 학습 경로

### 입문자

1. [프로젝트 README](../README.md) - libemb 개요
2. [예제 01: RingBuffer](../examples/01_basic_ringbuffer/) - 기본 데이터 구조
3. [예제 02: UART](../examples/02_uart_ringbuffer/) - 하드웨어 인터페이스
4. [예제 03: Display Driver](../examples/03_display_driver/) - 드라이버 사용

### 중급자 (드라이버 개발)

1. [ARCHITECTURE.md](./ARCHITECTURE.md) - 전체 구조 이해
2. [예제 코드](../examples/) - 실제 드라이버 분석
3. [ADDING_DRIVERS.md](./ADDING_DRIVERS.md) - 새 드라이버 작성
4. 새 드라이버 구현 및 테스트

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

### 크로스컴파일 (STM32F4)

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
| **코드 라인 수** | ~3,000 lines |
| **동적 할당** | 0 (금지) |
| **테스트 커버리지** | >80% |
| **최소 바이너리 크기** | <20 KB |

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

**Q: 메모리가 부족합니다**
A: `StaticVector` 크기를 줄이거나, 필요한 드라이버만 링크하세요.

**Q: 테스트 없이 실제 하드웨어에서만 개발할 수 있나요?**
A: 가능하지만, Mock을 사용한 개발을 강력 추천합니다.

**Q: STM32 외에 다른 MCU도 지원하나요?**
A: 아직 STM32만 공식 지원합니다. 다른 MCU는 [PORTING_MCU.md](./PORTING_MCU.md)를 따라 포팅할 수 있습니다.

### 연락처

- 📧 이메일: [support@libemb.dev](mailto:support@libemb.dev)
- 🐛 이슈 추적: [GitHub Issues](https://github.com/libemb/libemb/issues)
- 💬 토론: [GitHub Discussions](https://github.com/libemb/libemb/discussions)

---

## 📄 라이선스

이 프로젝트는 MIT 라이선스를 따릅니다. 자세한 내용은 [LICENSE](../LICENSE) 파일을 참고하세요.

---

## 🗺️ 로드맵

### 2025년 Q1
- [ ] STM32F4 포팅 공식 지원
- [ ] BMP280 센서 드라이버
- [ ] 성능 벤치마크 도구

### 2025년 Q2
- [ ] Nordic nRF52 포팅
- [ ] MPU6050 드라이버
- [ ] CI/CD 파이프라인 (GitHub Actions)

### 2025년 Q3
- [ ] ESP32 포팅
- [ ] 추가 드라이버 (WiFi, Bluetooth)
- [ ] 통합 테스트 프레임워크

---

**마지막 업데이트**: 2026년 3월

[메인 README로 돌아가기](../README.md)
