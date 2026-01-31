#include "nvic.h"

#include "regs.h"

void nvic_enable_interrupt(int id) {
    if(id >= 32) {
        NVIC->ISER[1] |= (1U << (id - 32)); 
    }
    else {
        NVIC->ISER[0] |= (1U << id);
    }
}

void nvic_disable_interrupt(int id) {
    if(id >= 32) {
        NVIC->ICER[1] |= (1U << (id - 32)); 
    }
    else {
        NVIC->ICER[0] |= (1U << id);
    }
}
