/*
 * Copyright (c) 2015-2017, Texas Instruments Incorporated
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

#ifndef TASKS_NODETASK_H_
#define TASKS_NODETASK_H_

#include "oad/native_oad/oad_storage.h"

#define NODE_EVENT_ALL                  0xFFFFFFFF
#define NODE_EVENT_NEW_ADC_VALUE        (uint32_t)(1 << 0)
#define NODE_EVENT_UPDATE_LCD           (uint32_t)(1 << 1)
#define NODE_EVENT_OAD_REQ              (uint32_t)(1 << 2)

/* Initializes the Node Task and creates all TI-RTOS objects */
void NodeTask_init(void);

/* Update the Node Task OAD Block display */
void NodeTask_displayOadBlockUpdate(uint16_t newOadBlock, uint16_t oadBNumBlocks, uint32_t retries);

/* Update the Node Task OAD status */
void NodeTask_displayOadStatusUpdate(OADStorage_Status_t status);

#ifdef OAD_U_APP
/* Save node address and server address to NVS, invalidate header, and reset */
void NodeTask_resetUserApp(void* pDstAddr);
#endif
#endif /* TASKS_NODETASK_H_ */
