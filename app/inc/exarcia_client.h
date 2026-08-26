#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* HAL_GetTick — provided by the STM32 HAL, forward-declared here so the
 * client protocol code stays platform-independent at the header level. */
uint32_t HAL_GetTick(void);

/* ---------- build-time configuration (from .env.local via scripts) ---------- */

#ifndef EXARCIA_BUILD_CORE_URL
#define EXARCIA_BUILD_CORE_URL ""
#endif
#ifndef EXARCIA_BUILD_DEVICE_IDENTITY
#define EXARCIA_BUILD_DEVICE_IDENTITY ""
#endif
#ifndef EXARCIA_BUILD_DEVICE_IDENTITY_TYPE
#define EXARCIA_BUILD_DEVICE_IDENTITY_TYPE ""
#endif
#ifndef EXARCIA_BUILD_DEVICE_PSK
#define EXARCIA_BUILD_DEVICE_PSK ""
#endif
#ifndef EXARCIA_BUILD_ORG_SLUG
#define EXARCIA_BUILD_ORG_SLUG ""
#endif
#ifndef EXARCIA_BUILD_BATCH_ID
#define EXARCIA_BUILD_BATCH_ID ""
#endif
#ifndef EXARCIA_BUILD_PROVISIONING_METHOD
#define EXARCIA_BUILD_PROVISIONING_METHOD "individual"
#endif

#define EXARCIA_FIRMWARE_VERSION "0.1.0-dev"
#define EXARCIA_POLL_INTERVAL_MS  60000
#define EXARCIA_HEARTBEAT_MS     300000

/* ---------- transport abstraction ---------- */

typedef struct {
    bool (*init)(void);
    bool (*http_get)(const char *url, const char *auth, char *buf, size_t buf_len, size_t *out_len);
    bool (*http_post)(const char *url, const char *auth, const char *content_type,
                      const char *body, size_t body_len,
                      char *resp, size_t resp_len, size_t *out_len);
    bool (*https_get_bin)(const char *url, uint8_t *buf, size_t buf_len, size_t *out_len);
    void (*deinit)(void);
} exarcia_transport_t;

/* ---------- flash write abstraction ---------- */

typedef struct {
    bool (*erase_bank)(uint32_t bank_addr, uint32_t bank_size);
    bool (*write_chunk)(uint32_t addr, const uint8_t *data, size_t len);
    bool (*validate_sha256)(uint32_t addr, size_t len, const uint8_t *expected);
    bool (*swap_bank)(void);
    void (*mark_valid)(void);
    void (*mark_rollback)(void);
} exarcia_flash_t;

/* ---------- client state ---------- */

typedef enum {
    EXARCIA_STATE_INIT,
    EXARCIA_STATE_REGISTERING,
    EXARCIA_STATE_REGISTERED,
    EXARCIA_STATE_POLLING,
    EXARCIA_STATE_DOWNLOADING,
    EXARCIA_STATE_INSTALLING,
    EXARCIA_STATE_CONFIRMED,
} exarcia_state_t;

typedef struct {
    const exarcia_transport_t *transport;
    const exarcia_flash_t     *flash;
    exarcia_state_t            state;
    char                       device_url[192];
    char                       auth_header[160];
    uint32_t                   last_heartbeat_ms;
    uint32_t                   last_poll_ms;
    bool                       registered;
    bool                       rollback_pending;
    uint8_t                    pending_sha256[32];
    bool                       pending_sha256_valid;
    size_t                     pending_size;
} exarcia_client_t;

/* ---------- public API ---------- */

void     exarcia_client_init(exarcia_client_t *client,
                             const exarcia_transport_t *transport,
                             const exarcia_flash_t *flash);
bool     exarcia_client_register(exarcia_client_t *client);
bool     exarcia_client_heartbeat(exarcia_client_t *client);
bool     exarcia_client_poll_deployment(exarcia_client_t *client);
bool     exarcia_client_confirm(exarcia_client_t *client);
void     exarcia_client_tick(exarcia_client_t *client, uint32_t now_ms);
