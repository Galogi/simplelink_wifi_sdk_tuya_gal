/******************************************************************************
 *  Filename:       startup_gcc.c
 *
 *  Description:    Startup code for CC35XX device family for use with GCC.
 *
 *  Copyright (c) 2024-2025 Texas Instruments Incorporated
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *
 *  1) Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *
 *  2) Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the documentation
 *     and/or other materials provided with the distribution.
 *
 *  3) Neither the name of the copyright holder nor the names of its
 *     contributors may be used to endorse or promote products derived from this
 *     software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 *
 ******************************************************************************/

//*****************************************************************************
//
// Check if compiler is GNU
//
//*****************************************************************************
#if !(defined(__GNUC__))
    #error "startup_gcc.c: Unsupported compiler!"
#endif

#include "../inc/hw_types.h"
#include "../driverlib/setup.h"

//*****************************************************************************
//
// Macro for weak symbol aliasing
//
//*****************************************************************************
#define WEAK_ALIAS(x) __attribute__((weak, alias(#x)))

//*****************************************************************************
//
//! Forward declaration of the reset ISR and the default fault handlers.
//
//*****************************************************************************
void ResetISR(void);
static void NmiISR(void);
static void FaultISR(void);
static void IntDefaultHandler(void);
extern int main(void);

//*****************************************************************************
//
// The following are constructs created by the linker, indicating where the
// the "data" and "bss" segments reside in memory.
//
//*****************************************************************************
extern uint32_t _ldata;
extern uint32_t _data;
extern uint32_t _edata;
extern uint32_t __bss_start__;
extern uint32_t __bss_end__;
extern uint32_t _estack;

//*****************************************************************************
//
//! The vector table. Note that the proper constructs must be placed on this to
//! ensure that it ends up at physical address 0x10001100 (see LPRFXXWARE-943)
//! or at the start of the program if located at a start address other than
//! 0x10001100.
//! The first flash sector (4KB bytes) and the first few bytes of the second
//! flash sector is reserved for metadata used by the bootloader.
//! The interrupt vectors must be 512B aligned so the vector table should
//! have been placed at 0x10000000 + 0x1000 + 0x200 = 0x10001200 but due to
//! the booloader requirement, incorrect alignment of 256B resulting in
//! 0x10001100, is currently used.
//! This needs to be fixed when the bootloader uses the correct alignment.
//
//*****************************************************************************
__attribute__((section(".resetVecs"), used)) void (*const resetVectors[])(void) = {
    (void (*)(void))((unsigned long)&_estack),
    //  0 The initial stack pointer
    ResetISR,          // The reset handler
    NmiISR,            // The NMI handler
    FaultISR,          // The hard fault handler
    IntDefaultHandler, // The MPU fault handler
    IntDefaultHandler, // The bus fault handler
    IntDefaultHandler, // The usage fault handler
    IntDefaultHandler, // Reserved
    IntDefaultHandler, // Reserved
    IntDefaultHandler, // Reserved
    IntDefaultHandler, // Reserved
    IntDefaultHandler, // SVCall handler
    IntDefaultHandler, // Debug monitor handler
    IntDefaultHandler, // Reserved
    IntDefaultHandler, // The PendSV handler
    IntDefaultHandler, // The SysTick handler
    // External interrupts
    IntDefaultHandler, // 16 INT_SP_UART_0_INT_REQ
    IntDefaultHandler, // 17 INT_SP_UART_1_INT_REQ
    IntDefaultHandler, // 18 INT_SP_I2C_0_INTREQ
    IntDefaultHandler, // 19 INT_SP_I2C_1_INTREQ
    IntDefaultHandler, // 20 INT_SP_SPI_0_EVT_REQ
    IntDefaultHandler, // 21 INT_SP_SPI_1_EVT_REQ
    IntDefaultHandler, // 22 INT_GPTIMER_0_EVT_CPU_IRQ
    IntDefaultHandler, // 23 INT_GPTIMER_1_EVT_CPU_IRQ
    IntDefaultHandler, // 24 INT_SP_UART_2_INT_REQ
    IntDefaultHandler, // 25 INT_I2S_IRQ_REQ
    IntDefaultHandler, // 26 INT_EVT_PDM_EVENT_REQ
    IntDefaultHandler, // 27 INT_EVT_SWINT0_REQ
    IntDefaultHandler, // 28 INT_EVT_SWINT1_REQ
    IntDefaultHandler, // 29 INT_EVT_SDMMC_PUB_REQ
    IntDefaultHandler, // 30 INT_SDIO_CARD_IRQ_REQ
    IntDefaultHandler, // 31 INT_ULL_USC_ULPADCHP_PUB_EVT0_REQ
    IntDefaultHandler, // 32 INT_NON_SECURED_GPIO_IRQ_EVT_IND_OUT
    IntDefaultHandler, // 33 INT_SECURED_GPIO_IRQ_EVT_IND_OUT
    IntDefaultHandler, // 34 INT_OSPR_HSM_HOST_0_SEC_IRQ
    IntDefaultHandler, // 35 INT_OSPR_HSM_HOST_0_IRQ
    IntDefaultHandler, // 36 INT_OSPR_HSM_HOST_1_IRQ
    IntDefaultHandler, // 37 INT_SVT_EVT_COMBINED_SYSTIM_OUT_IRQ
    IntDefaultHandler, // 38 INT_SVT_EVT_SYSTIMER_BIT_OUT_IRQ
    IntDefaultHandler, // 39 INT_SVT_EVT_SYSTIMER_OUT_0_IRQ
    IntDefaultHandler, // 40 INT_SVT_EVT_SYSTIMER_OUT_1_IRQ
    IntDefaultHandler, // 41 INT_NON_SECURED_DMA_IRQ_EVT_IND_OUT
    IntDefaultHandler, // 42 INT_SECURED_DMA_IRQ_EVT_IND_OUT
    IntDefaultHandler, // 43 INT_NON_SECURED_DOORBELL_IRQ_EVT_IND_OUT
    IntDefaultHandler, // 44 INT_SECURED_DOORBELL_IRQ_EVT_IND_OUT
    IntDefaultHandler, // 45 INT_ICACHE_ERR_IRQ
    IntDefaultHandler, // 46 INT_OSPI_IRQ
    IntDefaultHandler, // 47 INT_OTFDE_IRQ
    IntDefaultHandler, // 48 INT_XIP_ARB_IRQ
    IntDefaultHandler, // 49 INT_XIP_DMA_SEC_IRQ
    IntDefaultHandler, // 50 INT_XIP_DMA_NONSEC_IRQ
    IntDefaultHandler, // 51 INT_SW_INTERRUPT_0
    IntDefaultHandler, // 52 INT_SW_INTERRUPT_1
    IntDefaultHandler, // 53 INT_SW_INTERRUPT_2
    IntDefaultHandler, // 54 INT_SW_INTERRUPT_3
    IntDefaultHandler, // 55 INT_SW_INTERRUPT_4
    IntDefaultHandler, // 56 INT_SW_INTERRUPT_5
    IntDefaultHandler, // 57 INT_SW_INTERRUPT_6
    IntDefaultHandler, // 58 INT_SW_INTERRUPT_7
    IntDefaultHandler, // 59 INT_PRCM_IRQ
    IntDefaultHandler, // 60 INT_OCLA_IRQ
    IntDefaultHandler, // 61 INT_HIF_FIFO_IRQ
    IntDefaultHandler, // 62 INT_HOST_ELP_TMR_WAKEUP_REQ
    IntDefaultHandler, // 63 INT_NAB_HOST_IRQ
    IntDefaultHandler, // 64 INT_BLE_RFC_GPO_8_IRQ
    IntDefaultHandler, // 65 INT_RTC_EVENT_IRQ
    IntDefaultHandler, // 66 INT_DEBUGSS_HOST_CSYSPWRUPREQ
    IntDefaultHandler, // 67 INT_DEBUGSS_HOST_FORCEACTIVE
    IntDefaultHandler, // 68 INT_SECURED_ERROR_IRQ_EVT_IND_OUT
};

//*****************************************************************************
//
//! This is the code that gets called when the processor first starts execution
//! following a reset event. Only the absolutely necessary set is performed,
//! after which the application supplied entry() routine is called. Any fancy
//! actions (such as making decisions based on the reset cause register, and
//! resetting the bits in that register) are left solely in the hands of the
//! application.
//
//*****************************************************************************
void ResetISR(void)
{
    uint32_t *pSrc;
    uint32_t *pDest;
    uint32_t *bs;
    uint32_t *be;

    // Final trim of device
    SetupTrimDevice();

    // Copy the data segment initializers from FLASH to SRAM.
    pSrc = &_ldata;
    for (pDest = &_data; pDest < &_edata;)
    {
        *pDest++ = *pSrc++;
    }

    // Initialize .bss to zero
    bs = &__bss_start__;
    be = &__bss_end__;
    while (bs < be)
    {
        *bs = 0;
        bs++;
    }

    // Enable the FPU
    // CPACR is located at address 0xE000ED88
    // Set bits 20-23 in CPACR to enable CP10 and CP11 coprocessors
    __asm("    ldr.w   r0, =0xE000ED88\n"
          "    ldr     r1, [r0]\n"
          "    orr     r1, r1, #(0xF << 20)\n"
          "    str     r1, [r0]\n");

    // Call the application's entry point.
    main();

    // If we ever return signal Error
    FaultISR();
}

//*****************************************************************************
//
//! This is the code that gets called when the processor receives a NMI. This
//! simply enters an infinite loop, preserving the system state for examination
//! by a debugger.
//
//*****************************************************************************
static void NmiISR(void)
{
    // Enter an infinite loop.
    while (1) {}
}

//*****************************************************************************
//
//! This is the code that gets called when the processor receives a fault
//! interrupt. This simply enters an infinite loop, preserving the system state
//! for examination by a debugger.
//
//*****************************************************************************
static void FaultISR(void)
{
    // Enter an infinite loop.
    while (1) {}
}

//*****************************************************************************
//
//! This is the code that gets called when the processor receives an unexpected
//! interrupt. This simply enters an infinite loop, preserving the system state
//! for examination by a debugger.
//
//*****************************************************************************
static void IntDefaultHandler(void)
{
    // Enter an infinite loop.
    while (1) {}
}
