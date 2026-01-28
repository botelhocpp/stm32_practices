#ifndef AFIO_H
#define AFIO_H

#include "regs.h"

typedef enum afio_peripheral_t {
    AFIO_PERIPHERAL_SPI1 = 0,
    AFIO_PERIPHERAL_I2C1 = 1,
    AFIO_PERIPHERAL_USART1 = 2,
    AFIO_PERIPHERAL_USART2 = 3,
    AFIO_PERIPHERAL_USART3 = 4,
    AFIO_PERIPHERAL_TIM1 = 6,
    AFIO_PERIPHERAL_TIM2 = 8,
    AFIO_PERIPHERAL_TIM3 = 10,
    AFIO_PERIPHERAL_TIM4 = 12,
    AFIO_PERIPHERAL_CAN = 13,
    AFIO_PERIPHERAL_PD01 = 15,
    AFIO_PERIPHERAL_TIM5CH4 = 16,
    AFIO_PERIPHERAL_ADC1_ETRGINJ = 17,
    AFIO_PERIPHERAL_ADC1_ETRGREG = 18,
    AFIO_PERIPHERAL_ADC2_ETRGINJ = 19,
    AFIO_PERIPHERAL_ADC2_ETRGREG = 20,
    AFIO_PERIPHERAL_SWJ_CFG = 24,
} afio_peripheral_t;

typedef enum debug_config_t {
    DEBUG_CONFIG_FULL_SWJ = 0b000,
    DEBUG_CONFIG_FULL_SWJ_DIS_JNTRST = 0b001,
    DEBUG_CONFIG_DIS_JTAG_EN_SWD = 0b010,
    DEBUG_CONFIG_SWJ_DIS = 0b100,
} debug_config_t;

void afio_remap(afio_peripheral_t afio_peripheral, uint8_t option);

void afio_debug_config(debug_config_t debug_config);

#endif // AFIO_H
