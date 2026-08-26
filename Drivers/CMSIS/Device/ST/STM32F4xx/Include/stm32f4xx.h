#pragma once

#include <stdint.h>

#define __CM4_REV                  0x0001
#define __NVIC_PRIO_BITS           4
#define __Vendor_SysTickConfig     0
#define __MPU_PRESENT              1
#define __FPU_PRESENT              1

typedef enum { NonMaskableInt_IRQn = -14, HardFault_IRQn = -13,
               MemoryManagement_IRQn = -12, BusFault_IRQn = -11,
               UsageFault_IRQn = -10, SVCall_IRQn = -5,
               DebugMonitor_IRQn = -4, PendSV_IRQn = -2,
               SysTick_IRQn = -1 } IRQn_Type;

#include "core_cm4.h"
