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
 *  ======== staticAlloc ========
 */
/* REQ_TAG(SYSBIOS-930) */
function staticAlloc(stateObj, fldName)
{
    var t = stateObj.$type;
    var mod = xdc.om[t.substring(0, t.lastIndexOf('.'))];
    var res = null;
    var gate = mod.common$.gate;
    if (gate) {
        res = gate.$module.create(mod.common$.gateParams ?
                                  mod.common$.gateParams : {});
    }
    stateObj[fldName] = res;
}

/*
 *  ======== Ref$sizeof ========
 */
function Ref$sizeof()
{
    return (Program.build.target.stdTypes.t_Ptr.size);
}

/*
 *  ======== Ref$alignof ========
 */
function Ref$alignof()
{
    return (Program.build.target.stdTypes.t_Ptr.align);
}

/*
 *  ======== Ref$encode ========
 */
function Ref$encode(gate)
{
    if (!gate) {
        return "0";
    }

    var g = gate.$orig;
    var iarr = g.$module.$name.replace(/\./g, '_') + "_Object__table__V";
    return "((xdc_runtime_Types_GateRef)(&" + iarr + "[" + g.$index + "]))";
}
/*
 *  @(#) xdc.runtime; 2, 1, 0,0; 4-17-2020 14:55:36; /db/ztree/library/trees/xdc/xdc-I11/src/packages/
 */

