/******************************************************************************

 @file  ant_array2_config_boostxl_rev1v1.c

 @brief This file contains the antenna array tables for
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

#include "ant_array2_config_boostxl_rev1v1.h"

AoA_AntennaConfig_t BOOSTXL_AoA_Config_ArrayA2;

AoA_AntennaPair_t pair_A2[] =
{
   {// v12
    .a = 0,         // First antenna in pair
    .b = 1,         // Second antenna in pair
    .sign = 1,     // Sign for the result
    .offset = 10,  // Measurement offset compensation
    .gain = 0.95,   // Measurement gain compensation
   },
   {// v23
    .a = 1,
    .b = 2,
    .sign = 1,
    .offset = -5,
    .gain = 0.9,
   },
   {// v13
    .a = 0,
    .b = 2,
    .sign = 1,
    .offset = 20,
    .gain = 0.50,
   },
};

AoA_AntennaConfig_t BOOSTXL_AoA_Config_ArrayA2 =
{
 .numAntennas = BOOSTXL_AOA_NUM_ANT,
 .numPairs = sizeof(pair_A2) / sizeof(pair_A2[0]),
 .pairs = pair_A2
};

// Channel offset compensation array.
// This is one point compensation for variation over frequency
// Compensation values are found when incoming signal is coming straight at antenna array 1 (0 degree to antenna array 1)
// Better accuracy and linearity can be achieved by adding compensation values for more angles
int8_t channelOffset_A2[40] = {0, // Channel 0
                               1, // Channel 1
                               1, // Channel 2
                               0, // Channel 3
                               1, // Channel 4
                               1, // Channel 5
                               1, // Channel 6
                               1, // Channel 7
                               1, // Channel 8
                               2, // Channel 9
                               2, // Channel 10
                               4, // Channel 11
                              -4, // Channel 12
                               3, // Channel 13
                               3, // Channel 14
                               4, // Channel 15
                               3, // Channel 16
                               4, // Channel 17
                               2, // Channel 17
                               2, // Channel 18
                               1, // Channel 20
                               1, // Channel 21
                               1, // Channel 22
                               0, // Channel 23
                               1, // Channel 24
                               1, // Channel 25
                               1, // Channel 26
                               1, // Channel 27
                               0, // Channel 28
                               0, // Channel 29
                               0, // Channel 30
                               0, // Channel 31
                               0, // Channel 32
                               0, // Channel 33
                               0, // Channel 34
                               1, // Channel 35
                               1, // Channel 36
                               0, // Channel 37
                               0, // Channel 38
                               0, // Channel 39
                               };

AoA_AntennaConfig_t *getAntennaArray2Config(void)
{
  return &BOOSTXL_AoA_Config_ArrayA2;
}

int8_t *getAntennaArray2ChannelOffsets(void)
{
  return channelOffset_A2;
}
