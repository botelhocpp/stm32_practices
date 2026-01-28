#include "tim.h"

#define TIM_EGR_UG      (0)
#define TIM_CR1_ARPE    (7)
#define TIM_CR1_OPM     (3)
#define TIM_CR1_URS     (2)
#define TIM_CR1_UDIS    (1)
#define TIM_CR1_CEN     (0)

void tim_init(tim_t *tim, const tim_config_t *config) {
    tim->CR1 =  (config->update_disable << TIM_CR1_UDIS) | 
                (config->only_overflow << TIM_CR1_URS) | 
                (config->one_pulse << TIM_CR1_OPM) | 
                (config->enable_preload << TIM_CR1_ARPE); 

    tim->PSC = config->prescaler;

    tim_generate_event(tim);

    tim_clear_flag(tim);
}

bool tim_check_flag(tim_t *tim) {
    return (tim->SR & 1U);
}

void tim_clear_flag(tim_t *tim) {
    tim->SR = 0U;
}

void tim_set_reload_value(tim_t *tim, uint16_t value) {
    tim->ARR = value;
}

void tim_generate_event(tim_t *tim) {
    tim->EGR = (1U << TIM_EGR_UG);
}

void tim_start(tim_t *tim) {
    tim->CR1 |= (1U << TIM_CR1_CEN);
}

void tim_stop(tim_t *tim) {
    tim->CR1 &= ~(1U << TIM_CR1_CEN);
}