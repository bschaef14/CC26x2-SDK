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
 *  ======== rfEasyLinkListenBeforeTalk_nortos.c ========
 */
/* Application header files */
#include <ti_radio_config.h>
#include "ti_drivers_config.h"

/* Standard C Libraries */
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

/* High-level Ti-Drivers */
#include <ti/drivers/pin/PINCC26XX.h>
#include <ti/drivers/power/PowerCC26XX.h>
#include <ti/drivers/rf/RF.h>
#include <ti/devices/DeviceFamily.h>

/* Driver Header files */
#include <ti/drivers/TRNG.h>
#include <ti/drivers/cryptoutils/cryptokey/CryptoKeyPlaintext.h>

/* EasyLink API Header files */
#include "easylink/EasyLink.h"

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

static volatile bool lbtDoneFlag;

uint32_t getRandomNumber( void )
{
    int_fast16_t result = TRNG_STATUS_ERROR;
    uint8_t breakCounter = MAX_TRNG_RETRIES;
    do
    {
        if(0U == breakCounter--)
        {
            // Unable to generate a random value
            while(1);
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

    lbtDoneFlag = true;
}

void *mainThread(void *arg0)
{
    uint32_t absTime;
    EasyLink_TxPacket lbtPacket = { {0}, 0, 0, {0} };

    /* Set power dependency for the TRNG */
    Power_setDependency(PowerCC26XX_PERIPH_TRNG);

    /* Open LED pins */
    pinHandle = PIN_open(&pinState, pinTable);
    if(!pinHandle)
    {
        while(1);
    }

    /* Clear LED pins */
    PIN_setOutputValue(pinHandle, CONFIG_PIN_GLED, 0);
    PIN_setOutputValue(pinHandle, CONFIG_PIN_RLED, 0);

    /* Set the transmission flag to its default state */
    lbtDoneFlag = false;

    /* Initialize the TRNG driver and a blank crypto key */
    TRNG_init();
    TRNG_Params trngParams;
    TRNG_Params_init(&trngParams);
    trngParams.returnBehavior = TRNG_RETURN_BEHAVIOR_POLLING;
    trngHandle = TRNG_open(CONFIG_TRNG_EASYLINK, &trngParams);
    if(NULL == trngHandle)
    {
        // Failed to init TRNG driver
        while(1);
    }
    int_fast16_t result = CryptoKeyPlaintext_initBlankKey(&entropyKey, entropyBuffer, KEY_LENGTH_BYTES);
    if(CryptoKey_STATUS_SUCCESS != result)
    {
        // Unable to create a blank crypto key
        while(1);
    }

    // Initialize the EasyLink parameters to their default values
    EasyLink_Params easyLink_params;
    EasyLink_Params_init(&easyLink_params);

    // Change the RNG function used for clear channel assessment
    easyLink_params.pGrnFxn = (EasyLink_GetRandomNumber)getRandomNumber;

    /* Initialize EasyLink */
    if(EasyLink_init(&easyLink_params) != EasyLink_Status_Success)
    {
        // EasyLink_init failed
        while(1);
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

        // Set the Transmit done flag to false, callback will set it to true
        lbtDoneFlag = false;
        EasyLink_transmitCcaAsync(&lbtPacket, lbtDoneCb);

        /* Wait forever for TX to complete */
        while(lbtDoneFlag == false){};
    }
}


