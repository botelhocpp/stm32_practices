#include <stdint.h>

#include "rcc.h"
#include "gpio.h"
#include "afio.h"

/* Pins */
const pin_t onboard_led =   { .port = GPIOC, .num = 13 };
const pin_t red_button =    { .port = GPIOB, .num = 15 };
const pin_t black_button =  { .port = GPIOA, .num = 15 };
const pin_t blue_button =   { .port = GPIOA, .num = 9 };

void delay_ms(uint32_t ms);

int main(void) {
    rcc_enable_clock(RCC_CLK_EN_ID_AFIO);
    rcc_enable_clock(RCC_CLK_EN_ID_GPIOA);
    rcc_enable_clock(RCC_CLK_EN_ID_GPIOB);
    rcc_enable_clock(RCC_CLK_EN_ID_GPIOC);

    /* Disable JTAG to release PA15 */
    afio_debug_config(DEBUG_CONFIG_SWJ_DIS);

    gpio_init(&onboard_led, PORT_MODE_OUTPUT_2MHZ, PORT_CONF_GP_OUTPUT_PUSH_PULL, DONT_USE_PULL);
    gpio_init(&red_button, PORT_MODE_INPUT, PORT_CONF_INPUT_PULL, USE_PULL_DOWN);
    gpio_init(&black_button, PORT_MODE_INPUT, PORT_CONF_INPUT_PULL, USE_PULL_DOWN);
    gpio_init(&blue_button, PORT_MODE_INPUT, PORT_CONF_INPUT_PULL, USE_PULL_DOWN);

    while(1) {
        gpio_write(&onboard_led, !gpio_read(&red_button));
        gpio_write(&onboard_led, !gpio_read(&blue_button));
        gpio_write(&onboard_led, !gpio_read(&black_button));
    }
}

void delay_ms(uint32_t ms) {
    for(volatile int i = 0; i < 5000; i++) {
        for(volatile int j = 0; j < ms; j++) {
            asm ("nop");
        }
    }
}
