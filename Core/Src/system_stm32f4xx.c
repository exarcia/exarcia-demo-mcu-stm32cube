/* Minimal system_stm32f4xx.c — clock already configured in HAL_Init. */
#include "stm32f4xx.h"
#include "stm32f407xx.h"
#include "system_stm32f4xx.h"

uint32_t SystemCoreClock = 168000000UL;

void SystemInit(void) {
    RCC->CR |= RCC_CR_HSION;
    RCC->CFGR = 0x00000000;
    RCC->CR &= ~(RCC_CR_HSEON | RCC_CR_PLLON);
    SCB->VTOR = 0x08000000UL;
}

void SystemCoreClockUpdate(void) {
    SystemCoreClock = 168000000UL;
}
