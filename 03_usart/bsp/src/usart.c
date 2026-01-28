#include "usart.h"

#include "rcc.h"

#define USART_CR1_UE    (13)
#define USART_CR1_TE    (3)
#define USART_CR1_RE    (2)

#define USART_SR_TXE    (7)
#define USART_SR_RXNE   (5)

void usart_init(usart_t *usart, const pin_t *rx, const pin_t *tx, uint32_t baudrate) {
    gpio_init(tx, PORT_MODE_OUTPUT_50MHZ, PORT_CONF_AF_OUTPUT_PUSH_PULL, DONT_USE_PULL);

    gpio_init(rx, PORT_MODE_INPUT, PORT_CONF_INPUT_FLOAT, DONT_USE_PULL);

    usart->BRR = RCC_DEF_CORE_CLOCK / baudrate;

    usart->CR1 |= (1U << USART_CR1_UE) | (1U << USART_CR1_TE) | (1U << USART_CR1_RE);
}

bool usart_transmitter_done(usart_t *usart) {
    return ((usart->SR >> USART_SR_TXE) & 0x1);
}

bool usart_receiver_done(usart_t *usart) {
    return ((usart->SR >> USART_SR_RXNE) & 0x1);
}

void usart_write(usart_t *usart, uint8_t byte) {
    usart->DR = byte;
}

uint8_t usart_read(usart_t *usart) {
    return usart->DR;
}

void usart_write_blocking(usart_t *usart, uint8_t byte) {
    usart_write(usart, byte);

    while(!usart_transmitter_done(usart));
}

void usart_print(usart_t *usart, const char *str) {
    while(*str != '\0') {
        usart_write_blocking(usart, *str++);
    }
}

uint8_t usart_read_blocking(usart_t *usart) {
    while(!usart_receiver_done(usart));

    return usart_read(usart);
}
