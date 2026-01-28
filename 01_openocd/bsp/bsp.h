#ifndef BSP_H
#define BSP_H

#define HWREG(x)    (*((volatile uint32_t *) (x)))
 
/* RCC */

#define RCC_BASE            (0x40021000U)

#define RCC_APB2ENR         (0x0018U)
 
/* GPIO */

#define GPIOC_BASE          (0x40011000U)

#define GPIO_CRH            (0x0004U)
#define GPIO_ODR            (0x000CU)

#endif // BSP_H
