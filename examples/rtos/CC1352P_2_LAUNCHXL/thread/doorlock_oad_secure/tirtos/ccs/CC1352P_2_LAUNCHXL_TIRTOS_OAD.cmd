/*
 * Copyright (c) 2017, Texas Instruments Incorporated
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

/*
 *  ======== CC13X2P_2_LAUNCHXL.cmd ========
 */

--stack_size=1024   /* C stack is also used for ISR stack */

HEAPSIZE = 0x1000;  /* Size of heap buffer used by HeapMem */

/* Retain interrupt vector table variable                                    */
--retain=g_pfnVectors
/* Override default entry point.                                             */
--entry_point ResetISR
/* Allow main() to take args                                                 */
--args 0x8
/* Suppress warnings and errors:                                             */
/* - 10063: Warning about entry point not being _c_int00                     */
/* - 16011, 16012: 8-byte alignment errors. Observed when linking in object  */
/*   files compiled using Keil (ARM compiler)                                */
--diag_suppress=10063,16011,16012

/* Suppress OAD specific warnings:
 * - 10325: Creating memory range for ".TI.bound:flashBuf
 * - 10229: TI-RTOS ROM data sections reference Flash constants
 */
--diag_suppress=10325,10229

/* The following command line options are set as part of the CCS project.    */
/* If you are building using the command line, or for some reason want to    */
/* define them here, you can uncomment and modify these lines as needed.     */
/* If you are using CCS for building, it is probably better to make any such */
/* modifications in your CCS project and leave this file alone.              */
/*                                                                           */
/* --heap_size=0                                                             */
/* --stack_size=256                                                          */
/* --library=rtsv7M3_T_le_eabi.lib                                           */

/* The starting address of the application.  Normally the interrupt vectors  */
/* must be located at the beginning of the application.                      */
#define FLASH_BASE              0x0
#define FLASH_SIZE              0x58000
#define RAM_BASE                0x20000000
#define RAM_SIZE                0x14000
#define GPRAM_BASE              0x11000000
#define GPRAM_SIZE              0x2000
#define FLASH_SECTOR_SIZE       0x2000

/*
 * page N-1 = CCFG & BIM as one image.
 */
#define FLASH_BIM_CCFG_BASE          ((FLASH_BASE + FLASH_SIZE) - FLASH_SECTOR_SIZE)
#define FLASH_BIM_CCFG_SIZE          (FLASH_SECTOR_SIZE)

/* System memory map */

MEMORY
{
    /* Application stored in and executes from internal flash */
    FLASH_IMG_HDR   : origin = FLASH_BASE         , length = 0x100
    FLASH (RX)      : origin = FLASH_BASE + 0x100 , length = (FLASH_SIZE - (3 * FLASH_SECTOR_SIZE) - 0x100)
    FLASH_BIM_CCFG (RX)  : origin = FLASH_BIM_CCFG_BASE , length = FLASH_BIM_CCFG_SIZE
    /* Application uses internal RAM for data */
    SRAM (RWX) : origin = RAM_BASE, length = RAM_SIZE
    /* Application can use GPRAM region as RAM if cache is disabled in the CCFG
    (DEFAULT_CCFG_SIZE_AND_DIS_FLAGS.SET_CCFG_SIZE_AND_DIS_FLAGS_DIS_GPRAM = 0) */
    GPRAM (RWX): origin = GPRAM_BASE, length = GPRAM_SIZE
}

SECTIONS
{
    .oad_image_header : > FLASH_IMG_HDR

    .TI.ramfunc     : {} load=FLASH, run=SRAM, table(BINIT)

    GROUP > FLASH
    {
        .intvecs
        .text
        .const
        .constdata
        .rodata
        .binit
        .cinit
        .pinit
        .init_array
        .emb_text
        .ARM.exidx
        .ARM.extab
        .flash_end_address
    }

    .bim_oad_debug_image_bin   :   > FLASH_BIM_CCFG

    .vtable         :   > SRAM
    .vtable_ram     :   > SRAM
     vtable_ram     :   > SRAM
    .data           :   > SRAM
    .bss            :   > SRAM
    .sysmem         :   > SRAM
    .stack          :   > SRAM (HIGH)
    .nonretenvar    :   > SRAM
    /* Heap buffer used by HeapMem */
    .priheap   : {
        __primary_heap_start__ = .;
        . += HEAPSIZE;
        __primary_heap_end__ = .;
    } > SRAM align 8
    .gpram          :   > GPRAM
}
