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
    /* 1. Habilitar o HSE (High Speed External) - O cristal de 8MHz */
    RCC->CR |= RCC_CR_HSEON;

    /* Esperar o HSE estabilizar */
    while (!(RCC->CR & RCC_CR_HSERDY));

    /* 2. Configurar a Latência da Flash (MUITO IMPORTANTE)
       A memória Flash não consegue ler a 72MHz. Precisamos de "Wait States".
       Para 72MHz, precisamos de 2 Wait States (LATENCY_2). */
    FLASH->ACR |= FLASH_ACR_LATENCY_2;
    
    /* Opcional: Habilitar o buffer de prefetch para melhorar performance */
    FLASH->ACR |= FLASH_ACR_PRFTBE;

    /* 3. Configurar os Prescalers dos Barramentos (HCLK, PCLK1, PCLK2) */
    
    /* AHB Prescaler = 1 (O core vai rodar a 72MHz) */
    RCC->CFGR |= RCC_CFGR_HPRE_DIV1;
    
    /* APB2 Prescaler = 1 (Periféricos rápidos como GPIO, ADC vão a 72MHz) */
    RCC->CFGR |= RCC_CFGR_PPRE2_DIV1;
    
    /* APB1 Prescaler = 2 (IMPORTANTE: APB1 é limitado a 36MHz. 72/2 = 36) */
    RCC->CFGR |= RCC_CFGR_PPRE1_DIV2;

    /* 4. Configurar o PLL (Phase Locked Loop) */
    /* Fonte do PLL = HSE (Cristal externo) 
       Multiplicador = 9 (8MHz * 9 = 72MHz) */
    RCC->CFGR &= ~(RCC_CFGR_PLLSRC | RCC_CFGR_PLLMULL); // Limpa bits antigos
    RCC->CFGR |= (RCC_CFGR_PLLSRC | RCC_CFGR_PLLMULL9);

    /* 5. Ligar o PLL */
    RCC->CR |= RCC_CR_PLLON;

    /* Esperar o PLL travar (estabilizar) */
    while (!(RCC->CR & RCC_CR_PLLRDY));

    /* 6. Selecionar o PLL como fonte do clock do sistema (SYSCLK) */
    RCC->CFGR &= ~RCC_CFGR_SW;   // Limpa bits de seleção
    RCC->CFGR |= RCC_CFGR_SW_PLL; // Seleciona PLL

    /* 7. Esperar a troca ser concluída pelo hardware */
    while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL);

    core_clock = RCC_MAX_CORE_CLOCK;
}
