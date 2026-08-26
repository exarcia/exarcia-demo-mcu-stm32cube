.syntax unified
.cpu cortex-m4
.fpu fpv4-sp-d16
.thumb

.global g_pfnVectors
.global Default_Handler

.word _sidata
.word _sdata
.word _edata
.word _sbss
.word _ebss

.section .text.Reset_Handler
.weak Reset_Handler
.type Reset_Handler, %function
Reset_Handler:
    ldr r0, =_estack
    mov sp, r0

    ldr r0, =_sdata
    ldr r1, =_edata
    ldr r2, =_sidata
    movs r3, #0
    b LoopCopyDataInit
CopyDataInit:
    ldr r4, [r2, r3]
    str r4, [r0, r3]
    adds r3, r3, #4
LoopCopyDataInit:
    adds r4, r0, r3
    cmp r4, r1
    bcc CopyDataInit

    ldr r2, =_sbss
    ldr r4, =_ebss
    movs r3, #0
    b LoopFillZerobss
FillZerobss:
    str r3, [r2]
    adds r2, r2, #4
LoopFillZerobss:
    cmp r2, r4
    bcc FillZerobss

    bl SystemInit
    bl main
    b .

.size Reset_Handler, .-Reset_Handler

.section .isr_vector, "a", %progbits
.type g_pfnVectors, %object
g_pfnVectors:
    .word _estack
    .word Reset_Handler
    .word NMI_Handler
    .word HardFault_Handler
    .word MemManage_Handler
    .word BusFault_Handler
    .word UsageFault_Handler
    .word 0
    .word 0
    .word 0
    .word 0
    .word SVC_Handler
    .word DebugMon_Handler
    .word 0
    .word PendSV_Handler
    .word SysTick_Handler

    .word WWDG_IRQHandler
    .word PVD_IRQHandler
    .word TAMP_STAMP_IRQHandler
    .word RTC_WKUP_IRQHandler
    .word FLASH_IRQHandler
    .word RCC_IRQHandler
    .word EXTI0_IRQHandler
    .word EXTI1_IRQHandler
    .word EXTI2_IRQHandler
    .word EXTI3_IRQHandler
    .word EXTI4_IRQHandler
    .word DMA1_Stream0_IRQHandler
    .word DMA1_Stream1_IRQHandler
    .word DMA1_Stream2_IRQHandler
    .word DMA1_Stream3_IRQHandler
    .word DMA1_Stream4_IRQHandler
    .word DMA1_Stream5_IRQHandler
    .word DMA1_Stream6_IRQHandler
    .word ADC_IRQHandler

.weak NMI_Handler
NMI_Handler: b .
.weak HardFault_Handler
HardFault_Handler: b .
.weak MemManage_Handler
MemManage_Handler: b .
.weak BusFault_Handler
BusFault_Handler: b .
.weak UsageFault_Handler
UsageFault_Handler: b .
.weak SVC_Handler
SVC_Handler: b .
.weak DebugMon_Handler
DebugMon_Handler: b .
.weak PendSV_Handler
PendSV_Handler: b .
.weak SysTick_Handler
SysTick_Handler: b .

.weak WWDG_IRQHandler
WWDG_IRQHandler: b .
.weak PVD_IRQHandler
PVD_IRQHandler: b .
.weak TAMP_STAMP_IRQHandler
TAMP_STAMP_IRQHandler: b .
.weak RTC_WKUP_IRQHandler
RTC_WKUP_IRQHandler: b .
.weak FLASH_IRQHandler
FLASH_IRQHandler: b .
.weak RCC_IRQHandler
RCC_IRQHandler: b .
.weak EXTI0_IRQHandler
EXTI0_IRQHandler: b .
.weak EXTI1_IRQHandler
EXTI1_IRQHandler: b .
.weak EXTI2_IRQHandler
EXTI2_IRQHandler: b .
.weak EXTI3_IRQHandler
EXTI3_IRQHandler: b .
.weak EXTI4_IRQHandler
EXTI4_IRQHandler: b .
.weak DMA1_Stream0_IRQHandler
DMA1_Stream0_IRQHandler: b .
.weak DMA1_Stream1_IRQHandler
DMA1_Stream1_IRQHandler: b .
.weak DMA1_Stream2_IRQHandler
DMA1_Stream2_IRQHandler: b .
.weak DMA1_Stream3_IRQHandler
DMA1_Stream3_IRQHandler: b .
.weak DMA1_Stream4_IRQHandler
DMA1_Stream4_IRQHandler: b .
.weak DMA1_Stream5_IRQHandler
DMA1_Stream5_IRQHandler: b .
.weak DMA1_Stream6_IRQHandler
DMA1_Stream6_IRQHandler: b .
.weak ADC_IRQHandler
ADC_IRQHandler: b .

.weak Default_Handler
Default_Handler: b .
