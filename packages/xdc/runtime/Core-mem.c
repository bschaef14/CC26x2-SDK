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
 *  ======== Core-mem.c ========
 */

#include <string.h>

#include <xdc/runtime/Assert.h>
#include <xdc/runtime/Error.h>
#include <xdc/runtime/Gate.h>
#include <xdc/runtime/Memory.h>
#include <xdc/runtime/IInstance.h>
#include <xdc/runtime/Types.h>

#include "package/internal/Core.xdc.h"

/*
 *  ======== Core_createObject ========
 *  Initialize instance object and instance creation parameter structure
 *
 *  This function is called for created instance objects and is responsible for:
 *      1. allocating instance object memory (if necessary)
 *      2. initializing the instance object fields (name and functions) and
 *         all other fields to 0
 *      3. initializing instance creation parameters based on user supplied
 *         params
 *  Module-specific initializion occurs _after_ this function completes.
 *
 *  All dynamically created instance objects are of the form:
 *              Types_InstHdr instHdr;
 *      inst -> [Ptr           fxns;]
 *               :                      // module-specific fields
 *
 *  where [Ptr fxns] is only present in modules that inherit from an
 *  interface.
 *
 *  All constructed instance objects are of the form:
 *      inst -> [Ptr           fxns]
 *               :                      // module-specific fields
 *
 *  Again, [Ptr fxns] is only present in modules that inherit from an
 *  interface.
 *
 *  Input parameters:
 *    od          - pointer to a config generated object "descriptor"
 *                  structure (in cfg/.c)
 *    argPrms     - pointer to a client initialized pkg_Mod_Params structure
 *    argPrmsSize - sizeof(module Params structure)
 *
 *  Output parameters:
 *    resPrms     - pointer to an uninitialized pkg_Mod_Params structure to
 *                  be initialized based on argPrms
 *
 *  curObj and argPrmsSize are not used but we can't remove them until
 *  ROM images that have Core_createObject embedded are not supported anymore.
 */
/* REQ_TAG(SYSBIOS-871), REQ_TAG(SYSBIOS-878) */
Ptr Core_createObject(const Core_ObjDesc *od, Ptr curObj, Ptr resPrms,
                      CPtr argPrms, SizeT argPrmsSize, Error_Block *eb)
{
    Types_InstHdr *instHdr;
    Types_PrmsHdr *prmsHdr;
    Ptr resObj;
    IInstance_Params *instPrms;
    IArg gateKey;

    /* initialize resPrms with the values from Mod_Object__PARAMS__C */
    (void)memcpy(resPrms, od->prmsInit, od->prmsSize);
    prmsHdr = (Types_PrmsHdr *)resPrms;
    prmsHdr->self = prmsHdr;    /* mark resParms as initialized */

    /* use params passed from client, if any */
    if (argPrms != (CPtr)NULL) {
        /* REQ_TAG(SYSBIOS-879) */
        Assert_isTrue(((const Types_PrmsHdr *)argPrms)->self == argPrms,
            Core_A_initializedParams);

        Core_assignParams(resPrms, argPrms, od->prmsSize,
            sizeof(IInstance_Params));
    }

    instPrms = (IInstance_Params *)prmsHdr->instPrms;

    /* create object and add to list of dynamically created instances */
    instHdr = (Types_InstHdr *)
        Memory_calloc(od->objHeap, od->objSize, od->objAlign, eb);
    if (instHdr == NULL) {
        return (NULL);
    }

    resObj = instHdr + 1;

    /* atomically insert instance on list of all runtime instances */
    gateKey = Gate_enterSystem();
        instHdr->link.next = od->modLink;
        instHdr->link.prev = od->modLink->prev;
        od->modLink->prev->next = &instHdr->link;
        od->modLink->prev = &instHdr->link;
    Gate_leaveSystem(gateKey);

    /* initialize fxns (if the field is present) */
    if (od->fxnTab != (CPtr)-1) {
        *((CPtr *)resObj) = od->fxnTab;
    }

    /* initialize instance name (if this module supports named instances) */
    if (od->objName != 0U) {
        *((CPtr *)((Char *)resObj + od->objName)) = instPrms->name;
    }

    return (resObj);
}

/*
 *  ======== Core_deleteObject ========
 */
/* REQ_TAG(SYSBIOS-872) */
Void Core_deleteObject(const Core_ObjDesc *od, Ptr curObj, Fxn finalFxn,
    Int istat, Bool consFlg)
{
    typedef Void (*FinalFxn1)(Ptr);
    typedef Void (*FinalFxn2)(Ptr, Int);

    Types_InstHdr *instHdr;
    IArg gateKey;

    if (curObj == (Ptr)NULL) {
        return;     /* silently ignore NULL object references */
    }

    if (consFlg == FALSE) {
        instHdr = ((Types_InstHdr *)curObj) - 1;

        gateKey = Gate_enterSystem();
            instHdr->link.prev->next = instHdr->link.next;
            instHdr->link.next->prev = instHdr->link.prev;

            /* Zeroing pointers in instHdr, so that anyone pointing at this
             * instance can notice that the instance is invalid (CQ10402).
             */
            instHdr->link.prev = NULL;
            instHdr->link.next = NULL;
        Gate_leaveSystem(gateKey);
    }

    if (finalFxn != (Fxn)NULL) {
        if (istat == Core_NOSTATE) {
            ((FinalFxn1)finalFxn)(curObj);
        }
        else {
            ((FinalFxn2)finalFxn)(curObj, istat);
        }
    }

    if (consFlg == FALSE) {
        Memory_free(od->objHeap, instHdr, od->objSize);
    }
}
/*
 *  @(#) xdc.runtime; 2, 1, 0,0; 4-17-2020 14:55:36; /db/ztree/library/trees/xdc/xdc-I11/src/packages/
 */

