/**
 * @file tkl_board.c
 * @brief Tuya Kernel Layer - Board Configuration Implementation
 */

#include "tkl_board_config.h"
#include <string.h>

/* Global Configuration Storage */
static TKL_BOARD_CONFIG_T g_board_config;
static bool g_is_initialized = false;

/* Helper Macro to check bounds and initialization */
#define CHECK_MAP(port, max, map_array)            \
    do {                                           \
        if (!g_is_initialized || (port) >= (max))  \
            return -1;                             \
        return g_board_config.map_array[(port)];   \
    } while (0)

/**
 * @brief Initialize the Board Configuration
 */
void tkl_hw_board_init(const TKL_BOARD_CONFIG_T *config)
{
    if (config) {
        memcpy(&g_board_config, config, sizeof(TKL_BOARD_CONFIG_T));
        g_is_initialized = true;
    }
}

int16_t tkl_hw_get_uart_index(uint8_t tuya_port)
{
    CHECK_MAP(tuya_port, TKL_MAX_UART_PORTS, uart_map);
}

int16_t tkl_hw_get_adc_index(uint8_t tuya_port)
{
    CHECK_MAP(tuya_port, TKL_MAX_ADC_PORTS, adc_map);
}

int16_t tkl_hw_get_pwm_index(uint8_t tuya_port)
{
    CHECK_MAP(tuya_port, TKL_MAX_PWM_CHANNELS, pwm_map);
}

int16_t tkl_hw_get_gpio_index(uint8_t tuya_pin)
{
    CHECK_MAP(tuya_pin, TKL_MAX_GPIO_PINS, gpio_map);
}

int16_t tkl_hw_get_spi_index(uint8_t tuya_port)
{
    CHECK_MAP(tuya_port, TKL_MAX_SPI_PORTS, spi_map);
}

int16_t tkl_hw_get_i2c_index(uint8_t tuya_port)
{
    CHECK_MAP(tuya_port, TKL_MAX_I2C_PORTS, i2c_map);
}

int16_t tkl_hw_get_i2s_index(uint8_t tuya_port)
{
    CHECK_MAP(tuya_port, TKL_MAX_I2S_PORTS, i2s_map);
}
