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
#include <string.h>
#include <stdio.h>

/* --- LwIP Includes for IP Address Handling --- */
#include <lwip/netif.h>
#include <lwip/ip_addr.h>

/* --- TI SDK Wi-Fi Host Driver --- */
#include <ti/drivers/net/wifi/wifi_host_driver/inc_adapt/wlan_if.h>

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

/* New: Track current connection status locally */
static WF_STATION_STAT_E g_wifi_status = WSS_IDLE;

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

/* --- Core TKL Functions --- 

OPERATE_RET tkl_wifi_init(WIFI_EVENT_CB cb)
{
    int ret;

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

    _wifi_trace("before Wlan_Start");
    ret = Wlan_Start(TiWlanEventHandler);
    _wifi_trace("after Wlan_Start");
    if (ret != 0) return OPRT_COM_ERROR;

    RoleUpStaCmd_t staParams = {0};
    staParams.countryDomain[0] = 'E';
    staParams.countryDomain[1] = 'U';
    staParams.countryDomain[2] = 'I'; 
    staParams.wpsDisabled = TRUE; 
    staParams.p2pDeviceEnabled = FALSE;

    _wifi_trace("before Wlan_RoleUp");
    ret = Wlan_RoleUp(WLAN_ROLE_STA, &staParams, 0);
    _wifi_trace("after Wlan_RoleUp");
    if (ret != 0) return OPRT_COM_ERROR;

    g_wifi_status = WSS_IDLE;
    g_wifi_initialized = TRUE;
    _wifi_trace("exit tkl_wifi_init");
    return OPRT_OK;
}*/



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
    int ret;

    if (mac == NULL) return OPRT_INVALID_PARM;
    if (wf != WF_STATION) return OPRT_NOT_SUPPORTED;

    macParam.roleType = WLAN_ROLE_STA;
    ret = Wlan_Get(WLAN_GET_MACADDRESS, &macParam);

    if (ret == 0) {
        memcpy(mac->mac, macParam.pMacAddress, 6);
        return OPRT_OK;
    }
    return OPRT_COM_ERROR;
}

OPERATE_RET tkl_wifi_get_work_mode(WF_WK_MD_E *mode)
{
    *mode = WWM_STATION;
    return OPRT_OK;
}

OPERATE_RET tkl_wifi_set_work_mode(const WF_WK_MD_E mode)
{
    if (mode == WWM_STATION) return OPRT_OK;
    return OPRT_NOT_SUPPORTED;
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
    return OPRT_NOT_SUPPORTED;
}

OPERATE_RET tkl_wifi_stop_ap(void)
{
    return OPRT_NOT_SUPPORTED;
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

    if (ip == NULL) {
        return OPRT_INVALID_PARM;
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
    return OPRT_NOT_SUPPORTED;
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
