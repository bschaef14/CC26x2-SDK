/* --COPYRIGHT--,EPL
 *  Copyright (c) 2009-2018 Texas Instruments Incorporated
 *  All rights reserved. This program and the accompanying materials
 *  are made available under the terms of the Eclipse Public License v1.0
 *  which accompanies this distribution, and is available at
 *  http://www.eclipse.org/legal/epl-v10.html
 *
 *  Contributors:
 *      Texas Instruments - initial implementation
 *
 * --/COPYRIGHT--*/
/*
 *  ======== Win32.xdc ========
 */

/*!
 *  ======== Win32 ========
 *  Microsoft Windows 32-bit target using the Visual C/C++ compiler
 *
 *  This target can be configured to be used with different versions of
 *  Microsoft Visual C/C++ and different installations of Platform SDK (or a
 *  newer equivalent Windows SDK). The optional Platform SDK is needed only
 *  for building applications that use Windows API.
 *  The target `Win32` contains default configurations for Visual C/C++
 *  versions 8.0, and 14.0, but in order to support future versions of
 *  Visual C/C++ and different Windows SDK installations, there
 *  is a parameter {@link #sdkPath}, which users can set in their `config.bld`
 *  files to enable usage of this target with their installations.
 *
 *  Unlike the Visual C/C++ 6.x tool chain, this compiler complies very
 *  closely with the C/C++ language specification.  This target may be
 *  necessary in situations that take full advantage of the C++ language.
 */
metaonly module Win32 inherits microsoft.targets.ITarget {

    override readonly config string name                = "Win32";
    override readonly config string os                  = "Windows";
    override readonly config string suffix              = "86W";
    override readonly config string isa                 = "x86";
    override readonly config xdc.bld.ITarget.Model model = {
        endian: "little"
    };

    override readonly config string rts = "microsoft.targets.rts";

    override config string platform     = "host.platforms.PC";
    override config string execExt      = ".exe";

    /*!
     *  ======== path ========
     *  Path components added during execution of commands
     */
    readonly config string path =
        "$(rootDir)/VC/bin;$(rootDir)/Common7/IDE";

    /*!
     *  ======== SDKPath ========
     *
     *  A structure type that defines Windows SDK paths to be used for building
     *  Windows applications with this target.
     *
     *  A Windows SDK installation is needed for building programs that use
     *  Windows API. However, Windows SDK installations can be anywhere on the
     *  disk and also include SDK version numbers (10.0.10586.0 for example).
     *  Therefore, these paths can't be derived from the target's rootDir.
     *
     *  @field(includePath) Absolute path to the Include directory of a Windows
     *  SDK installation. Windows SDK is required if the target is used to build
     *  executables that use Windows API.
     *
     *  @field(libraryPath) Absolute path to the Lib directory of a Windows
     *  SDK installation.
     *
     *  @field(libs) List of Windows SDK libraries to be added to the linker
     * command line.
     */
    struct SDKPath {
        string includePath[];
        string libraryPath[];
        string libs;
    };

    /*!
     *  ======== sdkPath ========
     *  Location of the header and library files in a Windows SDK installation
     *
     *  This parameter contains paths to the optional Windows SDK.
     *
     *  @a(Examples)
     *  If the content of `sdkPath`, set in `config.bld` is
     *  @p(code)
     *      Win32.sdkPath = {
     *          includePath: "C:/PROGRA~2/WI3CF2~1/10/Include/10.0.10586.0/ucrt",
     *          libraryPath: "C:/PROGRA~2/WI3CF2~1/10/Lib/10.0.10586.0/um/x86";
     *          libs: " libucrt.lib setargv.obj oldnames.lib ole32.lib
     * oleaut32.lib olepro32.lib uuid.lib kernel32.lib user32.lib gdi32.lib
     * advapi32.lib shell32.lib comctl32.lib mfc70.lib ucrt.lib"
     *      };
     *  the contents of `includePath`, `libraryPath` and `libs` are added to
     *  appropriate command lines.
     *
     *  If Windows SDK is not installed or needed, sdkPath's `includePath` and
     *  `libraryPath` should be empty.
     *  @p
     *
     */
    config SDKPath sdkPath = {
        includePath: [],
        libraryPath: [],
        libs: "ole32.lib oleaut32.lib olepro32.lib uuid.lib kernel32.lib user32.lib gdi32.lib advapi32.lib shell32.lib comctl32.lib"
    };

    /*!
     *  ======== profiles ========
     *  Standard options profiles are overwritten because we can't use 'MTs'
     *  and -D_DEBUG=1. Both of them added the debug version of the C runtime
     *  library to the manifest.
     */
    override config xdc.bld.ITarget.OptionSet profiles[string] = [
        ["debug", {
            compileOpts: {
                copts: "-Z7 -Odi -MTd",
                defs:  "",
            },
            linkOpts: "-debug msvcrtd.lib vcruntimed.lib ucrtd.lib oldnames.lib",
        }],

        ["release", {
            compileOpts: {
                copts: "-O2 -MT",
                defs:  "",
            },
            linkOpts: "msvcrt.lib vcruntime.lib ucrt.lib oldnames.lib",
        }],
    ];

    /*!
     *  ======== ar ========
     *  The archiver command and all required options
     *  @p(dlist)
     *      -`-nologo`
     *          don't display archiver copyright
     */
    override readonly config xdc.bld.ITarget2.Command ar = {
        cmd: "$(rootDir)/VC/bin/lib.exe -nologo",
        opts: ""
    };

    /*!
     *  ======== cc ========
     *  The compile command and all required options
     *  @p(dlist)
     *      -`-W3`
     *          enable all warnings recommended for production purposes.
     *      -`-c`
     *          don't link
     *      -`-nologo`
     *          don't display compiler copyright
     *      -`-Zp1`
     *          Packs structure members on 1-byte boundry
     */
    override readonly config xdc.bld.ITarget2.Command cc = {
        cmd: "$(rootDir)/VC/bin/cl.exe -nologo -c",
        opts: '-Zp1 -W3 -D_DLL -DWINVER=_WIN32_WINNT_WIN7 -D_WIN32_WINNT=_WIN32_WINNT_WIN7 -DWIN32 -DEXPORT=""'
    };

    /*!
     *  ======== asm ========
     *  The assemble command and all required options
     *  @p(dlist)
     *      -`-c`
     *          don't link
     *      -`-nologo`
     *          don't display macro assembler copyright
     */
    override readonly config xdc.bld.ITarget2.Command asm = {
        cmd: "$(rootDir)/VC/bin/ml -c",
        opts: "-nologo"
    };

    /*!
     *  ======== lnk ========
     *  The linker command and all required options
     *  @p(dlist)
     *      -`-nologo`
     *          Don't display linker copyright
     */
    override readonly config xdc.bld.ITarget2.Command lnk = {
        cmd: "$(rootDir)/VC/bin/link",
        opts: "-nologo"
    };

    /*!
     *  ======== ccOpts ========
     *  User modifiable default options.
     *  @p(dlist)
     *      -`-G5` (removed because it is incompatible with VC8.0)
     *          Optimizes code to favor the Pentium processor
     *      -`-Ob1`
     *          Expand only functions marked as inline or, in a C++
     *          member function, defined within a class declaration
     *      -`-Gs`
     *          Probe stack to automatically grow stack as necessary
     *      -`-GX` (removed because it is incompatible with VC8.0)
     *          Enables synchronous exception handling
     *      -`-WL`
     *          Write errors/warnings on a single output line
     */
    override config xdc.bld.ITarget2.Options ccOpts = {
        prefix: "-Ob1 -Gs",
        suffix: "-Dfar= "
    };

    /*!
     *  ======== lnkOpts ========
     *  User modifiable linker options
     *  @p(dlist)
     *      -`-libpath`
     *              directories to search for toolchain specific libraries
     *      -`-nodefaultlib`
     *              don't search for default libraries when linking; all
     *              libraries used must be explicitly named
     *      -`-incremental:no`
     *              link for execution (no subsequent link will occur)
     *      -`-machine:ix86`
     *              link for the Intel x86 architecture
     *      -`-map:$(XDCCFGDIR)/$@.map`
     *              output any link map information to the specified file
     *              ($(XDCCFGDIR) is usually package/cfg)
     *      -`-pdb:$(XDCCFGDIR)/$@.pdb`
     *              output any program debug information to the specified file
     *              ($(XDCCFGDIR) is usually package/cfg)
     */
    override config xdc.bld.ITarget2.Options lnkOpts = {
        prefix: "-libpath:$(rootDir)/VC/lib -libpath:$(rootDir)/VC/atlmfc/lib",
        suffix: "-map:$(XDCCFGDIR)/$@.map -pdb:$(XDCCFGDIR)/$@.pdb -machine:ix86 -nodefaultlib -incremental:no"
    };

    /*!
     *  ======== includeOpts ========
     *  User modifiable include paths
     *  @p(dlist)
     *      -`-I$(rootDir)/$(compilerDir)/include`
     *          include compiler specific headers
     */
    override config string includeOpts = "-I$(rootDir)/VC/include -I$(rootDir)/VC/atlmfc/include";
}
