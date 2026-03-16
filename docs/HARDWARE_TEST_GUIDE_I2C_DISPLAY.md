# I2C Display 예제: SSD1306 OLED 실제 하드웨어 테스트

ABOV A31G123에서 I2C를 통해 SSD1306 OLED 디스플레이를 제어하는 완전 가이드.

## 📋 사전 준비

### 하드웨어 체크리스트

```
□ ABOV A31G123 개발 보드
□ SSD1306 OLED 디스플레이 (128x64, I2C 버전)
□ ST-Link v2 디버거
□ I2C 풀-업 저항 2개 (10kΩ)
□ 멀티 점프 와이어
□ 브레드보드
□ 멀티미터 (전압 확인용)
□ 오실로스코프 (선택, I2C 신호 분석용)
```

### SSD1306 디스플레이 사양

```
디스플레이:
  - 해상도: 128 x 64 픽셀
  - 색상: 흰색/검은색
  - 인터페이스: I2C (선택 가능)
  - 주소: 0x3C (기본값, 핀 설정에 따라 0x3D 가능)
  - 전압: 3.3V 또는 5V (호환)
  - 전류: ~10mA 정상, 최대 100mA

전형적인 핀 배치:
  1. GND ← 접지
  2. VCC ← 3.3V/5V
  3. SCL ← I2C 클록 (PA1 on ABOV)
  4. SDA ← I2C 데이터 (PA0 on ABOV)
```

---

## 🔧 Step 1: 하드웨어 연결

### 1.1 I2C 핀 맵

ABOV A31G123 I2C 포트:

```
I2C0 (권장)
  SDA: PA0 (데이터, 양방향)
  SCL: PA1 (클록, 양방향)
  Standard: 100 kHz
  Fast: 400 kHz

I2C1
  SDA: PA10
  SCL: PA11

I2C2
  SDA: PC0
  SCL: PC1
```

### 1.2 I2C 연결도

```
ABOV A31G123              SSD1306 OLED
───────────────────────────────────────

VCC (3.3V) ──────────────→ VCC
GND ───────────────────→ GND
PA0 (SDA) ──┬──────────→ SDA
            │
        [10kΩ Pull-up]
            │
           VCC (3.3V)

PA1 (SCL) ──┬──────────→ SCL
            │
        [10kΩ Pull-up]
            │
           VCC (3.3V)

상세 회로도:
                   ┌─────────────────┐
     PA0 (SDA) ────┤ SDA    SSD1306 │
                   │        OLED    │
     PA1 (SCL) ────┤ SCL     128x64│
                   │                │
   VCC (3.3V) ─────┤ VCC           │
                   │                │
    GND ───────────┤ GND           │
                   └─────────────────┘

풀-업 저항 연결:
    PA0 ──┬── [10kΩ] ── VCC
          └──────────────→ SSD1306 SDA

    PA1 ──┬── [10kΩ] ── VCC
          └──────────────→ SSD1306 SCL

(풀-업 저항이 없으면 I2C 통신 실패 가능)
```

### 1.3 전압 확인

```bash
# 멀티미터로 측정

# 1. 전원 확인
VCC: 3.3V ± 0.1V ✓
GND: 0V ✓

# 2. I2C 신호 (디스플레이 연결 후)
# 신호 없을 때:
PA0 (SDA): 3.3V (풀-업 저항에 의해 HIGH)
PA1 (SCL): 3.3V (풀-업 저항에 의해 HIGH)

# 통신 중:
PA0/PA1: 3.3V ↔ 0V 토글 (I2C 신호)
```

---

## 🏗️ Step 2: 소스 코드 확인

### 2.1 I2C Display 예제 코드 구조

```
i2c_display.cpp
├── main()
├── initialize_display()
│   └── Send SSD1306 initialization commands
├── clear_display()
│   └── Fill display with 0xFF (clear)
├── draw_pattern()
│   └── Send pixel data to OLED
└── Infinite loop with animations
```

### 2.2 핵심 코드

```cpp
// 1. I2C 초기화
libemb::port::abov_a31g123::A31G123I2c i2c(0, 100000);
// I2C0, 100kHz standard mode

// 2. 디스플레이 초기화
// SSD1306 초기화 명령어 시퀀스:
uint8_t init_cmds[] = {
    0xAE,           // Display OFF
    0xD5, 0x80,     // Set display clock
    0xA8, 0x3F,     // Set multiplex ratio (64)
    0xD3, 0x00,     // Set display offset
    0x40,           // Set start line
    0x8D, 0x14,     // Enable charge pump
    0x20, 0x00,     // Memory addressing mode
    0xA1,           // Set segment remap
    0xC8,           // Set COM output direction
    0xDA, 0x12,     // Set COM pins
    0x81, 0xCF,     // Set contrast
    0xD9, 0xF1,     // Set precharge
    0xDB, 0x40,     // Set VCOMH
    0x2E,           // Exit scroll
    0xAF            // Display ON
};

// 전송
i2c.write(0x3C, init_cmds, sizeof(init_cmds));

// 3. 픽셀 데이터 전송
// SSD1306는 8픽셀의 수직 바이트로 구성
// 128 × 64 픽셀 = 128 × 8 = 1024 바이트

uint8_t frame_buffer[128 * 8];  // 1KB 버퍼

// Frame buffer 채우기
for (int i = 0; i < sizeof(frame_buffer); ++i) {
    frame_buffer[i] = 0xFF;  // 모든 픽셀 ON
}

// 디스플레이로 전송
i2c.write(0x3C, frame_buffer, sizeof(frame_buffer));
```

---

## 🛠️ Step 3: 빌드 및 플래시

### 3.1 빌드

```bash
cd /path/to/libemb

# I2C Display 예제 빌드
cmake -B build/a31g123 \
  -DCMAKE_TOOLCHAIN_FILE=cmake/toolchains/abov-a31g123.cmake \
  -DLIBEMB_BUILD_EXAMPLES=ON \
  -DCMAKE_BUILD_TYPE=Release

cmake --build build/a31g123 --target a31g123_i2c_display
```

### 3.2 플래시

```bash
# OpenOCD 시작 (백그라운드)
openocd -f openocd_a31g123.cfg &
sleep 1

# GDB로 플래시
arm-none-eabi-gdb \
  -ex "file build/a31g123/examples/05_abov_a31g123_demo/a31g123_i2c_display.elf" \
  -ex "target extended-remote localhost:3333" \
  -ex "monitor reset halt" \
  -ex "load" \
  -ex "monitor reset run" \
  -ex "quit"
```

### 3.3 바이너리 크기 확인

```bash
arm-none-eabi-size build/a31g123/examples/05_abov_a31g123_demo/a31g123_i2c_display.elf

   text    data     bss     dec     hex filename
   6348      16     280    6644   19f4 a31g123_i2c_display.elf

메모리 사용:
  코드: 6.3 KB (Flash)
  RAM: 280 B (수동 버퍼)
  사용률: 4.9% Flash, 0.9% RAM ✓
```

---

## ✅ Step 4: I2C 신호 검증

### 4.1 오실로스코프로 I2C 신호 확인

```
채널 1: PA0 (SDA)
채널 2: PA1 (SCL)
시간 축: 100 µs/div
시간 확대: 2ms/div

예상 신호:
1. SCL 주기: 10 µs (100 kHz 클록)
2. SDA 신호: SCL이 LOW일 때만 변화
3. START/STOP 조건: SCL이 HIGH일 때 SDA 변화

I2C 신호 형식:
┌─────────────────────────────────┐
│ START │ ADDRESS │ DATA │ STOP  │
└─────────────────────────────────┘

SCL ┐      ┌───┐   ┌───┐
    └──┬───┘   └───┘   └───┬───┘
       │                   │
SDA ┐  └─┬───┬─┬─┬─┐   ┌───┘
    └────┘ └─┘ └─┘ └───┘

START: SCL=HIGH, SDA=HIGH→LOW
STOP:  SCL=HIGH, SDA=LOW→HIGH
```

### 4.2 I2C 버스 스캔

디스플레이의 주소를 확인하려면 I2C 스캔 코드 사용:

```cpp
// I2C 디바이스 스캔
void i2c_scan(A31G123I2c& i2c) {
    for (uint8_t addr = 0x08; addr < 0x78; ++addr) {
        I2cStatus status = i2c.write(addr, nullptr, 0);
        if (status == I2cStatus::OK) {
            uart.putChar('[');
            uart.write((uint8_t*)&addr, 1);
            uart.write((uint8_t*)"]\r\n", 3);
        }
    }
}

// 실행
A31G123Uart uart(0, 115200);
A31G123I2c i2c(0, 100000);
i2c_scan(i2c);

// 출력 예:
// [0x3C]  ← SSD1306은 기본 주소 0x3C
```

---

## 📺 Step 5: 디스플레이 테스트

### 5.1 동작 확인

**기본 테스트 순서:**

1. **전원 켜기**
   - SSD1306 디스플레이에 백라이트가 켜짐
   - 한 두 초 후 초기화 완료

2. **첫 프레임 표시**
   - 모든 픽셀이 ON (흰색 화면)
   - 또는 진행 중인 애니메이션 표시

3. **메모리 쓰기 테스트**
   - 프레임 버퍼를 반복적으로 쓰기
   - 화면이 깜박이는지 확인 (매 프레임마다)

### 5.2 예상 애니메이션

i2c_display.cpp에서:

```cpp
while (true) {
    // 1. 전체 화면 켜기
    for (int i = 0; i < 1024; ++i) {
        frame_buffer[i] = 0xFF;  // 모든 픽셀
    }
    send_frame_to_display();

    // 2. 전체 화면 끄기
    for (int i = 0; i < 1024; ++i) {
        frame_buffer[i] = 0x00;  // 픽셀 없음
    }
    send_frame_to_display();

    // 3. 체스보드 패턴
    for (int i = 0; i < 1024; ++i) {
        frame_buffer[i] = (i % 2) ? 0xAA : 0x55;
    }
    send_frame_to_display();
}
```

**화면 변화:**
```
[1초] 흰색 ↔ [1초] 검은색 ↔ [1초] 패턴 ↔ ...
```

---

## 🔍 Step 6: 고급 테스트

### 6.1 콘트래스트 조정

SSD1306 초기화 중:

```cpp
uint8_t contrast_cmd[] = {
    0x81,     // Contrast command
    0x7F      // 0x00 (어두움) ~ 0xFF (밝음)
};

i2c.write(0x3C, contrast_cmd, sizeof(contrast_cmd));
```

다양한 콘트래스트로 테스트:
```
0x00 - 매우 어두움 (거의 보이지 않음)
0x3F - 약간 어두움
0x7F - 중간 (권장)
0xBF - 밝음
0xFF - 매우 밝음 (눈 피로)
```

### 6.2 인버스 디스플레이

```cpp
uint8_t invert_cmd[] = {
    0xA7  // Inverse ON (검은색→흰색, 흰색→검은색)
    // 또는
    // 0xA6  // Inverse OFF (정상)
};
```

### 6.3 성능 측정

프레임 업데이트 속도:

```cpp
// 타이머 없이 루프 속도 측정
// I2C @100kHz: 1024바이트 전송 시간
// 공식: (1024바이트 × 10bits/byte) / 100,000 = ~102ms

// 각 프레임: ~102ms
// 프레임 레이트: ~10 FPS (1000ms / 102ms ≈ 10)

// I2C @400kHz로 시도하면:
// 각 프레임: ~25ms
// 프레임 레이트: ~40 FPS

// 코드에서 I2C 속도 변경:
A31G123I2c i2c(0, 400000);  // 400kHz 모드
```

---

## 📊 GDB에서 I2C 디버깅

### I2C 레지스터 상태 확인

```bash
arm-none-eabi-gdb build/.../a31g123_i2c_display.elf

(gdb) target extended-remote localhost:3333

# I2C0 기본 주소: 0x40020000

# I2C 컨트롤 레지스터
(gdb) print *(uint32_t*)0x40020000
$1 = 0x1234      # I2C 설정

# I2C 상태 레지스터
(gdb) print *(uint32_t*)0x40020004
$2 = 0x0001      # TXE = 1 (전송 버퍼 비어있음)

# I2C 데이터 레지스터
(gdb) print *(uint32_t*)0x40020008
$3 = 0x00        # 마지막 전송 바이트

# 프레임 버퍼 메모리 확인
(gdb) print &frame_buffer[0]@16
```

### 트래픽 모니터링

논리 분석기로 I2C 버스 캡처:

```
I2C Protocol Trace:
  [00:00:00.000] START
  [00:00:00.005] ADDR=0x3C (W)  ACK
  [00:00:00.010] DATA=0xAE      ACK  (Display OFF)
  [00:00:00.015] DATA=0xD5      ACK
  [00:00:00.020] DATA=0x80      ACK  (Clock setting)
  ...
  [00:00:00.500] STOP

전체 초기화: ~500 messages, ~100ms
프레임 전송: 1024 + metadata, ~102ms @ 100kHz
```

---

## ⚠️ 문제 해결

### 문제 1: 디스플레이가 켜지지 않음

**진단:**

```bash
# 1. 전원 확인
# 멀티미터로 VCC 측정: 3.3V ✓

# 2. I2C 통신 확인
# 오실로스코프: SCL/SDA 신호 있는지 확인
# 신호 없음 → I2C 초기화 실패

# 3. 펌웨어 플래시 확인
openocd 로그 확인: "verified successfully"

# 4. 코드 검증
arm-none-eabi-objdump -d build/.../a31g123_i2c_display.elf | grep -A 10 main
```

**해결:**

```bash
# 재플래시
arm-none-eabi-gdb -ex "... (위의 플래시 절차)"

# 풀-업 저항 추가 (없으면)
# PA0(SDA) - [10kΩ] - VCC
# PA1(SCL) - [10kΩ] - VCC

# I2C 주소 확인
# 기본: 0x3C
# 대체: 0x3D (핀 설정에 따라)
```

### 문제 2: I2C 통신 타임아웃

**원인:**

```
1. SDA/SCL 연결 안 됨
2. 풀-업 저항 부족
3. I2C 주소 잘못 설정
4. I2C 속도 너무 빠름
```

**해결:**

```cpp
// I2C 속도 낮춤 (안정성)
A31G123I2c i2c(0, 50000);  // 50kHz (매우 느림, 안정적)

// I2C 상태 로깅
I2cStatus status = i2c.write(0x3C, init_cmds, sizeof(init_cmds));
if (status != I2cStatus::OK) {
    // I2C 실패 처리
    // 재초기화 또는 에러 메시지
}

// 주소 스캔
for (uint8_t addr = 0x3C; addr <= 0x3D; ++addr) {
    status = i2c.write(addr, nullptr, 0);
    if (status == I2cStatus::OK) {
        // 올바른 주소 찾음
        break;
    }
}
```

### 문제 3: 화면이 깜박이거나 흐릿함

**원인:**

```
1. 콘트래스트 설정 오류
2. I2C 데이터 오류 (일부 바이트 손실)
3. 타이밍 이슈
```

**해결:**

```cpp
// 콘트래스트 증가
uint8_t contrast[] = {0x81, 0xBF};  // 더 밝게
i2c.write(0x3C, contrast, 2);

// 각 바이트마다 확인 (느리지만 안정적)
for (int i = 0; i < 1024; ++i) {
    i2c.write(0x3C, &frame_buffer[i], 1);
    __asm("nop");  // 작은 지연
}

// 또는 청크 단위로 전송
for (int chunk = 0; chunk < 8; ++chunk) {
    i2c.write(0x3C, &frame_buffer[chunk * 128], 128);
}
```

---

## 📝 테스트 보고서

**I2C Display 테스트 보고서 템플릿:**

```markdown
# I2C Display (SSD1306) 테스트 보고서

## 환경
- 날짜: ___
- 보드: ABOV A31G123
- 디스플레이: SSD1306 128x64 OLED
- I2C 속도: 100 kHz / 400 kHz

## 하드웨어 확인
- [ ] VCC = 3.3V ± 0.1V
- [ ] GND = 0V
- [ ] PA0(SDA) = 3.3V (풀-업 저항 있음)
- [ ] PA1(SCL) = 3.3V (풀-업 저항 있음)

## I2C 통신
- [ ] SCL 신호 정상 (오실로스코프 확인)
- [ ] SDA 신호 정상
- [ ] I2C 스캔 결과: 주소 0x3C 인식됨

## 디스플레이 동작
- [ ] 초기화 후 화면 ON
- [ ] 애니메이션 실행 (깜박임)
- [ ] 콘트래스트 적절

## 성능
- 프레임 레이트: ___ FPS
- I2C 통신 시간: ___ ms/frame
- 에러율: ___ (100개 프레임 중)

## 결론
- [ ] 완전 성공
- [ ] 부분 성공 (이슈: ___)
- [ ] 실패 (원인: ___)

## 후속 조치
- [ ] 없음
- [ ] I2C 속도 조정 필요
- [ ] 풀-업 저항 추가 필요
- [ ] 디스플레이 교체 필요
```

---

## 🚀 다음 단계

### 1. 커스텀 그래픽 그리기

```cpp
// 간단한 직선 그리기
void draw_line(int x0, int y0, int x1, int y1) {
    // Bresenham 알고리즘 구현
    // frame_buffer에 픽셀 설정
}

// 텍스트 표시
void draw_char(char c, int x, int y) {
    // 5x7 폰트 데이터 사용
    // frame_buffer에 쓰기
}
```

### 2. 센서 값 표시

```cpp
// 온도/습도 센서와 함께 사용
// 1. I2C에서 온도 읽기
// 2. 프레임 버퍼에 텍스트 작성
// 3. SSD1306으로 전송
```

### 3. GUI 라이브러리

libemb에 그래픽 라이브러리 추가:
- 텍스트 렌더링
- 메뉴 시스템
- 그래프 표시

---

## 📚 추가 리소스

- **SSD1306 데이터시트**: https://cdn-shop.adafruit.com/datasheets/SSD1306.pdf
- **I2C 프로토콜**: 표준 I2C 스펙
- **libemb I2C API**: `include/libemb/hal/II2c.hpp`

---

**Happy OLED Display Hacking! 🎨✨**
