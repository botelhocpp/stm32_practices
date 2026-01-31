#include "gpio.h"

#include "afio.h"
#include "nvic.h"

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

/* EXTI Control */;

static afio_port_t gpio_get_afio_port(const gpio_t *gpio);

void gpio_exti_enable_interrupt(const pin_t *pin, port_exti_trigger_t trigger) {
    const afio_port_t port = gpio_get_afio_port(pin->port);

    afio_enable_ext_interrupt(port, pin->num);

    gpio_exti_unmask_interrupt(pin);

    gpio_exti_set_trigger(pin, trigger);
}

void gpio_exti_disable_interrupt(const pin_t *pin) {
    const afio_port_t port = gpio_get_afio_port(pin->port);

    afio_disable_ext_interrupt(port, pin->num);

    gpio_exti_mask_interrupt(pin);

    gpio_exti_set_trigger(pin, PORT_EXTI_TRIGGER_NONE);
}

void gpio_exti_set_trigger(const pin_t *pin, port_exti_trigger_t trigger) {
    const uint32_t mask = (1U << pin->num);

    if(trigger & PORT_EXTI_TRIGGER_RISING) {
        EXTI->RTSR |= mask;
    }
    else {
        EXTI->RTSR &= ~mask;
    }

    if(trigger & PORT_EXTI_TRIGGER_FALLING) {
        EXTI->FTSR |= mask;
    }
    else {
        EXTI->FTSR &= ~mask;
    }
}

void gpio_exti_mask_interrupt(const pin_t *pin) {
    EXTI->IMR &= ~(1U << pin->num);
}

void gpio_exti_unmask_interrupt(const pin_t *pin) {
    EXTI->IMR |= (1U << pin->num);
}

bool gpio_exti_check_pending_flag(const pin_t *pin) {
    return (EXTI->PR >> pin->num) & 1U;
}

void gpio_exti_clear_pending_flag(const pin_t *pin) {
    EXTI->PR |= (1U << pin->num);
}

uint32_t gpio_exti_get_id(uint32_t pin) {
    uint32_t id = 0;

    if(pin >= 10) {
        id = NVIC_EXTI10_TO_15;
    }
    else if(pin >= 5) {
        id = NVIC_EXTI5_TO_9;
    }
    else {
        id = NVIC_EXTI0 + pin;
    }

    return id;
}

static afio_port_t gpio_get_afio_port(const gpio_t *gpio) {
    afio_port_t port = 0;

    switch((uint32_t) gpio) {
        case (uint32_t) GPIOA:
            port = AFIO_PORT_A;
            break;
        case (uint32_t) GPIOB:
            port = AFIO_PORT_B;
            break;
        case (uint32_t) GPIOC:
            port = AFIO_PORT_C;
            break;
        case (uint32_t) GPIOD:
            port = AFIO_PORT_D;
            break;
        case (uint32_t) GPIOE:
            port = AFIO_PORT_E;
            break;
        case (uint32_t) GPIOF:
            port = AFIO_PORT_F;
            break;
        case (uint32_t) GPIOG:
            port = AFIO_PORT_G;
            break;
    }

    return port;
}
