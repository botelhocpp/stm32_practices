#ifndef REGS_H
#define REGS_H

#include <stdint.h>
 
#define _I      volatile const
#define _O      volatile
#define _IO     volatile

/* RCC */

typedef struct rcc_t {
    _IO uint32_t CR;
    _IO uint32_t CFGR;
    _IO uint32_t CIR;
    _IO uint32_t APB2RSTR;
    _IO uint32_t APB1RSTR;
    _IO uint32_t AHBENR;
    _IO uint32_t APB2ENR;
    _IO uint32_t APB1ENR;
    _IO uint32_t BDCR;
    _I  uint32_t CSR;
    _IO uint32_t AHBSTR;
    _IO uint32_t CFGR2;
} rcc_t;

#define RCC     ((rcc_t *)     (0x40021000U))
 
/* GPIO */

typedef struct gpio_t {
    _IO uint32_t CRL;
    _IO uint32_t CRH;
    _I  uint32_t IDR;
    _IO uint32_t ODR;
    _O  uint32_t BSRR;
    _IO uint32_t BRR;
    _IO uint32_t LCKR;
} gpio_t;

#define GPIOA   ((gpio_t *)    (0x40010800))
#define GPIOB   ((gpio_t *)    (0x40010C00))
#define GPIOC   ((gpio_t *)    (0x40011000))
#define GPIOD   ((gpio_t *)    (0x40011400))
#define GPIOE   ((gpio_t *)    (0x40011800))
#define GPIOF   ((gpio_t *)    (0x40011C00))
#define GPIOG   ((gpio_t *)    (0x40012000))
 
/* AFIO */

typedef struct afio_t {
    _IO uint32_t EVCR;
    _IO uint32_t MAPR;
    _IO uint32_t EXTICR1;
    _IO uint32_t EXTICR2;
    _IO uint32_t EXTICR3;
    _IO uint32_t EXTICR4;
} afio_t;

#define AFIO   ((afio_t *)     (0x40010000))
 
/* USART */

typedef struct usart_t {
    _IO uint32_t SR;
    _IO uint32_t DR;
    _IO uint32_t BRR;
    _IO uint32_t CR1;
    _IO uint32_t CR2;
    _IO uint32_t CR3;
    _IO uint32_t GTPR;
} usart_t;

#define USART1  ((usart_t *)   (0x40013800))
#define USART2  ((usart_t *)   (0x40004400))
#define USART3  ((usart_t *)   (0x40004800))

#endif // REGS_H
