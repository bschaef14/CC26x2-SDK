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
 *  ======== Startup.c ========
 */

#include <xdc/runtime/System.h>
#include "package/internal/Startup.xdc.h"

/*
 *  ======== Startup_exec ========
 */
/* REQ_TAG(SYSBIOS-950) */
Void Startup_exec(Void)
{
    Int i;

    if (module->execFlag == FALSE) {
        module->execFlag = TRUE;

        for (i = 0; i < Startup_firstFxns.length; i++) {
            /* REQ_TAG(SYSBIOS-955) */
            Startup_firstFxns.elem[i]();
        }

        (Startup_execImpl)();

        for (i = 0; i < Startup_lastFxns.length; i++) {
            /* REQ_TAG(SYSBIOS-956) */
            Startup_lastFxns.elem[i]();
        }
    }
}

/*
 *  ======== Startup_rtsDone ========
 */
/* REQ_TAG(SYSBIOS-954) */
Bool Startup_rtsDone(Void)
{
    return (module->rtsDoneFlag);
}

/*
 *  ======== Startup_startMods ========
 */
/* REQ_TAG(SYSBIOS-949) */
Void Startup_startMods(Int state[], Int len)
{
    Int curPass;
    Int i;
    Bool done = FALSE;

    /* We initialize 'state' before running any Startup function to have
     * the array look good in ROV. The runtime Startup functions must not
     * depend on any other module already finished with the startup, so
     * they could run even before 'state' is fully initialized.
     */
    for (i = 0; i < len; i++) {
        state[i] = xdc_runtime_Startup_NOTDONE;
    }

    module->stateTab = state;

    for (i = 0; i < len; i++) {
        if (Startup_sfxnRts[i] != FALSE) {
            state[i] = Startup_sfxnTab[i](Startup_NOTDONE);
        }
    }

    module->rtsDoneFlag = TRUE;

    /* REQ_TAG(SYSBIOS-952) */
    for (curPass = 0; curPass < Startup_maxPasses; curPass++) {
        done = TRUE;
        for (i = 0; i < len; i++) {
            if (state[i] != Startup_DONE) {
                state[i] = Startup_sfxnTab[i](state[i]);
            }
            /* Without the cast, MISRA complains about operands of different
             * types. The variable 'done' is xdc_Bool, which is really unsigned
             * short, while the equality operator returns real 'Boolean'.
             */
            done &= (Bool)(state[i] == Startup_DONE);
        }
        if (done == TRUE) {
            break;
        }
    }

    if (done == FALSE) {
        System_abort("xdc.runtime.Startup: 'maxPasses' exceeded");
    }

    module->stateTab = NULL;
}
/*
 *  @(#) xdc.runtime; 2, 1, 0,0; 4-17-2020 14:55:37; /db/ztree/library/trees/xdc/xdc-I11/src/packages/
 */

