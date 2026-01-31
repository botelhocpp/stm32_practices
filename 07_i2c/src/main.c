#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "bsp.h"

#include "oled.h"

/* Macros */

#define MAX_RETRY           (5)

#define SERIAL_BAUDRATE     (9600)

/* Pins */
const pin_t onboard_led =   { .port = GPIOC, .num = 13 };
const pin_t black_button =  { .port = GPIOA, .num = 15 };
const pin_t blue_button =   { .port = GPIOA, .num = 9  };
const pin_t tx =            { .port = GPIOB, .num = 6  };
const pin_t rx =            { .port = GPIOB, .num = 7  };
const pin_t sda =           { .port = GPIOB, .num = 9  };
const pin_t scl =           { .port = GPIOB, .num = 8  };

i2c_handle_t i2c_bus = {
    .bus = I2C1,
    .sda = &sda,
    .scl = &scl
};

/* Private Functions */

static void init_clock(void);

static void init_gpio(void);

static void init_serial(void);

static void init_display(void);

static void draw_display(const char *msg);

static void sys_display_init(void);

static void sys_display_update(void);

static bool sys_i2c_recover(void);

/* Main Functions */

int main(void) {
    DISABLE_IRQ();

    init_clock();
    init_gpio();
    init_serial();
    init_display();

    ENABLE_IRQ();

    puts("starting...\r");

    while(1) {
        bool blue_flag = gpio_read(&blue_button);
        bool black_flag = gpio_read(&black_button);

        gpio_write(&onboard_led, (blue_flag || black_flag));

        if(blue_flag) {
            draw_display("Blue");
        }
        else if(black_flag) {
            draw_display("Black");
        }
        else {
            draw_display("None");
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
    rcc_enable_clock(RCC_CLK_EN_ID_I2C1);
}

void init_gpio(void) {
    /* Disable JTAG to release PA15 */
    afio_debug_config(DEBUG_CONFIG_DIS_JTAG_EN_SWD);

    gpio_init(&onboard_led, PORT_MODE_OUTPUT_2MHZ, PORT_CONF_GP_OUTPUT_PUSH_PULL, DONT_USE_PULL);
    gpio_init(&black_button, PORT_MODE_INPUT, PORT_CONF_INPUT_PULL, USE_PULL_DOWN);
    gpio_init(&blue_button, PORT_MODE_INPUT, PORT_CONF_INPUT_PULL, USE_PULL_DOWN);
}   

void init_serial(void) {
    afio_remap(AFIO_PERIPHERAL_USART1, AFIO_REMAP);

    uint32_t freq = rcc_get_core_clock();
    usart_init(USART1, &rx, &tx, freq, SERIAL_BAUDRATE);
}

void init_display(void) {
    afio_remap(AFIO_PERIPHERAL_I2C1, AFIO_REMAP);

    i2c_reset_bus(&i2c_bus);

    i2c_init(&i2c_bus);

    sys_display_init();
}

#define TITLE_STR       ("OLED Display")
#define SUBTITLE_STR    ("Pedro Botelho")

void draw_display(const char *msg) {
    framebuffer_clear();

    framebuffer_print(TITLE_STR, 0, 1, OLED_COLOR_REGULAR);
    framebuffer_print(SUBTITLE_STR, 1, 1, OLED_COLOR_REGULAR);
    framebuffer_print(msg, CENTRALIZE_ROW(1), CENTRALIZE_COL(strlen(msg) - 1), OLED_COLOR_INVERTED);

    sys_display_update();
}

void sys_display_init(void) {
    uint8_t retry_count = MAX_RETRY;
    bool oled_ok = false;

    while(retry_count > 0) {
        if (oled_init(&i2c_bus)) {
            oled_ok = true;
            break;
        }
        
        sys_i2c_recover();
        retry_count--;
    }

    if(!oled_ok) {
        puts("[critical error: display does not respond]\r");
    }
}

void sys_display_update(void) {
    uint8_t retry_count = MAX_RETRY;
    bool oled_ok = false;

    while(retry_count > 0) {
        if (oled_refresh(&i2c_bus)) {
            oled_ok = true;
            break;
        }
        
        sys_i2c_recover();
        oled_init(&i2c_bus);
        retry_count--;
    }

    if(!oled_ok) {
        while(1);
    }
}

bool sys_i2c_recover(void) {
    puts("[entering recovery mode]\r");
    
    i2c_reset_bus(&i2c_bus);

    i2c_init(&i2c_bus);

    return !i2c_start(&i2c_bus);
}
