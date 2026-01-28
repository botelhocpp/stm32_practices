#ifndef TIM_H
#define TIM_H

#include <stdbool.h>

#include "regs.h"

typedef struct tim_config_t {
    bool enable_preload;
    bool one_pulse;
    bool update_disable;
    bool only_overflow;
    uint16_t prescaler;
} tim_config_t;

void tim_init(tim_t *tim, const tim_config_t *config);

bool tim_check_flag(tim_t *tim);

void tim_clear_flag(tim_t *tim);

void tim_set_reload_value(tim_t *tim, uint16_t value);

void tim_generate_event(tim_t *tim);

void tim_start(tim_t *tim);

void tim_stop(tim_t *tim);

#endif // TIM_H
