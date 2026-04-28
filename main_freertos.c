/*
 * Copyright (c) 2024, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
/*
 *  ======== main_freertos.c ========
 */

#include <stdint.h>

#ifdef _ICCARM_
#include <DLib_Threads.h>
#endif

/* POSIX Header files */
#include <pthread.h>

/* RTOS header files */
#include <FreeRTOS.h>
#include <task.h>

#include <ti/drivers/Board.h>
#include <ti_drivers_config.h>

#include "tkl_board_config.h"
#include "wrapper_config.h"
#include "tal_kv.h"

extern void *mainThread(void *arg0);

/* Stack size in bytes */
#define THREADSTACKSIZE 6048

/*
 * ======= Application Side Config =======
 */

static void init_tuya_map_defaults(TKL_BOARD_CONFIG_T *map)
{
    for (int i = 0; i < TKL_MAX_UART_PORTS; ++i) {
        map->uart_map[i] = -1;
    }
    for (int i = 0; i < TKL_MAX_ADC_PORTS; ++i) {
        map->adc_map[i] = -1;
    }
    for (int i = 0; i < TKL_MAX_PWM_CHANNELS; ++i) {
        map->pwm_map[i] = -1;
    }
    for (int i = 0; i < TKL_MAX_GPIO_PINS; ++i) {
        map->gpio_map[i] = -1;
    }
    for (int i = 0; i < TKL_MAX_SPI_PORTS; ++i) {
        map->spi_map[i] = -1;
    }
    for (int i = 0; i < TKL_MAX_I2C_PORTS; ++i) {
        map->i2c_map[i] = -1;
    }
    for (int i = 0; i < TKL_MAX_I2S_PORTS; ++i) {
        map->i2s_map[i] = -1;
    }
}

/* ====== Product Fields ======== */

#define PRODUCT_ID        "tjoktmmglvy4hc9x"
#define PRODUCT_UUID      "uuid4a3e78a4b5695414"
#define AUTH_KEY          "MS37HFWKHv6aff0WeXa8eGXkToZl15vQ"
#define SOFTWARE_VERSION  "1.0.0"

/* ====== Product Fields ======== */

/* Declare and initialize Tuya config */
static minimal_config_iot_t config = {
    .product_id   = PRODUCT_ID,
    .uuid         = PRODUCT_UUID,
    .auth_key     = AUTH_KEY,
    .software_ver = SOFTWARE_VERSION
};

static tal_kv_cfg_t kv_cfg = {
    .seed = "vmlkasdh93dlvlcy",
    .key  = "dflfuap134ddlduq"
};

/*
 *  ======== main ========
 */
int main(void)
{
    pthread_t thread;
    pthread_attr_t attrs;
    struct sched_param priParam;
    int retc;

#ifdef _ICCARM_
    /* initialize the system locks */
    __iar_Initlocks();
#endif

    Board_init();

    /* Initialize KV */
    int kv_ret = tal_kv_init(&kv_cfg);
    if (kv_ret != 0) {
        while (1) {}
    }

    /* Create and initialize Tuya HW mapping */
    TKL_BOARD_CONFIG_T tuya_hw_map = {0};
    init_tuya_map_defaults(&tuya_hw_map);

    /* UART */
#ifdef CONFIG_UART2_0
    tuya_hw_map.uart_map[0] = CONFIG_UART2_0;
#endif

    /* ADC */
#ifdef CONFIG_ADC_0
    tuya_hw_map.adc_map[0] = CONFIG_ADC_0;
#endif

    /* PWM */
#ifdef CONFIG_PWM_0
    tuya_hw_map.pwm_map[0] = CONFIG_PWM_0;
#endif

    /* GPIO */
#ifdef CONFIG_GPIO_LED_0
    tuya_hw_map.gpio_map[0] = CONFIG_GPIO_LED_0;
#endif

    /* SPI */
#ifdef CONFIG_SPI_0
    tuya_hw_map.spi_map[0] = CONFIG_SPI_0;
#endif

    /* Optional future mappings */
    /* tuya_hw_map.i2c_map[0] = ...; */
    /* tuya_hw_map.i2s_map[0] = ...; */

    tkl_hw_board_init(&tuya_hw_map);

    /* Tuya minimal init */
    if (config_minimal_init(&config) != 0) {
        return -1;
    }

    /* Initialize the attributes structure with default values */
    pthread_attr_init(&attrs);

    /* Set priority, detach state, and stack size attributes */
    priParam.sched_priority = 5;
    retc = pthread_attr_setschedparam(&attrs, &priParam);
    retc |= pthread_attr_setdetachstate(&attrs, PTHREAD_CREATE_DETACHED);
    retc |= pthread_attr_setstacksize(&attrs, THREADSTACKSIZE);

    if (retc != 0) {
        while (1) {}
    }

    retc = pthread_create(&thread, &attrs, mainThread, NULL);
    if (retc != 0) {
        while (1) {}
    }

    /* Start the FreeRTOS scheduler */
    vTaskStartScheduler();

    return 0;
}