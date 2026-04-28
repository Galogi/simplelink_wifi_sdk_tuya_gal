/**
 * @file tkl_wakeup.c
 * @brief Tuya TKL wakeup adapter for TI (CC35xx/WFF3) using TI GPIO driver
 *
 * Implements TUYA_WAKEUP_SOURCE_GPIO via TI GPIO interrupts.
 * TIMER/RTC sources: not supported here (can be added later using ClockP/Power).
 */

// --- BEGIN: user defines and implements ---
#include "tkl_wakeup.h"
#include "tuya_error_code.h"

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include <ti/drivers/GPIO.h>

// --- END: user defines and implements ---

/* ---------------------------
 * TUYA GPIO -> TI CONFIG mapping (Option 1)
 * You define CONFIG_TUYA_GPIO_0.. in SysConfig.
 * --------------------------- */
static const int tuya_gpio_to_ti_index[] = {
#if defined(CONFIG_TUYA_GPIO_0)
    CONFIG_TUYA_GPIO_0,
#else
    -1,
#endif
#if defined(CONFIG_TUYA_GPIO_1)
    CONFIG_TUYA_GPIO_1,
#else
    -1,
#endif
#if defined(CONFIG_TUYA_GPIO_2)
    CONFIG_TUYA_GPIO_2,
#else
    -1,
#endif
#if defined(CONFIG_TUYA_GPIO_3)
    CONFIG_TUYA_GPIO_3,
#else
    -1,
#endif
#if defined(CONFIG_TUYA_GPIO_4)
    CONFIG_TUYA_GPIO_4,
#else
    -1,
#endif
#if defined(CONFIG_TUYA_GPIO_5)
    CONFIG_TUYA_GPIO_5,
#else
    -1,
#endif
#if defined(CONFIG_TUYA_GPIO_6)
    CONFIG_TUYA_GPIO_6,
#else
    -1,
#endif
#if defined(CONFIG_TUYA_GPIO_7)
    CONFIG_TUYA_GPIO_7,
#else
    -1,
#endif
#if defined(CONFIG_TUYA_GPIO_8)
    CONFIG_TUYA_GPIO_8,
#else
    -1,
#endif
#if defined(CONFIG_TUYA_GPIO_9)
    CONFIG_TUYA_GPIO_9,
#else
    -1,
#endif
#if defined(CONFIG_TUYA_GPIO_10)
    CONFIG_TUYA_GPIO_10,
#else
    -1,
#endif
#if defined(CONFIG_TUYA_GPIO_11)
    CONFIG_TUYA_GPIO_11,
#else
    -1,
#endif
#if defined(CONFIG_TUYA_GPIO_12)
    CONFIG_TUYA_GPIO_12,
#else
    -1,
#endif
#if defined(CONFIG_TUYA_GPIO_13)
    CONFIG_TUYA_GPIO_13,
#else
    -1,
#endif
#if defined(CONFIG_TUYA_GPIO_14)
    CONFIG_TUYA_GPIO_14,
#else
    -1,
#endif
#if defined(CONFIG_TUYA_GPIO_15)
    CONFIG_TUYA_GPIO_15,
#else
    -1,
#endif
};

static bool s_gpio_inited = false;

/* Keep active wake source (GPIO) so clear() can undo */
typedef struct {
    bool active;
    TUYA_GPIO_NUM_E tuya_gpio;
    int ti_index;
    TUYA_GPIO_WAKE_TYPE_E wake_type;
} wake_gpio_ctx_t;

static wake_gpio_ctx_t g_wake_gpio = {0};

static void ti_wakeup_gpio_isr(uint_least8_t index);

static inline int tuya_gpio_to_ti(TUYA_GPIO_NUM_E n)
{
    if (n < TUYA_GPIO_NUM_0 || n >= TUYA_GPIO_NUM_MAX) return -1;

    const int max = (int)(sizeof(tuya_gpio_to_ti_index) / sizeof(tuya_gpio_to_ti_index[0]));
    if ((int)n >= max) return -1;

    return tuya_gpio_to_ti_index[(int)n];
}

static inline uint32_t map_tuya_wake_to_ti_int(TUYA_GPIO_WAKE_TYPE_E t)
{
    /* TI GPIO is mostly edge interrupts in driver land */
    switch (t) {
        case TUYA_GPIO_WAKEUP_RISE: return GPIO_CFG_IN_INT_RISING;
        case TUYA_GPIO_WAKEUP_FALL: return GPIO_CFG_IN_INT_FALLING;
        case TUYA_GPIO_WAKEUP_HIGH: return GPIO_CFG_IN_INT_RISING;
        case TUYA_GPIO_WAKEUP_LOW:  return GPIO_CFG_IN_INT_FALLING;
        default:                    return GPIO_CFG_IN_INT_FALLING;
    }
}

static OPERATE_RET wake_gpio_enable(int ti_index, TUYA_GPIO_WAKE_TYPE_E wake_type)
{
    if (ti_index < 0) return OPRT_INVALID_PARM;

    if (!s_gpio_inited) {
        GPIO_init();
        s_gpio_inited = true;
    }

    /* input + pull-up, change to PU/PD per your hardware */
    uint32_t cfg = GPIO_CFG_IN_PU | map_tuya_wake_to_ti_int(wake_type);

    GPIO_setConfig((uint_least8_t)ti_index, cfg);
    GPIO_setCallback((uint_least8_t)ti_index, ti_wakeup_gpio_isr);
    GPIO_enableInt((uint_least8_t)ti_index);

    return OPRT_OK;
}

static OPERATE_RET wake_gpio_disable(int ti_index)
{
    if (ti_index < 0) return OPRT_INVALID_PARM;
    if (!s_gpio_inited) return OPRT_OK;

    GPIO_disableInt((uint_least8_t)ti_index);
    GPIO_setCallback((uint_least8_t)ti_index, NULL);
    return OPRT_OK;
}

static void ti_wakeup_gpio_isr(uint_least8_t index)
{
    /* Keep ISR minimal. If you later want a hook – add it here. */
    (void)index;
}

/* ============ Tuya APIs ============ */

OPERATE_RET tkl_wakeup_source_set(const TUYA_WAKEUP_SOURCE_BASE_CFG_T *param)
{
    if (param == NULL) return OPRT_INVALID_PARM;

    if (param->source == TUYA_WAKEUP_SOURCE_GPIO) {

        const TUYA_WAKEUP_SOURCE_GPIO_T *g = &param->wakeup_para.gpio_param;

        int ti_index = tuya_gpio_to_ti(g->gpio_num);
        if (ti_index < 0) return OPRT_NOT_SUPPORTED;

        /* Replace existing wake GPIO if set */
        if (g_wake_gpio.active) {
            (void)wake_gpio_disable(g_wake_gpio.ti_index);
            memset(&g_wake_gpio, 0, sizeof(g_wake_gpio));
        }

        OPERATE_RET rc = wake_gpio_enable(ti_index, g->level);
        if (rc != OPRT_OK) return rc;

        g_wake_gpio.active = true;
        g_wake_gpio.tuya_gpio = g->gpio_num;
        g_wake_gpio.ti_index = ti_index;
        g_wake_gpio.wake_type = g->level;

        return OPRT_OK;
    }

    if (param->source == TUYA_WAKEUP_SOURCE_TIMER) {
        return OPRT_NOT_SUPPORTED;
    }

    if (param->source == TUYA_WAKEUP_SOURCE_RTC) {
        return OPRT_NOT_SUPPORTED;
    }

    return OPRT_NOT_SUPPORTED;
}

OPERATE_RET tkl_wakeup_source_clear(const TUYA_WAKEUP_SOURCE_BASE_CFG_T *param)
{
    /* NULL means "clear everything we configured" */
    if (param == NULL) {
        if (g_wake_gpio.active) {
            (void)wake_gpio_disable(g_wake_gpio.ti_index);
            memset(&g_wake_gpio, 0, sizeof(g_wake_gpio));
        }
        return OPRT_OK;
    }

    if (param->source == TUYA_WAKEUP_SOURCE_GPIO) {
        if (g_wake_gpio.active) {
            /* optionally clear only if same gpio:
               if (g_wake_gpio.tuya_gpio != param->wakeup_para.gpio_param.gpio_num) return OPRT_OK;
            */
            (void)wake_gpio_disable(g_wake_gpio.ti_index);
            memset(&g_wake_gpio, 0, sizeof(g_wake_gpio));
        }
        return OPRT_OK;
    }

    return OPRT_NOT_SUPPORTED;
}