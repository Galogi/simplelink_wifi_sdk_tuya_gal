/*****************************************************************************

 FILENAME:       ble_if.c

 DESCRIPTION:    implements the BLE host-controller interface

 AUTHOR:         Guy M.
 Copyright (c) 2022 Texas Instruments Inc.
 All Rights Reserved This program is the confidential and proprietary
 product of Texas Instruments Inc.  Any Unauthorized use, reproduction or
 transfer of this program is strictly prohibited.

 *****************************************************************************/
#include "ble_if.h"
#include "ble_transport.h"
#include "hci_transport.h"
#include "uart_hci.h"
#include "osi_type.h"
#include "osi_kernel.h"
#include "control_cmd_fw.h"
#ifdef CC35XX
#include <third_party/psa_crypto/include/psa/crypto.h>
#include <third_party/psa_crypto/include/psa/crypto_types.h>
#include <third_party/psa_crypto/include/psa/crypto_values.h>
#endif

/*****************************************************************************/
/* DEFINITIONS & GLOBALS                                                     */
/*****************************************************************************/
ble_if_cb_t bleIfCb;
OsiSyncObj_t bleInitEventSyncObj;
#define HCI_VE_EVENT_CODE 0xFF
#define HCI_VE_BLE_INIT_DONE 0x042A

/*****************************************************************************/
/* IMPLEMENTATION                                                            */
/*****************************************************************************/
int BleIf_OpenTransport()
{
#ifdef CC33XX
    if (HciTransport_IsInitialized(HCI_TRANSPORT_UART_BLE) == FALSE)
    {
        //Open the UART for HCI
        UartHciOpen();

        //Initialize the HCI transport
        HciTransport_Init(HCI_TRANSPORT_UART_BLE, &uartForHci);
    }

    //Register callbacks
    bleIfCb.cmd_cb = HciTransport_WriteHciPacketToCtrl;
    bleIfCb.event_cb_reg = HciTransport_HciRxCallbackbRegister;

    Report("\n\rBLE Serial transport opened\r\n");
#endif
#ifdef CC35XX
    //Note: shared transport was previously opened when WLAN started
    //It will not be opened here

    //Register callbacks
    bleIfCb.cmd_cb = BleTransport_SendCommand;
    bleIfCb.event_cb_reg = BleTransport_EventCallbackbRegister;

    Report("\n\rBLE Shared transport opened\r\n");
#endif

    return 0;
}

int BleIf_CloseTransport()
{
#ifdef CC33XX
    if (HciTransport_IsInitialized(HCI_TRANSPORT_UART_BLE) == TRUE)
    {
        //Close the UART for HCI
        UartHciClose();

        //Release the HCI transport
        HciTransport_Release(HCI_TRANSPORT_UART_BLE);
    }
#endif

    //Note: shared transport was previously opened when WLAN started
    //It will not be closed here

    //Clear registered callbacks
    bleIfCb.cmd_cb = NULL;
    bleIfCb.event_cb_reg = NULL;

    return 0;
}

int BleIf_SendCommand(uint8_t* cmd, uint16_t cmdLen)
{
    return bleIfCb.cmd_cb(cmd, cmdLen);
}

int BleIf_EventCbRegister(ble_event_cb_t cb)
{
    if (cb == NULL)
    {
        return (-1);
    }

    bleIfCb.event_cb_reg(cb);
    return 0;
}

int BleIf_VendorEventHandler(uint8_t* data, uint16_t len)
{
    uint8_t eventCode = data[1];
    uint16_t eventOpcode = (data[3] << 8) | data[4];

    if (eventCode == HCI_VE_EVENT_CODE)
    {
        switch(eventOpcode)
        {
            case HCI_VE_BLE_INIT_DONE:
                osi_SyncObjSignal(&bleInitEventSyncObj);
                return TRUE;
                break;
            default:
                break;
        }
    }

    return FALSE;
}

int BleIf_EnableBLE()
{
    int ret = 0;

    ret = osi_SyncObjCreate(&bleInitEventSyncObj);
    if(ret != 0)
    {
        Report("\n\rBleIf_EnableBLE: Failed to create sync object. error number %d", ret);
        ASSERT_GENERAL(0);
    }

#ifdef CC35XX
    ret = BleIf_SetSeed(MAX_SEED_SIZE);
    if (ret < 0)
    {
        Report("\n\rBleIf_EnableBLE: Failed to set initial seed. error number: %d", ret);
        ASSERT_GENERAL(0);
    }
#endif

    //Send BLE enable command to the controller
    ret = ctrlCmdFw_EnableBLECmd();

    if (ret < 0)
    {
        Report("\n\rBleIf_EnableBLE: Failed to enable BLE. error number: %d", ret);
        ASSERT_GENERAL(0);
    }

    //Wait for the controller to be enabled before starting host
    Report("\n\r-------------- Wait for BLE Controller");

    ret = osi_SyncObjWait(&bleInitEventSyncObj, OSI_WAIT_FOR_SECOND);
    if (OSI_OK != ret)
    {
        Report("\n\rBleIf_EnableBLE: Failed to receive BLE Init Done. error number %d", ret);
        ASSERT_GENERAL(0);
    }

    ret = osi_SyncObjDelete(&bleInitEventSyncObj);
    if (OSI_OK != ret)
    {
        Report("\n\rBleIf_EnableBLE: Failed to delete sync object. error number %d", ret);
        ASSERT_GENERAL(0);
    }

    return ret;
}

int BleIf_SetBdAddr(const unsigned char *pBdAddr)
{
    return ctrlCmdFw_SetBdAddrCmd(pBdAddr);
}

int BleIf_SetSeed(uint8_t size)
{
    int ret = -1;
    uint8_t seed[MAX_SEED_SIZE] = {0};

    //Validate seed size
    if (size > MAX_SEED_SIZE)
    {
        Report("\n\rBleIf_SetSeed: seed size exceeded");
        return ret;
    }

#ifdef CC35XX
    //Check crypto engine status
    psa_status_t status  = psa_crypto_init();
    if (status != PSA_SUCCESS) {
        Report("\n\rBleIf_SetSeed: Crypto is not initialized: %d", status);
        return ret;
    }

    //Get new seed from HSM
    status = psa_generate_random(seed, size);
    if (status != PSA_SUCCESS) {
        Report("\n\rBleIf_SetSeed: Failed to get initial seed. error number: %d", status);
        return ret;
    }
#endif

    //Send set initial seed to the controller
    return ctrlCmdFw_SetSeedCmd(seed, size);
}
