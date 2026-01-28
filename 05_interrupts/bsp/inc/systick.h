#ifndef SYSTICK_H
#define SYSTICK_H

#include <stdint.h>
#include <stdbool.h>

#define SYSTICK_ENABLE_EXCEPTION    (true)
#define SYSTICK_DISABLE_EXCEPTION   (false)

void systick_init(uint32_t period_ms, bool enable_exception);

void systick_set_period_ms(uint32_t period_ms);

void systick_set_period_us(uint32_t period_us);

uint32_t systick_current_val(void);

void systick_start(void);

void systick_stop(void);

bool systick_check_flag(void);

void systick_enable_exception(void);

void systick_disable_exception(void);

#endif // SYSTICK_H
