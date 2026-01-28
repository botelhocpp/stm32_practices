#ifndef REGS_H
#define REGS_H

#include <stdint.h>
 
/* RCC */

typedef struct rcc_t {
    uint32_t CR;
    uint32_t CFGR;
    uint32_t CIR;
    uint32_t APB2RSTR;
    uint32_t APB1RSTR;
    uint32_t AHBENR;
    uint32_t APB2ENR;
    uint32_t APB1ENR;
    uint32_t BDCR;
    uint32_t CSR;
    uint32_t AHBSTR;
    uint32_t CFGR2;
} rcc_t;

#define RCC     ((volatile rcc_t *)     (0x40021000U))
 
/* GPIO */

typedef struct gpio_t {
    uint32_t CRL;
    uint32_t CRH;
    uint32_t IDR;
    uint32_t ODR;
    uint32_t BSRR;
    uint32_t BRR;
    uint32_t LCKR;
} gpio_t;

#define GPIOA   ((volatile gpio_t *)    (0x40010800))
#define GPIOB   ((volatile gpio_t *)    (0x40010C00))
#define GPIOC   ((volatile gpio_t *)    (0x40011000))
#define GPIOD   ((volatile gpio_t *)    (0x40011400))
#define GPIOE   ((volatile gpio_t *)    (0x40011800))
#define GPIOF   ((volatile gpio_t *)    (0x40011C00))
#define GPIOG   ((volatile gpio_t *)    (0x40012000))
 
/* AFIO */

typedef struct afio_t {
    uint32_t AVCR;
    uint32_t MAPR;
    uint32_t EXTICR1;
    uint32_t EXTICR2;
    uint32_t EXTICR3;
    uint32_t EXTICR4;
} afio_t;

#define AFIO   ((volatile afio_t *)     (0x40010000))

#endif // REGS_H
