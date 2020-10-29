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

 /*
 *  ======== Per.c ========
 */

 /* BIOS Header files */
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/knl/Clock.h>

/* XDCtools Header files */
#include <xdc/runtime/Error.h>

/* EasyLink API Header files */ 
#include "easylink/EasyLink.h"

#include "at/AtParams.h"
#include "at/AtProcess.h"
#include "at/platform/inc/AtTerm.h"

#define MAX_PKT_SIZE    88

// PER test receiver statistics
typedef struct {
    uint8_t expectedSeqNum;
    uint64_t rcvdPkts;
    uint64_t lostPkts;
    uint16_t rssiSum;
} perRxStats_t;

AtProcess_Status Per_testTx(uint32_t burstSz, uint32_t burstInterval, uint32_t pktLen, uint32_t numPkts)
{
    EasyLink_TxPacket txPacket = { {0}, 0, 0, {0} };
    AtProcess_Status status = AtProcess_Status_EasyLinkError;
    uint8_t pktIdx;
    uint32_t burstIdx;
    uint32_t absTime;
    
    burstIdx = 0;

    txPacket.absTime = 0;

    if(pktLen > MAX_PKT_SIZE)
        pktLen = MAX_PKT_SIZE;

    //length including seq num
    txPacket.len = pktLen;

    //set address size to 0
    EasyLink_setCtrl(EasyLink_Ctrl_AddSize, 0);

    //seq number is the first byte of payload
    uint8_t* seqNumber = txPacket.payload;
    *seqNumber = 0;
    for(pktIdx = 1; pktIdx < txPacket.len; pktIdx++)
    {
        txPacket.payload[pktIdx] = pktIdx;
    }

    AtParams_easyLinkStatus = EasyLink_Status_Success;
    while( ((numPkts == 0) || (*seqNumber < numPkts)) &&
            (AtParams_easyLinkStatus == EasyLink_Status_Success))
    {
        if((AtParams_easyLinkStatus = EasyLink_transmit(&txPacket)) == EasyLink_Status_Success)
        {
            //Increase and print the SeqNumber
            AtTerm_sendStringUi8Value("TPER: ", *seqNumber, 16);
            AtTerm_sendString("\r");
            *seqNumber+=1;

            if(burstIdx == (burstSz-1))
            {
                //Start next burst
                //Next Tx Time in 4MHz Ticks
                uint32_t txDelay = EasyLink_ms_To_RadioTime(burstInterval);
                if(EasyLink_getAbsTime(&absTime) != EasyLink_Status_Success)
                {
                    // Problem getting absolute time
                }
                txPacket.absTime = absTime + txDelay;
                burstIdx = 0;
            }
            else
            {
                //Continue burst
                //Tx onterval within bust is set to 10ms
                txPacket.absTime = EasyLink_ms_To_RadioTime(10);
                burstIdx++;
            }
        }
    }

    if( AtParams_easyLinkStatus == EasyLink_Status_Success )
    {
        AtTerm_sendString("TPER: Done ");
        status = AtProcess_Status_Success;
    }
    else
    {
        AtTerm_sendString("TPER: ");
        status = AtProcess_Status_EasyLinkError;
    }

    return status;
}

uint32_t Per_testRx(uint32_t numPkts)
{
    uint32_t status = AtProcess_Status_EasyLinkError;
    perRxStats_t perRxStats = {0};
    EasyLink_RxPacket rxPacket = { {0}, 0, 0, 0, 0, {0} };

    //set address size to 0
    EasyLink_setCtrl(EasyLink_Ctrl_AddSize, 0);

    perRxStats.expectedSeqNum = 0;
    perRxStats.rcvdPkts = 0;
    perRxStats.lostPkts = 0;
    perRxStats.rssiSum = 0;

    rxPacket.rxTimeout = AtParams_rxTimeout;

    uint32_t easyLinkStatus = EasyLink_Status_Success;
    uint8_t *seqNumber = rxPacket.payload;
    while( ((numPkts == 0) || (*seqNumber < (numPkts-1))) &&
           (easyLinkStatus != EasyLink_Status_Rx_Timeout) )
    {
        rxPacket.absTime = 0;
        if ((easyLinkStatus = EasyLink_receive(&rxPacket)) == EasyLink_Status_Success)
        {
            if(perRxStats.expectedSeqNum != *seqNumber)
            {
                if(perRxStats.rcvdPkts != 0)
                {
                  perRxStats.lostPkts++;
                }
                perRxStats.expectedSeqNum = *seqNumber;
            }
            else
            {
                perRxStats.rssiSum += rxPacket.rssi;
                perRxStats.rcvdPkts++;
            }

            perRxStats.expectedSeqNum++;
        }
        else //!EasyLink_Status_Success
        {
            //Increase lost packet and print
            perRxStats.expectedSeqNum++;
            perRxStats.lostPkts++;
        }

        //Print results
        AtTerm_sendStringUi8Value("RPER: ", (uint8_t) *seqNumber, 16);
        AtTerm_sendStringUi16Value(", ", (uint32_t) perRxStats.rcvdPkts, 16);
        AtTerm_sendStringUi16Value(", ", (uint32_t) perRxStats.lostPkts, 16);
        AtTerm_sendStringI8Value(", ", (uint32_t) rxPacket.rssi, 10);
        AtTerm_sendString("\r");
    }

    if( (easyLinkStatus == EasyLink_Status_Success) && (perRxStats.lostPkts == 0) )
    {
        status = AtProcess_Status_Success;
        AtTerm_sendString("<RPER>: Done ");
    }
    else if(easyLinkStatus == EasyLink_Status_Rx_Timeout)
    {
        AtTerm_sendString("<RPER>: Timeout ");
        status = AtProcess_Status_EasyLinkError;
    }
    else
    {
        AtTerm_sendString("<RPER>: ");
        status = AtProcess_Status_EasyLinkError;
    }

    return status;
}
