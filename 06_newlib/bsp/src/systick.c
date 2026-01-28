#include "systick.h"

#include "regs.h"

#define SYSTICK_CTRL_COUNTFLAG   (16)
#define SYSTICK_CTRL_CLKSOURCE   (2)
#define SYSTICK_CTRL_TICKINT     (1)
#define SYSTICK_CTRL_ENABLE      (0)

void systick_init(uint32_t period_ms, bool enable_exception) {
    systick_set_period_ms(period_ms);

    if(enable_exception) {
        systick_enable_exception();
    }

    systick_start();
}

void systick_set_period_ms(uint32_t period_ms) {
    SYSTICK->LOAD = (period_ms * SYSTICK->CALIB) - 1;
}

void systick_set_period_us(uint32_t period_us) {
    SYSTICK->LOAD = (period_us * SYSTICK->CALIB/1000) - 1;
}

uint32_t systick_current_val(void) {
    return SYSTICK->VAL;
}

void systick_start(void) {
    SYSTICK->CTRL |= (1U << SYSTICK_CTRL_ENABLE);
}

void systick_stop(void) {
    SYSTICK->CTRL &= ~(1U << SYSTICK_CTRL_ENABLE);
}

bool systick_check_flag(void) {
    return (SYSTICK->CTRL >> SYSTICK_CTRL_COUNTFLAG) & 0x1;
}

void systick_enable_exception(void) {
    SYSTICK->CTRL |= (1U << SYSTICK_CTRL_TICKINT);
}

void systick_disable_exception(void) {
    SYSTICK->CTRL &= ~(1U << SYSTICK_CTRL_TICKINT);
}
