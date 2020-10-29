/******************************************************************************
 Group: CMCU LPRF
 Target Device: cc13x2_26x2

 ******************************************************************************
 
 Copyright (c) 2019-2020, Texas Instruments Incorporated
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

/*!

@file NodeStorage.c

@brief

*/

/***** Includes *****/

#include "NodeStorage.h"

/* Standard C Libraries */
#include <string.h>

/* Board Header files */
#include "ti_drivers_config.h"


/***** Defines *****/

#define NV_DATA_START_SEQ_LEN    (16U)


/***** Variable Declarations *****/

/* NVS Driver */
static NVS_Handle nvsHandle = NULL;
static NVS_Params nvsParams;
static NVS_Attrs  regionAttrs;

/* Sequence signifying the start of valid connection data in NVS region */
static const char *nvDataStartSeq = "EZ_WSN_CONN_DATA";


/***** Function definitions *****/

NodeStorage_Status NodeStorage_init(void)
{
    if (nvsHandle == NULL)
    {
        NVS_init();
        NVS_Params_init(&nvsParams);
        nvsHandle = NVS_open(Board_NVSINTERNAL0, &nvsParams);

        if (nvsHandle == NULL) {
            return NodeStorage_InitError;
        }

        NVS_getAttrs(nvsHandle, &regionAttrs);
    }

    return NodeStorage_Success;
}

NodeStorage_Status NodeStorage_read(size_t offset, void *buff, size_t len)
{
    if (nvsHandle == NULL)
    {
        return NodeStorage_Error;
    }

    int_fast16_t status = NVS_read(nvsHandle, offset, buff, len);

    if (status != NVS_STATUS_SUCCESS)
    {
        return NodeStorage_ReadError;
    }

    return NodeStorage_Success;
}

NodeStorage_Status NodeStorage_write(size_t offset, void *buff, size_t len)
{
    if (nvsHandle == NULL)
    {
        return NodeStorage_Error;
    }

    int_fast16_t status = NVS_write(nvsHandle, offset, buff, len,
        NVS_WRITE_POST_VERIFY);

    if (status != NVS_STATUS_SUCCESS)
    {
        return NodeStorage_WriteError;
    }

    return NodeStorage_Success;
}

NodeStorage_Status NodeStorage_writeStartSeq(void)
{
    return NodeStorage_write(0, (void *)nvDataStartSeq, NV_DATA_START_SEQ_LEN);
}

NodeStorage_Status NodeStorage_eraseAll(void)
{
    if (nvsHandle == NULL)
    {
        return NodeStorage_Error;
    }

    int_fast16_t status = NVS_erase(nvsHandle, 0, regionAttrs.sectorSize);

    if (status != NVS_STATUS_SUCCESS)
    {
        return NodeStorage_WriteError;
    }

    return NodeStorage_Success;
}

bool NodeStorage_verifyStorage(void)
{
    if (nvsHandle == NULL)
    {
        return false;
    }

    uint8_t buffer[NV_DATA_START_SEQ_LEN + 1] = {0};

    NVS_read(nvsHandle, 0, (void *) buffer, NV_DATA_START_SEQ_LEN);

    /*
     * Compare the data read from NV to the nvDataStartSeq, if they match then
     * return true
     */
    if (strcmp((char *) buffer, (char *) nvDataStartSeq) == 0)
    {
        return true;
    }

    return false;
}
