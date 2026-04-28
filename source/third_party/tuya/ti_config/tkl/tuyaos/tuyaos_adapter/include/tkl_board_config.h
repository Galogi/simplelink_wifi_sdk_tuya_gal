/**
 * @file tkl_board_config.h
 * @brief Tuya Kernel Layer - Hardware Board Configuration
 */

#ifndef TKL_BOARD_CONFIG_H
#define TKL_BOARD_CONFIG_H

#include <stdint.h>
#include <stdbool.h>

/* --- Board Limits --- */
/* These match the macros used in your tkl_*.c files */
#define TKL_MAX_UART_PORTS     4
#define TKL_MAX_ADC_PORTS      8
#define TKL_MAX_PWM_CHANNELS   16
#define TKL_MAX_GPIO_PINS      64
#define TKL_MAX_SPI_PORTS      4
#define TKL_MAX_I2C_PORTS      4
#define TKL_MAX_I2S_PORTS      2

/* --- Constants --- */
#define TKL_ADC_WIDTH_12BIT         12
#define TKL_ADC_REF_VOLTAGE_MV      3300

/* --- Board Configuration Structure --- */
typedef struct {
    int16_t uart_map[TKL_MAX_UART_PORTS];
    int16_t adc_map[TKL_MAX_ADC_PORTS];
    int16_t pwm_map[TKL_MAX_PWM_CHANNELS];
    int16_t gpio_map[TKL_MAX_GPIO_PINS];
    int16_t spi_map[TKL_MAX_SPI_PORTS];
    int16_t i2c_map[TKL_MAX_I2C_PORTS];
    int16_t i2s_map[TKL_MAX_I2S_PORTS];
} TKL_BOARD_CONFIG_T;

/* --- API Prototypes --- */
void tkl_hw_board_init(const TKL_BOARD_CONFIG_T *config);

int16_t tkl_hw_get_uart_index(uint8_t tuya_port);
int16_t tkl_hw_get_adc_index(uint8_t tuya_port);
int16_t tkl_hw_get_pwm_index(uint8_t tuya_port);
int16_t tkl_hw_get_gpio_index(uint8_t tuya_pin);
int16_t tkl_hw_get_spi_index(uint8_t tuya_port);
int16_t tkl_hw_get_i2c_index(uint8_t tuya_port);
int16_t tkl_hw_get_i2s_index(uint8_t tuya_port);

#endif /* TKL_BOARD_CONFIG_H */