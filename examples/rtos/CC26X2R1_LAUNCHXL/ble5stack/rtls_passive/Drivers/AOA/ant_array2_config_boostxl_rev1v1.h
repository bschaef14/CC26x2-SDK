/******************************************************************************

 @file  ant_array2_config_boostxl_rev1v1.h

 @brief This file contains the antenna array definitions and prototypes for
        Angle of Arrival feature.

 Group: WCS, BTS
 Target Device: cc13x2_26x2

 ******************************************************************************
 
 Copyright (c) 2018-2020, Texas Instruments Incorporated
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

#ifndef ANT_ARRAY2_H_
#define ANT_ARRAY2_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include <stdlib.h>

#define ANT_ARRAY_A1x 1
#define ANT_ARRAY_A2x 2
#define BOOSTXL_AOA_NUM_ANT 3

#define CALC_NUM_ANT_PAIRS(numAnt) ((1 + (numAnt - 1)) * (numAnt - 1)/2)

/// @brief Antenna Pair Structure
typedef struct
{
  uint8_t a ;    //!< First antenna in pair
  uint8_t b ;    //!< Second antenna in pair
  float d;       //!< Variable used in antenna pairs
  int8_t sign;   //!< Sign for the result
  int8_t offset; //!< Measurement offset compensation
  float gain;    //!< Measurement gain compensation
} AoA_AntennaPair_t;

/// @brief Antenna Configurations structure
typedef struct
{
  uint8_t numAntennas;      //!< Number of antennas
  uint8_t numPairs;         //!< Number of antenna pairs
  AoA_AntennaPair_t *pairs; //!< antenna pair information array
  int8_t *channelOffset;    //!< RF Channel offset
} AoA_AntennaConfig_t;

AoA_AntennaConfig_t *getAntennaArray2Config(void);
int8_t *getAntennaArray2ChannelOffsets(void);

#ifdef __cplusplus
}
#endif

#endif /* ANT_ARRAY2_H_ */
