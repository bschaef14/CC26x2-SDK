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
/* XDCtools Header files */
#include <xdc/std.h>
#include <xdc/runtime/System.h>

#include "oad/native_oad/oad_storage.h"
#include "ConcentratorRadioTask.h"

/* BIOS Header files */
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/knl/Event.h>

/* TI-RTOS Header files */
#include <ti/drivers/rf/RF.h>
#include <ti/drivers/PIN.h>

/* Board Header files */
#include "ti_drivers_config.h"

/* EasyLink API Header files */
#include "easylink/EasyLink.h"


/***** Defines *****/
#define CONCENTRATORRADIO_TASK_STACK_SIZE 1024
#define CONCENTRATORRADIO_TASK_PRIORITY   3

#define RADIO_EVENT_ALL                      0xFFFFFFFF
#define RADIO_EVENT_VALID_PACKET_RECEIVED    (uint32_t)(1 << 0)
#define RADIO_EVENT_INVALID_PACKET_RECEIVED  (uint32_t)(1 << 1)

#define CONCENTRATORRADIO_MAX_RETRIES 2
#define CONCENTRATORRADIO_FRAMEPENDING_DELAY_TIME_MS (5)


#define CONCENTRATOR_ACTIVITY_LED CONFIG_PIN_RLED

/***** Type declarations *****/



/***** Variable declarations *****/
static Task_Params concentratorRadioTaskParams;
Task_Struct concentratorRadioTask; /* not static so you can see in ROV */
static uint8_t concentratorRadioTaskStack[CONCENTRATORRADIO_TASK_STACK_SIZE];
Event_Struct radioOperationEvent;  /* not static so you can see in ROV */
static Event_Handle radioOperationEventHandle;



static ConcentratorRadio_PacketReceivedCallback packetReceivedCallback;
static union ConcentratorPacket latestRxPacket;
static EasyLink_TxPacket txRequest;
static bool txRequestPending;
static uint8_t concentratorAddress;
static int8_t latestRssi;


/***** Prototypes *****/
static void concentratorRadioTaskFunction(UArg arg0, UArg arg1);
static void rxDoneCallback(EasyLink_RxPacket * rxPacket, EasyLink_Status status);
static void notifyPacketReceived(union ConcentratorPacket* latestRxPacket);
static void sendAck(uint8_t latestSourceAddress);

/* Pin driver handle */
static PIN_Handle ledPinHandle;
static PIN_State ledPinState;

/* Configure LED Pin */
PIN_Config ledPinTable[] = {
        CONCENTRATOR_ACTIVITY_LED | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL | PIN_DRVSTR_MAX,
    PIN_TERMINATE
};

/***** Function definitions *****/
void ConcentratorRadioTask_init(void) {

    /* Open LED pins */
    ledPinHandle = PIN_open(&ledPinState, ledPinTable);
    if (!ledPinHandle)
    {
        System_abort("Error initializing board 3.3V domain pins\n");
    }

    /* set Sub1G Activity LED high */
    PIN_setOutputValue(ledPinHandle, CONCENTRATOR_ACTIVITY_LED, 1);

    /* Create event used internally for state changes */
    Event_Params eventParam;
    Event_Params_init(&eventParam);
    Event_construct(&radioOperationEvent, &eventParam);
    radioOperationEventHandle = Event_handle(&radioOperationEvent);

    /* Create the concentrator radio protocol task */
    Task_Params_init(&concentratorRadioTaskParams);
    concentratorRadioTaskParams.stackSize = CONCENTRATORRADIO_TASK_STACK_SIZE;
    concentratorRadioTaskParams.priority = CONCENTRATORRADIO_TASK_PRIORITY;
    concentratorRadioTaskParams.stack = &concentratorRadioTaskStack;
    Task_construct(&concentratorRadioTask, concentratorRadioTaskFunction, &concentratorRadioTaskParams, NULL);
}

void ConcentratorRadioTask_registerPacketReceivedCallback(ConcentratorRadio_PacketReceivedCallback callback) {
    packetReceivedCallback = callback;
}

void ConcentratorRadioTask_sendNodeMsg(uint8_t *pAddress, uint8_t *msg, uint8_t msgLen)
{
    if( (!txRequestPending) && msgLen < (EASYLINK_MAX_DATA_LENGTH -1) )
    {
        memcpy(txRequest.payload, msg, msgLen);
        txRequest.absTime = 0;
		txRequest.dstAddr[0] = *pAddress;
        txRequest.payload[RADIO_PACKET_SRCADDR_OFFSET] = concentratorAddress;
        txRequest.len = msgLen;
        txRequestPending = true;
    }
}

void ConcentratorRadioTask_abortNodeMsg(void)
{
    txRequestPending = false;
}

static void concentratorRadioTaskFunction(UArg arg0, UArg arg1)
{
    /* set Sub1G Activity LED low */
    PIN_setOutputValue(ledPinHandle, CONCENTRATOR_ACTIVITY_LED, 0);

    // Initialize the EasyLink parameters to their default values
    EasyLink_Params easyLink_params;
    EasyLink_Params_init(&easyLink_params);

#ifdef DEFINED_RADIO_EASYLINK_MODULATION
    // Change the modulation from the default found in ti_easylink_config.h
	easyLink_params.ui32ModType = DEFINED_RADIO_EASYLINK_MODULATION;
#endif

    /* Initialize EasyLink */
	if(EasyLink_init(&easyLink_params) != EasyLink_Status_Success){
		System_abort("EasyLink_init failed");
	}


    /* If you wich to use a frequency other than the default use
     * the below API
     * EasyLink_setFrequency(868000000);
     */

    /* Set concentrator address */;
    concentratorAddress = RADIO_CONCENTRATOR_ADDRESS;
    EasyLink_enableRxAddrFilter(&concentratorAddress, 1, 1);

    /* Enter receive */
    if(EasyLink_receiveAsync(rxDoneCallback, 0) != EasyLink_Status_Success) {
        System_abort("EasyLink_receiveAsync failed");
    }

    while (1)
    {
        uint32_t events = Event_pend(radioOperationEventHandle, 0, RADIO_EVENT_ALL, BIOS_WAIT_FOREVER);

        /* If valid packet received */
        if (events & RADIO_EVENT_VALID_PACKET_RECEIVED)
        {

            /* Send ack packet */
            sendAck(latestRxPacket.header.sourceAddress);

            if(txRequestPending)
            {
                if (EasyLink_transmit(&txRequest) != EasyLink_Status_Success)
                {
                    System_abort("EasyLink_transmit failed");
                }
                txRequestPending = false;
            }

            /* Call packet received callback */
            notifyPacketReceived(&latestRxPacket);

            /* Go back to RX */
            if(EasyLink_receiveAsync(rxDoneCallback, 0) != EasyLink_Status_Success) {
                System_abort("EasyLink_receiveAsync failed");
            }

            /* toggle Activity LED */
            PIN_setOutputValue(ledPinHandle, CONCENTRATOR_ACTIVITY_LED,
                    !PIN_getOutputValue(CONCENTRATOR_ACTIVITY_LED));
        }

        /* If invalid packet received */
        if (events & RADIO_EVENT_INVALID_PACKET_RECEIVED)
        {
            /* Go back to RX */
            if (EasyLink_receiveAsync(rxDoneCallback, 0) != EasyLink_Status_Success)
            {
                System_abort("EasyLink_receiveAsync failed");
            }
        }
    }
}

static void sendAck(uint8_t latestSourceAddress)
{
    EasyLink_TxPacket txAck;

    /* Set destinationAdress, but use EasyLink layers destination address capability */
    txAck.dstAddr[0] = latestSourceAddress;

    /* Copy ACK packet to payload, skipping the destination address byte.
     * Note that the EasyLink API will implicitly both add the length byte and the destination address byte. */
    //set source address
    txAck.payload[0] = concentratorAddress;
    txAck.payload[1] = RADIO_PACKET_TYPE_ACK_PACKET;
    txAck.payload[2] = txRequestPending;
    txAck.len = 3;

    /* Transmit immediately */
    txAck.absTime = 0;

    /* Send packet  */
    if (EasyLink_transmit(&txAck) != EasyLink_Status_Success)
    {
        System_abort("EasyLink_transmit failed");
    }
}

static void notifyPacketReceived(union ConcentratorPacket* latestRxPacket)
{
    if (packetReceivedCallback)
    {
        packetReceivedCallback(latestRxPacket, latestRssi);
    }
}

static void rxDoneCallback(EasyLink_RxPacket * rxPacket, EasyLink_Status status)
{
    union ConcentratorPacket* tmpRxPacket;

    /* If we received a packet successfully */
    if (status == EasyLink_Status_Success)
    {
        /* Save the latest RSSI, which is later sent to the receive callback */
        latestRssi = (int8_t)rxPacket->rssi;

        /* Check that this is a valid packet */
        tmpRxPacket = (union ConcentratorPacket*)(rxPacket->payload);

        /* If this is a known packet */
        if (tmpRxPacket->header.packetType == RADIO_PACKET_TYPE_ADC_SENSOR_PACKET)
        {
            /* Save packet */
            latestRxPacket.header.sourceAddress = rxPacket->payload[0];
            latestRxPacket.header.packetType = rxPacket->payload[1];
            latestRxPacket.adcSensorPacket.adcValue = (rxPacket->payload[2] << 8) | rxPacket->payload[3];

            /* Signal packet received */
            Event_post(radioOperationEventHandle, RADIO_EVENT_VALID_PACKET_RECEIVED);
        }
        else if (tmpRxPacket->header.packetType == RADIO_PACKET_TYPE_DM_SENSOR_PACKET)
        {
            /* Save packet */
            latestRxPacket.header.sourceAddress = rxPacket->payload[0];
            latestRxPacket.header.packetType = rxPacket->payload[1];
            latestRxPacket.dmSensorPacket.adcValue = (rxPacket->payload[2] << 8) | rxPacket->payload[3];
            latestRxPacket.dmSensorPacket.batt = (rxPacket->payload[4] << 8) | rxPacket->payload[5];
            latestRxPacket.dmSensorPacket.time100MiliSec = (rxPacket->payload[6] << 24) |
                                                           (rxPacket->payload[7] << 16) |
                                                           (rxPacket->payload[8] << 8) |
                                                            rxPacket->payload[9];
            latestRxPacket.dmSensorPacket.button = rxPacket->payload[10];

            /* Signal packet received */
            Event_post(radioOperationEventHandle, RADIO_EVENT_VALID_PACKET_RECEIVED);
        }
        else if (tmpRxPacket->header.packetType == RADIO_PACKET_TYPE_OAD_PACKET)
        {
            /* Save packet */
            latestRxPacket.header.sourceAddress = rxPacket->payload[0];
            latestRxPacket.header.packetType = rxPacket->payload[1];
#if (defined(DeviceFamily_CC13X2) || defined(DeviceFamily_CC26X2))
            memcpy(&latestRxPacket.oadPacket.oadPayload, &rxPacket->payload[2], (OADStorage_BLOCK_SIZE + 2));
#else
			memcpy(&latestRxPacket.oadPacket.oadPayload, &rxPacket->payload[2], (OAD_BLOCK_SIZE + 2 + 2));
#endif

            /* Signal packet received */
            Event_post(radioOperationEventHandle, RADIO_EVENT_VALID_PACKET_RECEIVED);
        }
        else
        {
            /* Signal invalid packet received */
            Event_post(radioOperationEventHandle, RADIO_EVENT_INVALID_PACKET_RECEIVED);
        }
    }
    else
    {
        /* Signal invalid packet received */
        Event_post(radioOperationEventHandle, RADIO_EVENT_INVALID_PACKET_RECEIVED);
    }
}
