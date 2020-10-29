/*
 * Copyright (c) 2020, Texas Instruments Incorporated
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
#error "startup_msp432e4_ticlang.c: Unsupported compiler!"
#endif

#include <stdint.h>

#define __MSP432E411Y__
#include <ti/devices/msp432e4/inc/msp432.h>

#include <ti/devices/msp432e4/driverlib/sysctl.h>

#include <FreeRTOSConfig.h>

//*****************************************************************************
//
// Forward declaration of the default fault handlers.
//
//*****************************************************************************
void resetISR(void);
static void nmiISR(void);
static void faultISR(void);
static void defaultHandler(void);
static void busFaultHandler(void);

//*****************************************************************************
//
// External declaration for the reset handler that is to be called when the
// processor is started
//
//*****************************************************************************
extern void _c_int00(void);
extern void vPortSVCHandler(void);
extern void xPortPendSVHandler(void);
extern void xPortSysTickHandler(void);

//*****************************************************************************
//
// Linker variables that marks the top and bottom of the stack.
//
//*****************************************************************************
extern uint32_t __STACK_END;
extern void *__stack;

//*****************************************************************************
// The vector table.  Note that the proper constructs must be placed on this to
// ensure that it ends up at physical address 0x0000.0000 or at the start of
// the program if located at a start address other than 0.
//
//*****************************************************************************
__attribute__ ((section(".intvecs"))) __attribute__ ((used))
static void (* const resetVectors[])(void) =
{
    (void (*)(void))((uint32_t)&__STACK_END),
                                         // The initial stack pointer
    resetISR,                            // The reset handler
    nmiISR,                              // The NMI handler
    faultISR,                            // The hard fault handler
    defaultHandler,                      // The MPU fault handler
    busFaultHandler,                     // The bus fault handler
    defaultHandler,                      // The usage fault handler
    0,                                   // Reserved
    0,                                   // Reserved
    0,                                   // Reserved
    0,                                   // Reserved
    vPortSVCHandler,                     // SVCall handler
    defaultHandler,                      // Debug monitor handler
    0,                                   // Reserved
    xPortPendSVHandler,                  // The PendSV handler
    xPortSysTickHandler,                 // The SysTick handler
    defaultHandler,                      // GPIO Port A
    defaultHandler,                      // GPIO Port B
    defaultHandler,                      // GPIO Port C
    defaultHandler,                      // GPIO Port D
    defaultHandler,                      // GPIO Port E
    defaultHandler,                      // UART0 Rx and Tx
    defaultHandler,                      // UART1 Rx and Tx
    defaultHandler,                      // SSI0 Rx and Tx
    defaultHandler,                      // I2C0 Master and Slave
    defaultHandler,                      // PWM Fault
    defaultHandler,                      // PWM Generator 0
    defaultHandler,                      // PWM Generator 1
    defaultHandler,                      // PWM Generator 2
    defaultHandler,                      // Quadrature Encoder 0
    defaultHandler,                      // ADC Sequence 0
    defaultHandler,                      // ADC Sequence 1
    defaultHandler,                      // ADC Sequence 2
    defaultHandler,                      // ADC Sequence 3
    defaultHandler,                      // Watchdog timer
    defaultHandler,                      // Timer 0 subtimer A
    defaultHandler,                      // Timer 0 subtimer B
    defaultHandler,                      // Timer 1 subtimer A
    defaultHandler,                      // Timer 1 subtimer B
    defaultHandler,                      // Timer 2 subtimer A
    defaultHandler,                      // Timer 2 subtimer B
    defaultHandler,                      // Analog Comparator 0
    defaultHandler,                      // Analog Comparator 1
    defaultHandler,                      // Analog Comparator 2
    defaultHandler,                      // System Control (PLL, OSC, BO)
    defaultHandler,                      // FLASH Control
    defaultHandler,                      // GPIO Port F
    defaultHandler,                      // GPIO Port G
    defaultHandler,                      // GPIO Port H
    defaultHandler,                      // UART2 Rx and Tx
    defaultHandler,                      // SSI1 Rx and Tx
    defaultHandler,                      // Timer 3 subtimer A
    defaultHandler,                      // Timer 3 subtimer B
    defaultHandler,                      // I2C1 Master and Slave
    defaultHandler,                      // CAN0
    defaultHandler,                      // CAN1
    defaultHandler,                      // Ethernet
    defaultHandler,                      // Hibernate
    defaultHandler,                      // USB0
    defaultHandler,                      // PWM Generator 3
    defaultHandler,                      // uDMA Software Transfer
    defaultHandler,                      // uDMA Error
    defaultHandler,                      // ADC1 Sequence 0
    defaultHandler,                      // ADC1 Sequence 1
    defaultHandler,                      // ADC1 Sequence 2
    defaultHandler,                      // ADC1 Sequence 3
    defaultHandler,                      // External Bus Interface 0
    defaultHandler,                      // GPIO Port J
    defaultHandler,                      // GPIO Port K
    defaultHandler,                      // GPIO Port L
    defaultHandler,                      // SSI2 Rx and Tx
    defaultHandler,                      // SSI3 Rx and Tx
    defaultHandler,                      // UART3 Rx and Tx
    defaultHandler,                      // UART4 Rx and Tx
    defaultHandler,                      // UART5 Rx and Tx
    defaultHandler,                      // UART6 Rx and Tx
    defaultHandler,                      // UART7 Rx and Tx
    defaultHandler,                      // I2C2 Master and Slave
    defaultHandler,                      // I2C3 Master and Slave
    defaultHandler,                      // Timer 4 subtimer A
    defaultHandler,                      // Timer 4 subtimer B
    defaultHandler,                      // Timer 5 subtimer A
    defaultHandler,                      // Timer 5 subtimer B
    defaultHandler,                      // FPU
    0,                                   // Reserved
    0,                                   // Reserved
    defaultHandler,                      // I2C4 Master and Slave
    defaultHandler,                      // I2C5 Master and Slave
    defaultHandler,                      // GPIO Port M
    defaultHandler,                      // GPIO Port N
    0,                                   // Reserved
    defaultHandler,                      // Tamper
    defaultHandler,                      // GPIO Port P (Summary or P0)
    defaultHandler,                      // GPIO Port P1
    defaultHandler,                      // GPIO Port P2
    defaultHandler,                      // GPIO Port P3
    defaultHandler,                      // GPIO Port P4
    defaultHandler,                      // GPIO Port P5
    defaultHandler,                      // GPIO Port P6
    defaultHandler,                      // GPIO Port P7
    defaultHandler,                      // GPIO Port Q (Summary or Q0)
    defaultHandler,                      // GPIO Port Q1
    defaultHandler,                      // GPIO Port Q2
    defaultHandler,                      // GPIO Port Q3
    defaultHandler,                      // GPIO Port Q4
    defaultHandler,                      // GPIO Port Q5
    defaultHandler,                      // GPIO Port Q6
    defaultHandler,                      // GPIO Port Q7
    defaultHandler,                      // GPIO Port R
    defaultHandler,                      // GPIO Port S
    defaultHandler,                      // SHA/MD5 0
    defaultHandler,                      // AES 0
    defaultHandler,                      // DES3DES 0
    defaultHandler,                      // LCD Controller 0
    defaultHandler,                      // Timer 6 subtimer A
    defaultHandler,                      // Timer 6 subtimer B
    defaultHandler,                      // Timer 7 subtimer A
    defaultHandler,                      // Timer 7 subtimer B
    defaultHandler,                      // I2C6 Master and Slave
    defaultHandler,                      // I2C7 Master and Slave
    defaultHandler,                      // HIM Scan Matrix Keyboard 0
    defaultHandler,                      // One Wire 0
    defaultHandler,                      // HIM PS/2 0
    defaultHandler,                      // HIM LED Sequencer 0
    defaultHandler,                      // HIM Consumer IR 0
    defaultHandler,                      // I2C8 Master and Slave
    defaultHandler,                      // I2C9 Master and Slave
    defaultHandler                       // GPIO Port T
};

//*****************************************************************************
//
// This function is called at reset entry early in the boot sequence.
//
//*****************************************************************************
void localProgramStart(void)
{
    unsigned long *vtor = (unsigned long *)0xE000ED08;
    uint32_t newBasePri;

    /* Disable interrupts */
    __asm volatile (
      "mov %0, %1 \n\t"
      "msr basepri, %0 \n\t"
      "isb \n\t"
      "dsb \n\t"
      :"=r" (newBasePri) : "i" (configMAX_SYSCALL_INTERRUPT_PRIORITY) : "memory"
    );

#if configENABLE_ISR_STACK_INIT
    /* Initialize ISR stack to known value for Runtime Object View */
    register uint32_t *top = (uint32_t *)&__stack;
    register uint32_t *end = (uint32_t *)&newBasePri;
    while (top < end) {
        *top++ = (uint32_t)0xa5a5a5a5;
    }
#endif

    /* Set the frequency to 120MHz */
    SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ |
                        SYSCTL_OSC_MAIN |
                        SYSCTL_USE_PLL |
                        SYSCTL_CFG_VCO_480),
                        configCPU_CLOCK_HZ);

    /*
     * Set vector table base to point to above vectors in Flash; during
     * driverlib interrupt initialization this table will be copied to RAM
     */
    *vtor = (unsigned long)&resetVectors[0];

    //
    // Jump to the CCS C initialization routine.  This will enable the
    // floating-point unit as well, so that does not need to be done here.
    //
    __asm("    .global _c_int00\n"
          "    b.w     _c_int00");
}

//*****************************************************************************
//
// This is the code that gets called when the processor first starts execution
// following a reset event.  Only the absolutely necessary set is performed,
// after which the application supplied entry() routine is called.  Any fancy
// actions (such as making decisions based on the reset cause register, and
// resetting the bits in that register) are left solely in the hands of the
// application.
//
//*****************************************************************************
__attribute__ ((used))
void resetISR(void)
{
    /*
     *  Some debuggers do not load the stack pointer from the reset vector.
     *  This code ensures that the stack pointer is initialized.
     */
    __asm__ __volatile__ (
        " movw r0, #:lower16:resetVectors\n"
        " movt r0, #:upper16:resetVectors\n"
        " ldr r0, [r0]\n"
        " mov sp, r0\n"
        " b localProgramStart"
        );
}

//*****************************************************************************
//
// This is the code that gets called when the processor receives a NMI.  This
// simply enters an infinite loop, preserving the system state for examination
// by a debugger.
//
//*****************************************************************************
static void
nmiISR(void)
{
    /* Enter an infinite loop. */
    while(1)
    {
    }
}

//*****************************************************************************
//
// This is the code that gets called when the processor receives a fault
// interrupt.  This simply enters an infinite loop, preserving the system state
// for examination by a debugger.
//
//*****************************************************************************
static void
faultISR(void)
{
    /* Enter an infinite loop. */
    while(1)
    {
    }
}

//*****************************************************************************
//
// This is the code that gets called when the processor receives an unexpected
// interrupt.  This simply enters an infinite loop, preserving the system state
// for examination by a debugger.
//
//*****************************************************************************

static void
busFaultHandler(void)
{
    /* Enter an infinite loop. */
    while(1)
    {
    }
}

//*****************************************************************************
//
// This is the code that gets called when the processor receives an unexpected
// interrupt.  This simply enters an infinite loop, preserving the system state
// for examination by a debugger.
//
//*****************************************************************************
static void
defaultHandler(void)
{
    /* Enter an infinite loop. */
    while(1)
    {
    }
}
