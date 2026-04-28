/*****************************************************************************

  Copyright (c) 2022-2024 Texas Instruments Incorporated

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:

  1) Redistributions of source code must retain the above copyright notice,
     this list of conditions and the following disclaimer.

  2) Redistributions in binary form must reproduce the above copyright notice,
     this list of conditions and the following disclaimer in the documentation
     and/or other materials provided with the distribution.

  3) Neither the name of the copyright holder nor the names of its
     contributors may be used to endorse or promote products derived from this
     software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
  POSSIBILITY OF SUCH DAMAGE.

*****************************************************************************/

/* Retain section containing the interrupt vector table                      */
--retain=resetVectors
/* Override default entry point.                                             */
--entry_point ResetISR
/* Allow main to take args                                                   */
--args 0x8
/* Suppress warnings and errors:                                             */
/* - 10063: Warning about entry point not being _c_int00                     */
/* - 16011, 16012: 8-byte alignment errors. Observed when linking in object  */
/*   files compiled using Keil (ARM compiler)                                */
--diag_suppress=10063,16011,16012

/* The following command line options are set as part of the CCS project.    */
/* If you are building using the command line, or for some reason want to    */
/* define them here, you can uncomment and modify these lines as needed.     */
/* If you are using CCS for building, it is probably better to make any such */
/* modifications in your CCS project and leave this file alone.              */
/*                                                                           */
/* --heap_size=0                                                             */
/* --stack_size=256                                                          */

/* The starting address of the application. The first flash sector (4KB bytes)
 * and the first few bytes of the second flash sector is reserved for metadata
 * used by the bootloader. The interrupt vectors must be 512B aligned so the
 * vector table must be at 0x10000000 + 0x1000 + 0x200 = 0x10001200
 * TODO: Incorrect alignment of 256B resulting in 0x10001100 is currently used,
 * to match what the bootloader currently expects. This needs to be fixed when
 * the bootloader uses the correct alignment. See LPRFXXWARE-943.
 */
#define FLASH_BASE              0x10001100
#define FLASH_SIZE              0x0003EF00
#define CRAM_BASE               0x00000000
#define CRAM_SIZE               0x00008000
#define DRAM_BASE               0x28000000
#define DRAM_SIZE               0x00030000 /* (Static only) DRAM1: 128K + DRAM2: 64K */

MEMORY
{
    /* Application stored in and executes from external flash */
    FLASH      (RX)  : origin = FLASH_BASE, length = FLASH_SIZE
    /* Code RAM */
    CRAM       (RWX) : origin = CRAM_BASE,  length = CRAM_SIZE
    /* Data RAM */
    DRAM       (RWX) : origin = DRAM_BASE,  length = DRAM_SIZE 

    /* Other memory regions */
    SLAVES_API (RW)  : origin = 0x45602000, length = 0x0000001F
    DB_MEM     (RW)  : origin = 0x45A80000, length = 0x0000FFFF
    PHY_CTX    (RW)  : origin = 0x45900000, length = 0x00010000
    PHY_SCR    (RW)  : origin = 0x45910000, length = 0x00004800
    CPERAM     (RWX) : origin = 0x45C00000, length = 0x00010000 /* 64 K PROGRAM MEMORY  */
    MCERAM     (RWX) : origin = 0x45C80000, length = 0x00001000 /* 4 K PROGRAM MEMORY   */
    RFERAM     (RWX) : origin = 0x45CA0000, length = 0x00001000 /* 4 K PROGRAM MEMORY   */
    MDMRAM     (RWX) : origin = 0x45CC0000, length = 0x00000100 /* 256 B PROGRAM MEMORY */
}

SECTIONS
{
    /* Flash */
    .resetVecs      :   > FLASH_BASE
    .text           :   > FLASH PALIGN(4)
    .const          :   > FLASH PALIGN(4)
    .constdata      :   > FLASH PALIGN(4)
    .rodata         :   > FLASH PALIGN(4)
    .binit          :   > FLASH PALIGN(4)
    .cinit          :   > FLASH PALIGN(4)
    .pinit          :   > FLASH PALIGN(4)
    .init_array     :   > FLASH PALIGN(4)
    .emb_text       :   > FLASH PALIGN(4)

    /* Code RAM */
    .ramVecs        :   > CRAM, type = NOLOAD, ALIGN(512)
    .TI.ramfunc : {} load=FLASH, run=CRAM, table(BINIT)
    .cram           :   > CRAM PALIGN(4)

    /* Data RAM */
    .data           :   > DRAM
    .bss            :   > DRAM
    .sysmem         :   > DRAM
    .stack          :   > DRAM (HIGH)
    .nonretenvar    :   > DRAM
    .TI.noinit      :   > DRAM

    .cio            :   > DRAM
    .sysmem         :   > DRAM
    .ARM.exidx      :   > DRAM
    .vtable         :   > DRAM
    .args           :   > DRAM

    /* Other meomory regions */
    .slave_if       :   > SLAVES_API
    .db_mem         :   > DB_MEM
    .ctx_ull        :   > PHY_CTX
    .scr_ull        :   > PHY_SCR
    .cperam         :   > CPERAM /* CPE CODE */
    .rferam         :   > RFERAM /* RFE CODE */
    .mceram         :   > MCERAM /* MCE CODE */
    .mdmram         :   > MDMRAM /* MDM CODE */
}

/* Create global constant that points to top of stack */
/* CCS: Change stack size under Project Properties    */
__STACK_TOP = __stack + __STACK_SIZE;
