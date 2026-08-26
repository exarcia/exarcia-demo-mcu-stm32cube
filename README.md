# exarcia-demo-mcu-stm32cube

Minimal STM32 OTA demo firmware for [Exarcia](https://github.com/exarcia).
Built with arm-none-eabi-gcc + CMake; designed for the STM32F407VG Discovery
board or any STM32F407 board with USART2 on PA2/PA3.

## Requirements

- `arm-none-eabi-gcc` (Debian/Ubuntu: `apt install gcc-arm-none-eabi`)
- `cmake` ≥ 3.20

## Quick start

```bash
# Build (produces build/exarcia-demo-mcu-stm32cube.bin)
make build

# Check firmware size
make size

# Flash via ST-Link (requires st-flash or STM32CubeProgrammer)
st-flash write build/exarcia-demo-mcu-stm32cube.bin 0x08000000

# Or flash via OpenOCD
openocd -f interface/stlink.cfg -f target/stm32f4x.cfg \
  -c "program build/exarcia-demo-mcu-stm32cube.elf verify reset exit"
```

## Local configuration

Copy `.env.example` to `.env.local` and fill in device credentials:

```bash
cp .env.example .env.local
```

Run `make config` to generate the build header and validate values:

```bash
make config
```

**Never commit `.env.local` or generated headers.**

## Architecture

```
Core/Src/               CubeMX-generated application code
Core/Startup/           Cortex-M4 vector table + Reset_Handler
Drivers/                Minimal STM32 HAL + CMSIS (vendored, compilable)
app/inc/                Exarcia client API and platform abstractions
app/src/                Client protocol, UART transport, flash driver, main
Project.ioc             STM32CubeMX project (detects as stm32cube)
CMakeLists.txt          Cross-compilation build definition
cmake/                  arm-none-eabi-gcc toolchain file
STM32F407VGTx_FLASH.ld Linker script (1 MB flash, 192 KB SRAM)
```

## OTA flow

```
STM32                       Exarcia Core
  |  register (identity, PSK)  |
  |--------------------------->|
  |  heartbeat (version)       |
  |--------------------------->|
  |  poll /deployments/current |
  |--------------------------->|
  |<-- { download_url, sha256 }|
  |  GET firmware binary       |
  |----[artifact storage]----->|
  |  validate SHA-256          |
  |  swap flash bank, reboot   |
  |  confirm deployment        |
  |--------------------------->|
```

## Network transport

The default transport bridges UART2 to the host via `exarcia serve-uart`
(planned CLI command).  For production, replace `exarcia_network_stm32.c`
with lwIP, a cellular modem driver, or another TCP/IP stack.

## Flash bank strategy

STM32F407 has 1 MB flash split into two 512 KB banks.  The OTA client
downloads new firmware to bank 1, validates SHA-256, then swaps the
active bank via VTOR and resets.  A validity flag prevents rollback
until the new image confirms with Core.
