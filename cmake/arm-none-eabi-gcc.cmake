# CMake toolchain file for cross-compiling with arm-none-eabi-gcc
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)

find_program(CMAKE_C_COMPILER   NAMES arm-none-eabi-gcc  REQUIRED)
find_program(CMAKE_ASM_COMPILER NAMES arm-none-eabi-gcc  REQUIRED)
find_program(CMAKE_CXX_COMPILER NAMES arm-none-eabi-g++  REQUIRED)
find_program(CMAKE_OBJCOPY      NAMES arm-none-eabi-objcopy REQUIRED)
find_program(CMAKE_OBJDUMP      NAMES arm-none-eabi-objdump REQUIRED)
find_program(CMAKE_SIZE         NAMES arm-none-eabi-size    REQUIRED)

set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# MCU flags — Cortex-M4 with FPU (STM32F407)
set(MCU_FLAGS "-mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard")

set(CMAKE_C_FLAGS_INIT   "${MCU_FLAGS}")
set(CMAKE_CXX_FLAGS_INIT "${MCU_FLAGS}")
set(CMAKE_ASM_FLAGS_INIT "${MCU_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS_INIT "${MCU_FLAGS} -specs=nano.specs -specs=nosys.specs")

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
