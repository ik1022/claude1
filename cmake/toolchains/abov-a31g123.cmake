# ABOV A31G123 ARM Cortex-M0+ CMake Toolchain
# Cross-compilation configuration for ABOV Semiconductor A31G123 MCU
# 32-bit ARM Cortex-M0+, 70 MHz, 32KB RAM, 128KB Flash

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR ARM)

# Specify the cross-compiler
set(CMAKE_C_COMPILER arm-none-eabi-gcc CACHE FILEPATH "C compiler")
set(CMAKE_CXX_COMPILER arm-none-eabi-g++ CACHE FILEPATH "C++ compiler")
set(CMAKE_ASM_COMPILER arm-none-eabi-gcc CACHE FILEPATH "ASM compiler")
set(CMAKE_AR arm-none-eabi-ar CACHE FILEPATH "Archive tool")
set(CMAKE_RANLIB arm-none-eabi-ranlib CACHE FILEPATH "Archive indexer")
set(CMAKE_OBJCOPY arm-none-eabi-objcopy CACHE FILEPATH "Object copy")
set(CMAKE_OBJDUMP arm-none-eabi-objdump CACHE FILEPATH "Object dump")
set(CMAKE_SIZE arm-none-eabi-size CACHE FILEPATH "Size tool")

# Prevent CMake from testing the compiler with a linking step
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# ============================================================================
# MCU-Specific Compiler Flags
# ============================================================================

# Cortex-M0+ processor flags
set(MCU_FLAGS "-mcpu=cortex-m0plus -mthumb -mno-unaligned-access")

# C/C++ language flags
set(LANGUAGE_FLAGS "-fno-exceptions -fno-rtti -fno-unwind-tables")

# Common optimization flags
set(OPTIMIZATION_FLAGS "-ffunction-sections -fdata-sections -fno-builtin")

# Warning flags
set(WARNING_FLAGS "-Wall -Wextra -Wpedantic -Wno-unused-parameter")

# Debug/Release specific
set(CMAKE_C_FLAGS_DEBUG "-g3 -O0 ${MCU_FLAGS} ${LANGUAGE_FLAGS} ${OPTIMIZATION_FLAGS} ${WARNING_FLAGS}")
set(CMAKE_CXX_FLAGS_DEBUG "-g3 -O0 ${MCU_FLAGS} ${LANGUAGE_FLAGS} ${OPTIMIZATION_FLAGS} ${WARNING_FLAGS}")

set(CMAKE_C_FLAGS_RELEASE "-Os -DNDEBUG ${MCU_FLAGS} ${LANGUAGE_FLAGS} ${OPTIMIZATION_FLAGS} -flto ${WARNING_FLAGS}")
set(CMAKE_CXX_FLAGS_RELEASE "-Os -DNDEBUG ${MCU_FLAGS} ${LANGUAGE_FLAGS} ${OPTIMIZATION_FLAGS} -flto ${WARNING_FLAGS}")

# ASM flags
set(CMAKE_ASM_FLAGS "-x assembler-with-cpp ${MCU_FLAGS}")

# ============================================================================
# Linker Flags
# ============================================================================

# Common linker flags
set(LINKER_COMMON_FLAGS "-Wl,--gc-sections -Wl,--print-memory-usage -Wl,-Map=firmware.map")

set(CMAKE_EXE_LINKER_FLAGS_INIT "${LINKER_COMMON_FLAGS}")
set(CMAKE_SHARED_LINKER_FLAGS_INIT "${LINKER_COMMON_FLAGS}")
set(CMAKE_MODULE_LINKER_FLAGS_INIT "${LINKER_COMMON_FLAGS}")

# ============================================================================
# C/C++ Standard Settings
# ============================================================================

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ============================================================================
# Find Programs
# ============================================================================

find_program(ARM_SIZE_TOOL arm-none-eabi-size REQUIRED)
find_program(ARM_OBJCOPY_TOOL arm-none-eabi-objcopy REQUIRED)
find_program(ARM_OBJDUMP_TOOL arm-none-eabi-objdump REQUIRED)

# ============================================================================
# Platform-Specific Definitions
# ============================================================================

add_definitions(
    -DPLATFORM_ABOV_A31G123
    -DARM_CORTEX_M0PLUS
    -D__CORTEX_M0PLUS__=1
)

# ============================================================================
# Executable Targets: Create HEX/BIN
# ============================================================================

function(add_arm_executable TARGET)
    add_executable(${TARGET} ${ARGN})

    # Add custom command to generate .hex
    add_custom_command(TARGET ${TARGET} POST_BUILD
        COMMAND ${ARM_OBJCOPY_TOOL} -O ihex ${TARGET}.elf ${TARGET}.hex
        COMMENT "Generating HEX file: ${TARGET}.hex"
    )

    # Add custom command to generate .bin
    add_custom_command(TARGET ${TARGET} POST_BUILD
        COMMAND ${ARM_OBJCOPY_TOOL} -O binary ${TARGET}.elf ${TARGET}.bin
        COMMENT "Generating BIN file: ${TARGET}.bin"
    )

    # Add custom command for size information
    add_custom_command(TARGET ${TARGET} POST_BUILD
        COMMAND ${ARM_SIZE_TOOL} ${TARGET}.elf
        COMMENT "Size information for ${TARGET}"
    )
endfunction()

# ============================================================================
# Linker Script Configuration
# ============================================================================

# Linker script must be specified when linking
# Usage: target_link_options(target PRIVATE -T${LINKER_SCRIPT})

message(STATUS "ABOV A31G123 ARM Cortex-M0+ Toolchain Loaded")
message(STATUS "  Compiler: arm-none-eabi-gcc")
message(STATUS "  MCU: Cortex-M0+, 70 MHz, 32KB RAM, 128KB Flash")
message(STATUS "  C Standard: C11, C++ Standard: C++17")
