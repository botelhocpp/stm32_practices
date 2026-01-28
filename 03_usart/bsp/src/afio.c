#include "afio.h"

void afio_remap(afio_peripheral_t afio_peripheral, uint8_t option) {
    switch(afio_peripheral) {
        case AFIO_PERIPHERAL_SPI1:
        case AFIO_PERIPHERAL_I2C1:
        case AFIO_PERIPHERAL_USART1:
        case AFIO_PERIPHERAL_USART2:
        case AFIO_PERIPHERAL_TIM4:
        case AFIO_PERIPHERAL_PD01:
        case AFIO_PERIPHERAL_TIM5CH4:
            AFIO->MAPR &= ~(0b1 << afio_peripheral);
            break;

        case AFIO_PERIPHERAL_USART3:
        case AFIO_PERIPHERAL_TIM1:
        case AFIO_PERIPHERAL_TIM2:
        case AFIO_PERIPHERAL_TIM3:
        case AFIO_PERIPHERAL_CAN:
        case AFIO_PERIPHERAL_ADC1_ETRGINJ:
        case AFIO_PERIPHERAL_ADC1_ETRGREG:
        case AFIO_PERIPHERAL_ADC2_ETRGINJ:
        case AFIO_PERIPHERAL_ADC2_ETRGREG:
            AFIO->MAPR &= ~(0b11 << afio_peripheral);
            break;
        default:
            break;
    }
    
    AFIO->MAPR |= (option << afio_peripheral);
}

void afio_debug_config(debug_config_t debug_config) {
    AFIO->MAPR &= (0b111 << 24);
    AFIO->MAPR |= (debug_config << 24);
}
