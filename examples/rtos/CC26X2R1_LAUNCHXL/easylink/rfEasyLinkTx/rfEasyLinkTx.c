/*
 * Copyright (c) 2017-2018, Texas Instruments Incorporated
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
 *  ======== rfEasyLinkTx.c ========
 */
 /* Standard C Libraries */
#include <stdlib.h>

/* XDCtools Header files */
#include <xdc/std.h>
#include <xdc/runtime/Assert.h>
#include <xdc/runtime/Error.h>
#include <xdc/runtime/System.h>

/* BIOS Header files */
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/knl/Clock.h>

/* TI-RTOS Header files */
#include <ti/drivers/PIN.h>

/* Board Header files */
#include "ti_drivers_config.h"

/* EasyLink API Header files */
#include "easylink/EasyLink.h"

/* Application header files */
#include <ti_radio_config.h>

/* Undefine to not use async mode */
#define RFEASYLINKTX_ASYNC

#define RFEASYLINKTX_TASK_STACK_SIZE    1024
#define RFEASYLINKTX_TASK_PRIORITY      2

#define RFEASYLINKTX_BURST_SIZE         10
#define RFEASYLINKTXPAYLOAD_LENGTH      30

Task_Struct txTask;    /* not static so you can see in ROV */
static Task_Params txTaskParams;
static uint8_t txTaskStack[RFEASYLINKTX_TASK_STACK_SIZE];

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

static uint16_t seqNumber;

#ifdef RFEASYLINKTX_ASYNC
static Semaphore_Handle txDoneSem;
#endif //RFEASYLINKTX_ASYNC

#ifdef RFEASYLINKTX_ASYNC
void txDoneCb(EasyLink_Status status)
{
    if (status == EasyLink_Status_Success)
    {
        /* Toggle GLED to indicate TX */
        PIN_setOutputValue(pinHandle, CONFIG_PIN_GLED,!PIN_getOutputValue(CONFIG_PIN_GLED));
    }
    else if(status == EasyLink_Status_Aborted)
    {
        /* Toggle RLED to indicate command aborted */
        PIN_setOutputValue(pinHandle, CONFIG_PIN_RLED,!PIN_getOutputValue(CONFIG_PIN_RLED));
    }
    else
    {
        /* Toggle GLED and RLED to indicate error */
        PIN_setOutputValue(pinHandle, CONFIG_PIN_GLED,!PIN_getOutputValue(CONFIG_PIN_GLED));
        PIN_setOutputValue(pinHandle, CONFIG_PIN_RLED,!PIN_getOutputValue(CONFIG_PIN_RLED));
    }

    Semaphore_post(txDoneSem);
}
#endif //RFEASYLINKTX_ASYNC

static void rfEasyLinkTxFnx(UArg arg0, UArg arg1)
{
    uint8_t txBurstSize = 0;
    uint32_t absTime;

#ifdef RFEASYLINKTX_ASYNC
    /* Create a semaphore for Async */
    Semaphore_Params params;
    Error_Block eb;

    /* Init params */
    Semaphore_Params_init(&params);
    Error_init(&eb);

    /* Create semaphore instance */
    txDoneSem = Semaphore_create(0, &params, &eb);
    if(txDoneSem == NULL)
    {
        System_abort("Semaphore creation failed");
    }

#endif //TX_ASYNC

    // Initialize the EasyLink parameters to their default values
    EasyLink_Params easyLink_params;
    EasyLink_Params_init(&easyLink_params);

    /*
     * Initialize EasyLink with the settings found in ti_easylink_config.h
     * Modify EASYLINK_PARAM_CONFIG in ti_easylink_config.h to change the default
     * PHY
     */
    if(EasyLink_init(&easyLink_params) != EasyLink_Status_Success)
    {
        System_abort("EasyLink_init failed");
    }

    /*
     * If you wish to use a frequency other than the default, use
     * the following API:
     * EasyLink_setFrequency(868000000);
     */

    while(1) {
        EasyLink_TxPacket txPacket =  { {0}, 0, 0, {0} };

        /* Create packet with incrementing sequence number and random payload */
        txPacket.payload[0] = (uint8_t)(seqNumber >> 8);
        txPacket.payload[1] = (uint8_t)(seqNumber++);
        uint8_t i;
        for (i = 2; i < RFEASYLINKTXPAYLOAD_LENGTH; i++)
        {
          txPacket.payload[i] = rand();
        }

        txPacket.len = RFEASYLINKTXPAYLOAD_LENGTH;

        /*
         * Address filtering is enabled by default on the Rx device with the
         * an address of 0xAA. This device must set the dstAddr accordingly.
         */
        txPacket.dstAddr[0] = 0xaa;

        /* Add a Tx delay for > 500ms, so that the abort kicks in and brakes the burst */
        if(EasyLink_getAbsTime(&absTime) != EasyLink_Status_Success)
        {
            // Problem getting absolute time
        }
        if(txBurstSize++ >= RFEASYLINKTX_BURST_SIZE)
        {
          /* Set Tx absolute time to current time + 1s */
          txPacket.absTime = absTime + EasyLink_ms_To_RadioTime(1000);
          txBurstSize = 0;
        }
        /* Else set the next packet in burst to Tx in 100ms */
        else
        {
          /* Set Tx absolute time to current time + 100ms */
          txPacket.absTime = absTime + EasyLink_ms_To_RadioTime(100);
        }

#ifdef RFEASYLINKTX_ASYNC
        EasyLink_transmitAsync(&txPacket, txDoneCb);
        /* Wait 300ms for Tx to complete */
        if(Semaphore_pend(txDoneSem, (300000 / Clock_tickPeriod)) == FALSE)
        {
            /* TX timed out, abort */
            if(EasyLink_abort() == EasyLink_Status_Success)
            {
                /*
                 * Abort will cause the txDoneCb to be called and the txDoneSem
                 * to be released, so we must consume the txDoneSem
                 */
               Semaphore_pend(txDoneSem, BIOS_WAIT_FOREVER);
            }
        }
#else
        EasyLink_Status result = EasyLink_transmit(&txPacket);

        if (result == EasyLink_Status_Success)
        {
            /* Toggle GLED to indicate TX */
            PIN_setOutputValue(pinHandle, CONFIG_PIN_GLED,!PIN_getOutputValue(CONFIG_PIN_GLED));
        }
        else
        {
            /* Toggle GLED and RLED to indicate error */
            PIN_setOutputValue(pinHandle, CONFIG_PIN_GLED,!PIN_getOutputValue(CONFIG_PIN_GLED));
            PIN_setOutputValue(pinHandle, CONFIG_PIN_RLED,!PIN_getOutputValue(CONFIG_PIN_RLED));
        }
#endif //RFEASYLINKTX_ASYNC
    }
}

void txTask_init(PIN_Handle inPinHandle) {
    pinHandle = inPinHandle;

    Task_Params_init(&txTaskParams);
    txTaskParams.stackSize = RFEASYLINKTX_TASK_STACK_SIZE;
    txTaskParams.priority = RFEASYLINKTX_TASK_PRIORITY;
    txTaskParams.stack = &txTaskStack;
    txTaskParams.arg0 = (UInt)1000000;

    Task_construct(&txTask, rfEasyLinkTxFnx, &txTaskParams, NULL);
}

/*
 *  ======== main ========
 */
int main(void)
{
    /* Call driver init functions. */
    Board_initGeneral();

    /* Open LED pins */
    pinHandle = PIN_open(&pinState, pinTable);
    Assert_isTrue(pinHandle != NULL, NULL);

    /* Clear LED pins */
    PIN_setOutputValue(pinHandle, CONFIG_PIN_GLED, 0);
    PIN_setOutputValue(pinHandle, CONFIG_PIN_RLED, 0);

    txTask_init(pinHandle);

    /* Start BIOS */
    BIOS_start();

    return (0);
}
