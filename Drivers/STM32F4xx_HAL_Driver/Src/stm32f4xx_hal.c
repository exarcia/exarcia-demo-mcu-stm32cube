#include "stm32f4xx_hal.h"
#include "stm32f407xx.h"
#include "system_stm32f4xx.h"

static volatile uint32_t s_tick = 0;

uint32_t HAL_GetTick(void) { return s_tick; }
void HAL_IncTick(void)     { s_tick++; }
void HAL_Delay(uint32_t ms) {
    uint32_t start = s_tick;
    while ((s_tick - start) < ms) { __NOP(); }
}

void HAL_Init(void) {
    HAL_NVIC_SetPriorityGrouping(0x03);
    HAL_RCC_OscConfig();
    HAL_RCC_ClockConfig();
}

void HAL_RCC_OscConfig(void) {
    RCC->CR |= RCC_CR_HSEON;
    while (!(RCC->CR & RCC_CR_HSERDY)) {}
}

void HAL_RCC_ClockConfig(void) {
    FLASH->ACR = FLASH_ACR_LATENCY_5WS | FLASH_ACR_PRFTEN | FLASH_ACR_ICEN | FLASH_ACR_DCEN;
    RCC->CFGR = RCC_CFGR_PPRE1_DIV4 | RCC_CFGR_PPRE2_DIV2;
    RCC->PLLCFGR = (8UL) | (336UL << 6) | (0UL << 16) | (7UL << 24) | RCC_PLLCFGR_PLLSRC_HSE;
    RCC->CR |= RCC_CR_PLLON;
    while (!(RCC->CR & RCC_CR_PLLRDY)) {}
    RCC->CFGR = (RCC->CFGR & ~RCC_CFGR_SW_MASK) | RCC_CFGR_SW_PLL;
    while ((RCC->CFGR & RCC_CFGR_SWS_MASK) != RCC_CFGR_SWS_PLL) {}
    SystemCoreClock = 168000000UL;
}

void HAL_NVIC_SetPriorityGrouping(uint32_t prio) {
    NVIC_SetPriorityGrouping(prio);
}

void HAL_NVIC_SetPriority(IRQn_Type irq, uint32_t prio) {
    NVIC_SetPriority(irq, prio);
}

void HAL_NVIC_EnableIRQ(IRQn_Type irq) {
    NVIC_EnableIRQ(irq);
}

void HAL_GPIO_Init(GPIO_TypeDef *GPIOx, GPIO_InitTypeDef *init) {
    uint32_t pos = 0;
    while (init->Pin >> pos) {
        if (init->Pin & (1U << pos)) {
            uint32_t tmp;
            tmp = GPIOx->MODER & ~(0x3UL << (pos * 2));
            GPIOx->MODER = tmp | ((init->Mode & 0x3UL) << (pos * 2));
            tmp = GPIOx->PUPDR & ~(0x3UL << (pos * 2));
            GPIOx->PUPDR = tmp | ((init->Pull & 0x3UL) << (pos * 2));
            tmp = GPIOx->OSPEEDR & ~(0x3UL << (pos * 2));
            GPIOx->OSPEEDR = tmp | ((init->Speed & 0x3UL) << (pos * 2));
        }
        pos++;
    }
}

void HAL_GPIO_WritePin(GPIO_TypeDef *GPIOx, uint32_t pin, uint32_t state) {
    if (state) { GPIOx->BSRR = pin; }
    else       { GPIOx->BSRR = (pin << 16); }
}

void HAL_GPIO_TogglePin(GPIO_TypeDef *GPIOx, uint32_t pin) {
    GPIOx->ODR ^= pin;
}
