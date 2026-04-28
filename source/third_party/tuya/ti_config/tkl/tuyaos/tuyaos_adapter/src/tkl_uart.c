/**
 * @file tkl_uart.c
 * @brief Tuya Kernel Layer - Generic UART
 */

// --- BEGIN: user defines and implements ---
#include "tkl_uart.h"
#include "tuya_error_code.h"
#include <ti/drivers/UART2.h>

// [DEPENDENCY INJECTION] Include Board Config
#include "tkl_board_config.h"

// Support up to TKL_MAX_UART_PORTS UARTs
// NO MAGIC NUMBER: Using constant from board config
#define MAX_UART_PORTS TKL_MAX_UART_PORTS

// Array to store handles for multiple ports
static UART2_Handle g_uart_handles[MAX_UART_PORTS] = {NULL};

// Helper to check port validity
static bool is_valid_port(TUYA_UART_NUM_E port_id) {
    return (port_id < MAX_UART_PORTS);
}
// --- END: user defines and implements ---

/**
 * @brief uart init
 * * @param[in] port_id: uart port id, id index starts at 0
 * @param[in] cfg: uart config
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET tkl_uart_init(TUYA_UART_NUM_E port_id, TUYA_UART_BASE_CFG_T *cfg)
{
    // --- BEGIN: user implements ---
    if (!is_valid_port(port_id) || cfg == NULL) {
        return OPRT_INVALID_PARM;
    }

    // [MAPPING] Look up TI index dynamically
    int16_t ti_uart_index = tkl_hw_get_uart_index((uint8_t)port_id);

    // If result is negative, the Application did not map this UART port.
    if (ti_uart_index < 0) {
        return OPRT_NOT_SUPPORTED;
    }

    // Close existing if open
    if (g_uart_handles[port_id] != NULL) {
        UART2_close(g_uart_handles[port_id]);
        g_uart_handles[port_id] = NULL;
    }

    UART2_Params params;
    UART2_Params_init(&params);

    params.baudRate = cfg->baudrate;

    // Parity
    switch (cfg->parity) {
        case TUYA_UART_PARITY_TYPE_EVEN: params.parityType = UART2_Parity_EVEN; break;
        case TUYA_UART_PARITY_TYPE_ODD:  params.parityType = UART2_Parity_ODD; break;
        case TUYA_UART_PARITY_TYPE_NONE: 
        default:                         params.parityType = UART2_Parity_NONE; break;
    }

    // Stop Bits
    if (cfg->stopbits == TUYA_UART_STOP_LEN_2BIT) {
        params.stopBits = UART2_StopBits_2;
    } else {
        params.stopBits = UART2_StopBits_1;
    }

    // Data Bits
    if (cfg->databits == TUYA_UART_DATA_LEN_7BIT) {
        params.dataLength = UART2_DataLen_7;
    } else {
        params.dataLength = UART2_DataLen_8;
    }

    // Important: Use BLOCKING for now. 
    // If you need Interrupts later (tkl_uart_rx_irq_cb_reg), 
    // you must change this to UART2_Mode_CALLBACK and store the callbacks.
    params.readMode = UART2_Mode_BLOCKING;
    params.writeMode = UART2_Mode_BLOCKING;
    
    // Open using Mapped Index
    g_uart_handles[port_id] = UART2_open(ti_uart_index, &params);

    if (g_uart_handles[port_id] == NULL) {
        return OPRT_COM_ERROR;
    }

    return OPRT_OK;
    // --- END: user implements ---
}

/**
 * @brief uart deinit
 * * @param[in] port_id: uart port id, id index starts at 0
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET tkl_uart_deinit(TUYA_UART_NUM_E port_id)
{
    // --- BEGIN: user implements ---
    if (!is_valid_port(port_id)) {
        return OPRT_INVALID_PARM;
    }

    if (g_uart_handles[port_id] != NULL) {
        UART2_close(g_uart_handles[port_id]);
        g_uart_handles[port_id] = NULL;
    }
    return OPRT_OK;
    // --- END: user implements ---
}

/**
 * @brief uart write data
 * * @param[in] port_id: uart port id, id index starts at 0
 * @param[in] data: write buff
 * @param[in] len:  buff len
 *
 * @return return > 0: number of data written; return <= 0: write errror
 */
int tkl_uart_write(TUYA_UART_NUM_E port_id, void *buff, uint16_t len)
{
    // --- BEGIN: user implements ---
    if (!is_valid_port(port_id) || g_uart_handles[port_id] == NULL) {
        return -1;
    }

    size_t bytesWritten = 0;
    UART2_write(g_uart_handles[port_id], buff, (size_t)len, &bytesWritten);
    
    return (int)bytesWritten;
    // --- END: user implements ---
}

/**
 * @brief uart read data
 * * @param[in] port_id: uart port id, id index starts at 0
 * @param[out] data: read buff
 * @param[in] len:  buff len
 *
 * @return return >= 0: number of data read; return < 0: read errror
 */
int tkl_uart_read(TUYA_UART_NUM_E port_id, void *buff, uint16_t len)
{
    // --- BEGIN: user implements ---
    if (!is_valid_port(port_id) || g_uart_handles[port_id] == NULL) {
        return -1;
    }

    size_t bytesRead = 0;
    UART2_read(g_uart_handles[port_id], buff, (size_t)len, &bytesRead);
    
    return (int)bytesRead;
    // --- END: user implements ---
}

/**
 * @brief enable uart rx interrupt and regist interrupt callback
 * * @param[in] port_id: uart port id, id index starts at 0
 * @param[in] rx_cb: receive callback
 *
 * @return none
 */
void tkl_uart_rx_irq_cb_reg(TUYA_UART_NUM_E port_id, TUYA_UART_IRQ_CB rx_cb)
{
    // --- BEGIN: user implements ---
    // Note: Interrupt callbacks require UART2_Mode_CALLBACK. 
    // Current implementation uses BLOCKING mode for log stability.
    // Leaving empty to allow compilation.
    return;
    // --- END: user implements ---
}

/**
 * @brief regist uart tx interrupt callback
 * * @param[in] port_id: uart port id, id index starts at 0
 * @param[in] tx_cb: transmit callback
 *
 * @return none
 */
void tkl_uart_tx_irq_cb_reg(TUYA_UART_NUM_E port_id, TUYA_UART_IRQ_CB tx_cb)
{
    // --- BEGIN: user implements ---
    return;
    // --- END: user implements ---
}

/**
 * @brief set uart transmit interrupt status
 * * @param[in] port_id: uart port id, id index starts at 0
 * @param[in] enable: TRUE-enable tx interrupt, FALSE-disable tx interrupt
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET tkl_uart_set_tx_int(TUYA_UART_NUM_E port_id, BOOL_T enable)
{
    // --- BEGIN: user implements ---
    return OPRT_NOT_SUPPORTED;
    // --- END: user implements ---
}

/**
 * @brief set uart receive flowcontrol
 * * @param[in] port_id: uart port id, id index starts at 0
 * @param[in] enable: TRUE-enable rx flowcontrol, FALSE-disable rx flowcontrol
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET tkl_uart_set_rx_flowctrl(TUYA_UART_NUM_E port_id, BOOL_T enable)
{
    // --- BEGIN: user implements ---
    return OPRT_NOT_SUPPORTED;
    // --- END: user implements ---
}

/**
 * @brief wait for uart data
 * * @param[in] port_id: uart port id, id index starts at 0
 * @param[in] timeout_ms: the wait timeout, unit: ms
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET tkl_uart_wait_for_data(TUYA_UART_NUM_E port_id, int timeout_ms)
{
    // --- BEGIN: user implements ---
    return OPRT_NOT_SUPPORTED;
    // --- END: user implements ---
}

/**
 * @brief uart control
 * * @param[in] port_id: uart port id, id index starts at 0
 * @param[in] cmd: control command
 * @param[in] arg: command argument
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET tkl_uart_ioctl(TUYA_UART_NUM_E port_id, uint32_t cmd, void *arg)
{
    // --- BEGIN: user implements ---
    return OPRT_NOT_SUPPORTED;
    // --- END: user implements ---
}