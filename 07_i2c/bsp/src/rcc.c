#include "rcc.h"

#include "regs.h"

#define FLASH_ACR_LATENCY_2     (0b010)
#define FLASH_ACR_PRFTBE        (1U << 4)

#define RCC_CR_HSEON            (1U << 16)
#define RCC_CR_HSERDY           (1U << 17) 
#define RCC_CR_PLLON            (1U << 24)
#define RCC_CR_PLLRDY           (1U << 25)
#define RCC_CFGR_SW             (0x03U)
#define RCC_CFGR_SW_PLL         (0x02U)
#define RCC_CFGR_SWS            (0x0CU)
#define RCC_CFGR_SWS_PLL        (0x08U)
#define RCC_CFGR_HPRE_DIV1      (0x00U)
#define RCC_CFGR_PPRE1_DIV2     (0x400U)
#define RCC_CFGR_PPRE2_DIV1     (0x00U)
#define RCC_CFGR_PLLSRC         (1U << 16)
#define RCC_CFGR_PLLMULL        (0x3C0000U)
#define RCC_CFGR_PLLMULL9       (0x1C0000U)

uint32_t core_clock = RCC_DEF_CORE_CLOCK;

static void rcc_enable_crystal(void);

static void rcc_flash_config_latency(void);

static void rcc_bus_config(void);

static void rcc_pll_setup(void);

void rcc_enable_clock(rcc_clk_en_id_t id) {
    if(id >= RCC_CLK_EN_ID_APB2) {
        RCC->APB2ENR |= 1U << (id - RCC_CLK_EN_ID_APB2);
    }
    else if(id >= RCC_CLK_EN_ID_APB1) {
        RCC->APB1ENR |= 1U << (id - RCC_CLK_EN_ID_APB1);
    }
    else if(id >= RCC_CLK_EN_ID_AHB) {
        RCC->AHBENR |= 1U << (id - RCC_CLK_EN_ID_AHB);
    }
}

uint32_t rcc_get_core_clock(void) {
    return core_clock;
}

void rcc_setup_core_clock(void) {
    rcc_enable_crystal();

    rcc_flash_config_latency();

    rcc_bus_config();

    rcc_pll_setup();

    core_clock = RCC_MAX_CORE_CLOCK;
}

void rcc_enable_crystal(void) {
    /* Enable HSE (High Speed External), the 8MHz crystal */
    RCC->CR |= RCC_CR_HSEON;

    /* Waits HSE stabilizes */
    while (!(RCC->CR & RCC_CR_HSERDY));
}

void rcc_flash_config_latency(void) {
    /* Config 2 Wait States for the Flash to read at 72 MHz */
    FLASH->ACR |= FLASH_ACR_LATENCY_2;
    
    /* Enable Prefetch Buffer */
    FLASH->ACR |= FLASH_ACR_PRFTBE;
}

void rcc_bus_config(void) {
    /* AHB Prescaler = 1: Core will run at 72MHz (HCLK) */
    RCC->CFGR |= RCC_CFGR_HPRE_DIV1;
    
    /* APB2 Prescaler = 1: Fast peripherals will run at 72MHz (PCLK1), like GPIO and ADC */
    RCC->CFGR |= RCC_CFGR_PPRE2_DIV1;
    
    /* APB1 Prescaler = 2: APB1 is limited at 36MHz (PCLK2) */
    RCC->CFGR |= RCC_CFGR_PPRE1_DIV2;
}

void rcc_pll_setup(void) {
    /* Set HSE as the PLL source and a multiplier of 9 (8MHz * 9 = 72MHz)*/
    RCC->CFGR &= ~(RCC_CFGR_PLLSRC | RCC_CFGR_PLLMULL);
    RCC->CFGR |= (RCC_CFGR_PLLSRC | RCC_CFGR_PLLMULL9);

    /* Enable PLL */
    RCC->CR |= RCC_CR_PLLON;

    /* Waits PLL lock (stabilize) */
    while (!(RCC->CR & RCC_CR_PLLRDY));

    /* Select PLL as the system clock source (SYSCLK) */
    RCC->CFGR &= ~RCC_CFGR_SW;   // Limpa bits de seleção
    RCC->CFGR |= RCC_CFGR_SW_PLL; // Seleciona PLL

    /* Waits the change be concluded by the hardware */
    while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL);
}
