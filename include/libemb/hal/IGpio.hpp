#pragma once

namespace libemb::hal {

/**
 * @brief GPIO 핀 모드 열거형
 *
 * MCU GPIO 핀의 동작 모드를 정의합니다.
 */
enum class GpioMode {
    INPUT,          ///< 입력 모드 (플로팅)
    OUTPUT,         ///< 출력 모드
    INPUT_PULLUP,   ///< 내부 풀업 저항을 가진 입력 모드
    INPUT_PULLDOWN, ///< 내부 풀다운 저항을 가진 입력 모드
    ANALOG,         ///< 아날로그 모드 (ADC 입력 등)
};

/**
 * @brief GPIO 핀 상태 열거형
 *
 * GPIO 핀의 디지털 출력/입력 상태를 나타냅니다.
 */
enum class GpioState {
    LOW = 0,  ///< 낮은 논리 상태 (0V)
    HIGH = 1, ///< 높은 논리 상태 (Vcc)
};

/**
 * @brief 하드웨어 독립적인 핀 제어를 위한 추상 GPIO 인터페이스
 *
 * 이 인터페이스는 MCU 특정 GPIO 구현이 충족해야 하는 계약을 정의합니다.
 * 드라이버는 이 인터페이스에만 의존하며 구체적인 MCU 구현에는 의존하지 않습니다.
 */
class IGpio {
public:
    virtual ~IGpio() = default;

    /**
     * @brief GPIO 핀 모드를 구성합니다
     * @param mode 원하는 GPIO 모드
     */
    virtual void setMode(GpioMode mode) = 0;

    /**
     * @brief GPIO 핀에 값을 씁니다
     * @param state 쓸 상태 (HIGH 또는 LOW)
     */
    virtual void write(GpioState state) = 0;

    /**
     * @brief GPIO 핀의 현재 상태를 읽습니다
     * @return 현재 GPIO 상태
     */
    virtual GpioState read() const = 0;

    /**
     * @brief GPIO 핀 상태를 토글합니다
     */
    virtual void toggle() = 0;
};

} // namespace libemb::hal
