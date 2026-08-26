cmake_minimum_required(VERSION 3.20)

# Load toolchain before project() — CMake needs the compiler at configure time.
set(CMAKE_TOOLCHAIN_FILE "${CMAKE_SOURCE_DIR}/cmake/arm-none-eabi-gcc.cmake")

project(exarcia-demo-mcu-stm32cube C ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

# --- sources --------------------------------------------------------------- #

set(HAL_DIR  "${CMAKE_SOURCE_DIR}/Drivers/STM32F4xx_HAL_Driver")
set(CMSIS_DIR "${CMAKE_SOURCE_DIR}/Drivers/CMSIS")
set(APP_DIR  "${CMAKE_SOURCE_DIR}/app")

set(HAL_SOURCES
    ${HAL_DIR}/Src/stm32f4xx_hal.c
    ${HAL_DIR}/Src/stm32f4xx_hal_uart.c
)

set(APP_SOURCES
    ${APP_DIR}/src/exarcia_client.c
    ${APP_DIR}/src/exarcia_network_stm32.c
    ${APP_DIR}/src/exarcia_flash_stm32.c
    ${APP_DIR}/src/main.c
)

set(CORE_SOURCES
    ${CMAKE_SOURCE_DIR}/Core/Src/system_stm32f4xx.c
    ${CMAKE_SOURCE_DIR}/Core/Startup/startup_stm32f407vgtx.s
)

add_executable(${PROJECT_NAME}.elf
    ${HAL_SOURCES}
    ${APP_SOURCES}
    ${CORE_SOURCES}
)

# --- includes -------------------------------------------------------------- #

target_include_directories(${PROJECT_NAME}.elf PRIVATE
    ${HAL_DIR}/Inc
    ${CMSIS_DIR}/Include
    ${CMSIS_DIR}/Device/ST/STM32F4xx/Include
    ${APP_DIR}/inc
)

# --- compile flags --------------------------------------------------------- #

target_compile_options(${PROJECT_NAME}.elf PRIVATE
    -Wall -Wextra -Werror -Wno-unused-parameter
    -ffunction-sections -fdata-sections
    -fno-common
)

target_link_options(${PROJECT_NAME}.elf PRIVATE
    -T${CMAKE_SOURCE_DIR}/STM32F407VGTx_FLASH.ld
    -Wl,--gc-sections
    -Wl,-Map=${PROJECT_NAME}.map
    -Wl,--print-memory-usage
)

# --- post-build: generate .bin and .hex ------------------------------------ #

add_custom_command(TARGET ${PROJECT_NAME}.elf POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:${PROJECT_NAME}.elf> ${PROJECT_NAME}.bin
    COMMAND ${CMAKE_OBJCOPY} -O ihex   $<TARGET_FILE:${PROJECT_NAME}.elf> ${PROJECT_NAME}.hex
    COMMAND ${CMAKE_SIZE}    $<TARGET_FILE:${PROJECT_NAME}.elf>
    COMMENT "Generating .bin and .hex, printing size"
)
