#ifndef RCC_H
#define RCC_H

#define RCC_CLK_EN_ID_AHB   (0)
#define RCC_CLK_EN_ID_APB1  (32)
#define RCC_CLK_EN_ID_APB2  (64)

typedef enum rcc_clk_en_id_t {
    /* AHB peripheral clock enable IDs */
    RCC_CLK_EN_ID_DMA1     = (RCC_CLK_EN_ID_AHB + 0),
    RCC_CLK_EN_ID_DMA2     = (RCC_CLK_EN_ID_AHB + 1),
    RCC_CLK_EN_ID_SRAM     = (RCC_CLK_EN_ID_AHB + 2),
    RCC_CLK_EN_ID_FLITF    = (RCC_CLK_EN_ID_AHB + 4),
    RCC_CLK_EN_ID_CRC      = (RCC_CLK_EN_ID_AHB + 6),
    RCC_CLK_EN_ID_FSMC     = (RCC_CLK_EN_ID_AHB + 8),
    RCC_CLK_EN_ID_SDIO     = (RCC_CLK_EN_ID_AHB + 10),

    /* APB1 peripheral clock enable IDs */
    RCC_CLK_EN_ID_TIM2     = (RCC_CLK_EN_ID_APB1 + 0),
    RCC_CLK_EN_ID_TIM3     = (RCC_CLK_EN_ID_APB1 + 1),
    RCC_CLK_EN_ID_TIM4     = (RCC_CLK_EN_ID_APB1 + 2),
    RCC_CLK_EN_ID_TIM5     = (RCC_CLK_EN_ID_APB1 + 3),
    RCC_CLK_EN_ID_TIM6     = (RCC_CLK_EN_ID_APB1 + 4),
    RCC_CLK_EN_ID_TIM7     = (RCC_CLK_EN_ID_APB1 + 5),
    RCC_CLK_EN_ID_WWDG     = (RCC_CLK_EN_ID_APB1 + 11),
    RCC_CLK_EN_ID_SPI2     = (RCC_CLK_EN_ID_APB1 + 14),
    RCC_CLK_EN_ID_SPI3     = (RCC_CLK_EN_ID_APB1 + 15),
    RCC_CLK_EN_ID_USART2   = (RCC_CLK_EN_ID_APB1 + 17),
    RCC_CLK_EN_ID_USART3   = (RCC_CLK_EN_ID_APB1 + 18),
    RCC_CLK_EN_ID_USART4   = (RCC_CLK_EN_ID_APB1 + 19),
    RCC_CLK_EN_ID_USART5   = (RCC_CLK_EN_ID_APB1 + 20),
    RCC_CLK_EN_ID_I2C1     = (RCC_CLK_EN_ID_APB1 + 21),
    RCC_CLK_EN_ID_I2C2     = (RCC_CLK_EN_ID_APB1 + 22),
    RCC_CLK_EN_ID_USB      = (RCC_CLK_EN_ID_APB1 + 23),
    RCC_CLK_EN_ID_CAN      = (RCC_CLK_EN_ID_APB1 + 25),
    RCC_CLK_EN_ID_BKP      = (RCC_CLK_EN_ID_APB1 + 27),
    RCC_CLK_EN_ID_PWR      = (RCC_CLK_EN_ID_APB1 + 28),
    RCC_CLK_EN_ID_DAC      = (RCC_CLK_EN_ID_APB1 + 29),

    /* APB2 peripheral clock enable IDs */
    RCC_CLK_EN_ID_AFIO     = (RCC_CLK_EN_ID_APB2 + 0),
    RCC_CLK_EN_ID_GPIOA    = (RCC_CLK_EN_ID_APB2 + 2),
    RCC_CLK_EN_ID_GPIOB    = (RCC_CLK_EN_ID_APB2 + 3),
    RCC_CLK_EN_ID_GPIOC    = (RCC_CLK_EN_ID_APB2 + 4),
    RCC_CLK_EN_ID_GPIOD    = (RCC_CLK_EN_ID_APB2 + 5),
    RCC_CLK_EN_ID_GPIOE    = (RCC_CLK_EN_ID_APB2 + 6),
    RCC_CLK_EN_ID_GPIOF    = (RCC_CLK_EN_ID_APB2 + 7),
    RCC_CLK_EN_ID_GPIOG    = (RCC_CLK_EN_ID_APB2 + 8),
    RCC_CLK_EN_ID_ADC1     = (RCC_CLK_EN_ID_APB2 + 9),
    RCC_CLK_EN_ID_ADC2     = (RCC_CLK_EN_ID_APB2 + 10),
    RCC_CLK_EN_ID_TIM1     = (RCC_CLK_EN_ID_APB2 + 11),
    RCC_CLK_EN_ID_SPI1     = (RCC_CLK_EN_ID_APB2 + 12),
    RCC_CLK_EN_ID_TIM8     = (RCC_CLK_EN_ID_APB2 + 13),
    RCC_CLK_EN_ID_USART1   = (RCC_CLK_EN_ID_APB2 + 14),
    RCC_CLK_EN_ID_ADC3     = (RCC_CLK_EN_ID_APB2 + 15),
} rcc_clk_en_id_t;

void rcc_enable_clock(rcc_clk_en_id_t id);

#endif // RCC_H
