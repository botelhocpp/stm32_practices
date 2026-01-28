#ifndef USART_H
#define USART_H

#include <stdint.h>
#include <stdbool.h>

#include "regs.h"
#include "gpio.h"

#define USART_FLAG_TXE_INT_EN   (1U << 7)
#define USART_FLAG_RXNE_INT_EN  (1U << 5)

#define USART_FLAG_RX_NOT_EMPTY (1U << 5)
#define USART_FLAG_TX_EMPTY     (1U << 7)

typedef enum usart_interrupt_t {
    USART_INTERRUPT_IDLE,
    USART_INTERRUPT_RX_NOT_EMPTY,
    USART_INTERRUPT_TX_COMPLETE,
    USART_INTERRUPT_TX_EMPTY,
    USART_INTERRUPT_PARITY_ERROR,
} usart_interrupt_t;

#define usart_tx_empty(x)       (usart_get_status(x) & USART_FLAG_TX_EMPTY)
#define usart_rx_not_empty(x)   (usart_get_status(x) & USART_FLAG_RX_NOT_EMPTY)

void usart_init(usart_t *usart, const pin_t *rx, const pin_t *tx, uint32_t freq, uint32_t baudrate);

uint32_t usart_get_status(usart_t *usart);

void usart_write(usart_t *usart, uint8_t byte);

uint8_t usart_read(usart_t *usart);

void usart_write_blocking(usart_t *usart, uint8_t byte);

void usart_print(usart_t *usart, const char *str);

uint8_t usart_read_blocking(usart_t *usart);

void usart_enable_interrupt(usart_t *usart, usart_interrupt_t interrupt);

void usart_disable_interrupt(usart_t *usart, usart_interrupt_t interrupt);

#endif // USART_H
