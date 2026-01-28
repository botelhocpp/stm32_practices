#ifndef GPIO_H
#define GPIO_H

#include <stdbool.h>

#include "regs.h"

#define HIGH    (1)
#define LOW     (0)

#define USE_PULL_UP     (true)
#define USE_PULL_DOWN   (false)
#define DONT_USE_PULL   (false)

typedef enum port_mode_t {
    PORT_MODE_INPUT = 0b00,
    PORT_MODE_OUTPUT_10MHZ = 0b01,
    PORT_MODE_OUTPUT_2MHZ = 0b10,
    PORT_MODE_OUTPUT_50MHZ = 0b11
} port_mode_t;

typedef enum port_conf_t {
    PORT_CONF_INPUT_ANALOG = 0b00,
    PORT_CONF_INPUT_FLOAT = 0b01,
    PORT_CONF_INPUT_PULL = 0b10,
    PORT_CONF_GP_OUTPUT_PUSH_PULL = 0b00,
    PORT_CONF_GP_OUTPUT_OPEN_DRAIN = 0b01,
    PORT_CONF_AF_OUTPUT_PUSH_PULL = 0b10,
    PORT_CONF_AF_OUTPUT_OPEN_DRAIN = 0b11
} port_conf_t;

typedef struct pin_t {
    volatile gpio_t *port;
    uint8_t num;
} pin_t;

void gpio_init(const pin_t *pin, port_mode_t mode, port_conf_t conf, bool use_pull_up);

void gpio_write(const pin_t *pin, uint8_t level);

void gpio_toggle(const pin_t *pin);

uint8_t gpio_read(const pin_t *pin);

#endif // GPIO_H
