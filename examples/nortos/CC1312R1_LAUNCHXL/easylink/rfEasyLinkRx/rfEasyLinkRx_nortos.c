/*
 * Copyright (c) 2015-2018, Texas Instruments Incorporated
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
 *  ======== rfEasyLinkRx_nortos.c ========
 */
/* Application header files */
#include <ti_radio_config.h>
#include "ti_drivers_config.h"

/* Standard C Libraries */
#include <stdbool.h>
#include <stdint.h>

/* TI Drivers */
#include <ti/drivers/pin/PINCC26XX.h>
#include <ti/drivers/Power.h>
#include <ti/drivers/rf/RF.h>
#include <ti/drivers/timer/GPTimerCC26XX.h>
#include <ti/devices/DeviceFamily.h>

/* Driverlib APIs */
#include DeviceFamily_constructPath(driverlib/sys_ctrl.h)

/* EasyLink API Header files */
#include "easylink/EasyLink.h"

/***** Defines *****/

/* Undefine to remove async mode */
#define RFEASYLINKRX_ASYNC

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


#ifdef RFEASYLINKRX_ASYNC
/* GPTimer handle and timeout value */
GPTimerCC26XX_Handle hTimer;
GPTimerCC26XX_Value rxTimeoutVal;

/* GP Timer Callback */
void rxTimeoutCb(GPTimerCC26XX_Handle handle,
                 GPTimerCC26XX_IntMask interruptMask);

static volatile bool rxDoneFlag;
static volatile bool rxTimeoutFlag;
#endif

/***** Function definitions *****/
#ifdef RFEASYLINKRX_ASYNC
void rxDoneCb(EasyLink_RxPacket * rxPacket, EasyLink_Status status)
{
    uint32_t currTimerVal;

    if (status == EasyLink_Status_Success)
    {
        /* Toggle RLED to indicate RX */
        PIN_setOutputValue(pinHandle, CONFIG_PIN_RLED,!PIN_getOutputValue(CONFIG_PIN_RLED));

        /*
         * Stop the Receiver timeout timer, find the current free-running
         * counter value and add it to the existing interval load value
         */
        GPTimerCC26XX_stop(hTimer);
        currTimerVal = GPTimerCC26XX_getValue(hTimer);
        GPTimerCC26XX_setLoadValue(hTimer, rxTimeoutVal + currTimerVal);
    }
    else if(status == EasyLink_Status_Aborted)
    {
        /* Toggle GLED to indicate command aborted */
        PIN_setOutputValue(pinHandle, CONFIG_PIN_GLED,!PIN_getOutputValue(CONFIG_PIN_GLED));
    }
    else
    {
        /* Toggle GLED and RLED to indicate error */
        PIN_setOutputValue(pinHandle, CONFIG_PIN_GLED,!PIN_getOutputValue(CONFIG_PIN_GLED));
        PIN_setOutputValue(pinHandle, CONFIG_PIN_RLED,!PIN_getOutputValue(CONFIG_PIN_RLED));

        /*
         * Stop the Receiver timeout timer, find the current free-running
         * counter value and add it to the existing interval load value
         */
        GPTimerCC26XX_stop(hTimer);
        currTimerVal = GPTimerCC26XX_getValue(hTimer);
        GPTimerCC26XX_setLoadValue(hTimer, rxTimeoutVal + currTimerVal);
    }

    rxDoneFlag = true;
}
#endif //RFEASYLINKRX_ASYNC

void *mainThread(void *arg0)
{

    /* Open LED pins */
    pinHandle = PIN_open(&pinState, pinTable);
    if (pinHandle == NULL)
    {
        while(1);
    }

    /* Clear LED pins */
    PIN_setOutputValue(pinHandle, CONFIG_PIN_GLED, 0);
    PIN_setOutputValue(pinHandle, CONFIG_PIN_RLED, 0);

#ifdef RFEASYLINKRX_ASYNC
    /* Reset the timeout flag */
    rxTimeoutFlag = false;
    /* Set the reception flag to its default state */
    rxDoneFlag = false;

    /* Open the GPTimer driver */
    GPTimerCC26XX_Params params;
    GPTimerCC26XX_Params_init(&params);
    params.width          = GPT_CONFIG_32BIT;
    params.mode           = GPT_MODE_ONESHOT;
    params.direction      = GPTimerCC26XX_DIRECTION_UP;
    params.debugStallMode = GPTimerCC26XX_DEBUG_STALL_OFF;
    hTimer = GPTimerCC26XX_open(CONFIG_TIMER_0, &params);
    if(hTimer == NULL)
    {
        while(1);
    }

    /* Set Timeout value to 300ms */
    rxTimeoutVal = (SysCtrlClockGet()*3UL)/10UL - 1UL;
    GPTimerCC26XX_setLoadValue(hTimer, rxTimeoutVal);


    /* Register the GPTimer interrupt */
    GPTimerCC26XX_registerInterrupt(hTimer, rxTimeoutCb, GPT_INT_TIMEOUT);
#else
    // Create an RX packet
    EasyLink_RxPacket rxPacket = {0};
#endif //RFEASYLINKRX_ASYNC

    // Initialize the EasyLink parameters to their default values
    EasyLink_Params easyLink_params;
    EasyLink_Params_init(&easyLink_params);

    /*
     * Initialize EasyLink with the settings found in ti_easylink_config.h
     * Modify EASYLINK_PARAM_CONFIG in ti_easylink_config.h to change the default
     * PHY
     */
    if (EasyLink_init(&easyLink_params) != EasyLink_Status_Success){
        while(1);
    }

    /*
     * If you wish to use a frequency other than the default, use
     * the following API:
     * EasyLink_setFrequency(868000000);
     */

    while(1) {
#ifdef RFEASYLINKRX_ASYNC
        // Set the Receive done flag to false, callback will
        // set it to true
        rxDoneFlag = false;

        // Wait to receive a packet
        EasyLink_receiveAsync(rxDoneCb, 0);

        /*
         * Start the Receiver timeout timer (300ms) before
         * EasyLink_receiveAsync enables the power policy
         */
        GPTimerCC26XX_start(hTimer);

        while(rxDoneFlag == false){
            bool previousHwiState = IntMasterDisable();
            /*
             * Tricky IntMasterDisable():
             * true  : Interrupts were already disabled when the function was
             *         called.
             * false : Interrupts were enabled and are now disabled.
             */
            IntMasterEnable();
            Power_idleFunc();
            IntMasterDisable();

            if(!previousHwiState)
            {
                IntMasterEnable();
            }

            /* Break if timeout flag is set */
            if(rxTimeoutFlag == true){
                /* Reset the timeout flag */
                rxTimeoutFlag = false;
                /* RX timed out, abort */
                if(EasyLink_abort() == EasyLink_Status_Success)
                {
                    /* Wait for the abort */
                    while(rxDoneFlag == false){};
                }
                break;
            }
        }
#else
        rxPacket.absTime = 0;
        EasyLink_Status result = EasyLink_receive(&rxPacket);

        if (result == EasyLink_Status_Success)
        {
            /* Toggle RLED to indicate RX */
            PIN_setOutputValue(pinHandle, CONFIG_PIN_RLED,!PIN_getOutputValue(CONFIG_PIN_RLED));
        }
        else
        {
            /* Toggle GLED and RLED to indicate error */
            PIN_setOutputValue(pinHandle, CONFIG_PIN_GLED,!PIN_getOutputValue(CONFIG_PIN_GLED));
            PIN_setOutputValue(pinHandle, CONFIG_PIN_RLED,!PIN_getOutputValue(CONFIG_PIN_RLED));
        }
#endif //RFEASYLINKRX_ASYNC
    }
}


#ifdef RFEASYLINKRX_ASYNC
/* GP Timer Callback Function */
void rxTimeoutCb(GPTimerCC26XX_Handle handle,
                 GPTimerCC26XX_IntMask interruptMask)
{
    /* Set the Timeout Flag */
    rxTimeoutFlag = true;

    /*
     * Timer is automatically stopped in one-shot mode and needs to be reset by
     * loading the interval load value
     */
    GPTimerCC26XX_setLoadValue(hTimer, rxTimeoutVal);
}
#endif // RFEASYLINKRX_ASYNC
