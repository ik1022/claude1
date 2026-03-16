# UART Echo 예제: 실제 하드웨어 시리얼 통신 테스트

ABOV A31G123에서 UART를 사용한 시리얼 통신 완전 가이드.

## 📋 사전 준비

### 하드웨어 체크리스트

```
□ ABOV A31G123 개발 보드
□ ST-Link v2 디버거
□ USB-to-UART 변환기 (FTDI 또는 CH340)
  또는 보드의 내장 UART 포트
□ 멀티 점프 와이어
□ 터미널 프로그램 (minicom, PuTTY, screen)
```

### 소프트웨어

```
✓ arm-none-eabi-gcc / gdb
✓ CMake
✓ OpenOCD
✓ 터미널 프로그램:
  - Linux: minicom, screen, picocom
  - macOS: minicom, screen
  - Windows: PuTTY, TeraTerm, RealTerm
```

---

## 🔧 Step 1: UART 하드웨어 연결

### 1.1 UART 포트 맵

ABOV A31G123은 4개의 UART를 지원합니다:

```
UART0 (사용 권장)
  TX: PA2 (Data Out)
  RX: PA3 (Data In)
  Speed: 115200 baud (기본값)

UART1
  TX: PA6
  RX: PA7
  Speed: 115200 baud

UART2
  TX: PA12
  RX: PA13
  Speed: 115200 baud

UART3
  TX: PC2
  RX: PC3
  Speed: 115200 baud
```

### 1.2 USB-UART 변환기 연결

```
ABOV Board              USB-UART Converter
────────────────────────────────────────

PA2 (TX) ────────────→ RX (입력)
PA3 (RX) ←──────────── TX (출력)
GND ─────────────────→ GND (공통 접지)
VCC ─────────────────→ VCC (선택, 전원용)
                       USB → PC

예시 (FTDI FT232RL):
  ┌─────────────────┐
  │ GND  RX  TX  VCC│
  │  │    │   │   │ │
  │  └─┬──┬───┬───┘
  └────┼──┼───┼─────────
       │  │   │
      GND│   └─→ PA2 (UART0 TX)
        └────→ PA3 (UART0 RX)
```

### 1.3 USB 드라이버 설치

**Linux:**
```bash
# 대부분 자동으로 인식됨
# 확인:
ls -l /dev/ttyUSB*
# 출력 예: /dev/ttyUSB0
```

**macOS:**
```bash
# FTDI 드라이버 필요할 수 있음
brew install ftdi-eeprom
# 확인:
ls -l /dev/tty.usbserial*
```

**Windows:**
- FTDI: https://ftdichip.com/drivers/
- CH340: https://github.com/WCHSoftware/ch341ser

---

## 🏗️ Step 2: 소스 코드 확인

### 2.1 UART Echo 예제 코드

```cpp
// examples/05_abov_a31g123_demo/uart_echo.cpp

#include "libemb/port/abov_a31g123/A31G123Uart.hpp"
#include <cstdint>

int main() {
    // UART0 초기화 (115200 baud)
    libemb::port::abov_a31g123::A31G123Uart uart(0, 115200);

    // 시작 메시지
    uart.write((uint8_t*)"[UART Echo Server]\r\n", 20);
    uart.write((uint8_t*)"Type something: ", 16);

    while (true) {
        // 데이터가 도착했는지 확인
        if (uart.isDataAvailable()) {
            // 한 바이트 읽기
            uint8_t byte = uart.getChar();

            // Echo: 받은 데이터를 그대로 다시 전송
            uart.putChar(byte);

            // 특수 키 처리
            if (byte == '\r') {  // Enter 키
                uart.putChar('\n');
                uart.write((uint8_t*)"> ", 2);  // 프롬프트
            }
        }
    }

    return 0;
}
```

### 2.2 UART API 레퍼런스

```cpp
// 초기화
A31G123Uart uart(port, baudrate);
// port: 0-3 (UART0-3)
// baudrate: 9600, 19200, 38400, 115200 등

// 송신 (blocking)
uart.putChar(byte);           // 1바이트
uart.write(buffer, length);   // 여러 바이트

// 수신 (non-blocking)
if (uart.isDataAvailable()) {
    uint8_t byte = uart.getChar();  // 1바이트
}

// 수신 버퍼 상태
bool has_data = uart.isDataAvailable();
size_t count = uart.available();  // 대기 중인 바이트 수
```

---

## 🛠️ Step 3: 빌드 및 플래시

### 3.1 빌드

```bash
cd /path/to/libemb

# 전체 빌드 설정 (LED 이미 빌드됨)
cmake -B build/a31g123 \
  -DCMAKE_TOOLCHAIN_FILE=cmake/toolchains/abov-a31g123.cmake \
  -DLIBEMB_BUILD_EXAMPLES=ON \
  -DCMAKE_BUILD_TYPE=Release

# UART Echo 빌드
cmake --build build/a31g123 --target a31g123_uart_echo
```

### 3.2 플래시 (OpenOCD)

```bash
# 터미널 1: OpenOCD 시작 (백그라운드)
openocd -f openocd_a31g123.cfg &

# 터미널 2: GDB로 플래시
sleep 1
arm-none-eabi-gdb -ex "file build/a31g123/examples/05_abov_a31g123_demo/a31g123_uart_echo.elf" \
                  -ex "target extended-remote localhost:3333" \
                  -ex "monitor reset halt" \
                  -ex "load" \
                  -ex "monitor reset run" \
                  -ex "quit"
```

**또는 GDB 인터랙티브:**

```bash
arm-none-eabi-gdb build/a31g123/examples/05_abov_a31g123_demo/a31g123_uart_echo.elf

(gdb) target extended-remote localhost:3333
(gdb) load
(gdb) monitor reset run
(gdb) quit
```

---

## 💻 Step 4: 터미널 설정 및 테스트

### 4.1 미니콤(minicom) 설정 - Linux/macOS

```bash
# 설치 (처음 1회)
sudo apt-get install minicom  # Linux
brew install minicom           # macOS

# 실행
minicom -D /dev/ttyUSB0 -b 115200

# 또는
minicom -D /dev/tty.usbserial-XXXX -b 115200

# 설정 메뉴 진입: Ctrl+A, Z
# Serial port setup: Ctrl+A, Z, O
# - Baud rate: 115200
# - Data bits: 8
# - Stop bits: 1
# - Parity: None
# - Flow control: None

# 종료: Ctrl+A, X (Yes)
```

### 4.2 Screen 사용 - Linux/macOS

```bash
# 간단한 연결
screen /dev/ttyUSB0 115200

# 또는
screen /dev/tty.usbserial-XXXX 115200

# 종료: Ctrl+A, :quit
```

### 4.3 PuTTY - Windows

```
1. PuTTY 실행
2. Connection Type: Serial
3. Serial line: COM3 (또는 해당 포트)
4. Speed: 115200
5. Data bits: 8
6. Stop bits: 1
7. Parity: None
8. Flow control: None
9. Open
```

---

## ✅ Step 5: 상호작용 테스트

### 5.1 기본 테스트

**터미널에 표시되는 것:**

```
[UART Echo Server]
Type something: |
```

커서가 깜박이면 UART 통신 성공! ✓

### 5.2 테스트 절차

```bash
# 1. "Hello" 입력 후 Enter
Type something: Hello↵
Hellp>

# 2. 각 문자가 에코되는지 확인
# 입력: "test"
# 출력: "test" (각 문자가 즉시 표시됨)

# 3. 특수 문자 테스트
# Backspace (제어 문자): 화면에 표시 안 될 수 있음
# Enter (CR, 0x0D): 새 줄 + 프롬프트

# 4. 연속 입력 테스트
Type something: >The quick brown fox jumps over lazy dog↵
T
h
e

q
u
i
c
k

...

>
```

### 5.3 예상 동작

| 입력 | 출력 | 설명 |
|------|------|------|
| `A` | `A` | 일반 문자는 에코 |
| `1` | `1` | 숫자도 에코 |
| `@` | `@` | 특수 문자도 에코 |
| `\r` (Enter) | `\r\n> ` | 줄바꿈 + 프롬프트 |
| `\x00` (NULL) | `\x00` | 바이트 에코 |

---

## 🔍 Step 6: 고급 테스트

### 6.1 성능 측정

```bash
# 처리량 테스트
# 128바이트를 빠르게 전송하고 에코 시간 측정

# 방법: 파일 전송으로 테스트
(echo "This is a test message for UART echo performance";
 for i in {1..10}; do
   echo "Line $i: The quick brown fox jumps over the lazy dog"
 done) > test.txt

# 오시콤에서 파일 전송
minicom -D /dev/ttyUSB0 -b 115200

# (minicom 내에서)
# Ctrl+A, S (파일 전송)
# test.txt 선택
# 전송 시작

# 측정: 1000바이트 에코 시간 ≈ 100ms @ 115200 baud
# 계산: 1000바이트 × 8비트 / 115200 = 69ms (송수신)
```

### 6.2 정확성 검증

```bash
# 체크섬 검증
# 1. 100바이트 시퀀스 생성
# 2. 에코된 데이터와 비교
# 3. 모든 바이트가 정확하게 전송되었는지 확인

python3 << 'EOF'
import serial
import sys

uart = serial.Serial('/dev/ttyUSB0', 115200, timeout=1)

# 테스트 메시지
test_data = b'Hello, UART! This is a test. \x00\x01\x02\xff'

# 전송
uart.write(test_data)

# 수신
received = uart.read(len(test_data))

# 비교
if received == test_data:
    print("✓ Perfect match!")
else:
    print("✗ Mismatch:")
    print(f"  Sent:     {test_data.hex()}")
    print(f"  Received: {received.hex()}")

    # 차이 분석
    for i, (s, r) in enumerate(zip(test_data, received)):
        if s != r:
            print(f"  Byte {i}: sent 0x{s:02x}, got 0x{r:02x}")

uart.close()
EOF
```

### 6.3 GDB에서 UART 상태 확인

```bash
arm-none-eabi-gdb build/a31g123/examples/05_abov_a31g123_demo/a31g123_uart_echo.elf

(gdb) target extended-remote localhost:3333

# UART 레지스터 읽기 (기본 주소: 0x40030000 for UART0)
(gdb) set mem inaccessible-by-default off

# UART 상태 레지스터
(gdb) print *(uint32_t*)0x40030008
$1 = 0x01        # TXE = 1 (TX 버퍼 비어있음)

# UART 데이터 레지스터
(gdb) print *(uint32_t*)0x40030000
$2 = 0x48        # 'H' (0x48)

# RX 버퍼 크기 확인
(gdb) print uart_instance->rx_buffer_.size()
```

---

## 📊 Step 7: 문제 해결

### 문제 1: 아무것도 나타나지 않음

**진단:**

```bash
# 1. 터미널이 올바른 포트에 연결되었는지 확인
ls -l /dev/ttyUSB*
# 여러 포트가 있으면 올바른 포트를 선택했는지 확인

# 2. 보드율이 맞는지 확인
minicom -D /dev/ttyUSB0 -b 115200

# 3. 플래시가 제대로 되었는지 확인
arm-none-eabi-gdb (위의 GDB 절차 반복)
```

**해결:**

```bash
# USB 포트 재연결
# 또는
sudo systemctl restart udev

# 터미널 재시작
minicom -D /dev/ttyUSB0 -b 115200 -Z
```

### 문제 2: 가비지 출력 (읽을 수 없는 문자)

**원인:** 보드율 불일치

```bash
# 현재 설정 확인
# 예: "Hello" 대신 "☺ℌℓℓö" 표시

# 해결: 다른 보드율로 시도
minicom -D /dev/ttyUSB0 -b 9600      # 너무 느림
minicom -D /dev/ttyUSB0 -b 57600     # 중간값
minicom -D /dev/ttyUSB0 -b 115200    # 표준 (권장)

# 코드에서 보드율 확인
grep -n "115200\|57600\|9600" examples/05_abov_a31g123_demo/uart_echo.cpp
```

### 문제 3: 입력이 에코되지 않음

**진단:**

```bash
# 1. TX 케이블 확인 (보드 TX → USB RX)
# 2. RX 케이블 확인 (보드 RX ← USB TX)

# 3. 한 방향 통신만 되는지 확인
# - 보드 메시지는 보이지만 입력 에코 없음 → RX 문제
# - 메시지 없고 입력도 에코 없음 → TX 문제

# 4. 핀 확인
grep -n "PA2\|PA3" port/abov_a31g123/uart/A31G123Uart.cpp
```

**해결:**

```bash
# 케이블 교체 (TX ↔ RX 스왑 확인)
# 또는 코드에서 핀 확인
# uart 초기화 위치: 대부분 UART0 (PA2=TX, PA3=RX)
```

---

## 🎯 Step 8: 고급 기능 테스트

### 8.1 여러 라인 프롬프트

코드를 수정하면:

```cpp
// 각 라인마다 숫자 표시
uart.write((uint8_t*)"Line 1: ", 8);

// 또는 명령어 프롬프트
uart.write((uint8_t*)"cmd> ", 5);

// 입력 받고 처리
while (uart.isDataAvailable()) {
    uint8_t cmd = uart.getChar();

    if (cmd == 'L') {
        uart.write((uint8_t*)"LED ON\r\n", 8);
    }
    else if (cmd == 'l') {
        uart.write((uint8_t*)"LED OFF\r\n", 9);
    }
}
```

### 8.2 인터럽트 기반 수신

현재 구현은 RX 인터럽트를 사용합니다:

```cpp
// UART 인터럽트 핸들러
extern "C" void UART0_IRQHandler() {
    // 레지스터에서 데이터 읽음
    // RingBuffer에 저장 (256바이트 큐)
    // 메인 루프: non-blocking 수신
}
```

**이점:**
- 메인 루프가 I/O에서 차단되지 않음
- 256바이트까지 버퍼링 (빠른 입력 처리)
- 다른 작업과 병렬로 수신 가능

---

## 📝 테스트 보고서

**UART Echo 테스트 보고서 템플릿:**

```markdown
# UART Echo 테스트 보고서

## 환경
- 날짜: ___
- 보드: ABOV A31G123
- USB 변환기: ___ (FTDI/CH340)
- PC OS: ___

## 테스트 결과

### 기본 통신
- [ ] "Type something:" 메시지 나타남
- [ ] 입력한 문자가 에코됨
- [ ] Enter 입력시 프롬프트 표시됨

### 속도 측정
- 전송 시간 (100바이트): ___ ms
- 예상값: ~70ms (115200 baud)
- 오차: ___

### 특수 문자 테스트
- [ ] NULL (0x00) 전송/수신 가능
- [ ] 0xFF 전송/수신 가능
- [ ] 역슬래시 처리 가능

### 결론
- [ ] 성공
- [ ] 부분 성공 (이유: ___)
- [ ] 실패 (이유: ___)
```

---

## 🚀 다음 단계

### 1. I2C Display 테스트
```bash
cmake --build build/a31g123 --target a31g123_i2c_display
```

### 2. 자체 프로토콜 개발
```cpp
// 예: 온도 센서 + UART 전송
// 1. I2C에서 온도 읽기
// 2. UART로 전송
// 3. PC에서 모니터링
```

### 3. 로깅 시스템 구축
```cpp
// UART를 통한 디버그 로깅
void log(const char* format, ...) {
    // snprintf로 포맷팅
    // uart.write()로 전송
}
```

---

**Happy Serial Development! 🚀**
