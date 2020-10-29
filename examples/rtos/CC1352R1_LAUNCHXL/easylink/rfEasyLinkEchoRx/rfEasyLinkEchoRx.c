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
 *  ======== rfEasyLinkEchoRx.c ========
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

#ifdef RFEASYLINKECHO_ASYNC
static Semaphore_Handle echoDoneSem;
#endif //RFEASYLINKECHO_ASYNC

static bool bBlockTransmit = false;

EasyLink_TxPacket txPacket = {{0}, 0, 0, {0}};

#ifdef RFEASYLINKECHO_ASYNC
void echoTxDoneCb(EasyLink_Status status)
{
    if (status == EasyLink_Status_Success)
    {
        /* Toggle RLED to indicate Echo TX, clear GLED */
        PIN_setOutputValue(pinHandle, CONFIG_PIN_RLED,!PIN_getOutputValue(CONFIG_PIN_RLED));
        PIN_setOutputValue(pinHandle, CONFIG_PIN_GLED, 0);
    }
    else
    {
        /* Set GLED and clear RLED to indicate error */
        PIN_setOutputValue(pinHandle, CONFIG_PIN_GLED, 1);
        PIN_setOutputValue(pinHandle, CONFIG_PIN_RLED, 0);
    }

    Semaphore_post(echoDoneSem);
}

void echoRxDoneCb(EasyLink_RxPacket * rxPacket, EasyLink_Status status)
{
    if (status == EasyLink_Status_Success)
    {
        /* Toggle RLED to indicate RX, clear GLED */
        PIN_setOutputValue(pinHandle, CONFIG_PIN_RLED,!PIN_getOutputValue(CONFIG_PIN_RLED));
        PIN_setOutputValue(pinHandle, CONFIG_PIN_GLED, 0);
        /* Copy contents of RX packet to TX packet */
        memcpy(&txPacket.payload, rxPacket->payload, rxPacket->len);
        /* Permit echo transmission */
        bBlockTransmit = false;
    }
    else
    {
        /* Set GLED and clear RLED to indicate error */
        PIN_setOutputValue(pinHandle, CONFIG_PIN_GLED, 1);
        PIN_setOutputValue(pinHandle, CONFIG_PIN_RLED, 0);
        /* Block echo transmission */
        bBlockTransmit = true;
    }

    Semaphore_post(echoDoneSem);
}
#endif //RFEASYLINKECHO_ASYNC

static void rfEasyLinkEchoRxFnx(UArg arg0, UArg arg1)
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

    while(1) {
#ifdef RFEASYLINKECHO_ASYNC
        // Wait to receive a packet
        EasyLink_receiveAsync(echoRxDoneCb, 0);

        /* Wait indefinitely for Rx */
        Semaphore_pend(echoDoneSem, BIOS_WAIT_FOREVER);
#else
        rxPacket.absTime = 0;
        EasyLink_Status result = EasyLink_receive(&rxPacket);

        if (result == EasyLink_Status_Success)
        {
            /* Toggle RLED to indicate RX, clear GLED */
            PIN_setOutputValue(pinHandle, CONFIG_PIN_RLED,!PIN_getOutputValue(CONFIG_PIN_RLED));
            PIN_setOutputValue(pinHandle, CONFIG_PIN_GLED, 0);
            /* Copy contents of RX packet to TX packet */
            memcpy(&txPacket.payload, &rxPacket.payload, rxPacket.len);
            /* Permit echo transmission */
            bBlockTransmit = false;
        }
        else
        {
            /* Set GLED and clear RLED to indicate error */
            PIN_setOutputValue(pinHandle, CONFIG_PIN_GLED, 1);
            PIN_setOutputValue(pinHandle, CONFIG_PIN_RLED, 0);
            /* Block echo transmission */
            bBlockTransmit = true;
        }
#endif // RFEASYLINKECHO_ASYNC

        if(bBlockTransmit == false)
        {
            /* Switch to Transmitter and echo the packet if transmission
             * is not blocked
             */
            txPacket.len = RFEASYLINKECHO_PAYLOAD_LENGTH;

            /*
             * Address filtering is enabled by default on the Rx device with the
             * an address of 0xAA. This device must set the dstAddr accordingly.
             */
            txPacket.dstAddr[0] = 0xaa;

            /* Set Tx absolute time to current time + 100ms*/
            if(EasyLink_getAbsTime(&absTime) != EasyLink_Status_Success)
            {
                // Problem getting absolute time
            }
            txPacket.absTime = absTime + EasyLink_ms_To_RadioTime(100);

#ifdef RFEASYLINKECHO_ASYNC
            EasyLink_transmitAsync(&txPacket, echoTxDoneCb);

            /* Wait for Tx to complete. A Successful TX will cause the echoTxDoneCb
             * to be called and the echoDoneSem to be released, so we must
             * consume the echoDoneSem
             */
            Semaphore_pend(echoDoneSem, BIOS_WAIT_FOREVER);

#else
            EasyLink_Status result = EasyLink_transmit(&txPacket);

            if (result == EasyLink_Status_Success)
            {
                /* Toggle RLED to indicate Echo TX, clear GLED */
                PIN_setOutputValue(pinHandle, CONFIG_PIN_RLED,!PIN_getOutputValue(CONFIG_PIN_RLED));
                PIN_setOutputValue(pinHandle, CONFIG_PIN_GLED, 0);
            }
            else
            {
                /* Set GLED and clear RLED to indicate error */
                PIN_setOutputValue(pinHandle, CONFIG_PIN_GLED, 1);
                PIN_setOutputValue(pinHandle, CONFIG_PIN_RLED, 0);
            }
#endif //RFEASYLINKECHO_ASYNC
        }
    }
}

void echoTask_init(PIN_Handle inPinHandle) {
    pinHandle = inPinHandle;

    Task_Params_init(&echoTaskParams);
    echoTaskParams.stackSize = RFEASYLINKECHO_TASK_STACK_SIZE;
    echoTaskParams.priority = RFEASYLINKECHO_TASK_PRIORITY;
    echoTaskParams.stack = &echoTaskStack;
    echoTaskParams.arg0 = (UInt)1000000;

    Task_construct(&echoTask, rfEasyLinkEchoRxFnx, &echoTaskParams, NULL);
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
