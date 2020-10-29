/**************************************************************************************************
  Filename:       mt_ota.h
  Revised:        $Date: 2010-09-24 08:53:19 -0700 (Fri, 24 Sep 2010) $
  Revision:       $Revision: 23901 $


  Description:    MonitorTest functions for the ZCL OTA Upgrade.


  Copyright 2004-2007 Texas Instruments Incorporated. All rights reserved.

  IMPORTANT: Your use of this Software is limited to those specific rights
  granted under the terms of a software license agreement between the user
  who downloaded the software, his/her employer (which must be your employer)
  and Texas Instruments Incorporated (the "License").  You may not use this
  Software unless you agree to abide by the terms of the License. The License
  limits your use, and you acknowledge, that the Software may not be modified,
  copied or distributed unless embedded on a Texas Instruments microcontroller
  or used solely and exclusively in conjunction with a Texas Instruments radio
  frequency transceiver, which is integrated into your product.  Other than for
  the foregoing purpose, you may not use, reproduce, copy, prepare derivative
  works of, modify, distribute, perform, display or sell this Software and/or
  its documentation for any purpose.

  YOU FURTHER ACKNOWLEDGE AND AGREE THAT THE SOFTWARE AND DOCUMENTATION ARE
  PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED,
  INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF MERCHANTABILITY, TITLE,
  NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT SHALL
  TEXAS INSTRUMENTS OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER CONTRACT,
  NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR OTHER
  LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
  INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE
  OR CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT
  OF SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
  (INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.

  Should you have any questions regarding your right to use this Software,
  contact Texas Instruments Incorporated at www.TI.com.
**************************************************************************************************/

/***************************************************************************************************
 * INCLUDES
 ***************************************************************************************************/
#include "zcomdef.h"
#include "mt.h"
#include "aps_mede.h"
#include "af.h"
#include "ota_common.h"

#if !defined( WIN32 )

#endif

typedef struct
{
  OsalPort_EventHdr hdr;
  uint8_t len;
  uint8_t cmd;
  uint8_t data[];
} OTA_MtMsg_t;

/***************************************************************************************************
 * CONSTANTS
 ***************************************************************************************************/

#define MT_OTA_FILE_READ_REQ_LEN                          26
#define MT_OTA_FILE_READ_RSP_LEN                          26

#define MT_OTA_GET_IMG_MSG_LEN                            31


/***************************************************************************************************
 * EXTERNAL FUNCTIONS
 ***************************************************************************************************/

/*
 *   Process all the MT OTA commands that are issued by OTA Console
 */
extern uint8_t MT_OtaCommandProcessing(uint8_t* pBuf);

/*
 *   Send messages to OTA Console from ZCL OTA
 */
extern uint8_t MT_OtaFileReadReq(afAddrType_t *pAddr, zclOTA_FileID_t *pFileId,
                               uint8_t len, uint32_t offset);

extern uint8_t MT_OtaGetImage(afAddrType_t *pAddr, zclOTA_FileID_t *pFileId,
                            uint16_t hwVer, uint8_t *ieee, uint8_t options);

extern uint8_t MT_OtaSendStatus(uint16_t shortAddr, uint8_t type, uint8_t status, uint8_t optional);

/*
 * Registration for MT OTA Callback Messages
 */
extern void MT_OtaRegister(uint8_t taskId);

/***************************************************************************************************
***************************************************************************************************/
