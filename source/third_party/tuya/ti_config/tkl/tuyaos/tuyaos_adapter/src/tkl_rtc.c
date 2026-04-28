/**
 * @file tkl_rtc.c
 * @brief RTC adapter implementation for Tuya TKL on TI CC35xx.
 *
 * This port does NOT use ti/drivers/dpl/Seconds.h (not available in this SDK build).
 * Instead it implements a simple epoch clock using:
 *   epoch_now = base_epoch + (ticks_now - base_ticks) converted to seconds
 *
 * Accuracy: depends on system tick period. Good enough for timestamps once time is set
 * (e.g., via NTP/cloud). No battery-backed RTC is assumed.
 */

#include "tkl_rtc.h"
#include "tuya_error_code.h"

#include <ti/drivers/dpl/ClockP.h>
#include <stdint.h>
#include <stdbool.h>

/* ------------------------------------------------------------
 * Local epoch base
 * ------------------------------------------------------------ */
static uint32_t g_rtc_base_epoch     = 0;   /* seconds */
static uint64_t g_rtc_base_ticks64   = 0;   /* ClockP ticks at base epoch */
static bool     g_rtc_time_is_set    = false;

static uint32_t ticks_to_seconds_u32(uint64_t dticks)
{
    uint32_t tick_us = ClockP_getSystemTickPeriod(); /* microseconds per tick */

    if (tick_us == 0) {
        return 0;
    }

    /* seconds = (ticks * tick_us) / 1,000,000 */
    uint64_t usec = dticks * (uint64_t)tick_us;
    return (uint32_t)(usec / 1000000ULL);
}

static uint32_t rtc_now_epoch_seconds(void)
{
    uint64_t now_ticks = ClockP_getSystemTicks64();
    uint64_t dticks    = now_ticks - g_rtc_base_ticks64;

    return g_rtc_base_epoch + ticks_to_seconds_u32(dticks);
}

/* ------------------------------------------------------------
 * Tuya TKL RTC API (common patterns)
 * ------------------------------------------------------------ */

/**
 * @brief Get current time in epoch seconds.
 *
 * @param[out] time_sec Pointer to store epoch seconds.
 */
OPERATE_RET tkl_rtc_get_time(uint32_t *time_sec)
{
    if (time_sec == NULL) {
        return OPRT_INVALID_PARM;
    }

    /* If never set, return uptime seconds (base_epoch=0) */
    *time_sec = rtc_now_epoch_seconds();
    return OPRT_OK;
}

/**
 * @brief Set current time (epoch seconds).
 *
 * @param[in] time_sec Epoch seconds.
 */
OPERATE_RET tkl_rtc_set_time(uint32_t time_sec)
{
    g_rtc_base_epoch   = time_sec;
    g_rtc_base_ticks64 = ClockP_getSystemTicks64();
    g_rtc_time_is_set  = true;
    return OPRT_OK;
}

/**
 * @brief Optional: Get milliseconds since boot (some Tuya ports require it).
 * If your header doesn't declare this, you can remove it.
 */
OPERATE_RET tkl_rtc_get_ms(uint64_t *time_ms)
{
    if (time_ms == NULL) {
        return OPRT_INVALID_PARM;
    }

    uint64_t ticks  = ClockP_getSystemTicks64();
    uint32_t tick_us = ClockP_getSystemTickPeriod();

    if (tick_us == 0) {
        *time_ms = 0;
        return OPRT_OK;
    }

    /* ms = (ticks * tick_us) / 1000 */
    uint64_t usec = ticks * (uint64_t)tick_us;
    *time_ms = usec / 1000ULL;

    return OPRT_OK;
}

/**
 * @brief Optional: check if time was set.
 * If your header doesn't declare this, you can remove it.
 */
OPERATE_RET tkl_rtc_is_time_set(bool *is_set)
{
    if (is_set == NULL) {
        return OPRT_INVALID_PARM;
    }
    *is_set = g_rtc_time_is_set;
    return OPRT_OK;
}
