/* 
 *  Copyright (c) 2008-2020 Texas Instruments Incorporated
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
 *  ======== package.xs ========
 */

/*
 *  ======== package.init ========
 */
function init()
{
    if (xdc.om.$name != 'cfg') {
        return;
    }

    for each (var mod in this.$modules) {
        if ((mod.$name == 'xdc.runtime.Error')
            || (mod.$name == 'xdc.runtime.Main')
            || (mod.$name == 'xdc.runtime.Registry')
            || !("common$" in mod)          /* skip meta-only modules */
            || mod.$name == 'xdc.runtime.Defaults') {
            continue;
        }

        /* for most modules in xdc.runtime, set default logger to NULL */
        mod.common$.logger = null;

        /* and turn off all their diags */
        for (var dl in mod.common$) {
            if (dl.match(/^diags_/) && dl != 'diags_ASSERT') {
                mod.common$[dl] = this.Diags.ALWAYS_OFF;
            }
        }
    }
}

/*
 *  ======== package.close ========
 */
function close()
{
    if (xdc.om.$name != 'cfg') {
        return;
    }

    var targMod = null;

    for (var i = 0; i < xdc.om.$modules.length; i++) {
        var mod = xdc.om.$modules[i];
        if (mod.$used && !mod.$hostonly && !mod.$$nortsflag) {
            targMod = mod;
            break;
        }
    }

    if (Program.system !== undefined) {
        if (targMod && Program.system === null) {
            Program.$logError("the target module "
                + targMod.$name + " is present, but Program.system == null",
                Program, "system");
            return;
        }
        else if (Program.build.cfgScript) {
            Program.$logWarning("assignment to Program.system is deprecated",
                Program, "system");
        }
    }

    if (targMod) {
        xdc.useModule('xdc.runtime.System');
    }
    else {
	/* no target modules used but we may need to define a reset function */
        var Startup = xdc.module("xdc.runtime.Startup");
        var Reset = xdc.module("xdc.runtime.Reset");

        if (Startup.resetFxn != null || Reset.fxns.length > 0) {
            /* define RESETFXN so reset function will be called from boot */
            /* REQ_TAG(SYSBIOS-951) */
            Program.symbol["xdc_runtime_Startup__RESETFXN__C"] = 1;

            /* ensure Reset.xdt template will be expanded */
            xdc.useModule("xdc.runtime.Reset");
        }
        else {
            /* no reset fxn is defined, stop boot from calling anything */
            Program.symbol["xdc_runtime_Startup__RESETFXN__C"] = 0;
            Program.symbol["xdc_runtime_Startup_reset__I"] = 0;
        }
        Program.symbol["xdc_runtime_Startup__EXECFXN__C"] = 0;
        Program.symbol["xdc_runtime_Startup_exec__E"] = 0;
    }

    /* This would normally happen in Core.module$use, but we need to do it as
     * late as possible to be sure that Core.noAsserts will not change anymore.
     */
    if (this.Core.$used && this.Core.noAsserts == false) {
        xdc.useModule('xdc.runtime.Assert');
    }

    var Types = xdc.module('xdc.runtime.Types');
    var Defaults = xdc.module('xdc.runtime.Defaults');
    for (var i = 0; i < xdc.om.$modules.length; i++) {
        var mod = xdc.om.$modules[i];
//        print(mod.$name, mod.$$instflag, mod.$used);
        if (mod.$used && !mod.$hostonly && mod.$$instflag
            && Defaults.getCommon(mod, "memoryPolicy") != Types.STATIC_POLICY) {
                xdc.useModule('xdc.runtime.Memory');
                break;
        }
    }

    /* 'maxAtexitHandlers' is set here because after close(), all configuration
     * parameters are sealed and this is the last chance to change them.
     */
    var System = xdc.module('xdc.runtime.System');
    System.maxAtexitHandlers += System.exitFxns.length;

    var Memory = this.Memory;
    /* We want to set Memory's default values very late, after all other
     * modules had a chance to set them.
     */
    if (Memory.$used) {
        /*
         *  Make sure it has a default instance. This instance is used in
         *  Memory_alloc if NULL is specified for the heap.
         */
        if (Memory.defaultHeapInstance == null) {
            var HeapStd = xdc.useModule('xdc.runtime.HeapStd');
            Memory.defaultHeapInstance = HeapStd.create(
                {size: HeapStd.HEAP_MAX});
        }
        Memory.HeapProxy = Memory.defaultHeapInstance.$module;

        /* declare that instances may come from different modules; the
         * generated HeapProxy function stub must call through a vtable
         * pointer rather than directly call the HeapProxy module
         */
        Memory.HeapProxy.abstractInstances$ = true;
    }

    /* In custom builds, where we can eliminate Asserts and Logs from a build,
     * we don't need module config parameters related to Assert and Log modules.
     */
    var Log = this.Log;
    if (!Log.$used) {
        for each (var mod in xdc.om.$modules) {
            if ('$$logEvtCfgs' in mod && mod.$$logEvtCfgs.length > 0) {
                for each (var cn in mod.$$logEvtCfgs) {
                    mod[cn].$private.id = $$NOGEN;
                }
            }

            if (!mod.$used || mod.$hostonly || mod.PROXY$) {
                continue;
            }
            mod.Module__loggerFxn0 = $$NOGEN;
            mod.Module__loggerFxn1 = $$NOGEN;
            mod.Module__loggerFxn2 = $$NOGEN;
            mod.Module__loggerFxn4 = $$NOGEN;
            mod.Module__loggerFxn8 = $$NOGEN;
            //mod.Module__loggerDefined = $$NOGEN;
            mod.Module__loggerObj = $$NOGEN;
        }
    }

    var Assert = this.Assert;
    if (!Assert.$used) {
        for each (var mod in xdc.om.$modules) {
            if ('$$assertDescCfgs' in mod && mod.$$assertDescCfgs.length > 0) {
                for each (var cn in mod.$$assertDescCfgs) {
                    mod[cn].$private.id = $$NOGEN;
                }
            }
        }
    }

    var Diags = this.Diags;
    if (!Log.$used && !Assert.$used && !Diags.$used) {
        for each (var mod in xdc.om.$modules) {
            if (!mod.$used || mod.$hostonly || mod.PROXY$) {
                continue;
            }
            mod.Module__diagsMask = $$NOGEN;
            mod.Module__diagsIncluded = $$NOGEN;
            mod.Module__diagsEnabled = $$NOGEN;
        }
    }

    var LoggerBuf = this.LoggerBuf;
    /* We want to set LoggerBuf default values very late, after all other
     * modules had a chance to set them.
     */
    if (LoggerBuf.$used) {
	/* bind LoggerBuf.TimestampProxy to whatever Timestamp is using,
	 * unless LoggerBuf.TimestampProxy was set already.
	 */
	if (LoggerBuf.TimestampProxy == null) {
	    var Timestamp = xdc.module('xdc.runtime.Timestamp');
	    LoggerBuf.TimestampProxy = 
		xdc.useModule(Timestamp.SupportProxy.delegate$.$name, true);
	}

        /* disable trace on timestamp proxy to prevent recursive callbacks */
        var modName = LoggerBuf.TimestampProxy.delegate$.$name;
        Diags.setMaskMeta(modName, Diags.ALL_LOGGING, Diags.ALWAYS_OFF);
    }
}

/*
 *  ======== checkProxies ========
 */
function checkProxies(unit)
{
    if (!('$$proxies' in unit) || unit.PROXY$ || '$$proxyCheck' in unit) {
        return;
    }

    unit.$$bind('$$proxyCheck', 1);

    for (var i = 0; i < unit.$$proxies.length; i++) {
        var p = unit.$$proxies[i];
        var pud = unit[p + '$proxy'];

        if (!pud.delegate$) {
            throw new Error("unbound proxy: " + pud.$name);
        }

        if (!pud.delegate$.$used) {
            throw new Error("The delegate '" + pud.delegate$.$name + "' is "
                + "assigned to proxy '" + pud.$name + "' but this delegate "
                + "has not been used. A workaround is to call xdc.useModule('"
                + pud.delegate$.$name + "') in the configuration script.",
                unit, p);
        }

        if ('PROXY$' in pud.delegate$ && pud.delegate$.PROXY$) {
            throw new Error("proxy cannot be bound to another proxy: "
                + pud.delegate$.$name);
        }
    }
}

/*
 *  ======== finalize ========
 *  Called by the configuration model to initialize all common module settings
 */
function finalize()
{
    if (Program.system === null) {
        return;
    }

    var Defaults = xdc.module('xdc.runtime.Defaults');
    var System = xdc.module('xdc.runtime.System');

    /* if System gate is defined and Defaults isn't, make the System gate
     * the default
     */
    for each (var p in ['gate', 'gateParams']) {
        if (System.common$[p] !== undefined && !Defaults.common$[p]) {
            Defaults.common$[p] = System.common$[p];
        }
    }

    /* propagate the Defaults gate to all modules needing one; if the
     * Defaults gate is null, use GateNull
     */
    for each (var mod in xdc.om.$modules) {
        if (!mod.$used || mod.$hostonly || mod.PROXY$) {
            continue;
        }
        /* REQ_TAG(SYSBIOS-921), REQ_TAG(SYSBIOS-922) */
        if (!mod.$spec.attrBool('@Gated')) {
            if (mod.common$.gate != undefined
                && mod.$name != "xdc.runtime.Defaults") {
                mod.$logWarning("this module is non-gated but it was assigned "
                    + "a non-null gate", mod, "common$.gate");
            }
            continue;
        }

        mod.Module__gateObj = (mod.common$.gate !== undefined) ?
            mod.common$.gate : Defaults.common$.gate;
        mod.Module__gatePrms = (mod.common$.gateParams !== undefined) ?
            mod.common$.gateParams : Defaults.common$.gateParams;
        mod.Module_GateProxy = mod.Module__gateObj ?
            mod.Module__gateObj.$module : xdc.useModule('xdc.runtime.GateNull');
        if (mod.common$.gate != null && mod.common$.gateParams != null) {
            var modName = mod.common$.gate.$module.$name;
            var prmName = mod.common$.gateParams.$name.substr(0,
                mod.common$.gateParams.$name.lastIndexOf(".Params#"));
            if (modName != prmName) {
                mod.$logError("Module '" + mod.$name + "' uses '" + modName
                    + "' for mod.common$.gate, and '" + prmName + "' for "
                    + "mod.common$gateParams. They both have to originate "
                    + "from the same module.", mod, "common$.gate");
            }
        }
    }

    /* for all used target module in the configuration ... */
    for each (var mod in xdc.om.$modules) {

        if (mod.$hostonly || !mod.$used) {
            continue;
        }

        checkProxies(mod);

        /*  Unless otherwise specified, force some modules to be named.
         *    Main  : This improves the error messages that come from calls to
         *            Error_raise() and Log_print/write in "non module" code.
         *    Memory: This improves the error message from the call to
         *            Error_raise in Memory_alloc().
         */
        if (mod.$name == "xdc.runtime.Memory"
            || mod.$name == "xdc.runtime.Main") {
            if (mod.common$.namedModule === undefined) {
                mod.common$.namedModule = true;
            }
        }

        /* prevent unintended logging of loggers (usually a mistake) */
        if (mod instanceof xdc.om["xdc.runtime.ILogger"].Module) {
            if (mod.common$.logger === undefined) {
                mod.common$.logger = null;
            }
        }

        /* propagate Defaults to all used target modules */
        if (mod.$spec.needsRuntime()) {
            for (var p in Defaults.common$) {
                if (mod.common$[p] === undefined) {
                    mod.common$[p] = Defaults.common$[p];
                }
            }
        }
    }
}

/*
 *  ======== validate ========
 *  This function is called during the validation phase of configuration
 */
function validate()
{
    /* for each module in this package ... */
    for each (var mod in this.$modules) {

        /* if the module's capsule has a validate function, run it */
        var cap = mod.$capsule;
        if (cap && "validate" in cap && mod.$used) {
            cap.validate.apply(mod, []);
        }
    }
}
/*
 *  @(#) xdc.runtime; 2, 1, 0,0; 4-17-2020 14:55:37; /db/ztree/library/trees/xdc/xdc-I11/src/packages/
 */

