#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "stm32f4xx_hal.h"

/* UART2 transport — bridges to Exarcia CLI over serial */
bool exarcia_uart_transport_init(void);
bool exarcia_uart_transport_get(const char *url, const char *auth, char *buf, size_t buf_len, size_t *out_len);
bool exarcia_uart_transport_post(const char *url, const char *auth, const char *content_type,
                                 const char *body, size_t body_len,
                                 char *resp, size_t resp_len, size_t *out_len);
bool exarcia_uart_transport_https_get_bin(const char *url, uint8_t *buf, size_t buf_len, size_t *out_len);
void exarcia_uart_transport_deinit(void);

/* HAL handles — defined in main.c, used by transport layer */
extern UART_HandleTypeDef huart2;
