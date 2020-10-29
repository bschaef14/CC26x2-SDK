/* 
 *  Copyright (c) 2008-2018 Texas Instruments Incorporated
 *  All rights reserved. This program and the accompanying materials
 *  are made available under the terms of the Eclipse Public License v1.0
 *  which accompanies this distribution, and is available at
 *  http://www.eclipse.org/legal/epl-v10.html
 *
 *  Contributors:
 *      Texas Instruments - initial implementation
 *
 * */
/*
 *  ======== getLibs ========
 */
function getLibs() 
{
    if (Program.build.target.name == "Win64") {
	/* *.o files are returned only when Dos.dll is built within this
	 * package.
	 */
	if (Program.$$isasm == 1 && Program.buildPackage == this.$name) {
            if (Program.build.profile == "debug") {
                return ("package/lib/debug/Doss/Dos.o" +
                	Program.build.target.suffix +
                        ";package/lib/debug/Doss/StringUtils.o" +
                	Program.build.target.suffix);
            }
            else {
                return ("package/lib/release/Doss/Dos.o" +
                	Program.build.target.suffix +
                        ";package/lib/release/Doss/StringUtils.o" +
                	Program.build.target.suffix);
            }
	}
	else {
	    return (Program.build.profile + "/Doss.lib");
	}
    }
    else {
        return null;
    }
}
/*
 *  @(#) xdc.services.io; 1, 0, 0,0; 4-17-2020 14:56:08; /db/ztree/library/trees/xdc/xdc-I11/src/packages/
 */

