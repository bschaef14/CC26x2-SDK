/******************************************************************************

 @file  rtls_master.c

 @brief This file contains the RTLS Master sample application for use
        with the CC2650 Bluetooth Low Energy Protocol Stack.

 Group: WCS, BTS
 Target Device: cc13x2_26x2

 ******************************************************************************
 
 Copyright (c) 2013-2020, Texas Instruments Incorporated
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions
 are met:

 *  Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.

 *  Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.

 *  Neither the name of Texas Instruments Incorporated nor the names of
    its contributors may be used to endorse or promote products derived
    from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

 ******************************************************************************
 
 
 *****************************************************************************/

/*********************************************************************
 * INCLUDES
 */
#include <string.h>

#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Event.h>
#include <ti/sysbios/knl/Queue.h>

#include "bcomdef.h"
#include "l2cap.h"

#include <icall.h>
#include "util.h"
/* This Header file contains all BLE API and icall structure definition */
#include <icall_ble_api.h>
#include "osal_list.h"
#include "board_key.h"
#include <ti_drivers_config.h>

#include "ble_user_config.h"

#include "rtls_master.h"

#include "rtls_ctrl_api.h"
#include "rtls_ble.h"
#include "rtls_aoa_api.h"

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * CONSTANTS
 */

// Application events
#define RM_EVT_SCAN_ENABLED        0x01
#define RM_EVT_SCAN_DISABLED       0x02
#define RM_EVT_ADV_REPORT          0x03
#define RM_EVT_PAIR_STATE          0x04
#define RM_EVT_PASSCODE_NEEDED     0x05
#define RM_EVT_INSUFFICIENT_MEM    0x06
#define RM_EVT_RTLS_CTRL_MSG_EVT   0x07
#define RM_EVT_RTLS_SRV_MSG_EVT    0x08
#define RM_EVT_CONN_EVT            0x09

// RTLS Master Task Events
#define RM_ICALL_EVT                         ICALL_MSG_EVENT_ID  // Event_Id_31
#define RM_QUEUE_EVT                         UTIL_QUEUE_EVENT_ID // Event_Id_30

#define RM_ALL_EVENTS                        (RM_ICALL_EVT | RM_QUEUE_EVT)

// Address mode of the local device
// Note: When using the DEFAULT_ADDRESS_MODE as ADDRMODE_RANDOM or 
// ADDRMODE_RP_WITH_RANDOM_ID, GAP_DeviceInit() should be called with 
// it's last parameter set to a static random address
#define DEFAULT_ADDRESS_MODE                 ADDRMODE_PUBLIC

// Default PHY for scanning and initiating
#define DEFAULT_SCAN_PHY                     SCAN_PRIM_PHY_1M
#define DEFAULT_INIT_PHY                     INIT_PHY_1M

// Default scan duration in 10 ms
#define DEFAULT_SCAN_DURATION                200 // 2 sec

// Default supervision timeout in 10ms
#define DEFAULT_UPDATE_CONN_TIMEOUT          200

// Task configuration
#define RM_TASK_PRIORITY                     1

#ifndef RM_TASK_STACK_SIZE
#define RM_TASK_STACK_SIZE                   1024
#endif

// Advertising report fields to keep in the list
// Interested in only peer address type and peer address
#define RM_ADV_RPT_FIELDS   (SCAN_ADVRPT_FLD_ADDRTYPE | SCAN_ADVRPT_FLD_ADDRESS)

// Connection event registration
typedef enum
{
  NOT_REGISTERED     = 0x0,
  FOR_RTLS            = 0x2,
} connectionEventRegisterCause_u;

// Set the register cause to the registration bit-mask
#define CONNECTION_EVENT_REGISTER_BIT_SET(RegisterCause) (connEventRegCauseBitmap |= RegisterCause)
// Remove the register cause from the registration bit-mask
#define CONNECTION_EVENT_REGISTER_BIT_REMOVE(RegisterCause) (connEventRegCauseBitmap &= (~RegisterCause))
// Gets whether the current App is registered to the receive connection events
#define CONNECTION_EVENT_IS_REGISTERED (connEventRegCauseBitmap > 0)
// Gets whether the RegisterCause was registered to receive connection event
#define CONNECTION_EVENT_REGISTRATION_CAUSE(RegisterCause) (connEventRegCauseBitmap & RegisterCause)

// Hard coded PSM for passing data between central and peripheral
#define RTLS_PSM      0x0080
#define RTLS_PDU_SIZE MAX_PDU_SIZE

/*********************************************************************
 * TYPEDEFS
 */

// App event passed from profiles.
typedef struct
{
  appEvtHdr_t hdr; // event header
  uint8_t *pData;  // event data
} rmEvt_t;

// Container to store paring state info when passing from gapbondmgr callback
// to app event. See the pfnPairStateCB_t documentation from the gapbondmgr.h
// header file for more information on each parameter.
typedef struct
{
  uint16_t connHandle;
  uint8_t  status;
} rmPairStateData_t;

// Scanned device information record
typedef struct
{
  uint8_t addrType;         // Peer Device's Address Type
  uint8_t addr[B_ADDR_LEN]; // Peer Device Address
} scanRec_t;

// Container to store passcode data when passing from gapbondmgr callback
// to app event. See the pfnPasscodeCB_t documentation from the gapbondmgr.h
// header file for more information on each parameter.
typedef struct
{
  uint8_t deviceAddr[B_ADDR_LEN];
  uint16_t connHandle;
  uint8_t uiInputs;
  uint8_t uiOutputs;
  uint32_t numComparison;
} rmPasscodeData_t;

typedef struct
{
  uint16_t cocCID;
  uint8_t  isActive;
} rmConnCB_t;

/*********************************************************************
 * GLOBAL VARIABLES
 */

/*********************************************************************
 * EXTERNAL VARIABLES
 */
#define APP_EVT_BLE_LOG_STRINGS_MAX  0x9
char *appEvent_BleLogStrings[] = {
  "APP_EVT_ZERO              ",
  "APP_EVT_SCAN_ENABLED      ",
  "APP_EVT_SCAN_DISABLED     ",
  "APP_EVT_ADV_REPORT        ",
  "APP_EVT_PAIR_STATE        ",
  "APP_EVT_PASSCODE_NEEDED   ",
  "APP_EVT_INSUFFICIENT_MEM  ",
  "APP_EVT_RTLS_CTRL_MSG_EVT ",
  "APP_EVT_RTLS_SRV_MSG_EVT  ",
  "APP_EVT_CONN_EVT          ",
};

/*********************************************************************
 * LOCAL VARIABLES
 */

// Entity ID globally used to check for source and/or destination of messages
static ICall_EntityID selfEntity;

// Event globally used to post local events and pend on system and
// local events.

static ICall_SyncHandle syncEvent;
// Queue object used for app messages
static Queue_Struct appMsg;
static Queue_Handle appMsgQueue;

// Task configuration
Task_Struct rmTask;
#if defined __TI_COMPILER_VERSION__
#pragma DATA_ALIGN(rmTaskStack, 8)
#else
#pragma data_alignment=8
#endif
uint8_t rmTaskStack[RM_TASK_STACK_SIZE];

// Array of connection handles and information for each handle
static rmConnCB_t rmConnCB[MAX_NUM_BLE_CONNS];

// Address mode
static GAP_Addr_Modes_t addrMode = DEFAULT_ADDRESS_MODE;

// Number of scan results and scan result index
static uint8_t scanRes = 0;

// Scan result list
static scanRec_t scanList[DEFAULT_MAX_SCAN_RES];

// Handle the registration and un-registration for the connection event
uint32_t connEventRegCauseBitmap = NOT_REGISTERED;

/*********************************************************************
 * LOCAL FUNCTIONS
 */
static void RTLSMaster_init(void);
static void RTLSMaster_taskFxn(uintptr_t a0, uintptr_t a1);

static uint8_t RTLSMaster_processStackMsg(ICall_Hdr *pMsg);
static void RTLSMaster_processGapMsg(gapEventHdr_t *pMsg);
static void RTLSMaster_processAppMsg(rmEvt_t *pMsg);
static void RTLSMaster_processPairState(uint8_t state, rmPairStateData_t* pPairStateData);
static void RTLSMaster_processPasscode(rmPasscodeData_t *pData);
static void RTLSMaster_addDeviceInfo(GapScan_Evt_AdvRpt_t *pEvent);
static void RTLSMaster_passcodeCb(uint8_t *deviceAddr, uint16_t connHandle,
                                     uint8_t uiInputs, uint8_t uiOutputs,
                                     uint32_t numComparison);
static status_t RTLSMaster_enqueueMsg(uint8_t event, uint8_t status,
                                        uint8_t *pData);
static void RTLSMaster_pairStateCb(uint16_t connHandle, uint8_t state, uint8_t status);
static void RTLSMaster_scanCb(uint32_t evt, void* msg, uintptr_t arg);

// RTLS specific functions
static bStatus_t RTLSMaster_sendRTLSData(rtlsPacket_t *pMsg);
static void RTLSMaster_processRTLSScanReq(void);
static void RTLSMaster_processRTLSScanRes(GapScan_Evt_AdvRpt_t *deviceInfo);
static void RTLSMaster_processRTLSConnReq(bleConnReq_t *bleConnReq);
static void RTLSMaster_processRTLSConnInfo(uint16_t connHandle);
static void RTLSMaster_setAoaParamsReq(rtlsAoaConfigReq_t *config);
static void RTLSMaster_enableRtlsSync(rtlsEnableSync_t *enable);
static void RTLSMaster_connEvtCB(Gap_ConnEventRpt_t *pReport);
static void RTLSMaster_processConnEvt(Gap_ConnEventRpt_t *pReport);
static void RTLSMaster_processRtlsCtrlMsg(uint8_t *pMsg);
static void RTLSMaster_terminateLinkReq(rtlsTerminateLinkReq_t *termInfo);
static void RTLSMaster_rtlsSrvlMsgCb(rtlsSrv_evt_t *pRtlsSrvEvt);
static void RTLSMaster_processRtlsSrvMsg(rtlsSrv_evt_t *pEvt);
static void RTLSMaster_processRTLSUpdateConnInterval(rtlsUpdateConnIntReq_t *updateReq);

// L2CAP COC Handling
static bStatus_t RTLSMaster_openL2CAPChanCoc(uint16_t connHandle);
static void RTLSMaster_processL2CAPSignalEvent(l2capSignalEvent_t *pMsg);
static void RTLSMaster_processL2CAPDataEvent(l2capDataEvent_t *pMsg);

/*********************************************************************
 * EXTERN FUNCTIONS
 */
extern void AssertHandler(uint8 assertCause, uint8 assertSubcause);

/*********************************************************************
 * PROFILE CALLBACKS
 */

// Bond Manager Callbacks
static gapBondCBs_t bondMgrCBs =
{
 (pfnPasscodeCB_t)RTLSMaster_passcodeCb, // Passcode callback
  RTLSMaster_pairStateCb // Pairing/Bonding state Callback
};

/*********************************************************************
 * PUBLIC FUNCTIONS
 */

/*********************************************************************
 * @fn      RTLSMaster_createTask
 *
 * @brief   Task creation function for the RTLS Master.
 *
 * @param   none
 *
 * @return  none
 */
void RTLSMaster_createTask(void)
{
  Task_Params taskParams;

  // Configure task
  Task_Params_init(&taskParams);
  taskParams.stack = rmTaskStack;
  taskParams.stackSize = RM_TASK_STACK_SIZE;
  taskParams.priority = RM_TASK_PRIORITY;

  Task_construct(&rmTask, RTLSMaster_taskFxn, &taskParams, NULL);
}

/*********************************************************************
 * @fn      RTLSMaster_Init
 *
 * @brief   Initialization function for the RTLS Master App Task.
 *          This is called during initialization and should contain
 *          any application specific initialization (ie. hardware
 *          initialization/setup, table initialization, power up
 *          notification).
 *
 * @param   none
 *
 * @return  none
 */
static void RTLSMaster_init(void)
{
  BLE_LOG_INT_TIME(0, BLE_LOG_MODULE_APP, "APP : ---- init ", RM_TASK_PRIORITY);

  // ******************************************************************
  // N0 STACK API CALLS CAN OCCUR BEFORE THIS CALL TO ICall_registerApp
  // ******************************************************************
  // Register the current thread as an ICall dispatcher application
  // so that the application can send and receive messages.
  ICall_registerApp(&selfEntity, &syncEvent);

  // Create an RTOS queue for message from profile to be sent to app.
  appMsgQueue = Util_constructQueue(&appMsg);
  
  //Set default values for Data Length Extension
  //Extended Data Length Feature is already enabled by default
  //in build_config.opt in stack project.
  {
    //Change initial values of RX/TX PDU and Time, RX is set to max. by default(251 octets, 2120us)
    #define APP_SUGGESTED_RX_PDU_SIZE 251     //default is 251 octets(RX)
    #define APP_SUGGESTED_RX_TIME     17000   //default is 17000us(RX)
    #define APP_SUGGESTED_TX_PDU_SIZE 27      //default is 27 octets(TX)
    #define APP_SUGGESTED_TX_TIME     328     //default is 328us(TX)

    //This API is documented in hci.h
    //See the LE Data Length Extension section in the BLE5-Stack User's Guide for information on using this command:
    //http://software-dl.ti.com/lprf/ble5stack-latest/
    HCI_EXT_SetMaxDataLenCmd(APP_SUGGESTED_TX_PDU_SIZE, APP_SUGGESTED_TX_TIME, APP_SUGGESTED_RX_PDU_SIZE, APP_SUGGESTED_RX_TIME);
  }

  // Set Bond Manager parameters
  {
    // Don't send a pairing request after connecting; the device waits for the
    // application to start pairing
    uint8_t pairMode = GAPBOND_PAIRING_MODE_INITIATE;
    // Do not use authenticated pairing
    uint8_t mitm = FALSE;
    // This is a display only device
    uint8_t ioCap = GAPBOND_IO_CAP_DISPLAY_ONLY;
    // Create a bond during the pairing process
    uint8_t bonding = TRUE;

    GAPBondMgr_SetParameter(GAPBOND_PAIRING_MODE, sizeof(uint8_t), &pairMode);
    GAPBondMgr_SetParameter(GAPBOND_MITM_PROTECTION, sizeof(uint8_t), &mitm);
    GAPBondMgr_SetParameter(GAPBOND_IO_CAPABILITIES, sizeof(uint8_t), &ioCap);
    GAPBondMgr_SetParameter(GAPBOND_BONDING_ENABLED, sizeof(uint8_t), &bonding);
  }

  // Start Bond Manager and register callback
  // This must be done before initialing the GAP layer
  VOID GAPBondMgr_Register(&bondMgrCBs);

  // Accept all parameter update requests
  GAP_SetParamValue(GAP_PARAM_LINK_UPDATE_DECISION, GAP_UPDATE_REQ_ACCEPT_ALL);

  // Register with GAP for HCI/Host messages (for RSSI)
  GAP_RegisterForMsgs(selfEntity);

  BLE_LOG_INT_TIME(0, BLE_LOG_MODULE_APP, "APP : ---- call GAP_DeviceInit", GAP_PROFILE_CENTRAL);
  // Initialize GAP layer for Central role and register to receive GAP events
  GAP_DeviceInit(GAP_PROFILE_CENTRAL, selfEntity, addrMode, NULL);

  //Read the LE locally supported features
  HCI_LE_ReadLocalSupportedFeaturesCmd();

  // Initialize RTLS Services
  RTLSSrv_init(MAX_NUM_BLE_CONNS);
  RTLSSrv_register(RTLSMaster_rtlsSrvlMsgCb);
}

/*********************************************************************
 * @fn      RTLSMaster_taskFxn
 *
 * @brief   Application task entry point for the RTLS Master.
 *
 * @param   none
 *
 * @return  events not processed
 */
static void RTLSMaster_taskFxn(uintptr_t a0, uintptr_t a1)
{
  // Initialize application
  RTLSMaster_init();

  // Application main loop
  for (;;)
  {
    uint32_t events;

    events = Event_pend(syncEvent, Event_Id_NONE, RM_ALL_EVENTS,
                        ICALL_TIMEOUT_FOREVER);

    if (events)
    {
      ICall_EntityID dest;
      ICall_ServiceEnum src;
      ICall_HciExtEvt *pMsg = NULL;

      if (ICall_fetchServiceMsg(&src, &dest,
                                (void **)&pMsg) == ICALL_ERRNO_SUCCESS)
      {
        uint8 safeToDealloc = TRUE;

        if ((src == ICALL_SERVICE_CLASS_BLE) && (dest == selfEntity))
        {
          ICall_Stack_Event *pEvt = (ICall_Stack_Event *)pMsg;

          // Check for BLE stack events first
          if (pEvt->signature != 0xffff)
          {
            // Process inter-task message
            safeToDealloc = RTLSMaster_processStackMsg((ICall_Hdr *)pMsg);
          }
        }

        if (pMsg && safeToDealloc)
        {
          ICall_freeMsg(pMsg);
        }
      }

      // If RTOS queue is not empty, process app message
      if (events & RM_QUEUE_EVT)
      {
        rmEvt_t *pMsg;
        while (pMsg = (rmEvt_t *)Util_dequeueMsg(appMsgQueue))
        {
          // Process message
          RTLSMaster_processAppMsg(pMsg);

          // Free the space from the message
          ICall_free(pMsg);
        }
      }
    }
  }
}

/*********************************************************************
 * @fn      RTLSMaster_processStackMsg
 *
 * @brief   Process an incoming task message.
 *
 * @param   pMsg - message to process
 *
 * @return  TRUE if safe to deallocate incoming message, FALSE otherwise.
 */
static uint8_t RTLSMaster_processStackMsg(ICall_Hdr *pMsg)
{
  uint8_t safeToDealloc = TRUE;

  BLE_LOG_INT_INT(0, BLE_LOG_MODULE_APP, "APP : Stack msg status=%d, event=0x%x\n", pMsg->status, pMsg->event);

  switch (pMsg->event)
  {
    case GAP_MSG_EVENT:
      RTLSMaster_processGapMsg((gapEventHdr_t*) pMsg);
      break;

    case L2CAP_SIGNAL_EVENT:
      RTLSMaster_processL2CAPSignalEvent((l2capSignalEvent_t *)pMsg);
      break;

    case L2CAP_DATA_EVENT:
      RTLSMaster_processL2CAPDataEvent((l2capDataEvent_t *)pMsg);
      break;

    case HCI_GAP_EVENT_EVENT:
    {
      // Process HCI message
      switch (pMsg->status)
      {
        case HCI_COMMAND_COMPLETE_EVENT_CODE:
        {
          // Parse Command Complete Event for opcode and status
          hciEvt_CmdComplete_t* command_complete = (hciEvt_CmdComplete_t*) pMsg;

          //find which command this command complete is for
          switch (command_complete->cmdOpcode)
          {
            case HCI_LE_READ_LOCAL_SUPPORTED_FEATURES:
            {
              uint8_t featSet[8];

              // Get current feature set from received event (byte 1-8)
              memcpy( featSet, &command_complete->pReturnParam[1], 8 );

              // Clear the CSA#2 feature bit
              CLR_FEATURE_FLAG( featSet[1], LL_FEATURE_CHAN_ALGO_2 );

              // Enable CTE
              SET_FEATURE_FLAG( featSet[2], LL_FEATURE_CONNECTION_CTE_REQUEST );
              SET_FEATURE_FLAG( featSet[2], LL_FEATURE_CONNECTION_CTE_RESPONSE );
              SET_FEATURE_FLAG( featSet[2], LL_FEATURE_ANTENNA_SWITCHING_DURING_CTE_RX );
              SET_FEATURE_FLAG( featSet[2], LL_FEATURE_RECEIVING_CTE );

              // Update controller with modified features
              HCI_EXT_SetLocalSupportedFeaturesCmd( featSet );
            }
            break;

            default:
              break;
          }
        }
        break;

        case HCI_BLE_HARDWARE_ERROR_EVENT_CODE:
        {
          AssertHandler(HAL_ASSERT_CAUSE_HARDWARE_ERROR,0);
        }
        break;

        // LE Events
        case HCI_LE_EVENT_CODE:
        {
          hciEvt_BLEChanMapUpdate_t *pCMU = (hciEvt_BLEChanMapUpdate_t*) pMsg;

          // Update the host on channel map changes
          if (pCMU->BLEEventCode == HCI_BLE_CHANNEL_MAP_UPDATE_EVENT)
          {
            if (pCMU->connHandle != LINKDB_CONNHANDLE_INVALID)
            {
              BLE_LOG_INT_INT(0, BLE_LOG_MODULE_APP, "APP : Stack msg HCI_GAP_EVENT_EVENT HCI_LE_EVENT_CODE, HCI_BLE_CHANNEL_MAP_UPDATE_EVENT %d,0x%x\n", pMsg->status, pCMU->BLEEventCode);
              // Upon param update, resend connection information
              RTLSMaster_processRTLSConnInfo(pCMU->connHandle);
            }
          }

          break;
        }

        default:
          break;
      }
      break;
    }
    default:
      break;
  }

  return (safeToDealloc);
}

/*********************************************************************
 * @fn      RTLSMaster_processAppMsg
 *
 * @brief   Scanner application event processing function.
 *
 * @param   pMsg - pointer to event structure
 *
 * @return  none
 */
static void RTLSMaster_processAppMsg(rmEvt_t *pMsg)
{
  bool safeToDealloc = TRUE;

  if (pMsg->hdr.event <= APP_EVT_BLE_LOG_STRINGS_MAX)
  {
    if (pMsg->hdr.event != RM_EVT_CONN_EVT)
    {
      BLE_LOG_INT_STR(0, BLE_LOG_MODULE_APP, "APP : App msg status=%d, event=%s\n", 0, appEvent_BleLogStrings[pMsg->hdr.event]);
    }
  }
  else
  {
    BLE_LOG_INT_INT(0, BLE_LOG_MODULE_APP, "APP : App msg status=%d, event=0x%x\n", 0, pMsg->hdr.event);
  }

  switch (pMsg->hdr.event)
  {
    case RM_EVT_ADV_REPORT:
    {
      GapScan_Evt_AdvRpt_t* pAdvRpt = (GapScan_Evt_AdvRpt_t*) (pMsg->pData);

      char slaveScanRsp[] = {'R','T','L','S','S','l','a','v','e'};

      // Filter results by the slave's scanRsp array
      if (memcmp(&pAdvRpt->pData[2], slaveScanRsp, sizeof(slaveScanRsp)) == 0)
      {
        RTLSMaster_addDeviceInfo(pAdvRpt);
      }

      // Free report payload data
      if (pAdvRpt->pData != NULL)
      {
        ICall_free(pAdvRpt->pData);
      }
    }
    break;

    case RM_EVT_SCAN_DISABLED:
    {
      if(((gapEstLinkReqEvent_t*) pMsg)->hdr.status == SUCCESS)
      {
        // Scan stopped (no more results)
        RTLSCtrl_scanResultEvt(RTLS_SUCCESS, NULL, NULL);
      }
      else
      {
        // Scan stopped (failed due to wrong parameters)
        RTLSCtrl_scanResultEvt(RTLS_FAIL, NULL, NULL);
      }
    }
    break;

    // Pairing event
    case RM_EVT_PAIR_STATE:
    {
      RTLSMaster_processPairState(pMsg->hdr.state, (rmPairStateData_t*)(pMsg->pData));
    }
    break;

    // Passcode event
    case RM_EVT_PASSCODE_NEEDED:
    {
      RTLSMaster_processPasscode((rmPasscodeData_t *)(pMsg->pData));
    }
    break;

    case RM_EVT_RTLS_CTRL_MSG_EVT:
    {
      RTLSMaster_processRtlsCtrlMsg((uint8_t *)pMsg->pData);
    }
    break;

    case RM_EVT_RTLS_SRV_MSG_EVT:
    {
      RTLSMaster_processRtlsSrvMsg((rtlsSrv_evt_t *)pMsg->pData);
    }
    break;

    case RM_EVT_CONN_EVT:
    {
      RTLSMaster_processConnEvt((Gap_ConnEventRpt_t *)pMsg->pData);
    }
    break;

    default:
      // Do nothing.
    break;
  }

  if ((safeToDealloc == TRUE) && (pMsg->pData != NULL))
  {
    ICall_free(pMsg->pData);
  }
}

/*********************************************************************
 * @fn      RTLSMaster_processGapMsg
 *
 * @brief   GAP message processing function.
 *
 * @param   pMsg - pointer to event message structure
 *
 * @return  none
 */
static void RTLSMaster_processGapMsg(gapEventHdr_t *pMsg)
{
  uint16_t connHandle;

  switch (pMsg->opcode)
  {
    case GAP_DEVICE_INIT_DONE_EVENT:
    {
      uint8_t temp8;
      uint16_t temp16;

      BLE_LOG_INT_TIME(0, BLE_LOG_MODULE_APP, "APP : ---- got GAP_DEVICE_INIT_DONE_EVENT", 0);
      // Setup scanning
      // For more information, see the GAP section in the User's Guide:
      // http://software-dl.ti.com/lprf/ble5stack-latest/

      // Register callback to process Scanner events
      GapScan_registerCb(RTLSMaster_scanCb, NULL);

      // Set Scanner Event Mask
      GapScan_setEventMask(GAP_EVT_SCAN_ENABLED | GAP_EVT_SCAN_DISABLED |
                           GAP_EVT_ADV_REPORT);

      // Set Scan PHY parameters
      GapScan_setPhyParams(DEFAULT_SCAN_PHY, SCAN_TYPE_PASSIVE,
                           SCAN_PARAM_DFLT_INTERVAL, SCAN_PARAM_DFLT_WINDOW);

      // Set Advertising report fields to keep
      temp16 = RM_ADV_RPT_FIELDS;
      GapScan_setParam(SCAN_PARAM_RPT_FIELDS, &temp16);
      // Set Scanning Primary PHY
      temp8 = DEFAULT_SCAN_PHY;
      GapScan_setParam(SCAN_PARAM_PRIM_PHYS, &temp8);
      // Set LL Duplicate Filter
      temp8 = SCAN_FLT_DUP_ENABLE;
      GapScan_setParam(SCAN_PARAM_FLT_DUP, &temp8);

      // Set PDU type filter -
      // Only 'Connectable' and 'Complete' packets are desired.
      // It doesn't matter if received packets are
      // whether Scannable or Non-Scannable, whether Directed or Undirected,
      // whether Scan_Rsp's or Advertisements, and whether Legacy or Extended.
      temp16 = SCAN_FLT_PDU_CONNECTABLE_ONLY | SCAN_FLT_PDU_COMPLETE_ONLY;
      BLE_LOG_INT_TIME(0, BLE_LOG_MODULE_APP, "APP : ---- GapScan_setParam", 0);
      GapScan_setParam(SCAN_PARAM_FLT_PDU_TYPE, &temp16);
    }
    break;

    case GAP_LINK_ESTABLISHED_EVENT:
    {
      BLE_LOG_INT_TIME(0, BLE_LOG_MODULE_APP, "APP : ---- got GAP_LINK_ESTABLISHED_EVENT", 0);
      connHandle = ((gapEstLinkReqEvent_t *) pMsg)->connectionHandle;

      if (connHandle != LINKDB_CONNHANDLE_INVALID && connHandle < MAX_NUM_BLE_CONNS &&
          ((gapEstLinkReqEvent_t *) pMsg)->hdr.status == SUCCESS)
      {
        rmConnCB[connHandle].isActive = TRUE;

        HCI_LE_ReadRemoteUsedFeaturesCmd(connHandle);

        // We send out the connection information at this point
        // Note: we are not yet connected (will be after pairing)
        RTLSMaster_processRTLSConnInfo(connHandle);

      }
      else
      {
        // Link failed to establish
        RTLSCtrl_connResultEvt(LINKDB_CONNHANDLE_INVALID, RTLS_LINK_ESTAB_FAIL);
      }
    }
    break;

    case GAP_LINK_TERMINATED_EVENT:
    {
      BLE_LOG_INT_STR(0, BLE_LOG_MODULE_APP, "APP : GAP msg status=%d, opcode=%s\n", 0, "GAP_LINK_TERMINATED_EVENT");
      connHandle = ((gapTerminateLinkEvent_t *) pMsg)->connectionHandle;

      if (connHandle != LINKDB_CONNHANDLE_INVALID && connHandle < MAX_NUM_BLE_CONNS)
      {
        // This connection is inactive
        rmConnCB[connHandle].isActive = FALSE;

        // Link terminated
        RTLSCtrl_connResultEvt(connHandle, RTLS_LINK_TERMINATED);
      }
    }
    break;

    case GAP_LINK_PARAM_UPDATE_EVENT:
    {
      BLE_LOG_INT_STR(0, BLE_LOG_MODULE_APP, "APP : GAP msg status=%d, opcode=%s\n", 0, "GAP_LINK_PARAM_UPDATE_EVENT");
      connHandle = ((gapLinkUpdateEvent_t *) pMsg)->connectionHandle;

      if (connHandle != LINKDB_CONNHANDLE_INVALID && connHandle < MAX_NUM_BLE_CONNS &&
          ((gapLinkUpdateEvent_t *) pMsg)->hdr.status == SUCCESS)
      {
        // Upon param update, resend connection information
        RTLSMaster_processRTLSConnInfo(connHandle);
      }
    }
    break;

    default:
      break;
  }
}

/*********************************************************************
 * @fn      RTLSMaster_processPairState
 *
 * @brief   Process the new paring state.
 *
 * @return  none
 */
static void RTLSMaster_processPairState(uint8_t state, rmPairStateData_t* pPairData)
{
  uint8_t status = pPairData->status;

#ifdef RTLS_DEBUG
  RTLSCtrl_sendDebugEvt("RTLSMaster_processPairState", state);
#endif

  switch (state)
  {
    // Once Master and Slave are paired, we can open a COC channel
    case GAPBOND_PAIRING_STATE_COMPLETE:
    case GAPBOND_PAIRING_STATE_ENCRYPTED:
    {
      if (status == SUCCESS)
      {
        // We are paired, open a L2CAP channel to pass data
        if (RTLSMaster_openL2CAPChanCoc(pPairData->connHandle) != SUCCESS)
        {
          // We could not establish an L2CAP link, drop the connection
          // We will notify host that this connection is terminated when the GAP_LINK_TERMINATED_EVENT returns
          GAP_TerminateLinkReq(pPairData->connHandle, HCI_DISCONNECT_REMOTE_USER_TERM);
        }
      }
      else
      {
        // We could not establish an L2CAP link, drop the connection
        // We will notify host that this connection is terminated when the GAP_LINK_TERMINATED_EVENT returns
        GAP_TerminateLinkReq(pPairData->connHandle, HCI_DISCONNECT_REMOTE_USER_TERM);
      }
    }
    break;

    default:
      break;
  }
}

/*********************************************************************
 * @fn      RTLSMaster_processPasscode
 *
 * @brief   Process the Passcode request.
 *
 * @return  none
 */
static void RTLSMaster_processPasscode(rmPasscodeData_t *pData)
{
  // This app uses a default passcode. A real-life scenario would handle all
  // pairing scenarios and likely generate this randomly.
  uint32_t passcode = B_APP_DEFAULT_PASSCODE;

  // Send passcode response
  GAPBondMgr_PasscodeRsp(pData->connHandle, SUCCESS, passcode);
}

/*********************************************************************
 * @fn      RTLSMaster_addDeviceInfo
 *
 * @brief   Add a device to the device discovery result list
 *
 * @return  none
 */
static void RTLSMaster_addDeviceInfo(GapScan_Evt_AdvRpt_t *deviceInfo)
{
  uint8_t i;

  // If result count not at max
  if (scanRes < DEFAULT_MAX_SCAN_RES)
  {
    // Check if device is already in scan results
    for (i = 0; i < scanRes; i++)
    {
      if (memcmp(deviceInfo->addr, scanList[i].addr , B_ADDR_LEN) == 0)
      {
        return;
      }
    }

    // Send the device info to RTLS Control
    RTLSMaster_processRTLSScanRes(deviceInfo);

    // Add addr to scan result list
    memcpy(scanList[scanRes].addr, deviceInfo->addr, B_ADDR_LEN);
    scanList[scanRes].addrType = deviceInfo->addrType;

    // Increment scan result count
    scanRes++;
  }
}

/*********************************************************************
 * @fn      RTLSMaster_pairStateCb
 *
 * @brief   Pairing state callback.
 *
 * @param connectionHandle - connection handle of current pairing process
 * @param state - @ref GAPBondMgr_Events
 * @param status - pairing status
 *
 * @return  none
 */
static void RTLSMaster_pairStateCb(uint16_t connHandle, uint8_t state, uint8_t status)
{
  rmPairStateData_t *pData;

  // Allocate space for the event data.
  if ((pData = ICall_malloc(sizeof(rmPairStateData_t))))
  {
    pData->connHandle = connHandle;
    pData->status = status;

    // Queue the event.
    if (RTLSMaster_enqueueMsg(RM_EVT_PAIR_STATE, state, (uint8_t*) pData) != SUCCESS)
    {
      ICall_free(pData);
    }
  }
}

/*********************************************************************
* @fn      RTLSMaster_passcodeCb
*
* @brief   Passcode callback.
*
* @param   deviceAddr - pointer to device address
* @param   connHandle - the connection handle
* @param   uiInputs - pairing User Interface Inputs
* @param   uiOutputs - pairing User Interface Outputs
* @param   numComparison - numeric Comparison 20 bits
*
* @return  none
*/
static void RTLSMaster_passcodeCb(uint8_t *deviceAddr, uint16_t connHandle,
                                  uint8_t uiInputs, uint8_t uiOutputs,
                                  uint32_t numComparison)
{
  rmPasscodeData_t *pData = ICall_malloc(sizeof(rmPasscodeData_t));

  // Allocate space for the passcode event.
  if (pData)
  {
    pData->connHandle = connHandle;
    memcpy(pData->deviceAddr, deviceAddr, B_ADDR_LEN);
    pData->uiInputs = uiInputs;
    pData->uiOutputs = uiOutputs;
    pData->numComparison = numComparison;

    // Enqueue the event.
    if (RTLSMaster_enqueueMsg(RM_EVT_PASSCODE_NEEDED, 0,(uint8_t *) pData) != SUCCESS)
    {
      ICall_free(pData);
    }
  }
}

/*********************************************************************
 * @fn      RTLSMaster_enqueueMsg
 *
 * @brief   Creates a message and puts the message in RTOS queue.
 *
 * @param   event - message event.
 * @param   state - message state.
 * @param   pData - message data pointer.
 *
 * @return  TRUE or FALSE
 */
static status_t RTLSMaster_enqueueMsg(uint8_t event, uint8_t state,
                                           uint8_t *pData)
{
  uint8_t success;
  rmEvt_t *pMsg = ICall_malloc(sizeof(rmEvt_t));

  // Create dynamic pointer to message.
  if (pMsg)
  {
    pMsg->hdr.event = event;
    pMsg->hdr.state = state;
    pMsg->pData = pData;

    // Enqueue the message.
    success = Util_enqueueMsg(appMsgQueue, syncEvent, (uint8_t *)pMsg);
    return (success) ? SUCCESS : FAILURE;
  }

  return(bleMemAllocError);
}

/*********************************************************************
 * @fn      RTLSMaster_scanCb
 *
 * @brief   Callback called by GapScan module
 *
 * @param   evt - event
 * @param   msg - message coming with the event
 * @param   arg - user argument
 *
 * @return  none
 */
static void RTLSMaster_scanCb(uint32_t evt, void* pMsg, uintptr_t arg)
{
  uint8_t event;

  if (evt & GAP_EVT_ADV_REPORT)
  {
    event = RM_EVT_ADV_REPORT;
  }
  else if (evt & GAP_EVT_SCAN_ENABLED)
  {
    event = RM_EVT_SCAN_ENABLED;
  }
  else if (evt & GAP_EVT_SCAN_DISABLED)
  {
    event = RM_EVT_SCAN_DISABLED;
  }
  else
  {
    return;
  }

  if (RTLSMaster_enqueueMsg(event, SUCCESS, pMsg) != SUCCESS)
  {
    ICall_free(pMsg);
  }
}

/*********************************************************************
 * @fn      RTLSMaster_connEvtCB
 *
 * @brief   Connection event callback.
 *
 * @param   pReport pointer to connection event report
 */
static void RTLSMaster_connEvtCB(Gap_ConnEventRpt_t *pReport)
{
  // Enqueue the event for processing in the app context.
  if (RTLSMaster_enqueueMsg(RM_EVT_CONN_EVT, SUCCESS, (uint8_t *)pReport) != SUCCESS)
  {
    ICall_free(pReport);
  }
}

/*********************************************************************
 * @fn      RTLSMaster_connEvtCB
 *
 * @brief   Connection event callback.
 *
 * @param   pReport - pointer to connection event report
 *
 * @return  none
 */
static void RTLSMaster_processConnEvt(Gap_ConnEventRpt_t *pReport)
{
  // Sanity check
  if (!pReport)
  {
  return;
  }

  if (CONNECTION_EVENT_REGISTRATION_CAUSE(FOR_RTLS) && rmConnCB[pReport->handle].isActive)
  {
    rtlsStatus_e status;

    // Convert BLE specific status to RTLS Status
    if (pReport->status != GAP_CONN_EVT_STAT_MISSED)
    {
      status = RTLS_SUCCESS;
    }
    else
    {
      status = RTLS_FAIL;
    }

#ifdef RTLS_TEST_CHAN_MAP_DYNAMIC_CHANGE
    // For testing - do dynamic change of the channel map after 10 connection events
    {
        static int connectionEventCount = 0;

        if (++connectionEventCount == 10)
        {
          uint8_t chanMap[5] = {0xFF, 0xFF, 0xFF, 0x00, 0x1F};      // unmap channels 24..31
          HCI_LE_SetHostChanClassificationCmd(chanMap);
        }
    }
#endif //RTLS_TEST_CHAN_MAP_DYNAMIC_CHANGE

    RTLSCtrl_syncNotifyEvt(pReport->handle, status, pReport->nextTaskTime, pReport->lastRssi, pReport->channel);
  }

  if (pReport != NULL)
  {
    // Free the report once we are done using it
    ICall_free(pReport);
  }
}

/*********************************************************************
 * @fn      RTLSMaster_sendRTLSData
 *
 * @brief   Send RTLS data to the peer
 *
 * @param   pMsg - pointer to the message to send
 *
 * @return  none
 */
static bStatus_t RTLSMaster_sendRTLSData(rtlsPacket_t *pMsg)
{
  l2capPacket_t pkt;
  bStatus_t status = SUCCESS;

  // Sanity check
  if (!pMsg)
  {
    return FAILURE;
  }

  // Tell L2CAP the desired Channel ID
  pkt.CID = rmConnCB[pMsg->connHandle].cocCID;

  // Allocate space for payload
  pkt.pPayload = L2CAP_bm_alloc(pMsg->payloadLen);

  if (pkt.pPayload != NULL)
  {
    // The request is the payload for the L2CAP SDU
    memcpy(pkt.pPayload, pMsg, pMsg->payloadLen);
    pkt.len = pMsg->payloadLen;
    status = L2CAP_SendSDU(&pkt);

    // Check that the packet was sent
    if (SUCCESS != status)
    {
      // If SDU wasn't sent, free
      BM_free(pkt.pPayload);
    }
  }
  else
  {
    status = bleMemAllocError;
  }

  return (status);
}

/*********************************************************************
 * @fn      RTLSMaster_processRTLSScanRes
 *
 * @brief   Process a scan response and forward to RTLS Control
 *
 * @param   deviceInfo - a single scan response
 *
 * @return  none
 */
static void RTLSMaster_processRTLSScanRes(GapScan_Evt_AdvRpt_t *deviceInfo)
{
  GapScan_Evt_AdvRpt_t *devInfo;
  size_t resSize;
  bleScanInfo_t *scanResult;

  // Sanity check
  if (!deviceInfo)
  {
    return;
  }

  devInfo = deviceInfo;

  // Assign and allocate space
  resSize = sizeof(bleScanInfo_t) + devInfo->dataLen;
  scanResult = (bleScanInfo_t *)ICall_malloc(resSize);

  // We could not allocate memory, report to host and exit
  if (!scanResult)
  {
    RTLSCtrl_scanResultEvt(RTLS_OUT_OF_MEMORY, NULL, 0);
    return;
  }

  memcpy(scanResult->addr, devInfo->addr, B_ADDR_LEN);
  scanResult->addrType = devInfo->addrType;
  scanResult->eventType = devInfo->evtType;
  scanResult->dataLen = devInfo->dataLen;
  scanResult->rssi = devInfo->rssi;
  memcpy(scanResult->pEvtData, devInfo->pData, devInfo->dataLen);

  RTLSCtrl_scanResultEvt(RTLS_SUCCESS, (uint8_t*)scanResult, resSize);

  ICall_free(scanResult);
}

/*********************************************************************
 * @fn      RTLSMaster_processRTLSScanReq
 *
 * @brief   Process a scan request
 *
 * @param   none
 *
 * @return  none
 */
static void RTLSMaster_processRTLSScanReq(void)
{
  scanRes = 0;

  // Start discovery
  GapScan_enable(0, DEFAULT_SCAN_DURATION, DEFAULT_MAX_SCAN_RES);
}

/*********************************************************************
 * @fn      RTLSMaster_processRTLSConnReq
 *
 * @brief   Start the connection process with another device
 *
 * @param   bleConnReq - pointer from RTLS control containing connection params
 *
 * @return  none
 */
static void RTLSMaster_processRTLSConnReq(bleConnReq_t *bleConnReq)
{
  // Sanity check
  if (!bleConnReq)
  {
   return;
  }

  //Set connection interval and supervision timeout
  GapInit_setPhyParam(INIT_PHY_1M | INIT_PHY_2M | INIT_PHY_CODED, INIT_PHYPARAM_CONN_INT_MAX, bleConnReq->connInterval);
  GapInit_setPhyParam(INIT_PHY_1M | INIT_PHY_2M | INIT_PHY_CODED, INIT_PHYPARAM_CONN_INT_MIN, bleConnReq->connInterval);
  GapInit_setPhyParam(INIT_PHY_1M | INIT_PHY_2M | INIT_PHY_CODED, INIT_PHYPARAM_SUP_TIMEOUT, DEFAULT_UPDATE_CONN_TIMEOUT);

  GapInit_connect(bleConnReq->addrType & MASK_ADDRTYPE_ID, bleConnReq->addr, DEFAULT_INIT_PHY, 0);
}

/*********************************************************************
 * @fn      RTLSMaster_processRTLSConnRes
 *
 * @brief   Process a connection established event - send conn info to RTLS Control
 *
 * @param   connHandle - connection handle
 *
 * @return  none
 */
static void RTLSMaster_processRTLSConnInfo(uint16_t connHandle)
{
  hciActiveConnInfo_t connInfo;
  linkDBInfo_t addrInfo;
  bleConnInfo_t rtlsConnInfo = {0};

  // Get BD Address of the requested Slave
  linkDB_GetInfo(connHandle, &addrInfo);
  memcpy(rtlsConnInfo.addr, addrInfo.addr, B_ADDR_LEN);

  // Get current active connection information
  HCI_EXT_GetActiveConnInfoCmd(connHandle, &connInfo);

  BLE_LOG_INT_INT(0, BLE_LOG_MODULE_APP, "APP : RTLSConnInfo hopValue=%d, currChan=%d\n", connInfo.hopValue, connInfo.nextChan);
  rtlsConnInfo.connHandle = connHandle;
  rtlsConnInfo.accessAddr = connInfo.accessAddr;
  rtlsConnInfo.connInterval = connInfo.connInterval;
  rtlsConnInfo.currChan = connInfo.nextChan;
  rtlsConnInfo.hopValue = connInfo.hopValue;
  rtlsConnInfo.mSCA = connInfo.mSCA;
  rtlsConnInfo.crcInit = BUILD_UINT32(connInfo.crcInit[0], connInfo.crcInit[1], connInfo.crcInit[2], 0);
  memcpy(rtlsConnInfo.chanMap, connInfo.chanMap, LL_NUM_BYTES_FOR_CHAN_MAP);

  RTLSCtrl_connInfoEvt((uint8_t*)&rtlsConnInfo, sizeof(bleConnInfo_t));
}

/*********************************************************************
 * @fn      RTLSMaster_openL2CAPChanCoc
 *
 * @brief   Opens a communication channel between RTLS Master/Slave
 *
 * @param   connHandle - connection handle
 *
 * @return  status - 0 = success, 1 = failed
 */
static bStatus_t RTLSMaster_openL2CAPChanCoc(uint16_t connHandle)
{
  uint8_t ret;
  l2capPsm_t psm;
  l2capPsmInfo_t psmInfo;

  if (L2CAP_PsmInfo(RTLS_PSM, &psmInfo) == INVALIDPARAMETER)
  {
    // Prepare the PSM parameters
    psm.initPeerCredits = 0xFFFF;
    psm.maxNumChannels = MAX_NUM_BLE_CONNS;
    psm.mtu = RTLS_PDU_SIZE;
    psm.peerCreditThreshold = 0;
    psm.pfnVerifySecCB = NULL;
    psm.psm = RTLS_PSM;
    psm.taskId = ICall_getLocalMsgEntityId(ICALL_SERVICE_CLASS_BLE_MSG, selfEntity);

    // Register PSM with L2CAP task
    ret = L2CAP_RegisterPsm(&psm);

    if (ret == SUCCESS)
    {
      // Send the connection request to RTLS slave
      ret = L2CAP_ConnectReq(connHandle, RTLS_PSM, RTLS_PSM);
    }
  }
  else
  {
    // Send the connection request to RTLS slave
    ret = L2CAP_ConnectReq(connHandle, RTLS_PSM, RTLS_PSM);
  }

  return ret;
}

/*********************************************************************
 * @fn      RTLSMaster_processL2CAPSignalEvent
 *
 * @brief   Handle L2CAP signal events
 *
 * @param   pMsg - pointer to the signal that was received
 *
 * @return  none
 */
static void RTLSMaster_processL2CAPSignalEvent(l2capSignalEvent_t *pMsg)
{
  // Sanity check
  if (!pMsg)
  {
    return;
  }

  switch (pMsg->opcode)
  {
    case L2CAP_CHANNEL_ESTABLISHED_EVT:
    {
      l2capChannelEstEvt_t *pEstEvt = &(pMsg->cmd.channelEstEvt);

      // Connection established, save the CID
      if (pMsg->connHandle != LINKDB_CONNHANDLE_INVALID && pMsg->connHandle < MAX_NUM_BLE_CONNS)
      {
        rmConnCB[pMsg->connHandle].cocCID = pEstEvt->CID;

        // Give max credits to the other side
        L2CAP_FlowCtrlCredit(pEstEvt->CID, 0xFFFF);

        // L2CAP establishing a COC channel means that both Master and Slave are ready
        // Tell RTLS Control that we are ready for more commands
        RTLSCtrl_connResultEvt(pMsg->connHandle, RTLS_SUCCESS);
      }
      else
      {
        // We could not establish an L2CAP link, drop the connection
        RTLSCtrl_sendDebugEvt("L2CAP COC: could not establish", pMsg->connHandle);
        GAP_TerminateLinkReq(pMsg->connHandle, HCI_DISCONNECT_REMOTE_USER_TERM);
      }
    }
    break;

    case L2CAP_SEND_SDU_DONE_EVT:
    {
      if (pMsg->hdr.status == SUCCESS)
      {
        RTLSCtrl_dataSentEvt(pMsg->connHandle, RTLS_SUCCESS);
      }
      else
      {
        RTLSCtrl_dataSentEvt(pMsg->connHandle, RTLS_FAIL);
      }
    }
    break;

    case L2CAP_CHANNEL_TERMINATED_EVT:
    {
      // Terminate the connection
      GAP_TerminateLinkReq(pMsg->connHandle, HCI_DISCONNECT_REMOTE_USER_TERM);
      RTLSCtrl_sendDebugEvt("L2CAP COC: terminated connHandle: ", pMsg->connHandle);
    }
    break;
  }
}

/*********************************************************************
 * @fn      RTLSMaster_processL2CAPDataEvent
 *
 * @brief   Handles incoming L2CAP data
 *          RTLS Master does not expect any incoming data
 *
 * @param   pMsg - pointer to the signal that was received
 *
 * @return  none
 */
static void RTLSMaster_processL2CAPDataEvent(l2capDataEvent_t *pMsg)
{
  // Sanity check
  if (!pMsg)
  {
    return;
  }

  // Free the payload (must use BM_free here according to L2CAP documentation)
  BM_free(pMsg->pkt.pPayload);
}

/*********************************************************************
 * @fn      RTLSMaster_enableRtlsSync
 *
 * @brief   This function is used by RTLS Control to notify the RTLS application
 *          to start sending synchronization events (for BLE this is a connection event)
 *
 * @param   enable - start/stop synchronization
 *
 * @return  none
 */
static void RTLSMaster_enableRtlsSync(rtlsEnableSync_t *enable)
{
  bStatus_t status = RTLS_FALSE;

  if (enable->enable == RTLS_TRUE)
  {
    if (!CONNECTION_EVENT_IS_REGISTERED)
    {
      status = Gap_RegisterConnEventCb(RTLSMaster_connEvtCB, GAP_CB_REGISTER, LINKDB_CONNHANDLE_ALL);
    }

    if (status == SUCCESS)
    {
      CONNECTION_EVENT_REGISTER_BIT_SET(FOR_RTLS);
    }
  }
  else if (enable->enable == RTLS_FALSE)
  {
    CONNECTION_EVENT_REGISTER_BIT_REMOVE(FOR_RTLS);

    // If there is nothing registered to the connection event, request to unregister
    if (!CONNECTION_EVENT_IS_REGISTERED)
    {
      Gap_RegisterConnEventCb(RTLSMaster_connEvtCB, GAP_CB_UNREGISTER, LINKDB_CONNHANDLE_ALL);
    }
  }
}

/*********************************************************************
 * @fn      RTLSMaster_terminateLinkReq
 *
 * @brief   Terminate active link
 *
 * @param   termInfo - information about the connection to terminate
 *
 * @return  none
 */
static void RTLSMaster_terminateLinkReq(rtlsTerminateLinkReq_t *termInfo)
{
  if (termInfo->connHandle != LINKDB_CONNHANDLE_INVALID && termInfo->connHandle < MAX_NUM_BLE_CONNS)
  {
    L2CAP_DisconnectReq(rmConnCB[termInfo->connHandle].cocCID);
  }
  else
  {
    RTLSCtrl_sendDebugEvt("Connection Handle invalid", LINKDB_CONNHANDLE_INVALID);
  }
}

/*********************************************************************
 * @fn      RTLSMaster_setAoaParamsReq
 *
 * @brief   Configure AoA parameters to the BLE Stack
 *
 * @param   config - Parameters to configure
 *
 * @return  none
 */
static void RTLSMaster_setAoaParamsReq(rtlsAoaConfigReq_t *pConfig)
{
  PIN_Handle status;

  // Sanity check
  if (pConfig == NULL)
  {
    return;
  }

  // Initialize GPIO's specified in ble_user_config.c (antennaTbl)
  // Initialize one of the antenna ID's as the main antenna (in this case the first antenna in the pattern)
  // BOOSTXL-AOA array switch IO is handled by rtls_ctrl_aoa.c
  status = RTLSSrv_initAntArray(pConfig->pAntPattern[0]);

  if (status == NULL)
  {
    RTLSCtrl_sendDebugEvt("Antenna array configuration invalid", 0);
    AssertHandler(HAL_ASSERT_CAUSE_HARDWARE_ERROR,0);
  }

  // Configure AoA receiver parameters
  RTLSSrv_setConnCteReceiveParams(pConfig->connHandle,
                                  pConfig->samplingEnable,
                                  pConfig->slotDurations,
                                  pConfig->numAnt,
                                  pConfig->pAntPattern);

  // Configure sample accuracy
  RTLSSrv_setCteSampleAccuracy(pConfig->connHandle,
                               pConfig->sampleRate,
                               pConfig->sampleSize,
                               pConfig->sampleRate,
                               pConfig->sampleSize,
                               pConfig->sampleCtrl);
}

/*********************************************************************
 * @fn      RTLSMaster_enableAoaReq
 *
 * @brief   Enable sampling AoA
 *
 * @param   config - Parameters to configure
 *
 * @return  none
 */
static void RTLSMaster_enableAoaReq(rtlsAoaEnableReq_t *pReq)
{
  // Sanity check
  if (pReq == NULL)
  {
    return;
  }

  // Request CTE from our peer
  RTLSSrv_setConnCteRequestEnableCmd(pReq->connHandle,
                                     pReq->enableAoa,
                                     pReq->cteInterval,
                                     pReq->cteLength,
                                     RTLSSRV_CTE_TYPE_AOA);
}

/*********************************************************************
 * @fn      RTLSMaster_processRTLSUpdateConnInterval
 *
 * @brief   Update connection interval
 *
 * @param   updateReq - pointer from RTLS control containing connection params
 *
 * @return  none
 */
static void RTLSMaster_processRTLSUpdateConnInterval(rtlsUpdateConnIntReq_t *updateReq)
{
  gapUpdateLinkParamReq_t params;
  linkDBInfo_t linkInfo;

  // Sanity check
  if (!updateReq)
  {
    return;
  }

  if (linkDB_GetInfo(updateReq->connHandle, &linkInfo) == SUCCESS)
  {
    params.connLatency = linkInfo.connLatency;
    params.connTimeout = linkInfo.connTimeout;
    params.connectionHandle = updateReq->connHandle;

    // Min/Max set to the same value
    params.intervalMax = updateReq->connInterval;
    params.intervalMin = updateReq->connInterval;

    GAP_UpdateLinkParamReq(&params);
  }
}

/*********************************************************************
 * @fn      RTLSMaster_processRtlsCtrlMsg
 *
 * @brief   Handle processing messages from RTLS Control
 *
 * @param   msg - a pointer to the message
 *
 * @return  none
 */
static void RTLSMaster_processRtlsCtrlMsg(uint8_t *pMsg)
{
  rtlsCtrlReq_t *pReq;

  // Sanity check
  if (!pMsg)
  {
    return;
  }

  // Cast to appropriate struct
  pReq = (rtlsCtrlReq_t *)pMsg;

  if (pReq->reqOp <= RTLS_REQ_BLE_LOG_STRINGS_MAX)
  {
    BLE_LOG_INT_STR(0, BLE_LOG_MODULE_APP, "APP : RTLS msg status=%d, event=%s\n", 0, rtlsReq_BleLogStrings[pReq->reqOp]);
  }
  else
  {
    BLE_LOG_INT_INT(0, BLE_LOG_MODULE_APP, "APP : RTLS msg status=%d, event=0x%x\n", 0, pReq->reqOp);
  }

  switch(pReq->reqOp)
  {
    case RTLS_REQ_CONN:
    {
      RTLSMaster_processRTLSConnReq((bleConnReq_t *)pReq->pData);
    }
    break;

    case RTLS_REQ_SCAN:
    {
      RTLSMaster_processRTLSScanReq();
    }
    break;

    case RTLS_REQ_ENABLE_SYNC:
    {
      RTLSMaster_enableRtlsSync((rtlsEnableSync_t *)pReq->pData);
    }
    break;

    case RTLS_REQ_SEND_DATA:
    {
      RTLSMaster_sendRTLSData((rtlsPacket_t *)pReq->pData);
    }
    break;

    case RTLS_REQ_TERMINATE_LINK:
    {
      RTLSMaster_terminateLinkReq((rtlsTerminateLinkReq_t *)pReq->pData);
    }
    break;

    case RTLS_REQ_SET_AOA_PARAMS:
    {
      RTLSMaster_setAoaParamsReq((rtlsAoaConfigReq_t *)pReq->pData);
    }
    break;

    case RTLS_REQ_AOA_ENABLE:
    {
      RTLSMaster_enableAoaReq((rtlsAoaEnableReq_t *)pReq->pData);
    }
    break;

    case RTLS_REQ_UPDATE_CONN_INTERVAL:
    {
      RTLSMaster_processRTLSUpdateConnInterval((rtlsUpdateConnIntReq_t *)pReq->pData);
    }
    break;

    case RTLS_REQ_GET_ACTIVE_CONN_INFO:
    {
      rtlsGetActiveConnInfo_t *pConnInfoReq = (rtlsGetActiveConnInfo_t *)pReq->pData;

      if (pConnInfoReq)
      {
        RTLSMaster_processRTLSConnInfo(pConnInfoReq->connHandle);
      }
    }
    break;

    default:
      break;
  }

  // Free the payload
  if (pReq->pData)
  {
    ICall_free(pReq->pData);
  }
}

/*********************************************************************
 * @fn      RTLSMaster_processRtlsSrvMsg
 *
 * @brief   Handle processing messages from RTLS Services host module
 *
 * @param   pEvt - a pointer to the event
 *
 * @return  none
 */
static void RTLSMaster_processRtlsSrvMsg(rtlsSrv_evt_t *pEvt)
{
  if (!pEvt)
  {
    return;
  }

  BLE_LOG_INT_INT(0, BLE_LOG_MODULE_APP, "APP : RTLSsrv msg status=%d, event=0x%x\n", 0, pEvt->evtType);
  switch (pEvt->evtType)
  {
    case RTLSSRV_CONNECTION_CTE_IQ_REPORT_EVT:
    {
      rtlsSrv_connectionIQReport_t *pReport = (rtlsSrv_connectionIQReport_t *)pEvt->evtData;

      RTLSAoa_processAoaResults(pReport->connHandle,
                                pReport->rssi,
                                pReport->dataChIndex,
                                pReport->sampleCount,
                                pReport->sampleRate,
                                pReport->sampleSize,
                                pReport->sampleCtrl,
                                pReport->slotDuration,
                                pReport->numAnt,
                                pReport->iqSamples);
    }
    break;

    case RTLSSRV_ANTENNA_INFORMATION_EVT:
    {
      // This is for demonstration purposes - we could either use RTLSCtrl_sendDebugEvent
      // Or read the information by using a debugger
      // rtlsSrv_antennaInfo_t *pAntInfo = (rtlsSrv_antennaInfo_t)pEvt->evtData;
    }
    break;

    case RTLSSRV_CTE_REQUEST_FAILED_EVT:
    {
      rtlsSrv_cteReqFailed_t *pReqFail = (rtlsSrv_cteReqFailed_t *)pEvt->evtData;
      RTLSCtrl_sendDebugEvt("RTLS Services CTE Req Fail", (uint32_t)pReqFail->status);

    }
    break;

    case RTLSSRV_ERROR_EVT:
    {
      rtlsSrv_errorEvt_t *pError = (rtlsSrv_errorEvt_t *)pEvt->evtData;
      RTLSCtrl_sendDebugEvt("RTLS Services Error", (uint32_t)pError->errCause);
    }
    break;

    default:
      break;
  }

  // Free the payload
  if (pEvt->evtData)
  {
    ICall_free(pEvt->evtData);
  }
}


/*********************************************************************
 * @fn      RTLSMaster_rtlsCtrlMsgCb
 *
 * @brief   Callback given to RTLS Control
 *
 * @param  cmd - the command to be enqueued
 *
 * @return  none
 */
void RTLSMaster_rtlsCtrlMsgCb(uint8_t *cmd)
{
  // Enqueue the message to switch context
  RTLSMaster_enqueueMsg(RM_EVT_RTLS_CTRL_MSG_EVT, SUCCESS, (uint8_t *)cmd);
}

/*********************************************************************
 * @fn      RTLSMaster_rtlsSrvlMsgCb
 *
 * @brief   Callback given to RTLS Services
 *
 * @param   pRtlsSrvEvt - the command to be enqueued
 *
 * @return  none
 */
void RTLSMaster_rtlsSrvlMsgCb(rtlsSrv_evt_t *pRtlsSrvEvt)
{
  // Enqueue the message to switch context
  RTLSMaster_enqueueMsg(RM_EVT_RTLS_SRV_MSG_EVT, SUCCESS, (uint8_t *)pRtlsSrvEvt);
}

/*********************************************************************
*********************************************************************/
