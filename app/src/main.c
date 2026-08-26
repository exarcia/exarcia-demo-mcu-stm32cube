/* main.c — Exarcia STM32 OTA demo entry point.
 *
 * Hardware: STM32F407VG Discovery (or Nucleo-F401RE with LED on PA5).
 * Transport: UART2 (PA2/PA3) at 115200 baud, bridged to host via USB-UART.
 * LED:      PA5 (LD4 on Discovery board) blinks during normal operation,
 *           solid during OTA download, off after reset.
 */

#include <stdio.h>
#include <string.h>

#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_gpio.h"
#include "stm32f4xx_hal_uart.h"
#include "stm32f407xx.h"
#include "exarcia_client.h"
#include "exarcia_network.h"
#include "exarcia_flash.h"
#include "system_stm32f4xx.h"

/* ---- HAL handles ---- */
UART_HandleTypeDef huart2;
GPIO_InitTypeDef   hled_pin;

/* ---- Exarcia transport + flash bindings ---- */
static const exarcia_transport_t uart_transport = {
    .init           = exarcia_uart_transport_init,
    .http_get       = exarcia_uart_transport_get,
    .http_post      = exarcia_uart_transport_post,
    .https_get_bin  = exarcia_uart_transport_https_get_bin,
    .deinit         = exarcia_uart_transport_deinit,
};

static const exarcia_flash_t stm32_flash = {
    .erase_bank      = stm32_flash_erase_bank,
    .write_chunk     = stm32_flash_write_chunk,
    .validate_sha256 = stm32_flash_validate_sha256,
    .swap_bank       = stm32_flash_swap_bank,
    .mark_valid      = stm32_flash_mark_valid,
    .mark_rollback   = stm32_flash_mark_rollback,
};

static exarcia_client_t client;

/* ---- printf redirect to UART2 ---- */
int _write(int file, char *ptr, int len) {
    (void)file;
    HAL_UART_Transmit(&huart2, (uint8_t *)ptr, (uint32_t)len, 1000);
    return len;
}

/* ---- peripherals ---- */
static void MX_GPIO_Init(void) {
    __HAL_RCC_GPIOA_CLK_ENABLE();
    hled_pin.Pin   = GPIO_PIN_5;
    hled_pin.Mode  = GPIO_MODE_OUTPUT_PP;
    hled_pin.Pull  = GPIO_NOPULL;
    hled_pin.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOA, &hled_pin);
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, 0);
}

static void MX_USART2_Init(void) {
    __HAL_RCC_USART2_CLK_ENABLE();

    /* Configure PA2 (TX) and PA3 (RX) as AF7 (USART2) */
    GPIOA->AFR[0] |= (0x07UL << (2 * 4)) | (0x07UL << (3 * 4));
    GPIOA->MODER   = (GPIOA->MODER & ~(0x3UL << (2 * 2))) | (0x02UL << (2 * 2));
    GPIOA->MODER   = (GPIOA->MODER & ~(0x3UL << (3 * 2))) | (0x02UL << (3 * 2));

    huart2.Instance          = USART2;
    huart2.Init.BaudRate     = 115200;
    huart2.Init.WordLength   = USART_WORDLENGTH_8B;
    huart2.Init.StopBits     = USART_STOPBITS_1;
    huart2.Init.Parity       = USART_PARITY_NONE;
    huart2.Init.Mode         = USART_MODE_TX_RX;
    HAL_UART_Init(&huart2);
}

/* ---- SysTick handler (called from startup vector) ---- */
void SysTick_Handler(void) {
    HAL_IncTick();
}

/* ---- main ---- */
int main(void) {
    HAL_Init();
    SystemInit();
    MX_GPIO_Init();
    MX_USART2_Init();

    printf("\r\n--- Exarcia STM32 OTA Demo ---\r\n");
    printf("Firmware: %s\r\n", EXARCIA_FIRMWARE_VERSION);
    printf("Core URL: %s\r\n", EXARCIA_BUILD_CORE_URL);
    printf("Identity: %s\r\n", EXARCIA_BUILD_DEVICE_IDENTITY);
    printf("Org:      %s\r\n", EXARCIA_BUILD_ORG_SLUG);
    printf("Batch:    %s\r\n", EXARCIA_BUILD_BATCH_ID);

    exarcia_client_init(&client, &uart_transport, &stm32_flash);

    /* Brief LED flash to show boot */
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, 1);
    HAL_Delay(200);
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, 0);

    while (1) {
        uint32_t now = HAL_GetTick();
        exarcia_client_tick(&client, now);

        /* Blink LED at 1 Hz when idle */
        if (client.state == EXARCIA_STATE_REGISTERED ||
            client.state == EXARCIA_STATE_CONFIRMED) {
            if ((now / 500) % 2) { HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, 1); }
            else                 { HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, 0); }
        }
        /* Solid LED during download */
        else if (client.state == EXARCIA_STATE_DOWNLOADING) {
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, 1);
        }

        HAL_Delay(10);
    }
}
