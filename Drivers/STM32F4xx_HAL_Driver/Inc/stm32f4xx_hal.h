#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "stm32f4xx.h"
#include "stm32f407xx.h"

#define HAL_OK      0x00
#define HAL_ERROR   0x01
#define HAL_BUSY    0x02
#define HAL_TIMEOUT 0x03

typedef struct {
    uint32_t BaudRate;
    uint32_t WordLength;
    uint32_t StopBits;
    uint32_t Parity;
    uint32_t Mode;
} UART_InitTypeDef;

typedef struct __UART_HandleTypeDef {
    USART_TypeDef    *Instance;
    UART_InitTypeDef  Init;
    uint32_t          State;
} UART_HandleTypeDef;

typedef struct {
    GPIO_TypeDef *Instance;
    uint32_t      Pin;
    uint32_t      Mode;
    uint32_t      Pull;
    uint32_t      Speed;
    uint32_t      Alternate;
} GPIO_InitTypeDef;

typedef struct __GPIO_HandleTypeDef {
    GPIO_TypeDef *Instance;
    uint32_t      Pin;
    uint32_t      Mode;
    uint32_t      Pull;
    uint32_t      Speed;
    uint32_t      Alternate;
} GPIO_HandleTypeDef;

/* Clock enable macros */
#define __HAL_RCC_GPIOA_CLK_ENABLE() (RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN)
#define __HAL_RCC_GPIOB_CLK_ENABLE() (RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN)
#define __HAL_RCC_GPIOC_CLK_ENABLE() (RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN)
#define __HAL_RCC_GPIOD_CLK_ENABLE() (RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN)
#define __HAL_RCC_USART2_CLK_ENABLE() (RCC->APB1ENR |= RCC_APB1ENR_USART2EN)
#define __HAL_RCC_USART3_CLK_ENABLE() (RCC->APB1ENR |= RCC_APB1ENR_USART3EN)
#define __HAL_RCC_PWR_CLK_ENABLE()    (RCC->APB1ENR |= RCC_APB1ENR_PWREN)

/* System reset */
#define NVIC_SystemReset() do { SCB->AIRCR = (0x5FAUL << 16) | (1UL << 2); while(1) {} } while(0)

void     HAL_Init(void);
void     HAL_RCC_OscConfig(void);
void     HAL_RCC_ClockConfig(void);
uint32_t HAL_GetTick(void);
void     HAL_Delay(uint32_t delay);
void     HAL_IncTick(void);

void     HAL_NVIC_SetPriorityGrouping(uint32_t prio);
void     HAL_NVIC_SetPriority(IRQn_Type irq, uint32_t prio);
void     HAL_NVIC_EnableIRQ(IRQn_Type irq);

void HAL_GPIO_Init(GPIO_TypeDef *GPIOx, GPIO_InitTypeDef *init);
void HAL_GPIO_WritePin(GPIO_TypeDef *GPIOx, uint32_t pin, uint32_t state);
void HAL_GPIO_TogglePin(GPIO_TypeDef *GPIOx, uint32_t pin);

void HAL_UART_Init(UART_HandleTypeDef *huart);
void HAL_UART_Transmit(UART_HandleTypeDef *huart, const uint8_t *data, uint32_t len, uint32_t timeout);
