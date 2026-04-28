/*
 * Copyright (c) 2022-2025, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#if !(defined(__clang__))
    #error "startup_cc35xx_ticlang.c: Unsupported compiler!"
#endif

#include <stdint.h>
#include <string.h>

#include <ti/devices/DeviceFamily.h>
#include DeviceFamily_constructPath(driverlib/setup.h)

#include <FreeRTOSConfig.h>

//*****************************************************************************
//
// The entry point for the application startup code.
//
//*****************************************************************************
extern void _c_int00(void);
void resetISR(void);

//*****************************************************************************
//
// Linker variable that marks the bottom of the stack.
//
//*****************************************************************************
extern unsigned long __STACK_END;

//*****************************************************************************
//
// Reset vectors defined and populated in SysConfig.
//
//*****************************************************************************
extern void (*const resetVectors[])(void);

//*****************************************************************************
//
// This function is called at reset entry early in the boot sequence.
//
//*****************************************************************************
void localProgramStart(void)
{
    unsigned long *vtor = (unsigned long *)0xE000ED08;
    uint32_t newBasePri;

    /* Do final trim of device */
    SetupTrimDevice();

    /* Disable interrupts */
    __asm volatile(" mov %0, %1 \n"
                   " msr basepri, %0 \n"
                   " isb \n"
                   " dsb \n"
                   : "=r"(newBasePri)
                   : "i"(configMAX_SYSCALL_INTERRUPT_PRIORITY)
                   : "memory");

#if configENABLE_ISR_STACK_INIT
    /* Initialize ISR stack to known value for Runtime Object View.
     * Inline assembly is used to prevent optimizations such as replacing a loop
     * with a memset() call, which might result in pushes to the stack which
     * would end up being corrupted.
     * It is assumed that __stack is word aligned, that is, &__stack is a
     * multiple of 4.
     */
    __asm__ __volatile__(
        /* Load address of the top of the stack to R0. R0 will be used as a
         * moving pointer to where to write 0xa5a5a5a5 to next.
         */
        " movw r0, #:lower16:__stack \n"
        " movt r0, #:upper16:__stack \n"
        /* Load 0xa5a5a5a5 to R1 */
        " movw r1, #0xa5a5 \n"
        " movt r1, #0xa5a5 \n"
        /* Loop while R0 < SP */
        " b compare \n"
        " loop: \n"
        /* Write value of R1 (0xa5a5a5a5) to the address pointed to by R0 */
        " str r1, [r0] \n"
        /* Increment R0 to the next word address */
        " add r0, r0, #4 \n"
        /* Continue looping if R0 < SP */
        " compare: \n"
        " cmp r0, sp \n"
        /* Branch to the start of the loop if R0 is unsigned lower than SP */
        " blo loop \n"
        : /* Output */
        : /* Input */
        : /* Clobbers */ "r0", "r1", "cc", "memory");
#endif

    /*
     * Set vector table base to point to above vectors. This is necessary to
     * ensure that the application is using the correct vector table when using
     * a debugger.
     */
    *vtor = (unsigned long)&resetVectors[0];

    /* jump to the C initialization routine. */
    __asm(" .global _c_int00\n"
          " b.w     _c_int00");
}

//*****************************************************************************
//
// This is the code that gets called when the processor first starts execution
// following a reset event.  Only the absolutely necessary steps are performed,
// after which the application supplied entry routine is called.
//
//*****************************************************************************
void resetISR(void)
{
    /*
     * Set stack pointer based on the stack value stored in the vector table.
     * This is necessary to ensure that the application is using the correct
     * stack when using a debugger since a reset within the debugger will
     * load the stack pointer from the bootloader's vector table at address '0'.
     */
    __asm__ __volatile__(" movw r0, #:lower16:resetVectors\n"
                         " movt r0, #:upper16:resetVectors\n"
                         " ldr r0, [r0]\n"
                         " mov sp, r0\n"
                         " b localProgramStart");
}
