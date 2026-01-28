#include <stdint.h>

#include "rcc.h"
#include "tim.h"
#include "gpio.h"
#include "afio.h"
#include "nvic.h"
#include "usart.h"
#include "systick.h"

/* Macros */

#define WELCOME_STR         ("STM32\n\r")

#define SERIAL_BAUDRATE     (9600)

#define SYSTICK_PERIOD      (1000)

#define TIM2_PRESCALER      ((RCC_MAX_CORE_CLOCK / 10000) - 1)

#define TIM_CALC_HZ(f)      ((RCC_MAX_CORE_CLOCK)/((TIM2_PRESCALER + 1) * (f)) - 1)

/* Pins */
const pin_t onboard_led =   { .port = GPIOC, .num = 13 };
const pin_t red_button =    { .port = GPIOB, .num = 5 };
const pin_t black_button =  { .port = GPIOA, .num = 15 };
const pin_t blue_button =   { .port = GPIOA, .num = 9  };
const pin_t tx =            { .port = GPIOB, .num = 6  };
const pin_t rx =            { .port = GPIOB, .num = 7  };

/* Private Functions */

static void init_clock(void);

static void init_gpio(void);

static void init_serial(void);

static void init_timers(void);

static void usart_tx_handler(void);

static void usart_rx_handler(void);

/* Variables */

volatile bool red_flag = false;
volatile bool blue_flag = false;
volatile bool black_flag = false;

typedef struct message_t {
    char *str;
    uint8_t size;
    uint8_t idx;
    bool done;
} message_t;

volatile message_t tx_msg = {
    .str = (char *) WELCOME_STR,
    .size = sizeof(WELCOME_STR),
    .idx = 0,
    .done = false
};

volatile char rx_buffer = '\0';

/* Main Functions */

int main(void) {
    DISABLE_IRQ();

    init_clock();
    init_gpio();
    init_serial();
    init_timers();

    ENABLE_IRQ();

    while(1) {
        if(red_flag || blue_flag || black_flag) {
            gpio_toggle(&onboard_led);

            if(red_flag) red_flag = false;
            if(blue_flag) blue_flag = false;
            if(black_flag) black_flag = false;
        }
    }
}

/* Init Functions */

void init_clock(void) {
    rcc_setup_core_clock();

    rcc_enable_clock(RCC_CLK_EN_ID_AFIO);
    rcc_enable_clock(RCC_CLK_EN_ID_GPIOA);
    rcc_enable_clock(RCC_CLK_EN_ID_GPIOB);
    rcc_enable_clock(RCC_CLK_EN_ID_GPIOC);
    rcc_enable_clock(RCC_CLK_EN_ID_USART1);
    rcc_enable_clock(RCC_CLK_EN_ID_TIM2);
}

void init_gpio(void) {
    /* Disable JTAG to release PA15 */
    afio_debug_config(DEBUG_CONFIG_DIS_JTAG_EN_SWD);

    gpio_init(&onboard_led, PORT_MODE_OUTPUT_2MHZ, PORT_CONF_GP_OUTPUT_PUSH_PULL, DONT_USE_PULL);
    gpio_init(&red_button, PORT_MODE_INPUT, PORT_CONF_INPUT_PULL, USE_PULL_DOWN);
    gpio_init(&black_button, PORT_MODE_INPUT, PORT_CONF_INPUT_PULL, USE_PULL_DOWN);
    gpio_init(&blue_button, PORT_MODE_INPUT, PORT_CONF_INPUT_PULL, USE_PULL_DOWN);

    gpio_exti_enable_interrupt(&red_button, PORT_EXTI_TRIGGER_RISING);
    gpio_exti_enable_interrupt(&black_button, PORT_EXTI_TRIGGER_RISING);
    gpio_exti_enable_interrupt(&blue_button, PORT_EXTI_TRIGGER_RISING);

    nvic_enable_interrupt(NVIC_EXTI5_TO_9);
    nvic_enable_interrupt(NVIC_EXTI10_TO_15);
}   

void init_serial(void) {
    afio_remap(AFIO_PERIPHERAL_USART1, 1);

    usart_init(USART1, &rx, &tx, rcc_get_core_clock(), SERIAL_BAUDRATE);

    usart_enable_interrupt(USART1, USART_INTERRUPT_TX_EMPTY);
    usart_enable_interrupt(USART1, USART_INTERRUPT_RX_NOT_EMPTY);

    nvic_enable_interrupt(NVIC_USART1);
}

void init_timers(void) {
    systick_init(SYSTICK_PERIOD, SYSTICK_ENABLE_EXCEPTION);

    /* Setup TIM2 as a 1kHz one-pulse timer */
    const tim_config_t tim2_config = {
        .enable_preload = true,
        .one_pulse = false,
        .only_overflow = true,
        .update_disable = false,
        .prescaler = TIM2_PRESCALER
    };
    tim_init(TIM2, &tim2_config);
    tim_set_reload_value(TIM2, 3 * TIM_CALC_HZ(1));
    tim_generate_event(TIM2);
    tim_enable_update_interrupt(TIM2);
    nvic_enable_interrupt(NVIC_TIM2);
    tim_start(TIM2);
}

/* Global Handlers */

void _exti5_to_exti9_irq_handler(void) {
    if(gpio_exti_check_pending_flag(&red_button)) {
        red_flag = true;
        gpio_exti_clear_pending_flag(&red_button);
    }
    if(gpio_exti_check_pending_flag(&blue_button)) {
        blue_flag = true;
        gpio_exti_clear_pending_flag(&blue_button);
    }
}

void _exti10_to_exti15_irq_handler(void) {
    if(gpio_exti_check_pending_flag(&black_button)) {
        black_flag = true;
        gpio_exti_clear_pending_flag(&black_button);
    }
} 

void _usart1_irq_handler(void) {
    uint32_t flags = usart_get_status(USART1);

    if(flags & USART_FLAG_RX_NOT_EMPTY) {
        usart_rx_handler();
    }

    if(flags & USART_FLAG_TX_EMPTY) {
        usart_tx_handler();   
    }
}

void _tim2_irq_handler(void) {
    tim_clear_flag(TIM2);

    if(!tx_msg.done) return;

    static const char buffer[] = "TIM2\n\r";
    tx_msg = (message_t) {
        .str = (char *) buffer,
        .size = sizeof(buffer),
        .idx = 0,
        .done = false
    };
    usart_enable_interrupt(USART1, USART_INTERRUPT_TX_EMPTY);
}

void _systick_handler(void) {
    gpio_toggle(&onboard_led);   
}

/* Specific Handlers */

void usart_tx_handler(void) {
    if(tx_msg.done) return;

    if(tx_msg.idx < tx_msg.size) {
        usart_write(USART1, tx_msg.str[tx_msg.idx++]);
    }
    else {
        tx_msg.done = true;
        usart_disable_interrupt(USART1, USART_INTERRUPT_TX_EMPTY);
    }
}

void usart_rx_handler(void) {
    if(!tx_msg.done) return;

    rx_buffer = usart_read(USART1);
    tx_msg = (message_t) {
        .str = (char *) &rx_buffer,
        .size = 1,
        .idx = 0,
        .done = false
    };
    usart_enable_interrupt(USART1, USART_INTERRUPT_TX_EMPTY);
}
