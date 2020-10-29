/*
 * Copyright (c) 2019, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef NODESTORAGE_H_
#define NODESTORAGE_H_

/***** Includes *****/

/* Standard C Libraries */
#include <stdbool.h>
#include <stddef.h>

/* TI Drivers */
#include <ti/drivers/NVS.h>


/***** Defines *****/

#define NODESTORAGE_NODE_ADDR_OFFSET    (16U)
#define NODESTORAGE_SERVER_ADDR_OFFSET  (17U)

/***** Enums *****/

typedef enum
{
    NodeStorage_Success = 0,
    NodeStorage_Error,
    NodeStorage_InitError,
    NodeStorage_ReadError,
    NodeStorage_WriteError,

} NodeStorage_Status;


/***** Function Definitions *****/

/*!
 *  @brief   Initialize NV storage variables
 *
 *  @return  NodeStorage_Success    NVS init successful
 *  @return  NodeStorage_InitError  NVS init failed
 */
extern NodeStorage_Status NodeStorage_init(void);

/*!
 *  @brief   Read data from the Node's NV storage
 *
 *  @param   offset    The byte offset into the NVS region to start
 *                     reading from
 *
 *  @param   buff      A buffer to copy the data to
 *
 *  @param   len       Number of bytes to read from NVS
 *
 *  @return  NodeStorage_Success    Read successful
 *  @return  NodeStorage_ReadError  Read failed
 *  @return  NodeStorage_Error      NodeStorage_init() not successfully called
 */
extern NodeStorage_Status NodeStorage_read(size_t offset, void *buff, size_t len);

/*!
 *  @brief   Write data to the Node's NV storage
 *
 *  @param   offset    The byte offset into the NVS region to start
 *                     writing to
 *
 *  @param   buff      A buffer to read the data from
 *
 *  @param   len       Number of bytes to read from the buffer
 *
 *  @return  NodeStorage_Success     Write successful
 *  @return  NodeStorage_WriteError  Write failed
 *  @return  NodeStorage_Error       NodeStorage_init() not successfully called
 */
extern NodeStorage_Status NodeStorage_write(size_t offset, void *buff, size_t len);

/*!
 *  @brief   Write connection data start sequence to NV storage
 *
 *  @return  NodeStorage_Success     Write successful
 *  @return  NodeStorage_WriteError  Write failed
 *  @return  NodeStorage_Error       NodeStorage_init() not successfully called
 */
extern NodeStorage_Status NodeStorage_writeStartSeq(void);

/*!
 *  @brief   Erase all NV storage
 *
 *  @return  NodeStorage_Success     Erase successful
 *  @return  NodeStorage_WriteError  Erase failed
 *  @return  NodeStorage_Error       NodeStorage_init() not successfully called
 */
extern NodeStorage_Status NodeStorage_eraseAll(void);

/*!
 *  @brief   Checks the NVS region starting at offset 0 for nvDataStartSeq
 *
 *  @return  true    Data sequence found; false returned otherwise
 */
extern bool NodeStorage_verifyStorage(void);

#endif /* NODESTORAGE_H_ */
