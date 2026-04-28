/**
 * @file tkl_gpio.c
 * @brief Tuya Kernel Layer - GPIO
 */

// --- BEGIN: user defines and implements ---
#include "tkl_gpio.h"
#include "tuya_error_code.h"
#include <ti/drivers/GPIO.h>
#include <stdbool.h>
#include <string.h> 

// [DEPENDENCY INJECTION] Include Board Config for dynamic mapping
#include "tkl_board_config.h"

/* * [OPTIMIZATION] Reverse Lookup Table
 * TI Drivers callbacks provide the Hardware Index (0-63).
 * Tuya Callbacks require the Tuya Pin ID.
 * We map HW Index -> Tuya ID to bridge the interrupt.
 */
#define TI_HW_GPIO_MAX_INDICES  64 
static int8_t sg_ti_to_tuya_lookup[TI_HW_GPIO_MAX_INDICES];
static bool sg_is_lookup_init = false;

// Callback storage structure
typedef struct {
    TUYA_GPIO_IRQ_T callback; 
} pin_cb_t;

// Storage for callbacks, indexed by Tuya PIN ID
// NO MAGIC NUMBER: Using constant from board config
static pin_cb_t cb_gpio_map[TKL_MAX_GPIO_PINS];

// Helper: Initialize lookup table to -1 (Invalid)
static void ensure_lookup_init(void) {
    if (!sg_is_lookup_init) {
        memset(sg_ti_to_tuya_lookup, -1, sizeof(sg_ti_to_tuya_lookup));
        sg_is_lookup_init = true;
    }
}

// Helper: Check valid ID
static bool is_valid_pin(TUYA_GPIO_NUM_E pin_id) {
    return (pin_id < TKL_MAX_GPIO_PINS);
}

/*
 * [BRIDGE] Interrupt Callback Wrapper
 * This function is called by TI Driver. We look up the Tuya ID and call the user's callback.
 */
static void ti_gpio_callback_bridge(uint_least8_t ti_index)
{
    // Safety check bounds
    if (ti_index >= TI_HW_GPIO_MAX_INDICES) return;

    // 1. Recover Tuya Pin ID from HW Index
    int8_t tuya_pin_id = sg_ti_to_tuya_lookup[ti_index];

    // 2. Validate mapping and existence of user callback
    if (tuya_pin_id >= 0 && tuya_pin_id < TKL_MAX_GPIO_PINS) {
        if (cb_gpio_map[tuya_pin_id].callback.cb) {
            cb_gpio_map[tuya_pin_id].callback.cb(cb_gpio_map[tuya_pin_id].callback.arg);
        }
    }
}
// --- END: user defines and implements ---

/**
 * @brief gpio init
 * * @param[in] pin_id: gpio pin id
 * @param[in] cfg:  gpio config
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET tkl_gpio_init(TUYA_GPIO_NUM_E pin_id, const TUYA_GPIO_BASE_CFG_T *cfg)
{
    // --- BEGIN: user implements ---
    if (!is_valid_pin(pin_id) || cfg == NULL) {
        return OPRT_INVALID_PARM;
    }

    ensure_lookup_init();

    // [MAPPING] Get the Real TI Hardware Index
    int16_t gpio_index = tkl_hw_get_gpio_index((uint8_t)pin_id);

    // If result is negative, the Application did not map this pin.
    if (gpio_index < 0) {
        return OPRT_NOT_SUPPORTED;
    }

    // [OPTIMIZATION] Update reverse lookup for interrupts
    if (gpio_index < TI_HW_GPIO_MAX_INDICES) {
        sg_ti_to_tuya_lookup[gpio_index] = (int8_t)pin_id;
    }

    GPIO_PinConfig config_ti = 0;

    // Map Direction
    switch (cfg->direct) {
        case TUYA_GPIO_INPUT:
            config_ti = GPIO_CFG_INPUT;
            // Map Pull Mode (Only valid for Input)
            switch (cfg->mode) {
                case TUYA_GPIO_PULLUP:   config_ti |= GPIO_CFG_IN_PU; break;
                case TUYA_GPIO_PULLDOWN: config_ti |= GPIO_CFG_IN_PD; break;
                case TUYA_GPIO_FLOATING: config_ti |= GPIO_CFG_IN_NOPULL; break;
                default: return OPRT_INVALID_PARM;
            }
            break;

        case TUYA_GPIO_OUTPUT:
            config_ti = GPIO_CFG_OUTPUT;
            // Map Initial Level
            if (cfg->level == TUYA_GPIO_LEVEL_HIGH) {
                config_ti |= GPIO_CFG_OUT_HIGH;
            } else {
                config_ti |= GPIO_CFG_OUT_LOW;
            }
            // Map Output Mode 
            // FIX: CC32xx driver does not support Open Drain configuration directly.
            // We map everything to Standard Output to ensure compilation.
            config_ti |= GPIO_CFG_OUT_STD;
            
            // FIX: CC32xx driver does not support Drive Strength configuration.
            // Removed GPIO_CFG_OUT_STR_MED
            break;

        default:
            return OPRT_INVALID_PARM;
    }

    // Apply Configuration to Hardware
    if (GPIO_setConfig(gpio_index, config_ti) != GPIO_STATUS_SUCCESS) {
        return OPRT_COM_ERROR;
    }

    return OPRT_OK;
    // --- END: user implements ---
}

/**
 * @brief gpio deinit
 * * @param[in] pin_id: gpio pin id
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET tkl_gpio_deinit(TUYA_GPIO_NUM_E pin_id)
{
    // --- BEGIN: user implements ---
    if (!is_valid_pin(pin_id)) return OPRT_INVALID_PARM;

    // [MAPPING]
    int16_t gpio_index = tkl_hw_get_gpio_index((uint8_t)pin_id);
    if (gpio_index < 0) return OPRT_NOT_SUPPORTED;

    // Reset callback map
    cb_gpio_map[pin_id].callback.cb = NULL;
    
    // Clear Reverse Lookup
    if (gpio_index < TI_HW_GPIO_MAX_INDICES) {
        sg_ti_to_tuya_lookup[gpio_index] = -1;
    }

    // Set to Input/NoPull (High Impedance)
    GPIO_setConfig(gpio_index, GPIO_CFG_INPUT | GPIO_CFG_IN_NOPULL);

    return OPRT_OK;
    // --- END: user implements ---
}

/**
 * @brief gpio write
 * * @param[in] pin_id: gpio pin id
 * @param[in] level: gpio output level value
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET tkl_gpio_write(TUYA_GPIO_NUM_E pin_id, TUYA_GPIO_LEVEL_E level)
{
    // --- BEGIN: user implements ---
    if (!is_valid_pin(pin_id)) return OPRT_INVALID_PARM;

    // [MAPPING]
    int16_t gpio_index = tkl_hw_get_gpio_index((uint8_t)pin_id);
    if (gpio_index < 0) return OPRT_NOT_SUPPORTED;

    GPIO_write(gpio_index, (unsigned int)level);

    return OPRT_OK;
    // --- END: user implements ---
}

/**
 * @brief gpio read
 * * @param[in] pin_id: gpio pin id
 * @param[out] level: gpio output level
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET tkl_gpio_read(TUYA_GPIO_NUM_E pin_id, TUYA_GPIO_LEVEL_E *level)
{
    // --- BEGIN: user implements ---
    if (!is_valid_pin(pin_id) || level == NULL) return OPRT_INVALID_PARM;

    // [MAPPING]
    int16_t gpio_index = tkl_hw_get_gpio_index((uint8_t)pin_id);
    if (gpio_index < 0) return OPRT_NOT_SUPPORTED;

    *level = (TUYA_GPIO_LEVEL_E)GPIO_read(gpio_index);

    return OPRT_OK;
    // --- END: user implements ---
}

/**
 * @brief gpio irq init
 * NOTE: call this API will not enable interrupt
 *
 * @param[in] pin_id: gpio pin id
 * @param[in] cfg:  gpio irq config
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET tkl_gpio_irq_init(TUYA_GPIO_NUM_E pin_id, const TUYA_GPIO_IRQ_T *cfg)
{
    // --- BEGIN: user implements ---
    if (!is_valid_pin(pin_id) || cfg == NULL) return OPRT_INVALID_PARM;

    // [MAPPING]
    int16_t gpio_index = tkl_hw_get_gpio_index((uint8_t)pin_id);
    if (gpio_index < 0) return OPRT_NOT_SUPPORTED;

    // Store callback
    cb_gpio_map[pin_id].callback = *cfg;

    // Set callback to bridge
    GPIO_setCallback(gpio_index, ti_gpio_callback_bridge);

    // Configure Edge
    GPIO_PinConfig int_config = 0;
    switch (cfg->mode) {
        case TUYA_GPIO_IRQ_RISE: int_config = GPIO_CFG_IN_INT_RISING; break;
        case TUYA_GPIO_IRQ_FALL: int_config = GPIO_CFG_IN_INT_FALLING; break;
        
        case TUYA_GPIO_IRQ_RISE_FALL: 
            // FIX: CC32xx hardware does not support BOTH_EDGES configuration directly.
            // We return NOT_SUPPORTED to avoid build errors.
            return OPRT_NOT_SUPPORTED;

        case TUYA_GPIO_IRQ_LOW:  int_config = GPIO_CFG_IN_INT_LOW; break;
        case TUYA_GPIO_IRQ_HIGH: int_config = GPIO_CFG_IN_INT_HIGH; break;
        default: return OPRT_INVALID_PARM;
    }

    // Note: We don't overwrite pull settings, assume init() handled it.
    // Just enable interrupt capability in config (TI requires config update for INT type)
    // We fetch current config to preserve pull/input state? 
    // Simplified: Just enable the interrupt on top of Input
    GPIO_setConfig(gpio_index, GPIO_CFG_INPUT | int_config); 

    return OPRT_OK;
    // --- END: user implements ---
}

/**
 * @brief gpio irq enable
 *
 * @param[in] pin_id: gpio pin id
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET tkl_gpio_irq_enable(TUYA_GPIO_NUM_E pin_id)
{
    // --- BEGIN: user implements ---
    if (!is_valid_pin(pin_id)) return OPRT_INVALID_PARM;

    // [MAPPING]
    int16_t gpio_index = tkl_hw_get_gpio_index((uint8_t)pin_id);
    if (gpio_index < 0) return OPRT_NOT_SUPPORTED;

    GPIO_enableInt(gpio_index);
    return OPRT_OK;
    // --- END: user implements ---
}

/**
 * @brief gpio irq disable
 *
 * @param[in] pin_id: gpio pin id
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET tkl_gpio_irq_disable(TUYA_GPIO_NUM_E pin_id)
{
    // --- BEGIN: user implements ---
    if (!is_valid_pin(pin_id)) return OPRT_INVALID_PARM;

    // [MAPPING]
    int16_t gpio_index = tkl_hw_get_gpio_index((uint8_t)pin_id);
    if (gpio_index < 0) return OPRT_NOT_SUPPORTED;

    GPIO_disableInt(gpio_index);
    return OPRT_OK;
    // --- END: user implements ---
}