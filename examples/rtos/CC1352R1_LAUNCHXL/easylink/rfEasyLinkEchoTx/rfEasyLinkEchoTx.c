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
 *  ======== rfEasyLinkEchoTx.c ========
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

/* Application Header files */
#include <ti_radio_config.h>

/* EasyLink API Header files */
#include "easylink/EasyLink.h"

/* Undefine to not use async mode */
#define RFEASYLINKECHO_ASYNC

#define RFEASYLINKECHO_TASK_STACK_SIZE    1024
#define RFEASYLINKECHO_TASK_PRIORITY      2

#define RFEASYLINKECHO_PAYLOAD_LENGTH     30

Task_Struct echoTask;    /* not static so you can see in ROV */
static Task_Params echoTaskParams;
static uint8_t echoTaskStack[RFEASYLINKECHO_TASK_STACK_SIZE];

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

#ifdef RFEASYLINKECHO_ASYNC
static Semaphore_Handle echoDoneSem;
#endif //RFEASYLINKECHO_ASYNC

EasyLink_TxPacket txPacket = {{0}, 0, 0, {0}};

bool isPacketCorrect(EasyLink_RxPacket *rxp, EasyLink_TxPacket *txp)
{
    uint16_t i;
    bool status = true;

    for(i = 0; i < rxp->len; i++)
    {
        if(rxp->payload[i] != txp->payload[i])
        {
            status = false;
            break;
        }
    }
    return(status);
}

#ifdef RFEASYLINKECHO_ASYNC
void echoTxDoneCb(EasyLink_Status status)
{
    if (status == EasyLink_Status_Success)
    {
        /* Toggle GLED to indicate TX */
        PIN_setOutputValue(pinHandle, CONFIG_PIN_GLED,!PIN_getOutputValue(CONFIG_PIN_GLED));
        /* Turn RLED off, in case there was a prior error */
        PIN_setOutputValue(pinHandle, CONFIG_PIN_RLED, 0);
    }
    else
    {
        /* Set both GLED and RLED to indicate error */
        PIN_setOutputValue(pinHandle, CONFIG_PIN_GLED, 1);
        PIN_setOutputValue(pinHandle, CONFIG_PIN_RLED, 1);
    }

    Semaphore_post(echoDoneSem);
}

void echoRxDoneCb(EasyLink_RxPacket * rxPacket, EasyLink_Status status)
{
    if ((status == EasyLink_Status_Success) &&
            (isPacketCorrect(rxPacket, &txPacket)))
    {
        /* Toggle GLED, clear RLED to indicate Echo RX */
        PIN_setOutputValue(pinHandle, CONFIG_PIN_GLED,!PIN_getOutputValue(CONFIG_PIN_GLED));
        PIN_setOutputValue(pinHandle, CONFIG_PIN_RLED, 0);
    }
    else if (status == EasyLink_Status_Aborted)
    {
        /* Set RLED and clear GLED to indicate Abort */
        PIN_setOutputValue(pinHandle, CONFIG_PIN_RLED, 1);
        PIN_setOutputValue(pinHandle, CONFIG_PIN_GLED, 0);
    }
    else
    {
        /* Set both GLED and RLED to indicate error */
        PIN_setOutputValue(pinHandle, CONFIG_PIN_GLED, 1);
        PIN_setOutputValue(pinHandle, CONFIG_PIN_RLED, 1);
    }

    Semaphore_post(echoDoneSem);
}
#endif //RFEASYLINKECHO_ASYNC

static void rfEasyLinkEchoTxFnx(UArg arg0, UArg arg1)
{
    uint32_t absTime;

#ifdef RFEASYLINKECHO_ASYNC
    /* Create a semaphore for Async */
    Semaphore_Params params;
    Error_Block      eb;

    /* Init params */
    Semaphore_Params_init(&params);
    Error_init(&eb);

    /* Create semaphore instance */
    echoDoneSem = Semaphore_create(0, &params, &eb);
    if(echoDoneSem == NULL)
    {
        System_abort("Semaphore creation failed");
    }

#else
    EasyLink_RxPacket rxPacket = {{0}, 0, 0, 0, 0, {0}};
#endif //RFEASYLINKECHO_ASYNC

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

    // Packet Originator
    while(1) {
        /* Create packet with incrementing sequence number and random payload */
        txPacket.payload[0] = (uint8_t)(seqNumber >> 8);
        txPacket.payload[1] = (uint8_t)(seqNumber++);
        uint8_t i;
        for (i = 2; i < RFEASYLINKECHO_PAYLOAD_LENGTH; i++)
        {
            txPacket.payload[i] = rand();
        }

        txPacket.len = RFEASYLINKECHO_PAYLOAD_LENGTH;

        /*
         * Address filtering is enabled by default on the Rx device with the
         * an address of 0xAA. This device must set the dstAddr accordingly.
         */
        txPacket.dstAddr[0] = 0xaa;

        /* Set Tx absolute time to current time + 1000ms */
        if(EasyLink_getAbsTime(&absTime) != EasyLink_Status_Success)
        {
            // Problem getting absolute time
        }
        txPacket.absTime = absTime + EasyLink_ms_To_RadioTime(1000);

#ifdef RFEASYLINKECHO_ASYNC
        EasyLink_transmitAsync(&txPacket, echoTxDoneCb);

        /* Wait for Tx to complete. A Successful TX will cause the echoTxDoneCb
         * to be called and the echoDoneSem to be released, so we must
         * consume the echoDoneSem
         */
        Semaphore_pend(echoDoneSem, BIOS_WAIT_FOREVER);

        /* Switch to Receiver */
        EasyLink_receiveAsync(echoRxDoneCb, 0);

        /* Wait 500ms for Rx */
        if(Semaphore_pend(echoDoneSem, (500000 / Clock_tickPeriod)) == FALSE)
        {
            /* RX timed out abort */
            if(EasyLink_abort() == EasyLink_Status_Success)
            {
               /* Wait for the abort */
               Semaphore_pend(echoDoneSem, BIOS_WAIT_FOREVER);
            }
        }
#else
        EasyLink_Status result = EasyLink_transmit(&txPacket);

        if (result == EasyLink_Status_Success)
        {
            /* Toggle GLED to indicate TX */
            PIN_setOutputValue(pinHandle, CONFIG_PIN_GLED,!PIN_getOutputValue(CONFIG_PIN_GLED));
            /* Turn RLED off, in case there was a prior error */
            PIN_setOutputValue(pinHandle, CONFIG_PIN_RLED, 0);
        }
        else
        {
            /* Set both GLED and RLED to indicate error */
            PIN_setOutputValue(pinHandle, CONFIG_PIN_GLED, 1);
            PIN_setOutputValue(pinHandle, CONFIG_PIN_RLED, 1);
        }

        /* Switch to Receiver, set a timeout interval of 500ms */
        rxPacket.absTime = 0;
        rxPacket.rxTimeout = EasyLink_ms_To_RadioTime(500);
        result = EasyLink_receive(&rxPacket);

        /* Check Received packet against what was sent, it should be identical
         * to the transmitted packet
         */
        if (result == EasyLink_Status_Success &&
                isPacketCorrect(&rxPacket, &txPacket))
        {
            /* Toggle GLED, clear RLED to indicate Echo RX */
            PIN_setOutputValue(pinHandle, CONFIG_PIN_GLED,!PIN_getOutputValue(CONFIG_PIN_GLED));
            PIN_setOutputValue(pinHandle, CONFIG_PIN_RLED, 0);
        }
        else if (result == EasyLink_Status_Rx_Timeout)
        {
            /* Set RLED and clear GLED to indicate Rx Timeout */
            PIN_setOutputValue(pinHandle, CONFIG_PIN_RLED, 1);
            PIN_setOutputValue(pinHandle, CONFIG_PIN_GLED, 0);
        }
        else
        {
            /* Set both GLED and RLED to indicate error */
            PIN_setOutputValue(pinHandle, CONFIG_PIN_GLED, 1);
            PIN_setOutputValue(pinHandle, CONFIG_PIN_RLED, 1);
        }
#endif //RFEASYLINKECHO_ASYNC
    }
}

void echoTask_init(PIN_Handle inPinHandle) {
    pinHandle = inPinHandle;

    Task_Params_init(&echoTaskParams);
    echoTaskParams.stackSize = RFEASYLINKECHO_TASK_STACK_SIZE;
    echoTaskParams.priority = RFEASYLINKECHO_TASK_PRIORITY;
    echoTaskParams.stack = &echoTaskStack;
    echoTaskParams.arg0 = (UInt)1000000;

    Task_construct(&echoTask, rfEasyLinkEchoTxFnx, &echoTaskParams, NULL);
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

    echoTask_init(pinHandle);

    /* Start BIOS */
    BIOS_start();

    return (0);
}
