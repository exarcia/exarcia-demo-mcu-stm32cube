/* exarcia_network_stm32.c — UART-bridge transport for STM32 bare-metal
 *
 * This implements the exarcia_transport_t interface using USART2 as a
 * bidirectional bridge to the Exarcia CLI on a host machine.  The host
 * runs `exarcia serve-uart` (planned CLI command) which translates
 * serial commands into Core API calls.
 *
 * Wire format (simplified JSON-over-UART):
 *   TX (device -> host):  {"cmd":"GET","url":"...","auth":"..."}\n
 *   TX (device -> host):  {"cmd":"POST","url":"...","auth":"...","body":"..."}\n
 *   RX (host -> device):  {"status":200,"body":"..."}\n
 *
 * For production use, replace this with lwIP TCP/IP, a cellular modem
 * driver, or another network stack.
 */

#include <stdio.h>
#include <string.h>

#include "exarcia_network.h"
#include "exarcia_client.h"

#define UART_RX_BUF_SIZE 2048

static size_t rx_pos = 0;

static bool uart_read_line(char *buf, size_t buf_len, size_t *out_len, uint32_t timeout_ms) {
    uint32_t start = HAL_GetTick();
    rx_pos = 0;
    while ((HAL_GetTick() - start) < timeout_ms) {
        if (huart2.Instance->SR & USART_SR_RXNE) {
            char c = (char)(huart2.Instance->DR & 0xFF);
            if (c == '\n') {
                buf[rx_pos] = '\0';
                *out_len = rx_pos;
                return rx_pos > 0;
            }
            if (rx_pos < buf_len - 1) {
                buf[rx_pos++] = c;
            }
        }
    }
    return false;
}

static bool uart_send_line(const char *line) {
    HAL_UART_Transmit(&huart2, (const uint8_t *)line, strlen(line), 1000);
    HAL_UART_Transmit(&huart2, (const uint8_t *)"\n", 1, 100);
    return true;
}

bool exarcia_uart_transport_init(void) {
    rx_pos = 0;
    return true;
}

bool exarcia_uart_transport_get(const char *url, const char *auth,
                                char *buf, size_t buf_len, size_t *out_len) {
    char cmd[512];
    snprintf(cmd, sizeof(cmd), "{\"cmd\":\"GET\",\"url\":\"%s\",\"auth\":%s}", url, auth);
    if (!uart_send_line(cmd)) return false;
    return uart_read_line(buf, buf_len, out_len, 10000);
}

bool exarcia_uart_transport_post(const char *url, const char *auth, const char *content_type,
                                 const char *body, size_t body_len,
                                 char *resp, size_t resp_len, size_t *out_len) {
    (void)content_type;
    char cmd[2048];
    snprintf(cmd, sizeof(cmd), "{\"cmd\":\"POST\",\"url\":\"%s\",\"auth\":%s,\"body\":%.*s}",
             url, auth, (int)body_len, body);
    if (!uart_send_line(cmd)) return false;
    return uart_read_line(resp, resp_len, out_len, 10000);
}

bool exarcia_uart_transport_https_get_bin(const char *url, uint8_t *buf, size_t buf_len, size_t *out_len) {
    /* Binary transfer over UART: host sends raw bytes preceded by length header */
    char cmd[512];
    snprintf(cmd, sizeof(cmd), "{\"cmd\":\"GET_BIN\",\"url\":\"%s\",\"max_size\":%zu}", url, buf_len);
    if (!uart_send_line(cmd)) return false;

    /* Read length line */
    size_t hdr_len = 0;
    char hdr[32];
    if (!uart_read_line(hdr, sizeof(hdr), &hdr_len, 10000)) return false;

    size_t bin_len = 0;
    for (size_t i = 0; i < hdr_len && hdr[i] >= '0' && hdr[i] <= '9'; i++) {
        bin_len = bin_len * 10 + (hdr[i] - '0');
    }
    if (bin_len == 0 || bin_len > buf_len) return false;

    /* Read raw binary data */
    uint32_t start = HAL_GetTick();
    size_t pos = 0;
    while (pos < bin_len && (HAL_GetTick() - start) < 60000) {
        if (huart2.Instance->SR & USART_SR_RXNE) {
            buf[pos++] = (uint8_t)(huart2.Instance->DR & 0xFF);
        }
    }
    *out_len = pos;
    return pos == bin_len;
}

void exarcia_uart_transport_deinit(void) {
    rx_pos = 0;
}
