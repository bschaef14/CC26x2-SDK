/******************************************************************************

 @file  AOA.c

 @brief This file contains methods to enable/disable and control AoA
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

#include <ti/devices/DeviceFamily.h>

#include <stdlib.h>
#include <complex.h>
#include <math.h>

#include "rf_hal.h"
#include "AOA.h"

#include <ti/drivers/PIN.h>
#include <ti/drivers/pin/PINCC26XX.h>
#include <ti/drivers/rf/RF.h>

/*******************************************************************************
 * CONSTANTS
 */

#define AOA_NUM_VALID_SAMPLES            8
#define AOA_OFFSET_FIRST_VALID_SAMPLE    8
#define AOA_NUM_SAMPLES_PER_BLOCK        16
#define AOA_SLOT_DURATION_1US            1

#define AOA_CTEINFO_ADDR                 0x21000021
#define AOA_CTE_TIME_ADDR                0x210000C7

#define AOA_CTE_NO_PROCCESSING_NO_SMPL   0

#define RadToDeg                         180/3.14159265358979323846

#define angleconst                       180/128

// Number of AoA reps to run
#define AOA_NUM_REPS(x)                 (AOA_RES_MAX_SIZE / (x * AOA_NUM_SAMPLES_PER_BLOCK))

// CTE RF registers
#define RFC_CTE_MCE_RAM_DATA                           (RFC_RAM_BASE + 0x8000) //0x21008000
#define RFC_CTE_RFE_RAM_DATA                           (RFC_RAM_BASE + 0xC000) //0x2100C000
#define RFC_CTE_LAST_CAPTURE                           (RFC_RAM_BASE + 0x19)
#define RFC_CTE_MCE_RAM_STATE                          (RFC_RAM_BASE + 0x1C)
#define RFC_CTE_MCE_RX_CTEINFO                         (RFC_RAM_BASE + 0x1D)
#define RFC_CTE_MCE_RF_GAIN                            (RFC_RAM_BASE + 0x1E)
#define RFC_CTE_RFE_RAM_STATE                          (RFC_RAM_BASE + 0x20)
#define RFC_CTE_RFE_RX_CTEINFO                         (RFC_RAM_BASE + 0x21)
#define RFC_CTE_RFE_RF_GAIN                            (RFC_RAM_BASE + 0x22)

// CTE RF ram types
#define RFC_CTE_CAPTURE_RAM_MCE                        (0x00)
#define RFC_CTE_CAPTURE_RAM_RFE                        (0x01)
#define RFC_CTE_NO_CAPTURE                             (0xFF)

// CTE RF ram states
#define RFC_CTE_RAM_STATE_EMPTY                        (0x00)
#define RFC_CTE_RAM_STATE_BUSY                         (0x01)
#define RFC_CTE_RAM_STATE_DUAL_BUSY                    (0x02)
#define RFC_CTE_RAM_STATE_READY                        (0x03)
#define RFC_CTE_RAM_STATE_DUAL_READY                   (0x04)

// RF FW write param command type
#define RFC_FWPAR_ADDRESS_TYPE_BYTE                    (0x03)
#define RFC_FWPAR_ADDRESS_TYPE_DWORD                   (0x00)

// RF FW write param command CTE address
#define RFC_FWPAR_CTE_CONFIG                           (187)   // 1 byte
#define RFC_FWPAR_CTE_SAMPLING_CONFIG                  (188)   // 1 byte
#define RFC_FWPAR_CTE_OFFSET                           (189)   // 1 byte
#define RFC_FWPAR_CTE_ANT_SWITCH                       (208)   // 4 bytes
#define RFC_FWPAR_CTE_INFO_TX_TEST                     (159)   // 1 byte

// RF force clock command parameters
#define RFC_FORCE_CLK_DIS_RAM                          (0x0000)
#define RFC_FORCE_CLK_ENA_RAM_MCE                      (0x0010)
#define RFC_FORCE_CLK_ENA_RAM_RFE                      (0x0040)

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * LOCAL VARIABLES
 */

// Pin Handle
PIN_Handle gPinHandle = NULL;

#ifdef RTLS_PASSIVE
AoA_IQSample_Ext_t *gSamplesBuff = 0;
#endif

// Will be equal to 4*cteScanOvs
uint8_t gAoaNumSamplesPerBlock;

// Number of CTE samples
uint16_t gNumCteSamples;

// RF Commands to handle CTE read-out
rfOpCmd_runImmedCmd_t     runFwParCmd;
rfOpCmd_runImmedCmd_t     runEnableRamCmd;
rfOpImmedCmd_ForceClkEnab_t enableRamCmd;

// RF Handle
extern RF_Handle urfiHandle;

// Sample state for passive
AoA_IQSampleState_t gSampleState = SAMPLES_NOT_READY;

/*********************************************************************
 * LOCAL FUNCTIONS
 */
int16_t AOA_iatan2sc(int32_t y, int32_t x);
int32_t AOA_AngleComplexProductComp(int32_t Xre, int32_t Xim, int32_t Yre, int32_t Yim);
bool AOA_initAntArray(uint8_t antArray[], uint8_t antArrLen);
void AOA_rfEnableRam( uint16 selectedRam);
void AOA_getRfIqSamples(RF_Handle rfHandle, RF_CmdHandle cmdHandle, RF_EventMask events);

/*********************************************************************
* @fn      iat2
*
* @brief   Evaluate x,y coordinates
*
* @param   none
*
* @return  result - result of evaluation
*/
static inline int16_t iat2(int32_t y, int32_t x)
{
  return ((y*32+(x/2))/x)*2;  // 3.39 mxdiff
}

/*********************************************************************
* @fn      AOA_iatan2sc
*
* @brief   Evaluate x,y coordinates
*
* @param   none
*
* @return  status - success/fail to open pins
*/
int16_t AOA_iatan2sc(int32_t y, int32_t x)
{
  // determine octant
  if (y >= 0) {   // oct 0,1,2,3
    if (x >= 0) { // oct 0,1
      if (x > y) {
        return iat2(-y, -x)/2 + 0*32;
      } else {
        if (y == 0) return 0; // (x=0,y=0)
        return -iat2(-x, -y)/2 + 2*32;
      }
    } else { // oct 2,3
      // if (-x <= y) {
      if (x >= -y) {
        return iat2(x, -y)/2 + 2*32;
      } else {
        return -iat2(-y, x)/2 + 4*32;
      }
    }
  } else { // oct 4,5,6,7
    if (x < 0) { // oct 4,5
      // if (-x > -y) {
      if (x < y) {
        return iat2(y, x)/2 + -4*32;
      } else {
        return -iat2(x, y)/2 + -2*32;
      }
    } else { // oct 6,7
      // if (x <= -y) {
      if (-x >= y) {
        return iat2(-x, y)/2 + -2*32;
      } else {
        return -iat2(y, -x)/2 + -0*32;
      }
    }
  }
}

/*********************************************************************
* @fn      AOA_angleComplexProductComp
*
* @brief   Example code to process I/Q samples
*
* @param   Xre, Xim, Yre, Yim - real and imaginary coordinates
*
* @return  result - angle*angleconst
*/
int32_t AOA_AngleComplexProductComp(int32_t Xre, int32_t Xim, int32_t Yre, int32_t Yim)
{
  int32_t Zre, Zim;
  int16_t angle;

  // X*conj(Y)
  Zre = Xre*Yre + Xim*Yim;
  Zim = Xim*Yre - Xre*Yim;

  // Angle. The angle is returned in 256/2*pi format [-128,127] values
  angle = AOA_iatan2sc((int32_t) Zim, (int32_t) Zre);

  return (angle * angleconst);
}

/*********************************************************************
* @fn      AOA_initAntArray
*
* @brief   Open pins used for antenna board
*
* @param   antArray - GPIO's representing antennas
* @param   antArrLen - length of antArray
*
* @return  status - success/fail to open pins
*/
bool AOA_initAntArray(uint8_t antArray[], uint8_t antArrLen)
{
  uint32_t pinCfg;
  PIN_State pinState;

  pinCfg = PIN_TERMINATE;

  if (gPinHandle == NULL)
  {
    gPinHandle = PIN_open(&pinState, &pinCfg);
  }

  for (int i = 0; i < antArrLen; i++)
  {
    if (i == 0)
    {
      pinCfg = antArray[i] | PIN_GPIO_OUTPUT_EN | PIN_GPIO_HIGH | PIN_PUSHPULL | PIN_INPUT_DIS | PIN_DRVSTR_MED;
    }
    else
    {
      pinCfg = antArray[i] | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL | PIN_INPUT_DIS | PIN_DRVSTR_MED;
    }

    if (PIN_add(gPinHandle, pinCfg) != PIN_SUCCESS)
    {
      PIN_close(gPinHandle);
      return FALSE;
    }
  }

  return TRUE;
}

/*********************************************************************
* @fn      AOA_init
*
* @brief   Initialize AoA for the defined role
*
* @param   startAntenna - start samples with antenna 1 or 2
*
* @return  none
*/
void AOA_init(uint8_t startAntenna)
{
#ifdef RTLS_PASSIVE
  uint8_t antArray[BOOSTXL_AOA_NUM_ANT + 1] = {ANT2, ANT1, ANT3, ANT_ARRAY};

  // Init only once
  if (gPinHandle == NULL)
  {
    AOA_initAntArray(antArray, BOOSTXL_AOA_NUM_ANT + 1);
  }

    // Initialize ant array switch pin
  if (startAntenna == 2)
  {
    // Start with A2 (ANT_ARRAY pin high is A1, low is A2)
    PINCC26XX_setOutputValue(ANT_ARRAY, 0);
  }
  else
  {
    // Start with A1 (ANT_ARRAY pin high is A1, low is A2)
    PINCC26XX_setOutputValue(ANT_ARRAY, 1);
  }
#endif
}

#ifdef RTLS_PASSIVE
/*********************************************************************
* @fn      AOA_getPairAngles
*
* @brief   Extract results and estimates an angle between two antennas
*
* @param   antConfig - antenna configuration provided from antenna files
* @param   antResult - struct to write results into
*
* @return  none
*/
void AOA_getPairAngles(AoA_AntennaConfig_t *antConfig, AoA_AntennaResult_t *antResult)
{
  const uint16_t numReps = AOA_NUM_REPS(antConfig->numAntennas);
  const uint8_t numAnt = antConfig->numAntennas;
  const uint8_t numPairs = antConfig->numPairs;

  // Average relative angle across repetitions
  int32_t antenna_versus_avg[6][6] = {0};
  int32_t antenna_versus_cnt[6][6] = {0};

  for (uint16_t r = 1; r < numReps; ++r)
  {
    for (uint16_t i = AOA_OFFSET_FIRST_VALID_SAMPLE; i < AOA_NUM_VALID_SAMPLES + AOA_OFFSET_FIRST_VALID_SAMPLE; ++i)
    {
      // Loop through antenna pairs and calculate phase difference
      for (uint8_t pair = 0; pair < numPairs; ++pair)
      {
        const AoA_AntennaPair_t *p = &antConfig->pairs[pair];
        uint8_t a = p->a; // First antenna in pair
        uint8_t b = p->b; // Second antenna in pair

        // Calculate the phase drift across one antenna repetition (X * complex conjugate (Y))
        int16_t Paa_rel = AOA_AngleComplexProductComp(gSamplesBuff[32 + r*numAnt*AOA_NUM_SAMPLES_PER_BLOCK     + a*AOA_NUM_SAMPLES_PER_BLOCK + i].i,
                                                      gSamplesBuff[32 + r*numAnt*AOA_NUM_SAMPLES_PER_BLOCK     + a*AOA_NUM_SAMPLES_PER_BLOCK + i].q,
                                                      gSamplesBuff[32 + (r-1)*numAnt*AOA_NUM_SAMPLES_PER_BLOCK + a*AOA_NUM_SAMPLES_PER_BLOCK + i].i,
                                                      gSamplesBuff[32 + (r-1)*numAnt*AOA_NUM_SAMPLES_PER_BLOCK + a*AOA_NUM_SAMPLES_PER_BLOCK + i].q);

        // Calculate phase difference between antenna a vs. antenna b
        int16_t Pab_rel = AOA_AngleComplexProductComp(gSamplesBuff[32 + r*numAnt*AOA_NUM_SAMPLES_PER_BLOCK + a*AOA_NUM_SAMPLES_PER_BLOCK + i].i,
                                                      gSamplesBuff[32 + r*numAnt*AOA_NUM_SAMPLES_PER_BLOCK + a*AOA_NUM_SAMPLES_PER_BLOCK + i].q,
                                                      gSamplesBuff[32 + r*numAnt*AOA_NUM_SAMPLES_PER_BLOCK + b*AOA_NUM_SAMPLES_PER_BLOCK + i].i,
                                                      gSamplesBuff[32 + r*numAnt*AOA_NUM_SAMPLES_PER_BLOCK + b*AOA_NUM_SAMPLES_PER_BLOCK + i].q);

        // Add to averages
        // v-- Correct for angle drift / ADC sampling frequency error
        antenna_versus_avg[a][b] += Pab_rel + ((Paa_rel * abs(a-b)) / numAnt);
        antenna_versus_cnt[a][b] ++;
      }
    }
  }

  // Calculate the average relative angles
  for (int i = 0; i < numAnt; ++i)
  {
    for (int j = 0; j < numAnt; ++j)
    {
      antenna_versus_avg[i][j] /= antenna_versus_cnt[i][j];
    }
  }

  // Write back result for antenna pairs
  for (uint8_t pair = 0; pair < numPairs; ++pair)
  {
    AoA_AntennaPair_t *p = &antConfig->pairs[pair];
    antResult->pairAngle[pair] = (int)((p->sign * antenna_versus_avg[p->a][p->b] + p->offset) * p->gain);
  }
}
#elif RTLS_MASTER
/*********************************************************************
* @fn      AOA_getPairAngles
*
* @brief   Extract results and estimates an angle between two antennas
*
* @param   antConfig - antenna configuration provided from antenna files
* @param   antResult - struct to write results into
* @param   numIqSamples - number of I and Q samples
* @param   sampleRate - sample rate that was used to capture (1,2,3 or 4 Mhz)
* @param   sampleSize - sample size 1 = 8 bit, 2 = 16 bit
* @param   numAnt - number of antennas in capture array
* @param   pIQ - pointer to IQ samples
*
* @return  none
*/
void AOA_getPairAngles(AoA_AntennaConfig_t *antConfig, AoA_AntennaResult_t *antResult, uint16_t numIqSamples, uint8_t sampleRate, uint8_t sampleSize, uint8_t slotDuration, uint8_t numAnt, int8_t *pIQ)
{
  // IQ Samples can be 32 bit per sample or 16 bit per sample (where each sample is normalized to 8 bits)
  AoA_IQSample_Ext_t *pIQExt;
  AoA_IQSample_t *pIQNorm;

  int16_t Paa_rel;
  int16_t Pab_rel;
  int8_t  secondIQfactor = 1; // in slot duration of 1 usec, there are 180 degrees between adjacent antennas samples so the factor will be -1

  uint8_t numPairs = antConfig->numPairs;

  // Average relative angle across repetitions
  int32_t antenna_versus_avg[6][6] = {0};
  int32_t antenna_versus_cnt[6][6] = {0};

  for (uint16_t r = 1; r < (numIqSamples - (AOA_OFFSET_FIRST_VALID_SAMPLE*sampleRate))/(numAnt*sampleRate) ; ++r) // Sample Slot
  {
    for (uint16_t i = 0; i < sampleRate; ++i) // Sample inside Sample Slot
    {
      // Loop through antenna pairs and calculate phase difference
      for (uint8_t pair = 0; pair < numPairs; ++pair)
      {
        const AoA_AntennaPair_t *p = &antConfig->pairs[pair];
        uint8_t a = p->a; // First antenna in pair
        uint8_t b = p->b; // Second antenna in pair

        // In slot duration of 1 usec, there are 180 degrees between adjacent antennas samples
        if (slotDuration == AOA_SLOT_DURATION_1US)
        {
          if ((abs(a-b) % 2) == 1)
          {
            secondIQfactor = -1; // in slot duration of 1 usec, there are 180 degrees between adjacent antennas samples,
                                 // because the antenna switch is in the middle of the sine wave period
          }
        }

        if (sampleSize == 1)
        {
          pIQNorm = (AoA_IQSample_t *)pIQ;

          // Calculate the phase drift across one antenna repetition (X * complex conjugate (Y))
          Paa_rel = AOA_AngleComplexProductComp(pIQNorm[AOA_OFFSET_FIRST_VALID_SAMPLE*sampleRate + r*numAnt*sampleRate + a*sampleRate + i].i,
                                                pIQNorm[AOA_OFFSET_FIRST_VALID_SAMPLE*sampleRate + r*numAnt*sampleRate + a*sampleRate + i].q,
                                                pIQNorm[AOA_OFFSET_FIRST_VALID_SAMPLE*sampleRate + (r-1)*numAnt*sampleRate + a*sampleRate + i].i * secondIQfactor,
                                                pIQNorm[AOA_OFFSET_FIRST_VALID_SAMPLE*sampleRate + (r-1)*numAnt*sampleRate + a*sampleRate + i].q * secondIQfactor);

          // Calculate phase difference between antenna a vs. antenna b
          Pab_rel = AOA_AngleComplexProductComp(pIQNorm[AOA_OFFSET_FIRST_VALID_SAMPLE*sampleRate + r*numAnt*sampleRate + a*sampleRate + i].i,
                                                pIQNorm[AOA_OFFSET_FIRST_VALID_SAMPLE*sampleRate + r*numAnt*sampleRate + a*sampleRate + i].q,
                                                pIQNorm[AOA_OFFSET_FIRST_VALID_SAMPLE*sampleRate + r*numAnt*sampleRate + b*sampleRate + i].i * secondIQfactor,
                                                pIQNorm[AOA_OFFSET_FIRST_VALID_SAMPLE*sampleRate + r*numAnt*sampleRate + b*sampleRate + i].q * secondIQfactor);
        }
        else // sampleSize == 2
        {
          pIQExt = (AoA_IQSample_Ext_t *)pIQ;

          // Calculate the phase drift across one antenna repetition (X * complex conjugate (Y))
          Paa_rel = AOA_AngleComplexProductComp(pIQExt[AOA_OFFSET_FIRST_VALID_SAMPLE*sampleRate + r*numAnt*sampleRate + a*sampleRate + i].i,
                                                pIQExt[AOA_OFFSET_FIRST_VALID_SAMPLE*sampleRate + r*numAnt*sampleRate + a*sampleRate + i].q,
                                                pIQExt[AOA_OFFSET_FIRST_VALID_SAMPLE*sampleRate + (r-1)*numAnt*sampleRate + a*sampleRate + i].i * secondIQfactor,
                                                pIQExt[AOA_OFFSET_FIRST_VALID_SAMPLE*sampleRate + (r-1)*numAnt*sampleRate + a*sampleRate + i].q * secondIQfactor);

          // Calculate phase difference between antenna a vs. antenna b
          Pab_rel = AOA_AngleComplexProductComp(pIQExt[AOA_OFFSET_FIRST_VALID_SAMPLE*sampleRate + r*numAnt*sampleRate + a*sampleRate + i].i,
                                                pIQExt[AOA_OFFSET_FIRST_VALID_SAMPLE*sampleRate + r*numAnt*sampleRate + a*sampleRate + i].q,
                                                pIQExt[AOA_OFFSET_FIRST_VALID_SAMPLE*sampleRate + r*numAnt*sampleRate + b*sampleRate + i].i * secondIQfactor,
                                                pIQExt[AOA_OFFSET_FIRST_VALID_SAMPLE*sampleRate + r*numAnt*sampleRate + b*sampleRate + i].q * secondIQfactor);
        }


        // Add to averages
        // v-- Correct for angle drift / ADC sampling frequency error
        antenna_versus_avg[a][b] += Pab_rel + ((Paa_rel * abs(a-b)) / numAnt);
        antenna_versus_cnt[a][b] ++;
      }
    }
  }

  // Calculate the average relative angles
  for (int i = 0; i < numAnt; ++i)
  {
    for (int j = 0; j < numAnt; ++j)
    {
      antenna_versus_avg[i][j] /= antenna_versus_cnt[i][j];
    }
  }

  // Write back result for antenna pairs
  for (uint8_t pair = 0; pair < numPairs; ++pair)
  {
    AoA_AntennaPair_t *p = &antConfig->pairs[pair];
    antResult->pairAngle[pair] = (int)((p->sign * antenna_versus_avg[p->a][p->b] + p->offset) * p->gain);
  }
}
#endif

/*********************************************************************
* @fn      AOA_postProcess
*
* @brief   This function will update the final result report with rssi and channel
*          For RTLS Passive it will also send a command to enable RF RAM so we can read
*          the samples that we captured (if any)
*
* @param   rssi - rssi to be stamped on final result
* @param   channel - channel to be stamped on final result
* @param   samplesBuff - buffer to copy samples to
*
*/
void AOA_postProcess(int8_t rssi, uint8_t channel, AoA_IQSample_Ext_t *samplesBuff)
{
#ifdef RTLS_PASSIVE
  gSamplesBuff = samplesBuff;
  AOA_rfEnableRam(RFC_FORCE_CLK_ENA_RAM_RFE);
#endif
}

#ifdef RTLS_PASSIVE
/*********************************************************************
* @fn      AOA_getRawSamples
*
* @brief   Returns pointer to raw I/Q samples
*
* @param   none
*
* @return  Pointer to raw I/Q samples
*/
AoA_IQSample_Ext_t * AOA_getRawSamples(void)
{
  return gSamplesBuff;
}
#endif

/*******************************************************************************
 * @fn          AOA_getSampleState
 *
 * @brief       Used to check if samples are ready
 *
 * input parameters
 *
 * @param       none
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      State of samples
 */
AoA_IQSampleState_t AOA_getSampleState(void)
{
  return gSampleState;
}

#ifdef RTLS_PASSIVE
/*******************************************************************************
 * @fn          AOA_rfEnableRam
 *
 * @brief       This function is used to force enable or disable MCE RAM, RFE RAM or both
 *
 * input parameters
 *
 * @param       selectedRam - as the following bitmask values:
 *              RFC_FORCE_CLK_ENA_RAM_MCE - to enable MCE ram
 *              RFC_FORCE_CLK_ENA_RAM_RFE - to enable RFE ram
 *              or
 *              RFC_FORCE_CLK_DIS_RAM - to disable
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      None
 */
void AOA_rfEnableRam(uint16 selectedRam)
{
  RF_ScheduleCmdParams cmdParams = {
    0,
    RF_StartNotSpecified,
    RF_AllowDelayAny,
    0,
    RF_EndNotSpecified,
    0,
    0,
    RF_PriorityCoexDefault
  };

  enableRamCmd.cmdNum = CMD_FORCE_CLK_ENA;
  enableRamCmd.clkEnab = selectedRam;

  // setup radio command to run an immediate command
  runEnableRamCmd.rfOpCmd.cmdNum    = CMD_RUN_IMMEDIATE_COMMAND;
  runEnableRamCmd.rfOpCmd.status    = RFSTAT_IDLE;
  runEnableRamCmd.rfOpCmd.pNextRfOp = (rfOpCmd_t *)NULL;
  runEnableRamCmd.rfOpCmd.startTime = 0;
  runEnableRamCmd.rfOpCmd.startTrig = TRIGTYPE_NOW;
  runEnableRamCmd.rfOpCmd.condition = CONDTYPE_ALWAYS_RUN_NEXT_CMD;
  runEnableRamCmd.reserved          = 0;
  runEnableRamCmd.cmdVal            = (uint32)&enableRamCmd;
  runEnableRamCmd.cmdStatVal        = 0;

  RF_scheduleCmd( urfiHandle,
                 (RF_Op *)&runEnableRamCmd,
                 &cmdParams,
                 (RF_Callback)AOA_getRfIqSamples,
                 RF_EventCmdDone | RF_EventInternalError );
}

/*******************************************************************************
 * @fn          AOA_getRfIqSamples
 *
 * @brief       This callback is used to get the IQ samples from RF RAM
 *
 * input parameters
 *
 * @param       rfHandle - RF Handle
 *              cmdHandle - Cmd Handle
 *              events - RF Event
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      None
 */
void AOA_getRfIqSamples(RF_Handle rfHandle, RF_CmdHandle cmdHandle, RF_EventMask events)
{
  uint8_t state = 0;
  uint8_t lastCapture;
  uint32_t *cteData;
  uint32_t *extCteData = NULL;

  gSampleState = SAMPLES_NOT_READY;

  if (events & RF_EventCmdDone)
  {
    lastCapture = HWREGB(RFC_CTE_LAST_CAPTURE);
    // check which RAM is capturing samples
    if ((lastCapture == RFC_CTE_CAPTURE_RAM_MCE) || (lastCapture == RFC_CTE_CAPTURE_RAM_RFE))
    {
      // wait while buffer is busy
      do
      {
        state = (lastCapture == RFC_CTE_CAPTURE_RAM_MCE)?HWREGB(RFC_CTE_MCE_RAM_STATE):HWREGB(RFC_CTE_RFE_RAM_STATE);
      } while ((state == RFC_CTE_RAM_STATE_BUSY) || (state == RFC_CTE_RAM_STATE_DUAL_BUSY));

      // check if buffer is ready for reading
      if ((state == RFC_CTE_RAM_STATE_READY) || (state == RFC_CTE_RAM_STATE_DUAL_READY))
      {
        if (lastCapture == RFC_CTE_CAPTURE_RAM_MCE)
        {
          // get the MCE buffer address
          cteData = (uint32_t *)RFC_CTE_MCE_RAM_DATA;
          extCteData = (uint32_t *)RFC_CTE_RFE_RAM_DATA;
        }
        else
        {
          // get the RFE buffer address
          cteData = (uint32_t *)RFC_CTE_RFE_RAM_DATA;
        }

        // This will trigger the upper layer which polls samplesReady
        if (cteData != NULL && extCteData == NULL)
        {
          memcpy(gSamplesBuff, cteData, AOA_RES_MAX_SIZE * sizeof(AoA_IQSample_Ext_t));
          gSampleState = SAMPLES_READY;
        }
      }
    }
  }

  // release the RAM so it should be available for next CTE
  HWREGB(RFC_CTE_MCE_RAM_STATE) = RFC_CTE_RAM_STATE_EMPTY;
  HWREGB(RFC_CTE_RFE_RAM_STATE) = RFC_CTE_RAM_STATE_EMPTY;

  // If we don't have samples at this point then they are not valid
  if (gSampleState != SAMPLES_READY)
  {
    gSampleState = SAMPLES_NOT_VALID;
  }
}
#endif // RTLS_PASSIVE
