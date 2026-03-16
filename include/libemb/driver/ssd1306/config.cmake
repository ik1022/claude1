# Ssd1306 Driver Configuration
# Declares dependencies and optional features for the Ssd1306 OLED display driver

# Required dependencies
set(LIBEMB_DRIVER_SSD1306_REQUIRES
    HAL::I2C           # I2C hardware interface for display communication
    UTIL::StaticVector # Fixed-size buffer for frame data
)

# Optional features
set(LIBEMB_DRIVER_SSD1306_OPTIONAL
    # Currently none
)

# Mock dependencies for testing
set(LIBEMB_DRIVER_SSD1306_TEST_MOCKS
    mock::I2c          # Mock I2C for unit tests
)

# Driver capabilities
set(LIBEMB_DRIVER_SSD1306_FEATURES
    "I2C_DISPLAY"      # I2C-based display driver
    "FRAMEBUFFER"      # Supports frame buffer operations
    "1BIT_MONOCHROME"  # 1-bit monochrome display
    "128x64"           # 128x64 pixel display
)
