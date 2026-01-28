#include <stdint.h>

#include "rcc.h"
#include "tim.h"
#include "gpio.h"
#include "afio.h"
#include "usart.h"

#define SERIAL_BAUDRATE     (9600)

/* TIM2 is 1kHz */
#define TIM2_PRESCALER      ((RCC_DEF_CORE_CLOCK / 1000) - 1)

/* Pins */
const pin_t onboard_led =   { .port = GPIOC, .num = 13 };
const pin_t red_button =    { .port = GPIOB, .num = 15 };
const pin_t black_button =  { .port = GPIOA, .num = 15 };
const pin_t blue_button =   { .port = GPIOA, .num = 9  };
const pin_t tx =            { .port = GPIOB, .num = 6  };
const pin_t rx =            { .port = GPIOB, .num = 7  };

void init_clock(void);

void init_peripherals(void);

void delay_ms(uint16_t ms);

int main(void) {
    init_clock();
    init_peripherals();
    
    usart_print(USART1, "STM32\n\r");

    while(1) {
        const uint8_t red = gpio_read(&red_button);
        const uint8_t blue = gpio_read(&blue_button);
        const uint8_t black = gpio_read(&black_button);

        gpio_write(&onboard_led, !(red || blue || black));

        if(red) {
            usart_print(USART1, "Red\n\r");
            delay_ms(500);
        }
        if(blue) {
            usart_print(USART1, "Blue\n\r");
            delay_ms(500);
        }
        if(black) {
            usart_print(USART1, "Black\n\r");
            delay_ms(500);
        }
        
        if(usart_receiver_done(USART1)) {
            switch(usart_read(USART1)) {
                case '0':
                    usart_print(USART1, "ZERO\n\r");
                    break;
                case '1':
                    usart_print(USART1, "ONE\n\r");
                    break;
                case '2':
                    usart_print(USART1, "TWO\n\r");
                    break;
                case '3':
                    usart_print(USART1, "THREE\n\r");
                    break;
                default:
                    usart_print(USART1, "OH!\n\r");
                    break;
            }
        }
    }
}

void init_clock(void) {
    rcc_enable_clock(RCC_CLK_EN_ID_AFIO);
    rcc_enable_clock(RCC_CLK_EN_ID_GPIOA);
    rcc_enable_clock(RCC_CLK_EN_ID_GPIOB);
    rcc_enable_clock(RCC_CLK_EN_ID_GPIOC);
    rcc_enable_clock(RCC_CLK_EN_ID_USART1);
    rcc_enable_clock(RCC_CLK_EN_ID_TIM2);
}

void init_peripherals(void) {
    /* Disable JTAG to release PA15 */
    afio_debug_config(DEBUG_CONFIG_DIS_JTAG_EN_SWD);

    gpio_init(&onboard_led, PORT_MODE_OUTPUT_2MHZ, PORT_CONF_GP_OUTPUT_PUSH_PULL, DONT_USE_PULL);
    gpio_init(&red_button, PORT_MODE_INPUT, PORT_CONF_INPUT_PULL, USE_PULL_DOWN);
    gpio_init(&black_button, PORT_MODE_INPUT, PORT_CONF_INPUT_PULL, USE_PULL_DOWN);
    gpio_init(&blue_button, PORT_MODE_INPUT, PORT_CONF_INPUT_PULL, USE_PULL_DOWN);

    afio_remap(AFIO_PERIPHERAL_USART1, 1);

    usart_init(USART1, &rx, &tx, SERIAL_BAUDRATE);

    /* Setup TIM2 as a 1kHz one-pulse timer */
    const tim_config_t tim2_config = {
        .enable_preload = true,
        .one_pulse = true,
        .only_overflow = true,
        .update_disable = false,
        .prescaler = TIM2_PRESCALER
    };
    tim_init(TIM2, &tim2_config);
}

void delay_ms(uint16_t ms) {
    if(ms == 0) return;

    tim_set_reload_value(TIM2, ms - 1);

    tim_generate_event(TIM2);

    tim_clear_flag(TIM2);

    tim_start(TIM2);

    while(!tim_check_flag(TIM2));

    tim_clear_flag(TIM2);
}
