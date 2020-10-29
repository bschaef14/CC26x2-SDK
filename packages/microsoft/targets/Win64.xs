/* --COPYRIGHT--,EPL
 * Copyright (c) 2018 Texas Instruments Incorporated - http://www.ti.com
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * --/COPYRIGHT--*/

var utils = xdc.loadCapsule('microsoft/targets/_utils.xs');

/*
 *  ======== Win64.compile ========
 */
function compile(goal) {
    return (utils.compile(this, goal, false));
}


/*
 *  ======== Win64.link ========
 */
function link(goal) {
    var result = this.$super.link.$fxn.call(this, goal);
    return (result);
}

/*
 *  ======== Win64.scompile ========
 */
function scompile(goal) {
    return (utils.compile(this, goal, true));
}

var addSdkCalled = false;

/*
 *  ======== Win64.setPath ========
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
    var targ = xdc.module("microsoft.targets.Win64");
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
