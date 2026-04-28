#ifndef WRAPPER_CLIENT_H
#define WRAPPER_CLIENT_H

#ifdef __cplusplus
extern "C" {
#endif

#define NULL_EXCEPTION (-1)


typedef struct minimal_config_iot_t {
  const char *product_id;
  const char *uuid;
  const char *auth_key;
  const char *software_ver;
} minimal_config_iot_t;

/* client is opaque here to avoid pulling Tuya headers into application files */
int config_minimal_init(const minimal_config_iot_t *conf);
void *get_wrapped_client(void);
#ifdef __cplusplus
}
#endif

#endif
