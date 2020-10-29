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

/***************************************************************************************************
 * INCLUDES
 ***************************************************************************************************/
#include <stdlib.h> /* atoi */
#include <string.h> /* memcpy */

#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Task.h>
#include <xdc/runtime/Error.h>

#include <ti/devices/DeviceFamily.h>
#include DeviceFamily_constructPath(inc/hw_ccfg.h)
#include DeviceFamily_constructPath(inc/hw_ccfg_simple_struct.h)
#include DeviceFamily_constructPath(driverlib/sys_ctrl.h)

#include "at/AtProcess.h"
#include "at/AtControl.h"
#include "at/platform/inc/AtTerm.h"
#include "at/platform/tirtos/DbgPrint.h"
#include "at/AtParams.h"

#include "Per.h"

#include "easylink/EasyLink.h"

/***************************************************************************************************
 * MACROS
 ***************************************************************************************************/

/***************************************************************************************************
 * CONSTANTS
 ***************************************************************************************************/

/***************************************************************************************************
 *                                         GLOBAL VARIABLES
 ***************************************************************************************************/

/***************************************************************************************************
 *                                          LOCAL FUNCTIONS
 ***************************************************************************************************/
static AtProcess_Status processRx(EasyLink_RxPacket rxPacket);
static AtProcess_Status atCtrlrFInit(char* paramStr, uint32_t pramLen);
static AtProcess_Status atCtrlRxMsg(char* paramStr, uint32_t pramLen);
static AtProcess_Status atCtrlTxMsg(char* paramStr, uint32_t pramLen);
static AtProcess_Status atCtrlReset(char* paramStr, uint32_t pramLen);

static AtCommand_t atControlCmds[] = {
        { "I", atCtrlrFInit },
        { "i", atCtrlrFInit },
        { "RX", atCtrlRxMsg },
        { "rx", atCtrlRxMsg },
        { "TX", atCtrlTxMsg },
        { "tx", atCtrlTxMsg },
        { "rs", atCtrlReset },
        { "RS", atCtrlReset },
};

static AtProcess_Status processRx(EasyLink_RxPacket rxPacket)
{
    AtProcess_Status status = AtProcess_Status_EasyLinkError;
    char* str;

    AtTerm_sendString("RX: ");

    if (AtParams_binDataMode == 1)
    {
        uint8_t msgIdx;
        for(msgIdx = 0; msgIdx < rxPacket.len; msgIdx++)
        {
            AtTerm_sendStringUi8Value("",rxPacket.payload[msgIdx], 16);
        }
        status = AtProcess_Status_Success;
    } else
    {
        str = malloc(rxPacket.len + 1);
        if(str != NULL)
        {
            memcpy(str, &rxPacket.payload, rxPacket.len);
            //null terminate
            str[rxPacket.len] = '\0';
            AtTerm_sendString(str);
            status = AtProcess_Status_Success;
            free(str);
        }
    }
    AtTerm_sendString(" ");

    AtParams_lastRxTime = rxPacket.absTime;
    AtParams_lastRssi = rxPacket.rssi;
    memcpy(AtParams_lastRxDstAddr, rxPacket.dstAddr, AtParams_addrSize);

    return status;
}

static AtProcess_Status atCtrlrFInit(char* paramStr, uint32_t pramLen)
{
    AtProcess_Status status = AtProcess_Status_EasyLinkError;

    DbgPrint_printf("atCtrlStartNetwork[%d]: %s\n", pramLen, (char*) paramStr);
	
	EasyLink_Params easyLink_params;
    EasyLink_Params_init(&easyLink_params);
    if(EasyLink_getCtrl(EasyLink_Ctrl_AddSize, (uint32_t *)&AtParams_addrSize) != EasyLink_Status_Success)
    {
        // If unable to successfully read EasyLink default address size, assume it
        // is 0 and zero initialize the txAddr filter
        AtParams_addrSize = 0U;
    }
    if(AtParams_addrSize != 0U)
    {
        memcpy(AtParams_txAddr, &((uint8_t [])EASYLINK_ADDR_FILTER_TABLE), AtParams_addrSize);
    }
    else
    {
        memset(AtParams_txAddr, 0U, EASYLINK_MAX_ADDR_SIZE);
    }

    easyLink_params.ui32ModType = (EasyLink_PhyType) (atoi(&(paramStr[0])));

    AtParams_easyLinkStatus = EasyLink_init(&easyLink_params);

    if(AtParams_easyLinkStatus == EasyLink_Status_Success)
    {
        status = AtProcess_Status_Success;
    }

    return status;
}

static AtProcess_Status atCtrlRxMsg(char* paramStr, uint32_t pramLen)
{
    uint32_t absTime;
    AtProcess_Status status = AtProcess_Status_ParamError;
    EasyLink_RxPacket rxPacket = { {0}, 0, 0, 0, 0, {0} };

    DbgPrint_printf("atCtrlRxMsg[%d]: %s\n", pramLen, (char*) paramStr);

    if( (AtParams_timeType == ATPARAMS_TIME_TYPE_REL) && (AtParams_timeToRx != 0) )
    {
        if(EasyLink_getAbsTime(&absTime) != EasyLink_Status_Success)
        {
            // Problem getting absolute time, set to 0 (immediate)
            AtTerm_sendString("ERR: Abs Time unavailable, set to 0 "); 
            absTime = 0;
        }
        rxPacket.absTime = AtParams_timeToRx + absTime;
    }
    else
    {
        rxPacket.absTime = AtParams_timeToRx;
    }

    rxPacket.rxTimeout = AtParams_rxTimeout;

    if((AtParams_easyLinkStatus = EasyLink_receive(&rxPacket)) == EasyLink_Status_Success)
    {
        if (AtParams_easyLinkStatus == EasyLink_Status_Success)
        {
            processRx(rxPacket);
            status = AtProcess_Status_Success;
        }
    }
    else if(AtParams_easyLinkStatus == EasyLink_Status_Rx_Timeout)
    {
        AtTerm_sendString("RX: Timeout ");
        status = AtProcess_Status_EasyLinkError;
    }
    else if(AtParams_easyLinkStatus == EasyLink_Status_Aborted)
    {
        AtTerm_sendString("RX: Aborted ");
        status = AtProcess_Status_EasyLinkError;
    }
    else
    {
        AtTerm_sendString("RX: "); //error printed by atProcess
        status = AtProcess_Status_EasyLinkError;
    }

    return status;
}

static AtProcess_Status atCtrlTxMsg(char* paramStr, uint32_t pramLen)
{
    uint32_t absTime;
    AtProcess_Status status  = AtProcess_Status_EasyLinkError;
    EasyLink_TxPacket txPacket;

    DbgPrint_printf("atCtrlTxMsg[%d]: %s\n", pramLen, (char*) paramStr);

    memcpy(txPacket.dstAddr, AtParams_txAddr, AtParams_addrSize);

    if( (AtParams_timeType == ATPARAMS_TIME_TYPE_REL) && (AtParams_timeToTx != 0) )
    {
        if(EasyLink_getAbsTime(&absTime) != EasyLink_Status_Success)
        {
            // Problem getting absolute time, set to 0 (immediate)
            AtTerm_sendString("ERR: Abs Time unavailable, set to 0 "); 
            absTime = 0;
        }
        txPacket.absTime = AtParams_timeToTx + absTime;
    }
    else
    {
        txPacket.absTime = AtParams_timeToTx;
    }

    if (AtParams_binDataMode == 1)
    {
        //format is "001122..." each 1B bin payload is represented by 2 chars
        txPacket.len = (pramLen)/2;

        uint8_t msgIdx;
        //start from paramStr[1] to avoid white space separator
        for(msgIdx = 0; msgIdx < txPacket.len; msgIdx++)
        {
            if(sscanf(&(paramStr[msgIdx*2 + 1]),
                      "%02X", (unsigned int *)&(txPacket.payload[msgIdx])) != 1)
            {
                //error reading data
                return AtProcess_Status_ParamError;
            }
        }
    } else // send string
    {
        txPacket.len = pramLen - 1;
        memcpy(txPacket.payload, (uint8_t*) &paramStr[1], pramLen);
    }

    if(txPacket.len < EASYLINK_MAX_DATA_LENGTH)
    {
        if((AtParams_easyLinkStatus = EasyLink_transmit(&txPacket)) == EasyLink_Status_Success)
        {
            if (AtParams_easyLinkStatus == EasyLink_Status_Success)
            {
                status = AtProcess_Status_Success;
            }
            else if(AtParams_easyLinkStatus == EasyLink_Status_Aborted)
            {
                AtTerm_sendString("TX: Aborted ");
                status = AtProcess_Status_EasyLinkError;
            }
            else
            {
                AtTerm_sendString("TX: "); //Error printed by atProcess
                status = AtProcess_Status_EasyLinkError;
            }
        }
    }

    return status;
}

static AtProcess_Status atCtrlReset(char* paramStr, uint32_t pramLen)
{
    SysCtrlSystemReset();
    return AtProcess_Status_Success;
}

/***************************************************************************************************
 *                                          INTERFACE FUNCTIONS
 ***************************************************************************************************/

AtProcess_Status AtControl_parseIncoming(char *command, uint8_t cmdLen)
{
    uint8_t cmdIdx, paramOffSet;
    AtProcess_Status status = AtProcess_Status_CmdIdError;

    DbgPrint_printf("atParseIncomingControl[%d]: %s\n", cmdLen, command);

    for (cmdIdx = 0; cmdIdx < (sizeof(atControlCmds) / sizeof(AtCommand_t));
            cmdIdx++)
    {
        if ((paramOffSet = AtProcess_cmdCmp(atControlCmds[cmdIdx].cmdStr, command))
                && (atControlCmds[cmdIdx].cmdFxn != NULL))
        {
            status = atControlCmds[cmdIdx].cmdFxn(&(command[paramOffSet]),
                    cmdLen - paramOffSet);
        }
    }

    return status;
}

/***************************************************************************************************
 ***************************************************************************************************/
