/******************************************************************************
*  @file      cc35xxx.lds
*  @brief     CC35XX linker configuration file for GNU toolchain
*             (256KB FLASH, 162KB RAM).
*
*  Copyright (c) 2024 Texas Instruments Incorporated
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

/* Entry Point */
ENTRY( ResetISR )

/* System memory map */
MEMORY
{
    /* Application is stored in and executes from external flash.
     * The first flash sector (4KB bytes) and the first few bytes of the second
     * flash sector is reserved for metadata used by the bootloader.
     * The interrupt vectors must be 512B aligned so the vector table must be
     * at 0x10000000 + 0x1000 + 0x200 = 0x10001200
     * TODO: Incorrect alignment of 256B resulting in 0x10001100 is currently used,
     * to match what the bootloader currently expects. This needs to be fixed when
     * the bootloader uses the correct alignment. See LPRFXXWARE-943.
     */
    FLASH (RX)       : ORIGIN = 0x10001100, LENGTH = 0x0003EF00
    /* Code RAM */
    CRAM (RWX)       : ORIGIN = 0x00000000, LENGTH = 0x00008000
    /* Data RAM */
    DRAM (RWX)       : ORIGIN = 0x28000000, LENGTH = 0x00030000 /* (Static only) DRAM1: 128K + DRAM2: 64K */
    /* Other memory regions */
    SLAVES_API (RW)  : ORIGIN = 0x45602000, LENGTH = 0x0000001F
    DB_MEM     (RW)  : ORIGIN = 0x45A80000, LENGTH = 0x0000FFFF
    PHY_CTX    (RW)  : ORIGIN = 0x45900000, LENGTH = 0x00010000
    PHY_SCR    (RW)  : ORIGIN = 0x45910000, LENGTH = 0x00004800
    CPERAM     (RWX) : ORIGIN = 0x45C00000, LENGTH = 0x00010000 /* 64 K PROGRAM MEMORY  */
    MCERAM     (RWX) : ORIGIN = 0x45C80000, LENGTH = 0x00001000 /* 4 K PROGRAM MEMORY   */
    RFERAM     (RWX) : ORIGIN = 0x45CA0000, LENGTH = 0x00001000 /* 4 K PROGRAM MEMORY   */
    MDMRAM     (RWX) : ORIGIN = 0x45CC0000, LENGTH = 0x00000100 /* 256 B PROGRAM MEMORY */
}

/* Highest address of the stack. Used in startup file. */
_estack = ORIGIN(DRAM) + LENGTH(DRAM); /* End of DRAM. */

/* Generate a link error if heap and stack does not fit into RAM. */
_Min_Heap_Size = 0;
_Min_Stack_Size = 0x100;


/* Section allocation in memory */
SECTIONS
{
    .text :
    {
        _text = .;
        KEEP(*(.resetVecs))
        *(.text*)
        *(.rodata*)
        *(.init)
        *(.fini*)
        *(.eh_frame*)
        _etext = .;
    } > FLASH = 0

   .ARM.exidx :
    {
        __exidx_start = .;
        *(.ARM.exidx* .gnu.linkonce.armexidx.*)
        __exidx_end = .;
    } > FLASH

    .ARM.extab :
    {
        *(.ARM.extab* .gnu.linkonce.armextab.*)
    } > FLASH

    .init_array :
    {
        _init_array = .;
        KEEP(*(SORT(.init_array.*)))
        KEEP(*(.init_array*))
        _einit_array = .;
    } > FLASH


    .data :
    {
        _data = .;
        *(.data*)
        . = ALIGN (4);
        *(.TI.ramfunc)
        . = ALIGN (4);
        _edata = .;
    } > DRAM AT > FLASH
    _ldata = LOADADDR(.data);

    .ramVecs (NOLOAD) : ALIGN(512)
    {
    	*(.ramVecs)
    } > CRAM

    .bss :
    {
        __bss_start__ = .;
        _bss = .;
        *(.bss*)
        *(COMMON)
        _ebss = .;
        __bss_end__ = .;
    } > DRAM

    /* User_heap_stack section, used to check that there is enough DRAM left */
    ._user_heap_stack :
    {
      . = ALIGN(4);
      . = . + _Min_Heap_Size;
      . = . + _Min_Stack_Size;
      . = ALIGN(4);
    } > DRAM
}
