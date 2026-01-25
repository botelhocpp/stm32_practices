#include <stdint.h>

extern int main(void);

__attribute__((naked, noreturn))
void _start(void) {
    extern uint32_t _sdata;
    extern uint32_t _edata;
    extern uint32_t _sbss;
    extern uint32_t _ebss;
    extern uint32_t _sidata;

    /* Copy .data from Flash to SRAM */
    for (uint32_t *src = &_sdata, *dst = &_sidata; src < &_edata; src++, dst++) {
        *src = *dst;
    }

    /* Zero .bss in SRAM */
    for (uint32_t *src = &_sbss; src < &_ebss; src++) {
        *src = 0;
    }

    main();

    while(1);
}

extern void _estack(void);

__attribute__((section(".vectors")))
void (*vectors[])(void) = {
    &_estack,
    (void (*)(void)) (&_start)
};
