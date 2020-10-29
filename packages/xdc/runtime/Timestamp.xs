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
 *  ======== Timestamp.xs ========
 */

/*
 *  ======== module$meta$init ========
 */
function module$meta$init()
{
    if (xdc.om.$name == 'cfg') {
        /* set default SupportProxy to TimestampNull */
        /* REQ_TAG(SYSBIOS-881), REQ_TAG(SYSBIOS-882) */
        this.SupportProxy = xdc.module('xdc.runtime.TimestampNull');
    }
}
/*
 *  @(#) xdc.runtime; 2, 1, 0,0; 4-17-2020 14:55:37; /db/ztree/library/trees/xdc/xdc-I11/src/packages/
 */

