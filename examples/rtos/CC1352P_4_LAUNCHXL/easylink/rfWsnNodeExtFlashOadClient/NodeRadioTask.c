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

/***** Includes *****/
/* XDCtools Header files */
#include <xdc/std.h>
#include <stdlib.h>
#include <xdc/runtime/System.h>

/* BIOS Header files */
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/knl/Event.h>
#include <ti/sysbios/knl/Clock.h>

/* TI-RTOS Header files */
#include <ti/drivers/Power.h>
#include <ti/drivers/power/PowerCC26XX.h>
#include <ti/drivers/rf/RF.h>
#include <ti/drivers/PIN.h>

/* Board Header files */
#include "ti_drivers_config.h"

/* EasyLink API Header files */
#include "easylink/EasyLink.h"

/* Application Header files */
#include "RadioProtocol.h"
#include "NodeRadioTask.h"
#include "oad/native_oad/oad_protocol.h"

#ifdef OAD_P_APP
#include "NodeStorage.h"
#endif

#include <ti/devices/DeviceFamily.h>
#include DeviceFamily_constructPath(driverlib/aon_batmon.h)
#include DeviceFamily_constructPath(driverlib/trng.h)

/***** Defines *****/
#define NODERADIO_TASK_STACK_SIZE 1024
#define NODERADIO_TASK_PRIORITY   3

#define RADIO_EVENT_ALL                 0xFFFFFFFF
#define RADIO_EVENT_SEND_ADC_DATA       (uint32_t)(1 << 0)
#define RADIO_EVENT_DATA_ACK_RECEIVED   (uint32_t)(1 << 1)
#define RADIO_EVENT_ACK_TIMEOUT         (uint32_t)(1 << 2)
#define RADIO_EVENT_MSG_RECEIVED        (uint32_t)(1 << 3)
#define RADIO_EVENT_SEND_FAIL           (uint32_t)(1 << 4)

#define NODERADIO_MAX_RETRIES 2
#define NORERADIO_ACK_TIMEOUT_TIME_MS (20)
#define NORERADIO_FRAMEPENDING_TIMEOUT_TIME_MS (20)

/***** Type declarations *****/
struct RadioOperation {
    EasyLink_TxPacket easyLinkTxPacket;
    uint8_t retriesDone;
    uint8_t maxNumberOfRetries;
    uint32_t ackTimeoutMs;
    enum NodeRadioOperationStatus result;
};


/***** Variable declarations *****/
static Task_Params nodeRadioTaskParams;
Task_Struct nodeRadioTask;        /* not static so you can see in ROV */
static uint8_t nodeRadioTaskStack[NODERADIO_TASK_STACK_SIZE];
Semaphore_Struct radioAccessSem;  /* not static so you can see in ROV */
static Semaphore_Handle radioAccessSemHandle;
Event_Struct radioOperationEvent; /* not static so you can see in ROV */
static Event_Handle radioOperationEventHandle;
Semaphore_Struct radioResultSem;  /* not static so you can see in ROV */
static Semaphore_Handle radioResultSemHandle;
static struct RadioOperation currentRadioOperation;
static uint16_t adcData;
static uint8_t nodeAddress = 0;
static struct DualModeSensorPacket dmSensorPacket;
static EasyLink_RxPacket latestRxPacket;

/* previous Tick count used to calculate uptime for the Sub1G packet */
static uint32_t prevTicks;
static bool framePending;

/* Pin driver handle */
extern PIN_Handle ledPinHandle;

/***** Prototypes *****/
static void nodeRadioTaskFunction(UArg arg0, UArg arg1);
static void returnRadioOperationStatus(enum NodeRadioOperationStatus status);
static void sendDmPacket(struct DualModeSensorPacket sensorPacket, uint8_t maxNumberOfRetries, uint32_t ackTimeoutMs);
static void resendPacket();
static void rxDoneCallback(EasyLink_RxPacket * rxPacket, EasyLink_Status status);

/***** Function definitions *****/
void NodeRadioTask_init(void) {

    /* Create semaphore used for exclusive radio access */
    Semaphore_Params semParam;
    semParam.mode = Semaphore_Mode_BINARY;
    Semaphore_Params_init(&semParam);
    Semaphore_construct(&radioAccessSem, 1, &semParam);
    radioAccessSemHandle = Semaphore_handle(&radioAccessSem);

    /* Create semaphore used for callers to wait for result */
    Semaphore_construct(&radioResultSem, 0, &semParam);
    radioResultSemHandle = Semaphore_handle(&radioResultSem);

    /* Create event used internally for state changes */
    Event_Params eventParam;
    Event_Params_init(&eventParam);
    Event_construct(&radioOperationEvent, &eventParam);
    radioOperationEventHandle = Event_handle(&radioOperationEvent);

    /* Create the radio protocol task */
    Task_Params_init(&nodeRadioTaskParams);
    nodeRadioTaskParams.stackSize = NODERADIO_TASK_STACK_SIZE;
    nodeRadioTaskParams.priority = NODERADIO_TASK_PRIORITY;
    nodeRadioTaskParams.stack = &nodeRadioTaskStack;
    Task_construct(&nodeRadioTask, nodeRadioTaskFunction, &nodeRadioTaskParams, NULL);
}

uint8_t nodeRadioTask_getNodeAddr(void) {
    return nodeAddress;
}

static void nodeRadioTaskFunction(UArg arg0, UArg arg1)
{
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


    /* If you wish to use a frequency other than the default use
     * the below API
     * EasyLink_setFrequency(868000000);
     */

	bool useRNGAddr = true;

#ifdef OAD_P_APP

	NodeStorage_init();

	/* Check NVS for connection data. If found load that connection */
	if (NodeStorage_verifyStorage())
	{
	    NodeStorage_Status stat;
	    stat = NodeStorage_read(NODESTORAGE_NODE_ADDR_OFFSET, &nodeAddress, 1);

	    if (stat == NodeStorage_Success)
	    {
	        useRNGAddr = false;
	    }
	}

#endif /* OAD_P_APP */

	if (useRNGAddr)
	{
        /* Use the True Random Number Generator to generate sensor node address
         * randomly
         */
        Power_setDependency(PowerCC26XX_PERIPH_TRNG);
        TRNGEnable();
        /* Do not accept the same address as the concentrator, in that case get
         * a new random value
         */
        do
        {
            while (!(TRNGStatusGet() & TRNG_NUMBER_READY))
            {
                //wait for random number generator
            }
            nodeAddress = (uint8_t)TRNGNumberGet(TRNG_LOW_WORD);
        } while (nodeAddress == RADIO_CONCENTRATOR_ADDRESS);
        TRNGDisable();
        Power_releaseDependency(PowerCC26XX_PERIPH_TRNG);
	}

    /* Set the filter to the generated random address */
    if (EasyLink_enableRxAddrFilter(&nodeAddress, 1, 1) != EasyLink_Status_Success)
    {
        System_abort("EasyLink_enableRxAddrFilter failed");
    }

    /* Setup ADC sensor packet */
    dmSensorPacket.header.sourceAddress = nodeAddress;
    dmSensorPacket.header.packetType = RADIO_PACKET_TYPE_DM_SENSOR_PACKET;


    /* Initialize previous Tick count used to calculate uptime for the TLM beacon */
    prevTicks = Clock_getTicks();

#ifdef OAD_P_APP

    /* Send response to OAD reset request if connection was loaded from NVS */
    if (!useRNGAddr)
    {
        uint8_t serverAddr;
        NodeStorage_read(NODESTORAGE_SERVER_ADDR_OFFSET, &serverAddr, 1);
        OADProtocol_sendOadResetRsp(&serverAddr);

        NodeStorage_eraseAll();
    }

#endif /* OAD_P_APP */

    /* Enter main task loop */
    while (1)
    {
        /* Wait for an event */
        uint32_t events = Event_pend(radioOperationEventHandle, 0, RADIO_EVENT_ALL, BIOS_WAIT_FOREVER);

        /* If we should send ADC data */
        if (events & RADIO_EVENT_SEND_ADC_DATA)
        {
            uint32_t currentTicks;

            currentTicks = Clock_getTicks();
            //check for wrap around
            if (currentTicks > prevTicks)
            {
                //calculate time since last reading in 0.1s units
                dmSensorPacket.time100MiliSec += ((currentTicks - prevTicks) * Clock_tickPeriod) / 100000;
            }
            else
            {
                //calculate time since last reading in 0.1s units
                dmSensorPacket.time100MiliSec += ((prevTicks - currentTicks) * Clock_tickPeriod) / 100000;
            }
            prevTicks = currentTicks;

            dmSensorPacket.batt = AONBatMonBatteryVoltageGet();
            dmSensorPacket.adcValue = adcData;
            dmSensorPacket.button = !PIN_getInputValue(CONFIG_PIN_BTN1);

            sendDmPacket(dmSensorPacket, NODERADIO_MAX_RETRIES, NORERADIO_ACK_TIMEOUT_TIME_MS);
        }

        /* If we get an ACK from the concentrator */
        if (events & RADIO_EVENT_DATA_ACK_RECEIVED)
        {
            /* check if a frame is being sent */
            if(framePending)
            {
                /* Set frame pending timeout */
                EasyLink_setCtrl(EasyLink_Ctrl_AsyncRx_TimeOut,
                                 EasyLink_ms_To_RadioTime(NORERADIO_FRAMEPENDING_TIMEOUT_TIME_MS));

                /* Enter RX and wait for pending frame */
                if (EasyLink_receiveAsync(rxDoneCallback, 0) != EasyLink_Status_Success)
                {
                    System_abort("EasyLink_receiveAsync failed");
                }
            }

            returnRadioOperationStatus(NodeRadioStatus_Success);
        }

        /* If we get an ACK from the concentrator */
        if (events & RADIO_EVENT_MSG_RECEIVED)
        {
            /* is a known message */
            if(latestRxPacket.payload[RADIO_PACKET_PKTTYPE_OFFSET] == RADIO_PACKET_TYPE_OAD_PACKET)
            {
                OADProtocol_ParseIncoming(&latestRxPacket.payload[RADIO_PACKET_SRCADDR_OFFSET],
                                          &latestRxPacket.payload[RADIO_PACKET_PAYLOAD_OFFSET]);
            }
        }

        /* If we get an ACK timeout */
        if (events & RADIO_EVENT_ACK_TIMEOUT)
        {
            /* If we haven't resent it the maximum number of times yet, then resend packet */
            if (!framePending && currentRadioOperation.retriesDone < currentRadioOperation.maxNumberOfRetries)
            {
                resendPacket();
            }
            else if(!framePending)
            {
                /* Else return send fail */
                Event_post(radioOperationEventHandle, RADIO_EVENT_SEND_FAIL);
            }
            /* if we missed a framePending just ignore and go back to sleep */
        }

        /* If send fail */
        if (events & RADIO_EVENT_SEND_FAIL)
        {
            returnRadioOperationStatus(NodeRadioStatus_Failed);
        }

    }
}

enum NodeRadioOperationStatus NodeRadioTask_sendAdcData(uint16_t data)
{
    enum NodeRadioOperationStatus status;

    /* Get radio access sempahore */
    Semaphore_pend(radioAccessSemHandle, BIOS_WAIT_FOREVER);

    /* Save data to send */
    adcData = data;

    /* Raise RADIO_EVENT_SEND_ADC_DATA event */
    Event_post(radioOperationEventHandle, RADIO_EVENT_SEND_ADC_DATA);

    /* Wait for result */
    Semaphore_pend(radioResultSemHandle, BIOS_WAIT_FOREVER);

    /* Get result */
    status = currentRadioOperation.result;

    /* if the ack had a frame pending then the radioAccess
     * sem will be released when the frame is received or the
     * timed out
     */
    if(!framePending)
    {
        /* Return radio access semaphore */
        Semaphore_post(radioAccessSemHandle);
    }


    return status;
}

enum NodeRadioOperationStatus NodeRadioTask_sendOadMsg(uint8_t dstAddr, uint8_t* pPacket, uint8_t packetLen)
{
    enum NodeRadioOperationStatus status = NodeRadioStatus_Success;

    /* Get radio access sempahore */
    Semaphore_pend(radioAccessSemHandle, BIOS_WAIT_FOREVER);

    /* Set destination address in EasyLink API */
    currentRadioOperation.easyLinkTxPacket.dstAddr[0] = dstAddr;
    /* Set spurce address in payload */
    pPacket[RADIO_PACKET_SRCADDR_OFFSET] = nodeAddress;

    memcpy( currentRadioOperation.easyLinkTxPacket.payload,
            pPacket, packetLen);

    currentRadioOperation.easyLinkTxPacket.len = packetLen;

    /* No retries - fire and forget, retires handle by OAD Client */
    currentRadioOperation.maxNumberOfRetries = 0;
    currentRadioOperation.ackTimeoutMs = 0;
    currentRadioOperation.retriesDone = 0;

    /* Send packet  */
    if (EasyLink_transmit(&currentRadioOperation.easyLinkTxPacket) != EasyLink_Status_Success)
    {
        System_abort("EasyLink_transmit failed");
    }

    /* Return radio access semaphore */
    Semaphore_post(radioAccessSemHandle);

    return status;
}

static void returnRadioOperationStatus(enum NodeRadioOperationStatus result)
{
    /* Save result */
    currentRadioOperation.result = result;

    /* Post result semaphore */
    Semaphore_post(radioResultSemHandle);
}

static void sendDmPacket(struct DualModeSensorPacket sensorPacket, uint8_t maxNumberOfRetries, uint32_t ackTimeoutMs)
{
    /* Set destination address in EasyLink API */
    currentRadioOperation.easyLinkTxPacket.dstAddr[0] = RADIO_CONCENTRATOR_ADDRESS;

    /* Copy ADC packet to payload
     * Note that the EasyLink API will implcitily both add the length byte and the destination address byte. */
    currentRadioOperation.easyLinkTxPacket.payload[0] = dmSensorPacket.header.sourceAddress;
    currentRadioOperation.easyLinkTxPacket.payload[1] = dmSensorPacket.header.packetType;
    currentRadioOperation.easyLinkTxPacket.payload[2] = (dmSensorPacket.adcValue & 0xFF00) >> 8;
    currentRadioOperation.easyLinkTxPacket.payload[3] = (dmSensorPacket.adcValue & 0xFF);
    currentRadioOperation.easyLinkTxPacket.payload[4] = (dmSensorPacket.batt & 0xFF00) >> 8;
    currentRadioOperation.easyLinkTxPacket.payload[5] = (dmSensorPacket.batt & 0xFF);
    currentRadioOperation.easyLinkTxPacket.payload[6] = (dmSensorPacket.time100MiliSec & 0xFF000000) >> 24;
    currentRadioOperation.easyLinkTxPacket.payload[7] = (dmSensorPacket.time100MiliSec & 0x00FF0000) >> 16;
    currentRadioOperation.easyLinkTxPacket.payload[8] = (dmSensorPacket.time100MiliSec & 0xFF00) >> 8;
    currentRadioOperation.easyLinkTxPacket.payload[9] = (dmSensorPacket.time100MiliSec & 0xFF);
    currentRadioOperation.easyLinkTxPacket.payload[10] = dmSensorPacket.button;

    currentRadioOperation.easyLinkTxPacket.len = sizeof(struct DualModeSensorPacket);

    /* Setup retries */
    currentRadioOperation.maxNumberOfRetries = maxNumberOfRetries;
    currentRadioOperation.ackTimeoutMs = ackTimeoutMs;
    currentRadioOperation.retriesDone = 0;
    EasyLink_setCtrl(EasyLink_Ctrl_AsyncRx_TimeOut, EasyLink_ms_To_RadioTime(ackTimeoutMs));

    /* Send packet  */
    if (EasyLink_transmit(&currentRadioOperation.easyLinkTxPacket) != EasyLink_Status_Success)
    {
        System_abort("EasyLink_transmit failed");
    }

    /* Enter RX */
    if (EasyLink_receiveAsync(rxDoneCallback, 0) != EasyLink_Status_Success)
    {
        System_abort("EasyLink_receiveAsync failed");
    }
}

static void resendPacket()
{
    /* Send packet  */
    if (EasyLink_transmit(&currentRadioOperation.easyLinkTxPacket) != EasyLink_Status_Success)
    {
        System_abort("EasyLink_transmit failed");
    }

    /* Enter RX and wait for ACK with timeout */
    if (EasyLink_receiveAsync(rxDoneCallback, 0) != EasyLink_Status_Success)
    {
        System_abort("EasyLink_receiveAsync failed");
    }

    /* Increase retries by one */
    currentRadioOperation.retriesDone++;
}

static void rxDoneCallback(EasyLink_RxPacket * rxPacket, EasyLink_Status status)
{
    struct PacketHeader* packetHeader;
    bool localFramePending = framePending;

    if(framePending)
    {
        /*
         * if we had an Rx, Rx error or Rx timeout while there
         * was a frame pending release the RadioAccess sem
         */
        Semaphore_post(radioAccessSemHandle);
        framePending = 0;
    }

    /* If this callback is called because of a packet received */
    if (status == EasyLink_Status_Success)
    {
        /* Check the payload header */
        packetHeader = (struct PacketHeader*)rxPacket->payload;

        /* Check if this is an ACK packet */
        if (packetHeader->packetType == RADIO_PACKET_TYPE_ACK_PACKET)
        {
            framePending = rxPacket->payload[2];

            /* Signal ACK packet received */
            Event_post(radioOperationEventHandle, RADIO_EVENT_DATA_ACK_RECEIVED);
        }
        /* Check if this is an ACK packet */
        else if (localFramePending)
        {
            memcpy(&latestRxPacket, rxPacket, (rxPacket->len +
                    (sizeof(EasyLink_RxPacket) - EASYLINK_MAX_DATA_LENGTH)));

            /* Signal ACK packet received */
            Event_post(radioOperationEventHandle, RADIO_EVENT_MSG_RECEIVED);
        }
        else
        {
            /* Packet Error, treat as a Timeout and Post a RADIO_EVENT_ACK_TIMEOUT
               event */
            Event_post(radioOperationEventHandle, RADIO_EVENT_ACK_TIMEOUT);
        }
    }
    /* did the Rx timeout */
    else if(status == EasyLink_Status_Rx_Timeout)
    {
        /* Post a RADIO_EVENT_ACK_TIMEOUT event */
        Event_post(radioOperationEventHandle, RADIO_EVENT_ACK_TIMEOUT);
    }
    else
    {
        /* The Ack reception may have been corrupted causing an error.
         * Treat this as a timeout
         */
        Event_post(radioOperationEventHandle, RADIO_EVENT_ACK_TIMEOUT);
    }
}
