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

#include <stdlib.h>
#include <inttypes.h>
#include <string.h> /* memset */

#include "at/AtParams.h"
#include "at/AtProcess.h"
#include "at/platform/tirtos/DbgPrint.h"
#include "at/platform/tirtos/PlatformTypes.h"
#include "at/platform/inc/AtTerm.h"
#include "Per.h"

#include <ti/drivers/PIN.h>

#include <ti/devices/DeviceFamily.h>
#include DeviceFamily_constructPath(inc/hw_ccfg.h)
#include DeviceFamily_constructPath(inc/hw_ccfg_simple_struct.h)

/* Pin driver handle */
static PIN_Handle gpioPinHandle;
static PIN_Config *gpioPinTable;

EasyLink_Status AtParams_easyLinkStatus = EasyLink_Status_Success;
uint8_t AtParams_echoEnabled = 1;
uint8_t AtParams_binDataMode = 0;
uint8_t AtParams_txAddr[EASYLINK_MAX_ADDR_SIZE] = {0};
uint8_t AtParams_addrSize = 1;
uint8_t AtParams_timeType = ATPARAMS_TIME_TYPE_REL;
uint32_t AtParams_timeToTx = 0;
uint32_t AtParams_timeToRx = 0;
uint32_t AtParams_rxTimeout = 0;
uint8_t AtParams_lastRxDstAddr[8] = {0};
uint32_t AtParams_lastRxTime = 0;
int8_t AtParams_lastRssi = 0;
uint8_t AtParams_testMode = 0;

static uint32_t perBurstInterval = 60;
static uint32_t perBurstSz = 1;
static uint32_t perNumPkts = 0;
static uint32_t perPktLen = 30;

/***************************************************************************************************
 *                                          LOCAL FUNCTIONS
 ***************************************************************************************************/

static AtProcess_Status atParamReadLastStatus(char* paramStr, uint32_t pramLen);
static AtProcess_Status atParamReadFreq(char* paramStr, uint32_t pramLen);
static AtProcess_Status atParamWriteFreq(char* paramStr, uint32_t pramLen);
static AtProcess_Status atParamReadPower(char* paramStr, uint32_t pramLen);
static AtProcess_Status atParamWritePower(char* paramStr, uint32_t pramLen);
static AtProcess_Status atParamReadBinDataMode(char* paramStr, uint32_t pramLen);
static AtProcess_Status atParamWriteBinDataMode(char* paramStr, uint32_t pramLen);
static AtProcess_Status atParamReadIeee(char* paramStr, uint32_t pramLen);
static AtProcess_Status atParamReadAddrSize(char* paramStr, uint32_t pramLen);
static AtProcess_Status atParamWriteAddrSize(char* paramStr, uint32_t pramLen);
static AtProcess_Status atParamReadTxAddr(char* paramStr, uint32_t pramLen);
static AtProcess_Status atParamWriteTxAddr(char* paramStr, uint32_t pramLen);
static AtProcess_Status atParamReadTimeType(char* paramStr, uint32_t pramLen);
static AtProcess_Status atParamWriteTimeType(char* paramStr, uint32_t pramLen);
static AtProcess_Status atParamReadTimeToTx(char* paramStr, uint32_t pramLen);
static AtProcess_Status atParamWriteTimeToTx(char* paramStr, uint32_t pramLen);
static AtProcess_Status atParamReadTimeToRx(char* paramStr, uint32_t pramLen);
static AtProcess_Status atParamWriteTimeToRx(char* paramStr, uint32_t pramLen);
static AtProcess_Status atParamReadRxTimeout(char* paramStr, uint32_t pramLen);
static AtProcess_Status atParamWriteRxTimeout(char* paramStr, uint32_t pramLen);
static AtProcess_Status atParamReadLastRxDstAddr(char* paramStr, uint32_t pramLen);
static AtProcess_Status atParamReadLastRxTime(char* paramStr, uint32_t pramLen);
static AtProcess_Status atParamReadCurrTestRxRssi(char* paramStr, uint32_t pramLen);
static AtProcess_Status atParamReadLastRxRssi(char* paramStr, uint32_t pramLen);
static AtProcess_Status atParamReadAbsTime(char* paramStr, uint32_t pramLen);
static AtProcess_Status atParamReadAddrFilter(char* paramStr, uint32_t pramLen, uint8_t filterIdx);
static AtProcess_Status atParamWriteAddrFilter(char* paramStr, uint32_t pramLen, uint8_t filterIdx);
static AtProcess_Status atParamReadAddrFilter0(char* paramStr, uint32_t pramLen);
static AtProcess_Status atParamWriteAddrFilter0(char* paramStr, uint32_t pramLen);
static AtProcess_Status atParamReadAddrFilter1(char* paramStr, uint32_t pramLen);
static AtProcess_Status atParamWriteAddrFilter1(char* paramStr, uint32_t pramLen);
static AtProcess_Status atParamReadAddrFilter2(char* paramStr, uint32_t pramLen);
static AtProcess_Status atParamWriteAddrFilter2(char* paramStr, uint32_t pramLen);
static AtProcess_Status atParamReadTestMode(char* paramStr, uint32_t pramLen);
static AtProcess_Status atParamWriteTestMode(char* paramStr, uint32_t pramLen);
static AtProcess_Status atParamReadPerBurstInterval(char* paramStr, uint32_t pramLen);
static AtProcess_Status atParamWritePerBurstInterval(char* paramStr, uint32_t pramLen);
static AtProcess_Status atParamReadPerBurstSize(char* paramStr, uint32_t pramLen);
static AtProcess_Status atParamWritePerBurstSize(char* paramStr, uint32_t pramLen);
static AtProcess_Status atParamReadPerNumPackets(char* paramStr, uint32_t pramLen);
static AtProcess_Status atParamWritePerNumPackets(char* paramStr, uint32_t pramLen);
static AtProcess_Status atParamReadPerPacketLen(char* paramStr, uint32_t pramLen);
static AtProcess_Status atParamWritePerPacketLen(char* paramStr, uint32_t pramLen);
static AtProcess_Status atParamReadAtEcho(char* paramStr, uint32_t pramLen);
static AtProcess_Status atParamWriteAtEcho(char* paramStr, uint32_t pramLen);
static AtProcess_Status atParamReadGpioValue(char* paramStr, uint32_t pramLen, uint8_t pinIdx);
static AtProcess_Status atParamWriteGpioValue(char* paramStr, uint32_t pramLen, uint8_t pinIdx);
static AtProcess_Status atParamReadGpioValue00(char* paramStr, uint32_t pramLen);
static AtProcess_Status atParamWriteGpioValue00(char* paramStr, uint32_t pramLen);
static AtProcess_Status atParamReadGpioValue01(char* paramStr, uint32_t pramLen);
static AtProcess_Status atParamWriteGpioValue01(char* paramStr, uint32_t pramLen);
static AtProcess_Status atParamReadGpioValue02(char* paramStr, uint32_t pramLen);
static AtProcess_Status atParamWriteGpioValue02(char* paramStr, uint32_t pramLen);
static AtProcess_Status atParamReadGpioValue03(char* paramStr, uint32_t pramLen);
static AtProcess_Status atParamWriteGpioValue03(char* paramStr, uint32_t pramLen);
static AtProcess_Status atParamReadGpioMode(char* paramStr, uint32_t pramLen, uint8_t pinIdx);
static AtProcess_Status atParamWriteGpioMode(char* paramStr, uint32_t pramLen, uint8_t pinIdx);
static AtProcess_Status atParamReadGpioMode00(char* paramStr, uint32_t pramLen);
static AtProcess_Status atParamWriteGpioMode00(char* paramStr, uint32_t pramLen);
static AtProcess_Status atParamReadGpioMode01(char* paramStr, uint32_t pramLen);
static AtProcess_Status atParamWriteGpioMode01(char* paramStr, uint32_t pramLen);
static AtProcess_Status atParamReadGpioMode02(char* paramStr, uint32_t pramLen);
static AtProcess_Status atParamWriteGpioMode02(char* paramStr, uint32_t pramLen);
static AtProcess_Status atParamReadGpioMode03(char* paramStr, uint32_t pramLen);
static AtProcess_Status atParamWriteGpioMode03(char* paramStr, uint32_t pramLen);

AtCommand_t atParamsters[] =
{
        {"ST?", atParamReadLastStatus},
        {"FR?", atParamReadFreq},
        {"FR=", atParamWriteFreq},
        {"PW?", atParamReadPower},
        {"PW=", atParamWritePower},
        {"BM?", atParamReadBinDataMode},
        {"BM=", atParamWriteBinDataMode},
        {"IE?", atParamReadIeee},
        {"AS?", atParamReadAddrSize},
        {"AS=", atParamWriteAddrSize},
        {"TA?", atParamReadTxAddr},
        {"TA=", atParamWriteTxAddr},
        {"RT?", atParamReadAbsTime},
        {"TY?", atParamReadTimeType},
        {"TY=", atParamWriteTimeType},
        {"TT?", atParamReadTimeToTx},
        {"TT=", atParamWriteTimeToTx},
        {"TR?", atParamReadTimeToRx},
        {"TR=", atParamWriteTimeToRx},
        {"RO?", atParamReadRxTimeout},
        {"RO=", atParamWriteRxTimeout},
        {"LA?", atParamReadLastRxDstAddr},
        {"LT?", atParamReadLastRxTime},
        {"CR?", atParamReadCurrTestRxRssi},
        {"LR?", atParamReadLastRxRssi},
        {"F0?", atParamReadAddrFilter0},
        {"F0=", atParamWriteAddrFilter0},
        {"F1?", atParamReadAddrFilter1},
        {"F1=", atParamWriteAddrFilter1},
        {"F2?", atParamReadAddrFilter2},
        {"F2=", atParamWriteAddrFilter2},
        {"TM?", atParamReadTestMode},
        {"TM=", atParamWriteTestMode},
        {"PI?", atParamReadPerBurstInterval},
        {"PI=", atParamWritePerBurstInterval},
        {"PB?", atParamReadPerBurstSize},
        {"PB=", atParamWritePerBurstSize},
        {"PP?", atParamReadPerNumPackets},
        {"PP=", atParamWritePerNumPackets},
        {"PL?", atParamReadPerPacketLen},
        {"PL=", atParamWritePerPacketLen},
        {"AE?", atParamReadAtEcho},
        {"AE=", atParamWriteAtEcho},
        {"GV00?", atParamReadGpioValue00},
        {"GV00=", atParamWriteGpioValue00},
        {"GV01?", atParamReadGpioValue01},
        {"GV01=", atParamWriteGpioValue01},
        {"GV02?", atParamReadGpioValue02},
        {"GV02=", atParamWriteGpioValue02},
        {"GV03?", atParamReadGpioValue03},
        {"GV03=", atParamWriteGpioValue03},
        {"GM00?", atParamReadGpioMode00},
        {"GM00=", atParamWriteGpioMode00},
        {"GM01?", atParamReadGpioMode01},
        {"GM01=", atParamWriteGpioMode01},
        {"GM02?", atParamReadGpioMode02},
        {"GM02=", atParamWriteGpioMode02},
        {"GM03?", atParamReadGpioMode03},
        {"GM03=", atParamWriteGpioMode03},
};

//Address filter for max of 3 addresses of 8byte length
static uint8_t addrFilterTable[3 * 8] = {0};

static AtProcess_Status atParamReadLastStatus(char* paramStr, uint32_t pramLen)
{
    AtProcess_Status status = AtProcess_Status_ParamError;

    DbgPrint_printf("atParamReadStatus[%d]: %s\n", pramLen, (char*)paramStr);

    AtTerm_sendStringUi32Value("", AtParams_easyLinkStatus, 16);
    status = AtProcess_Status_Success;

    return status;
}

static AtProcess_Status atParamReadFreq(char* paramStr, uint32_t pramLen)
{
    AtProcess_Status status = AtProcess_Status_ParamError;
    uint32_t freq = 0;

    DbgPrint_printf("atParamReadFreq[%d]: %s\n", pramLen, (char*)paramStr);

    freq = EasyLink_getFrequency();

    if(freq != 0)
    {
        AtTerm_sendStringUi32Value("", freq, 10);
        status = AtProcess_Status_Success;
    }

    return status;
}

static AtProcess_Status atParamWriteFreq(char* paramStr, uint32_t pramLen)
{
    AtProcess_Status status = AtProcess_Status_EasyLinkError;
    uint32_t newFreq;

    DbgPrint_printf("atParamWriteFreq[%d]: %s\n", pramLen, (char*)paramStr);

    if(sscanf(paramStr, "%u", (unsigned int *)&newFreq) == 1)
    {
        if((AtParams_easyLinkStatus = EasyLink_setFrequency(newFreq)) == EasyLink_Status_Success)
        {
            status = AtProcess_Status_Success;
        }
    }
    else
    {
        status = AtProcess_Status_ParamError;
    }

    return status;
}

static AtProcess_Status atParamReadPower(char* paramStr, uint32_t pramLen)
{
    AtProcess_Status status = AtProcess_Status_ParamError;
    int8_t power;

    DbgPrint_printf("atParamReadPower[%d]: %s\n", pramLen, (char*)paramStr);

    if(EasyLink_getRfPower(&power) == EasyLink_Status_Success)
    {
        AtTerm_sendStringI8Value("", power, 10);
        status = AtProcess_Status_Success;
    }
    
    return status;
}

static AtProcess_Status atParamWritePower(char* paramStr, uint32_t pramLen)
{
    AtProcess_Status status = AtProcess_Status_EasyLinkError;
    int32_t newPower;

    DbgPrint_printf("atParamWritePower[%d]: %s\n", pramLen, (char*)paramStr);

    if(sscanf(paramStr, "%d", (int *)&newPower) == 1)
    {
        if((AtParams_easyLinkStatus = EasyLink_setRfPower(newPower)) == EasyLink_Status_Success)
        {
            status = AtProcess_Status_Success;
        }
    }
    else
    {
        status = AtProcess_Status_ParamError;
    }

    return status;
}

static AtProcess_Status atParamReadBinDataMode(char* paramStr, uint32_t pramLen)
{
    AtProcess_Status status = AtProcess_Status_ParamError;

    DbgPrint_printf("atParamReadBinDataMode: %s\n", (char*)paramStr);
    AtTerm_sendStringUi8Value("",AtParams_binDataMode, 10);
    status = AtProcess_Status_Success;

    return status;
}

static AtProcess_Status atParamWriteBinDataMode(char* paramStr, uint32_t pramLen)
{
    AtProcess_Status status = AtProcess_Status_ParamError;

    DbgPrint_printf("atParamWriteBinDataMode: %s\n", (char*)paramStr);
    AtParams_binDataMode = atoi(&paramStr[0]);
    status = AtProcess_Status_Success;

    return status;
}

static AtProcess_Status atParamReadIeee(char* paramStr, uint32_t pramLen)
{
    AtProcess_Status status = AtProcess_Status_ParamError;
    uint8_t ieeeAddr[8];

    DbgPrint_printf("atParamReadIeee[%d]: %s\n", pramLen, (char*)paramStr);

    if(EasyLink_getIeeeAddr(ieeeAddr) == EasyLink_Status_Success)
    {
        AtTerm_sendStringUi8Value("", ieeeAddr[0], 16);
        AtTerm_sendStringUi8Value("", ieeeAddr[1], 16);
        AtTerm_sendStringUi8Value("", ieeeAddr[2], 16);
        AtTerm_sendStringUi8Value("", ieeeAddr[3], 16);
        AtTerm_sendStringUi8Value("", ieeeAddr[4], 16);
        AtTerm_sendStringUi8Value("", ieeeAddr[5], 16);
        AtTerm_sendStringUi8Value("", ieeeAddr[6], 16);
        AtTerm_sendStringUi8Value("", ieeeAddr[7], 16);
        status = AtProcess_Status_Success;
    }

    return status;
}

static AtProcess_Status atParamReadAddrSize(char* paramStr, uint32_t pramLen)
{
    AtProcess_Status status = AtProcess_Status_ParamError;

    DbgPrint_printf("atParamReadAddrSize[%d]: %s\n", pramLen, (char*)paramStr);

    AtTerm_sendStringUi8Value("", AtParams_addrSize, 16);
    status = AtProcess_Status_Success;

    return status;
}

static AtProcess_Status atParamWriteAddrSize(char* paramStr, uint32_t pramLen)
{
    AtProcess_Status status = AtProcess_Status_ParamError;
    uint8_t newAddrSize = (uint8_t) atoi(paramStr);

    DbgPrint_printf("atParamWriteAddrSize[%d]: %s\n", pramLen, (char*)paramStr);

    if(newAddrSize != AtParams_addrSize)
    {
        if(EasyLink_setCtrl(EasyLink_Ctrl_AddSize, newAddrSize) == EasyLink_Status_Success)
        {
            AtParams_addrSize = newAddrSize;
            memset(addrFilterTable, 0, sizeof(addrFilterTable));
            // Disable address filtering
            EasyLink_enableRxAddrFilter(NULL, 0, 0);
            status = AtProcess_Status_Success;
        }
    }
    else if(newAddrSize == AtParams_addrSize)
    {
        status = AtProcess_Status_Success;
    }


    return status;
}

static AtProcess_Status atParamReadTxAddr(char* paramStr, uint32_t pramLen)
{
    uint8_t idx;

    DbgPrint_printf("atParamReadTxAddr[%d]: %s\n", pramLen, (char*)paramStr);

    if(AtParams_addrSize != 0)
    {
        for(idx=0; idx < AtParams_addrSize; idx++)
        {
            AtTerm_sendStringUi8Value("", AtParams_txAddr[idx], 16);
        }
    }
    else
    {
        AtTerm_sendString("n/a\n");
    }

    return AtProcess_Status_Success;
}

static AtProcess_Status atParamWriteTxAddr(char* paramStr, uint32_t pramLen)
{
    AtProcess_Status status = AtProcess_Status_ParamError;
    uint8_t numAddrBytes = 0;
    uint8_t txAddr[EASYLINK_MAX_ADDR_SIZE];

    DbgPrint_printf("atParamWriteTxAddr[%d]: %s\n", pramLen, (char*)paramStr);

    numAddrBytes = sscanf(paramStr,
            "%02X%02X%02X%02X%02X%02X%02X%02X",
            (unsigned int *)&txAddr[0],
            (unsigned int *)&txAddr[1],
            (unsigned int *)&txAddr[2],
            (unsigned int *)&txAddr[3],
            (unsigned int *)&txAddr[4],
            (unsigned int *)&txAddr[5],
            (unsigned int *)&txAddr[6],
            (unsigned int *)&txAddr[7]);

    if(numAddrBytes == AtParams_addrSize)
    {
        memcpy(AtParams_txAddr, txAddr, AtParams_addrSize);
        status = AtProcess_Status_Success;
    }

    return status;
}

static AtProcess_Status atParamReadTimeType(char* paramStr, uint32_t pramLen)
{
    AtProcess_Status status = AtProcess_Status_ParamError;

    DbgPrint_printf("atParamReadTimeType: %s\n", (char*)paramStr);
    AtTerm_sendStringUi32Value("",AtParams_timeType, 16);
    status = AtProcess_Status_Success;

    return status;
}

static AtProcess_Status atParamWriteTimeType(char* paramStr, uint32_t pramLen)
{
    AtProcess_Status status = AtProcess_Status_Success;

    DbgPrint_printf("atParamWriteTimeType: %s\n", (char*)paramStr);

    AtParams_timeType = (uint8_t) atoi(paramStr);

    return status;
}

static AtProcess_Status atParamReadTimeToTx(char* paramStr, uint32_t pramLen)
{
    AtProcess_Status status = AtProcess_Status_ParamError;

    DbgPrint_printf("atParamReadTimeToTx: %s\n", (char*)paramStr);
    AtTerm_sendStringUi32Value("",AtParams_timeToTx, 10);
    status = AtProcess_Status_Success;

    return status;
}

static AtProcess_Status atParamWriteTimeToTx(char* paramStr, uint32_t pramLen)
{
    AtProcess_Status status = AtProcess_Status_ParamError;

    DbgPrint_printf("atParamWriteTimeToTx: %s\n", (char*)paramStr);
    if(sscanf(paramStr, "%d", (int *)&AtParams_timeToTx) == 1)
    {
        status = AtProcess_Status_Success;
    }

    return status;
}

static AtProcess_Status atParamReadTimeToRx(char* paramStr, uint32_t pramLen)
{
    AtProcess_Status status = AtProcess_Status_ParamError;

    DbgPrint_printf("atParamReadTimeToRx: %s\n", (char*)paramStr);
    AtTerm_sendStringUi32Value("",AtParams_timeToRx, 10);
    status = AtProcess_Status_Success;

    return status;
}

static AtProcess_Status atParamWriteTimeToRx(char* paramStr, uint32_t pramLen)
{
    AtProcess_Status status = AtProcess_Status_ParamError;

    DbgPrint_printf("atParamWriteTimeToRx: %s\n", (char*)paramStr);
    if(sscanf(paramStr, "%d", (int *)&AtParams_timeToRx) == 1)
    {
        status = AtProcess_Status_Success;
    }

    return status;
}

static AtProcess_Status atParamReadRxTimeout(char* paramStr, uint32_t pramLen)
{
    AtProcess_Status status = AtProcess_Status_ParamError;

    DbgPrint_printf("atParamReadRxTimeout: %s\n", (char*)paramStr);
    AtTerm_sendStringUi32Value("",AtParams_rxTimeout, 10);
    status = AtProcess_Status_Success;

    return status;
}

static AtProcess_Status atParamWriteRxTimeout(char* paramStr, uint32_t pramLen)
{
    AtProcess_Status status = AtProcess_Status_ParamError;

    DbgPrint_printf("atParamWriteRxTimeout: %s\n", (char*)paramStr);
    if(sscanf(paramStr, "%d", (int *)&AtParams_rxTimeout) == 1)
    {
        status = AtProcess_Status_Success;
    }

    return status;
}

static AtProcess_Status atParamReadLastRxDstAddr(char* paramStr, uint32_t pramLen)
{
    uint8_t idx;

    DbgPrint_printf("atParamReadLastRxDstAddr[%d]: %s\n", pramLen, (char*)paramStr);

    AtTerm_sendStringUi8Value("", AtParams_lastRxDstAddr[0], 16);

    for(idx=1; idx < AtParams_addrSize; idx++)
    {
        AtTerm_sendStringUi8Value("", AtParams_lastRxDstAddr[idx], 16);
    }

    return AtProcess_Status_Success;
}

static AtProcess_Status atParamReadLastRxTime(char* paramStr, uint32_t pramLen)
{
    AtProcess_Status status = AtProcess_Status_ParamError;

    DbgPrint_printf("atParamReadLastRxTime: %s\n", (char*)paramStr);
    AtTerm_sendStringUi32Value("",AtParams_lastRxTime, 10);
    status = AtProcess_Status_Success;

    return status;
}

static AtProcess_Status atParamReadCurrTestRxRssi(char* paramStr, uint32_t pramLen)
{
    AtProcess_Status status = AtProcess_Status_ParamError;
	int8_t rssiDbm;
	
    DbgPrint_printf("atParamReadCurrTestRxRssi: %s\n", (char*)paramStr);
	
	if(EasyLink_getRssi(&rssiDbm) == EasyLink_Status_Success)
	{
		AtTerm_sendStringI8Value("", rssiDbm, 10);
		status = AtProcess_Status_Success;
	}

    return status;
}

static AtProcess_Status atParamReadLastRxRssi(char* paramStr, uint32_t pramLen)
{
    AtProcess_Status status = AtProcess_Status_ParamError;

    DbgPrint_printf("atParamReadLastRxRssi: %s\n", (char*)paramStr);
    AtTerm_sendStringI8Value("",AtParams_lastRssi, 10);
    status = AtProcess_Status_Success;

    return status;
}

static AtProcess_Status atParamReadAbsTime(char* paramStr, uint32_t pramLen)
{
    AtProcess_Status status = AtProcess_Status_ParamError;
    uint32_t absTime;
    
	DbgPrint_printf("atParamReadAbsTime: %s\n", (char*)paramStr);
    
    if(EasyLink_getAbsTime(&absTime) == EasyLink_Status_Success)
	{
		AtTerm_sendStringI8Value("", absTime, 10);
		status = AtProcess_Status_Success;
	}

    return status;
}

static AtProcess_Status atParamReadAddrFilter(char* paramStr, uint32_t pramLen, uint8_t filterIdx)
{
    AtProcess_Status status = AtProcess_Status_ParamError;
    uint8_t idx;

    DbgPrint_printf("atParamReadAddrFilterEnable[%d]: %s\n", pramLen, (char*)paramStr);

    if(AtParams_addrSize != 0)
    {
        for(idx=0; idx < AtParams_addrSize; idx++)
        {
            AtTerm_sendStringUi8Value("", addrFilterTable[idx + (AtParams_addrSize*filterIdx)], 16);
        }
    }
    else
    {
        AtTerm_sendString("n/a\n");
    }

    status = AtProcess_Status_Success;

    return status;
}

static AtProcess_Status atParamWriteAddrFilter(char* paramStr, uint32_t pramLen, uint8_t filterIdx)
{
    AtProcess_Status status = AtProcess_Status_ParamError;
    uint16_t numAddrBytes;
    unsigned int parsedAddress[8];

    DbgPrint_printf("atParamWriteAddrFilterEnable[%d]: %s\n", pramLen, (char*)paramStr);

    numAddrBytes = sscanf(paramStr,
            "%02X%02X%02X%02X%02X%02X%02X%02X",
            (unsigned int *)&parsedAddress[0], (unsigned int *)&parsedAddress[1],
            (unsigned int *)&parsedAddress[2], (unsigned int *)&parsedAddress[3],
            (unsigned int *)&parsedAddress[4], (unsigned int *)&parsedAddress[5],
            (unsigned int *)&parsedAddress[6], (unsigned int *)&parsedAddress[7]);

    uint8_t i = 0;
    for(i = 0; i < numAddrBytes; i++)
    {
        addrFilterTable[i + (AtParams_addrSize*filterIdx)] = (uint8_t)parsedAddress[i];
    }

    if(numAddrBytes == AtParams_addrSize)
    {
        if(EasyLink_enableRxAddrFilter(addrFilterTable,
                AtParams_addrSize, 3) == EasyLink_Status_Success)
        {
            status = AtProcess_Status_Success;
        }
    }

    return status;
}

static AtProcess_Status atParamReadAddrFilter0(char* paramStr, uint32_t pramLen)
{
    return atParamReadAddrFilter(paramStr, pramLen, 0);
}

static AtProcess_Status atParamReadAddrFilter1(char* paramStr, uint32_t pramLen)
{
    return atParamReadAddrFilter(paramStr, pramLen, 1);
}

static AtProcess_Status atParamReadAddrFilter2(char* paramStr, uint32_t pramLen)
{
    return atParamReadAddrFilter(paramStr, pramLen, 2);
}

static AtProcess_Status atParamWriteAddrFilter0(char* paramStr, uint32_t pramLen)
{
    return atParamWriteAddrFilter(paramStr, pramLen, 0);
}

static AtProcess_Status atParamWriteAddrFilter1(char* paramStr, uint32_t pramLen)
{
    return atParamWriteAddrFilter(paramStr, pramLen, 1);
}

static AtProcess_Status atParamWriteAddrFilter2(char* paramStr, uint32_t pramLen)
{
    return atParamWriteAddrFilter(paramStr, pramLen, 2);
}

static AtProcess_Status atParamReadTestMode(char* paramStr, uint32_t pramLen)
{
    AtProcess_Status status = AtProcess_Status_Success;;

    DbgPrint_printf("atParamReadTestMode[%d]: %s\n", pramLen, (char*)paramStr);
    AtTerm_sendStringUi8Value("", AtParams_testMode, 16);

    return status;
}

static AtProcess_Status atParamWriteTestMode(char* paramStr, uint32_t pramLen)
{
    AtProcess_Status status = AtProcess_Status_EasyLinkError;
    uint32_t mode = atoi(paramStr);

    DbgPrint_printf("atParamWriteTestMode[%d]: %s\n", pramLen, (char*)paramStr);

    if( (AtParams_testMode == 0) && (mode == 1) )
    {
        if( (AtParams_easyLinkStatus =  EasyLink_setCtrl(EasyLink_Ctrl_Test_Tone,  AtParams_rxTimeout))
                == EasyLink_Status_Success )
        {
            AtParams_testMode = mode;
            status = AtProcess_Status_Success;
        }
    }
    else if( (AtParams_testMode == 0) && (mode == 2) )
    {
        if( (AtParams_easyLinkStatus = EasyLink_setCtrl(EasyLink_Ctrl_Test_Signal,  AtParams_rxTimeout))
                == EasyLink_Status_Success )
        {
            AtParams_testMode = mode;
            status = AtProcess_Status_Success;
        }
    }
    else if( (AtParams_testMode == 0) && (mode == 3) )
    {
        status = Per_testTx(perBurstSz, perBurstInterval, perPktLen, perNumPkts);
        //set test mode back to 0 when returning
        AtParams_testMode = 0;
    }
    else if( (AtParams_testMode == 0) && (mode == 4) )
    {
        status = Per_testRx(perNumPkts);
        //set test mode back to 0 when returning
        AtParams_testMode = 0;
    }
    else if((AtParams_testMode == 0) && (mode == 5))
    {
        if((AtParams_easyLinkStatus = EasyLink_setCtrl(EasyLink_Ctrl_Rx_Test_Tone, AtParams_rxTimeout))
                == EasyLink_Status_Success )
        {
            AtParams_testMode = mode;
            status = AtProcess_Status_Success;
        }
    }
    else if( ((AtParams_testMode == 1) ||
              (AtParams_testMode == 2) ||
              (AtParams_testMode == 5)) &&
             (mode == 0) )
    {
        //CW or Modulated carrier needs to be aborted
        if( (AtParams_easyLinkStatus = EasyLink_abort())
                == EasyLink_Status_Success )
        {
            AtParams_testMode = 0;
            status = AtProcess_Status_Success;
        }
    }
    else
    {
        status = AtProcess_Status_ParamError;
    }

    return status;
}

static AtProcess_Status atParamWritePerBurstInterval(char* paramStr, uint32_t pramLen)
{
    AtProcess_Status status = AtProcess_Status_ParamError;

    DbgPrint_printf("atParamWritePerBurstInterval: %s\n", (char*)paramStr);
    if(sscanf(paramStr, "%d", (int *)&perBurstInterval) == 1)
    {
        status = AtProcess_Status_Success;
    }

    return status;
}

static AtProcess_Status atParamReadPerBurstInterval(char* paramStr, uint32_t pramLen)
{
    AtProcess_Status status = AtProcess_Status_ParamError;

    DbgPrint_printf("atParamReadPerTxRate: %s\n", (char*)paramStr);
    AtTerm_sendStringUi32Value("",perBurstInterval, 10);
    status = AtProcess_Status_Success;

    return status;
}

static AtProcess_Status atParamWritePerBurstSize(char* paramStr, uint32_t pramLen)
{
    AtProcess_Status status = AtProcess_Status_ParamError;

    DbgPrint_printf("atParamWritePerBurstSize: %s\n", (char*)paramStr);
    if(sscanf(paramStr, "%x", (unsigned int *)&perBurstSz) == 1)
    {
        status = AtProcess_Status_Success;
    }

    return status;
}

static AtProcess_Status atParamReadPerBurstSize(char* paramStr, uint32_t pramLen)
{
    AtProcess_Status status = AtProcess_Status_ParamError;

    DbgPrint_printf("atParamReadPerBurstSize: %s\n", (char*)paramStr);
    AtTerm_sendStringUi32Value("",perBurstSz, 16);
    status = AtProcess_Status_Success;

    return status;
}

static AtProcess_Status atParamWritePerNumPackets(char* paramStr, uint32_t pramLen)
{
    AtProcess_Status status = AtProcess_Status_ParamError;

    DbgPrint_printf("atParamReadPerNumPackets: %s\n", (char*)paramStr);
    if(sscanf(paramStr, "%d", (int *)&perNumPkts) == 1)
    {
        status = AtProcess_Status_Success;
    }

    return status;
}

static AtProcess_Status atParamReadPerNumPackets(char* paramStr, uint32_t pramLen)
{
    AtProcess_Status status = AtProcess_Status_ParamError;

    DbgPrint_printf("atParamReadPerBurstSize: %s\n", (char*)paramStr);
    AtTerm_sendStringUi32Value("",perNumPkts, 10);
    status = AtProcess_Status_Success;

    return status;
}

static AtProcess_Status atParamWritePerPacketLen(char* paramStr, uint32_t pramLen)
{
    AtProcess_Status status = AtProcess_Status_ParamError;

    DbgPrint_printf("atParamWritePerPacketLen: %s\n", (char*)paramStr);
    if(sscanf(paramStr, "%x", (unsigned int *)&perPktLen) == 1)
    {
        status = AtProcess_Status_Success;
    }

    return status;
}

static AtProcess_Status atParamReadPerPacketLen(char* paramStr, uint32_t pramLen)
{
    AtProcess_Status status = AtProcess_Status_ParamError;

    DbgPrint_printf("atParamReadPerPacketLen: %s\n", (char*)paramStr);
    AtTerm_sendStringUi32Value("",perPktLen, 16);
    status = AtProcess_Status_Success;

    return status;
}

static AtProcess_Status atParamWriteAtEcho(char* paramStr, uint32_t pramLen)
{
    AtProcess_Status status = AtProcess_Status_ParamError;

    DbgPrint_printf("atParamReadAtEcho: %s\n", (char*)paramStr);
    AtParams_echoEnabled = atoi(paramStr);
    status = AtProcess_Status_Success;

    return status;
}

static AtProcess_Status atParamReadAtEcho(char* paramStr, uint32_t pramLen)
{
    AtProcess_Status status = AtProcess_Status_ParamError;

    DbgPrint_printf("atParamReadAtEcho: %s\n", (char*)paramStr);
    AtTerm_sendStringUi8Value("",AtParams_echoEnabled, 10);
    status = AtProcess_Status_Success;

    return status;
}

static AtProcess_Status atParamWriteGpioValue(char* paramStr, uint32_t pramLen, uint8_t pinIdx)
{
    AtProcess_Status status = AtProcess_Status_ParamError;
    uint32_t pinValue;

    DbgPrint_printf("atParamWriteGpioValue: %s\n", (char*)paramStr);
    pinValue = atoi(paramStr);

    if(PIN_getConfig(PIN_ID(gpioPinTable[pinIdx])) && PIN_GPIO_OUTPUT_EN)
    {
        PIN_setOutputValue(gpioPinHandle, PIN_ID(gpioPinTable[pinIdx]), pinValue);
        status = AtProcess_Status_Success;
    }

    return status;
}

static AtProcess_Status atParamReadGpioValue(char* paramStr, uint32_t pramLen, uint8_t pinIdx)
{
    AtProcess_Status status = AtProcess_Status_ParamError;
    uint8_t pinVal;

    DbgPrint_printf("atParamReadGpioValue: %s\n", (char*)paramStr);

    if( (PIN_getConfig(PIN_ID(gpioPinTable[pinIdx])) & PIN_GPIO_OUTPUT_EN) ==
            PIN_GPIO_OUTPUT_EN)
    {
        pinVal = PIN_getOutputValue(PIN_ID(gpioPinTable[pinIdx]));
        AtTerm_sendStringUi8Value("",pinVal, 16);
    }
    else
    {
        pinVal = PIN_getInputValue(PIN_ID(gpioPinTable[pinIdx]));
        AtTerm_sendStringUi8Value("",pinVal, 16);
    }

    status = AtProcess_Status_Success;

    return status;
}

static AtProcess_Status atParamReadGpioValue00(char* paramStr, uint32_t pramLen)
{
    return atParamReadGpioValue(paramStr, pramLen, 0);
}

static AtProcess_Status atParamWriteGpioValue00(char* paramStr, uint32_t pramLen)
{
    return atParamWriteGpioValue(paramStr, pramLen, 0);
}

static AtProcess_Status atParamReadGpioValue01(char* paramStr, uint32_t pramLen)
{
    return atParamReadGpioValue(paramStr, pramLen, 1);
}

static AtProcess_Status atParamWriteGpioValue01(char* paramStr, uint32_t pramLen)
{
    return atParamWriteGpioValue(paramStr, pramLen, 1);
}

static AtProcess_Status atParamReadGpioValue02(char* paramStr, uint32_t pramLen)
{
    return atParamReadGpioValue(paramStr, pramLen, 2);
}

static AtProcess_Status atParamWriteGpioValue02(char* paramStr, uint32_t pramLen)
{
    return atParamWriteGpioValue(paramStr, pramLen, 2);
}

static AtProcess_Status atParamReadGpioValue03(char* paramStr, uint32_t pramLen)
{
    return atParamReadGpioValue(paramStr, pramLen, 3);
}

static AtProcess_Status atParamWriteGpioValue03(char* paramStr, uint32_t pramLen)
{
    return atParamWriteGpioValue(paramStr, pramLen, 3);
}

static AtProcess_Status atParamWriteGpioMode(char* paramStr, uint32_t pramLen, uint8_t pinIdx)
{
    AtProcess_Status status = AtProcess_Status_ParamError;
    uint32_t pinMode;

    DbgPrint_printf("atParamWriteGpioMode: %s\n", (char*)paramStr);
    pinMode = atoi(paramStr);

    if(pinMode == 0)
        pinMode = PIN_INPUT_EN | PIN_PULLUP;
    else
        pinMode = PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL | PIN_DRVSTR_MAX;

    if(PIN_setConfig(gpioPinHandle, PIN_BM_ALL, PIN_ID(gpioPinTable[pinIdx]) | pinMode) == PIN_SUCCESS)
    {

        status = AtProcess_Status_Success;
    }

    return status;
}

static AtProcess_Status atParamReadGpioMode(char* paramStr, uint32_t pramLen, uint8_t pinIdx)
{
    AtProcess_Status status = AtProcess_Status_ParamError;

    DbgPrint_printf("atParamReadGpioMode: %s\n", (char*)paramStr);

    if((PIN_getConfig(PIN_ID(gpioPinTable[pinIdx])) & PIN_GPIO_OUTPUT_EN) ==
            PIN_GPIO_OUTPUT_EN)
    {
        AtTerm_sendString("01 ");
    }
    else
    {
        AtTerm_sendString("00 ");
    }

    status = AtProcess_Status_Success;

    return status;
}

static AtProcess_Status atParamReadGpioMode00(char* paramStr, uint32_t pramLen)
{
    return atParamReadGpioMode(paramStr, pramLen, 0);
}

static AtProcess_Status atParamWriteGpioMode00(char* paramStr, uint32_t pramLen)
{
    return atParamWriteGpioMode(paramStr, pramLen, 0);
}

static AtProcess_Status atParamReadGpioMode01(char* paramStr, uint32_t pramLen)
{
    return atParamReadGpioMode(paramStr, pramLen, 1);
}

static AtProcess_Status atParamWriteGpioMode01(char* paramStr, uint32_t pramLen)
{
    return atParamWriteGpioMode(paramStr, pramLen, 1);
}

static AtProcess_Status atParamReadGpioMode02(char* paramStr, uint32_t pramLen)
{
    return atParamReadGpioMode(paramStr, pramLen, 2);
}

static AtProcess_Status atParamWriteGpioMode02(char* paramStr, uint32_t pramLen)
{
    return atParamWriteGpioMode(paramStr, pramLen, 2);
}

static AtProcess_Status atParamReadGpioMode03(char* paramStr, uint32_t pramLen)
{
    return atParamReadGpioMode(paramStr, pramLen, 3);
}

static AtProcess_Status atParamWriteGpioMode03(char* paramStr, uint32_t pramLen)
{
    return atParamWriteGpioMode(paramStr, pramLen, 3);
}

/***************************************************************************************************
 *                                          INTERFACE FUNCTIONS
 ***************************************************************************************************/

AtProcess_Status AtParams_parseIncoming( char *param, uint8_t paramLen )
{
    uint8_t cmdIdx, paramOffSet;
    AtProcess_Status status = AtProcess_Status_CmdIdError;

    DbgPrint_printf("atParseIncomingParamisterAccess[%d]: %s\n", paramLen, param);

    for(cmdIdx = 0; cmdIdx < (sizeof(atParamsters) / sizeof(AtCommand_t )); cmdIdx++)
    {
        if( (paramOffSet = AtProcess_cmdCmp(atParamsters[cmdIdx].cmdStr, param)) && (atParamsters[cmdIdx].cmdFxn != NULL) )
        {
            status = atParamsters[cmdIdx].cmdFxn(&(param[paramOffSet+1]), paramLen-1);
            break;
        }
    }

    return status;
}

AtProcess_Status AtParams_init( PIN_Handle pinHandle, PIN_Config *pinTable)
{
    gpioPinHandle = pinHandle;
    gpioPinTable = pinTable;

    return AtProcess_Status_Success;
}

/***************************************************************************************************
 ***************************************************************************************************/
