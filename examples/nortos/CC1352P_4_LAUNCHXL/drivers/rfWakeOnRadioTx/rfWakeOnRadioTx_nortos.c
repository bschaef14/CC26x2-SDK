/*
 * Copyright (c) 2019, Texas Instruments Incorporated
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
#include <stdlib.h>

/* Drivers */
#include <ti/drivers/rf/RF.h>
#include <ti/drivers/Power.h>
#include <ti/drivers/PIN.h>

#include <ti/devices/DeviceFamily.h>
#include DeviceFamily_constructPath(driverlib/cpu.h)
#include DeviceFamily_constructPath(driverlib/interrupt.h)

/* Display */
#include <ti/display/Display.h>

/* Board Header files */
#include "ti_drivers_config.h"

/* RF settings */
#include <ti_radio_config.h>

/***** Defines *****/
/* Wake-on-Radio configuration */
#define WOR_WAKEUPS_PER_SECOND 2

/* TX number of random payload bytes */
#define PAYLOAD_LENGTH 30

/* WOR Example configuration defines */
#define WOR_PREAMBLE_TIME_RAT_TICKS(x) \
    ((uint32_t)(4000000*(1.0f/(x))))


/***** Prototypes *****/
static void initializeTxAdvCmdFromTxCmd(rfc_CMD_PROP_TX_ADV_t* RF_cmdPropTxAdv, rfc_CMD_PROP_TX_t* RF_cmdPropTx);

/***** Variable declarations *****/
/* TX packet payload (length +1 to fit length byte) and sequence number */
static uint8_t packet[PAYLOAD_LENGTH +1];
static uint16_t seqNumber;

/* RF driver objects and handles */
static RF_Object rfObject;
static RF_Handle rfHandle;

/* Pin driver objects and handles */
static PIN_Handle ledPinHandle;
static PIN_Handle buttonPinHandle;
static PIN_State ledPinState;
static PIN_State buttonPinState;

/* TX button state */
volatile bool buttonPushed = false;

/* Advanced TX command for sending long preamble */
static rfc_CMD_PROP_TX_ADV_t RF_cmdPropTxAdv;

/*
 * Application LED pin configuration table:
 *   - All LEDs board LEDs are off.
 */
PIN_Config pinTable[] =
{
    CONFIG_PIN_RLED | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL | PIN_DRVSTR_MAX,
    CONFIG_PIN_GLED | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL | PIN_DRVSTR_MAX,
	PIN_TERMINATE
};

/*
 * Application button pin configuration table:
 *   - Buttons interrupts are configured to trigger on falling edge.
 */
PIN_Config buttonPinTable[] = {
    CONFIG_PIN_BTN1 | PIN_INPUT_EN | PIN_PULLUP | PIN_IRQ_NEGEDGE,
    PIN_TERMINATE
};


/***** Function definitions *****/
/* Pin interrupt Callback function board buttons configured in the pinTable. */
void buttonCallbackFunction(PIN_Handle handle, PIN_Id pinId) {

    /* Simple debounce logic, only toggle if the button is still pushed (low) */
    CPUdelay((uint32_t)((48000000/3)*0.050f));
    if (!PIN_getInputValue(pinId)) {
        /* Post TX semaphore to TX task */
        bool previousHwiState = IntMasterDisable();
        // Tricky IntMasterDisable():
        //true  : Interrupts were already disabled when the function was called.
        //false : Interrupts were enabled and are now disabled.
        buttonPushed = true;
        if (!previousHwiState) {
            IntMasterEnable();
        }
    }
}

/* TX task function. Executed in Task context by TI-RTOS when the scheduler starts. */
void *mainThread(void *arg0)
{
    /* Call display driver init functions. */
    Display_init();

    /* Open LED pins */
    ledPinHandle = PIN_open(&ledPinState, pinTable);
    if (ledPinHandle == NULL)
    {
        while(1);
    }

    /* Open Button pins */
    buttonPinHandle = PIN_open(&buttonPinState, buttonPinTable);
    if (buttonPinHandle == NULL)
    {
        while(1);
    }

    /* Initialize the display and try to open both UART and LCD types of display. */
    Display_Params params;
    Display_Params_init(&params);
    params.lineClearMode = DISPLAY_CLEAR_BOTH;
    Display_Handle uartDisplayHandle = Display_open(Display_Type_UART, &params);
    Display_Handle lcdDisplayHandle = Display_open(Display_Type_LCD, &params);

    /* Print initial display content on both UART and any LCD present */
    Display_printf(uartDisplayHandle, 0, 0, "Wake-on-Radio TX");
    Display_printf(uartDisplayHandle, 0, 0, "Pkts sent: %u", seqNumber);
    Display_printf(lcdDisplayHandle, 0, 0, "Wake-on-Radio TX");
    Display_printf(lcdDisplayHandle, 1, 0, "Pkts sent: %u", seqNumber);

    /* Setup callback for button pins */
    if (PIN_registerIntCb(buttonPinHandle, buttonCallbackFunction) != 0)
    {
        while(1);
    }

    /* Initialize the radio */
    RF_Params rfParams;
    RF_Params_init(&rfParams);

    /* Initialize TX_ADV command from TX command */
    initializeTxAdvCmdFromTxCmd(&RF_cmdPropTxAdv, &RF_cmdPropTx);

    /* Set application specific fields */
    RF_cmdPropTxAdv.pktLen = PAYLOAD_LENGTH +1; /* +1 for length byte */
    RF_cmdPropTxAdv.pPkt = packet;
    RF_cmdPropTxAdv.preTrigger.triggerType = TRIG_REL_START;
    RF_cmdPropTxAdv.preTime = WOR_PREAMBLE_TIME_RAT_TICKS(WOR_WAKEUPS_PER_SECOND);

    /* Request access to the radio */
#if defined(DeviceFamily_CC26X0R2)
    rfHandle = RF_open(&rfObject, &RF_prop, (RF_RadioSetup*)&RF_cmdPropRadioSetup, &rfParams);
#else
    rfHandle = RF_open(&rfObject, &RF_prop, (RF_RadioSetup*)&RF_cmdPropRadioDivSetup, &rfParams);
#endif// DeviceFamily_CC26X0R2

    /* Set the frequency */
    RF_runCmd(rfHandle, (RF_Op*)&RF_cmdFs, RF_PriorityNormal, NULL, 0);

    /* Enter main TX loop */
    while(1)
    {
        /* Wait for a button press */
        bool previousHwiState = IntMasterDisable();
        // Tricky IntMasterDisable():
        //true  : Interrupts were already disabled when the function was called.
        //false : Interrupts were enabled and are now disabled.
        while (buttonPushed == false) {
            IntMasterEnable();
            Power_idleFunc();
            IntMasterDisable();
        }
        buttonPushed = false;
        if (!previousHwiState) {
            IntMasterEnable();
        }

        /* Create packet with incrementing sequence number and random payload */
        packet[0] = PAYLOAD_LENGTH;
        packet[1] = (uint8_t)(seqNumber >> 8);
        packet[2] = (uint8_t)(seqNumber++);
        uint8_t i;
        for (i = 3; i < PAYLOAD_LENGTH +1; i++)
        {
            packet[i] = rand();
        }

        /* Send packet */
        RF_runCmd(rfHandle, (RF_Op*)&RF_cmdPropTxAdv, RF_PriorityNormal, NULL, 0);

        /* Update display */
        Display_printf(uartDisplayHandle, 0, 0, "Pkts sent: %u", seqNumber);
        Display_printf(lcdDisplayHandle, 1, 0, "Pkts sent: %u", seqNumber);

        /* Toggle LED */
        PIN_setOutputValue(ledPinHandle, CONFIG_PIN_GLED, !PIN_getOutputValue(CONFIG_PIN_GLED));
    }
}

/* Copy the basic RX configuration from CMD_PROP_RX to CMD_PROP_RX_SNIFF command. */
static void initializeTxAdvCmdFromTxCmd(rfc_CMD_PROP_TX_ADV_t* RF_cmdPropTxAdv, rfc_CMD_PROP_TX_t* RF_cmdPropTx)
{
    #define RADIO_OP_HEADER_SIZE 14

    /* Copy general radio operation header from TX commmand to TX_ADV */
    memcpy(RF_cmdPropTxAdv, RF_cmdPropTx, RADIO_OP_HEADER_SIZE);

    /* Set command to CMD_PROP_TX_ADV */
    RF_cmdPropTxAdv->commandNo = CMD_PROP_TX_ADV;

    /* Copy over relevant parameters */
    RF_cmdPropTxAdv->pktConf.bFsOff = RF_cmdPropTx->pktConf.bFsOff;
    RF_cmdPropTxAdv->pktConf.bUseCrc = RF_cmdPropTx->pktConf.bUseCrc;
    RF_cmdPropTxAdv->syncWord = RF_cmdPropTx->syncWord;
}
