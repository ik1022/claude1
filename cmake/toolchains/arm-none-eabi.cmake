# ARM Cortex-M cross-compilation toolchain for bare-metal development
# Usage: cmake -DCMAKE_TOOLCHAIN_FILE=cmake/toolchains/arm-none-eabi.cmake

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR ARM)

# Toolchain prefix
set(ARM_TOOLCHAIN_PREFIX "arm-none-eabi-")

# Set C and C++ compilers
set(CMAKE_C_COMPILER "${ARM_TOOLCHAIN_PREFIX}gcc" CACHE FILEPATH "C compiler")
set(CMAKE_CXX_COMPILER "${ARM_TOOLCHAIN_PREFIX}g++" CACHE FILEPATH "C++ compiler")
set(CMAKE_ASM_COMPILER "${ARM_TOOLCHAIN_PREFIX}gcc" CACHE FILEPATH "ASM compiler")

# Other tools
set(CMAKE_AR "${ARM_TOOLCHAIN_PREFIX}ar" CACHE FILEPATH "Archiver")
set(CMAKE_OBJCOPY "${ARM_TOOLCHAIN_PREFIX}objcopy" CACHE FILEPATH "Object copy utility")
set(CMAKE_OBJDUMP "${ARM_TOOLCHAIN_PREFIX}objdump" CACHE FILEPATH "Object dump utility")
set(CMAKE_SIZE "${ARM_TOOLCHAIN_PREFIX}size" CACHE FILEPATH "Size utility")

# Prevent CMake from testing compiler
set(CMAKE_TRY_COMPILE_TARGET_TYPE "STATIC_LIBRARY")

# Common ARM flags (customize per MCU)
set(ARM_COMPILE_FLAGS "-mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard")
set(ARM_COMPILE_FLAGS "${ARM_COMPILE_FLAGS} -fno-exceptions -fno-rtti -fno-unwind-tables")
set(ARM_COMPILE_FLAGS "${ARM_COMPILE_FLAGS} -ffunction-sections -fdata-sections")

set(CMAKE_C_FLAGS_INIT "${ARM_COMPILE_FLAGS} -nostdinc")
set(CMAKE_CXX_FLAGS_INIT "${ARM_COMPILE_FLAGS} -nostdinc -nostdinc++")
set(CMAKE_ASM_FLAGS_INIT "${ARM_COMPILE_FLAGS}")

# Linker flags
set(CMAKE_EXE_LINKER_FLAGS_INIT "-Wl,--gc-sections -Wl,--print-memory-usage")

# Embedded system settings
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

message(STATUS "Using ARM Cortex-M toolchain: ${ARM_TOOLCHAIN_PREFIX}gcc")

# Add bare-metal system headers as first include path (before GCC's internal headers)
set(BARE_METAL_HEADERS "${CMAKE_CURRENT_LIST_DIR}/../../include/libemb/bare-metal")
set(CMAKE_C_FLAGS_INIT "${CMAKE_C_FLAGS_INIT} -isystem ${BARE_METAL_HEADERS}")
set(CMAKE_CXX_FLAGS_INIT "${CMAKE_CXX_FLAGS_INIT} -isystem ${BARE_METAL_HEADERS}")
