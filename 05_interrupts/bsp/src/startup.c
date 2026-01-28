#include <stdint.h>

#include "cortex_m3.h"

extern uint32_t _sdata;
extern uint32_t _edata;
extern uint32_t _sbss;
extern uint32_t _ebss;
extern uint32_t _sidata;
extern void _estack(void);

extern int main(void);

__attribute__((naked, noreturn))
void _start(void) {

    DISABLE_IRQ();

    /* Copy .data from Flash to SRAM */
    for (uint32_t *p_src = &_sidata, *p_dst = &_sdata; p_dst < &_edata; p_src++, p_dst++) {
        *p_dst = *p_src;
    }

    /* Zero .bss in SRAM */
    for (uint32_t *p_dst = &_sbss; p_dst < &_ebss; p_dst++) {
        *p_dst = 0;
    }

    ENABLE_IRQ();

    main();

    while(1);
}

void _default_handler(void) {
    while(1);
}

#define WEAK_ALIAS __attribute__((weak, alias("_default_handler")))

void _nmi_handler(void) WEAK_ALIAS;
void _hardfault_handler(void) WEAK_ALIAS;
void _memmanage_handler(void) WEAK_ALIAS;
void _busfault_handler(void) WEAK_ALIAS;
void _usagefault_handler(void) WEAK_ALIAS;
void _svc_handler(void) WEAK_ALIAS;
void _debug_mon_handler(void) WEAK_ALIAS;
void _pendsv_handler(void) WEAK_ALIAS;
void _systick_handler(void) WEAK_ALIAS;

void _wwdg_irq_handler(void) WEAK_ALIAS;
void _pvd_irq_handler(void) WEAK_ALIAS;
void _tamper_irq_handler(void) WEAK_ALIAS;
void _rtc_irq_handler(void) WEAK_ALIAS;
void _flash_irq_handler(void) WEAK_ALIAS;
void _rcc_irq_handler(void) WEAK_ALIAS;
void _exti0_irq_handler(void) WEAK_ALIAS;
void _exti1_irq_handler(void) WEAK_ALIAS;
void _exti2_irq_handler(void) WEAK_ALIAS;
void _exti3_irq_handler(void) WEAK_ALIAS;
void _exti4_irq_handler(void) WEAK_ALIAS;
void _dma1_ch1_irq_handler(void) WEAK_ALIAS;
void _dma1_ch2_irq_handler(void) WEAK_ALIAS;
void _dma1_ch3_irq_handler(void) WEAK_ALIAS;
void _dma1_ch4_irq_handler(void) WEAK_ALIAS;
void _dma1_ch5_irq_handler(void) WEAK_ALIAS;
void _dma1_ch6_irq_handler(void) WEAK_ALIAS;
void _dma1_ch7_irq_handler(void) WEAK_ALIAS;
void _adc1_adc2_irq_handler(void) WEAK_ALIAS;
void _usb_hp_can_tx_irq_handler(void) WEAK_ALIAS;
void _usb_lp_can_rx_irq_handler(void) WEAK_ALIAS;
void _can_rx_irq_handler(void) WEAK_ALIAS;
void _can_sce_irq_handler(void) WEAK_ALIAS;
void _exti5_to_exti9_irq_handler(void) WEAK_ALIAS;
void _tim1_brk_irq_handler(void) WEAK_ALIAS;
void _tim1_up_irq_handler(void) WEAK_ALIAS;
void _tim1_trg_com_irq_handler(void) WEAK_ALIAS;
void _tim1_cc_irq_handler(void) WEAK_ALIAS;
void _tim2_irq_handler(void) WEAK_ALIAS;
void _tim3_irq_handler(void) WEAK_ALIAS;
void _tim4_irq_handler(void) WEAK_ALIAS;
void _i2c1_ev_irq_handler(void) WEAK_ALIAS;
void _i2c1_er_irq_handler(void) WEAK_ALIAS;
void _i2c2_ev_irq_handler(void) WEAK_ALIAS;
void _i2c2_er_irq_handler(void) WEAK_ALIAS;
void _spi1_irq_handler(void) WEAK_ALIAS;
void _spi2_irq_handler(void) WEAK_ALIAS;
void _usart1_irq_handler(void) WEAK_ALIAS;
void _usart2_irq_handler(void) WEAK_ALIAS;
void _usart3_irq_handler(void) WEAK_ALIAS;
void _exti10_to_exti15_irq_handler(void) WEAK_ALIAS;
void _rtc_alarm_irq_handler(void) WEAK_ALIAS;
void _usb_wakeup_irq_handler(void) WEAK_ALIAS;

__attribute__((section(".vectors")))
void (*vectors[])(void) = {
    &_estack,
    &_start,
    &_nmi_handler,
    &_hardfault_handler,
    &_memmanage_handler,
    &_busfault_handler,
    &_usagefault_handler,
    0,
    0,
    0,
    0,
    &_svc_handler,
    &_debug_mon_handler,
    0,
    &_pendsv_handler,
    &_systick_handler,
    &_wwdg_irq_handler,
    &_pvd_irq_handler,
    &_tamper_irq_handler,
    &_rtc_irq_handler,
    &_flash_irq_handler,
    &_rcc_irq_handler,
    &_exti0_irq_handler,
    &_exti1_irq_handler,
    &_exti2_irq_handler,
    &_exti3_irq_handler,
    &_exti4_irq_handler,
    &_dma1_ch1_irq_handler,
    &_dma1_ch2_irq_handler,
    &_dma1_ch3_irq_handler,
    &_dma1_ch4_irq_handler,
    &_dma1_ch5_irq_handler,
    &_dma1_ch6_irq_handler,
    &_dma1_ch7_irq_handler,
    &_adc1_adc2_irq_handler,
    &_usb_hp_can_tx_irq_handler,
    &_usb_lp_can_rx_irq_handler,
    &_can_rx_irq_handler,
    &_can_sce_irq_handler,
    &_exti5_to_exti9_irq_handler,
    &_tim1_brk_irq_handler,
    &_tim1_up_irq_handler,
    &_tim1_trg_com_irq_handler,
    &_tim1_cc_irq_handler,
    &_tim2_irq_handler,
    &_tim3_irq_handler,
    &_tim4_irq_handler,
    &_i2c1_ev_irq_handler,
    &_i2c1_er_irq_handler,
    &_i2c2_ev_irq_handler,
    &_i2c2_er_irq_handler,
    &_spi1_irq_handler,
    &_spi2_irq_handler,
    &_usart1_irq_handler,
    &_usart2_irq_handler,
    &_usart3_irq_handler,
    &_exti10_to_exti15_irq_handler,
    &_rtc_alarm_irq_handler,
    &_usb_wakeup_irq_handler,
};
