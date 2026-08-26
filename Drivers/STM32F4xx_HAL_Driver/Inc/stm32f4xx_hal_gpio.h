#pragma once

#include "stm32f4xx.h"
#include "stm32f407xx.h"
#include "stm32f4xx_hal.h"

#define GPIO_PIN_0   (1U << 0)
#define GPIO_PIN_1   (1U << 1)
#define GPIO_PIN_2   (1U << 2)
#define GPIO_PIN_3   (1U << 3)
#define GPIO_PIN_4   (1U << 4)
#define GPIO_PIN_5   (1U << 5)
#define GPIO_PIN_6   (1U << 6)
#define GPIO_PIN_7   (1U << 7)
#define GPIO_PIN_8   (1U << 8)
#define GPIO_PIN_9   (1U << 9)
#define GPIO_PIN_13  (1U << 13)

#define GPIO_MODE_OUTPUT_PP 0x01
#define GPIO_MODE_INPUT     0x00
#define GPIO_NOPULL         0x00
#define GPIO_SPEED_FREQ_LOW 0x00

void HAL_GPIO_Init(GPIO_TypeDef *GPIOx, GPIO_InitTypeDef *init);
void HAL_GPIO_WritePin(GPIO_TypeDef *GPIOx, uint32_t pin, uint32_t state);
void HAL_GPIO_TogglePin(GPIO_TypeDef *GPIOx, uint32_t pin);
