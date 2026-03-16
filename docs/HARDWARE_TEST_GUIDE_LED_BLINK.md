# LED Blink 예제: 실제 하드웨어 테스트 가이드

ABOV A31G123 개발 보드에서 LED를 제어하는 완전한 하드웨어 테스트 절차.

## 📋 사전 준비 확인

### 하드웨어 체크리스트

```
□ ABOV A31G123 개발 보드
□ ST-Link v2 (또는 호환 디버거)
□ USB 케이블 (A타입 ↔ Micro/Mini B)
□ LED (일반적인 5mm 또는 3mm)
□ 전류 제한 저항 (220Ω ~ 470Ω)
□ 브레드보드 또는 배선
□ 컴퓨터 (Linux/macOS/Windows)
```

### 소프트웨어 필수 프로그램

```
✓ arm-none-eabi-gcc (ARM 크로스 컴파일러)
✓ arm-none-eabi-gdb (디버거)
✓ CMake 3.20+
✓ OpenOCD (또는 ST-Link Utility)
✓ make / ninja
```

**설치 방법:**

```bash
# Ubuntu/Debian
sudo apt-get install -y \
  arm-none-eabi-gcc \
  arm-none-eabi-gdb \
  arm-none-eabi-newlib \
  build-essential \
  cmake \
  openocd

# macOS (Homebrew)
brew install arm-none-eabi-gcc cmake openocd

# Windows (MSYS2)
pacman -S mingw-w64-x86_64-arm-none-eabi-gcc cmake openocd
```

---

## 🔧 Step 1: 하드웨어 연결

### 1.1 LED 연결도

```
ABOV A31G123 Board
┌────────────────────────┐
│                        │
│  PA5 (GPIO OUT)  ────┬─┘
│                       │
│              ┌────────┘
│              │
│              │     [LED Anode (+)]
│              │    /
│         ┌────┴────┐
│         │ [220Ω]  │
│         │         │
│         │        GND ───── GND (Board)
│         │
│        [5mm LED]
│         │
│         │ (Cathode (-))
│
│   GND ────────────────────── GND
│
└────────────────────────┘

연결 요약:
  PA5 → 저항(220Ω) → LED(+) → LED(-) → GND
  GND → GND
```

### 1.2 디버거 연결

```
ST-Link v2 / JLINK
┌─────────────────────┐
│ 1. VCC (3.3V)       │  ──→ ABOV Board VCC
│ 2. GND              │  ──→ ABOV Board GND
│ 3. SWDIO            │  ──→ ABOV Board SWDIO (PA13)
│ 4. SWCLK            │  ──→ ABOV Board SWCLK (PA14)
└─────────────────────┘

[USB] ──── [ST-Link] ──── [ABOV Board]
            (확인: USB 전원 LED 켜짐)
```

### 1.3 전원 연결

```
ABOV Board Power
  ┌─────────────────────┐
  │ VCC (3.3V) ━━━━━━ USB 또는 외부 전원
  │ GND ━━━━━━━━━━━━━ GND
  └─────────────────────┘

권장: USB 전원 또는 안정적인 3.3V 전원 공급
```

---

## 🛠️ Step 2: 빌드 시스템 설정

### 2.1 소스 코드 확인

LED Blink 예제 위치:

```bash
ls -la examples/05_abov_a31g123_demo/
├── blink.cpp          # LED 제어 코드
├── uart_echo.cpp      # UART 에코 코드
├── i2c_display.cpp    # I2C 디스플레이 코드
└── CMakeLists.txt     # 빌드 설정
```

### 2.2 소스 코드 확인

```cpp
// examples/05_abov_a31g123_demo/blink.cpp

#include "libemb/port/abov_a31g123/A31G123Gpio.hpp"
#include <cstdint>

int main() {
    // PA5 핀을 GPIO 출력으로 설정
    libemb::port::abov_a31g123::A31G123Gpio led(GPIOA_BASE, 5);
    led.setMode(libemb::hal::GpioMode::OUTPUT);

    // 무한 루프: LED 켰다 껐다 반복
    while (true) {
        // LED 켜기 (HIGH)
        led.write(libemb::hal::GpioState::HIGH);

        // 약 500ms 대기 (for 루프로 구현)
        for (volatile uint32_t i = 0; i < 17500000; ++i) {
            __asm("nop");  // 아무 작업 안 함 (CPU 시간 소비)
        }

        // LED 끄기 (LOW)
        led.write(libemb::hal::GpioState::LOW);

        // 약 500ms 대기
        for (volatile uint32_t i = 0; i < 17500000; ++i) {
            __asm("nop");
        }
    }

    return 0;
}
```

**중요**: 계산 값 `17500000`은 70MHz CPU에서 약 250ms입니다 (정확도: ±5%)

---

## 🏗️ Step 3: 프로젝트 빌드

### 3.1 CMake 구성

```bash
# 프로젝트 디렉토리로 이동
cd /path/to/libemb

# ABOV A31G123용 빌드 디렉토리 생성
cmake -B build/a31g123 \
  -DCMAKE_TOOLCHAIN_FILE=cmake/toolchains/abov-a31g123.cmake \
  -DLIBEMB_BUILD_EXAMPLES=ON \
  -DCMAKE_BUILD_TYPE=Release
```

**예상 출력:**
```
-- The C compiler identification is GNU X.X.X
-- The CXX compiler identification is GNU X.X.X
-- Detecting CXX compiler ABI info
-- Build files have been written to: /path/to/build/a31g123
```

### 3.2 컴파일

```bash
# LED Blink 예제만 빌드
cmake --build build/a31g123 --target a31g123_blink

# 또는 모든 예제 빌드
cmake --build build/a31g123
```

**예상 결과:**
```
[10%] Building CXX object...
[20%] Linking CXX executable a31g123_blink.elf
[100%] Built target a31g123_blink
```

### 3.3 빌드 산물 확인

```bash
ls -lah build/a31g123/examples/05_abov_a31g123_demo/

-rw-r--r-- 1 user user 2.2K a31g123_blink.elf
-rw-r--r-- 1 user user 1.1K a31g123_blink.hex
-rw-r--r-- 1 user user 500B  a31g123_blink.bin
```

**3가지 포맷:**
- **`.elf`** ← GDB 디버깅용 (심볼 정보 포함)
- **`.hex`** ← Intel HEX 형식 (플래시 프로그래머용)
- **`.bin`** ← Raw 바이너리 (부트로더용)

### 3.4 바이너리 정보 확인

```bash
# ELF 파일 분석
arm-none-eabi-size build/a31g123/examples/05_abov_a31g123_demo/a31g123_blink.elf

   text    data     bss     dec     hex filename
   2156      16     264    2436     984 a31g123_blink.elf
```

**설명:**
- **text**: 프로그램 코드 크기 (Flash에 저장, 2156 B = 2.1 KB)
- **data**: 초기화된 데이터 (Flash에서 RAM으로 복사, 16 B)
- **bss**: 초기화되지 않은 데이터 (RAM, 264 B)
- **dec/hex**: 총 크기

**용량 확인:**
```
프로그램 크기: 2156 B (2.1 KB)
사용 가능: 128 KB Flash
사용률: 1.6% ✓ (충분함)

RAM 사용: 264 B
사용 가능: 32 KB RAM
사용률: 0.8% ✓ (충분함)
```

---

## 🔌 Step 4: 보드 플래싱

### 방법 A: OpenOCD 사용 (권장)

#### A1. OpenOCD 설정 파일 생성

보드의 오픈 JTAG/SWD 인터페이스에 따라 선택:

**파일: `openocd_a31g123.cfg`**

```tcl
# ST-Link v2 호환 디버거 사용
source [find interface/stlink.cfg]

# A31G123은 Cortex-M0+ (STM32F0 유사 구조)
source [find target/stm32f0x.cfg]

# Flash 크기 설정 (A31G123은 128KB)
set FLASH_SIZE 0x20000

# 초기화 후 자동 할당
init
targets 0
```

#### A2. OpenOCD 시작

```bash
# OpenOCD 데몬 시작
openocd -f openocd_a31g123.cfg

# 또는 로그 파일과 함께
openocd -f openocd_a31g123.cfg -d2 > openocd.log 2>&1 &

# 예상 출력:
# Info : OpenOCD X.X.X
# Info : JTAG tap: stm32f0x.cpu tap/id: 0xXXXXXXXX
# Info : stm32f0x.cpu: hardware has 2 breakpoint/watchpoint units
```

#### A3. GDB로 플래시

```bash
# 터미널 2에서 GDB 시작
arm-none-eabi-gdb build/a31g123/examples/05_abov_a31g123_demo/a31g123_blink.elf

(gdb) target extended-remote localhost:3333
Remote debugging using localhost:3333
0x00000000 in ?? ()

(gdb) monitor reset halt
target state: halted
target halted due to debug-request, current mode: Thread
xPSR: 0xc1000000 pc: 0x00000000 msp: 0xFFFFFFFF

(gdb) load
Loading section .text, size 0x86c lma 0x0
Loading section .data, size 0x10 lma 0x86c
Start address: 0x00000000, load size: 2172

(gdb) monitor reset run
target state: running

(gdb) quit
```

**설명:**
1. `target extended-remote localhost:3333` - OpenOCD의 GDB 서버에 연결
2. `monitor reset halt` - CPU 정지 및 초기화
3. `load` - 프로그램을 Flash에 복사
4. `monitor reset run` - CPU 재시작 (프로그램 실행)

### 방법 B: ST-Link Utility 사용 (Windows GUI)

```
1. ST-Link Utility 실행
   (또는 STM32CubeProgrammer)

2. Target → Connect
   (보드 연결 확인)

3. File → Open File
   → build/a31g123/.../a31g123_blink.hex 선택

4. Target → Program
   (플래시 시작)

5. Verify가 완료되면 "Programming completed successfully"
```

### 방법 C: arm-none-eabi-gdb 스크립트 (자동화)

**파일: `flash.gdb`**

```gdb
# OpenOCD에 연결
target extended-remote localhost:3333

# 보드 초기화
monitor reset halt

# 타이밍 조정
set mem inaccessible-by-default off

# Flash 크기 설정
monitor flash banks

# 프로그램 로드
load

# 검증
compare-sections

# 재시작
monitor reset run

# 종료
quit
```

**실행:**

```bash
# OpenOCD 시작
openocd -f openocd_a31g123.cfg &

# GDB 스크립트 실행
sleep 2  # OpenOCD 초기화 대기
arm-none-eabi-gdb \
  -ex "file build/a31g123/examples/05_abov_a31g123_demo/a31g123_blink.elf" \
  -x flash.gdb
```

---

## ✅ Step 5: 하드웨어에서 테스트

### 5.1 LED 동작 확인

**예상 동작:**
```
┌─────────────┐
│ LED OFF     │  (0.5초)
│             │
├─────────────┤
│ LED ON      │  (0.5초) ← 밝음 (점등)
│             │
├─────────────┤
│ LED OFF     │  (0.5초)
│             │
└─────────────┘

시간 → (1초마다 반복)
```

**확인 방법:**
1. LED가 규칙적으로 깜박이는지 관찰
2. 주기가 약 1초인지 측정 (0.5초 ON + 0.5초 OFF)
3. 밝기가 일정한지 확인

### 5.2 문제 해결

| 증상 | 원인 | 해결 |
|------|------|------|
| LED가 켜지지 않음 | 1. 극성 반대 | LED 방향 교체 |
| | 2. 저항값 너무 큼 | 저항값 확인 (220Ω 권장) |
| | 3. 플래시 실패 | GDB 로그 확인 |
| LED가 항상 켜짐 | 1. 코드 오류 | 플래시 재시도 |
| | 2. GPIO 설정 오류 | 레지스터 값 확인 |
| LED가 깜박이지 않음 | 1. 타이밍 값 오류 | 루프 카운트 조정 |
| | 2. CPU 속도 다름 | 칼리브레이션 재수행 |

### 5.3 GDB로 디버깅

```bash
# GDB 시작 (OpenOCD 배경 실행)
arm-none-eabi-gdb build/a31g123/examples/05_abov_a31g123_demo/a31g123_blink.elf

(gdb) target extended-remote localhost:3333

# 레지스터 값 읽기
(gdb) print *(uint32_t*)0x40040004
$1 = 0x20        # PA5가 HIGH (비트 5 = 0x20)

# 메모리 맵 확인
(gdb) info mem

# 실행 중인 코드 확인
(gdb) disassemble main
(gdb) stepi     # 한 명령어씩 실행
(gdb) continue  # 계속 실행

# 중단점 설정
(gdb) break main
(gdb) run

# 스택 상태
(gdb) info stack
(gdb) bt        # 백트레이스
```

### 5.4 성능 측정

**방법 1: 오실로스코프 사용 (정확)**

```
PA5 핀을 오실로스코프 채널 1에 연결
- 예상 주파수: 1 Hz (1초 주기)
- 파형: 정사각형파 (square wave)
- 듀티 사이클: 50% (0.5초 ON, 0.5초 OFF)
- 전압: 0V ↔ 3.3V
```

**방법 2: 로직 애널라이저 사용**

```
PA5 핀을 로직 애널라이저에 연결
- 캡처: 2초 이상
- 샘플링 레이트: 1 MHz 이상
- 분석: 펄스 주기 및 듀티 측정
```

**방법 3: 저속 카운터**

```cpp
// 수정된 코드로 1Hz보다 느린 LED 제어
// (카운팅이 쉽게 하기 위해)

// 대신 10초 주기로 설정:
// 5초 ON, 5초 OFF

// 루프 카운트: 17500000 → 350000000 (20배)
```

---

## 📊 Step 6: 성능 검증 보고서

테스트 후 이 정보를 기록하세요:

```markdown
# LED Blink 테스트 보고서

## 하드웨어 정보
- 보드 모델: ABOV A31G123
- 디버거: ST-Link v2
- 날짜: 2026-03-16

## 빌드 정보
- 컴파일러: arm-none-eabi-gcc (버전)
- 코드 크기: 2156 B
- RAM 사용: 264 B
- 빌드 시간: XXs

## 테스트 결과

### LED 동작
- [ ] LED가 깜박인다
- [ ] 주기가 약 1초다 (±10%)
- [ ] 밝기가 일정하다

### 측정값
- 실제 주기: ___ ms
- 듀티 사이클: ___ %
- 전압 범위: 0V ~ 3.3V

### 결론
- [ ] 성공 ✓
- [ ] 실패 (이유: ___)
```

---

## 🎯 다음 단계

LED Blink 테스트 후:

1. **UART Echo** - 시리얼 통신 테스트
   ```bash
   cmake --build build/a31g123 --target a31g123_uart_echo
   ```

2. **I2C Display** - 센서 통신 테스트
   ```bash
   cmake --build build/a31g123 --target a31g123_i2c_display
   ```

3. **자신의 프로젝트** - 실제 애플리케이션 개발

---

## 📚 추가 정보

### 추천 참고 자료

- `docs/TROUBLESHOOTING_ABOV_A31G123.md` - 문제 해결
- `docs/PORTING_ABOV_A31G123.md` - 하드웨어 상세 정보
- `include/libemb/hal/IGpio.hpp` - GPIO API 레퍼런스
- `examples/05_abov_a31g123_demo/` - 전체 소스 코드

### 일반적인 문제들

**Q: 플래시가 안 됩니다**
A: OpenOCD 연결을 확인하세요
   ```bash
   ps aux | grep openocd
   # openocd 프로세스가 실행 중이어야 함
   ```

**Q: 칼리브레이션이 필요합니다**
A: 정확한 타이밍을 위해:
   ```cpp
   // 현재: 250ms delay (17500000 루프)
   // 실제 측정 주기가 500ms가 아니면, 루프 카운트를 조정하세요
   // 공식: new_count = old_count * (measured_delay / 500ms)
   ```

### 연락처

문제가 생기면:
- `docs/TROUBLESHOOTING_ABOV_A31G123.md`에서 해답 찾기
- GitHub Issues에 보고
- 커뮤니티 포럼 참여

---

**Happy Embedded Development! 🚀**

이 가이드를 완료하면, 실제 하드웨어에서 첫 프로그램을 성공적으로 실행할 수 있습니다!
