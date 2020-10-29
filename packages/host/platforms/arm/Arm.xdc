/* 
 *  Copyright (c) 2008 Texas Instruments and others.
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
 *  ======== Arm.xdc ========
 */

package host.platforms.arm;

/*!
 *  ======== Arm ========
 *  Arm device data sheet module.
 *
 *  This module provides basic data suitable for an Arm-based
 *  native builds.
 */
metaonly module Arm inherits xdc.platform.ICpuDataSheet
{
instance:    
    override config string cpuCore	    = "v7A";
    override config string cpuCoreRevision   = "1.0";
    override config int    minProgUnitSize   = 1;
    override config int    minDataUnitSize   = 1;
    override config int    dataWordSize	    = 4;

    /*!
     *  ======== memMap ========
     *  The memory map returned be getMemeoryMap().
     */
    config xdc.platform.IPlatform.Memory memMap[string] = [];
}
/*
 *  @(#) host.platforms.arm; 1, 0, 0,0; 4-17-2020 14:54:54; /db/ztree/library/trees/xdc/xdc-I11/src/packages/
 */

