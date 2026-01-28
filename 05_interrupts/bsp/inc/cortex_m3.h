#ifndef CORTEX_M3
#define CORTEX_M3

#define ENABLE_IRQ()    asm("cpsie i")

#define DISABLE_IRQ()   asm("cpsid i")

#define FAST_MUL_4(x)   ((x) << (2U))

#endif // CORTEX_M3
