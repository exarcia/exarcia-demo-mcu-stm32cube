/* exarcia_client.c — platform-agnostic OTA client for STM32 bare-metal */
#include <stdio.h>
#include <string.h>

#include "exarcia_client.h"
#include "exarcia_flash.h"

/* ---- minimal JSON helpers (no external dependency) ---- */

static const char *json_find_key(const char *json, const char *key) {
    char needle[64];
    size_t klen = strlen(key);
    if (klen > 60) return NULL;
    snprintf(needle, sizeof(needle), "\"%s\"", key);
    const char *p = strstr(json, needle);
    if (!p) return NULL;
    p += klen + 2;
    while (*p == ' ' || *p == ':') p++;
    return p;
}

static bool json_get_string(const char *json, const char *key, char *out, size_t out_len) {
    const char *p = json_find_key(json, key);
    if (!p || *p != '"') return false;
    p++;
    size_t i = 0;
    while (*p && *p != '"' && i < out_len - 1) { out[i++] = *p++; }
    out[i] = '\0';
    return i > 0;
}

static bool json_get_number(const char *json, const char *key, size_t *out) {
    const char *p = json_find_key(json, key);
    if (!p) return false;
    *out = 0;
    while (*p >= '0' && *p <= '9') { *out = (*out * 10) + (*p - '0'); p++; }
    return true;
}

static bool json_get_bool(const char *json, const char *key) {
    const char *p = json_find_key(json, key);
    if (!p) return false;
    return (p[0] == 't');
}

/* ---- hex decode for SHA-256 ---- */

static int hex_digit(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    return -1;
}

static bool hex_decode(const char *hex, uint8_t *out, size_t out_len) {
    for (size_t i = 0; i < out_len; i++) {
        int hi = hex_digit(hex[i * 2]);
        int lo = hex_digit(hex[i * 2 + 1]);
        if (hi < 0 || lo < 0) return false;
        out[i] = (uint8_t)((hi << 4) | lo);
    }
    return true;
}

/* ---- build auth header ---- */

static void build_auth(exarcia_client_t *c) {
    snprintf(c->auth_header, sizeof(c->auth_header),
             "{\"identity\":\"%s\",\"identity_type\":\"%s\",\"psk\":\"%s\"}",
             EXARCIA_BUILD_DEVICE_IDENTITY,
             EXARCIA_BUILD_DEVICE_IDENTITY_TYPE,
             EXARCIA_BUILD_DEVICE_PSK);
}

/* ---- public API ---- */

void exarcia_client_init(exarcia_client_t *client,
                         const exarcia_transport_t *transport,
                         const exarcia_flash_t *flash) {
    memset(client, 0, sizeof(*client));
    client->transport = transport;
    client->flash     = flash;
    client->state     = EXARCIA_STATE_INIT;
    build_auth(client);
}

bool exarcia_client_register(exarcia_client_t *client) {
    client->state = EXARCIA_STATE_REGISTERING;

    char url[256];
    snprintf(url, sizeof(url), "%s/v1/orgs/%s/devices/%s/register",
             EXARCIA_BUILD_CORE_URL, EXARCIA_BUILD_ORG_SLUG, EXARCIA_BUILD_DEVICE_IDENTITY);

    char body[256];
    snprintf(body, sizeof(body),
             "{\"batch_id\":\"%s\",\"firmware_version\":\"%s\",\"platform\":\"stm32cube\"}",
             EXARCIA_BUILD_BATCH_ID, EXARCIA_FIRMWARE_VERSION);

    char resp[512];
    size_t resp_len = 0;
    if (!client->transport->http_post(url, client->auth_header, "application/json",
                                      body, strlen(body), resp, sizeof(resp), &resp_len)) {
        client->state = EXARCIA_STATE_INIT;
        return false;
    }
    resp[resp_len] = '\0';

    bool ok = json_get_bool(resp, "registered");
    if (ok) {
        snprintf(client->device_url, sizeof(client->device_url),
                 "%s/v1/orgs/%s/devices/%s",
                 EXARCIA_BUILD_CORE_URL, EXARCIA_BUILD_ORG_SLUG, EXARCIA_BUILD_DEVICE_IDENTITY);
        client->registered = true;
        client->state = EXARCIA_STATE_REGISTERED;
    } else {
        client->state = EXARCIA_STATE_INIT;
    }
    return ok;
}

bool exarcia_client_heartbeat(exarcia_client_t *client) {
    if (!client->registered) return false;

    char url[288];
    snprintf(url, sizeof(url), "%s/heartbeat", client->device_url);

    char body[256];
    snprintf(body, sizeof(body),
             "{\"firmware_version\":\"%s\",\"platform\":\"stm32cube\",\"uptime_ms\":%lu}",
             EXARCIA_FIRMWARE_VERSION, (unsigned long)HAL_GetTick());

    char resp[256];
    size_t resp_len = 0;
    bool ok = client->transport->http_post(url, client->auth_header, "application/json",
                                           body, strlen(body), resp, sizeof(resp), &resp_len);
    if (!ok) return false;

    /* Check for incoming deployment directive */
    resp[resp_len] = '\0';
    const char *deploy = strstr(resp, "\"deployment\"");
    if (deploy && strstr(deploy, "true")) {
        /* Deployment pending — poll will pick it up */
    }
    return true;
}

bool exarcia_client_poll_deployment(exarcia_client_t *client) {
    if (!client->registered) return false;

    char url[288];
    snprintf(url, sizeof(url), "%s/deployments/current", client->device_url);

    char resp[1024];
    size_t resp_len = 0;
    bool ok = client->transport->http_get(url, client->auth_header, resp, sizeof(resp), &resp_len);
    if (!ok) return false;
    resp[resp_len] = '\0';

    bool has_deploy = json_get_bool(resp, "has_deployment");
    if (!has_deploy) return true; /* No deployment — not an error */

    size_t deploy_size = 0;
    json_get_number(resp, "size", &deploy_size);
    if (deploy_size == 0) return false;

    char sha256_hex[65] = {0};
    if (!json_get_string(resp, "sha256", sha256_hex, sizeof(sha256_hex))) return false;

    char artifact_url[512] = {0};
    if (!json_get_string(resp, "download_url", artifact_url, sizeof(artifact_url))) return false;

    /* Download firmware binary */
    client->state = EXARCIA_STATE_DOWNLOADING;
    if (deploy_size > EXARCIA_FLASH_BANK0_SIZE) return false;

    uint8_t *dl_buf = (uint8_t *)EXARCIA_FLASH_BANK1_ADDR;
    size_t dl_len = 0;
    if (!client->transport->https_get_bin(artifact_url, dl_buf, deploy_size, &dl_len)) {
        client->state = EXARCIA_STATE_REGISTERED;
        return false;
    }
    if (dl_len != deploy_size) {
        client->state = EXARCIA_STATE_REGISTERED;
        return false;
    }

    /* Verify SHA-256 */
    uint8_t expected_sha[32];
    if (!hex_decode(sha256_hex, expected_sha, 32)) {
        client->state = EXARCIA_STATE_REGISTERED;
        return false;
    }
    if (!client->flash->validate_sha256(EXARCIA_FLASH_BANK1_ADDR, dl_len, expected_sha)) {
        client->state = EXARCIA_STATE_REGISTERED;
        return false;
    }

    /* Install: swap banks and reboot */
    client->state = EXARCIA_STATE_INSTALLING;
    client->flash->mark_valid();
    client->flash->swap_bank();
    return true;
}

bool exarcia_client_confirm(exarcia_client_t *client) {
    if (!client->registered) return false;

    char url[288];
    snprintf(url, sizeof(url), "%s/deployments/confirm", client->device_url);

    char body[128];
    snprintf(body, sizeof(body), "{\"firmware_version\":\"%s\"}", EXARCIA_FIRMWARE_VERSION);

    char resp[128];
    size_t resp_len = 0;
    bool ok = client->transport->http_post(url, client->auth_header, "application/json",
                                           body, strlen(body), resp, sizeof(resp), &resp_len);
    if (ok) {
        client->state = EXARCIA_STATE_CONFIRMED;
        client->rollback_pending = false;
    }
    return ok;
}

void exarcia_client_tick(exarcia_client_t *client, uint32_t now_ms) {
    switch (client->state) {
    case EXARCIA_STATE_INIT:
        if (exarcia_client_register(client)) {
            client->last_heartbeat_ms = now_ms;
            client->last_poll_ms = now_ms;
        }
        break;

    case EXARCIA_STATE_REGISTERED:
    case EXARCIA_STATE_CONFIRMED:
        if (now_ms - client->last_heartbeat_ms >= EXARCIA_HEARTBEAT_MS) {
            exarcia_client_heartbeat(client);
            client->last_heartbeat_ms = now_ms;
        }
        if (now_ms - client->last_poll_ms >= EXARCIA_POLL_INTERVAL_MS) {
            exarcia_client_poll_deployment(client);
            client->last_poll_ms = now_ms;
        }
        break;

    default:
        break;
    }
}
