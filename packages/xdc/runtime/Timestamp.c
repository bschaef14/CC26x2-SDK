/* 
 *  Copyright (c) 2008-2019 Texas Instruments Incorporated
 *  This program and the accompanying materials are made available under the
 *  terms of the Eclipse Public License v1.0 and Eclipse Distribution License
 *  v. 1.0 which accompanies this distribution. The Eclipse Public License is
 *  available at http://www.eclipse.org/legal/epl-v10.html and the Eclipse
 *  Distribution License is available at
 *  http://www.eclipse.org/org/documents/edl-v10.php.
 *
 *  Contributors:
 *      Texas Instruments - initial implementation
 * */
/*
 *  ======== Timestamp.c ========
 */

#include <xdc/runtime/Types.h>
#include "package/internal/Timestamp.xdc.h"

/*
 *  ======== Timestamp_get32 ========
 */
/* REQ_TAG(SYSBIOS-883) */
Bits32 Timestamp_get32(Void)
{
    return (Timestamp_SupportProxy_get32());
}

/*
 *  ======== Timestamp_get64 ========
 */
/* REQ_TAG(SYSBIOS-884) */
Void Timestamp_get64(Types_Timestamp64 *result)
{
    Timestamp_SupportProxy_get64(result);
}

/*
 *  ======== Timestamp_getFreq ========
 */
/* REQ_TAG(SYSBIOS-885) */
Void Timestamp_getFreq(Types_FreqHz *freq)
{
    Timestamp_SupportProxy_getFreq(freq);
}
/*
 *  @(#) xdc.runtime; 2, 1, 0,0; 4-17-2020 14:55:37; /db/ztree/library/trees/xdc/xdc-I11/src/packages/
 */

