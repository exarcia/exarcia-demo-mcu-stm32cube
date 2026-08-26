#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* STM32F407 internal flash geometry — two 512 KB banks for OTA */
#define EXARCIA_FLASH_BANK0_ADDR  0x08000000UL
#define EXARCIA_FLASH_BANK0_SIZE  (512UL * 1024UL)
#define EXARCIA_FLASH_BANK1_ADDR  (EXARCIA_FLASH_BANK0_ADDR + EXARCIA_FLASH_BANK0_SIZE)
#define EXARCIA_FLASH_BANK1_SIZE  EXARCIA_FLASH_BANK0_SIZE
#define EXARCIA_FLASH_SECTOR_SIZE (128UL * 1024UL)

bool stm32_flash_erase_bank(uint32_t bank_addr, uint32_t bank_size);
bool stm32_flash_write_chunk(uint32_t addr, const uint8_t *data, size_t len);
bool stm32_flash_validate_sha256(uint32_t addr, size_t len, const uint8_t *expected);
bool stm32_flash_swap_bank(void);
void stm32_flash_mark_valid(void);
void stm32_flash_mark_rollback(void);
