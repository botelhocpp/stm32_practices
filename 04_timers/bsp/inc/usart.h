#ifndef USART_H
#define USART_H

#include <stdint.h>
#include <stdbool.h>

#include "regs.h"
#include "gpio.h"

void usart_init(usart_t *usart, const pin_t *rx, const pin_t *tx, uint32_t baudrate);

bool usart_transmitter_done(usart_t *usart);

bool usart_receiver_done(usart_t *usart);

void usart_write(usart_t *usart, uint8_t byte);

uint8_t usart_read(usart_t *usart);

void usart_write_blocking(usart_t *usart, uint8_t byte);

void usart_print(usart_t *usart, const char *str);

uint8_t usart_read_blocking(usart_t *usart);

#endif // USART_H
