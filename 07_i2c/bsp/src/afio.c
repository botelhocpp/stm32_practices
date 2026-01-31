#include "afio.h"

#include <stdbool.h>

static void afio_config_ext_interrupt(afio_port_t port, uint8_t pin, bool set_port) {
    const uint8_t cr_num = (pin >> 2);
    const uint8_t field = FAST_MUL_4(pin & 0b11);

    volatile uint32_t *exticr = (volatile uint32_t *) (&AFIO->EXTICR1 + cr_num);

    *exticr &= ~(0xF << field);

    if(set_port) {
        *exticr |= (port << field);
    }
}

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
    AFIO->MAPR &= ~(0b111 << 24);
    AFIO->MAPR |= (debug_config << 24);
}

void afio_enable_ext_interrupt(afio_port_t port, uint8_t pin) {
    afio_config_ext_interrupt(port, pin, true);
}

void afio_disable_ext_interrupt(afio_port_t port, uint8_t pin) {
    afio_config_ext_interrupt(port, pin, false);
}
