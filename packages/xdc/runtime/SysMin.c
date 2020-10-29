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
 *  ======== SysMin.c ========
 */

#include <xdc/runtime/Startup.h>
#include <xdc/runtime/Gate.h>

#include <string.h>

#include "package/internal/SysMin.xdc.h"

/*
 *  ======== SysMin_Module_startup ========
 */
Int SysMin_Module_startup(Int phase)
{
    /* REQ_TAG(SYSBIOS-919) */
    if (SysMin_bufSize != 0U) {
        (void)memset(module->outbuf, 0, SysMin_bufSize);
    }
    return (Startup_DONE);
}

/*
 *  ======== SysMin_abort ========
 */
/* REQ_TAG(SYSBIOS-915) */
Void SysMin_abort(CString str)
{
    Char ch;

    if (SysMin_bufSize != 0U) {
        if (str != (CString)NULL) {
            ch = *str;
            str++;
            while (ch != '\0') {
                SysMin_putch(ch);
                ch = *str;
                str++;
            }
        }

        /* Only flush if configured to do so */
        /* REQ_TAG(SYSBIOS-920) */
        if (SysMin_flushAtExit != FALSE) {
            SysMin_flush();
        }
    }
}

/*
 *  ======== SysMin_exit ========
 */
/* REQ_TAG(SYSBIOS-916) */
Void SysMin_exit(Int stat)
{
    /* REQ_TAG(SYSBIOS-920) */
    if ((SysMin_flushAtExit == TRUE) && (SysMin_bufSize != 0U)) {
        SysMin_flush();
    }
}

/*
 *  ======== SysMin_putch ========
 */
/* REQ_TAG(SYSBIOS-917) */
Void SysMin_putch(Char ch)
{
    IArg key;

    if (SysMin_bufSize != 0U) {

        key = Gate_enterSystem();

        module->outbuf[module->outidx] = ch;
        module->outidx++;
        if (module->outidx == (UInt)SysMin_bufSize) {
            module->outidx = 0;
            module->wrapped = TRUE;
        }

        Gate_leaveSystem(key);
    }
}

/*
 *  ======== SysMin_ready ========
 */
/* REQ_TAG(SYSBIOS-918) */
Bool SysMin_ready(Void)
{
    return (Bool)(SysMin_bufSize != 0U);
}

/*
 *  ======== SysMin_flush ========
 *  Called during SysMin_exit, System_exit or System_flush.
 */
Void SysMin_flush(Void)
{
    IArg key;

    key = Gate_enterSystem();

    /*
     *  If a wrap occured, we need to flush the "end" of the internal buffer
     *  first to maintain fifo character output order.
     */
    if (module->wrapped == TRUE) {
        SysMin_outputFunc(module->outbuf + module->outidx,
                          (UInt)(SysMin_bufSize - module->outidx));
    }

    /* REQ_TAG(SYSBIOS-914) */
    SysMin_outputFunc(module->outbuf, module->outidx);
    module->outidx = 0;
    module->wrapped = FALSE;

    Gate_leaveSystem(key);
}
/*
 *  @(#) xdc.runtime; 2, 1, 0,0; 4-17-2020 14:55:37; /db/ztree/library/trees/xdc/xdc-I11/src/packages/
 */

