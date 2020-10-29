/*
 * Copyright (c) 2017-2019, Texas Instruments Incorporated
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
 *  ======== rfEasyLinkListenBeforeTalk.c ========
 */
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
#include <ti/drivers/power/PowerCC26XX.h>

/* Board Header files */
#include "ti_drivers_config.h"

/* EasyLink API Header files */
#include "easylink/EasyLink.h"

/* Application header files */
#include <ti_radio_config.h>

/* Runtime Library Header Files */
#include <stdlib.h>
#include <string.h>

/* Driverlib Header files */
#include <ti/devices/DeviceFamily.h>

/* Driver Header files */
#include <ti/drivers/TRNG.h>
#include <ti/drivers/cryptoutils/cryptokey/CryptoKeyPlaintext.h>

#define RFEASYLINKLBT_TASK_STACK_SIZE       (1024U)
#define RFEASYLINKLBT_TASK_PRIORITY         (2U)
#define RFEASYLINKLBT_PAYLOAD_LENGTH        (30U)

#define KEY_LENGTH_BYTES                    (4U)
#define MAX_TRNG_RETRIES                    (2U) // Max attempts to generate a random number

/*
 * Set to 1 if you want to attempt to retransmit a packet that couldn't be
 * transmitted after the CCA
 */
#define RFEASYLINKLBT_RETRANSMIT_PACKETS    1

#if RFEASYLINKLBT_RETRANSMIT_PACKETS
bool bAttemptRetransmission = false;
#endif // RFEASYLINKLBT_RETRANSMIT_PACKETS

Task_Struct lbtTask;                 /* not static so you can see in ROV */
static Task_Params lbtTaskParams;
static uint8_t lbtTaskStack[RFEASYLINKLBT_TASK_STACK_SIZE];

/* PIN driver handle */
static PIN_Handle pinHandle;
static PIN_State pinState;

/* TRNG driver handle */
static TRNG_Handle trngHandle;

/* Crypto Key driver variables */
static CryptoKey entropyKey;
uint8_t entropyBuffer[KEY_LENGTH_BYTES];

/*
 * Application LED pin configuration table:
 *  - All board LEDs are off
 */
PIN_Config pinTable[] = {
    CONFIG_PIN_GLED | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL | PIN_DRVSTR_MAX,
    CONFIG_PIN_RLED | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL | PIN_DRVSTR_MAX,
    PIN_TERMINATE
};

static uint16_t seqNumber;

static Semaphore_Handle lbtDoneSem;

uint32_t getRandomNumber( void )
{
    int_fast16_t result = TRNG_STATUS_ERROR;
    uint8_t breakCounter = MAX_TRNG_RETRIES;
    do
    {
        if(0U == breakCounter--)
        {
            System_abort("Unable to generate a random value");
        }
        else
        {
            result = TRNG_generateEntropy(trngHandle, &entropyKey);
        }
    }while(TRNG_STATUS_SUCCESS != result);

    return(*((uint32_t *)entropyBuffer));
}

void lbtDoneCb(EasyLink_Status status)
{
    if (status == EasyLink_Status_Success)
    {
        /* Toggle GLED to indicate TX */
        PIN_setOutputValue(pinHandle, CONFIG_PIN_GLED, !PIN_getOutputValue(CONFIG_PIN_GLED));
#if RFEASYLINKLBT_RETRANSMIT_PACKETS
        bAttemptRetransmission = false;
#endif // RFEASYLINKLBT_RETRANSMIT_PACKETS
    }
    else if (status == EasyLink_Status_Busy_Error)
    {
        /* Toggle RLED to indicate maximum retries reached */
        PIN_setOutputValue(pinHandle, CONFIG_PIN_RLED, !PIN_getOutputValue(CONFIG_PIN_RLED));

#if RFEASYLINKLBT_RETRANSMIT_PACKETS
        bAttemptRetransmission = true;
#endif // RFEASYLINKLBT_RETRANSMIT_PACKETS
    }
    else
    {
        /* Toggle GLED and RLED to indicate error */
        PIN_setOutputValue(pinHandle, CONFIG_PIN_GLED, !PIN_getOutputValue(CONFIG_PIN_GLED));
        PIN_setOutputValue(pinHandle, CONFIG_PIN_RLED, !PIN_getOutputValue(CONFIG_PIN_RLED));
    }

    Semaphore_post(lbtDoneSem);
}

static void rfEasyLinkLbtFnx(UArg arg0, UArg arg1)
{
    uint32_t absTime;
    EasyLink_TxPacket lbtPacket = { {0}, 0, 0, {0} };

    /* Create a semaphore */
    Semaphore_Params params;
    Error_Block eb;

    /* Init params */
    Semaphore_Params_init(&params);
    Error_init(&eb);

    /* Create a semaphore instance */
    lbtDoneSem = Semaphore_create(0, &params, &eb);
    if(lbtDoneSem == NULL)
    {
        System_abort("Semaphore creation failed");
    }

    /* Initialize the TRNG driver and a blank crypto key */
    TRNG_init();
    TRNG_Params trngParams;
    TRNG_Params_init(&trngParams);
    trngParams.returnBehavior = TRNG_RETURN_BEHAVIOR_POLLING;
    trngHandle = TRNG_open(CONFIG_TRNG_EASYLINK, &trngParams);
    if(NULL == trngHandle)
    {
        System_abort("Failed to init TRNG driver");
    }
    int_fast16_t result = CryptoKeyPlaintext_initBlankKey(&entropyKey, entropyBuffer, KEY_LENGTH_BYTES);
    if(CryptoKey_STATUS_SUCCESS != result)
    {
        System_abort("Unable to create a blank crypto key");
    }

    // Initialize the EasyLink parameters to their default values
    EasyLink_Params easyLink_params;
    EasyLink_Params_init(&easyLink_params);

    // Change the RNG function used for clear channel assessment
    easyLink_params.pGrnFxn = (EasyLink_GetRandomNumber)getRandomNumber;

    /* Initialize EasyLink */
    if(EasyLink_init(&easyLink_params) != EasyLink_Status_Success)
    {
        System_abort("EasyLink_init failed");
    }

    /*
     * If you wish to use a frequency other than the default, use
     * the following API:
     * EasyLink_setFrequency(868000000);
     */

    while(1)
    {
#if RFEASYLINKLBT_RETRANSMIT_PACKETS
        if(bAttemptRetransmission == false)
        {
#endif // RFEASYLINKLBT_RETRANSMIT_PACKETS
            // zero out the packet
            memset(&lbtPacket, 0, sizeof(EasyLink_TxPacket));

            /* Create packet with incrementing sequence number and random payload */
            lbtPacket.payload[0] = (uint8_t)(seqNumber >> 8);
            lbtPacket.payload[1] = (uint8_t)(seqNumber++);

            uint8_t i;
            for(i = 2; i < RFEASYLINKLBT_PAYLOAD_LENGTH; i++)
            {
                lbtPacket.payload[i] = rand();
            }

            lbtPacket.len = RFEASYLINKLBT_PAYLOAD_LENGTH;

            /*
             * Address filtering is enabled by default on the Rx device with the
             * an address of 0xAA. This device must set the dstAddr accordingly.
             */
            lbtPacket.dstAddr[0] = 0xaa;

            /* Set Tx absolute time to current time + 100ms */
            if(EasyLink_getAbsTime(&absTime) != EasyLink_Status_Success)
            {
                // Problem getting absolute time
            }
            lbtPacket.absTime = absTime + EasyLink_ms_To_RadioTime(100);
#if RFEASYLINKLBT_RETRANSMIT_PACKETS
        }
#endif // RFEASYLINKLBT_RETRANSMIT_PACKETS

        EasyLink_transmitCcaAsync(&lbtPacket, lbtDoneCb);

        /* Wait forever for TX to complete */
        Semaphore_pend(lbtDoneSem, BIOS_WAIT_FOREVER);
    }
}

void lbtTask_init(PIN_Handle inPinHandle)
{
    pinHandle = inPinHandle;

    Task_Params_init(&lbtTaskParams);
    lbtTaskParams.stackSize = RFEASYLINKLBT_TASK_STACK_SIZE;
    lbtTaskParams.priority  = RFEASYLINKLBT_TASK_PRIORITY;
    lbtTaskParams.stack     = (void *)&lbtTaskStack[0];
    lbtTaskParams.arg0      = (UInt)1000000;

    Task_construct(&lbtTask, rfEasyLinkLbtFnx, &lbtTaskParams, NULL);
}

/*
 *  ======== main ========
 */
int main(void)
{
    /* Call driver init functions */
    Board_initGeneral();

    /* Set power dependency for the TRNG */
    Power_setDependency(PowerCC26XX_PERIPH_TRNG);

    /* Open LED pins */
    pinHandle = PIN_open(&pinState, pinTable);
    Assert_isTrue(pinHandle != NULL, NULL);

    /* Clear LED pins */
    PIN_setOutputValue(pinHandle, CONFIG_PIN_GLED, 0);
    PIN_setOutputValue(pinHandle, CONFIG_PIN_RLED, 0);

    lbtTask_init(pinHandle);

    /* Start BIOS */
    BIOS_start();

    return(0);
}

