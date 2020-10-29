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
 *  ======== Error.c ========
 */

#include <xdc/runtime/Gate.h>
#include <xdc/runtime/System.h>
#include <xdc/runtime/Text.h>
#include <xdc/runtime/Types.h>

#include <string.h>

/* Implementation of the following requirements is in Error__epilogue.h */
/* REQ_TAG(SYSBIOS-859), REQ_TAG(SYSBIOS-860), REQ_TAG(SYSBIOS-863) */
#include "package/internal/Error.xdc.h"

/*
 *  ======== Error_init ========
 */
Void Error_init(Error_Block *eb)
{
    if (eb != NULL && eb != &xdc_runtime_Error_IgnoreBlock) {
        (void)memset(eb, 0, sizeof (Error_Block));
    }
}

/*
 *  ======== Error_check ========
 */
/* REQ_TAG(SYSBIOS-864) */
Bool Error_check(Error_Block *eb)
{
    /* The condition eb->id != 0 rejects Error_IGNORE */
    /* In C a variable of an enum type and an enum constant are not always of
     * the same type. MISRA requires that two operands in a comparison be of
     * the same type.
     */
    return (Bool)((UInt)Error_policy == (UInt)Error_UNWIND && eb != NULL
                  && eb->id != 0U);
}

/*
 *  ======== Error_getData ========
 */
/* REQ_TAG(SYSBIOS-868) */
Error_Data *Error_getData(Error_Block *eb)
{
    return (&eb->data);
}

/*
 *  ======== Error_getCode ========
 */
UInt16 Error_getCode(Error_Block *eb)
{
    return (Error_idToCode(eb->id));
}

/*
 *  ======== Error_getId ========
 */
/* REQ_TAG(SYSBIOS-869) */
Error_Id Error_getId(Error_Block *eb)
{
    return (eb->id);
}

/*
 *  ======== Error_getMsg ========
 */
/* REQ_TAG(SYSBIOS-867) */
CString Error_getMsg(Error_Block *eb)
{
    return (eb->msg);
}

/*
 *  ======== Error_getSite ========
 */
/* REQ_TAG(SYSBIOS-866) */
Types_Site *Error_getSite(Error_Block *eb)
{
    return (&eb->site);
}

/*
 *  ======== Error_print ========
 */
/* REQ_TAG(SYSBIOS-870) */
Void Error_print(Error_Block *eb)
{
    if (eb != NULL && eb->unused == 0U) {

        if (eb->msg != NULL) {
            (void)Text_putSite(Error_getSite(eb), NULL, -1);
            if (Text_isLoaded == TRUE) {
                (void)System_aprintf(eb->msg, eb->data.arg[0], eb->data.arg[1]);
            }
            else {
                (void)System_aprintf("error {id:0x%x, args:[0x%x, 0x%x]}",
                    eb->id, eb->data.arg[0], eb->data.arg[1]);
            }
            (void)System_printf("\n");
        }
    }
}

/*
 *  ======== Error_raiseX ========
 */
Void Error_raiseX(Error_Block *eb, Types_ModuleId mod, CString file,
    Int line, Error_Id id, IArg arg1, IArg arg2)
{
    /* REQ_TAG(SYSBIOS-856) */
    Error_policyFxn(eb, mod, file, line, id, arg1, arg2);
}

/*
 *  ======== Error_policyDefault ========
 */
/* REQ_TAG(SYSBIOS-853), REQ_TAG(SYSBIOS-865) */
Void Error_policyDefault(Error_Block *eb, Types_ModuleId mod, CString file,
                         Int line, Error_Id id, IArg arg1, IArg arg2)
{
    Error_Block defErr;
    IArg gateKey;
    UInt16 oldCount;
    Bool errorAbort = FALSE;

    if (eb == NULL || eb->unused != 0U) {
        errorAbort = (Bool)(eb == NULL);
        eb = &defErr;
    }

    /* fill in the error block */
    Error_setX(eb, mod, file, line, id, arg1, arg2);
    if (Module__LOGDEF == TRUE) {
        Error_policyLog(mod, file, line, eb->msg, arg1, arg2);
    }
    /* count nesting level of errors */
    gateKey = Gate_enterSystem();
    oldCount = module->count;
    module->count++;
    Gate_leaveSystem(gateKey);

    /* call any provided error hook, unless we are too deeply nested */
    /* REQ_TAG(SYSBIOS-857) */
    if (Error_raiseHook != (Error_HookFxn)NULL && oldCount < Error_maxDepth) {
        (Error_raiseHook)(eb);
    }

    /* REQ_TAG(SYSBIOS-852), REQ_TAG(SYSBIOS-859) */
    if ((UInt)Error_policy == (UInt)Error_TERMINATE || errorAbort == TRUE) {
        System_abort("xdc.runtime.Error.raise: terminating execution\n");
    }

    gateKey = Gate_enterSystem();
    module->count--;
    Gate_leaveSystem(gateKey);
}

/*
 *  ======== Error_policyMin ========
 */
/* REQ_TAG(SYSBIOS-855) */
Void Error_policyMin(Error_Block *eb, Types_ModuleId mod, CString file,
    Int line, Error_Id id, IArg arg1, IArg arg2)
{
    /* REQ_TAG(SYSBIOS-852) */
    if (eb == NULL || (UInt)Error_policy == (UInt)Error_TERMINATE) {
        for(;;) {
        }
    }
    else if (eb != &xdc_runtime_Error_IgnoreBlock) {
        eb->id = id;
    }
    else {
        return;
    }
}

/*
 *  ======== Error_policySpin ========
 */
/* REQ_TAG(SYSBIOS-854) */
/* LCOV_EXCL_START */
Void Error_policySpin(Error_Block *eb, Types_ModuleId mod, CString file,
    Int line, Error_Id id, IArg arg1, IArg arg2)
{
/* LCOV_EXCL_STOP */
    for(;;) {
    }
}

/*
 *  ======== Error_setX ========
 */
Void Error_setX(Error_Block *eb, Types_ModuleId mod, CString file,
    Int line, Error_Id id, IArg arg1, IArg arg2)
{
    Error_init(eb);

    /* REQ_TAG(SYSBIOS-862) */
    eb->data.arg[0] = arg1;
    eb->data.arg[1] = arg2;
    eb->id = id;
    eb->msg = (Text_isLoaded == TRUE)
        ? Text_ropeText((Text_RopeId)(id >> 16)) : "";
    eb->site.mod = mod;
    eb->site.file = file;
    eb->site.line = line;
}
/*
 *  @(#) xdc.runtime; 2, 1, 0,0; 4-17-2020 14:55:36; /db/ztree/library/trees/xdc/xdc-I11/src/packages/
 */

