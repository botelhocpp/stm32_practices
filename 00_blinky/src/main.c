#include <stdint.h>

#include "bsp.h"

#define LED (13)

int main(void) {
    HWREG(RCC_BASE + RCC_APB2ENR) |= (1 << 4);

    HWREG(GPIOC_BASE + GPIO_CRH) &= ~(0xF << (20));

    /* Output (Push-Pull) at 2MHz */
    HWREG(GPIOC_BASE + GPIO_CRH) |= (0x2 << (20));

    while(1) {
        HWREG(GPIOC_BASE + GPIO_ODR) ^= (1 << LED);

        for(volatile int i = 0; i < 500000; i++);
    }
}
