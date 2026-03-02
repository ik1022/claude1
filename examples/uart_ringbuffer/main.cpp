#include <cstdio>
#include <cstring>
#include "NonBlockingUart.hpp"
#include "libemb/hal/IUart.hpp"

/**
 * @brief 시연용 간단한 Mock UART
 */
class MockUart : public libemb::hal::IUart {
public:
    void putChar(uint8_t byte) override {
        if (tx_count_ < 256) {
            tx_buffer_[tx_count_++] = byte;
        }
    }

    uint8_t getChar() override {
        if (rx_pos_ < rx_count_) {
            return rx_buffer_[rx_pos_++];
        }
        return 0;
    }

    bool isDataAvailable() const override {
        return rx_pos_ < rx_count_;
    }

    void write(const uint8_t* data, size_t length) override {
        for (size_t i = 0; i < length && tx_count_ < 256; ++i) {
            tx_buffer_[tx_count_++] = data[i];
        }
    }

    size_t read(uint8_t* data, size_t maxLength) override {
        size_t count = 0;
        while (count < maxLength && rx_pos_ < rx_count_) {
            data[count++] = rx_buffer_[rx_pos_++];
        }
        return count;
    }

    // 테스트 헬퍼
    void queueRxData(const uint8_t* data, size_t length) {
        rx_count_ = 0;
        rx_pos_ = 0;
        for (size_t i = 0; i < length && i < 256; ++i) {
            rx_buffer_[i] = data[i];
        }
        rx_count_ = length;
    }

    const uint8_t* getTxBuffer() const { return tx_buffer_; }
    size_t getTxCount() const { return tx_count_; }
    void clearTxBuffer() { tx_count_ = 0; }

private:
    uint8_t tx_buffer_[256];
    uint8_t rx_buffer_[256];
    size_t tx_count_ = 0;
    size_t rx_count_ = 0;
    size_t rx_pos_ = 0;
};

/**
 * 비블로킹 UART 통신 예제
 *
 * 이 예제는 다음을 보여줍니다:
 * 1. 비블로킹 TX: 전송을 위해 데이터를 큐에 추가하고, 인터럽트가 전송합니다
 * 2. 비블로킹 RX: 인터럽트가 수신 데이터를 저장하고, 메인 루프가 처리합니다
 * 3. 순환 버퍼링: 고정 크기 메모리의 효율적인 사용
 *
 * 주요 이점:
 * - 메인 루프가 UART 대기로 블로킹되지 않음
 * - 패킷 손실 없이 버스트 데이터 처리
 * - 예측 가능한 메모리 사용 (동적 할당 없음)
 * - 실시간 시스템에 적합
 */

// Use the driver with Mock UART (for PC testing)
using UartDriver = libemb::example::NonBlockingUart<256, 128>;

void demo_basic_communication() {
    printf("\n=== 데모 1: 기본 비블로킹 TX/RX ===\n");

    // 드라이버 및 mock UART 생성
    UartDriver uart;
    MockUart mock_uart;
    uart.init(&mock_uart);

    // UART에 데이터 전송 시뮬레이션
    printf("전송 중: 'Hello'\n");
    const uint8_t tx_data[] = {'H', 'e', 'l', 'l', 'o'};
    uart.write(tx_data, 5);

    printf("TX 버퍼 수준: %zu 바이트\n", uart.getTxBufferLevel());

    // 인터럽트 전송 데이터 시뮬레이션
    printf("TX 인터럽트 시뮬레이션 중 (버퍼된 데이터 전송)...\n");
    while (uart.onTxInterrupt()) {
        // 사용 가능한 데이터가 있는 동안 계속 전송
    }

    // 이제 RX 인터럽트를 통한 데이터 수신 시뮬레이션
    printf("\n수신 데이터 시뮬레이션 중: 'Hi!'\n");
    const uint8_t rx_data[] = {'H', 'i', '!'};
    for (uint8_t byte : rx_data) {
        uart.onRxInterrupt(byte);
    }

    printf("RX 버퍼 수준: %zu 바이트\n", uart.getRxBufferLevel());

    // 메인 루프가 사용 가능한 데이터를 읽음
    printf("RX 버퍼에서 읽는 중: ");
    uint8_t buffer[16];
    size_t read_count = uart.read(buffer, sizeof(buffer));
    for (size_t i = 0; i < read_count; ++i) {
        printf("%c", buffer[i]);
    }
    printf("\n");
}

void demo_high_throughput() {
    printf("\n=== 데모 2: 높은 처리량 (버스트 데이터) ===\n");

    UartDriver uart;
    MockUart mock_uart;
    uart.init(&mock_uart);

    // 큰 패킷 전송
    const char message[] = "This is a longer message that demonstrates buffering";
    printf("메시지 전송 중 (%zu 바이트)...\n", strlen(message));

    if (uart.write((const uint8_t*)message, strlen(message)) ==
        UartDriver::Status::OK) {
        printf("전송을 위해 모든 데이터가 큐에 추가되었습니다\n");
    } else {
        printf("TX 버퍼 가득 찼습니다!\n");
    }

    printf("TX 버퍼 수준: %zu 바이트\n", uart.getTxBufferLevel());

    // 인터럽트를 통해 데이터를 점진적으로 전송하는 시뮬레이션
    size_t transmitted = 0;
    printf("인터럽트를 통해 바이트 단위로 전송 중...\n");
    while (uart.onTxInterrupt()) {
        transmitted++;
        if (transmitted % 10 == 0) {
            printf("  %zu 바이트 전송됨\n", transmitted);
        }
    }
    printf("전송 완료 (%zu 바이트)\n", transmitted);
}

void demo_echo_server() {
    printf("\n=== 데모 3: 에코 서버 (TX/RX 분리) ===\n");

    UartDriver uart;
    MockUart mock_uart;
    uart.init(&mock_uart);

    // 명령 수신 시뮬레이션
    const char command[] = "LED_ON";
    printf("수신 명령: '%s'\n", command);
    for (char c : command) {
        uart.onRxInterrupt(static_cast<uint8_t>(c));
    }

    // 메인 루프가 명령을 처리하고 응답을 전송
    printf("명령 처리 중...\n");
    uint8_t rx_buf[32];
    size_t rx_count = uart.read(rx_buf, sizeof(rx_buf));

    // 에코 백: "ACK: LED_ON"
    const char response[] = "ACK: ";
    uart.write((const uint8_t*)response, strlen(response));
    uart.write(rx_buf, rx_count);
    uart.writeByte('\n');

    printf("응답이 큐에 추가됨 (%zu 바이트)\n", uart.getTxBufferLevel());

    // 인터럽트를 통해 응답 전송
    printf("응답 전송 중: ");
    size_t tx_count = 0;
    while (uart.onTxInterrupt()) {
        tx_count++;
    }
    printf("(전송됨 %zu 바이트)\n", tx_count);
}

void demo_buffer_management() {
    printf("\n=== 데모 4: 버퍼 수준 모니터링 ===\n");

    UartDriver uart;
    MockUart mock_uart;
    uart.init(&mock_uart);

    printf("초기 상태:\n");
    printf("  RX 버퍼: %zu/256 바이트\n", uart.getRxBufferLevel());
    printf("  TX 버퍼: %zu/128 바이트\n", uart.getTxBufferLevel());
    printf("  RX 비어있음: %s\n", !uart.isDataAvailable() ? "예" : "아니오");
    printf("  TX 가득 찬: %s\n", uart.isTxFull() ? "예" : "아니오");

    // TX 데이터 큐 추가
    uint8_t tx_data[100];
    for (int i = 0; i < 100; ++i) {
        tx_data[i] = (uint8_t)(i % 256);
    }
    uart.write(tx_data, 100);

    printf("\n100 바이트를 큐에 추가한 후:\n");
    printf("  TX 버퍼: %zu/128 바이트\n", uart.getTxBufferLevel());
    printf("  TX 가득 찬: %s\n", uart.isTxFull() ? "예" : "아니오");

    // RX 시뮬레이션
    for (int i = 0; i < 20; ++i) {
        uart.onRxInterrupt(static_cast<uint8_t>('A' + (i % 26)));
    }

    printf("\n20 바이트를 수신한 후:\n");
    printf("  RX 버퍼: %zu/256 바이트\n", uart.getRxBufferLevel());
    printf("  RX 비어있음: %s\n", !uart.isDataAvailable() ? "예" : "아니오");

    // 다음 바이트를 제거하지 않고 미리보기
    uint8_t next_byte;
    if (uart.peekByte(next_byte)) {
        printf("  다음 바이트 (미리보기): '%c'\n", next_byte);
    }

    // 모든 데이터 읽기
    uint8_t buffer[256];
    size_t read_count = uart.read(buffer, sizeof(buffer));
    printf("  %zu 바이트 읽음\n", read_count);
    printf("  읽은 후 RX 버퍼: %zu 바이트\n", uart.getRxBufferLevel());
}

void demo_error_handling() {
    printf("\n=== 데모 5: 에러 처리 (TX 버퍼 오버플로우) ===\n");

    // 오버플로우를 보여주기 위한 작은 TX 버퍼
    using SmallUart = libemb::example::NonBlockingUart<256, 16>;
    SmallUart uart;
    MockUart mock_uart;
    uart.init(&mock_uart);

    printf("TX 버퍼 크기: 16 바이트\n");

    // 버퍼가 보유할 수 있는 것보다 더 많은 데이터를 전송하려고 시도
    uint8_t large_msg[50];
    for (int i = 0; i < 50; ++i) {
        large_msg[i] = static_cast<uint8_t>('A' + (i % 26));
    }

    printf("50 바이트 전송 시도 중...\n");
    auto status = uart.write(large_msg, 50);

    if (status == SmallUart::Status::TX_FULL) {
        printf("TX 버퍼 오버플로우! 모든 데이터가 큐에 추가되지 않았습니다.\n");
        printf("TX 버퍼 수준: %zu/16 바이트\n", uart.getTxBufferLevel());

        // 맞는 것만 전송
        printf("\n처음 16 바이트만 전송 중...\n");
        status = uart.write(large_msg, 16);
        if (status == SmallUart::Status::OK) {
            printf("첫 번째 배치가 성공적으로 큐에 추가됨\n");
        }
    }
}

int main() {
    printf("╔════════════════════════════════════════════════════════════════╗\n");
    printf("║   비블로킹 UART with RingBuffer - libemb 예제        ║\n");
    printf("╚════════════════════════════════════════════════════════════════╝\n");

    demo_basic_communication();
    demo_high_throughput();
    demo_echo_server();
    demo_buffer_management();
    demo_error_handling();

    printf("\n╔════════════════════════════════════════════════════════════════╗\n");
    printf("║                    모든 예제 완료!                     ║\n");
    printf("╚════════════════════════════════════════════════════════════════╝\n\n");

    printf("주요 내용:\n");
    printf("비블로킹 설계: 메인 루프가 UART 대기로 블로킹되지 않음\n");
    printf("순환 버퍼: 버스트 데이터의 효율적인 처리\n");
    printf("인터럽트 안전: TX/RX가 메인 애플리케이션에서 분리됨\n");
    printf("동적 할당 없음: 예측 가능한 메모리 사용\n");
    printf("상태 확인: 버퍼 수준 모니터링 및 오버플로우 감지\n\n");

    return 0;
}
