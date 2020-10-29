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
 *
 * */
function init()
{
    switch (String(environment['os.name'])) {
        case 'Linux':
            xdc.loadPackage('xdc.tools.sg.swt_linux');
            break;

        case 'Mac OS X':
            xdc.loadPackage('xdc.tools.sg.swt_macos');
            break;

        default:
            xdc.loadPackage('xdc.tools.sg.swt_win64');
            break;
    }
}
/*
 *  @(#) xdc.tools.sg; 1, 0, 0,; 4-17-2020 12:40:51; /db/rtree/library/trees/xdcprod/xdcprod-z18/product/xdccore/Linux/xdctools_3_51_01_18_core/packages/
 */

