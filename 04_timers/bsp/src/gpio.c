#include "gpio.h"

#define FAST_MUL_4(x)   ((x) << (2U))

void gpio_init(const pin_t *pin, port_mode_t mode, port_conf_t conf, bool use_pull_up) {
    /* Setup pull resistor */
    if(mode == PORT_MODE_INPUT && conf == PORT_CONF_INPUT_PULL) {
        gpio_write(pin, (uint8_t) use_pull_up);
    }

    const bool is_high = (pin->num > 7);

    const uint8_t shift_amount = (is_high) ?
        FAST_MUL_4(pin->num - 8) : 
        FAST_MUL_4(pin->num);
    
    volatile uint32_t *port = (is_high) ? 
        &pin->port->CRH : 
        &pin->port->CRL;
    
    const uint8_t data = (conf << 2) | (mode);

    *port &= ~(0xF << shift_amount);
    *port |=  (data << shift_amount);
}

void gpio_write(const pin_t *pin, uint8_t level) {
    if(level) {
        pin->port->ODR |= (1U << pin->num);
    }
    else {
        pin->port->ODR &= ~(1U << pin->num);
    }
}

void gpio_toggle(const pin_t *pin) {
    pin->port->ODR ^= (1U << pin->num);
}

uint8_t gpio_read(const pin_t *pin) {
    return (pin->port->IDR >> pin->num) & 1U;
}