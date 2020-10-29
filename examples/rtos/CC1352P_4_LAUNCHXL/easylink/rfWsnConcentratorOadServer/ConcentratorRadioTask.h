/*
 * Copyright (c) 2015-2017, Texas Instruments Incorporated
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

#ifndef TASKS_CONCENTRATORRADIOTASKTASK_H_
#define TASKS_CONCENTRATORRADIOTASKTASK_H_

#include "stdint.h"
#include "RadioProtocol.h"


enum ConcentratorRadioOperationStatus {
    ConcentratorRadioStatus_Success,
    ConcentratorRadioStatus_Failed,
    ConcentratorRadioStatus_FailedNotConnected,
};

/*
 * A union is used here instead of multiple structs because there are four
 * different packet types that could be received/transmitted. This avoids having
 * to create multiple function definitions to handlde each type when instead one
 * function definition could handle all packet types with a union.
 */
union ConcentratorPacket {
    struct PacketHeader header;
    struct AdcSensorPacket adcSensorPacket;
    struct DualModeSensorPacket dmSensorPacket;
    struct OadPacket oadPacket;
};

typedef void (*ConcentratorRadio_PacketReceivedCallback)(union ConcentratorPacket* packet, int8_t rssi);

/* Create the ConcentratorRadioTask and creates all TI-RTOS objects */
void ConcentratorRadioTask_init(void);

/* Register the packet received callback */
void ConcentratorRadioTask_registerPacketReceivedCallback(ConcentratorRadio_PacketReceivedCallback callback);

/* Send OAD Packet */
void ConcentratorRadioTask_sendNodeMsg(uint8_t *pAddress, uint8_t *oadMsg, uint8_t msgLen);

/* Abort Node Message from pending message */
void ConcentratorRadioTask_abortNodeMsg(void);

#endif /* TASKS_CONCENTRATORRADIOTASKTASK_H_ */
