/* Minimal CMSIS Cortex-M4 Core header for this demo. */
#pragma once

#include <stdint.h>

typedef struct {
    volatile uint32_t CTRL;
    volatile uint32_t LOAD;
    volatile uint32_t VAL;
    volatile uint32_t CALIB;
} SysTick_Type;

typedef struct {
    volatile uint32_t ISER[8];
    uint32_t          RESERVED0[24];
    volatile uint32_t ICER[8];
    uint32_t          RESERVED1[24];
    volatile uint32_t ISPR[8];
    uint32_t          RESERVED2[24];
    volatile uint32_t ICPR[8];
    uint32_t          RESERVED3[24];
    volatile uint32_t IABR[8];
    uint32_t          RESERVED4[56];
    volatile uint8_t  IP[240];
    uint32_t          RESERVED5[644];
    volatile uint32_t STIR;
} NVIC_Type;

typedef struct {
    volatile uint32_t CPUID;
    volatile uint32_t ICSR;
    volatile uint32_t VTOR;
    volatile uint32_t AIRCR;
    volatile uint32_t SCR;
    volatile uint32_t CCR;
    volatile uint32_t SHPR[3];
    volatile uint32_t SHCSR;
    volatile uint32_t CFSR;
    volatile uint32_t HFSR;
    volatile uint32_t DFSR;
    volatile uint32_t MMFAR;
    volatile uint32_t BFAR;
    volatile uint32_t AFSR;
} SCB_Type;

#define SCS_BASE    (0xE000E000UL)
#define SysTick_BASE (SCS_BASE + 0x0010UL)
#define NVIC_BASE   (SCS_BASE + 0x0100UL)
#define SCB_BASE    (SCS_BASE + 0x0D00UL)

#define SysTick ((SysTick_Type *)SysTick_BASE)
#define NVIC    ((NVIC_Type *)NVIC_BASE)
#define SCB     ((SCB_Type *)SCB_BASE)

static inline void __enable_irq(void)  { __asm volatile ("cpsie i"); }
static inline void __disable_irq(void) { __asm volatile ("cpsid i"); }
static inline void __DSB(void)         { __asm volatile ("dsb 0xF":::"memory"); }
static inline void __ISB(void)         { __asm volatile ("isb 0xF":::"memory"); }
static inline void __NOP(void)         { __asm volatile ("nop"); }
static inline void __WFI(void)         { __asm volatile ("wfi"); }

static inline void NVIC_SetPriorityGrouping(uint32_t prio) {
    uint32_t reg = SCB->AIRCR;
    reg &= ~(0xFFFFUL << 16 | 0x7UL << 8);
    reg |= (0x5FAUL << 16) | ((prio & 0x7UL) << 8);
    SCB->AIRCR = reg;
}

static inline void NVIC_SetPriority(IRQn_Type IRQn, uint32_t prio) {
    if ((int32_t)IRQn >= 0) {
        NVIC->IP[(uint32_t)IRQn] = (uint8_t)((prio << (8 - __NVIC_PRIO_BITS)) & 0xFF);
    } else {
        SCB->SHPR[(((uint32_t)IRQn) & 0xFUL) - 4UL] =
            (uint8_t)((prio << (8 - __NVIC_PRIO_BITS)) & 0xFF);
    }
}

static inline void NVIC_EnableIRQ(IRQn_Type IRQn) {
    if ((int32_t)IRQn >= 0) {
        NVIC->ISER[(uint32_t)IRQn >> 5UL] = (1UL << ((uint32_t)IRQn & 0x1FUL));
    }
}

#define SysTick_LOAD_RELOAD_Msk (0x00FFFFFFUL)
#define SysTick_CTRL_CLKSOURCE_Msk (1UL << 2)
#define SysTick_CTRL_TICKINT_Msk   (1UL << 1)
#define SysTick_CTRL_ENABLE_Msk    (1UL)
