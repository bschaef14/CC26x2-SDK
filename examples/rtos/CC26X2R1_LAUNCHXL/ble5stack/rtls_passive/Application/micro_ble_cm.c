/*********************************************************************
 * INCLUDES
 */
#include <string.h>
#include <stdlib.h>

#include <ti/display/Display.h>
#include <ti/sysbios/hal/Hwi.h>
#include <ti/sysbios/knl/Swi.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Event.h>
#include <ti/sysbios/knl/Queue.h>
#include <ti/sysbios/BIOS.h>
#include "bcomdef.h"

#include <ti_drivers_config.h>
#include "board_key.h"

#include "ugap.h"
#include "urfc.h"

#include "util.h"

#include "micro_ble_cm.h"
#include "rtls_ctrl_api.h"

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * CONSTANTS
 */
#define BLE_RAT_IN_16US              64   // Connection Jitter
#define BLE_RAT_IN_64US              256  // Radio Rx Settle Time
#define BLE_RAT_IN_100US             400  // 1M / 2500 RAT ticks (SCA PPM)
#define BLE_RAT_IN_140US             560  // Rx Back-end Time
#define BLE_RAT_IN_150US             600  // T_IFS
#define BLE_RAT_IN_256US             1024 // Radio Overhead + FS Calibration
#define BLE_RAT_IN_1515US            6063 // Two thrid of the maximum packet size
#define BLE_RAT_IN_2021US            8084 // Maximum packet size

#define BLE_SYNTH_CALIBRATION        (BLE_RAT_IN_256US)
#define BLE_RX_SETTLE_TIME           (BLE_RAT_IN_64US)
#define BLE_RX_RAMP_OVERHEAD         (BLE_SYNTH_CALIBRATION + \
                                      BLE_RX_SETTLE_TIME)
#define BLE_RX_SYNCH_OVERHEAD        (BLE_RAT_IN_140US)
#define BLE_JITTER_CORRECTION        (BLE_RAT_IN_16US)

#define BLE_OVERLAP_TIME             (BLE_RAT_IN_150US)
#define BLE_EVENT_PAD_TIME           (BLE_RX_RAMP_OVERHEAD +  \
                                      BLE_JITTER_CORRECTION + \
                                      BLE_RAT_IN_100US)

#define BLE_HOP_VALUE_MAX            16
#define BLE_HOP_VALUE_MIN            5
#define BLE_COMB_SCA_MAX             540
#define BLE_COMB_SCA_MIN             40

// The most significant nibble for possible "nextStartTime" wrap around.
#define BLE_MS_NIBBLE                0xF0000000

// Threshold for consecutive missed packets in monitor session before terminated link
#define BLE_CONSECUTIVE_MISSED_CONN_EVT_THRES       30

// Bus latency - used to calculate number of connection events to skip
#define BUS_LATENCY_IN_MS            320

/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * GLOBAL VARIABLES
 */

/*********************************************************************
 * EXTERNAL VARIABLES
 */
Display_Handle dispHandle;
extern uint8 tbmRowItemLast;

/*********************************************************************
 * EXTERNAL FUNCTIONS
 */

/*********************************************************************
 * LOCAL VARIABLES
 */
ubCM_GetConnInfoComplete_t ubCMConnInfo;

uint16_t lastSessionMask = 0x00; //! Mask to track which sessions have recently been watched
uint16_t sessionMask   = 0x00;   //! Mask to track which sessions are being watched

uint8_t cmMaster = TRUE; //! Flag to identify a packet from master

pfnRtlsPassiveCb gAppCb = NULL;

/*********************************************************************
 * LOCAL FUNCTIONS
 */

uint8_t ubCM_callApp(uint8_t eventId, uint8_t *data);
static uint8_t setNextDataChan( uint8_t sessionId, uint16_t numEvents );
static void monitor_stateChangeCB(ugapMonitorState_t newState);
static void monitor_indicationCB(bStatus_t status, uint8_t sessionId,
                                 uint8_t len, uint8_t *pPayload);

/*********************************************************************
 * @fn      setNextDataChan
 *
 * @brief   This function returns the next data channel for a CM connection
 *          based on the previous data channel, the hop length, and one
 *          connection interval to the next active event. If the
 *          derived channel is "used", then it is returned. If the derived
 *          channel is "unused", then a remapped data channel is returned.
 *
 *          Note: nextChan is updated, and must remain that way, even if the
 *                remapped channel is returned.
 *
 * @param   sessionId - Value identifying the CM session.
 * @param   numEvents - Number of events.
 *
 * @return  Next data channel.
 */
static uint8_t setNextDataChan( uint8_t sessionId, uint16_t numEvents )
{
  ubCM_ConnInfo_t *connInfo;
  uint8_t         chanMapTable[CM_MAX_NUM_DATA_CHAN];
  uint8_t         i, j;
  uint8_t         numUsedChans = 0;

  // get pointer to connection info
  connInfo = &ubCMConnInfo.ArrayOfConnInfo[sessionId-1];

  // channels 37..39 are not data channels and these bits should not be set
  connInfo->chanMap[CM_NUM_BYTES_FOR_CHAN_MAP-1] &= 0x1F;

  // the used channel map uses 1 bit per data channel, or 5 bytes for 37 chans
  for (i=0; i<CM_NUM_BYTES_FOR_CHAN_MAP; i++)
  {
    // save each valid channel for every channel map bit that's set
    // Note: When i is on the last byte, only 5 bits need to be checked, but
    //       it is easier here to check all 8 with the assumption that the rest
    //       of the reserved bits are zero.
    for (j=0; j<CM_BITS_PER_BYTE; j++)
    {
      // check if the channel is used; only interested in used channels
      if ( (connInfo->chanMap[i] >> j) & 1 )
      {
        // sequence used channels in ascending order
        chanMapTable[ numUsedChans ] = (i*8U)+j;

        // count it
        numUsedChans++;
      }
    }
  }

  // Number of use channels cannot be 0
  if (numUsedChans == 0)
  {
    RTLSCtrl_sendDebugEvt("Number of channels cannot be 0", 0);
    while(1);
  }

  // calculate the data channel based on hop and number of events to next event
  // Note: nextChan is now called UnmappedChannel in the spec.
  // Note: currentChan is now called lastUnmappedChannel in the spec.
  connInfo->nextChan = (connInfo->nextChan + (connInfo->hopValue * numEvents)) %
                       CM_MAX_NUM_DATA_CHAN;

  //RTLSCtrl_sendDebugEvent("Passive set chan %d", connInfo->nextChan);
  //BLE_LOG_INT_INT(0, BLE_LOG_MODULE_APP, "APP : setNextDataChan, numEvents=%d, nextChan=%d\n", numEvents, connInfo->nextChan);
  // check if next channel is a used data channel
  for (i=0; i<numUsedChans; i++)
  {
    // check if next channel is in the channel map table
    if ( connInfo->nextChan == chanMapTable[i] )
    {
      // it is, so return the used channel
      return( connInfo->nextChan );
    }
  }

  //RTLSCtrl_sendDebugEvent("Passive modulo chan %d", chanMapTable[connInfo->nextChan % numUsedChans]);
  //BLE_LOG_INT_INT(0, BLE_LOG_MODULE_APP, "APP : setNextDataChan, Modulo=%d, nextChan=%d\n", chanMapTable[connInfo->nextChan % numUsedChans], connInfo->nextChan);
  // next channel is unused, so return the remapped channel
  return( chanMapTable[connInfo->nextChan % numUsedChans] );
}

/*********************************************************************
 * @fn      ubCM_findNextPriorityEvt
 *
 * @brief   Find the next connection event.
 *
 * @param   None.
 *
 * @return  next sessionId.
 */
uint8_t ubCM_findNextPriorityEvt(void)
{
  uint8_t  i;
  uint8_t  rtnSessionId = 0;
  uint8_t  currentSessionId;
  uint16_t checkBit;

  // This function will use round robin priority scheme as default.
  // To change the priority scheme to least recently used,
  // define PRORITY_LEAST_RECENTLY_USED.

#if defined(PRORITY_LEAST_RECENTLY_USED)
  uint32_t oldestAnchorPoint = 0;

  // This will determine which connection has been heard the
  // least recently based on its saved timeStampMaster which is only updated
  // if a master packet was successfully caught. It also never monitors the
  // same session until all sessions being monitored have been checked.

  // Note: This scheme should not work well if connection intervals are very different
  // since the masking scheme will ignore channels that should be monitored
  // multiple times while waiting for the next conneciton on a longer interval.
  currentSessionId = ubCMConnInfo.ArrayOfConnInfo[0].sessionId;
  checkBit = 1 << currentSessionId;
  if((lastSessionMask & checkBit) == 0 && ((sessionMask & checkBit) == checkBit))
  {
    rtnSessionId = ubCMConnInfo.ArrayOfConnInfo[0].sessionId;
    oldestAnchorPoint = ubCMConnInfo.ArrayOfConnInfo[0].timeStampMaster;
  }

  for (i=1; i < CM_MAX_SESSIONS; i++)
  {
    currentSessionId = ubCMConnInfo.ArrayOfConnInfo[i].sessionId;
    checkBit = 1 << currentSessionId;
    if((lastSessionMask & checkBit) == 0 && (sessionMask & checkBit) == checkBit)
    {
      if(ubCMConnInfo.ArrayOfConnInfo[i].timeStampMaster <= oldestAnchorPoint ||
         oldestAnchorPoint == 0)
      {
        //We want to pick the oldest anchor point connection not recently monitored
        oldestAnchorPoint = ubCMConnInfo.ArrayOfConnInfo[i].timeStampMaster;
        rtnSessionId = currentSessionId;
      }
    }
  }
#else // PRORITY_LEAST_RECENTLY_USED
  // This implements the default round robin priority scheme.
  for (i=0; i < CM_MAX_SESSIONS; i++)
  {
    currentSessionId = ubCMConnInfo.ArrayOfConnInfo[i].sessionId;
    checkBit = 1 << currentSessionId;
    if ((lastSessionMask & checkBit) == 0 && (sessionMask & checkBit) == checkBit)
    {
      rtnSessionId = currentSessionId;
      break;
    }
  }
#endif // PRORITY_LEAST_RECENTLY_USED

  //Fill in mask
  lastSessionMask |= (1 << rtnSessionId);

  //If all sessions have been checked clear lastSessionId mask
  if((lastSessionMask & sessionMask) == sessionMask)
  {
    lastSessionMask = 0x00;
  }

  if(rtnSessionId != 0)
  {
    ubCM_setupNextCMEvent(rtnSessionId);
  }
  return rtnSessionId;
}

/*********************************************************************
 * @fn      ubCM_setupNextCMEvent
 *
 * @brief   The connection updates will be managed by the Host device
 *          and we are interested in connection events even when the
 *          slave may not send data. The following connInfo are updated
 *          after the call: currentChan, ScanDuration, currentStartTime,
 *          and nextStartTime.
 *
 * @param   sessionId - Value identifying the CM session.
 *
 * @return  None.
 */
void ubCM_setupNextCMEvent(uint8_t sessionId)
{
  ubCM_ConnInfo_t *connInfo;
  uint32_t        timeToNextEvt;
  uint32_t        scanDuration;
  uint32_t        currentTime;
  uint32_t        deltaTime;
  uint16_t        numEvents;
  volatile uint32 keySwi;

  keySwi = Swi_disable();

  // get pointer to connection info
  connInfo = &ubCMConnInfo.ArrayOfConnInfo[sessionId-1];

  currentTime = RF_getCurrentTime();

  // deltaTime is the distance between the current time and
  // the last anchor point.
  if (connInfo->timeStampMaster < currentTime)
  {
    deltaTime = currentTime - connInfo->timeStampMaster;
  }
  else
  {
    //Rx clock must have rolled over so adjust for wrap around
    deltaTime = currentTime + (0xFFFFFFFF - connInfo->timeStampMaster);
  }

  // Figure out how many connection events have passed.
  numEvents = deltaTime / (connInfo->connInterval * BLE_TO_RAT) + 1;

  // Update start time to the new anchor point.
  connInfo->currentStartTime = connInfo->timeStampMaster;

  // time to next event is just the connection intervals in 625us ticks
  timeToNextEvt = connInfo->connInterval * numEvents;

  // advance the anchor point in RAT ticks
  connInfo->currentStartTime += (timeToNextEvt * BLE_TO_RAT);

  //if not enough time to start scan, bump to next connection interval
  if(connInfo->currentStartTime - currentTime < BLE_EVENT_PAD_TIME)
  {
    connInfo->currentStartTime += connInfo->connInterval * BLE_TO_RAT;
  }

  // account for radio startup overhead and jitter per the spec, pull values
  // from BLE stack. Also need to compensate for "missing master and tracking
  // slave" scenario. The RX window will be opened ahead of time so that
  // the master is guaranteed to be captured.
  connInfo->currentStartTime -= (BLE_RX_RAMP_OVERHEAD +
                                 BLE_JITTER_CORRECTION +
                                 BLE_RAT_IN_256US +
                                 BLE_RAT_IN_150US);

  // Calc timerDrift, scaFactor is (CM ppm + Master ppm).
  connInfo->timerDrift = ( (timeToNextEvt * connInfo->combSCA) / BLE_RAT_IN_100US ) + 1;

  // setup the Start Time of the receive window
  // Note: In the case we don't receive a packet at the first connection
  //       event, (and thus, don't have an updated anchor point), this anchor
  //       point will be used for finding the start of the connection event
  //       after that. That is, the update is relative to the last valid anchor
  //       point.
  // Note: If the AP is valid, we have to adjust the AP by timer drift. If the
  //       AP is not valid, we still have to adjust the AP based on the amount
  //       of timer drift that results from a widened window. Since SL is
  //       disabled when the AP is invalid (i.e. a RX Timeout means no packet
  //       was received, and by the spec, SL is discontinued until one is),
  //       the time to next event is the connection interval, and timer drift
  //       was re-calculated based on (SL+1)*CI where SL=0.
  connInfo->nextStartTime = connInfo->currentStartTime - connInfo->timerDrift;

  // setup the receiver Timeout time
  // Note: If the AP is valid, then timeoutTime was previously cleared and any
  //       previous window widening accumulation was therefore reset to zero.
  // Note: Timeout trigger remains as it was when connection was formed.
  scanDuration = (2 * connInfo->timerDrift);

  // additional widening based on whether the AP is valid
  // Note: The overhead to receive a preamble and synch word to detect a
  //       packet is added in case a packet arrives right at the end of the
  //       receive window.
  scanDuration += BLE_RX_RAMP_OVERHEAD + (2 * BLE_JITTER_CORRECTION) + BLE_RX_SYNCH_OVERHEAD;

  // The CM endTime is a bit different than a standard slave. We only want to keep Rx on
  // long enough to receive 2 packets at each AP, one Master and one Slave if present.
  // Therefore we only need to at most remain active for the full timeoutTime + the duration
  // it takes to receive to max size packets + 300 us or 2*T_IFS for the min inter frame timing.
  // 1515 us is based on the time it takes to receive 2 packets with ~160 byte payloads + headers.
  scanDuration += (BLE_RAT_IN_1515US + 2 * BLE_RAT_IN_150US);
  connInfo->scanDuration = (scanDuration / BLE_TO_RAT) + 1;
#if 0 // issue on multi channel, will be fixed on next releases
  // next channel should always be calculated by adding only 1 hop (not numEvents)
  // on indicationCB() - set timeStamp of last data time from master/slave
  // on completeCB - calculate of numEvents of missing indications to set the new start time for the scan
  // but the next channel to scan should not use the num of missing indications it always increment by 1 hop
  // !!! do it only after first scan complete
  {
    static int entryCount = 0;
    if (entryCount++ >= 1)
    {
      numEvents = 1;
    }
  }
#endif

  // Finally we will update the next channel.
  connInfo->currentChan = setNextDataChan( sessionId, numEvents );

  Swi_restore(keySwi);
}

/*********************************************************************
 * CALLBACKS
 */

/*********************************************************************
 * @fn      monitor_stateChangeCB
 *
 * @brief   Callback from Micro Monitor indicating a state change.
 *
 * @param   newState - new state
 *
 * @return  None.
 */
static void monitor_stateChangeCB(ugapMonitorState_t newState)
{
  volatile uint32_t keyHwi;

  keyHwi = Hwi_disable();
  ugapMonitorState_t *pNewState = malloc(sizeof(ugapMonitorState_t));
  Hwi_restore(keyHwi);

  // Drop if we could not allocate
  if (!pNewState)
  {
    return;
  }

  *pNewState = newState;

  // Notify application that monitor state has changed
  if (FALSE == ubCM_callApp(CM_MONITOR_STATE_CHANGED_EVT, pNewState))
  {
    // Calling App failed, free the message
    keyHwi = Hwi_disable();
    free(pNewState);
    Hwi_restore(keyHwi);
  }
}

/*********************************************************************
 * @fn      monitor_indicationCB
 *
 * @brief   Callback from Micro monitor notifying that a data
 *          packet is received.
 *
 * @param   status status of a monitoring scan
 * @param   sessionId session ID
 * @param   len length of the payload
 * @param   pPayload pointer to payload
 *
 * @return  None.
 */
static void monitor_indicationCB(bStatus_t status, uint8_t sessionId,
                                 uint8_t len, uint8_t *pPayload)
{
  uint8_t  rawRssi;
  int8_t   rssi;
  uint32_t timeStamp;
  ubCM_ConnInfo_t *connInfo;
  packetReceivedEvt_t *pPacketInfo;
  rfStatus_t rfStatus;

  // Access the connection info array
  connInfo = &ubCMConnInfo.ArrayOfConnInfo[sessionId-1];

  // Copy RF status
  memcpy(&rfStatus, pPayload + len + CM_RFSTATUS_OFFSET, sizeof(rfStatus));

  //  We would like to check if the RSSI that we received is valid
  //  To do this we need to first verify if the CRC on the packet was correct
  //  If it is not correct then the validity of the RSSI cannot be trusted and it should be discarded
  if (!rfStatus.bCrcErr)
  {
    // CRC is good
    rawRssi = *(pPayload + len + CM_RSSI_OFFSET);

    // Corrects RSSI if valid
    rssi = CM_CHECK_LAST_RSSI( rawRssi );
  }
  else
  {
    // CRC is bad
    rssi = CM_RSSI_NOT_AVAILABLE;
  }

  timeStamp = *(uint32_t *)(pPayload + len + CM_TIMESTAMP_OFFSET);

  volatile uint32_t keyHwi;

  keyHwi = Hwi_disable();
  pPacketInfo = malloc(sizeof(packetReceivedEvt_t));
  Hwi_restore(keyHwi);

  // Drop the packet if we could not allocate
  if (!pPacketInfo)
  {
    return;
  }

  pPacketInfo->len = len;
  pPacketInfo->pPayload = pPayload;
  pPacketInfo->seesionId = sessionId;

  if (cmMaster == TRUE)
  {
    // Master packet
    connInfo->timeStampMaster = timeStamp;
    connInfo->rssiMaster = rssi;
    connInfo->timesScanned++;
    cmMaster = FALSE;

    pPacketInfo->masterPacket = TRUE;
  }
  else
  {
    // Slave packet
    connInfo->timeStampSlave = timeStamp;
    connInfo->rssiSlave = rssi;

    pPacketInfo->masterPacket = FALSE;
  }

  pPacketInfo->status = status;

  // Notify application that a packet was received
  if (FALSE == ubCM_callApp(CM_PACKET_RECEIVED_EVT, (uint8_t *)pPacketInfo))
  {
    // Calling App failed, free the message
    keyHwi = Hwi_disable();
    free(pPacketInfo);
    Hwi_restore(keyHwi);
  }
}

/*********************************************************************
 * @fn      monitor_completeCB
 *
 * @brief   Callback from Micro Monitor notifying that a monitoring
 *          scan is completed.
 *
 * @param   status - How the last event was done. SUCCESS or FAILURE.
 * @param   sessionId - Session ID
 *
 * @return  None.
 */
static void monitor_completeCB(bStatus_t status, uint8_t sessionId)
{
  ubCM_ConnInfo_t *connInfo;
  monitorCompleteEvt_t *pMonitorCompleteEvt;
  volatile uint32_t keyHwi;

  keyHwi = Hwi_disable();
  pMonitorCompleteEvt = malloc(sizeof(monitorCompleteEvt_t));
  Hwi_restore(keyHwi);

  // Drop if we could not allocate
  if (!pMonitorCompleteEvt)
  {
    return;
  }

  // Set event
  pMonitorCompleteEvt->status = status;
  pMonitorCompleteEvt->sessionId = sessionId;

  // Access the connection info array
  connInfo = &ubCMConnInfo.ArrayOfConnInfo[sessionId - 1];

  // continue attempt to monitor other sessions.
  // The context of this callback is from the uStack task.
  (void)ubCM_start(ubCM_findNextPriorityEvt());

  // If the threshold for consecutive missing packets is reached, terminate the connection
  if (connInfo->consecutiveTimesMissed > BLE_CONSECUTIVE_MISSED_CONN_EVT_THRES)
  {
    pMonitorCompleteEvt->status = CM_FAILED_NOT_ACTIVE;
  }

  if (pMonitorCompleteEvt->status != CM_FAILED_NOT_ACTIVE)
  {
    if (cmMaster == FALSE)
    {
      // An M->S packet was received. We are not sure about the S->M packet.
      // Reset the cmMaster to true for the next connection interval and consecutiveTimesMissed to zero.
      cmMaster = TRUE;
      connInfo->consecutiveTimesMissed = 0;
    }
    else
    {
      // Keep a record of missing packets in this monitor session
      connInfo->timesMissed++;
      connInfo->consecutiveTimesMissed++;
    }
  }

  // Notify application that a monitor session was complete
  if (FALSE == ubCM_callApp(CM_CONN_EVT_COMPLETE_EVT, (uint8_t *)pMonitorCompleteEvt))
  {
    // Calling App failed, free the message
    keyHwi = Hwi_disable();
    free(pMonitorCompleteEvt);
    Hwi_restore(keyHwi);
  }
}

/*********************************************************************
 * PUBLIC FUNCTIONS
 */

/*********************************************************************
 * @fn      ubCm_init
 *
 * @brief   Initialization function for micro BLE connection monitor.
 *          This function initializes the callbacks and default connection
 *          info.
 *
 * @param   none
 *
 * @return  true: CM initialization successful
 *          false: CM initialization failed
 */
bool ubCm_init(pfnRtlsPassiveCb appCb)
{
  uint8_t i;

  ugapMonitorCBs_t monitorCBs = {
    monitor_stateChangeCB,
    monitor_indicationCB,
    monitor_completeCB };

  // Initialize default connection info
  ubCMConnInfo.numHandles = 0;
  for (i = 0; i < CM_MAX_SESSIONS; i++)
  {
    ubCMConnInfo.ArrayOfConnInfo[i].sessionId = CM_INVALID_SESSION_ID;
    ubCMConnInfo.ArrayOfConnInfo[i].connInterval = CM_CONN_INTERVAL_MAX;
    ubCMConnInfo.ArrayOfConnInfo[i].timesScanned = 0;
  }

  if (appCb != NULL)
  {
    // Save application callback
    gAppCb = appCb;
  }
  else
  {
    return FALSE;
  }

  // Initilaize Micro GAP Monitor
  if (SUCCESS == ugap_monitorInit(&monitorCBs))
  {
    return TRUE;
  }

  return FALSE;
}

/*********************************************************************
 * @fn      ubCM_isSessionActive
 *
 * @brief   Check if the CM sessionId is active
 *
 * @param   sessionId - Value identifying the CM session to check.
 *
 * @return  CM_SUCCESS = 0: CM session active
 *          CM_FAILED_NOT_FOUND = 2: CM session not active
 */
uint8_t ubCM_isSessionActive(uint8_t sessionId)
{
  ubCM_ConnInfo_t *connInfo;

  if (sessionId == 0 || sessionId > CM_MAX_SESSIONS)
  {
    // Not a valid sessionId or no CM session has started
    return CM_FAILED_NOT_FOUND;
  }

  // Access the connection info array
  connInfo = &ubCMConnInfo.ArrayOfConnInfo[sessionId-1];
  if (connInfo->sessionId == 0)
  {
    // CM session has not been started
    return CM_FAILED_NOT_ACTIVE;
  }

  // CM session is indeed active
  return CM_SUCCESS;
}

/*********************************************************************
 * @fn      ubCM_start
 *
 * @brief   To establish a new CM session or continue monitor an
 *          existing CM session.
 *
 * @param   sessionId - Value identifying the CM session requested
 *          to start.
 *
 * @return  CM_SUCCESS = 0: CM session started.
 *          CM_FAILED_TO_START = 1: Failed to start because next
 *          anchor point is missed.
 */
uint8_t ubCM_start(uint8_t sessionId)
{
  ubCM_ConnInfo_t *connInfo;
  uint8_t status = CM_FAILED_TO_START;

  if (ubCM_isSessionActive(sessionId) == CM_SUCCESS ||
      ubCM_isSessionActive(sessionId) == CM_FAILED_NOT_ACTIVE)
  {
    // Access the connection info array
    connInfo = &ubCMConnInfo.ArrayOfConnInfo[sessionId-1];

    // Range checks
    if (connInfo->hopValue > BLE_HOP_VALUE_MAX ||
        connInfo->hopValue < BLE_HOP_VALUE_MIN ||
        connInfo->combSCA > BLE_COMB_SCA_MAX ||
        connInfo->combSCA < BLE_COMB_SCA_MIN)
    {
      return CM_FAILED_OUT_OF_RANGE;
    }

    // Save the next sessionId
    uble_setParameter(UBLE_PARAM_SESSIONID, sizeof(uint8_t), &sessionId);

    // Kick off the monitor request
    if (ugap_monitorRequest(connInfo->currentChan,
                            connInfo->accessAddr,
                            connInfo->nextStartTime,
                            connInfo->scanDuration,
                            connInfo->crcInit) == SUCCESS)
    {
      // Is the sessionId new?
      if (ubCM_isSessionActive(sessionId) == CM_FAILED_NOT_ACTIVE &&
          ubCMConnInfo.numHandles < CM_MAX_SESSIONS)
      {
        // Activate this session ID
        connInfo->sessionId = sessionId;
        ubCMConnInfo.numHandles++;
        // Set bit in mask
        sessionMask |= (1 << sessionId);
      }
      status = CM_SUCCESS;
    }
  }
  return status;
}

/*********************************************************************
 * @fn      ubCM_stop
 *
 * @brief   To discontinue a CM session.
 *
 * @param   sessionId - Value identifying the CM session requested to stop.
 *          For an invalid sessionId, this function will return
 *          CM_FAILED_NOT_FOUND status.
 *
 * @return  CM_SUCCESS = 0: CM session ended
 *          CM_FAILED_NOT_FOUND = 2: Could not find CM session to stop
 */
uint8_t ubCM_stop(uint8_t sessionId)
{
  ubCM_ConnInfo_t *connInfo;

  if (ubCM_isSessionActive(sessionId) == CM_SUCCESS &&
      ubCMConnInfo.numHandles > 0)
  {
    // Access the connection info array
    connInfo = &ubCMConnInfo.ArrayOfConnInfo[sessionId-1];

    // Mark the sessionId as invalid,
    // Initialize default connection info and
    // decrement the number of connection handles.
    // Note this will leave a hole in the array.
    memset(connInfo, 0, sizeof(ubCM_ConnInfo_t));
    ubCMConnInfo.numHandles--;
    // Clear bit
    sessionMask &= ~(1 << sessionId);
    lastSessionMask &= ~(1 << sessionId);
    return CM_SUCCESS;
  }
  else
  {
    return CM_FAILED_NOT_FOUND;
  }
}

/*********************************************************************
 * @fn      ubCM_startExt
 *
 * @brief   Initializes new connection data to start new CM sessions
 *
 * @param   hostConnHandle - connHandle from host requests
 *          accessAddress - accessAddress for requested connection
 *          connInterval - connection interval
 *          hopValue - hop value
 *          nextChan - the next channel that the requested connection will transmit on
 *          chanMap - channel map
 *          crcInit - crcInit value
 *
 * @return  index: A valid index will be less than CM_MAX_SESSIONS an invalid
 *                 index will be greater than or equal to CM_MAX_SESSIONS.
 */
uint8_t ubCM_startExt(uint8_t hostConnHandle, uint32_t accessAddress, uint16_t connInterval, uint8_t hopValue, uint8_t nextChan, uint8_t *chanMap, uint32_t crcInit)
{
  //i's initial value will make cmStart() fail if ubCMConnInfo.numHandles >= CM_MAX_SESSIONS
  uint8_t i = ubCMConnInfo.numHandles;
  uint8_t sessionId;
  uint16_t chanSkip;

  //First make sure we have not reached our session limit
  if (ubCMConnInfo.numHandles <= CM_MAX_SESSIONS)
  {
    //set sessionId to first inactive session id
    for (sessionId=1; sessionId <= CM_MAX_SESSIONS; sessionId++)
    {
      if(ubCM_isSessionActive(sessionId) == CM_FAILED_NOT_ACTIVE)
      {
        i = sessionId-1;//index of session id is 1 less than acctual id
        break;
      }
    }

    // check that session id is not out of bounds
    if (sessionId > CM_MAX_SESSIONS)
    {
      return CM_INVALID_SESSION_ID;
    }

    ubCMConnInfo.ArrayOfConnInfo[i].hostConnHandle = hostConnHandle;
    ubCMConnInfo.ArrayOfConnInfo[i].accessAddr = accessAddress;

    ubCMConnInfo.ArrayOfConnInfo[i].currentStartTime = RF_getCurrentTime() + 20 * BLE_TO_RAT;
    ubCMConnInfo.ArrayOfConnInfo[i].nextStartTime = ubCMConnInfo.ArrayOfConnInfo[i].currentStartTime;

    ubCMConnInfo.ArrayOfConnInfo[i].connInterval = connInterval;

    if(ubCMConnInfo.ArrayOfConnInfo[i].connInterval < CM_CONN_INTERVAL_MIN ||
       ubCMConnInfo.ArrayOfConnInfo[i].connInterval > CM_CONN_INTERVAL_MAX)
    {
      return CM_INVALID_SESSION_ID;
    }

    ubCMConnInfo.ArrayOfConnInfo[i].crcInit = crcInit;
    ubCMConnInfo.ArrayOfConnInfo[i].hopValue = hopValue;

    ubCMConnInfo.ArrayOfConnInfo[i].combSCA = 90; // Master = 50 + Slave = 40 leave hard coded for now

    ubCMConnInfo.ArrayOfConnInfo[i].currentChan = nextChan;
    ubCMConnInfo.ArrayOfConnInfo[i].nextChan = nextChan;

    memcpy(ubCMConnInfo.ArrayOfConnInfo[i].chanMap, chanMap, CM_NUM_BYTES_FOR_CHAN_MAP);

    // Mask last 8 bits to ensure we only consider data channels and not advert
    ubCMConnInfo.ArrayOfConnInfo[i].chanMap[4] = ubCMConnInfo.ArrayOfConnInfo[i].chanMap[4] & 0x1F;

    // With a 100ms connection interval we will skip 3 channels into the future (in each connection event we can monitor 1 channel)
    // This is mainly done to ensure that even a very slow bus is able to send the connection information in time
    // In time: before the Master/Slave already go past the channel that we chose to listen on
    // If Master/Slave are already past this point, we will have to wait numChannels*connInterval until we catch the
    // connection once again
    chanSkip = (uint16_t)((BUS_LATENCY_IN_MS/connInterval) + 1);

    ubCMConnInfo.ArrayOfConnInfo[i].currentChan = setNextDataChan(sessionId, chanSkip); //jump some [ms] channels into the future

    // Catch anchor point n+2 connection intervals in the future
    ubCMConnInfo.ArrayOfConnInfo[i].scanDuration = (uint16_t)(connInterval*(chanSkip + 1));

    //If i+1 > CM_MAX_SESSIONS then we know this function failed. Let cmStart()
    //return a failure message since i will be an out of range index
    if (CM_SUCCESS == ubCM_start(sessionId))
    {
      return (sessionId);
    }
  }

  return CM_INVALID_SESSION_ID;
}

/*********************************************************************
 * @fn      ubCM_updateExt
 *
 * @brief   Initializes new connection data without start new CM sessions
 *
 * @param   hostConnHandle - connHandle from host requests
 *          accessAddress - accessAddress for requested connection
 *          connInterval - connection interval
 *          hopValue - hop value
 *          nextChan - the next channel that the requested connection will transmit on
 *          chanMap - channel map
 *          crcInit - crcInit value
 *
 * @return  index: A valid index will be less than CM_MAX_SESSIONS an invalid
 *                 index will be greater than or equal to CM_MAX_SESSIONS.
 */
uint8_t ubCM_updateExt(uint8_t sessionId, uint8_t hostConnHandle, uint32_t accessAddress, uint16_t connInterval, uint8_t hopValue, uint8_t nextChan, uint8_t *chanMap, uint32_t crcInit)
{
  uint8_t i;

  i = sessionId-1; // index of session id is 1 less than acctual id

  // Copy only the channel map array
  // Don't change the nextChan to avoid race conditions that might 1. skip channels, 2. select same channel twice
  memcpy(ubCMConnInfo.ArrayOfConnInfo[i].chanMap, chanMap, CM_NUM_BYTES_FOR_CHAN_MAP);

  // Mask last 8 bits to ensure we only consider data channels and not advert
  ubCMConnInfo.ArrayOfConnInfo[i].chanMap[4] = ubCMConnInfo.ArrayOfConnInfo[i].chanMap[4] & 0x1F;

  // Update connection interval
  ubCMConnInfo.ArrayOfConnInfo[i].connInterval = connInterval;

  if(ubCMConnInfo.ArrayOfConnInfo[i].connInterval < CM_CONN_INTERVAL_MIN ||
     ubCMConnInfo.ArrayOfConnInfo[i].connInterval > CM_CONN_INTERVAL_MAX)
  {
    return CM_INVALID_SESSION_ID;
  }

  return (sessionId);
}

/*********************************************************************
 * @fn      ubCM_callApp
 *
 * @brief   Calls the callback provided by the application
 *
 * @param   eventId - CM event
 * @param   data  - pointer to the data
 *
 * @return  status
 */
uint8_t ubCM_callApp(uint8_t eventId, uint8_t *data)
{
  cmEvt_t *cmEvt;
  volatile uint32_t keyHwi;

  keyHwi = Hwi_disable();
  cmEvt = (cmEvt_t *)malloc(sizeof(cmEvt_t));
  Hwi_restore(keyHwi);

  // Check if we could alloc message
  if (!cmEvt)
  {
    return FALSE;
  }

  cmEvt->event = eventId;
  cmEvt->pData = data;

  if (gAppCb != NULL)
  {
    gAppCb((uint8_t *)cmEvt);
  }
  else
  {
    // Program cannot run without a CB
    while(1);
  }

  return TRUE;
}
/*********************************************************************
 *********************************************************************/
