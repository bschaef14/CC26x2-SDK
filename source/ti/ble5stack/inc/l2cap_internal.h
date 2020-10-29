/******************************************************************************

 @file  l2cap_internal.h

 @brief This file contains internal interfaces for the L2CAP module.

 Group: WCS, BTS
 Target Device: cc13x2_26x2

 ******************************************************************************
 
 Copyright (c) 2009-2020, Texas Instruments Incorporated
 All rights reserved.

 IMPORTANT: Your use of this Software is limited to those specific rights
 granted under the terms of a software license agreement between the user
 who downloaded the software, his/her employer (which must be your employer)
 and Texas Instruments Incorporated (the "License"). You may not use this
 Software unless you agree to abide by the terms of the License. The License
 limits your use, and you acknowledge, that the Software may not be modified,
 copied or distributed unless embedded on a Texas Instruments microcontroller
 or used solely and exclusively in conjunction with a Texas Instruments radio
 frequency transceiver, which is integrated into your product. Other than for
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

 ******************************************************************************
 
 
 *****************************************************************************/

/*********************************************************************
 *
 * WARNING!!!
 *
 * THE API'S FOUND IN THIS FILE ARE FOR INTERNAL STACK USE ONLY!
 * FUNCTIONS SHOULD NOT BE CALLED DIRECTLY FROM APPLICATIONS, AND ANY
 * CALLS TO THESE FUNCTIONS FROM OUTSIDE OF THE STACK MAY RESULT IN
 * UNEXPECTED BEHAVIOR
 *
 */

#ifndef L2CAP_INTERNAL_H
#define L2CAP_INTERNAL_H

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */
#include "hci.h"
#include "l2cap.h"

/*********************************************************************
 * MACROS
 */

// Macro to see if a given channel is a fixed channel
#define FIX_CHANNEL( CID )        ( (CID) == L2CAP_CID_SIG ||\
                                    (CID) == L2CAP_CID_ATT ||\
                                    (CID) == L2CAP_CID_SMP ||\
                                    (CID) == L2CAP_CID_GENERIC )

// Marco to convert a dynamic channel ID to an index into L2CAP Channel table
#define CID_TO_INDEX( CID )       ( (CID) - L2CAP_DYNAMIC_CID_MIN )

// Marco to convert a fixed channel ID to an index into L2CAP Fixed Channel table
#define FCID_TO_INDEX( CID )      ( (CID) - L2CAP_CID_ATT )

// Macro to return the record maintained a given fix channel
#define FIX_CHANNEL_REC( CID )    ( l2capFixedChannels[FCID_TO_INDEX( CID )] )

// Macro to return a pointer to PSM that Connection Oriented Channel belongs to
#define CoC_PSM( cidx )           ( ( l2capChannels[(cidx)].pCoC != NULL )          ? \
                                    &(l2capPsm[l2capChannels[(cidx)].pCoC->psmIdx]) : \
                                    NULL )

// Macro to return the index of an PSM entry
#define PSM_IDX( pPsm )           ( (pPsm) - l2capPsm )

/*********************************************************************
 * CONSTANTS
 */

// Task events
#define L2CAP_SEND_PKT_EVT              0x0001

// Signaling command header: Code (1 byte) + Identifier (1 byte) + Length (2 bytes)
#define SIGNAL_HDR_SIZE                 4

// Maximum size of data field of Signaling commands
#define SIGNAL_DATA_SIZE                ( L2CAP_SIG_MTU_SIZE - SIGNAL_HDR_SIZE )

/*********************************************************************
 * L2CAP CO Channel Status: status used for l2capCoChannel 'status' field
 */
// Channel is out of credit (i.e., local device cannot transmit SDU)
#define L2CAP_OUT_OF_CREDIT             0xFF

/*********************************************************************
 * TYPEDEFS
 */

// L2CAP Connection Oriented Channel structure. Allocated one for each channel.
typedef struct
{
  // Channel info
  uint16 credits;     // Number of LE-frames can be sent by local device
  uint16 peerCID;     // Peer channel id
  uint16 peerMtu;     // Maximum SDU size that can be received by peer device
  uint16 peerMps;     // Maximum payload size that can be received by peer device
  uint16 peerCredits; // Number of LE-frames can be sent by peer device

  // Tx SDU info
  uint8 *pTxSdu;      // SDU to be sent - pointer returned by osal_bm_alloc()
  uint16 txLen;       // Total length of SDU being sent
  uint16 txOffset;    // Start index of next segment being sent

  // Rx SDU info
  uint8 *pRxSdu;      // SDU to be received - pointer returned by osal_bm_alloc()
  uint16 rxLen;       // Total length of SDU being received
  uint16 rxOffset;    // Start index of next segment being received

  // PSM info
  uint8 psmIdx;       // Index of PSM that channel belongs to
  uint8 status;       // Channel status
} l2capCoChannel_t;

// L2CAP Channel structure. Allocated one per application connection
// between two devices. CID assignment is relative to a particular device
// and a device can assign CIDs independently from other devices (except
// for the fixed CIDs). The CIDs are dynamically allocated in the range
// from 0x0040 to 0x007F. The CIDs from 0x0080 to 0xFFFF are reserved.
typedef struct
{
  // Channel info
  uint8  state;      // Channel connection state
  uint16 CID;        // Local channel id
  uint8  id;         // Local identifier - matches responses with requests

  // Link info
  uint16 connHandle; // link connection handle

  // Application info
  uint8 taskId;      // task that channel belongs to

  // Timer id
  uint8 timerId;

  // Connection Oriented Channel info
  l2capCoChannel_t *pCoC;
} l2capChannel_t;

// L2CAP Fixed Channel structure. Allocated one for each fixed channel.
typedef struct
{
  uint16 CID;        // fixed channel id
  uint8 taskId;      // task registered with fixed channel

  // Buffered tx packet info
  uint8 *pPayload;   // pointer to information payload
  uint16 len;        // length of information payload
  uint16 connHandle; // connection packet to be sent on
} l2capFixedChannel_t;

// Signaling packet header format
typedef struct
{
  uint8 opcode;      // type of command
  uint8 id;          // identifier - matches responses with requests
  uint16 len;        // length of data field (doesn't cover Code, Identifier and Length fields)
} l2capSignalHdr_t;

/**
 * @brief   Callback function prototype for building a Signaling command.
 *
 * @param   pBuf - pointer to buffer to hold command data
 * @param   pData - pointer to command data
 *
 * @return  length of the command data
 */
typedef uint16 (*pfnL2CAPBuildCmd_t)( uint8 *pBuf, uint8 *pData );

/*********************************************************************
 * GLOBAL VARIABLES
 */
extern uint8 l2capTaskID;
extern uint8 flowCtrlTaskId;
extern uint8 flowCtrlFwdTaskId;
extern l2capFixedChannel_t l2capFixedChannels[];
extern l2capChannel_t *l2capChannels;
extern uint8 l2capNumChannels;
extern uint8 l2capNumCoChannels;
extern uint16 l2capMtuSize;
extern uint8 l2capNumDataPkts;
extern l2capPsm_t *l2capPsm;
extern uint8 l2capNumPSM;
extern l2capUserCfg_t l2capUserConfig;

/*********************************************************************
 * FUNCTIONS - API
 */

extern bStatus_t l2capStoreFCPkt( uint16 connHandle, l2capPacket_t *pPkt );

extern uint8 l2capSendPkt( void );

extern bStatus_t l2capEncapSendData( uint16 connHandle, l2capPacket_t *pPkt );

extern bStatus_t l2capSendCmd( uint16 connHandle, uint8 opcode, uint8 id, uint8 *pCmd, pfnL2CAPBuildCmd_t pfnBuildCmd );

extern bStatus_t l2capSendReq( uint16 connHandle, uint8 opcode, uint8 *pReq, pfnL2CAPBuildCmd_t pfnBuildCmd, uint8 state, uint8 taskId, l2capPsm_t *pPsm );

extern uint16 l2capBuildInfoReq( uint8 *pBuf, uint8 *pCmd );

extern uint16 l2capBuildParamUpdateReq( uint8 *pBuf, uint8 *pData );

extern l2capChannel_t *l2capFindLocalId( uint8 id );

extern void l2capFreeChannel( l2capChannel_t *pChannel );

extern void l2capStartTimer( l2capChannel_t *pChannel, uint16 timeout );

extern void l2capStopTimer( l2capChannel_t *pChannel );

extern void l2capHandleRxError( uint16 connHandle );

extern bStatus_t l2capNotifyData( uint8 taskId, uint16 connHandle, l2capPacket_t *pPkt );

extern void l2capNotifySignal( uint8 taskId, uint16 connHandle, uint8 status, uint8 opcode, uint8 id, l2capSignalCmd_t *pCmd );

extern uint8 l2capParsePacket( l2capPacket_t *pPkt, hciDataEvent_t *pHciMsg );

extern void l2capParseSignalHdr( l2capSignalHdr_t *pHdr, uint8 *pData );

extern bStatus_t l2capParseCmdReject( l2capSignalCmd_t *pCmd, uint8 *pData, uint16 len );

extern bStatus_t l2capParseInfoRsp( l2capSignalCmd_t *pCmd, uint8 *pData, uint16 len );

extern bStatus_t l2capParseParamUpdateRsp( l2capSignalCmd_t *pCmd, uint8 *pData, uint16 len );

extern bStatus_t l2capAllocConnChannel( uint16 connHandle, uint8 taskId, l2capPsm_t *pPsm, l2capChannel_t **p2pChannel );

extern void l2capDisconnectAllChannels( uint16 connHandle, uint16 reason );

extern bStatus_t l2capAllocPsm( l2capPsm_t *pPsm );

extern uint8 l2capNumActiveChannnels( l2capPsm_t *pPsm );

extern l2capPsm_t *l2capFindPsm( uint16 psm );

extern uint8 l2capReassembleSegment( uint16 connHandle, l2capPacket_t *pPkt );

extern bStatus_t l2capSendConnectRsp( uint16 connHandle, uint8 id, uint16 result, l2capChannel_t *pChannel );

extern uint16 l2capBuildConnectRsp( uint8 *pBuf, uint8 *pData );

extern bStatus_t l2capParseConnectRsp( l2capSignalCmd_t *pCmd, uint8 *pData, uint16 len );

extern uint16 l2capBuildDisconnectReq( uint8 *pBuf, uint8 *pCmd );

extern bStatus_t l2capParseDisconnectReq( l2capSignalCmd_t *pCmd, uint8 *pData, uint16 len );

extern uint16 l2capBuildDisconnectRsp( uint8 *pBuf, uint8 *pData );

extern bStatus_t l2capParseDisconnectRsp( l2capSignalCmd_t *pCmd, uint8 *pData, uint16 len );

extern uint16 l2capBuildFlowCtrlCredit( uint8 *pBuf, uint8 *pCmd );

extern l2capChannel_t *l2capFindRemoteId( uint16 connHandle, uint8 id );

extern l2capChannel_t *l2capFindLocalCID( uint16 srcCID );

extern l2capChannel_t *l2capFindRemoteCID( uint16 connHandle, uint16 CID );

extern void l2capDisconnectChannel( l2capChannel_t *pChannel, uint16 reason );

extern bStatus_t l2capFlowCtrlCredit( uint16 connHandle, uint16 CID, uint16 peerCredits );

extern void l2capGetCoChannelInfo( l2capCoChannel_t *pCoC, l2capCoCInfo_t *pInfo );

extern void l2capNotifyChannelEstEvt( l2capChannel_t *pChannel, uint8 status, uint16 result );

extern void l2capNotifyChannelTermEvt( l2capChannel_t *pChannel, uint8 status, uint16 reason );

extern l2capChannel_t *l2capAllocChannel( l2capPsm_t *pPsm );

extern void l2capBuildSignalHdr( l2capSignalHdr_t *pHdr, uint8 *pData );

extern uint8 l2capSendFCPkt( uint16 CID );

extern void l2capFreePendingPkt( uint16 CID, uint16 connHandle, uint8 cmd );

extern void l2capNotifyEvent( l2capChannel_t *pChannel, uint8 status );

extern void l2capHandleTimerCB( uint8 *pData );

extern bStatus_t L2CAP_SendDataPkt( uint16 connHandle, uint16 pktLen, uint8 *pData );

extern uint8 l2capSendNextSegment( void );

extern uint8 l2capFindNextSegment( uint8 nextIdx );

extern uint8 l2capSendSegment( l2capChannel_t *pChannel );

extern void l2capFreeTxSDU( l2capChannel_t *pChannel, uint8 txStatus );

extern void l2capNotifyCreditEvt( l2capChannel_t *pChannel, uint8 event );

extern void l2capNotifySendSduDoneEvt( l2capChannel_t *pChannel, uint8 txStatus );

extern void l2capProcessOSALMsg( osal_event_hdr_t *pMsg );

extern void l2capProcessRxData( hciDataEvent_t *pHciMsg );

extern void l2capProcessSignal( uint16 connHandle, l2capPacket_t *pPkt );

extern bStatus_t l2capProcessRsp( uint16 connHandle, l2capSignalHdr_t *pHdr, uint8 *pData );

extern bStatus_t l2capProcessReq( uint16 connHandle, l2capSignalHdr_t *pHdr, uint8 *pData );

extern void l2capProcessConnectReq( uint16 connHandle, uint8 id, l2capConnectReq_t *pConnReq );

extern void l2capHandleConnStatusCB( uint16 connectionHandle, uint8 changeType );

extern void l2capRegisterATTReTxTask( uint8_t taskId );

/*********************************************************************
 * @fn      l2capDisconnectRsp
 *
 * @brief   Send Disconnect Response.
 *
 *          Use like: l2capDiscRsp( uint16 connHandle, uint8 id, l2capDiscRsp_t *pDiscRsp );
 *
 * @param   connHandle - connection to use
 * @param   id - identifier received in request
 * @param   pEchoRsp - pinter to Disconnect Response to be sent
 *
 * @return  SUCCESS: Request was sent successfully.
 *          INVALIDPARAMETER: Data can not fit into one packet.
 *          MSG_BUFFER_NOT_AVAIL: No HCI buffer is available.
 *          bleNotConnected: Connection is down.
 *          bleMemAllocError: Memory allocation error occurred.
 */
#define l2capDisconnectRsp( connHandle, id, pDiscRsp )  MAP_l2capSendCmd( (connHandle), L2CAP_DISCONNECT_RSP, (id),\
                                                                          (uint8 *)(pDiscRsp), MAP_l2capBuildDisconnectRsp )

/*********************************************************************
 * @fn      l2capInfoRsp
 *
 * @brief   Send Info Response.
 *
 *          Use like: l2capInfoRsp( uint16 connHandle, uint8 id, l2capInfoRsp_t *pInfoRsp );
 *
 * @param   connHandle - connection to use
 * @param   id - identifier received in request
 * @param   pInfoRsp - pointer to Info Response to be sent
 *
 * @return  SUCCESS: Request was sent successfully.
 *          INVALIDPARAMETER: Data can not fit into one packet.
 *          MSG_BUFFER_NOT_AVAIL: No HCI buffer is available.
 *          bleNotConnected: Connection is down.
 *          bleMemAllocError: Memory allocation error occurred.
 */
#define l2capInfoRsp( connHandle, id, pInfoRsp )  MAP_l2capSendCmd( (connHandle), L2CAP_INFO_RSP, (id),\
                                                                    (uint8 *)(pInfoRsp), MAP_L2CAP_BuildInfoRsp )

/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* L2CAP_INTERNAL_H */
