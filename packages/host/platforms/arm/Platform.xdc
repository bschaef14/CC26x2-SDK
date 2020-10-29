/* 
 *  Copyright (c) 2008-2019 Texas Instruments Incorporated
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
 *  ======== Platform.xdc ========
 *
 */

package host.platforms.arm;

/*!
 *  ======== Platform ========
 *  ARM-on-Linux platform support
 *
 *  This module implements xdc.platform.IPlatform and defines configuration
 *  parameters that correspond to this platform's Cpu's, Board's, etc.
 */

metaonly module Platform inherits xdc.platform.IPlatform
{
    /*!
     *  ======== BOARD ========
     *  @_nodoc
     *  this structure exists to satisfy the
     *  IPlatform interface requirements; these fields are largely
     *  unnecessary for host platforms.
     */
    readonly config xdc.platform.IPlatform.Board BOARD = {
	id:		"0",
	boardName:	"ARM",
	boardFamily:    "ARM",
	boardRevision:  null
    };

    /*!
     *  ======== CPU ========
     *  @_nodoc 
     */
    readonly config xdc.platform.IExeContext.Cpu CPU = {
	id:		"0",
	clockRate:	1000.0,
	catalogName:	"host.platforms.arm",
	deviceName:	"Arm",
	revision:	"",
    };

  instance:

    /*!
     *  ======== deviceName ========
     *  The CPU simulated by this platform.
     *
     *  This parameter is required.
     */
    config string deviceName = "Arm";

    /*!
     *  ======== catalogName ========
     *  The name of the package that contains the module 'deviceName'.
     *
     *  This parameter is required.
     */
    config string catalogName = "host.platforms.arm";
}
/*
 *  @(#) host.platforms.arm; 1, 0, 0,0; 4-17-2020 14:54:54; /db/ztree/library/trees/xdc/xdc-I11/src/packages/
 */

