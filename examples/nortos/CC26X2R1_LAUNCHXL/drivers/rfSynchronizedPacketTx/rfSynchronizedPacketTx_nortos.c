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

/* Application Header files */
#include "StateMachine_nortos.h"
#include "rfSynchronizedPacket.h"
#include <ti_radio_config.h>

/* Standard C Libraries */
#include <stdlib.h>
#include <stdint.h>

/* Board Header files */
#include "ti_drivers_config.h"

/* TI Drivers */
#include <ti/drivers/pin/PINCC26XX.h>
#include <ti/drivers/rf/RF.h>

#include <ti/devices/DeviceFamily.h>
#include DeviceFamily_constructPath(driverlib/rf_common_cmd.h)
#include DeviceFamily_constructPath(driverlib/rf_mailbox.h)
#include DeviceFamily_constructPath(driverlib/rf_prop_cmd.h)
#include DeviceFamily_constructPath(driverlib/rf_prop_mailbox.h)
#include DeviceFamily_constructPath(driverlib/cpu.h)

/* Define events that can be posted to the application state machine */
typedef enum {
    Event_TxModeButtonPushed = StateMachine_Event00,
    Event_LedButtonPushed = StateMachine_Event01,
} Event;

/* Declare state handler functions for the application. */
StateMachine_DECLARE_STATE(SetupState);
StateMachine_DECLARE_STATE(PeriodicBeaconState);
StateMachine_DECLARE_STATE(SpontaneousBeaconState);


/*
 * Application LED pin configuration table:
 *   - All LEDs board LEDs are off.
 *   - Button pins are high by default and pulled to low when pressed.
 */
PIN_Config pinTable[] =
{
    CONFIG_PIN_GLED | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL | PIN_DRVSTR_MAX,
    CONFIG_PIN_RLED | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL | PIN_DRVSTR_MAX,
    CONFIG_PIN_BTN1 | PIN_INPUT_EN | PIN_PULLUP | PIN_IRQ_NEGEDGE,
    CONFIG_PIN_BTN2 | PIN_INPUT_EN | PIN_PULLUP | PIN_IRQ_NEGEDGE,
	PIN_TERMINATE
};


/***** Defines *****/
#define BEACON_INTERVAL_MS  500

/***** Prototypes *****/
void buttonCallbackFunction(PIN_Handle handle, PIN_Id pinId);
void PeriodicBeaconState_txCallback(RF_Handle h, RF_CmdHandle ch, RF_EventMask e);

/***** Variable declarations *****/
static PIN_Handle pinHandle;
static PIN_State pinState;

static RF_Object rfObject;
static RF_Handle rfHandle;

static BeaconPacket message;
static StateMachine_Struct stateMachine;

/***** Function definitions *****/

void *mainThread(void *arg0)
{
    /* Open LED pins */
    pinHandle = PIN_open(&pinState, pinTable);
    if (pinHandle == NULL)
    {
        while(1);
    }

    PIN_Status status = PIN_registerIntCb(pinHandle, &buttonCallbackFunction);
    /* Setup callback for button pins */
    if (status != PIN_SUCCESS)
    {
        while(1);
    }

    // Initialise the application state machine.
    StateMachine_construct(&stateMachine);

    /* Execute the state machine StateMachine_exec() function */
    StateMachine_exec(&stateMachine, SetupState);

    return 0;
}



/* Pin interrupt Callback function board buttons configured in the pinTable. */
void buttonCallbackFunction(PIN_Handle handle, PIN_Id pinId) {

    /* Debounce the button with a short delay */
    CPUdelay(CPU_convertMsToDelayCycles(5));
    if (PIN_getInputValue(pinId) == 1)
    {
        return;
    }

    switch (pinId)
    {
    case CONFIG_PIN_BTN1:
        StateMachine_postEvents(&stateMachine, Event_LedButtonPushed);
        PIN_setOutputValue(pinHandle, CONFIG_PIN_GLED, !PIN_getInputValue(CONFIG_PIN_GLED));
        break;
    case CONFIG_PIN_BTN2:
        StateMachine_postEvents(&stateMachine, Event_TxModeButtonPushed);
        break;
    }
}


void SetupState_function()
{
    /* Prepare the packet */
    RF_cmdPropTx.pktLen = sizeof(message);
    RF_cmdPropTx.pPkt = (uint8_t*)&message;
    RF_cmdPropTx.startTrigger.triggerType = TRIG_ABSTIME;
    RF_cmdPropTx.startTime = 0;

    message.beaconInterval = RF_convertMsToRatTicks(BEACON_INTERVAL_MS);

    /* Request access to the radio. This does not power-up the RF core, but only initialise
     * the driver and cache the setup command. */
    RF_Params rfParams;
    RF_Params_init(&rfParams);
#if defined(DeviceFamily_CC26X0R2)
    rfHandle = RF_open(&rfObject, &RF_prop, (RF_RadioSetup*)&RF_cmdPropRadioSetup, &rfParams);
#else
    rfHandle = RF_open(&rfObject, &RF_prop, (RF_RadioSetup*)&RF_cmdPropRadioDivSetup, &rfParams);
#endif// DeviceFamily_CC26X0R2
    if (rfHandle == NULL)
    {
        while(1);
    }

    /* Set the frequency. Now the RF driver powers the RF core up and runs the setup command from above.
     * The FS command is executed and also cached for later use when the RF driver does an automatic
     * power up. */
    RF_EventMask result = RF_runCmd(rfHandle, (RF_Op*)&RF_cmdFs, RF_PriorityNormal, NULL, 0);
    if ((result != RF_EventLastCmdDone) || ((volatile RF_Op*)&RF_cmdFs)->status != DONE_OK)
    {
        while(1);
    }

    /* Use the current time as an anchor point for future time stamps.
     * The Nth transmission in the future will be exactly N * 500ms after
     * this time stamp.  */
    RF_cmdPropTx.startTime = RF_getCurrentTime();

    /* A trigger in the past is triggered as soon as possible.
     * No error is given.
     * This avoids assertion when button debouncing causes delay in TX trigger.  */
    RF_cmdPropTx.startTrigger.pastTrig = 1;

    /* Route the PA signal to an LED to indicate ongoing transmissions.
     * Available signals are listed in the proprietary RF user's guide.
     */
    PINCC26XX_setMux(pinHandle, CONFIG_PIN_RLED, PINCC26XX_MUX_RFC_GPO1);

    StateMachine_setNextState(&stateMachine, PeriodicBeaconState);
}

void PeriodicBeaconState_function()
{
    /* Set absolute TX time in the future to utilise "deferred dispatching of commands with absolute timing".
     * This is explained in the proprietary RF user's guide. */
    RF_cmdPropTx.startTime += RF_convertMsToRatTicks(BEACON_INTERVAL_MS);

    message.txTime = RF_cmdPropTx.startTime;
    message.ledState = PIN_getInputValue(CONFIG_PIN_GLED);

    /* Because the TX command is due in 500ms and we use TRIG_ABSTIME as start trigger type,
     * the RF driver will now power down the RF core and and wait until ~1.5ms before RF_cmdPropTx.startTime.
     * Then the RF driver will power-up the RF core, re-synchronise the RAT and re-run the setup procedure.
     * The setup procedure includes RF_cmdPropRadioDivSetup and RF_cmdFs from above.
     * This will guarantee that RF_cmdPropTx is delivered to the RF core right before it has
     * to start. This is fully transparent to the application. It appears as the RF core was
     * never powered down.
     * This concept is explained in the proprietary RF user's guide. */
    RF_EventMask result = RF_runCmd(rfHandle, (RF_Op*)&RF_cmdPropTx, RF_PriorityNormal, NULL, 0);
    if ((result != RF_EventLastCmdDone) || ((volatile RF_Op*)&RF_cmdPropTx)->status != PROP_DONE_OK)
    {
        while(1);
    }

    if (StateMachine_pendEvents(&stateMachine, Event_TxModeButtonPushed, false) & Event_TxModeButtonPushed)
    {
        StateMachine_setNextState(&stateMachine, SpontaneousBeaconState);
    }
    else
    {
        // PeriodicBeaconState_function() will be entered again.
        StateMachine_setNextState(&stateMachine, PeriodicBeaconState);
    }
}


void SpontaneousBeaconState_function()
{
    StateMachine_EventMask events = StateMachine_pendEvents(&stateMachine, Event_TxModeButtonPushed | Event_LedButtonPushed, true);

    /* We need to find the next synchronized time slot that is far enough
     * in the future to allow the RF driver to power up the RF core.
     * We use 2 ms as safety margin. */
    uint32_t currentTime = RF_getCurrentTime() + RF_convertMsToRatTicks(2);
    uint32_t intervalsSinceLastPacket = DIV_INT_ROUND_UP(currentTime - RF_cmdPropTx.startTime, RF_convertMsToRatTicks(BEACON_INTERVAL_MS));
    RF_cmdPropTx.startTime += intervalsSinceLastPacket * RF_convertMsToRatTicks(BEACON_INTERVAL_MS);

    if (events & Event_TxModeButtonPushed)
    {
        StateMachine_setNextState(&stateMachine, PeriodicBeaconState);
    }

    if (events & Event_LedButtonPushed)
    {
        message.txTime = RF_cmdPropTx.startTime;
        message.ledState = PIN_getInputValue(CONFIG_PIN_GLED);

        /* Because the TX command is due in 500ms and we use TRIG_ABSTIME as start trigger type,
         * the RF driver will now power down the RF core and and wait until ~1.5ms before RF_cmdPropTx.startTime.
         * Then the RF driver will power-up the RF core, re-synchronise the RAT and re-run the setup procedure.
         * The setup procedure includes RF_cmdPropRadioDivSetup and RF_cmdFs from above.
         * This will guarantee that RF_cmdPropTx is delivered to the RF core right before it has
         * to start. This is fully transparent to the application. It appears as the RF core was
         * never powered down.
         * This concept is explained in the proprietary RF user's guide. */
        RF_EventMask result = RF_runCmd(rfHandle, (RF_Op*)&RF_cmdPropTx, RF_PriorityNormal, NULL, 0);
        if ((result != RF_EventLastCmdDone) || ((volatile RF_Op*)&RF_cmdPropTx)->status != PROP_DONE_OK)
        {
            while(1);
        }
    }
}
