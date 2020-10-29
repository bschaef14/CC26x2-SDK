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
package xdc.tools;

/*!
 *  ======== IRtscProductTemplate ========
 *  @_nodoc
 *  
 */
metaonly interface IRtscProductTemplate
{
    /*!
     *  Structure providing information on the available configurations
     *
     *  @field(title)       String containing the title of the template
     *  @field(fileList)    Array of file paths relative to the template
     *                      package folder
     *  @field(description) String containing the description of the template
     *  @field(template)    Array containing one 'TemplateInfo' object 
     */ 
    struct TemplateInfo {
        String title;
        String fileList[];
        String description;
        TemplateInfo template[];
    };

    /*!
     *  ======== appTemplates ========
     */
    config TemplateInfo appTemplates[];

    /*!
     *  ======== configTemplates ========
     */
    config TemplateInfo configTemplates[];
}
/*
 *  @(#) xdc.tools; 1, 0, 0,0; 4-17-2020 14:56:35; /db/ztree/library/trees/xdc/xdc-I11/src/packages/
 */

