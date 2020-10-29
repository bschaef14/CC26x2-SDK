/******************************************************************************

 @file  rtls_slave.c

 @brief This file contains the RTLS slave sample application for use
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

#include <ti/display/Display.h>

#if !(defined __TI_COMPILER_VERSION__)
#include <intrinsics.h>
#endif

#include <ti/drivers/utils/List.h>

#include <icall.h>
#include "util.h"
#include <bcomdef.h>
/* This Header file contains all BLE API and icall structure definition */
#include <icall_ble_api.h>

#ifdef USE_RCOSC
#include <rcosc_calibration.h>
#endif //USE_RCOSC

#include <ti_drivers_config.h>

#include "rtls_slave.h"

#include "rtls_ctrl_api.h"
#include "rtls_ble.h"

#include "ble_user_config.h"

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * CONSTANTS
 */

// Address mode of the local device
// Note: When using the DEFAULT_ADDRESS_MODE as ADDRMODE_RANDOM or 
// ADDRMODE_RP_WITH_RANDOM_ID, GAP_DeviceInit() should be called with 
// it's last parameter set to a static random address
#define DEFAULT_ADDRESS_MODE                  ADDRMODE_PUBLIC

// General discoverable mode: advertise indefinitely
#define DEFAULT_DISCOVERABLE_MODE             GAP_ADTYPE_FLAGS_GENERAL

// Minimum connection interval (units of 1.25ms, 80=100ms) for parameter update request
#define DEFAULT_DESIRED_MIN_CONN_INTERVAL     80

// Maximum connection interval (units of 1.25ms, 104=130ms) for  parameter update request
#define DEFAULT_DESIRED_MAX_CONN_INTERVAL     104

// Pass parameter updates to the app for it to decide.
#define DEFAULT_PARAM_UPDATE_REQ_DECISION     GAP_UPDATE_REQ_ACCEPT_ALL

// Task configuration
#define RS_TASK_PRIORITY                     1

#ifndef RS_TASK_STACK_SIZE
#define RS_TASK_STACK_SIZE                   1024
#endif

// Application events
#define RS_ADV_EVT                           0
#define RS_PAIR_STATE_EVT                    1
#define RS_PASSCODE_EVT                      2
#define RS_CONN_EVT                          3
#define RS_RTLS_CTRL_EVT                     4

// Internal Events for RTOS application
#define RS_ICALL_EVT                         ICALL_MSG_EVENT_ID // Event_Id_31
#define RS_QUEUE_EVT                         UTIL_QUEUE_EVENT_ID // Event_Id_30
#define RS_PERIODIC_EVT                      Event_Id_00

// Bitwise OR of all RTOS events to pend on
#define RS_ALL_EVENTS                        (RS_ICALL_EVT             | \
                                              RS_QUEUE_EVT             | \
                                              RS_PERIODIC_EVT)

// Spin if the expression is not true
#define RTLSSLAVE_ASSERT(expr) if (!(expr)) rtls_slave_spin();

// Set the register cause to the registration bit-mask
#define CONNECTION_EVENT_REGISTER_BIT_SET(RegisterCause) (connectionEventRegisterCauseBitMap |= RegisterCause )
// Remove the register cause from the registration bit-mask
#define CONNECTION_EVENT_REGISTER_BIT_REMOVE(RegisterCause) (connectionEventRegisterCauseBitMap &= (~RegisterCause) )
// Gets whether the current App is registered to the receive connection events
#define CONNECTION_EVENT_IS_REGISTERED (connectionEventRegisterCauseBitMap > 0)
// Gets whether the RegisterCause was registered to recieve connection event
#define CONNECTION_EVENT_REGISTRATION_CAUSE(RegisterCause) (connectionEventRegisterCauseBitMap & RegisterCause )

// Hard coded PSM for passing data between central and peripheral
#define RTLS_PSM      0x0080
#define RTLS_PDU_SIZE MAX_PDU_SIZE

/*********************************************************************
 * TYPEDEFS
 */

// App event passed from stack modules. This type is defined by the application
// since it can queue events to itself however it wants.
typedef struct
{
  uint8_t event;                // event type
  void    *pData;               // pointer to message
} rsEvt_t;

// Container to store passcode data when passing from gapbondmgr callback
// to app event. See the pfnPairStateCB_t documentation from the gapbondmgr.h
// header file for more information on each parameter.
typedef struct
{
  uint8_t state;
  uint16_t connHandle;
  uint8_t status;
} rsPairStateData_t;

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
} rsPasscodeData_t;

// Container to store advertising event data when passing from advertising
// callback to app event. See the respective event in GapAdvScan_Event_IDs
// in gap_advertiser.h for the type that pBuf should be cast to.
typedef struct
{
  uint32_t event;
  void *pBuf;
} rsGapAdvEventData_t;

/*********************************************************************
 * GLOBAL VARIABLES
 */

// Display Interface
Display_Handle dispHandle = NULL;

// Task configuration
Task_Struct rsTask;
#if defined __TI_COMPILER_VERSION__
#pragma DATA_ALIGN(rsTaskStack, 8)
#else
#pragma data_alignment=8
#endif
uint8_t rsTaskStack[RS_TASK_STACK_SIZE];

#define APP_EVT_EVENT_MAX  0x4
char *appEventStrings[] = {
  "APP_ADV_EVT         ",
  "APP_PAIR_STATE_EVT  ",
  "APP_PASSCODE_EVT    ",
  "APP_CONN_EVT        ",
  "APP_RTLS_CTRL_EVT   ",
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
static Queue_Struct appMsgQueue;
static Queue_Handle appMsgQueueHandle;

// Advertisement data
static uint8_t advertData[] =
{
  0x0A,							// Length of this data
  GAP_ADTYPE_LOCAL_NAME_SHORT,  // Type of this data
  'R',
  'T',
  'L',
  'S',
  'S',
  'l',
  'a',
  'v',
  'e',
  0x02,   // length of this data
  GAP_ADTYPE_FLAGS,
  DEFAULT_DISCOVERABLE_MODE | GAP_ADTYPE_FLAGS_BREDR_NOT_SUPPORTED,
};

// Scan Response Data
static uint8_t scanRspData[] =
{
  10,   						  // length of this data
  GAP_ADTYPE_LOCAL_NAME_COMPLETE, // Type of this data
  'R',
  'T',
  'L',
  'S',
  'S',
  'l',
  'a',
  'v',
  'e',

  // connection interval range
  5,   // length of this data
  GAP_ADTYPE_SLAVE_CONN_INTERVAL_RANGE,
  LO_UINT16(DEFAULT_DESIRED_MIN_CONN_INTERVAL),   // 100ms
  HI_UINT16(DEFAULT_DESIRED_MIN_CONN_INTERVAL),
  LO_UINT16(DEFAULT_DESIRED_MAX_CONN_INTERVAL),   // 1s
  HI_UINT16(DEFAULT_DESIRED_MAX_CONN_INTERVAL),

  // Tx power level
  2,   // length of this data
  GAP_ADTYPE_POWER_LEVEL,
  0       // 0dBm
};

// Advertising handles
static uint8 advHandleLegacy;
static uint8 advHandleLongRange;

// Address mode
static GAP_Addr_Modes_t addrMode = DEFAULT_ADDRESS_MODE;

/*********************************************************************
 * LOCAL FUNCTIONS
 */

static void RTLSSlave_init( void );
static void RTLSSlave_taskFxn(UArg a0, UArg a1);

static uint8_t RTLSSlave_processStackMsg(ICall_Hdr *pMsg);
static void RTLSSlave_processGapMessage(gapEventHdr_t *pMsg);
static void RTLSSlave_advCallback(uint32_t event, void *pBuf, uintptr_t arg);
static void RTLSSlave_processAdvEvent(rsGapAdvEventData_t *pEventData);
static void RTLSSlave_processAppMsg(rsEvt_t *pMsg);
static void RTLSSlave_passcodeCb(uint8_t *pDeviceAddr, uint16_t connHandle,
                                        uint8_t uiInputs, uint8_t uiOutputs,
                                        uint32_t numComparison);
static void RTLSSlave_pairStateCb(uint16_t connHandle, uint8_t state,
                                         uint8_t status);
static void RTLSSlave_processPairState(rsPairStateData_t *pPairState);
static void RTLSSlave_processPasscode(rsPasscodeData_t *pPasscodeData);
static status_t RTLSSlave_enqueueMsg(uint8_t event, void *pData);
static void RTLSSlave_connEvtCB(Gap_ConnEventRpt_t *pReport);
static void RTLSSlave_processConnEvt(Gap_ConnEventRpt_t *pReport);
static void RTLSSlave_openL2CAPChanCoc(void);
static void RTLSSlave_processL2CAPSignalEvent(l2capSignalEvent_t *pMsg);
static uint8_t RTLSSlave_processL2CAPDataEvent(l2capDataEvent_t *pMsg);
static void RTLSSlave_processRtlsMsg(uint8_t *pMsg);
static void RTLSSlave_enableRtlsSync(rtlsEnableSync_t *enable);

/*********************************************************************
 * EXTERN FUNCTIONS
 */
extern void AssertHandler(uint8 assertCause, uint8 assertSubcause);

/*********************************************************************
 * PROFILE CALLBACKS
 */

// GAP Bond Manager Callbacks
static gapBondCBs_t RTLSSlave_BondMgrCBs =
{
  RTLSSlave_passcodeCb,       // Passcode callback
  RTLSSlave_pairStateCb       // Pairing/Bonding state Callback
};

/*********************************************************************
 * PUBLIC FUNCTIONS
 */

/*********************************************************************
 * The following typedef and global handle the registration to connection event
 */
typedef enum
{
   NOT_REGISTERED     = 0x0,
   FOR_RTLS            = 0x2,
} connectionEventRegisterCause_u;

// Handle the registration and un-registration for the connection event, since only one can be registered.
uint32_t  connectionEventRegisterCauseBitMap = NOT_REGISTERED;

/*********************************************************************
 * @fn      rtls_slave_spin
 *
 * @brief   Spin forever
 *
 * @param   none
 */
static void rtls_slave_spin(void)
{
  volatile uint8_t x = 0;

  while(1)
  {
    x++;
  }
}

/*********************************************************************
 * @fn      RTLSSlave_createTask
 *
 * @brief   Task creation function for the RTLS Slave.
 */
void RTLSSlave_createTask(void)
{
  Task_Params taskParams;

  // Configure task
  Task_Params_init(&taskParams);
  taskParams.stack = rsTaskStack;
  taskParams.stackSize = RS_TASK_STACK_SIZE;
  taskParams.priority = RS_TASK_PRIORITY;

  Task_construct(&rsTask, RTLSSlave_taskFxn, &taskParams, NULL);
}

/*********************************************************************
 * @fn      RTLSSlave_init
 *
 * @brief   Called during initialization and contains application
 *          specific initialization (ie. hardware initialization/setup,
 *          table initialization, power up notification, etc), and
 *          profile initialization/setup.
 */
static void RTLSSlave_init(void)
{
  BLE_LOG_INT_TIME(0, BLE_LOG_MODULE_APP, "APP : ---- init ", RS_TASK_PRIORITY);
  // ******************************************************************
  // N0 STACK API CALLS CAN OCCUR BEFORE THIS CALL TO ICall_registerApp
  // ******************************************************************
  // Register the current thread as an ICall dispatcher application
  // so that the application can send and receive messages.
  ICall_registerApp(&selfEntity, &syncEvent);

#ifdef USE_RCOSC
  // Set device's Sleep Clock Accuracy
#if ( HOST_CONFIG & ( CENTRAL_CFG | PERIPHERAL_CFG ) )
  HCI_EXT_SetSCACmd(500);
#endif // (CENTRAL_CFG | PERIPHERAL_CFG)
  RCOSC_enableCalibration();
#endif // USE_RCOSC

  // Create an RTOS queue for message from profile to be sent to app.
  appMsgQueueHandle = Util_constructQueue(&appMsgQueue);

  // Set the Device Name characteristic in the GAP GATT Service
  // For more information, see the section in the User's Guide:
  // http://software-dl.ti.com/lprf/ble5stack-latest/
  //GGS_SetParameter(GGS_DEVICE_NAME_ATT, GAP_DEVICE_NAME_LEN, attDeviceName);

  // Configure GAP
  {
    uint16_t paramUpdateDecision = DEFAULT_PARAM_UPDATE_REQ_DECISION;

    // Pass all parameter update requests to the app for it to decide
    GAP_SetParamValue(GAP_PARAM_LINK_UPDATE_DECISION, paramUpdateDecision);
  }

  // Setup the GAP Bond Manager. For more information see the GAP Bond Manager
  // section in the User's Guide:
  // http://software-dl.ti.com/lprf/ble5stack-latest/
  {
    // Don't send a pairing request after connecting; the peer device must
    // initiate pairing
    uint8_t pairMode = GAPBOND_PAIRING_MODE_WAIT_FOR_REQ;
    // Use authenticated pairing: require passcode.
    uint8_t mitm = TRUE;
    // This device only has display capabilities. Therefore, it will display the
    // passcode during pairing. However, since the default passcode is being
    // used, there is no need to display anything.
    uint8_t ioCap = GAPBOND_IO_CAP_DISPLAY_ONLY;
    // Request bonding (storing long-term keys for re-encryption upon subsequent
    // connections without repairing)
    uint8_t bonding = TRUE;

#ifdef SC_HOST_DEBUG
    //Enable using pre-defined ECC Debug Keys
    uint8_t useDebugEccKeys = TRUE;

    GAPBondMgr_SetParameter(GAPBOND_SC_HOST_DEBUG, sizeof(uint8_t), &useDebugEccKeys);
#endif

    GAPBondMgr_SetParameter(GAPBOND_PAIRING_MODE, sizeof(uint8_t), &pairMode);
    GAPBondMgr_SetParameter(GAPBOND_MITM_PROTECTION, sizeof(uint8_t), &mitm);
    GAPBondMgr_SetParameter(GAPBOND_IO_CAPABILITIES, sizeof(uint8_t), &ioCap);
    GAPBondMgr_SetParameter(GAPBOND_BONDING_ENABLED, sizeof(uint8_t), &bonding);
  }

  // Start Bond Manager and register callback
  VOID GAPBondMgr_Register(&RTLSSlave_BondMgrCBs);

  // Register with GAP for HCI/Host messages. This is needed to receive HCI
  // events. For more information, see the HCI section in the User's Guide:
  // http://software-dl.ti.com/lprf/ble5stack-latest/
  GAP_RegisterForMsgs(selfEntity);

  // Set default values for Data Length Extension
  // Extended Data Length Feature is already enabled by default
  {
    // Set initial values to maximum, RX is set to max. by default(251 octets, 2120us)
    // Some brand smartphone is essentially needing 251/2120, so we set them here.
    #define APP_SUGGESTED_PDU_SIZE 251 //default is 27 octets(TX)
    #define APP_SUGGESTED_TX_TIME 2120 //default is 328us(TX)

    // This API is documented in hci.h
    // See the LE Data Length Extension section in the BLE5-Stack User's Guide for information on using this command:
    // http://software-dl.ti.com/lprf/ble5stack-latest/
    HCI_LE_WriteSuggestedDefaultDataLenCmd(APP_SUGGESTED_PDU_SIZE, APP_SUGGESTED_TX_TIME);
  }

  BLE_LOG_INT_TIME(0, BLE_LOG_MODULE_APP, "APP : ---- call GAP_DeviceInit", GAP_PROFILE_PERIPHERAL);
  //Initialize GAP layer for Peripheral role and register to receive GAP events
  GAP_DeviceInit(GAP_PROFILE_PERIPHERAL, selfEntity, addrMode, NULL);

  // The type of display is configured based on the BOARD_DISPLAY_USE...
  // preprocessor definitions
  dispHandle = Display_open(Display_Type_ANY, NULL);

  //Read the LE locally supported features
  HCI_LE_ReadLocalSupportedFeaturesCmd();

  // Initialize RTLS Services
  RTLSSrv_init(MAX_NUM_BLE_CONNS);
}

/*********************************************************************
 * @fn      RTLSSlave_taskFxn
 *
 * @brief   Application task entry point for the RTLS Slave.
 *
 * @param   a0, a1 - not used.
 */
static void RTLSSlave_taskFxn(UArg a0, UArg a1)
{
  // Initialize application
  RTLSSlave_init();

  // Application main loop
  for (;;)
  {
    uint32_t events;

    // Waits for an event to be posted associated with the calling thread.
    // Note that an event associated with a thread is posted when a
    // message is queued to the message receive queue of the thread
    events = Event_pend(syncEvent, Event_Id_NONE, RS_ALL_EVENTS,
                        ICALL_TIMEOUT_FOREVER);

    if (events)
    {
      ICall_EntityID dest;
      ICall_ServiceEnum src;
      ICall_HciExtEvt *pMsg = NULL;

      // Fetch any available messages that might have been sent from the stack
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
            safeToDealloc = RTLSSlave_processStackMsg((ICall_Hdr *)pMsg);
          }
        }

        if (pMsg && safeToDealloc)
        {
          ICall_freeMsg(pMsg);
        }
      }

      // If RTOS queue is not empty, process app message.
      if (events & RS_QUEUE_EVT)
      {
        while (!Queue_empty(appMsgQueueHandle))
        {
          rsEvt_t *pMsg = (rsEvt_t *)Util_dequeueMsg(appMsgQueueHandle);
          if (pMsg)
          {
            // Process message.
            RTLSSlave_processAppMsg(pMsg);

            // Free the space from the message.
            ICall_free(pMsg);
          }
        }
      }
    }
  }
}

/*********************************************************************
 * @fn      RTLSSlave_processStackMsg
 *
 * @brief   Process an incoming stack message.
 *
 * @param   pMsg - message to process
 *
 * @return  TRUE if safe to deallocate incoming message, FALSE otherwise.
 */
static uint8_t RTLSSlave_processStackMsg(ICall_Hdr *pMsg)
{
  // Always dealloc pMsg unless set otherwise
  uint8_t safeToDealloc = TRUE;

  BLE_LOG_INT_INT(0, BLE_LOG_MODULE_APP, "APP : Stack msg status=%d, event=0x%x\n", pMsg->status, pMsg->event);

  switch (pMsg->event)
  {
    case GAP_MSG_EVENT:
      RTLSSlave_processGapMessage((gapEventHdr_t*) pMsg);
    break;

    case L2CAP_SIGNAL_EVENT:
     RTLSSlave_processL2CAPSignalEvent((l2capSignalEvent_t *)pMsg);
    break;

    case L2CAP_DATA_EVENT:
      safeToDealloc = RTLSSlave_processL2CAPDataEvent((l2capDataEvent_t *)pMsg);
    break;

    case HCI_GAP_EVENT_EVENT:
    {
      // Process HCI message
      switch(pMsg->status)
      {
        case HCI_COMMAND_COMPLETE_EVENT_CODE:
        // Process HCI Command Complete Events here
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
          AssertHandler(HAL_ASSERT_CAUSE_HARDWARE_ERROR,0);
        break;

        default:
          break;
      }
    }

    default:
      // do nothing
      break;
  }

  return (safeToDealloc);
}

/*********************************************************************
 * @fn      RTLSSlave_processAppMsg
 *
 * @brief   Process an incoming callback from a profile.
 *
 * @param   pMsg - message to process
 *
 * @return  None.
 */
static void RTLSSlave_processAppMsg(rsEvt_t *pMsg)
{
  if (pMsg->event <= APP_EVT_EVENT_MAX)
  {
    BLE_LOG_INT_STR(0, BLE_LOG_MODULE_APP, "APP : App msg status=%d, event=%s\n", 0, appEventStrings[pMsg->event]);
  }
  else
  {
    BLE_LOG_INT_INT(0, BLE_LOG_MODULE_APP, "APP : App msg status=%d, event=0x%x\n", 0, pMsg->event);
  }

  switch (pMsg->event)
  {
    case RS_ADV_EVT:
      RTLSSlave_processAdvEvent((rsGapAdvEventData_t*)(pMsg->pData));
      break;

    case RS_PAIR_STATE_EVT:
      RTLSSlave_processPairState((rsPairStateData_t*)(pMsg->pData));
      break;

    case RS_PASSCODE_EVT:
      RTLSSlave_processPasscode((rsPasscodeData_t*)(pMsg->pData));
      break;

    case RS_CONN_EVT:
      RTLSSlave_processConnEvt((Gap_ConnEventRpt_t *)(pMsg->pData));
      break;

    case RS_RTLS_CTRL_EVT:
      RTLSSlave_processRtlsMsg((uint8_t *)pMsg->pData);
      break;

    default:
      // Do nothing.
      break;
  }

  // Free message data if it exists and we are to dealloc
  if (pMsg->pData != NULL)
  {
    ICall_free(pMsg->pData);
  }
}

/*********************************************************************
 * @fn      RTLSSlave_processGapMessage
 *
 * @brief   Process an incoming GAP event.
 *
 * @param   pMsg - message to process
 */
static void RTLSSlave_processGapMessage(gapEventHdr_t *pMsg)
{
  switch(pMsg->opcode)
  {
    case GAP_DEVICE_INIT_DONE_EVENT:
    {
      bStatus_t status = FAILURE;

      gapDeviceInitDoneEvent_t *pPkt = (gapDeviceInitDoneEvent_t *)pMsg;

      if(pPkt->hdr.status == SUCCESS)
      {
        Display_printf(dispHandle, 2, 0, "Initialized");

        BLE_LOG_INT_TIME(0, BLE_LOG_MODULE_APP, "APP : ---- got GAP_DEVICE_INIT_DONE_EVENT", 0);
        // Setup and start Advertising
        // For more information, see the GAP section in the User's Guide:
        // http://software-dl.ti.com/lprf/ble5stack-latest/

        // Temporary memory for advertising parameters for set #1. These will be copied
        // by the GapAdv module
        GapAdv_params_t advParamLegacy = GAPADV_PARAMS_LEGACY_SCANN_CONN;

        BLE_LOG_INT_INT(0, BLE_LOG_MODULE_APP, "APP : ---- call GapAdv_create set=%d,%d\n", 0, 0);
        // Create Advertisement set #1 and assign handle
        status = GapAdv_create(&RTLSSlave_advCallback, &advParamLegacy,
                               &advHandleLegacy);
        RTLSSLAVE_ASSERT(status == SUCCESS);

        // Load advertising data for set #1 that is statically allocated by the app
        status = GapAdv_loadByHandle(advHandleLegacy, GAP_ADV_DATA_TYPE_ADV,
                                     sizeof(advertData), advertData);
        RTLSSLAVE_ASSERT(status == SUCCESS);

        // Load scan response data for set #1 that is statically allocated by the app
        status = GapAdv_loadByHandle(advHandleLegacy, GAP_ADV_DATA_TYPE_SCAN_RSP,
                                     sizeof(scanRspData), scanRspData);
        RTLSSLAVE_ASSERT(status == SUCCESS);

        // Set event mask for set #1
        status = GapAdv_setEventMask(advHandleLegacy,
                                     GAP_ADV_EVT_MASK_START_AFTER_ENABLE |
                                     GAP_ADV_EVT_MASK_END_AFTER_DISABLE |
                                     GAP_ADV_EVT_MASK_SET_TERMINATED);

        // Enable legacy advertising for set #1
        status = GapAdv_enable(advHandleLegacy, GAP_ADV_ENABLE_OPTIONS_USE_MAX , 0);
        RTLSSLAVE_ASSERT(status == SUCCESS);

        // Use long range params to create long range set #2
        GapAdv_params_t advParamLongRange = GAPADV_PARAMS_AE_LONG_RANGE_CONN;

        BLE_LOG_INT_INT(0, BLE_LOG_MODULE_APP, "APP : ---- call GapAdv_create set=%d,%d\n", 1, 0);
        // Create Advertisement set #2 and assign handle
        status = GapAdv_create(&RTLSSlave_advCallback, &advParamLongRange,
                               &advHandleLongRange);
        RTLSSLAVE_ASSERT(status == SUCCESS);

        // Load advertising data for set #2 that is statically allocated by the app
        status = GapAdv_loadByHandle(advHandleLongRange, GAP_ADV_DATA_TYPE_ADV,
                                     sizeof(advertData), advertData);
        RTLSSLAVE_ASSERT(status == SUCCESS);

        // Set event mask for set #2
        status = GapAdv_setEventMask(advHandleLongRange,
                                     GAP_ADV_EVT_MASK_START_AFTER_ENABLE |
                                     GAP_ADV_EVT_MASK_END_AFTER_DISABLE |
                                     GAP_ADV_EVT_MASK_SET_TERMINATED);

        BLE_LOG_INT_TIME(0, BLE_LOG_MODULE_APP, "APP : ---- GapAdv_enable", 0);
        // Enable long range advertising for set #2
        status = GapAdv_enable(advHandleLongRange, GAP_ADV_ENABLE_OPTIONS_USE_MAX , 0);
        RTLSSLAVE_ASSERT(status == SUCCESS);

        // Display device address
        Display_printf(dispHandle, 1, 0, "%s Addr: %s",
                       (addrMode <= ADDRMODE_RANDOM) ? "Dev" : "ID",
                       Util_convertBdAddr2Str(pPkt->devAddr));
      }

      break;
    }

    case GAP_LINK_ESTABLISHED_EVENT:
    {
      gapEstLinkReqEvent_t *pPkt = (gapEstLinkReqEvent_t *)pMsg;

      BLE_LOG_INT_TIME(0, BLE_LOG_MODULE_APP, "APP : ---- got GAP_LINK_ESTABLISHED_EVENT", 0);
      // Display the amount of current connections
      uint8_t numActive = linkDB_NumActive();
      Display_printf(dispHandle, 2, 0, "Num Conns: %d",
                      (uint16_t)numActive);

      if (pPkt->hdr.status == SUCCESS)
      {
        RTLSSlave_openL2CAPChanCoc();

        // Once connection is established we are ready to receive CTE requests
        RTLSSrv_setConnCteTransmitParams(pPkt->connectionHandle, RTLSSRV_CTE_TYPE_AOA, 0, NULL);
        RTLSSrv_setConnCteResponseEnableCmd(pPkt->connectionHandle, TRUE);

        // Notify RTLS Control that we are connected
        RTLSCtrl_connResultEvt(NULL, RTLS_SUCCESS);

        // Display the address of this connection
        Display_printf(dispHandle, 2, 0, "Connected to %s",
                        Util_convertBdAddr2Str(pPkt->devAddr));
      }
      else
      {
        // Notify RTLS Control that we are not connected
         RTLSCtrl_connResultEvt(0, RTLS_FAIL);
      }

      if ((numActive < MAX_NUM_BLE_CONNS))
      {
        // Start advertising since there is room for more connections
        GapAdv_enable(advHandleLegacy, GAP_ADV_ENABLE_OPTIONS_USE_MAX , 0);
        GapAdv_enable(advHandleLongRange, GAP_ADV_ENABLE_OPTIONS_USE_MAX , 0);
      }
      else
      {
        // Stop advertising since there is no room for more connections
        GapAdv_disable(advHandleLongRange);
        GapAdv_disable(advHandleLegacy);
      }
      break;
    }

    case GAP_LINK_TERMINATED_EVENT:
    {
      Display_printf(dispHandle, 2, 0, "Disconnected");

      BLE_LOG_INT_STR(0, BLE_LOG_MODULE_APP, "APP : GAP msg: status=%d, opcode=%s\n", 0, "GAP_LINK_TERMINATED_EVENT");
      // Start advertising since there is room for more connections
      GapAdv_enable(advHandleLegacy, GAP_ADV_ENABLE_OPTIONS_USE_MAX , 0);
      GapAdv_enable(advHandleLongRange, GAP_ADV_ENABLE_OPTIONS_USE_MAX , 0);

      RTLSCtrl_connResultEvt(0, RTLS_LINK_TERMINATED);
    }
    break;

    default:
      break;
  }
}

/*********************************************************************
 * @fn      RTLSSlave_advCallback
 *
 * @brief   GapAdv module callback
 *
 * @param   pMsg - message to process
 */
static void RTLSSlave_advCallback(uint32_t event, void *pBuf, uintptr_t arg)
{
  rsGapAdvEventData_t *pData = ICall_malloc(sizeof(rsGapAdvEventData_t));

  if (pData)
  {
    pData->event = event;
    pData->pBuf = pBuf;

    if(RTLSSlave_enqueueMsg(RS_ADV_EVT, pData) != SUCCESS)
    {
      ICall_free(pData);
    }
  }
}

/*********************************************************************
 * @fn      RTLSSlave_processAdvEvent
 *
 * @brief   Process advertising event in app context
 *
 * @param   pEventData
 */
static void RTLSSlave_processAdvEvent(rsGapAdvEventData_t *pEventData)
{
  switch (pEventData->event)
  {
    case GAP_EVT_ADV_START_AFTER_ENABLE:
      BLE_LOG_INT_TIME(0, BLE_LOG_MODULE_APP, "APP : ---- GAP_EVT_ADV_START_AFTER_ENABLE", 0);
      Display_printf(dispHandle, 2, 0, "Advertising");
      break;

    case GAP_EVT_ADV_START:
      Display_printf(dispHandle, 2, 0, "Advertising");
      break;

    case GAP_EVT_INSUFFICIENT_MEMORY:
      Display_printf(dispHandle, 2, 0, "Insufficient Memory");
      break;

    default:
      break;
  }

  // All events have associated memory to free except the insufficient memory
  // event
  if (pEventData->event != GAP_EVT_INSUFFICIENT_MEMORY)
  {
    ICall_free(pEventData->pBuf);
  }
}


/*********************************************************************
 * @fn      RTLSSlave_pairStateCb
 *
 * @brief   Pairing state callback.
 *
 * @return  none
 */
static void RTLSSlave_pairStateCb(uint16_t connHandle, uint8_t state,
                                         uint8_t status)
{
  rsPairStateData_t *pData = ICall_malloc(sizeof(rsPairStateData_t));

  // Allocate space for the event data.
  if (pData)
  {
    pData->state = state;
    pData->connHandle = connHandle;
    pData->status = status;

    // Queue the event.
    if(RTLSSlave_enqueueMsg(RS_PAIR_STATE_EVT, pData) != SUCCESS)
    {
      ICall_free(pData);
    }
  }
}

/*********************************************************************
 * @fn      RTLSSlave_passcodeCb
 *
 * @brief   Passcode callback.
 *
 * @return  none
 */
static void RTLSSlave_passcodeCb(uint8_t *pDeviceAddr,
                                        uint16_t connHandle,
                                        uint8_t uiInputs,
                                        uint8_t uiOutputs,
                                        uint32_t numComparison)
{
  rsPasscodeData_t *pData = ICall_malloc(sizeof(rsPasscodeData_t));

  // Allocate space for the passcode event.
  if (pData )
  {
    pData->connHandle = connHandle;
    memcpy(pData->deviceAddr, pDeviceAddr, B_ADDR_LEN);
    pData->uiInputs = uiInputs;
    pData->uiOutputs = uiOutputs;
    pData->numComparison = numComparison;

    // Enqueue the event.
    if(RTLSSlave_enqueueMsg(RS_PASSCODE_EVT, pData) != SUCCESS)
    {
      ICall_free(pData);
    }
  }
}

/*********************************************************************
 * @fn      RTLSSlave_processPairState
 *
 * @brief   Process the new paring state.
 *
 * @return  none
 */
static void RTLSSlave_processPairState(rsPairStateData_t *pPairData)
{
  uint8_t state = pPairData->state;
  uint8_t status = pPairData->status;

  switch (state)
  {
    case GAPBOND_PAIRING_STATE_STARTED:
    {
      Display_printf(dispHandle, 2, 0, "Pairing started");
    }
    break;

    case GAPBOND_PAIRING_STATE_COMPLETE:
      if (status == SUCCESS)
      {
        Display_printf(dispHandle, 2, 0, "Pairing started");
      }
      else
      {
        Display_printf(dispHandle, 2, 0, "Pairing fail: %d", status);
      }
      break;

    case GAPBOND_PAIRING_STATE_BOND_SAVED:
      if (status == SUCCESS)
      {
        Display_printf(dispHandle, 2, 0, "Bond save success");
      }
      else
      {
        Display_printf(dispHandle, 2, 0, "Bond save failed: %d", status);
      }
      break;

    default:
      break;
  }
}

/*********************************************************************
 * @fn      RTLSSlave_processPasscode
 *
 * @brief   Process the Passcode request.
 *
 * @return  none
 */
static void RTLSSlave_processPasscode(rsPasscodeData_t *pPasscodeData)
{
  // This app uses a default passcode. A real-life scenario would handle all
  // pairing scenarios and likely generate this randomly.
  uint32_t passcode = B_APP_DEFAULT_PASSCODE;

  // Display passcode to user
  if (pPasscodeData->uiOutputs != 0)
  {
	  Display_printf(dispHandle, 4, 0, "Passcode: %d", passcode);
  }

  // Send passcode response
  GAPBondMgr_PasscodeRsp(pPasscodeData->connHandle , SUCCESS,
                          passcode);
}

/*********************************************************************
 * @fn      RTLSSlave_connEvtCB
 *
 * @brief   Connection event callback.
 *
 * @param pReport pointer to connection event report
 */
static void RTLSSlave_connEvtCB(Gap_ConnEventRpt_t *pReport)
{
  // Enqueue the event for processing in the app context.
  if(RTLSSlave_enqueueMsg(RS_CONN_EVT, pReport) != SUCCESS)
  {
    ICall_free(pReport);
  }
}

/*********************************************************************
 * @fn      RTLSSlave_processConnEvt
 *
 * @brief   Process connection event.
 *
 * @param pReport pointer to connection event report
 */
static void RTLSSlave_processConnEvt(Gap_ConnEventRpt_t *pReport)
{
  // Do a TOF Run, at the end of the active connection period
  if (CONNECTION_EVENT_REGISTRATION_CAUSE(FOR_RTLS))
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

	RTLSCtrl_syncNotifyEvt(pReport->handle, status, pReport->nextTaskTime, pReport->lastRssi, pReport->channel);
  }
}


/*********************************************************************
 * @fn      RTLSSlave_enqueueMsg
 *
 * @brief   Creates a message and puts the message in RTOS queue.
 *
 * @param   event - message event.
 * @param   state - message state.
 */
static status_t RTLSSlave_enqueueMsg(uint8_t event, void *pData)
{
  uint8_t success;
  rsEvt_t *pMsg = ICall_malloc(sizeof(rsEvt_t));

  // Create dynamic pointer to message.
  if(pMsg)
  {
    pMsg->event = event;
    pMsg->pData = pData;

    // Enqueue the message.
    success = Util_enqueueMsg(appMsgQueueHandle, syncEvent, (uint8_t *)pMsg);
    return (success) ? SUCCESS : FAILURE;
  }

  return(bleMemAllocError);
}

/*********************************************************************
 * @fn      RTLSSlave_openL2CAPChanCoc
 *
 * @brief   Opens a communication channel between RTLS Master/Slave
 *
 * @param   pairState - Verify that devices are paired
 *
 * @return  none
 */
static void RTLSSlave_openL2CAPChanCoc(void)
{
  l2capPsm_t psm;
  l2capPsmInfo_t psmInfo;

  if (L2CAP_PsmInfo(RTLS_PSM, &psmInfo) == INVALIDPARAMETER)
  {
    // Prepare the PSM parameters
    psm.initPeerCredits = 0xFFFF;
    psm.maxNumChannels = 1;
    psm.mtu = RTLS_PDU_SIZE;
    psm.peerCreditThreshold = 0;
    psm.pfnVerifySecCB = NULL;
    psm.psm = RTLS_PSM;
    psm.taskId = ICall_getLocalMsgEntityId(ICALL_SERVICE_CLASS_BLE_MSG, selfEntity);

    // Register PSM with L2CAP task
    L2CAP_RegisterPsm(&psm);
  }
  else
  {
    Display_printf(dispHandle, 4, 0, "Could not open L2CAP channel");
  }
}

/*********************************************************************
 * @fn      RTLSSlave_processL2CAPSignalEvent
 *
 * @brief   Handle L2CAP signal events
 *
 * @param   pMsg - pointer to the signal that was received
 *
 * @return  none
 */
static void RTLSSlave_processL2CAPSignalEvent(l2capSignalEvent_t *pMsg)
{
  switch (pMsg->opcode)
  {
    case L2CAP_CHANNEL_ESTABLISHED_EVT:
    {
      l2capChannelEstEvt_t *pEstEvt = &(pMsg->cmd.channelEstEvt);

      // Give max credits to the other side
      L2CAP_FlowCtrlCredit(pEstEvt->CID, 0xFFFF);

      Display_printf(dispHandle, 4, 0, "L2CAP Channel Open");
    }
    break;

    case L2CAP_SEND_SDU_DONE_EVT:
    {
      if (pMsg->hdr.status == SUCCESS)
      {
        RTLSCtrl_dataSentEvt(0, RTLS_SUCCESS);
      }
      else
      {
        RTLSCtrl_dataSentEvt(0, RTLS_FAIL);
      }
    }
    break;
  }
}

/*********************************************************************
 * @fn      RTLSSlave_processL2CAPDataEvent
 *
 * design /ref 159098678
 * @brief   Handles incoming L2CAP data
 *
 * @param   pMsg - pointer to the signal that was received
 *
 * @return  the return value determines whether pMsg can be freed or not
 */
static uint8_t RTLSSlave_processL2CAPDataEvent(l2capDataEvent_t *pMsg)
{
  rtlsPacket_t *pRtlsPkt;
  static uint16_t packetCounter;

  if (!pMsg)
  {
    // Caller needs to figure out by himself that pMsg is NULL
    return TRUE;
  }

  // This application doesn't care about other L2CAP data events other than RTLS
  // It is possible to expand this function to support multiple COC CID's
  pRtlsPkt = (rtlsPacket_t *)ICall_malloc(pMsg->pkt.len);

  // Check for malloc error
  if (!pRtlsPkt)
  {
    // Free the payload (must use BM_free here according to L2CAP documentation)
    BM_free(pMsg->pkt.pPayload);
    return TRUE;
  }

  // Copy the payload
  memcpy(pRtlsPkt, pMsg->pkt.pPayload, pMsg->pkt.len);

  Display_printf(dispHandle, 10, 0, "RTLS Packet Received, cmdId %d", pRtlsPkt->cmdOp);
  Display_printf(dispHandle, 11, 0, "Packet Len: %d", pMsg->pkt.len);
  Display_printf(dispHandle, 12, 0, "Number of packets received: %d", ++packetCounter);

  // Free the payload (must use BM_free here according to L2CAP documentation)
  BM_free(pMsg->pkt.pPayload);

  // RTLS Control will handle the information in the packet
  RTLSCtrl_rtlsPacketEvt((uint8_t *)pRtlsPkt);

  return TRUE;
}

/*********************************************************************
 * @fn      RTLSSlave_enableRtlsSync
 *
 * @brief   This function is used by RTLS Control to notify the RTLS application
 *          to start sending synchronization events (for BLE this is a connection event)
 *
 * @param   enable - start/stop synchronization
 *
 * @return  none
 */
static void RTLSSlave_enableRtlsSync(rtlsEnableSync_t *enable)
{
  bStatus_t status = RTLS_FALSE;

  if (enable->enable == RTLS_TRUE)
  {
    if (!CONNECTION_EVENT_IS_REGISTERED)
    {
      status = Gap_RegisterConnEventCb(RTLSSlave_connEvtCB, GAP_CB_REGISTER, LINKDB_CONNHANDLE_ALL);
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
      Gap_RegisterConnEventCb(RTLSSlave_connEvtCB, GAP_CB_UNREGISTER, LINKDB_CONNHANDLE_ALL);
    }
  }
}

/*********************************************************************
 * @fn      RTLSSlave_processRtlsMsg
 *
 * @brief   Handle processing messages from RTLS Control
 *
 * @param   msg - a pointer to the message
 *
 * @return  none
 */
static void RTLSSlave_processRtlsMsg(uint8_t *pMsg)
{
  rtlsCtrlReq_t *req = (rtlsCtrlReq_t *)pMsg;

  switch(req->reqOp)
  {
    case RTLS_REQ_ENABLE_SYNC:
    {
      RTLSSlave_enableRtlsSync((rtlsEnableSync_t *)req->pData);
    }
    break;

    default:
      break;
  }

  if (req->pData != NULL)
  {
    ICall_free(req->pData);
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
void RTLSSlave_rtlsCtrlMsgCb(uint8_t *cmd)
{
  // Enqueue the message to switch context
  if(RTLSSlave_enqueueMsg(RS_RTLS_CTRL_EVT, (uint8_t *)cmd) != SUCCESS)
  {
    ICall_free(cmd);
  }
}

/*********************************************************************
*********************************************************************/
