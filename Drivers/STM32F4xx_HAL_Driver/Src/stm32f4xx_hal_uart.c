#include "stm32f4xx_hal.h"
#include "stm32f407xx.h"

void HAL_UART_Init(UART_HandleTypeDef *huart) {
    USART_TypeDef *uart = huart->Instance;
    uart->CR1 = 0;
    /* BRR: 168 MHz / 4 / 115200 = 364.58 -> mantissa 364, frac 9 */
    uart->BRR = (364UL << 4) | 9UL;
    uart->CR1 = huart->Init.WordLength | huart->Init.Parity | USART_CR1_UE;
    uart->CR2 = huart->Init.StopBits;
    uart->CR3 = 0;
    uart->CR1 |= USART_CR1_TE | USART_CR1_RE;
}

void HAL_UART_Transmit(UART_HandleTypeDef *huart, const uint8_t *data, uint32_t len, uint32_t timeout) {
    (void)timeout;
    for (uint32_t i = 0; i < len; i++) {
        while (!(huart->Instance->SR & USART_SR_TXE)) {}
        huart->Instance->DR = data[i];
    }
    while (!(huart->Instance->SR & USART_SR_TC)) {}
}
