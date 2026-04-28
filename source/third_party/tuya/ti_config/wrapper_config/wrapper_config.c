/*========= Necessary Include Headers =========*/
#include "tkl_board_config.h"  
#include "tuya_iot_config.h" 
#include "tuya_cloud_types.h"
#include "tuya_iot.h" 
#include "wrapper_config.h"
#include <string.h>
/*========= Necessary Include Headers =========*/

// Singleton instance
static tuya_iot_client_t g_client;


// Null exception check
static inline int null_check(const minimal_config_iot_t *c){
    return c && c->product_id && c->product_id[0]
             && c->uuid && c->uuid[0]
             && c->auth_key && c->auth_key[0]
             && c->software_ver && c->software_ver[0];
}


// Wrapped i
int config_minimal_init(const minimal_config_iot_t * conf){
    if(!(null_check(conf))) return NULL_EXCEPTION;

    tuya_iot_config_t config;
    memset(&config,0,sizeof(config));

    config.productkey   = conf->product_id;
    config.uuid         = conf->uuid;
    config.authkey      = conf->auth_key;
    config.software_ver = conf->software_ver;

    return tuya_iot_init(&g_client,&config);

}

void *get_wrapped_client(void){
    return (void*)&g_client;
}