/*
 * Copyright (c) 2015, Texas Instruments Incorporated
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

//!
//! \defgroup atProcess

//!
//! \ingroup atProcess
//@{

#ifndef atParams__include
#define atParams__include

#ifdef __cplusplus
extern "C"
{
#endif

/***************************************************************************************************
 * INCLUDES
 ***************************************************************************************************/
#include <ti/drivers/PIN.h>

#include "at/platform/tirtos/PlatformTypes.h"
#include "at/AtProcess.h"
#include "easylink/EasyLink.h"

/***************************************************************************************************
 * MACROS
 ***************************************************************************************************/
#define ATPERAMS_TIME_TYPE_ABS   0
#define ATPARAMS_TIME_TYPE_REL   1

//! \brief At Parameter containing the last easyLink status to be returned
extern EasyLink_Status AtParams_easyLinkStatus;

//! \brief At Parameter containing the state of the UART echo used to echo incoming
//! characters
extern uint8_t AtParams_echoEnabled;

//! \brief At Parameter containing the state of the binary mode used for Tx and Rx
extern uint8_t AtParams_binDataMode;

//! \brief At Parameter containing the address size used for Tx and Rx
extern uint8_t AtParams_addrSize;

//! \brief At Parameter containing the address used for Tx
extern uint8_t AtParams_txAddr[8];

//! \brief At Parameter containing the time type (Abs or Relative)
//! for timeToTx and timeToRx
extern uint8_t AtParams_timeType;

//! \brief At Parameter containing the Abs or Relative time to
//! Tx the next packet
extern uint32_t AtParams_timeToTx;

//! \brief At Parameter containing the Abs or Relative time to
//! Rx the next packet
extern uint32_t AtParams_timeToRx;

//! \brief At Parameter containing the Abs or Relative time to
//! Rx the next packet
extern uint32_t AtParams_rxTimeout;

//! \brief At Parameter containing the Abs or Relative time to
//! Rx the next packet last Rx'ed address
extern uint8_t AtParams_lastRxDstAddr[8];

//! \brief At Parameter containing Relative time the the last packet
//! was Rx'ed
extern uint32_t AtParams_lastRxTime;

//! \brief At Parameter containing the RSSI if the last Rx Packet
extern int8_t AtParams_lastRssi;

//! \brief At Parameter for setting the test mode
extern uint8_t AtParams_testMode;

//*****************************************************************************
//
//! \brief Initializes the atParam module
//!
//! This function Initializes the atParam module
//!
//! \return AT error code
//
//*****************************************************************************
AtProcess_Status AtParams_init( PIN_Handle pinHandle, PIN_Config *pinTable);

//*****************************************************************************
//
//! \brief Parses incoming AT Parameter commands ("ATP")
//!
//! This function parses in coming at parameter commands for setting / getting
//! at Parameters, these commands are of the format "ATPxx=yy"/ "ATPxx?"
//! respectively
//!
//! \param param is the string containing the command to be parsed
//! \param len is the length of the command
//!
//! \return AT error code
//
//*****************************************************************************
AtProcess_Status AtParams_parseIncoming( char *param, uint8_t len );

/***************************************************************************************************
 ***************************************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* atParams__include */
