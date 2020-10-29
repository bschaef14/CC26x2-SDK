/*
 * Copyright (c) 2015-2017, Texas Instruments Incorporated
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
 *  ======== empty_min.c ========
 */
/* XDCtools Header files */
#include <xdc/std.h>
#include <xdc/runtime/Assert.h>
#include <xdc/runtime/System.h>

/* BIOS Header files */
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>

/* TI-RTOS Header files */
#include <ti/drivers/PIN.h>
#include <ti/drivers/UART.h>

/* Board Header files */
#include "ti_drivers_config.h"

/* EasyLink API Header files */
#include "easylink/EasyLink.h"

/* AT Header files */
#include "at/platform/inc/AtTerm.h"
#include "at/AtProcess.h"
#include "at/AtParams.h"

/* Pin driver handle */
static PIN_Handle pinHandle;
static PIN_State pinState;

/*
 * Application LED pin configuration table:
 *   - All LEDs board LEDs are off.
 */
PIN_Config pinTable[] = {
    CONFIG_PIN_GLED | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL | PIN_DRVSTR_MAX,
    CONFIG_PIN_RLED | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL | PIN_DRVSTR_MAX,
    PIN_TERMINATE
};

/***** Defines *****/
#define atnpTaskStackSize       2048
#define atnpTaskPriority        2

/***** Variable declarations *****/
static Task_Params atnpTaskParams;
Task_Struct atnpTask;    /* not static so you can see in ROV */
static uint8_t atnpTaskStack[atnpTaskStackSize];

static void atnpFxn(UArg arg0, UArg arg1)
{
    AtTerm_init();
    AtParams_init(pinHandle, pinTable);

    AtTerm_sendString("Reset:");
    AtTerm_sendString(EASYLINK_API_VERSION);
    AtTerm_sendString("\r");

    /* Loop forever echoing */
    while (1)
    {
        AtProcess_processingLoop();
    }

}
/*
 *  ======== main ========
 */
int main(void)
{
    /* Call driver init functions. */
    Board_initGeneral();
    UART_init();

    Task_Params_init(&atnpTaskParams);
    atnpTaskParams.stackSize = atnpTaskStackSize;
    atnpTaskParams.priority = atnpTaskPriority;
    atnpTaskParams.stack = &atnpTaskStack;
    atnpTaskParams.arg0 = (UInt) 1000000;

    Task_construct(&atnpTask, atnpFxn, &atnpTaskParams, NULL);

    System_printf("AT NP Example Started.\n");
    /* SysMin will only print to the console when you call flush or exit */
    System_flush();

    /* Open LED pins */
    pinHandle = PIN_open(&pinState, pinTable);
    Assert_isTrue(pinHandle != NULL, NULL);


    /* Clear LED pins */
    PIN_setOutputValue(pinHandle, CONFIG_PIN_GLED, 0);
    PIN_setOutputValue(pinHandle, CONFIG_PIN_RLED, 0);

    /* Start BIOS */
    BIOS_start();

    return (0);
}
