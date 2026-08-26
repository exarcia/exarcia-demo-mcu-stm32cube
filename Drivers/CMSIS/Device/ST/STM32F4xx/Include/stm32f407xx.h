/* Minimal STM32F407xx register definitions for this demo. */
#pragma once

#include "stm32f4xx.h"

/* GPIO registers */
typedef struct {
    volatile uint32_t MODER;
    volatile uint32_t OTYPER;
    volatile uint32_t OSPEEDR;
    volatile uint32_t PUPDR;
    volatile uint32_t IDR;
    volatile uint32_t ODR;
    volatile uint32_t BSRR;
    volatile uint32_t LCKR;
    volatile uint32_t AFR[2];
} GPIO_TypeDef;

/* RCC registers */
typedef struct {
    volatile uint32_t CR;
    volatile uint32_t PLLCFGR;
    volatile uint32_t CFGR;
    volatile uint32_t CIR;
    volatile uint32_t AHB1RSTR;
    volatile uint32_t AHB2RSTR;
    volatile uint32_t AHB3RSTR;
    uint32_t          RESERVED0;
    volatile uint32_t APB1RSTR;
    volatile uint32_t APB2RSTR;
    uint32_t          RESERVED1[2];
    volatile uint32_t AHB1ENR;
    volatile uint32_t AHB2ENR;
    volatile uint32_t AHB3ENR;
    uint32_t          RESERVED2;
    volatile uint32_t APB1ENR;
    volatile uint32_t APB2ENR;
    uint32_t          RESERVED3[2];
    volatile uint32_t AHB1LPENR;
    volatile uint32_t AHB2LPENR;
    volatile uint32_t AHB3LPENR;
    uint32_t          RESERVED4;
    volatile uint32_t APB1LPENR;
    volatile uint32_t APB2LPENR;
    uint32_t          RESERVED5[2];
    volatile uint32_t BDCR;
    volatile uint32_t CSR;
    uint32_t          RESERVED6[2];
    volatile uint32_t SSCGR;
    volatile uint32_t PLLI2SCFGR;
} RCC_TypeDef;

/* USART registers */
typedef struct {
    volatile uint32_t SR;
    volatile uint32_t DR;
    volatile uint32_t BRR;
    volatile uint32_t CR1;
    volatile uint32_t CR2;
    volatile uint32_t CR3;
    volatile uint32_t GTPR;
} USART_TypeDef;

/* FLASH registers */
typedef struct {
    volatile uint32_t ACR;
    volatile uint32_t KEYR;
    volatile uint32_t OPTKEYR;
    volatile uint32_t SR;
    volatile uint32_t CR;
    volatile uint32_t OPTCR;
} FLASH_TypeDef;

/* PWR registers */
typedef struct {
    volatile uint32_t CR;
    volatile uint32_t CSR;
} PWR_TypeDef;

/* Peripheral base addresses */
#define PERIPH_BASE     0x40000000UL
#define APB1PERIPH_BASE PERIPH_BASE
#define APB2PERIPH_BASE (PERIPH_BASE + 0x00010000UL)
#define AHB1PERIPH_BASE (PERIPH_BASE + 0x00020000UL)

#define GPIOA_BASE  (AHB1PERIPH_BASE + 0x0000UL)
#define GPIOB_BASE  (AHB1PERIPH_BASE + 0x0400UL)
#define GPIOC_BASE  (AHB1PERIPH_BASE + 0x0800UL)
#define GPIOD_BASE  (AHB1PERIPH_BASE + 0x0C00UL)
#define GPIOE_BASE  (AHB1PERIPH_BASE + 0x1000UL)
#define GPIOG_BASE  (AHB1PERIPH_BASE + 0x1800UL)
#define RCC_BASE    (AHB1PERIPH_BASE + 0x3800UL)
#define FLASH_R_BASE (AHB1PERIPH_BASE + 0x3C00UL)
#define PWR_BASE    (APB1PERIPH_BASE + 0x7000UL)

#define USART2_BASE (APB1PERIPH_BASE + 0x4400UL)
#define USART3_BASE (APB1PERIPH_BASE + 0x4800UL)

#define GPIOA ((GPIO_TypeDef *)GPIOA_BASE)
#define GPIOB ((GPIO_TypeDef *)GPIOB_BASE)
#define GPIOC ((GPIO_TypeDef *)GPIOC_BASE)
#define GPIOD ((GPIO_TypeDef *)GPIOD_BASE)
#define GPIOE ((GPIO_TypeDef *)GPIOE_BASE)
#define GPIOG ((GPIO_TypeDef *)GPIOG_BASE)
#define RCC   ((RCC_TypeDef *)RCC_BASE)
#define FLASH ((FLASH_TypeDef *)FLASH_R_BASE)
#define PWR   ((PWR_TypeDef *)PWR_BASE)
#define USART2 ((USART_TypeDef *)USART2_BASE)
#define USART3 ((USART_TypeDef *)USART3_BASE)

/* RCC bit definitions */
#define RCC_CR_HSION    (1UL << 0)
#define RCC_CR_HSIRDY   (1UL << 1)
#define RCC_CR_HSEON    (1UL << 16)
#define RCC_CR_HSERDY   (1UL << 17)
#define RCC_CR_PLLON    (1UL << 24)
#define RCC_CR_PLLRDY   (1UL << 25)

#define RCC_PLLCFGR_PLLSRC_HSE (1UL << 22)

#define RCC_CFGR_SW_HSI   0x0UL
#define RCC_CFGR_SW_HSE   0x1UL
#define RCC_CFGR_SW_PLL   0x2UL
#define RCC_CFGR_SW_MASK  0x3UL
#define RCC_CFGR_SWS_HSI  (0x0UL << 2)
#define RCC_CFGR_SWS_HSE  (0x1UL << 2)
#define RCC_CFGR_SWS_PLL  (0x2UL << 2)
#define RCC_CFGR_SWS_MASK (0x3UL << 2)
#define RCC_CFGR_PPRE1_DIV4 (0x5UL << 10)
#define RCC_CFGR_PPRE2_DIV2 (0x4UL << 13)

#define RCC_AHB1ENR_GPIOAEN (1UL << 0)
#define RCC_AHB1ENR_GPIOBEN (1UL << 1)
#define RCC_AHB1ENR_GPIOCEN (1UL << 2)
#define RCC_AHB1ENR_GPIODEN (1UL << 3)

#define RCC_APB1ENR_USART2EN (1UL << 17)
#define RCC_APB1ENR_USART3EN (1UL << 18)
#define RCC_APB1ENR_PWREN     (1UL << 28)

/* USART bit definitions */
#define USART_SR_TXE  (1UL << 7)
#define USART_SR_TC   (1UL << 6)
#define USART_SR_RXNE (1UL << 5)
#define USART_CR1_UE  (1UL << 13)
#define USART_CR1_TE  (1UL << 3)
#define USART_CR1_RE  (1UL << 2)

/* FLASH ACR */
#define FLASH_ACR_LATENCY_5WS 0x5UL
#define FLASH_ACR_PRFTEN      (1UL << 8)
#define FLASH_ACR_ICEN        (1UL << 9)
#define FLASH_ACR_DCEN        (1UL << 10)

/* PWR CR */
#define PWR_CR_VOS_MASK (0x3UL << 14)
#define PWR_CR_VOS_SCALE1 (0x3UL << 14)
