/* --COPYRIGHT--,EPL
 *  Copyright (c) 2008-2017 Texas Instruments Incorporated
 *  All rights reserved. This program and the accompanying materials
 *  are made available under the terms of the Eclipse Public License v1.0
 *  which accompanies this distribution, and is available at
 *  http://www.eclipse.org/legal/epl-v10.html
 *
 *  Contributors:
 *      Texas Instruments - initial implementation
 *
 * --/COPYRIGHT--*/
var utils = xdc.loadCapsule('microsoft/targets/_utils.xs');

/*
 *  ======== Win32.compile ========
 */
function compile(goal) {
    return (utils.compile(this, goal, false));
}


/*
 *  ======== Win32.link ========
 */
function link(goal) {
    var result = this.$super.link.$fxn.call(this, goal);
    if (this.$private._vcVersion == "VC8" && this.cmdPrefix == "") {
        /* VC8.0 programs require manifests and linker create them as
         * separate files. Since manifests are always required, and they
         * have to be in the same directory as programs, we use mt to
         * embed them into programs. However, it doesn't work for wine for now
         * because of missing Net Framework.
         */
        result.cmds += this.cmdPrefix + "$(rootDir)/VC/bin/mt -nologo -manifest $@.manifest -outputresource:'$@;#1'\n";
        result.cmds += "-$(RM) $@.manifest\n";
    }
    return (result);
}

/*
 *  ======== Win32.scompile ========
 */
function scompile(goal) {
    return (utils.compile(this, goal, true));
}

var addSdkCalled = false;

/*
 *  ======== Win32.setPath ========
 */
function setPath() {
    if (!addSdkCalled) {
        _addSDK();
    }
    return (this.$super.setPath.$fxn.call(this));
}

/*
 *  ======== _addSDK ========
 */
function _addSDK()
{
    if (addSdkCalled) {
        return;
    }
    addSdkCalled = true;
    var targ = xdc.module("microsoft.targets.Win32");
    targ.$unseal();
    for (var i = 0; i < targ.sdkPath.includePath.length; i++) {
        var incPath = targ.sdkPath.includePath[i];
        var sdk = new java.io.File(incPath);
        var LinuxFound = false;
        if (Build.hostOSName == "Linux") {
            var status = {};
            var attrs = {
                useEnv: true
            }

            var res = xdc.exec(targ.cmdPrefix + "cd " + incPath, attrs, status);
            if (res == 0) {
                LinuxFound = true;
            }
        }

        if (LinuxFound || (Build.hostOSName == "Windows" && sdk.exists())) {
            targ.includeOpts += " -I" + incPath;
        }
        else {
            print("The Windows SDK include path '" + incPath
                + "' could not be found. Ensure that the SDK include path is "
                + "specified correctly in "
                + java.lang.System.getenv("XDCBUILDCFG"));
        }
    }
    for (i = 0; i < targ.sdkPath.libraryPath.length; i++) {
        var libPath = targ.sdkPath.libraryPath[i];
        var sdk = new java.io.File(libPath);
        var LinuxFound = false;
        if (Build.hostOSName == "Linux") {
            var status = {};
            var attrs = {
                useEnv: true
            }

            var res = xdc.exec(targ.cmdPrefix + "cd " + libPath, attrs, status);
            if (res == 0) {
                LinuxFound = true;
            }
        }

        if (LinuxFound || (Build.hostOSName == "Windows" && sdk.exists())) {
                targ.lnkOpts.prefix += " -libpath:" + libPath;
        }
        else {
            print("Warning: The Windows SDK library path '" + libPath
                    + "' could not be found. Ensure that the SDK library path "
                    + "is specified correctly in "
                    + java.lang.System.getenv("XDCBUILDCFG"));
        }
        if (i == 0) {
            for (var j = 0; j < targ.profiles.length; j++) {
                targ.profiles[j].linkOpts += " " + targ.sdkPath.libs;
            }
        }
    }
    targ.$seal();
}
