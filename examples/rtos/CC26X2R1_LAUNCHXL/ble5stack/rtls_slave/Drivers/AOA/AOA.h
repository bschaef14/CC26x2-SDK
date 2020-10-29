/******************************************************************************

 @file  AOA.h

 @brief This file contains typedefs and API functions of AOA.c
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
 *  @defgroup AOA AOA
 *  @brief This module implements the Angle of Arrival (AOA)
 *
 *  @{
 *  @file  AOA.h
 *  @brief      AOA interface
 */

#ifndef AOA_H_
#define AOA_H_

#ifdef __cplusplus
extern "C"
{
#endif

/*******************************************************************************
 * INCLUDES
 */

#include <stdint.h>
#include <driverlib/ioc.h>
#include "ant_array2_config_boostxl_rev1v1.h"

/*********************************************************************
 * CONSTANTS
 */

/// @brief Relevent only for RTLS Passive
#define AOA_RES_MAX_SIZE                 512       //!< Data Size at maximum resolution
#define AOA_RES_MAX_CTE_TIME             20        //!< CTE Time at maximum resolution

/*********************************************************************
 * MACROS
 */

/// @brief Relevant only for RTLS Passive
#define AOA_PIN(x)                       (1 << (x&0xff))

// Antenna configuration for BOOSTXL-AOA
#define ANT_ARRAY (27)
#define ANT1      (28)
#define ANT2      (29)
#define ANT3      (30)

/*********************************************************************
 * TYPEDEFS
 */

/** @defgroup AOA_Structs AOA Structures
 * @{
 */

/// @brief AoA Device Role
typedef enum
{
  AOA_ROLE_SLAVE,    //!< Transmitter Role
  AOA_ROLE_MASTER,   //!< Receiver Role
  AOA_ROLE_PASSIVE   //!< Passive Role
} AoA_Role_t;

/// @brief AoA result per antenna array
typedef struct
{
  int16_t *pairAngle;       //!< Antenna pair angle
  int8_t rssi;              //!< Last Rx rssi
  uint8_t ch;               //!< Channel
} AoA_AntennaResult_t;

/// @brief 32 bit IQ Sample structure
typedef struct
{
  // Note that Passive takes the samples directly from RF core RAM
  // Hence the format is different than the Master
#ifdef RTLS_PASSIVE
  int16_t q;  //!< Q - Quadrature
  int16_t i;  //!< I - In-phase
#else
  int16_t i;  //!< I - In-phase
  int16_t q;  //!< Q - Quadrature
#endif
} AoA_IQSample_Ext_t;

/// @brief IQ Sample structure
typedef struct
{
  int8_t i;  //!< I - In-phase
  int8_t q;  //!< Q - Quadrature
} AoA_IQSample_t;

/** @} End AOA_Structs */

/// @brief IQ Sample state - relevant for Passive
typedef enum
{
  SAMPLES_NOT_READY,
  SAMPLES_NOT_VALID,
  SAMPLES_READY
} AoA_IQSampleState_t;

/*********************************************************************
 * EXTERNAL VARIABLES
 */

#ifdef RTLS_PASSIVE
/**
* @brief   Extract results and estimates an angle between two antennas
*
* @return  none
*/

void AOA_getPairAngles(AoA_AntennaConfig_t *antConfig, AoA_AntennaResult_t *antResult);
#elif RTLS_MASTER
/***
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
void AOA_getPairAngles(AoA_AntennaConfig_t *antConfig, AoA_AntennaResult_t *antResult, uint16_t numIqSamples, uint8_t sampleRate, uint8_t sampleSize, uint8_t slotDuration, uint8_t numAnt, int8_t *pIQ);
#endif
/**
* @brief   Initialize AoA for the defined role
*
* @param   startAntenna - start samples with antenna 1 or 2
*
* @return  none
*/
void AOA_init(uint8_t startAntenna);

/**
* @brief   This function enables the CTE capture in the rf core
*
* @param   cteTime   -  CTETime parameter defined in spec
* @param   cteScanOvs - used to enable CTE capturing and set the
*                       sampling rate in the IQ buffer
* @param   cteOffset -  number of microseconds from the beginning
*                       of the tone until the sampling starts
*
* @return  None
*/
void AOA_cteCapEnable(uint8_t cteTime, uint8_t cteScanOvs, uint8_t cteOffset);

/**
* @brief   This function calculate the number of IQ samples based
*          on the cte parameters from the CTEInfo header and our
*          patch params
*
* @param   cteTime - CTEInfo parameter defined in spec
* @param   cteScanOvs - used to enable CTE capturing and set the
*                       sampling rate in the IQ buffer
* @param   cteOffset - number of microseconds from the beginning
*                       of the tone until the sampling starts
*
* @return  uint16_t - The number of IQ samples to process
*/
uint16_t AOA_calcNumOfCteSamples(uint8_t cteTime, uint8_t cteScanOvs, uint8_t cteOffset);

/**
* @brief   This function will update the final result report with rssi and channel
*          For RTLS Passive it will also send a command to enable RF RAM so we can read
*          the samples that we captured (if any)
*
* @param   rssi - rssi to be stamped on final result
* @param   channel - channel to be stamped on final result
* @param   samplesBuff - buffer to copy samples to
*
*/
void AOA_postProcess(int8_t rssi, uint8_t channel, AoA_IQSample_Ext_t *samplesBuff);

#ifdef RTLS_PASSIVE
/**
* @brief   Returns pointer to raw I/Q samples
*
* @return  Pointer to raw I/Q samples
*/
AoA_IQSample_Ext_t *AOA_getRawSamples(void);
#endif
/**
* @brief   Returns active antenna id
*
* @return  Pointer to raw I/Q samples
*/
uint8_t AOA_getActiveAnt(void);

/**
* @brief   Returns active antenna id
*
* @return  Pointer to Sample State
*/
AoA_IQSampleState_t AOA_getSampleState(void);
/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* AOA_H_ */

/** @} End AOA */
