/**
 * @file tkl_pwm.c
 * @brief This file acts as the bridge between Tuya's abstract PWM interface and the TI SimpleLink PWM driver.
 */

// --- BEGIN: user defines and implements ---
#include "tkl_pwm.h"
#include "tuya_error_code.h"
#include <ti/drivers/pwm/PWMTimerWFF3.h>

#include <ti/drivers/PWM.h> 

// [DEPENDENCY INJECTION] Include Board Config
#include "tkl_board_config.h"

/* Define the maximum number of PWM channels supported by the board configuration.
   Set to 10 as a safe buffer. */
#define MAX_PWM_CHANNELS TKL_MAX_PWM_CHANNELS

// Global array to store TI PWM handles so we can reference them later
static PWM_Handle g_pwm_handles[MAX_PWM_CHANNELS] = {NULL};

// Helper: Defines Tuya's standard max duty cycle (10000 = 100%)
#define TUYA_PWM_MAX_DUTY 10000

/* --- FIXED MACROS --- */
// 1. Check if Channel ID is within bounds
#define CHECK_PWM_ID(ch) \
    do { \
        if ((ch) >= MAX_PWM_CHANNELS) return OPRT_INVALID_PARM; \
    } while(0)

// 2. Check if Channel ID is valid AND the TI Driver is actually open
#define CHECK_PWM_HANDLE(ch) \
    do { \
        CHECK_PWM_ID(ch); \
        if (g_pwm_handles[(ch)] == NULL) return OPRT_INVALID_PARM; \
    } while(0)

// 3. Check if a pointer is NULL
#define CHECK_PWM_PTR(ptr) \
    do { \
        if ((ptr) == NULL) return OPRT_INVALID_PARM; \
    } while(0)

// --- END: user defines and implements ---

/**
 * @brief pwm init
 * * @param[in] ch_id: pwm channal id
 * @param[in] cfg: pwm config
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET tkl_pwm_init(TUYA_PWM_NUM_E ch_id, const TUYA_PWM_BASE_CFG_T *cfg)
{
    // --- BEGIN: user implements ---
    PWM_Params params;

    CHECK_PWM_ID(ch_id); // Macro replaces manual check
    CHECK_PWM_PTR(cfg);  // Macro replaces manual check

    // [MAPPING] Get Real Hardware Index from Board Config
    int16_t ti_pwm_index = tkl_hw_get_pwm_index((uint8_t)ch_id);

    // If result is negative, the Application did not map this PWM channel.
    if (ti_pwm_index < 0) {
        return OPRT_NOT_SUPPORTED;
    }

    // Initialize TI PWM parameters with default values
    PWM_Params_init(&params);

    // 1. Set frequency (TI uses Hz by default)
    params.periodUnits = PWM_PERIOD_HZ;
    params.periodValue = cfg->frequency;

    // 2. Set Duty Cycle (Convert Tuya 0-10000 to TI Fraction)
    params.dutyUnits = PWM_DUTY_FRACTION;
    
    // Calculation: (UserDuty / 10000) * Max_Fraction
    uint64_t duty_calc = (uint64_t)cfg->duty * (uint64_t)PWM_DUTY_FRACTION_MAX;
    params.dutyValue = (uint32_t)(duty_calc / TUYA_PWM_MAX_DUTY);
    
    // 3. Set Polarity
    if(cfg->polarity == TUYA_PWM_POSITIVE){
        params.idleLevel = PWM_IDLE_LOW; // If active high, idle is low
    } else {
        params.idleLevel = PWM_IDLE_HIGH; // If active low, idle is high
    }

    // 4. Open the TI Driver using the Mapped Index
    if (g_pwm_handles[ch_id] == NULL) {
         g_pwm_handles[ch_id] = PWM_open(ti_pwm_index, &params);
    }
    
    if(g_pwm_handles[ch_id] == NULL){
        return OPRT_COM_ERROR;
    }

    return OPRT_OK;
    // --- END: user implements ---
}

/**
 * @brief pwm deinit
 * * @param[in] ch_id: pwm channal id
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET tkl_pwm_deinit(TUYA_PWM_NUM_E ch_id)
{
    // --- BEGIN: user implements ---
    CHECK_PWM_HANDLE(ch_id); // Checks ID and if handle exists

    PWM_close(g_pwm_handles[ch_id]);
    g_pwm_handles[ch_id] = NULL;

    return OPRT_OK;
    // --- END: user implements ---
}


/**
 * @brief pwm start
 * * @param[in] ch_id: pwm channal id
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET tkl_pwm_start(TUYA_PWM_NUM_E ch_id)
{
    // --- BEGIN: user implements ---
    CHECK_PWM_HANDLE(ch_id);

    PWM_start(g_pwm_handles[ch_id]);
    
    return OPRT_OK;
    // --- END: user implements ---
}

/**
 * @brief pwm stop
 * * @param[in] ch_id: pwm channal id
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET tkl_pwm_stop(TUYA_PWM_NUM_E ch_id)
{
    // --- BEGIN: user implements ---
    CHECK_PWM_HANDLE(ch_id);

    PWM_stop(g_pwm_handles[ch_id]);
    
    return OPRT_OK;
    // --- END: user implements ---
}

/**
 * @brief multiple pwm channel start
 * * @param[in] ch_id: pwm channal id list
 * @param[in] num: pwm channal number
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET tkl_pwm_multichannel_start(TUYA_PWM_NUM_E *ch_id, uint8_t num)
{
    // --- BEGIN: user implements ---
    CHECK_PWM_PTR(ch_id);

    // Loop through the specific channels provided in the array
    for(int i = 0; i < num; ++i){
        // We call our own start function, which handles checks internally
        tkl_pwm_start(ch_id[i]);
    }

    return OPRT_OK;
    // --- END: user implements ---
}

/**
 * @brief multiple pwm channel stop
 * * @param[in] ch_id: pwm channal id list
 * @param[in] num: pwm channal number
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET tkl_pwm_multichannel_stop(TUYA_PWM_NUM_E *ch_id, uint8_t num)
{
    // --- BEGIN: user implements ---
    CHECK_PWM_PTR(ch_id);

    // Loop through the specific channels provided in the array
    for(int i = 0; i < num; ++i){
        tkl_pwm_stop(ch_id[i]);
    }
    
    return OPRT_OK;
    // --- END: user implements ---
}

/**
 * @brief pwm duty set
 * * @param[in] ch_id: pwm channal id
 * @param[in] duty: pwm duty cycle
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET tkl_pwm_duty_set(TUYA_PWM_NUM_E ch_id, uint32_t duty)
{
    // --- BEGIN: user implements ---
    CHECK_PWM_HANDLE(ch_id);

    // Convert Tuya duty (0-10000) to TI Fraction
    uint64_t duty_calc = (uint64_t)duty * (uint64_t)PWM_DUTY_FRACTION_MAX;
    uint32_t ti_duty_fraction = (uint32_t)(duty_calc / TUYA_PWM_MAX_DUTY);

    int ret = PWM_setDuty(g_pwm_handles[ch_id], ti_duty_fraction);
    
    if(ret < 0) {
        return OPRT_COM_ERROR;
    }

    return OPRT_OK;
    // --- END: user implements ---
}

/**
 * @brief pwm frequency set
 * * @param[in] ch_id: pwm channal id
 * @param[in] frequency: pwm frequency
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET tkl_pwm_frequency_set(TUYA_PWM_NUM_E ch_id, uint32_t frequency)
{
    // --- BEGIN: user implements ---
    CHECK_PWM_HANDLE(ch_id);
    
    // Sets the period (frequency) in Hz
    int ret = PWM_setPeriod(g_pwm_handles[ch_id], frequency);
    
    if(ret < 0) {
        return OPRT_COM_ERROR;
    }

    return OPRT_OK;
    // --- END: user implements ---
}

/**
 * @brief pwm polarity set
 * * @param[in] ch_id: pwm channal id
 * @param[in] polarity: pwm polarity
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET tkl_pwm_polarity_set(TUYA_PWM_NUM_E ch_id, TUYA_PWM_POLARITY_E polarity)
{
    // --- BEGIN: user implements ---
    return OPRT_NOT_SUPPORTED;
    // --- END: user implements ---
}

/**
 * @brief set pwm info
 * * @param[in] ch_id: pwm channal id
 * @param[in] info: pwm info
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET tkl_pwm_info_set(TUYA_PWM_NUM_E ch_id, const TUYA_PWM_BASE_CFG_T *info)
{
    // --- BEGIN: user implements ---
    CHECK_PWM_ID(ch_id);
    CHECK_PWM_PTR(info);

    // 1. Stop current output (if handle exists)
    if (g_pwm_handles[ch_id] != NULL) {
        tkl_pwm_stop(ch_id);
        PWM_close(g_pwm_handles[ch_id]);
        g_pwm_handles[ch_id] = NULL;
    }

    // 2. Re-init with new settings
    return tkl_pwm_init(ch_id, info);
    // --- END: user implements ---
}

/**
 * @brief get pwm info
 * * @param[in] ch_id: pwm channal id
 * @param[out] info: pwm info
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET tkl_pwm_info_get(TUYA_PWM_NUM_E ch_id, TUYA_PWM_BASE_CFG_T *info)
{
    // --- BEGIN: user implements ---
    // TI Drivers do not support reading back configuration from the hardware.
    return OPRT_NOT_SUPPORTED;
    // --- END: user implements ---
}

/**
 * @brief pwm capture mode start
 * * @param[in] ch_id: pwm channal id
 * @param[in] cfg: pwm capture config
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET tkl_pwm_cap_start(TUYA_PWM_NUM_E ch_id, const TUYA_PWM_CAP_IRQ_T *cfg)
{
    // --- BEGIN: user implements ---
    return OPRT_NOT_SUPPORTED;
    // --- END: user implements ---
}

/**
 * @brief pwm capture mode stop
 * * @param[in] ch_id: pwm channal id
 *
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET tkl_pwm_cap_stop(TUYA_PWM_NUM_E ch_id)
{
    // --- BEGIN: user implements ---
    return OPRT_NOT_SUPPORTED;
    // --- END: user implements ---
}