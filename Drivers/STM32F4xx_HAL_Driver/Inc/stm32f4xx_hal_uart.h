#pragma once

#include "stm32f4xx.h"
#include "stm32f407xx.h"

#define USART_WORDLENGTH_8B 0x00000000U
#define USART_STOPBITS_1    0x00000000U
#define USART_PARITY_NONE   0x00000000U
#define USART_MODE_TX_RX    (USART_CR1_TE | USART_CR1_RE)

void HAL_UART_Init(UART_HandleTypeDef *huart);
void HAL_UART_Transmit(UART_HandleTypeDef *huart, const uint8_t *data, uint32_t len, uint32_t timeout);
