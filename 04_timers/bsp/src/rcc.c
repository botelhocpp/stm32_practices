#include "rcc.h"

#include "regs.h"

void rcc_enable_clock(rcc_clk_en_id_t id) {
    if(id >= RCC_CLK_EN_ID_APB2) {
        RCC->APB2ENR |= 1U << (id - RCC_CLK_EN_ID_APB2);
    }
    else if(id >= RCC_CLK_EN_ID_APB1) {
        RCC->APB1ENR |= 1U << (id - RCC_CLK_EN_ID_APB1);
    }
    else if(id >= RCC_CLK_EN_ID_AHB) {
        RCC->AHBENR |= 1U << (id - RCC_CLK_EN_ID_AHB);
    }
}
