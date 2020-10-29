/******************************************************************************

 @file  rtls_ctrl_aoa.c

 @brief This file contains the functions and structures specific to AoA
 Group: WCS, BTS
 Target Device: cc13x2_26x2

 ******************************************************************************
 
 Copyright (c) 2018-2020, Texas Instruments Incorporated
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

#include <stdint.h>
#include <stdlib.h>

#include "rtls_ctrl_aoa.h"
#include "rtls_host.h"
#include <ti/sysbios/hal/Hwi.h>
#include <ti/sysbios/knl/Swi.h>
#include <string.h>
#include <ti/drivers/pin/PINCC26XX.h>

#include "ant_array1_config_boostxl_rev1v1.h"
#include "ant_array2_config_boostxl_rev1v1.h"

/*********************************************************************
 * MACROS
 */


/*********************************************************************
 * CONSTANTS
 */

/*********************************************************************
 * TYPEDEFS
 */

// A single AoA sample
typedef struct
{
  int16_t angle;
  int16_t currentangle;
  int8_t  rssi;
  uint8_t channel;
  uint8_t antenna;
} AoA_Sample_t;

// Moving average structure
typedef struct
{
  int16_t array[6];
  uint8_t idx;
  uint8_t numEntries;
  uint8_t currentAntennaArray;
  int16_t currentAoA;
  int8_t  currentRssi;
  uint8_t currentCh;
  int32_t AoAsum;
  int16_t AoA;
} AoA_movingAverage_t;

typedef struct
{
  AoA_movingAverage_t AoA_ma;
  AoA_AntennaResult_t aoaResults;
} AoA_connInfo_t;

typedef struct
{
  AoA_connInfo_t *connResInfo;
  AoA_AntennaConfig_t *antArrayConfig;
  uint8_t sampleCtrl;
  uint8_t resultMode;
} AoA_controlBlock_t;

/*********************************************************************
 * GLOBAL VARIABLES
 */

/*********************************************************************
 * EXTERNAL VARIABLES
 */

/*********************************************************************
 * EXTERNAL FUNCTIONS
 */
extern void AssertHandler(uint8 assertCause, uint8 assertSubcause);

/*********************************************************************
 * LOCAL VARIABLES
 */

#ifdef RTLS_PASSIVE
AoA_IQSample_Ext_t samplesBuff[AOA_RES_MAX_SIZE];
#endif

AoA_controlBlock_t gAoaCb = {0};

/*********************************************************************
 * LOCAL FUNCTIONS
 */
AoA_Sample_t RTLSCtrl_estimateAngle(uint16_t connHandle, uint8_t sampleCtrl);

/*********************************************************************
* @fn      RTLSCtrl_postProcessAoa
*
* @brief   Called at the end of each connection event to extract I/Q samples
*
* @param   aoaControlBlock - AoA information saved by RTLS Control
* @param   rssi - rssi to be reported to RTLS Host
* @param   channel - channel that was used for this AoA run
* @param   sampleCtrl - sample control configs: 0x01 = RAW RF, 0x00 = Filtered results (switching period omitted), bit 4,5 0x10 - ONLY_ANT_1, 0x20 - ONLY_ANT_2
*
* @return  none
*/
#ifdef RTLS_PASSIVE
void RTLSCtrl_postProcessAoa(uint8_t connHandle, uint8_t resultMode, int8_t rssi, uint8_t channel, uint8_t sampleCtrl)
#else // RTLS_MASTER
void RTLSCtrl_postProcessAoa(rtlsAoaIqEvt_t *pEvt)
#endif
{
  uint8_t antenna;

#ifdef RTLS_PASSIVE

  AOA_postProcess(rssi, channel, samplesBuff);

  // Poll until we have samples to work with
  while(AOA_getSampleState() == SAMPLES_NOT_READY);

  // If samples are not valid, nothing to do here
  if (AOA_getSampleState() == SAMPLES_NOT_VALID)
  {
    return;
  }

#else // RTLS_MASTER
  uint16_t connHandle = pEvt->connHandle;
  uint8_t sampleCtrl = pEvt->sampleCtrl;
  int8_t rssi = pEvt->rssi;
  uint8_t channel = pEvt->channel;

  if (IS_AOA_CONFIG_RF_RAW(sampleCtrl) && gAoaCb.resultMode != AOA_MODE_RAW)
  {
    RTLSCtrl_sendDebugEvt("RAW RF only in AOA_MODE_RAW", RTLS_FAIL);
    return;
  }
#endif

  if (IS_AOA_CONFIG_ONLY_ANT_1(gAoaCb.sampleCtrl))
  {
    antenna = ANT_ARRAY_A1x;
  }
  else
  {
    antenna = ANT_ARRAY_A2x;
  }

  switch (gAoaCb.resultMode)
  {
    case AOA_MODE_ANGLE:
    {
      AoA_Sample_t aoaTempResult;
      rtlsAoaResultAngle_t aoaResult;

#ifdef RTLS_MASTER
      AOA_getPairAngles(gAoaCb.antArrayConfig,
                        &gAoaCb.connResInfo[pEvt->connHandle].aoaResults,
                        pEvt->numIqSamples,
                        pEvt->sampleRate,
                        pEvt->sampleSize,
                        pEvt->slotDuration,
                        pEvt->numAnt,
                        pEvt->pIQ);
#elif RTLS_PASSIVE
      AOA_getPairAngles(gAoaCb.antArrayConfig, &gAoaCb.connResInfo[connHandle].aoaResults);
#endif

      aoaTempResult = RTLSCtrl_estimateAngle(connHandle, sampleCtrl);

      aoaResult.connHandle = connHandle;
      aoaResult.angle = aoaTempResult.angle;
      aoaResult.antenna = antenna;
      aoaResult.rssi = rssi;
      aoaResult.channel = channel;

      RTLSHost_sendMsg(RTLS_CMD_AOA_RESULT_ANGLE, HOST_ASYNC_RSP, (uint8_t *)&aoaResult, sizeof(rtlsAoaResultAngle_t));
    }
    break;

    case AOA_MODE_PAIR_ANGLES:
    {
      rtlsAoaResultPairAngles_t aoaResult;
      int16_t *pairAngle;

#ifdef RTLS_MASTER
      AOA_getPairAngles(gAoaCb.antArrayConfig,
                        &gAoaCb.connResInfo[connHandle].aoaResults,
                        pEvt->numIqSamples,
                        pEvt->sampleRate,
                        pEvt->sampleSize,
                        pEvt->slotDuration,
                        pEvt->numAnt,
                        pEvt->pIQ);
#elif RTLS_PASSIVE
      AOA_getPairAngles(gAoaCb.antArrayConfig, &gAoaCb.connResInfo[connHandle].aoaResults);
#endif

      pairAngle = gAoaCb.connResInfo[connHandle].aoaResults.pairAngle;

      aoaResult.connHandle = connHandle;
      aoaResult.rssi = rssi;
      aoaResult.channel = channel;
      aoaResult.antenna = antenna;

      for (int i = 0; i < CALC_NUM_ANT_PAIRS(BOOSTXL_AOA_NUM_ANT); i++)
      {
        aoaResult.pairAngle[i] = pairAngle[i];
      }

      RTLSHost_sendMsg(RTLS_CMD_AOA_RESULT_PAIR_ANGLES, HOST_ASYNC_RSP, (uint8_t *)&aoaResult, sizeof(rtlsAoaResultPairAngles_t));
    }
    break;

    case AOA_MODE_RAW:
    {
      rtlsAoaResultRaw_t *aoaResult;
      uint16_t samplesToOutput;

#ifdef RTLS_PASSIVE
      AoA_IQSample_Ext_t *pIterExt;

      // Sanity check
      if ((aoaResult = RTLSCtrl_malloc(sizeof(rtlsAoaResultRaw_t) + (MAX_SAMPLES_SINGLE_CHUNK * sizeof(AoA_IQSample_Ext_t)))) == NULL)
      {
        return;
      }

      // Passive only supports samples of size int16
      pIterExt = AOA_getRawSamples();

      aoaResult->samplesLength = AOA_RES_MAX_SIZE;
#else // RTLS_MASTER

      // The samples may be of either type, depending on sampleSize
      // For the sake of simplicity, just set both to point to the samples and decide the output format later
      AoA_IQSample_Ext_t *pIterExt = (AoA_IQSample_Ext_t *)pEvt->pIQ;
      AoA_IQSample_t *pIter = (AoA_IQSample_t *)pEvt->pIQ;

      // Allocate result structure to consider both options of sampleSize
      aoaResult = RTLSCtrl_malloc(sizeof(rtlsAoaResultRaw_t) + (MAX_SAMPLES_SINGLE_CHUNK * sizeof(AoA_IQSample_Ext_t)));

      // Sanity check
      if (aoaResult == NULL)
      {
        return;
      }

      aoaResult->samplesLength = pEvt->numIqSamples;
#endif

      // Set various parameters
      aoaResult->connHandle = connHandle;
      aoaResult->channel = channel;
      aoaResult->rssi = rssi;
      aoaResult->antenna = antenna;

      // Set offset of the result set within the total bulk of samples
      aoaResult->offset = 0;

      do
      {
        // If the remainder is larger than buff size, tx maximum buff size
        if (aoaResult->samplesLength - aoaResult->offset > MAX_SAMPLES_SINGLE_CHUNK)
        {
          samplesToOutput = MAX_SAMPLES_SINGLE_CHUNK;
        }
        else
        {
          // If not, then output the remaining data
          samplesToOutput = aoaResult->samplesLength - aoaResult->offset;
        }

        // Copy the samples to output buffer
        for (int i = 0; i < samplesToOutput; i++)
        {
#ifdef RTLS_PASSIVE
            aoaResult->samples[i].i = pIterExt[i].i;
            aoaResult->samples[i].q = pIterExt[i].q;
#else // RTLS_MASTER
            if (pEvt->sampleSize == 2)
            {
              aoaResult->samples[i].i = pIterExt[i].i;
              aoaResult->samples[i].q = pIterExt[i].q;
            }
            else // sampleSize = 1
            {
              aoaResult->samples[i].i = pIter[i].i;
              aoaResult->samples[i].q = pIter[i].q;
            }
#endif
        }

        RTLSHost_sendMsg(RTLS_CMD_AOA_RESULT_RAW, HOST_ASYNC_RSP, (uint8_t *)aoaResult, sizeof(rtlsAoaResultRaw_t) + (sizeof(AoA_IQSample_Ext_t) * samplesToOutput));

        // Update offset
        aoaResult->offset += samplesToOutput;
#ifdef RTLS_MASTER
        pIter += MAX_SAMPLES_SINGLE_CHUNK;
#endif
        pIterExt += MAX_SAMPLES_SINGLE_CHUNK;
      }
      while (aoaResult->offset < aoaResult->samplesLength);

      if (aoaResult)
      {
        RTLSUTIL_FREE(aoaResult);
      }
    }
    break;

    default:
      break;
  } // Switch
}

/*********************************************************************
* @fn      RTLSCtrl_estimateAngle
*
* @brief   Estimate angle based on I/Q readings
*
* @param   connHandle - connection handle
* @param   sampleCtrl - sample control configs: 0x01 = RAW RF, 0x00 = Filtered results (switching period omitted), bit 4,5 0x10 - ONLY_ANT_1, 0x20 - ONLY_ANT_2
*
* @return  AoA Sample struct filled with calculated angles
*/
AoA_Sample_t RTLSCtrl_estimateAngle(uint16_t connHandle, uint8_t sampleCtrl)
{
  AoA_Sample_t AoA;

  int8_t channelOffset;
  uint8_t channel;
  uint8_t AoA_ma_size;
  int16_t AoA_A1;
  int16_t AoA_A2;
  uint8_t selectedAntenna;

  AoA_ma_size = sizeof(gAoaCb.connResInfo[connHandle].AoA_ma.array) / sizeof(gAoaCb.connResInfo[connHandle].AoA_ma.array[0]);

  channel = gAoaCb.connResInfo[connHandle].aoaResults.ch;
  channelOffset = gAoaCb.antArrayConfig->channelOffset[channel];

  // Calculate AoA for each antenna array
  if (IS_AOA_CONFIG_ONLY_ANT_1(gAoaCb.sampleCtrl))
  {
    AoA_A1 = ((gAoaCb.connResInfo[connHandle].aoaResults.pairAngle[0] + gAoaCb.connResInfo[connHandle].aoaResults.pairAngle[1]) / 2) + 45 + channelOffset;
    selectedAntenna = ANT_ARRAY_A1x;
  }
  else if (IS_AOA_CONFIG_ONLY_ANT_2(gAoaCb.sampleCtrl))
  {
    AoA_A2 = ((gAoaCb.connResInfo[connHandle].aoaResults.pairAngle[0] + gAoaCb.connResInfo[connHandle].aoaResults.pairAngle[1]) / 2) - 45 - channelOffset;
    selectedAntenna = ANT_ARRAY_A2x;
  }

  // Antenna array is selected according to sampleCtrl
  // 1. Either A1 or A2 are selected constantly according to configuration
  if (selectedAntenna == ANT_ARRAY_A1x)
  {
    // Use AoA from Antenna Array A1
    gAoaCb.connResInfo[connHandle].AoA_ma.array[gAoaCb.connResInfo[connHandle].AoA_ma.idx] = AoA_A1;
    gAoaCb.connResInfo[connHandle].AoA_ma.currentAoA = AoA_A1;
    gAoaCb.connResInfo[connHandle].AoA_ma.currentAntennaArray = ANT_ARRAY_A1x;
    AoA.currentangle = AoA_A1;
  }
  // Signal strength is higher on A2 vs A1
  else
  {
    // Use AoA from Antenna Array A2
    gAoaCb.connResInfo[connHandle].AoA_ma.array[gAoaCb.connResInfo[connHandle].AoA_ma.idx] = AoA_A2;
    gAoaCb.connResInfo[connHandle].AoA_ma.currentAoA = AoA_A2;
    gAoaCb.connResInfo[connHandle].AoA_ma.currentAntennaArray = ANT_ARRAY_A2x;
    AoA.currentangle = AoA_A2;
  }

  gAoaCb.connResInfo[connHandle].AoA_ma.currentRssi = gAoaCb.connResInfo[connHandle].aoaResults.rssi;
  gAoaCb.connResInfo[connHandle].AoA_ma.currentCh = gAoaCb.connResInfo[connHandle].aoaResults.ch;

  // Add new AoA to moving average
  gAoaCb.connResInfo[connHandle].AoA_ma.array[gAoaCb.connResInfo[connHandle].AoA_ma.idx] = gAoaCb.connResInfo[connHandle].AoA_ma.currentAoA;

  if (gAoaCb.connResInfo[connHandle].AoA_ma.numEntries < AoA_ma_size)
  {
    gAoaCb.connResInfo[connHandle].AoA_ma.numEntries++;
  }
  else
  {
    gAoaCb.connResInfo[connHandle].AoA_ma.numEntries = AoA_ma_size;
  }

  // Calculate new moving average
  gAoaCb.connResInfo[connHandle].AoA_ma.AoAsum = 0;

  for (uint8_t i = 0; i < gAoaCb.connResInfo[connHandle].AoA_ma.numEntries; i++)
  {
    gAoaCb.connResInfo[connHandle].AoA_ma.AoAsum += gAoaCb.connResInfo[connHandle].AoA_ma.array[i];
  }
  gAoaCb.connResInfo[connHandle].AoA_ma.AoA = gAoaCb.connResInfo[connHandle].AoA_ma.AoAsum / gAoaCb.connResInfo[connHandle].AoA_ma.numEntries;

  // Update moving average index
  if (gAoaCb.connResInfo[connHandle].AoA_ma.idx >= (AoA_ma_size - 1))
  {
    gAoaCb.connResInfo[connHandle].AoA_ma.idx = 0;
  }
  else
  {
    gAoaCb.connResInfo[connHandle].AoA_ma.idx++;
  }

  // Return results
  AoA.angle = gAoaCb.connResInfo[connHandle].AoA_ma.AoA;
  AoA.rssi = gAoaCb.connResInfo[connHandle].AoA_ma.currentRssi;
  AoA.channel = gAoaCb.connResInfo[connHandle].AoA_ma.currentCh;
  AoA.antenna =  gAoaCb.connResInfo[connHandle].AoA_ma.currentAntennaArray;

  return AoA;
}

/*********************************************************************
* @fn      RTLSCtrl_initAoa
*
* @brief   Initialize AoA - has to be called before running AoA
*
* @param   sampleCtrl - sample control configs: 0x01 = RAW RF, 0x00 = Filtered results (switching period omitted), bit 4,5 0x10 - ONLY_ANT_1, 0x20 - ONLY_ANT_2
* @param   maxConnections - number of connections we need to keep results for
* @param   numAnt - number of antennas in pAntPattern
* @param   pAntPattern - antenna pattern provided by the user
* @param   resultMode - AOA_MODE_ANGLE/AOA_MODE_PAIR_ANGLES/AOA_MODE_RAW
*
* @return  status - RTLS_AOA_CONFIG_NOT_SUPPORTED/RTLS_SUCCESS
*/
rtlsStatus_e RTLSCtrl_initAoa(uint8_t maxConnections, uint8_t sampleCtrl, uint8_t numAnt, uint8_t *pAntPattern, aoaResultMode_e resultMode)
{
  // Set result mode
  gAoaCb.resultMode = resultMode;

  // Check that a correct configuration was provided
  // The current configuration supported by rtls_ctrl_aoa post process module is either:
  // 1. sampleCtrl defines antenna array 1 && pAntPattern contains antenna ID's 0, 1, 2 (in this exact order)
  // 2. sampleCtrl defines antenna array 2 && pAntPattern contains antenna ID's 3, 4, 5 (in this exact order)
  // Note: Result mode is AOA_MODE_RAW (post processing done by the user) is allowed with any antenna pattern
  if (gAoaCb.resultMode != AOA_MODE_RAW)
  {
    for (int i = 0; i < numAnt; i++)
    {
      if (IS_AOA_CONFIG_ONLY_ANT_1(sampleCtrl))
      {
        if (pAntPattern[i] != i)
        {
          return RTLS_CONFIG_NOT_SUPPORTED;
        }
      }
      else if IS_AOA_CONFIG_ONLY_ANT_2(sampleCtrl)
      {
        if (pAntPattern[i] != i + 3)
        {
          return RTLS_CONFIG_NOT_SUPPORTED;
        }
      }
      else
      {
        return RTLS_CONFIG_NOT_SUPPORTED;
      }
    }
  }

  // Check if we are already initialized
  if (gAoaCb.connResInfo == NULL)
  {
    // Initialize result arrays per connection
    gAoaCb.connResInfo = (AoA_connInfo_t *)RTLSCtrl_malloc(sizeof(AoA_connInfo_t) * maxConnections);

    if (gAoaCb.connResInfo == NULL)
    {
      AssertHandler(RTLS_CTRL_ASSERT_CAUSE_OUT_OF_MEMORY, 0);
    }

    memset(gAoaCb.connResInfo, 0, sizeof(AoA_connInfo_t) * maxConnections);

    // Initialize Pair Angles result arrays for each connection
    for (int i = 0; i < maxConnections; i ++)
    {
      gAoaCb.connResInfo[i].aoaResults.pairAngle = (int16_t *)RTLSCtrl_malloc(sizeof(int16_t) * CALC_NUM_ANT_PAIRS(BOOSTXL_AOA_NUM_ANT));

      if (gAoaCb.connResInfo[i].aoaResults.pairAngle == NULL)
      {
        AssertHandler(RTLS_CTRL_ASSERT_CAUSE_OUT_OF_MEMORY, 0);
      }

      memset(gAoaCb.connResInfo[i].aoaResults.pairAngle, 0, sizeof(int16_t) * BOOSTXL_AOA_NUM_ANT);
    }
  }

  // Save sampleCtrl flags
  gAoaCb.sampleCtrl = sampleCtrl;

  // Configurations included from antenna array files
  if (IS_AOA_CONFIG_ONLY_ANT_1(gAoaCb.sampleCtrl))
  {
    // Set BOOSTXL-AOA A1.x config
    gAoaCb.antArrayConfig = getAntennaArray1Config();

    // Set BOOSTXL-AOA A1.x channel offsets
    gAoaCb.antArrayConfig->channelOffset = getAntennaArray1ChannelOffsets();

#ifdef RTLS_PASSIVE
    // Initialize Antenna Array switching
    AOA_init(ANT_ARRAY_A1x);
#endif
  }
  else
  {
    // Set BOOSTXL-AOA A2.x config
    gAoaCb.antArrayConfig = getAntennaArray2Config();

    // Set BOOSTXL-AOA A2.x channel offsets
    gAoaCb.antArrayConfig->channelOffset = getAntennaArray2ChannelOffsets();

#ifdef RTLS_PASSIVE
    // Initialize Antenna Array switching
    AOA_init(ANT_ARRAY_A2x);
#endif
  }

  return RTLS_SUCCESS;
}
