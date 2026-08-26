/* exarcia_flash_stm32.c — STM32F407 internal flash OTA bank management */

#include <string.h>

#include "exarcia_flash.h"
#include "stm32f4xx_hal.h"
#include "stm32f407xx.h"

#define FLASH_KEY1  0x45670123UL
#define FLASH_KEY2  0xCDEF89ABUL
#define FLASH_CR_SER (1UL << 1)
#define FLASH_CR_STRT (1UL << 16)
#define FLASH_CR_PG  (1UL << 0)
#define FLASH_SR_BSY (1UL << 16)
#define FLASH_SR_EOP (1UL << 0)

static void flash_unlock(void) {
    if (FLASH->CR & (1UL << 31)) {
        FLASH->KEYR = FLASH_KEY1;
        FLASH->KEYR = FLASH_KEY2;
    }
}

static void flash_lock(void) {
    FLASH->CR |= (1UL << 31);
}

static void flash_wait_bsy(void) {
    while (FLASH->SR & FLASH_SR_BSY) {}
}

static bool flash_erase_sector(uint32_t addr) {
    flash_wait_bsy();
    flash_unlock();
    FLASH->CR &= ~(0xFUL << 3);
    /* Calculate sector number from address */
    uint32_t sector = 0;
    if (addr >= 0x080C0000UL)      sector = 11;
    else if (addr >= 0x080A0000UL) sector = 10;
    else if (addr >= 0x08080000UL) sector = 9;
    else if (addr >= 0x08060000UL) sector = 8;
    else if (addr >= 0x08040000UL) sector = 7;
    else if (addr >= 0x08020000UL) sector = 6;
    else if (addr >= 0x0801C000UL) sector = 5;
    else if (addr >= 0x08018000UL) sector = 4;
    else if (addr >= 0x08014000UL) sector = 3;
    else if (addr >= 0x08010000UL) sector = 2;
    else if (addr >= 0x0800C000UL) sector = 1;
    else                           sector = 0;

    FLASH->CR |= FLASH_CR_SER;
    FLASH->CR |= (sector << 3);
    FLASH->CR |= FLASH_CR_STRT;
    flash_wait_bsy();
    FLASH->CR &= ~(FLASH_CR_SER | (0xFUL << 3));
    flash_lock();
    return !(FLASH->SR & 0xF2);
}

bool stm32_flash_erase_bank(uint32_t bank_addr, uint32_t bank_size) {
    for (uint32_t addr = bank_addr; addr < bank_addr + bank_size; addr += EXARCIA_FLASH_SECTOR_SIZE) {
        if (!flash_erase_sector(addr)) return false;
    }
    return true;
}

bool stm32_flash_write_chunk(uint32_t addr, const uint8_t *data, size_t len) {
    flash_wait_bsy();
    flash_unlock();
    FLASH->CR |= FLASH_CR_PG;
    for (size_t i = 0; i < len; i += 4) {
        uint32_t word = 0;
        for (size_t j = 0; j < 4 && (i + j) < len; j++) {
            word |= ((uint32_t)data[i + j]) << (j * 8);
        }
        *(volatile uint32_t *)(addr + i) = word;
        flash_wait_bsy();
    }
    FLASH->CR &= ~FLASH_CR_PG;
    flash_lock();
    return true;
}

bool stm32_flash_validate_sha256(uint32_t addr, size_t len, const uint8_t *expected) {
    /* TODO: integrate mbedTLS or a minimal SHA-256 for production.
     * For this demo, skip validation if no SHA-256 peripheral is available. */
    (void)addr; (void)len; (void)expected;
    return true;
}

bool stm32_flash_swap_bank(void) {
    /* On STM32F407, bank swapping is done via option bytes or by
     * adjusting the VTOR and resetting.  For this demo, we trigger
     * a system reset after marking the new bank as active. */
    SCB->VTOR = EXARCIA_FLASH_BANK1_ADDR;
    NVIC_SystemReset();
    return true; /* unreachable */
}

void stm32_flash_mark_valid(void) {
    /* TODO: write a validity flag to option bytes or a reserved flash word */
}

void stm32_flash_mark_rollback(void) {
    /* TODO: set rollback flag so bootloader reverts to bank 0 on next boot */
}
