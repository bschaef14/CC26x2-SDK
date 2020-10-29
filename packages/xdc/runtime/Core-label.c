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
 *  ======== Core-label.c ========
 */

#include <xdc/runtime/Text.h>
#include <xdc/runtime/Types.h>

#include "package/internal/Core.xdc.h"

/*
 *  ======== Core_assignLabel ========
 */
/* REQ_TAG(SYSBIOS-875), REQ_TAG(SYSBIOS-889) */
Void Core_assignLabel(Types_Label *lab, Text_CordAddr iname, Bool named)
{
    String ns;

    lab->named = named;

    if (iname == (Text_CordAddr)NULL && Text_isLoaded == TRUE) {
        /* REQ_TAG(SYSBIOS-890) */
        ns = Text_nameEmpty;
    }
    else {
        ns = Text_cordText(iname);
        if (ns == (String)NULL) {
            /* REQ_TAG(SYSBIOS-891) */
            ns = Text_nameStatic;
        }
    }

    lab->iname = ns;
}
/*
 *  @(#) xdc.runtime; 2, 1, 0,0; 4-17-2020 14:55:36; /db/ztree/library/trees/xdc/xdc-I11/src/packages/
 */

