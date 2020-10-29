/******************************************************************************

 @file  light_profile.h

 This file contains the Light profile definitions and prototypes.

 Group: WCS, BTS
 Target Device: cc13x2_26x2

 ******************************************************************************
 
 Copyright (c) 2009-2020, Texas Instruments Incorporated
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

#ifndef LIGHTPROFILE_H
#define LIGHTPROFILE_H

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */

#include "sensor_common.h"

/*********************************************************************
 * FUNCTIONS
 */

/**
 * Initialize the module
 *
 * Register with common sensors module, add the Humidity GATT service,
 * and initialize the periodic light sensor readings
 *
 * @return bleMemAllocError not enough heap to continue
 * @return bleAlreadyInRequestedMode Already initialized
 * @return SUCCESS
 */
extern bStatus_t LightProfile_init(I2C_Handle *pHandle);

/**
 * Enable the Light Profile.
 *
 * This will start the periodic light sensor readings
 *
 * @return SUCCESS Periodic readings started.
 * @return FAILURE OPT3001 failed to open
 */
extern bStatus_t LightProfile_enable();

/**
 * Disable the Light Profile.
 *
 * This will stop the periodic light sensor readings
 *
 * @return SUCCESS
 * @return FAILURE OPT3001 failed to close
 */
extern bStatus_t LightProfile_disable();

#ifdef __cplusplus
}
#endif

#endif /* LIGHTPROFILE_H */
