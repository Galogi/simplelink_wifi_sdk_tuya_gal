/**
 * @file tkl_wifi.c
 * @brief Wi-Fi driver implementation for TI CC35xx (SimpleLink)
 * @version Final + RSSI + Status
 * @note Implements Scan, Connect, Disconnect, MAC, RSSI, and Connection Status.
 */
#include "tuya_iot.h"
#include "tkl_wifi.h"
#include "tuya_error_code.h"
#include "tkl_memory.h"
#include "tkl_semaphore.h"
#include "tkl_output.h"
#include "errors.h"
#include <string.h>
#include <stdio.h>

/* --- LwIP Includes for IP Address Handling --- */
#include <lwip/netif.h>
#include <lwip/ip_addr.h>

/* --- TI SDK Wi-Fi Host Driver --- */
#include <ti/drivers/net/wifi/wifi_host_driver/inc_adapt/wlan_if.h>
#include <ti/drivers/net/wifi/wifi_host_driver/inc_adapt/osi_kernel.h>

/* --- Macros & Constants --- */
#ifndef WLAN_MAX_SCAN_COUNT
#define WLAN_MAX_SCAN_COUNT 20
#endif

/* --- Global Variables --- */
static WIFI_EVENT_CB g_wifi_event_cb = NULL;
static TKL_SEM_HANDLE g_scan_sem = NULL;
static AP_IF_S *g_scan_results_ptr = NULL;
static uint32_t g_scan_count = 0;
static BOOL_T g_wifi_initialized = FALSE;
static BOOL_T g_disconnect_requested = FALSE;
static BOOL_T g_sta_role_up = FALSE;
static BOOL_T g_ap_role_up = FALSE;
static WF_WK_MD_E g_requested_mode = WWM_STATION;
static uint8_t g_country_domain[3] = {'E', 'U', 'I'};

/* New: Track current connection status locally */
static WF_STATION_STAT_E g_wifi_status = WSS_IDLE;

/* network_terminal AP/LwIP helpers reused by the Tuya TKL layer */
extern void network_stack_add_if_ap(void);
extern void network_stack_remove_if_ap(void);
extern void network_stack_add_if_sta(void);
extern void network_stack_remove_if_sta(void);
extern void *network_get_ap_if(void);
extern void network_set_up(void *newif);
extern int8_t network_stack_set_dynamic_ip_if_ap(uint32_t ip, uint32_t netmask, uint32_t gw);
extern int8_t network_stack_get_if_ip(WlanRole_e role, uint32_t *ip, uint32_t *netmask, uint32_t *gw, uint32_t *dhcp);
extern int Report(const char *pcFormat, ...);
extern char g_wpsConfigMethods[];
extern const char g_manufacturer[];
extern const char g_modelName[];
extern const char g_modelNumber[];
extern const char g_serialNumber[];
extern const unsigned char g_uuid_string[];
extern const char g_primaryDeviceType[];

/* --- Helpers --- */

/**
 * @brief Convert TI Security Bitmap to Tuya Auth Mode
 */
static WF_AP_AUTH_MODE_E _ti_sec_to_tuya(uint16_t security_info)
{
    uint8_t sec_bitmap = WLAN_SCAN_RESULT_SEC_TYPE_BITMAP(security_info);

    switch (sec_bitmap) {
        case WLAN_SEC_TYPE_OPEN:      return WAAM_OPEN;
        case WLAN_SEC_TYPE_WPA_WPA2:  return WAAM_WPA2_PSK; 
        case WLAN_SEC_TYPE_WPA2_PLUS: return WAAM_WPA2_PSK;
        case WLAN_SEC_TYPE_WPA3:      return WAAM_WPA_WPA3_SAE; 
        default:                      return WAAM_WPA2_PSK;
    }
}

static BOOL_T _wifi_station_has_ipv4(void)
{
    struct netif *nif = netif_default;

    if (nif == NULL || !netif_is_up(nif)) {
        return FALSE;
    }

    return ip4_addr_isany_val(*netif_ip4_addr(nif)) ? FALSE : TRUE;
}

static WF_STATION_STAT_E _map_disconnect_reason(int16_t reason_code)
{
    switch (reason_code) {
        case WLAN_DISCONNECT_MIC_FAILURE:
        case WLAN_DISCONNECT_FOURWAY_HANDSHAKE_TIMEOUT:
        case WLAN_DISCONNECT_GROUPKEY_HANDSHAKE_TIMEOUT:
        case WLAN_DISCONNECT_INVALID_GROUP_CIPHER:
        case WLAN_DISCONNECT_INVALID_PAIRWISE_CIPHER:
        case WLAN_DISCONNECT_INVALID_AKMP:
        case WLAN_DISCONNECT_SECURITY_FAILURE:
        case WLAN_DISCONNECT_AUTH_TIMEOUT:
            return WSS_PASSWD_WRONG;
        case WLAN_DISCONNECT_ASSOC_TIMEOUT:
            return WSS_NO_AP_FOUND;
        default:
            return WSS_CONN_FAIL;
    }
}

static BOOL_T _ssid_matches_filter(const int8_t *filter, const AP_IF_S *entry)
{
    if (filter == NULL) {
        return TRUE;
    }

    size_t filter_len = strlen((const char *)filter);
    if (filter_len != entry->s_len) {
        return FALSE;
    }

    return (memcmp(entry->ssid, filter, filter_len) == 0) ? TRUE : FALSE;
}

static void _wifi_trace(const char *stage)
{
    if (stage != NULL) {
        printf("[TKL_WIFI] %s\n\r", stage);
    }
}

static const char *_work_mode_to_str(WF_WK_MD_E mode)
{
    switch (mode) {
        case WWM_STATION:   return "station";
        case WWM_SOFTAP:    return "softap";
        case WWM_STATIONAP: return "stationap";
        case WWM_POWERDOWN: return "powerdown";
        case WWM_SNIFFER:   return "sniffer";
        default:            return "unknown";
    }
}

static WF_WK_MD_E _get_current_work_mode(void)
{
    if (g_sta_role_up && g_ap_role_up) {
        return WWM_STATIONAP;
    }

    if (g_ap_role_up) {
        return WWM_SOFTAP;
    }

    if (g_sta_role_up) {
        return WWM_STATION;
    }

    return WWM_POWERDOWN;
}

static void _fill_default_wps_params(WpsParams_t *wps)
{
    if (wps == NULL) {
        return;
    }

    memset(wps, 0, sizeof(*wps));
    wps->deviceName = (char *)g_modelName;
    wps->configMethods = (char *)g_wpsConfigMethods;
    wps->manufacturer = (char *)g_manufacturer;
    wps->modelName = (char *)g_modelName;
    wps->modelNumber = (char *)g_modelNumber;
    wps->serialNumber = (char *)g_serialNumber;
    wps->uuid = (uint8_t *)g_uuid_string;
    wps->deviceType = (uint8_t *)g_primaryDeviceType;
}

static uint8_t _tuya_ap_sec_to_ti(WF_AP_AUTH_MODE_E mode)
{
    switch (mode) {
        case WAAM_OPEN:           return WLAN_SEC_TYPE_OPEN;
        case WAAM_WPA_PSK:
        case WAAM_WPA2_PSK:
        case WAAM_WPA_WPA2_PSK:   return WLAN_SEC_TYPE_WPA_WPA2;
        case WAAM_WPA_WPA3_SAE:   return WLAN_SEC_TYPE_WPA3;
        default:                  return WLAN_SEC_TYPE_WPA_WPA2;
    }
}

static BOOL_T _has_ip_string(const char *ip)
{
    return (ip != NULL && ip[0] != '\0') ? TRUE : FALSE;
}

static void _set_country_domain(COUNTRY_CODE_E ccode)
{
    switch (ccode) {
        case COUNTRY_CODE_US:
            g_country_domain[0] = 'U';
            g_country_domain[1] = 'S';
            g_country_domain[2] = 'I';
            break;
        case COUNTRY_CODE_JP:
            g_country_domain[0] = 'J';
            g_country_domain[1] = 'P';
            g_country_domain[2] = 'I';
            break;
        case COUNTRY_CODE_EU:
            g_country_domain[0] = 'E';
            g_country_domain[1] = 'U';
            g_country_domain[2] = 'I';
            break;
        case COUNTRY_CODE_CN:
        default:
            g_country_domain[0] = 'C';
            g_country_domain[1] = 'N';
            g_country_domain[2] = 'I';
            break;
    }
}

static int _wifi_role_up_sta(void)
{
    int ret;
    RoleUpStaCmd_t staParams = {0};

    if (g_sta_role_up == TRUE) {
        return 0;
    }

    network_stack_add_if_sta();

    staParams.countryDomain[0] = g_country_domain[0];
    staParams.countryDomain[1] = g_country_domain[1];
    staParams.countryDomain[2] = g_country_domain[2];
    staParams.wpsDisabled = FALSE;
    _fill_default_wps_params(&staParams.wpsParams);
    staParams.p2pDeviceEnabled = FALSE;

    ret = Wlan_RoleUp(WLAN_ROLE_STA, &staParams, WLAN_WAIT_FOREVER);
    if (ret == 0) {
        g_sta_role_up = TRUE;
    } else {
        network_stack_remove_if_sta();
    }

    return ret;
}

static int _wifi_role_down_sta(void)
{
    int ret;

    if (g_sta_role_up == FALSE) {
        return 0;
    }

    ret = Wlan_RoleDown(WLAN_ROLE_STA, WLAN_WAIT_FOREVER);
    if (ret == 0) {
        network_stack_remove_if_sta();
        g_sta_role_up = FALSE;
        g_wifi_status = WSS_IDLE;
    }

    return ret;
}

static int _wifi_role_down_ap(void)
{
    int ret;

    if (g_ap_role_up == FALSE) {
        return 0;
    }

    network_stack_remove_if_ap();

    ret = Wlan_RoleDown(WLAN_ROLE_AP, WLAN_WAIT_FOREVER);
    if (ret == 0) {
        g_ap_role_up = FALSE;
    }

    return ret;
}

static void _wifi_apply_ap_ip_config(const WF_AP_CFG_IF_S *cfg)
{
    uint32_t ip;
    uint32_t netmask;
    uint32_t gw;

    if (cfg == NULL || _has_ip_string((const char *)cfg->ip.ip) == FALSE) {
        return;
    }

    ip = ipaddr_addr((const char *)cfg->ip.ip);
    netmask = _has_ip_string((const char *)cfg->ip.mask) ? ipaddr_addr((const char *)cfg->ip.mask)
                                                         : ipaddr_addr("255.255.255.0");
    gw = _has_ip_string((const char *)cfg->ip.gw) ? ipaddr_addr((const char *)cfg->ip.gw)
                                                   : ip;

    (void)network_stack_set_dynamic_ip_if_ap(ip, netmask, gw);
}

/* --- TI Event Handler --- */
void TiWlanEventHandler(WlanEvent_t *pWlanEvent)
{
    if (pWlanEvent == NULL) return;

    switch (pWlanEvent->Id)
    {
        case WLAN_EVENT_CONNECTING:
            g_wifi_status = WSS_CONNECTING;
            break;

        case WLAN_EVENT_CONNECT:
            /* Update Status: Connected */
            g_wifi_status = WSS_CONN_SUCCESS;
            if (g_wifi_event_cb) {
                g_wifi_event_cb(WFE_CONNECTED, NULL);
            }
            break;

        case WLAN_EVENT_AUTHENTICATION_REJECTED:
        case WLAN_EVENT_ASSOCIATION_REJECTED:
            g_wifi_status = WSS_CONN_FAIL;
            if (g_wifi_event_cb) {
                g_wifi_event_cb(WFE_CONNECT_FAILED, NULL);
            }
            break;

        case WLAN_EVENT_DISCONNECT:
            if (pWlanEvent->Data.Disconnect.IsStaIsDiscnctInitiator || g_disconnect_requested) {
                g_wifi_status = WSS_IDLE;
            } else {
                g_wifi_status = _map_disconnect_reason(pWlanEvent->Data.Disconnect.ReasonCode);
                if (g_wifi_event_cb) {
                    g_wifi_event_cb(WFE_CONNECT_FAILED, NULL);
                }
            }
            g_disconnect_requested = FALSE;
            if (g_wifi_event_cb) {
                g_wifi_event_cb(WFE_DISCONNECTED, NULL);
            }
            break;

        case WLAN_EVENT_SCAN_RESULT:
{
    printf("\n\r[TKL WIFI] scan event entered\n\r");

    if (g_scan_sem != NULL) {
        WlanEventScanResult_t *scan_data = &pWlanEvent->Data.ScanResult;
        uint32_t count = scan_data->NetworkListResultLen;

        if (count > WLAN_MAX_SCAN_COUNT) {
            count = WLAN_MAX_SCAN_COUNT;
        }

        if (g_scan_results_ptr != NULL) {
            tkl_system_free(g_scan_results_ptr);
            g_scan_results_ptr = NULL;
        }

        g_scan_results_ptr = (AP_IF_S *)tkl_system_malloc(sizeof(AP_IF_S) * count);

        if (g_scan_results_ptr != NULL) {
            memset(g_scan_results_ptr, 0, sizeof(AP_IF_S) * count);

            for (uint32_t i = 0; i < count; i++) {
                WlanNetworkEntry_t *entry = &scan_data->NetworkListResult[i];

                g_scan_results_ptr[i].s_len = entry->SsidLen;
                if (g_scan_results_ptr[i].s_len > WIFI_SSID_LEN) {
                    g_scan_results_ptr[i].s_len = WIFI_SSID_LEN;
                }

                memcpy(g_scan_results_ptr[i].ssid, entry->Ssid, g_scan_results_ptr[i].s_len);
                g_scan_results_ptr[i].ssid[g_scan_results_ptr[i].s_len] = '\0';

                memcpy(g_scan_results_ptr[i].bssid, entry->Bssid, 6);
                g_scan_results_ptr[i].rssi = entry->Rssi;
                g_scan_results_ptr[i].channel = entry->Channel;
                g_scan_results_ptr[i].security = _ti_sec_to_tuya(entry->SecurityInfo);
            }

            g_scan_count = count;
        } else {
            g_scan_count = 0;
        }

        printf("\n\r[TKL WIFI] posting scan semaphore, count=%u\n\r", g_scan_count);
        tkl_semaphore_post(g_scan_sem);
        printf("\n\r[TKL WIFI] scan semaphore posted\n\r");
    }

    break;
}

        default:
            break;
    }
}

/* --- Core TKL Functions --- */

OPERATE_RET tkl_wifi_init(WIFI_EVENT_CB cb)
{
    _wifi_trace("enter tkl_wifi_init");

    if (cb != NULL) {
        g_wifi_event_cb = cb;
    }

    if (g_wifi_initialized == TRUE) {
        _wifi_trace("already initialized");
        return OPRT_OK;
    }

    if (g_scan_sem == NULL && tkl_semaphore_create_init(&g_scan_sem, 0, 1) != OPRT_OK) {
        _wifi_trace("scan semaphore create failed");
        return OPRT_COM_ERROR;
    }

    g_scan_count = 0;
    g_disconnect_requested = FALSE;
    g_sta_role_up = FALSE;
    g_ap_role_up = FALSE;
    g_requested_mode = WWM_STATION;
    g_wifi_status = WSS_IDLE;
    _set_country_domain(COUNTRY_CODE_EU);
    g_wifi_initialized = TRUE;
    printf("[TKL_WIFI] init ok (non-owning)\n\r");
    _wifi_trace("exit tkl_wifi_init");
    return OPRT_OK;
}

OPERATE_RET tkl_wifi_diag_get(TKL_WIFI_DIAG_T *diag)
{
    if (diag == NULL) {
        return OPRT_INVALID_PARM;
    }

    diag->initialized = g_wifi_initialized;
    diag->sta_role_up = g_sta_role_up;
    diag->ap_role_up = g_ap_role_up;
    diag->requested_mode = g_requested_mode;
    diag->current_mode = _get_current_work_mode();
    diag->sta_status = g_wifi_status;
    return OPRT_OK;
}



OPERATE_RET tkl_wifi_scan_ap(const int8_t *ssid, AP_IF_S **ap_ary, uint32_t *num)
{
    int ret;
    AP_IF_S *filtered = NULL;
    uint32_t filtered_count = 0;

    if (ap_ary == NULL || num == NULL) {
        return OPRT_INVALID_PARM;
    }

    *ap_ary = NULL;
    *num = 0;

    if (g_scan_results_ptr != NULL) {
        tkl_system_free(g_scan_results_ptr);
        g_scan_results_ptr = NULL;
    }

    g_scan_count = 0;

    printf("\n\r[TKL WIFI] before Wlan_Scan\n\r");
    ret = Wlan_Scan(WLAN_ROLE_STA, NULL, WLAN_MAX_SCAN_COUNT);
    printf("\n\r[TKL WIFI] after Wlan_Scan ret=%d\n\r", ret);

    if (ret != 0) {
        return OPRT_COM_ERROR;
    }

    printf("\n\r[TKL WIFI] before semaphore wait\n\r");
    ret = tkl_semaphore_wait(g_scan_sem, 3000);
    printf("\n\r[TKL WIFI] after semaphore wait ret=%d\n\r", ret);

    if (ret != OPRT_OK) {
        return OPRT_TIMEOUT;
    }

    printf("\n\r[TKL WIFI] g_scan_results_ptr=%p g_scan_count=%u\n\r",
               g_scan_results_ptr, g_scan_count);

    if (g_scan_results_ptr != NULL) {
        if (ssid == NULL) {
            *ap_ary = g_scan_results_ptr;
            *num = g_scan_count;
            printf("\n\r[TKL WIFI] returning full scan list\n\r");
            return OPRT_OK;
        }

        filtered = (AP_IF_S *)tkl_system_calloc(g_scan_count, sizeof(AP_IF_S));
        if (filtered == NULL) {
            tkl_system_free(g_scan_results_ptr);
            g_scan_results_ptr = NULL;
            return OPRT_MALLOC_FAILED;
        }

        for (uint32_t i = 0; i < g_scan_count; i++) {
            if (_ssid_matches_filter(ssid, &g_scan_results_ptr[i])) {
                filtered[filtered_count++] = g_scan_results_ptr[i];
            }
        }

        tkl_system_free(g_scan_results_ptr);
        g_scan_results_ptr = NULL;

        if (filtered_count == 0) {
            tkl_system_free(filtered);
            return OPRT_OK;
        }

        *ap_ary = filtered;
        *num = filtered_count;
        return OPRT_OK;
    }

    printf("\n\r[TKL WIFI] scan completed but no results ptr\n\r");
    return OPRT_OK;
}

OPERATE_RET tkl_wifi_release_ap(AP_IF_S *ap)
{
    if (ap != NULL) {
        tkl_system_free(ap);
    }
    return OPRT_OK;
}


OPERATE_RET tkl_wifi_station_connect(const int8_t *ssid, const int8_t *passwd)
{
    int16_t ret = 0;
    char sec_type = WLAN_SEC_TYPE_OPEN;
    char *password_ptr = NULL;
    int password_len = 0;

    /* Update status to connecting */
    g_wifi_status = WSS_CONNECTING;

    if (ssid == NULL || strlen((const char *)ssid) == 0U) return OPRT_INVALID_PARM;

    if (passwd == NULL || strlen((char*)passwd) == 0) {
        sec_type = WLAN_SEC_TYPE_OPEN;
    } else {
        sec_type = WLAN_SEC_TYPE_WPA_WPA2;
        password_ptr = (char *)passwd;
        password_len = strlen((char*)passwd);
    }

    ret = Wlan_Connect((signed char *)ssid, strlen((char*)ssid),
                       NULL,          
                       sec_type,      
                       password_ptr,  
                       password_len,  
                       0);            

    if (ret != 0) {
        g_wifi_status = WSS_CONN_FAIL;
        if (g_wifi_event_cb) {
            g_wifi_event_cb(WFE_CONNECT_FAILED, NULL);
        }
        return OPRT_COM_ERROR;
    }

    return OPRT_OK;
}

OPERATE_RET tkl_wifi_station_disconnect(void)
{
    int ret;

    g_disconnect_requested = TRUE;
    ret = Wlan_Disconnect(WLAN_ROLE_STA, NULL);
    if (ret != 0) {
        g_disconnect_requested = FALSE;
        return OPRT_COM_ERROR;
    }

    g_wifi_status = WSS_IDLE;
    return OPRT_OK;
}

OPERATE_RET tkl_wifi_get_mac(const WF_IF_E wf, NW_MAC_S *mac)
{
    WlanMacAddress_t macParam;
    WlanMacAddress_t fallbackMacParam;
    int ret;

    if (mac == NULL) {
        return OPRT_INVALID_PARM;
    }

    memset(&macParam, 0, sizeof(macParam));
    memset(&fallbackMacParam, 0, sizeof(fallbackMacParam));

    switch (wf) {
        case WF_STATION:
            macParam.roleType = WLAN_ROLE_STA;
            break;
        case WF_AP:
            if (g_ap_role_up == FALSE) {
                fallbackMacParam.roleType = (g_sta_role_up == TRUE) ? WLAN_ROLE_STA : WLAN_ROLE_DEVICE;
                ret = Wlan_Get(WLAN_GET_MACADDRESS, &fallbackMacParam);
                if (ret == 0) {
                    memcpy(mac->mac, fallbackMacParam.pMacAddress, 6);
                    tkl_log_output("[TKL_WIFI] get_mac pre-ap fallback role=%d %02X:%02X:%02X:%02X:%02X:%02X\r\n",
                                   fallbackMacParam.roleType,
                                   mac->mac[0], mac->mac[1], mac->mac[2],
                                   mac->mac[3], mac->mac[4], mac->mac[5]);
                    return OPRT_OK;
                }
            }
            macParam.roleType = WLAN_ROLE_AP;
            break;
        default:
            return OPRT_NOT_SUPPORTED;
    }

    ret = Wlan_Get(WLAN_GET_MACADDRESS, &macParam);
    if (ret != 0 && wf == WF_AP) {
        fallbackMacParam.roleType = WLAN_ROLE_STA;
        ret = Wlan_Get(WLAN_GET_MACADDRESS, &fallbackMacParam);
        if (ret == 0) {
            memcpy(mac->mac, fallbackMacParam.pMacAddress, 6);
            tkl_log_output("[TKL_WIFI] get_mac ap-fallback-to-sta %02X:%02X:%02X:%02X:%02X:%02X\r\n",
                           mac->mac[0], mac->mac[1], mac->mac[2],
                           mac->mac[3], mac->mac[4], mac->mac[5]);
            return OPRT_OK;
        }

        fallbackMacParam.roleType = WLAN_ROLE_DEVICE;
        ret = Wlan_Get(WLAN_GET_MACADDRESS, &fallbackMacParam);
        if (ret == 0) {
            memcpy(mac->mac, fallbackMacParam.pMacAddress, 6);
            tkl_log_output("[TKL_WIFI] get_mac ap-fallback-to-device %02X:%02X:%02X:%02X:%02X:%02X\r\n",
                           mac->mac[0], mac->mac[1], mac->mac[2],
                           mac->mac[3], mac->mac[4], mac->mac[5]);
            return OPRT_OK;
        }
    }

    if (ret == 0) {
        memcpy(mac->mac, macParam.pMacAddress, 6);
        tkl_log_output("[TKL_WIFI] get_mac wf=%d %02X:%02X:%02X:%02X:%02X:%02X\r\n",
                       wf,
                       mac->mac[0], mac->mac[1], mac->mac[2],
                       mac->mac[3], mac->mac[4], mac->mac[5]);
        return OPRT_OK;
    }

    tkl_log_output("[TKL_WIFI] get_mac fail wf=%d ret=%d\r\n", wf, ret);
    return OPRT_COM_ERROR;
}

OPERATE_RET tkl_wifi_get_work_mode(WF_WK_MD_E *mode)
{
    if (mode == NULL) {
        return OPRT_INVALID_PARM;
    }

    *mode = _get_current_work_mode();
    return OPRT_OK;
}

OPERATE_RET tkl_wifi_set_work_mode(const WF_WK_MD_E mode)
{
    int ret = 0;

    Report("[TKL_WIFI] set_work_mode req=%s\r\n", _work_mode_to_str(mode));

    switch (mode) {
        case WWM_STATION:
            if (g_ap_role_up == TRUE) {
                ret = _wifi_role_down_ap();
                if (ret != 0) {
                    return OPRT_COM_ERROR;
                }
            }

            ret = _wifi_role_up_sta();
            if (ret != 0) {
                return OPRT_COM_ERROR;
            }
            g_requested_mode = mode;
            return OPRT_OK;

        case WWM_SOFTAP:
            Report("[TKL_WIFI] set_work_mode softap path sta_up=%d ap_up=%d\r\n",
                   g_sta_role_up, g_ap_role_up);
            if (g_ap_role_up == TRUE && g_sta_role_up == TRUE) {
                ret = _wifi_role_down_sta();
                if (ret != 0) {
                    Report("[TKL_WIFI] set_work_mode softap down sta fail ret=%d\r\n", ret);
                    return OPRT_COM_ERROR;
                }
            }
            g_requested_mode = mode;
            Report("[TKL_WIFI] set_work_mode softap done req=%s current=%s\r\n",
                   _work_mode_to_str(g_requested_mode), _work_mode_to_str(_get_current_work_mode()));
            return OPRT_OK;

        case WWM_STATIONAP:
            ret = _wifi_role_up_sta();
            if (ret != 0) {
                return OPRT_COM_ERROR;
            }
            g_requested_mode = mode;
            return OPRT_OK;

        default:
            return OPRT_NOT_SUPPORTED;
    }
}

/* --- Added Functionality: Status --- */
OPERATE_RET tkl_wifi_station_get_status(WF_STATION_STAT_E *stat)
{
    if (stat == NULL) return OPRT_INVALID_PARM;
    if (_wifi_station_has_ipv4()) {
        g_wifi_status = WSS_GOT_IP;
    }
    *stat = g_wifi_status;
    return OPRT_OK;
}
void tkl_wifi_handle_event(WlanEvent_t *pWlanEvent)
{
    TiWlanEventHandler(pWlanEvent);
}
/* --- Added Functionality: RSSI --- */
OPERATE_RET tkl_wifi_station_get_conn_ap_rssi(int8_t *rssi)
{
    int ret;
    WlanBeaconRssi_t beaconRssi;

    if (rssi == NULL) return OPRT_INVALID_PARM;
    
    /* Only valid if connected */
    if (g_wifi_status != WSS_CONN_SUCCESS && g_wifi_status != WSS_GOT_IP) {
        return OPRT_COM_ERROR;
    }

    beaconRssi.role_id = WLAN_ROLE_STA;
    ret = Wlan_Get(WLAN_GET_RSSI, &beaconRssi);

    if (ret == 0) {
        /* Use avg data RSSI or beacon RSSI */
        *rssi = beaconRssi.rssi_data;
        return OPRT_OK;
    }

    return OPRT_COM_ERROR;
}

/* ------------------------------------------------------------------------- */
/* STUB Functions (Required by tkl_wifi.h but not implemented in this port)  */
/* ------------------------------------------------------------------------- */

OPERATE_RET tkl_wifi_start_ap(const WF_AP_CFG_IF_S *cfg)
{
    int ret;
    void *apif;
    RoleUpApCmd_t apParams = {0};
    uint8_t sec_type;

    if (cfg == NULL || cfg->s_len == 0 || cfg->s_len > WIFI_SSID_LEN) {
        Report("[TKL_WIFI] start_ap invalid cfg ret=%d\r\n", OPRT_INVALID_PARM);
        return OPRT_INVALID_PARM;
    }

    Report("[TKL_WIFI] start_ap enter requested_ssid=%s mode=%s sta_up=%d ap_up=%d chan=%u sec=%u max=%u hidden_req=%u\r\n",
           cfg->ssid, _work_mode_to_str(g_requested_mode), g_sta_role_up, g_ap_role_up,
           cfg->chan, cfg->md, cfg->max_conn, cfg->ssid_hidden);

    if (g_requested_mode == WWM_SOFTAP && g_sta_role_up == TRUE) {
        ret = _wifi_role_down_sta();
        if (ret != 0) {
            Report("[TKL_WIFI] start_ap down_sta fail ret=%d\r\n", ret);
            return OPRT_COM_ERROR;
        }
    }

    if (g_ap_role_up == TRUE) {
        ret = _wifi_role_down_ap();
        if (ret != 0) {
            Report("[TKL_WIFI] start_ap down_ap fail ret=%d\r\n", ret);
            return OPRT_COM_ERROR;
        }
    }

    sec_type = _tuya_ap_sec_to_ti(cfg->md);

    apParams.ssid = (int8_t *)cfg->ssid;
    /*
     * Keep Tuya onboarding AP visible. Some Tuya flows request a hidden SSID,
     * but on this platform that makes the network effectively undiscoverable
     * for manual bring-up and SmartLife onboarding.
     */
    apParams.hidden = FALSE;
    apParams.channel = (cfg->chan != 0U) ? cfg->chan : 6U;
    apParams.tx_pow = 0;
    apParams.sta_limit = (cfg->max_conn != 0U) ? cfg->max_conn : 1U;
    apParams.secParams.Type = sec_type;
    apParams.secParams.Key = (cfg->p_len > 0U) ? (int8_t *)cfg->passwd : NULL;
    apParams.secParams.KeyLen = cfg->p_len;
    apParams.countryDomain[0] = '0';
    apParams.countryDomain[1] = '0';
    apParams.countryDomain[2] = '\0';
    apParams.sae_pwe = 0;
    apParams.p2p_aGO = FALSE;
    apParams.wpsDisabled = FALSE;
    _fill_default_wps_params(&apParams.wpsParams);
    apParams.p2pDeviceEnabled = FALSE;

    Report("[TKL_WIFI] start_ap roleup ssid=%s chan=%u sec=%u keylen=%u hidden=%u country=%c%c%c\r\n",
           apParams.ssid, apParams.channel, apParams.secParams.Type, apParams.secParams.KeyLen,
           apParams.hidden, apParams.countryDomain[0], apParams.countryDomain[1], apParams.countryDomain[2]);

    network_stack_add_if_ap();
    os_sleep(1, 0);

    ret = Wlan_RoleUp(WLAN_ROLE_AP, &apParams, OSI_WAIT_FOR_SECOND * 10);
    if (ret != 0) {
        network_stack_remove_if_ap();
        Report("[TKL_WIFI] start_ap roleup fail ret=%d\r\n", ret);
        return OPRT_COM_ERROR;
    }

    apif = network_get_ap_if();
    if (apif != NULL) {
        _wifi_apply_ap_ip_config(cfg);
        network_set_up(apif);
        Report("[TKL_WIFI] start_ap netif up apif=%p\r\n", apif);
    } else {
        Report("[TKL_WIFI] start_ap apif is null\r\n");
    }

    g_ap_role_up = TRUE;
    g_requested_mode = (g_requested_mode == WWM_STATIONAP || g_sta_role_up == TRUE) ? WWM_STATIONAP : WWM_SOFTAP;

    Report("[TKL_WIFI] start_ap ok ssid=%s\r\n", apParams.ssid);
    return OPRT_OK;
}

OPERATE_RET tkl_wifi_stop_ap(void)
{
    int ret;

    if (g_ap_role_up == FALSE) {
        printf("[TKL_WIFI] stop_ap ok\n\r");
        return OPRT_OK;
    }

    ret = _wifi_role_down_ap();
    if (ret != 0) {
        printf("[TKL_WIFI] stop_ap fail ret=%d\n\r", ret);
        return OPRT_COM_ERROR;
    }

    if (g_sta_role_up == TRUE) {
        g_requested_mode = WWM_STATION;
    } else {
        g_requested_mode = WWM_POWERDOWN;
    }

    printf("[TKL_WIFI] stop_ap ok\n\r");
    return OPRT_OK;
}

OPERATE_RET tkl_wifi_set_cur_channel(const uint8_t chan)
{
    return OPRT_NOT_SUPPORTED;
}

OPERATE_RET tkl_wifi_get_cur_channel(uint8_t *chan)
{
    return OPRT_NOT_SUPPORTED;
}

OPERATE_RET tkl_wifi_set_sniffer(const BOOL_T en, const SNIFFER_CALLBACK cb)
{
    return OPRT_NOT_SUPPORTED;
}

OPERATE_RET tkl_wifi_get_ip(const WF_IF_E wf, NW_IP_S *ip)
{
    struct netif *nif = netif_default; 
    uint32_t ap_ip_addr = 0;
    uint32_t ap_netmask = 0;
    uint32_t ap_gw = 0;
    uint32_t dhcp = 0;
    ip4_addr_t ap_ip = {0};
    ip4_addr_t ap_mask = {0};
    ip4_addr_t ap_gateway = {0};

    if (ip == NULL) {
        return OPRT_INVALID_PARM;
    }

    if (wf == WF_AP) {
        if (network_stack_get_if_ip(WLAN_ROLE_AP, &ap_ip_addr, &ap_netmask, &ap_gw, &dhcp) != 0) {
            return OPRT_COM_ERROR;
        }

        ap_ip.addr = ap_ip_addr;
        ap_mask.addr = ap_netmask;
        ap_gateway.addr = ap_gw;

        memset(ip, 0, sizeof(*ip));
        snprintf(ip->ip, sizeof(ip->ip), "%s", ip4addr_ntoa(&ap_ip));
        snprintf(ip->mask, sizeof(ip->mask), "%s", ip4addr_ntoa(&ap_mask));
        snprintf(ip->gw, sizeof(ip->gw), "%s", ip4addr_ntoa(&ap_gateway));
        return OPRT_OK;
    }

    if (wf != WF_STATION) {
        return OPRT_NOT_SUPPORTED;
    }

    if (nif == NULL || !netif_is_up(nif)) {
        return OPRT_COM_ERROR;
    }

    const ip4_addr_t *ip_addr = netif_ip4_addr(nif);
    if (ip4_addr_isany_val(*ip_addr)) {
        return OPRT_COM_ERROR;
    }

    memset(ip, 0, sizeof(*ip));

    // IP Address
    snprintf(ip->ip, sizeof(ip->ip), "%s", ip4addr_ntoa(ip_addr));

    // Subnet Mask
    const ip4_addr_t *netmask = netif_ip4_netmask(nif);
    snprintf(ip->mask, sizeof(ip->mask), "%s", ip4addr_ntoa(netmask));

    // Gateway
    const ip4_addr_t *gw = netif_ip4_gw(nif);
    snprintf(ip->gw, sizeof(ip->gw), "%s", ip4addr_ntoa(gw));

    if (g_wifi_status == WSS_CONN_SUCCESS) {
        g_wifi_status = WSS_GOT_IP;
    }

    return OPRT_OK;
}

OPERATE_RET tkl_wifi_get_ipv6(const WF_IF_E wf, NW_IP_TYPE type, NW_IP_S *ip)
{
    return OPRT_NOT_SUPPORTED;
}

OPERATE_RET tkl_wifi_set_ip(const WF_IF_E wf, NW_IP_S *ip)
{
    return OPRT_NOT_SUPPORTED;
}

OPERATE_RET tkl_wifi_set_mac(const WF_IF_E wf, const NW_MAC_S *mac)
{
    return OPRT_NOT_SUPPORTED;
}

OPERATE_RET tkl_wifi_get_connected_ap_info(FAST_WF_CONNECTED_AP_INFO_T **fast_ap_info)
{
    return OPRT_NOT_SUPPORTED;
}

OPERATE_RET tkl_wifi_get_bssid(uint8_t *mac)
{
    return OPRT_NOT_SUPPORTED;
}

OPERATE_RET tkl_wifi_set_country_code(const COUNTRY_CODE_E ccode)
{
    _set_country_domain(ccode);
    printf("[TKL_WIFI] country set=%c%c%c\n\r", g_country_domain[0], g_country_domain[1], g_country_domain[2]);
    return OPRT_OK;
}

OPERATE_RET tkl_wifi_set_rf_calibrated(void)
{
    return OPRT_NOT_SUPPORTED;
}

OPERATE_RET tkl_wifi_set_lp_mode(const BOOL_T enable, const uint8_t dtim)
{
    return OPRT_NOT_SUPPORTED;
}

OPERATE_RET tkl_wifi_station_fast_connect(const FAST_WF_CONNECTED_AP_INFO_T *fast_ap_info)
{
    return OPRT_NOT_SUPPORTED;
}

OPERATE_RET tkl_wifi_send_mgnt(const uint8_t *buf, const uint32_t len)
{
    return OPRT_NOT_SUPPORTED;
}

OPERATE_RET tkl_wifi_register_recv_mgnt_callback(const BOOL_T enable, const WIFI_REV_MGNT_CB recv_cb)
{
    return OPRT_NOT_SUPPORTED;
}

OPERATE_RET tkl_wifi_ioctl(WF_IOCTL_CMD_E cmd, void *args)
{
    return OPRT_NOT_SUPPORTED;
}
