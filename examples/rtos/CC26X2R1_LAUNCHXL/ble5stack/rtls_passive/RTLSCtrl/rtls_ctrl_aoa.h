/******************************************************************************

 @file  rtls_ctrl_aoa.h

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

/**
 *  @defgroup RTLS_CTRL_AOA RTLS_CTRL_AOA
 *  @brief This module implements Real Time Localization System (RTLS)
 *         AoA post processing module
 *
 *  @{
 *  @file  rtls_ctrl_aoa.h
 *  @brief      This file contains the functions and structures specific to AoA
 */

#ifndef RTLS_CTRL_AOA_H_
#define RTLS_CTRL_AOA_H_

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */

#include "rtls_aoa_api.h"
#include "AOA.h"
#include "rtls_ctrl_api.h"
#include "rtls_ctrl.h"

/*********************************************************************
*  EXTERNAL VARIABLES
*/

/*********************************************************************
 * CONSTANTS
 */

#define MAX_SAMPLES_SINGLE_CHUNK 32    //!< Max number of samples reported in a single chunk when using RAW mode

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * TYPEDEFS
 */

/** @defgroup RTLS_CTRL_Structs RTLS Control Structures
 * @{
 */

/// @brief Enumeration for AOA Results modes
typedef enum
{
  AOA_MODE_ANGLE,
  AOA_MODE_PAIR_ANGLES,
  AOA_MODE_RAW
} aoaResultMode_e;

// AoA Parameters - Received from RTLS Node Manager
/// @brief List of AoA parameters
typedef struct __attribute__((packed))
{
  AoA_Role_t aoaRole;         //!< AOA_MASTER, AOA_SLAVE, AOA_PASSIVE
  aoaResultMode_e resultMode; //!< AOA_MODE_ANGLE, AOD_MODE_PAIR_ANGLES, AOA_MODE_RAW
  rtlsAoaConfigReq_t config;  //!< Configuration that will be passed to RTLS Application
} rtlsAoaParams_t;

/// @brief AoA Angle Result
typedef struct __attribute__((packed))
{
  uint16_t connHandle;       //!< Connection handle
  int16_t angle;             //!< AoA Angle Result
  int8_t  rssi;              //!< RSSI for the reported samples
  uint8_t antenna;           //!< Antenna the rssi was taken on
  uint8_t channel;           //!< The channel the samples were taken on
} rtlsAoaResultAngle_t;

/// @brief AoA Antenna Pairs Result
typedef struct __attribute__((packed))
{
  uint16_t connHandle;                                          //!< Connection handle
  int8_t  rssi;                                                 //!< RSSI for the reported samples
  uint8_t antenna;                                              //!< Antenna the rssi was taken on
  uint8_t channel;                                              //!< The channel the samples were taken on
  int16_t pairAngle[CALC_NUM_ANT_PAIRS(BOOSTXL_AOA_NUM_ANT)];   //!< AoA Antenna Pairs Result
} rtlsAoaResultPairAngles_t;

/// @brief AoA Raw Result
typedef struct __attribute__((packed))
{
  uint16_t connHandle;          //!< Connection handle
  int8_t  rssi;                 //!< Rssi for this antenna
  uint8_t antenna;              //!< Antenna array used for this result
  uint8_t channel;              //!< BLE data channel for this measurement
  uint16_t offset;              //!< Offset in RAW result (size of samples[])
  uint16_t samplesLength;       //!< Expected length of entire RAW sample
  AoA_IQSample_Ext_t samples[]; //!< The data itself
} rtlsAoaResultRaw_t;

// AoA post process event
typedef struct
{
  uint16_t connHandle;         //!< Connection handle
  int8_t rssi;                 //!< rssi for this CTE
  uint8_t channel;             //!< channel this CTE was captured on
  uint16_t numIqSamples;       //!< Number of samples
  uint8_t sampleRate;          //!< Sampling rate that was used for the run
  uint8_t sampleSize;          //!< Sample size 1 = 8 bit, 2 = 16 bit
  uint8_t sampleCtrl;          //!< 1 = RAW RF, 0 = Filtered results (switching period omitted)
  uint8_t slotDuration;        //!< Duration 1 = 1us, 2 = 2us
  uint8_t numAnt;              //!< Number of Antennas that were used for the run
  int8_t *pIQ;                 //!< Pointer to IQ samples
} rtlsAoaIqEvt_t;

typedef struct
{
  AoA_Role_t aoaRole;          //!< AOA_MASTER, AOA_SLAVE, AOA_PASSIVE
  aoaResultMode_e resultMode;  //!< AOA_MODE_ANGLE, AOD_MODE_PAIR_ANGLES, AOA_MODE_RAW
  uint8_t sampleCtrl;          //!< 0x01 = RAW RF, 0x00 = Filtered results (switching period omitted), bit 4,5 0x10 - ONLY_ANT_1, 0x20 - ONLY_ANT_2
} rtlsAoa_t;
/** @} End RTLS_CTRL_Structs */

/*********************************************************************
 * API FUNCTIONS
 */

#ifndef RTLS_PASSIVE

/**
* @brief   Called at the end of each connection event to extract I/Q samples
*
* @param   pEvt - Pointer to IQ Event
*
* @return  none
*/
void RTLSCtrl_postProcessAoa(rtlsAoaIqEvt_t *pEvt);
#else

/**
* @brief   Called at the end of each connection event to extract I/Q samples
*
* @param   connHandle - connection handle
* @param   resultMode - AoA information saved by RTLS Control
* @param   rssi - rssi to be reported to RTLS Host
* @param   channel - channel that was used for this AoA run
* @param   sampleCtrl - sample control configs: 0x01 = RAW RF, 0x00 = Filtered results (switching period omitted), bit 4,5 0x10 - ONLY_ANT_1, 0x20 - ONLY_ANT_2
*
* @return  none
*/
void RTLSCtrl_postProcessAoa(uint8_t connHandle, uint8_t resultMode, int8_t rssi, uint8_t channel, uint8_t sampleCtrl);
#endif

/**
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
rtlsStatus_e RTLSCtrl_initAoa(uint8_t maxConnections, uint8_t sampleCtrl, uint8_t numAnt, uint8_t *pAntPattern, aoaResultMode_e resultMode);

/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* RTLS_CTRL_AOA_H_ */

/** @} End RTLS_CTRL */
