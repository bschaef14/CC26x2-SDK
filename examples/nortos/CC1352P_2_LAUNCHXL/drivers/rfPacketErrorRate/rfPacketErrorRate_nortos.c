/*
 * Copyright (c) 2016-2018, Texas Instruments Incorporated
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

/***** Includes *****/
/* Standard C Libraries */
#include <stdlib.h>

/* TI Drivers */
#include <ti/display/Display.h>
#include <ti/drivers/PIN.h>
#include <ti/drivers/SPI.h>
#include <ti/drivers/UART.h>
#include <ti/drivers/rf/RF.h>
#include <ti/drivers/PIN.h>

/* Board Header files */
#include "ti_drivers_config.h"

/* Application specific Header files */
#include "menu.h"

#include <ti/devices/DeviceFamily.h>
#include DeviceFamily_constructPath(driverlib/cpu.h)

/***** Variable declarations *****/

/* Pin driver objects and handles */
PIN_Handle buttonPinHandle;
static PIN_State buttonPinState;

extern void menu_runTask();

/*
Application button pin configuration table.
Buttons interrupts are configured to trigger on falling edge.
*/
PIN_Config buttonPinTable[] =
{
    CONFIG_PIN_BTN1 | PIN_INPUT_EN | PIN_PULLUP | PIN_IRQ_NEGEDGE,
    CONFIG_PIN_BTN2 | PIN_INPUT_EN | PIN_PULLUP | PIN_IRQ_NEGEDGE,
    PIN_TERMINATE,
};

/*
Interrupt handler for the button pints.
*/
void buttonCallbackFunction(PIN_Handle handle, PIN_Id pinId)
{
    /* Simple debounce logic, only toggle if the button is still pushed (low) */
    CPUdelay((uint32_t)((48000000/3)*0.050f));
    if (!PIN_getInputValue(pinId))
    {
        if (PIN_getInputValue(CONFIG_PIN_BTN1))
        {
            menu_notifyButtonPressed(Button_Navigate);
        }
        else if (PIN_getInputValue(CONFIG_PIN_BTN2))
        {
            menu_notifyButtonPressed(Button_Select);
        }
    }
}

void mainThread(void *arg0)
{
    /* Initialize the radio */
    RF_Params rfParams;
    RF_Params_init(&rfParams);

    /* Initialize the UART and SPI for the display driver. */
    Display_init();
    UART_init();
    SPI_init();

    /* Open Button pins */
    buttonPinHandle = PIN_open(&buttonPinState, buttonPinTable);
    if (buttonPinHandle == NULL)
    {
        while(1);
    }

    /* Setup callback for button pins */
    PIN_Status status = PIN_registerIntCb(buttonPinHandle, &buttonCallbackFunction);
    if (status != PIN_SUCCESS)
    {
        while(1);
    }

    menu_init();

    /* Start task execution */

	menu_runTask();
}
