/**
 * @file tkl_timer.c
 * @brief Tuya TKL timer adapter for TI (ClockP-based)
 *
 * NOTE:
 *  - TI ClockP works in system ticks (not true microseconds).
 *  - We convert microseconds <-> ticks using ClockP_getSystemTickPeriod().
 */

#include "tkl_timer.h"
#include "tuya_error_code.h"

#include <ti/drivers/dpl/ClockP.h>

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#ifndef MAX_TUYA_TIMERS
#define MAX_TUYA_TIMERS 4
#endif

typedef struct {
    bool inited;

    ClockP_Struct clock_struct;
    ClockP_Handle clock_handle;

    TUYA_TIMER_ISR_CB cb;
    void *args;

    TUYA_TIMER_MODE_E mode;

    uint32_t period_us;      /* what user requested in us */
    uint32_t start_ticks;    /* ticks when started (best-effort) */
    uint32_t period_ticks;   /* converted ticks */
} tuya_timer_ctx_t;

static tuya_timer_ctx_t g_timer_ctx[MAX_TUYA_TIMERS] = {0};

/* Convert microseconds to ClockP ticks (ceil, min 1 if us>0) */
static uint32_t _us_to_ticks(uint32_t us)
{
    uint32_t tick_us = ClockP_getSystemTickPeriod(); /* in microseconds */
    if (tick_us == 0) {
        /* Defensive fallback */
        tick_us = 1000;
    }

    if (us == 0) {
        return 0;
    }

    /* ceil(us / tick_us) */
    uint32_t ticks = (us + tick_us - 1u) / tick_us;
    if (ticks == 0) ticks = 1;
    return ticks;
}

/* Convert ticks to microseconds (best-effort) */
static uint32_t _ticks_to_us(uint32_t ticks)
{
    uint32_t tick_us = ClockP_getSystemTickPeriod();
    if (tick_us == 0) tick_us = 1000;
    return ticks * tick_us;
}

/* ClockP callback wrapper */
static void _clockp_cb(uintptr_t arg)
{
    TUYA_TIMER_NUM_E timer_id = (TUYA_TIMER_NUM_E)arg;

    if (timer_id >= (TUYA_TIMER_NUM_E)MAX_TUYA_TIMERS) {
        return;
    }

    tuya_timer_ctx_t *ctx = &g_timer_ctx[timer_id];

    /* Call user callback first (matches typical timer semantics) */
    if (ctx->cb) {
        ctx->cb(ctx->args);
    }

    /* If periodic -> re-arm by setting timeout again and starting.
       ClockP instances are one-shot by design unless period is set.
       We implement periodic manually for portability. */
    if (ctx->mode != TUYA_TIMER_MODE_ONCE && ctx->period_ticks > 0) {
        ClockP_setTimeout(ctx->clock_handle, ctx->period_ticks);
        ctx->start_ticks = ClockP_getSystemTicks();
        ClockP_start(ctx->clock_handle);
    }
}

/**
 * @brief timer init
 */
OPERATE_RET tkl_timer_init(TUYA_TIMER_NUM_E timer_id, TUYA_TIMER_BASE_CFG_T *cfg)
{
    if (timer_id >= (TUYA_TIMER_NUM_E)MAX_TUYA_TIMERS || cfg == NULL) {
        return OPRT_INVALID_PARM;
    }

    tuya_timer_ctx_t *ctx = &g_timer_ctx[timer_id];

    /* If already initialized, deinit first to avoid dangling */
    if (ctx->inited) {
        (void)tkl_timer_deinit(timer_id);
    }

    memset(ctx, 0, sizeof(*ctx));

    ctx->cb   = cfg->cb;
    ctx->args = cfg->args;
    ctx->mode = cfg->mode;

    /* Create a ClockP instance (one-shot). We'll set timeout in start(). */
    ClockP_Params params;
    ClockP_Params_init(&params);
    params.startFlag = false;
    params.period    = 0; /* one-shot */
    params.arg       = (uintptr_t)timer_id;

    ctx->clock_handle = ClockP_construct(&ctx->clock_struct, _clockp_cb, 1 /* temp */, &params);
    if (ctx->clock_handle == NULL) {
        return OPRT_COM_ERROR;
    }

    ctx->inited = true;
    return OPRT_OK;
}

/**
 * @brief timer start
 */
OPERATE_RET tkl_timer_start(TUYA_TIMER_NUM_E timer_id, uint32_t us)
{
    if (timer_id >= (TUYA_TIMER_NUM_E)MAX_TUYA_TIMERS || us == 0) {
        return OPRT_INVALID_PARM;
    }

    tuya_timer_ctx_t *ctx = &g_timer_ctx[timer_id];
    if (!ctx->inited || ctx->clock_handle == NULL) {
        return OPRT_INVALID_PARM;
    }

    ctx->period_us    = us;
    ctx->period_ticks = _us_to_ticks(us);

    /* Stop if running, then arm */
    ClockP_stop(ctx->clock_handle);
    ClockP_setTimeout(ctx->clock_handle, ctx->period_ticks);

    ctx->start_ticks = ClockP_getSystemTicks();
    ClockP_start(ctx->clock_handle);

    return OPRT_OK;
}

/**
 * @brief timer stop
 */
OPERATE_RET tkl_timer_stop(TUYA_TIMER_NUM_E timer_id)
{
    if (timer_id >= (TUYA_TIMER_NUM_E)MAX_TUYA_TIMERS) {
        return OPRT_INVALID_PARM;
    }

    tuya_timer_ctx_t *ctx = &g_timer_ctx[timer_id];
    if (!ctx->inited || ctx->clock_handle == NULL) {
        return OPRT_OK;
    }

    ClockP_stop(ctx->clock_handle);
    return OPRT_OK;
}

/**
 * @brief timer deinit
 */
OPERATE_RET tkl_timer_deinit(TUYA_TIMER_NUM_E timer_id)
{
    if (timer_id >= (TUYA_TIMER_NUM_E)MAX_TUYA_TIMERS) {
        return OPRT_INVALID_PARM;
    }

    tuya_timer_ctx_t *ctx = &g_timer_ctx[timer_id];
    if (!ctx->inited) {
        return OPRT_OK;
    }

    if (ctx->clock_handle != NULL) {
        ClockP_stop(ctx->clock_handle);
        ClockP_destruct(&ctx->clock_struct);
        ctx->clock_handle = NULL;
    }

    ctx->inited = false;
    ctx->cb = NULL;
    ctx->args = NULL;
    ctx->period_us = 0;
    ctx->period_ticks = 0;
    ctx->start_ticks = 0;

    return OPRT_OK;
}

/**
 * @brief current timer get (best-effort)
 *
 * Returns "elapsed time since last start" in microseconds (approx. by ticks).
 */
OPERATE_RET tkl_timer_get_current_value(TUYA_TIMER_NUM_E timer_id, uint32_t *us)
{
    if (timer_id >= (TUYA_TIMER_NUM_E)MAX_TUYA_TIMERS || us == NULL) {
        return OPRT_INVALID_PARM;
    }

    tuya_timer_ctx_t *ctx = &g_timer_ctx[timer_id];
    if (!ctx->inited) {
        return OPRT_INVALID_PARM;
    }

    uint32_t now = ClockP_getSystemTicks();
    uint32_t elapsed_ticks = now - ctx->start_ticks;

    *us = _ticks_to_us(elapsed_ticks);
    return OPRT_OK;
}

/**
 * @brief timer get (interval)
 */
OPERATE_RET tkl_timer_get(TUYA_TIMER_NUM_E timer_id, uint32_t *us)
{
    if (timer_id >= (TUYA_TIMER_NUM_E)MAX_TUYA_TIMERS || us == NULL) {
        return OPRT_INVALID_PARM;
    }

    tuya_timer_ctx_t *ctx = &g_timer_ctx[timer_id];
    if (!ctx->inited) {
        return OPRT_INVALID_PARM;
    }

    *us = ctx->period_us;
    return OPRT_OK;
}
