#ifndef I2C_H
#define I2C_H

#include <stdbool.h>

#include "regs.h"
#include "gpio.h"

typedef struct i2c_handle_t {
    i2c_t *bus;
    const pin_t *sda;
    const pin_t *scl;
} i2c_handle_t;

#define I2C_MASTER_WRITE    (0)
#define I2C_MASTER_READ     (1)

void i2c_init(i2c_handle_t *i2c);

bool i2c_start(i2c_handle_t *i2c);

void i2c_stop(i2c_handle_t *i2c);

bool i2c_write_byte(i2c_handle_t *i2c, uint8_t byte);

void i2c_reset_bus(i2c_handle_t *i2c);

#endif // I2C_H
