/*
 *  Do not modify this file; it is automatically 
 *  generated and any modifications will be overwritten.
 *
 * @(#) xdc-H25
 */
import java.util.*;
import org.mozilla.javascript.*;
import xdc.services.intern.xsr.*;
import xdc.services.spec.Session;

public class microsoft_targets
{
    static final String VERS = "@(#) xdc-H25\n";

    static final Proto.Elm $$T_Bool = Proto.Elm.newBool();
    static final Proto.Elm $$T_Num = Proto.Elm.newNum();
    static final Proto.Elm $$T_Str = Proto.Elm.newStr();
    static final Proto.Elm $$T_Obj = Proto.Elm.newObj();

    static final Proto.Fxn $$T_Met = new Proto.Fxn(null, null, 0, -1, false);
    static final Proto.Map $$T_Map = new Proto.Map($$T_Obj);
    static final Proto.Arr $$T_Vec = new Proto.Arr($$T_Obj);

    static final XScriptO $$DEFAULT = Value.DEFAULT;
    static final Object $$UNDEF = Undefined.instance;

    static final Proto.Obj $$Package = (Proto.Obj)Global.get("$$Package");
    static final Proto.Obj $$Module = (Proto.Obj)Global.get("$$Module");
    static final Proto.Obj $$Instance = (Proto.Obj)Global.get("$$Instance");
    static final Proto.Obj $$Params = (Proto.Obj)Global.get("$$Params");

    static final Object $$objFldGet = Global.get("$$objFldGet");
    static final Object $$objFldSet = Global.get("$$objFldSet");
    static final Object $$proxyGet = Global.get("$$proxyGet");
    static final Object $$proxySet = Global.get("$$proxySet");
    static final Object $$delegGet = Global.get("$$delegGet");
    static final Object $$delegSet = Global.get("$$delegSet");

    Scriptable xdcO;
    Session ses;
    Value.Obj om;

    boolean isROV;
    boolean isCFG;

    Proto.Obj pkgP;
    Value.Obj pkgV;

    ArrayList<Object> imports = new ArrayList<Object>();
    ArrayList<Object> loggables = new ArrayList<Object>();
    ArrayList<Object> mcfgs = new ArrayList<Object>();
    ArrayList<Object> icfgs = new ArrayList<Object>();
    ArrayList<String> inherits = new ArrayList<String>();
    ArrayList<Object> proxies = new ArrayList<Object>();
    ArrayList<Object> sizes = new ArrayList<Object>();
    ArrayList<Object> tdefs = new ArrayList<Object>();

    void $$IMPORTS()
    {
        Global.callFxn("loadPackage", xdcO, "xdc");
        Global.callFxn("loadPackage", xdcO, "xdc.corevers");
        Global.callFxn("loadPackage", xdcO, "xdc.bld");
    }

    void $$OBJECTS()
    {
        pkgP = (Proto.Obj)om.bind("microsoft.targets.Package", new Proto.Obj());
        pkgV = (Value.Obj)om.bind("microsoft.targets", new Value.Obj("microsoft.targets", pkgP));
    }

    void ITarget$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("microsoft.targets.ITarget.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("microsoft.targets.ITarget", new Value.Obj("microsoft.targets.ITarget", po));
        pkgV.bind("ITarget", vo);
        // decls 
        om.bind("microsoft.targets.ITarget.Model", om.findStrict("xdc.bld.ITarget.Model", "microsoft.targets"));
        om.bind("microsoft.targets.ITarget.DebugGen", om.findStrict("xdc.bld.ITarget.DebugGen", "microsoft.targets"));
        om.bind("microsoft.targets.ITarget.Extension", om.findStrict("xdc.bld.ITarget.Extension", "microsoft.targets"));
        om.bind("microsoft.targets.ITarget.CompileOptions", om.findStrict("xdc.bld.ITarget.CompileOptions", "microsoft.targets"));
        om.bind("microsoft.targets.ITarget.OptionSet", om.findStrict("xdc.bld.ITarget.OptionSet", "microsoft.targets"));
        om.bind("microsoft.targets.ITarget.CompileGoal", om.findStrict("xdc.bld.ITarget.CompileGoal", "microsoft.targets"));
        om.bind("microsoft.targets.ITarget.LinkGoal", om.findStrict("xdc.bld.ITarget.LinkGoal", "microsoft.targets"));
        om.bind("microsoft.targets.ITarget.ArchiveGoal", om.findStrict("xdc.bld.ITarget.ArchiveGoal", "microsoft.targets"));
        om.bind("microsoft.targets.ITarget.CommandSet", om.findStrict("xdc.bld.ITarget.CommandSet", "microsoft.targets"));
        om.bind("microsoft.targets.ITarget.TypeInfo", om.findStrict("xdc.bld.ITarget.TypeInfo", "microsoft.targets"));
        om.bind("microsoft.targets.ITarget.StdTypes", om.findStrict("xdc.bld.ITarget.StdTypes", "microsoft.targets"));
        om.bind("microsoft.targets.ITarget.Command", om.findStrict("xdc.bld.ITarget2.Command", "microsoft.targets"));
        om.bind("microsoft.targets.ITarget.Options", om.findStrict("xdc.bld.ITarget2.Options", "microsoft.targets"));
    }

    void Win32$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("microsoft.targets.Win32.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("microsoft.targets.Win32", new Value.Obj("microsoft.targets.Win32", po));
        pkgV.bind("Win32", vo);
        // decls 
        om.bind("microsoft.targets.Win32.Model", om.findStrict("xdc.bld.ITarget.Model", "microsoft.targets"));
        om.bind("microsoft.targets.Win32.DebugGen", om.findStrict("xdc.bld.ITarget.DebugGen", "microsoft.targets"));
        om.bind("microsoft.targets.Win32.Extension", om.findStrict("xdc.bld.ITarget.Extension", "microsoft.targets"));
        om.bind("microsoft.targets.Win32.CompileOptions", om.findStrict("xdc.bld.ITarget.CompileOptions", "microsoft.targets"));
        om.bind("microsoft.targets.Win32.OptionSet", om.findStrict("xdc.bld.ITarget.OptionSet", "microsoft.targets"));
        om.bind("microsoft.targets.Win32.CompileGoal", om.findStrict("xdc.bld.ITarget.CompileGoal", "microsoft.targets"));
        om.bind("microsoft.targets.Win32.LinkGoal", om.findStrict("xdc.bld.ITarget.LinkGoal", "microsoft.targets"));
        om.bind("microsoft.targets.Win32.ArchiveGoal", om.findStrict("xdc.bld.ITarget.ArchiveGoal", "microsoft.targets"));
        om.bind("microsoft.targets.Win32.CommandSet", om.findStrict("xdc.bld.ITarget.CommandSet", "microsoft.targets"));
        om.bind("microsoft.targets.Win32.TypeInfo", om.findStrict("xdc.bld.ITarget.TypeInfo", "microsoft.targets"));
        om.bind("microsoft.targets.Win32.StdTypes", om.findStrict("xdc.bld.ITarget.StdTypes", "microsoft.targets"));
        om.bind("microsoft.targets.Win32.Command", om.findStrict("xdc.bld.ITarget2.Command", "microsoft.targets"));
        om.bind("microsoft.targets.Win32.Options", om.findStrict("xdc.bld.ITarget2.Options", "microsoft.targets"));
        spo = (Proto.Obj)om.bind("microsoft.targets.Win32$$SDKPath", new Proto.Obj());
        om.bind("microsoft.targets.Win32.SDKPath", new Proto.Str(spo, true));
    }

    void Win64$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("microsoft.targets.Win64.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("microsoft.targets.Win64", new Value.Obj("microsoft.targets.Win64", po));
        pkgV.bind("Win64", vo);
        // decls 
        om.bind("microsoft.targets.Win64.Model", om.findStrict("xdc.bld.ITarget.Model", "microsoft.targets"));
        om.bind("microsoft.targets.Win64.DebugGen", om.findStrict("xdc.bld.ITarget.DebugGen", "microsoft.targets"));
        om.bind("microsoft.targets.Win64.Extension", om.findStrict("xdc.bld.ITarget.Extension", "microsoft.targets"));
        om.bind("microsoft.targets.Win64.CompileOptions", om.findStrict("xdc.bld.ITarget.CompileOptions", "microsoft.targets"));
        om.bind("microsoft.targets.Win64.OptionSet", om.findStrict("xdc.bld.ITarget.OptionSet", "microsoft.targets"));
        om.bind("microsoft.targets.Win64.CompileGoal", om.findStrict("xdc.bld.ITarget.CompileGoal", "microsoft.targets"));
        om.bind("microsoft.targets.Win64.LinkGoal", om.findStrict("xdc.bld.ITarget.LinkGoal", "microsoft.targets"));
        om.bind("microsoft.targets.Win64.ArchiveGoal", om.findStrict("xdc.bld.ITarget.ArchiveGoal", "microsoft.targets"));
        om.bind("microsoft.targets.Win64.CommandSet", om.findStrict("xdc.bld.ITarget.CommandSet", "microsoft.targets"));
        om.bind("microsoft.targets.Win64.TypeInfo", om.findStrict("xdc.bld.ITarget.TypeInfo", "microsoft.targets"));
        om.bind("microsoft.targets.Win64.StdTypes", om.findStrict("xdc.bld.ITarget.StdTypes", "microsoft.targets"));
        om.bind("microsoft.targets.Win64.Command", om.findStrict("xdc.bld.ITarget2.Command", "microsoft.targets"));
        om.bind("microsoft.targets.Win64.Options", om.findStrict("xdc.bld.ITarget2.Options", "microsoft.targets"));
        spo = (Proto.Obj)om.bind("microsoft.targets.Win64$$SDKPath", new Proto.Obj());
        om.bind("microsoft.targets.Win64.SDKPath", new Proto.Str(spo, true));
    }

    void Net32$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("microsoft.targets.Net32.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("microsoft.targets.Net32", new Value.Obj("microsoft.targets.Net32", po));
        pkgV.bind("Net32", vo);
        // decls 
        om.bind("microsoft.targets.Net32.Model", om.findStrict("xdc.bld.ITarget.Model", "microsoft.targets"));
        om.bind("microsoft.targets.Net32.DebugGen", om.findStrict("xdc.bld.ITarget.DebugGen", "microsoft.targets"));
        om.bind("microsoft.targets.Net32.Extension", om.findStrict("xdc.bld.ITarget.Extension", "microsoft.targets"));
        om.bind("microsoft.targets.Net32.CompileOptions", om.findStrict("xdc.bld.ITarget.CompileOptions", "microsoft.targets"));
        om.bind("microsoft.targets.Net32.OptionSet", om.findStrict("xdc.bld.ITarget.OptionSet", "microsoft.targets"));
        om.bind("microsoft.targets.Net32.CompileGoal", om.findStrict("xdc.bld.ITarget.CompileGoal", "microsoft.targets"));
        om.bind("microsoft.targets.Net32.LinkGoal", om.findStrict("xdc.bld.ITarget.LinkGoal", "microsoft.targets"));
        om.bind("microsoft.targets.Net32.ArchiveGoal", om.findStrict("xdc.bld.ITarget.ArchiveGoal", "microsoft.targets"));
        om.bind("microsoft.targets.Net32.CommandSet", om.findStrict("xdc.bld.ITarget.CommandSet", "microsoft.targets"));
        om.bind("microsoft.targets.Net32.TypeInfo", om.findStrict("xdc.bld.ITarget.TypeInfo", "microsoft.targets"));
        om.bind("microsoft.targets.Net32.StdTypes", om.findStrict("xdc.bld.ITarget.StdTypes", "microsoft.targets"));
        om.bind("microsoft.targets.Net32.Command", om.findStrict("xdc.bld.ITarget2.Command", "microsoft.targets"));
        om.bind("microsoft.targets.Net32.Options", om.findStrict("xdc.bld.ITarget2.Options", "microsoft.targets"));
    }

    void ITarget$$CONSTS()
    {
        // interface ITarget
    }

    void Win32$$CONSTS()
    {
        // module Win32
    }

    void Win64$$CONSTS()
    {
        // module Win64
    }

    void Net32$$CONSTS()
    {
        // module Net32
    }

    void ITarget$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

    }

    void Win32$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

    }

    void Win64$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

    }

    void Net32$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

    }

    void ITarget$$FUNCTIONS()
    {
        Proto.Fxn fxn;

        // fxn ITarget.setPath
        fxn = (Proto.Fxn)om.bind("microsoft.targets.ITarget$$setPath", new Proto.Fxn(om.findStrict("microsoft.targets.ITarget.Module", "microsoft.targets"), null, 0, -1, false));
    }

    void Win32$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void Win64$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void Net32$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void ITarget$$SIZES()
    {
    }

    void Win32$$SIZES()
    {
    }

    void Win64$$SIZES()
    {
    }

    void Net32$$SIZES()
    {
    }

    void ITarget$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "microsoft/targets/ITarget.xs");
        om.bind("microsoft.targets.ITarget$$capsule", cap);
        po = (Proto.Obj)om.findStrict("microsoft.targets.ITarget.Module", "microsoft.targets");
        po.init("microsoft.targets.ITarget.Module", om.findStrict("xdc.bld.ITarget3.Module", "microsoft.targets"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("stdInclude", $$T_Str, "microsoft/targets/std.h", "rh");
        po.addFld("dllExt", $$T_Str, ".dll", "wh");
        po.addFld("debugGen", (Proto)om.findStrict("xdc.bld.ITarget.DebugGen", "microsoft.targets"), Global.newObject("execTemplate", "microsoft/targets/vc_exec.xdt", "execPattern", "$(cfgName).vcproj"), "wh");
        po.addFld("profiles", new Proto.Map((Proto)om.findStrict("xdc.bld.ITarget.OptionSet", "microsoft.targets")), Global.newArray(new Object[]{Global.newArray(new Object[]{"debug", Global.newObject("compileOpts", Global.newObject("copts", "-Z7 -Odi -MTd", "defs", "-D_DEBUG=1"), "linkOpts", "-debug msvcrt.lib setargv.obj oldnames.lib ole32.lib oleaut32.lib olepro32.lib uuid.lib kernel32.lib user32.lib gdi32.lib advapi32.lib shell32.lib comctl32.lib")}), Global.newArray(new Object[]{"release", Global.newObject("compileOpts", Global.newObject("copts", "-O2 -MT", "defs", ""), "linkOpts", "msvcrt.lib setargv.obj oldnames.lib ole32.lib oleaut32.lib olepro32.lib uuid.lib kernel32.lib user32.lib gdi32.lib advapi32.lib shell32.lib comctl32.lib")})}), "wh");
        po.addFld("cmdPrefix", $$T_Str, "", "wh");
        po.addFld("strictANSI", $$T_Bool, true, "wh");
        po.addFld("version", $$T_Str, $$UNDEF, "wh");
        po.addFld("stdTypes", (Proto)om.findStrict("xdc.bld.ITarget.StdTypes", "microsoft.targets"), Global.newObject("t_IArg", Global.newObject("size", 4L, "align", 1L), "t_Char", Global.newObject("size", 1L, "align", 1L), "t_Double", Global.newObject("size", 8L, "align", 1L), "t_Float", Global.newObject("size", 4L, "align", 1L), "t_Fxn", Global.newObject("size", 4L, "align", 1L), "t_Int", Global.newObject("size", 4L, "align", 1L), "t_Int8", Global.newObject("size", 1L, "align", 1L), "t_Int16", Global.newObject("size", 2L, "align", 1L), "t_Int32", Global.newObject("size", 4L, "align", 1L), "t_Int64", Global.newObject("size", 8L, "align", 1L), "t_Long", Global.newObject("size", 4L, "align", 1L), "t_LDouble", Global.newObject("size", 8L, "align", 1L), "t_Ptr", Global.newObject("size", 4L, "align", 1L), "t_Short", Global.newObject("size", 2L, "align", 1L), "t_Size", Global.newObject("size", 4L, "align", 1L)), "rh");
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("microsoft.targets.ITarget$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("microsoft.targets.ITarget$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("microsoft.targets.ITarget$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
                fxn = Global.get(cap, "archive");
                if (fxn != null) po.addFxn("archive", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$archive", "microsoft.targets"), fxn);
                fxn = Global.get(cap, "compile");
                if (fxn != null) po.addFxn("compile", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$compile", "microsoft.targets"), fxn);
                fxn = Global.get(cap, "scompile");
                if (fxn != null) po.addFxn("scompile", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$scompile", "microsoft.targets"), fxn);
                fxn = Global.get(cap, "link");
                if (fxn != null) po.addFxn("link", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$link", "microsoft.targets"), fxn);
                fxn = Global.get(cap, "getVersion");
                if (fxn != null) po.addFxn("getVersion", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$getVersion", "microsoft.targets"), fxn);
                fxn = Global.get(cap, "getISAChain");
                if (fxn != null) po.addFxn("getISAChain", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$getISAChain", "microsoft.targets"), fxn);
                fxn = Global.get(cap, "findSuffix");
                if (fxn != null) po.addFxn("findSuffix", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$findSuffix", "microsoft.targets"), fxn);
                fxn = Global.get(cap, "selectSuffix");
                if (fxn != null) po.addFxn("selectSuffix", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$selectSuffix", "microsoft.targets"), fxn);
                fxn = Global.get(cap, "genConstCustom");
                if (fxn != null) po.addFxn("genConstCustom", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genConstCustom", "microsoft.targets"), fxn);
                fxn = Global.get(cap, "genVisibleData");
                if (fxn != null) po.addFxn("genVisibleData", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genVisibleData", "microsoft.targets"), fxn);
                fxn = Global.get(cap, "genVisibleFxns");
                if (fxn != null) po.addFxn("genVisibleFxns", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genVisibleFxns", "microsoft.targets"), fxn);
                fxn = Global.get(cap, "genVisibleLibFxns");
                if (fxn != null) po.addFxn("genVisibleLibFxns", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genVisibleLibFxns", "microsoft.targets"), fxn);
                fxn = Global.get(cap, "getRawVersion");
                if (fxn != null) po.addFxn("getRawVersion", (Proto.Fxn)om.findStrict("xdc.bld.ITarget3$$getRawVersion", "microsoft.targets"), fxn);
                fxn = Global.get(cap, "setPath");
                if (fxn != null) po.addFxn("setPath", (Proto.Fxn)om.findStrict("microsoft.targets.ITarget$$setPath", "microsoft.targets"), fxn);
    }

    void Win32$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "microsoft/targets/Win32.xs");
        om.bind("microsoft.targets.Win32$$capsule", cap);
        po = (Proto.Obj)om.findStrict("microsoft.targets.Win32.Module", "microsoft.targets");
        po.init("microsoft.targets.Win32.Module", om.findStrict("microsoft.targets.ITarget.Module", "microsoft.targets"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("name", $$T_Str, "Win32", "rh");
        po.addFld("os", $$T_Str, "Windows", "rh");
        po.addFld("suffix", $$T_Str, "86W", "rh");
        po.addFld("isa", $$T_Str, "x86", "rh");
        po.addFld("model", (Proto)om.findStrict("xdc.bld.ITarget.Model", "microsoft.targets"), Global.newObject("endian", "little"), "rh");
        po.addFld("rts", $$T_Str, "microsoft.targets.rts", "rh");
        po.addFld("platform", $$T_Str, "host.platforms.PC", "wh");
        po.addFld("execExt", $$T_Str, ".exe", "wh");
        po.addFld("path", $$T_Str, "$(rootDir)/VC/bin;$(rootDir)/Common7/IDE", "rh");
        po.addFld("sdkPath", (Proto)om.findStrict("microsoft.targets.Win32.SDKPath", "microsoft.targets"), Global.newObject("includePath", Global.newArray(new Object[]{}), "libraryPath", Global.newArray(new Object[]{}), "libs", "ole32.lib oleaut32.lib olepro32.lib uuid.lib kernel32.lib user32.lib gdi32.lib advapi32.lib shell32.lib comctl32.lib"), "wh");
        po.addFld("profiles", new Proto.Map((Proto)om.findStrict("xdc.bld.ITarget.OptionSet", "microsoft.targets")), Global.newArray(new Object[]{Global.newArray(new Object[]{"debug", Global.newObject("compileOpts", Global.newObject("copts", "-Z7 -Odi -MTd", "defs", ""), "linkOpts", "-debug msvcrtd.lib vcruntimed.lib ucrtd.lib oldnames.lib")}), Global.newArray(new Object[]{"release", Global.newObject("compileOpts", Global.newObject("copts", "-O2 -MT", "defs", ""), "linkOpts", "msvcrt.lib vcruntime.lib ucrt.lib oldnames.lib")})}), "wh");
        po.addFld("ar", (Proto)om.findStrict("xdc.bld.ITarget2.Command", "microsoft.targets"), Global.newObject("cmd", "$(rootDir)/VC/bin/lib.exe -nologo", "opts", ""), "rh");
        po.addFld("cc", (Proto)om.findStrict("xdc.bld.ITarget2.Command", "microsoft.targets"), Global.newObject("cmd", "$(rootDir)/VC/bin/cl.exe -nologo -c", "opts", "-Zp1 -W3 -D_DLL -DWINVER=_WIN32_WINNT_WIN7 -D_WIN32_WINNT=_WIN32_WINNT_WIN7 -DWIN32 -DEXPORT=\"\""), "rh");
        po.addFld("asm", (Proto)om.findStrict("xdc.bld.ITarget2.Command", "microsoft.targets"), Global.newObject("cmd", "$(rootDir)/VC/bin/ml -c", "opts", "-nologo"), "rh");
        po.addFld("lnk", (Proto)om.findStrict("xdc.bld.ITarget2.Command", "microsoft.targets"), Global.newObject("cmd", "$(rootDir)/VC/bin/link", "opts", "-nologo"), "rh");
        po.addFld("ccOpts", (Proto)om.findStrict("xdc.bld.ITarget2.Options", "microsoft.targets"), Global.newObject("prefix", "-Ob1 -Gs", "suffix", "-Dfar= "), "wh");
        po.addFld("lnkOpts", (Proto)om.findStrict("xdc.bld.ITarget2.Options", "microsoft.targets"), Global.newObject("prefix", "-libpath:$(rootDir)/VC/lib -libpath:$(rootDir)/VC/atlmfc/lib", "suffix", "-map:$(XDCCFGDIR)/$@.map -pdb:$(XDCCFGDIR)/$@.pdb -machine:ix86 -nodefaultlib -incremental:no"), "wh");
        po.addFld("includeOpts", $$T_Str, "-I$(rootDir)/VC/include -I$(rootDir)/VC/atlmfc/include", "wh");
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("microsoft.targets.Win32$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("microsoft.targets.Win32$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("microsoft.targets.Win32$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
                fxn = Global.get(cap, "archive");
                if (fxn != null) po.addFxn("archive", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$archive", "microsoft.targets"), fxn);
                fxn = Global.get(cap, "compile");
                if (fxn != null) po.addFxn("compile", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$compile", "microsoft.targets"), fxn);
                fxn = Global.get(cap, "scompile");
                if (fxn != null) po.addFxn("scompile", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$scompile", "microsoft.targets"), fxn);
                fxn = Global.get(cap, "link");
                if (fxn != null) po.addFxn("link", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$link", "microsoft.targets"), fxn);
                fxn = Global.get(cap, "getVersion");
                if (fxn != null) po.addFxn("getVersion", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$getVersion", "microsoft.targets"), fxn);
                fxn = Global.get(cap, "getISAChain");
                if (fxn != null) po.addFxn("getISAChain", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$getISAChain", "microsoft.targets"), fxn);
                fxn = Global.get(cap, "findSuffix");
                if (fxn != null) po.addFxn("findSuffix", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$findSuffix", "microsoft.targets"), fxn);
                fxn = Global.get(cap, "selectSuffix");
                if (fxn != null) po.addFxn("selectSuffix", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$selectSuffix", "microsoft.targets"), fxn);
                fxn = Global.get(cap, "genConstCustom");
                if (fxn != null) po.addFxn("genConstCustom", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genConstCustom", "microsoft.targets"), fxn);
                fxn = Global.get(cap, "genVisibleData");
                if (fxn != null) po.addFxn("genVisibleData", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genVisibleData", "microsoft.targets"), fxn);
                fxn = Global.get(cap, "genVisibleFxns");
                if (fxn != null) po.addFxn("genVisibleFxns", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genVisibleFxns", "microsoft.targets"), fxn);
                fxn = Global.get(cap, "genVisibleLibFxns");
                if (fxn != null) po.addFxn("genVisibleLibFxns", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genVisibleLibFxns", "microsoft.targets"), fxn);
                fxn = Global.get(cap, "getRawVersion");
                if (fxn != null) po.addFxn("getRawVersion", (Proto.Fxn)om.findStrict("xdc.bld.ITarget3$$getRawVersion", "microsoft.targets"), fxn);
                fxn = Global.get(cap, "setPath");
                if (fxn != null) po.addFxn("setPath", (Proto.Fxn)om.findStrict("microsoft.targets.ITarget$$setPath", "microsoft.targets"), fxn);
        // struct Win32.SDKPath
        po = (Proto.Obj)om.findStrict("microsoft.targets.Win32$$SDKPath", "microsoft.targets");
        po.init("microsoft.targets.Win32.SDKPath", null);
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFld("includePath", new Proto.Arr($$T_Str, false), $$DEFAULT, "w");
                po.addFld("libraryPath", new Proto.Arr($$T_Str, false), $$DEFAULT, "w");
                po.addFld("libs", $$T_Str, $$UNDEF, "w");
    }

    void Win64$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "microsoft/targets/Win64.xs");
        om.bind("microsoft.targets.Win64$$capsule", cap);
        po = (Proto.Obj)om.findStrict("microsoft.targets.Win64.Module", "microsoft.targets");
        po.init("microsoft.targets.Win64.Module", om.findStrict("microsoft.targets.ITarget.Module", "microsoft.targets"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("name", $$T_Str, "Win64", "rh");
        po.addFld("os", $$T_Str, "Windows", "rh");
        po.addFld("suffix", $$T_Str, "64W", "rh");
        po.addFld("isa", $$T_Str, "x64", "rh");
        po.addFld("model", (Proto)om.findStrict("xdc.bld.ITarget.Model", "microsoft.targets"), Global.newObject("endian", "little"), "rh");
        po.addFld("rts", $$T_Str, "microsoft.targets.rts", "rh");
        po.addFld("platform", $$T_Str, "host.platforms.PC", "wh");
        po.addFld("execExt", $$T_Str, ".exe", "wh");
        po.addFld("path", $$T_Str, "$(rootDir)/VC/bin/amd64;$(rootDir)/Common7/IDE", "rh");
        po.addFld("sdkPath", (Proto)om.findStrict("microsoft.targets.Win64.SDKPath", "microsoft.targets"), Global.newObject("includePath", Global.newArray(new Object[]{}), "libraryPath", Global.newArray(new Object[]{}), "libs", "ole32.lib oleaut32.lib uuid.lib kernel32.lib user32.lib gdi32.lib advapi32.lib shell32.lib comctl32.lib"), "wh");
        po.addFld("profiles", new Proto.Map((Proto)om.findStrict("xdc.bld.ITarget.OptionSet", "microsoft.targets")), Global.newArray(new Object[]{Global.newArray(new Object[]{"debug", Global.newObject("compileOpts", Global.newObject("copts", "-Z7 -Odi -MTd", "defs", ""), "linkOpts", "-debug msvcrtd.lib vcruntimed.lib ucrtd.lib oldnames.lib")}), Global.newArray(new Object[]{"release", Global.newObject("compileOpts", Global.newObject("copts", "-O2 -MT", "defs", ""), "linkOpts", "msvcrt.lib vcruntime.lib ucrt.lib oldnames.lib")})}), "wh");
        po.addFld("ar", (Proto)om.findStrict("xdc.bld.ITarget2.Command", "microsoft.targets"), Global.newObject("cmd", "$(rootDir)/VC/bin/amd64/lib.exe -nologo", "opts", ""), "rh");
        po.addFld("cc", (Proto)om.findStrict("xdc.bld.ITarget2.Command", "microsoft.targets"), Global.newObject("cmd", "$(rootDir)/VC/bin/amd64/cl.exe -nologo -c", "opts", "-Zp1 -W3 -D_DLL -DWINVER=_WIN32_WINNT_WIN7 -D_WIN32_WINNT=_WIN32_WINNT_WIN7 -DWIN64 -DEXPORT=\"\""), "rh");
        po.addFld("asm", (Proto)om.findStrict("xdc.bld.ITarget2.Command", "microsoft.targets"), Global.newObject("cmd", "$(rootDir)/VC/bin/amd64/ml64 -c", "opts", "-nologo"), "rh");
        po.addFld("lnk", (Proto)om.findStrict("xdc.bld.ITarget2.Command", "microsoft.targets"), Global.newObject("cmd", "$(rootDir)/VC/bin/amd64/link", "opts", "-nologo"), "rh");
        po.addFld("ccOpts", (Proto)om.findStrict("xdc.bld.ITarget2.Options", "microsoft.targets"), Global.newObject("prefix", "-Ob1 -Gs", "suffix", "-Dfar= "), "wh");
        po.addFld("lnkOpts", (Proto)om.findStrict("xdc.bld.ITarget2.Options", "microsoft.targets"), Global.newObject("prefix", "-libpath:$(rootDir)/VC/lib/amd64 -libpath:$(rootDir)/VC/atlmfc/lib/amd64", "suffix", "-map:$(XDCCFGDIR)/$@.map -pdb:$(XDCCFGDIR)/$@.pdb -machine:x64 -nodefaultlib -incremental:no"), "wh");
        po.addFld("includeOpts", $$T_Str, "-I$(rootDir)/VC/include -I$(rootDir)/VC/atlmfc/include", "wh");
        po.addFld("stdTypes", (Proto)om.findStrict("xdc.bld.ITarget.StdTypes", "microsoft.targets"), Global.newObject("t_IArg", Global.newObject("size", 8L, "align", 1L), "t_Char", Global.newObject("size", 1L, "align", 1L), "t_Double", Global.newObject("size", 8L, "align", 1L), "t_Float", Global.newObject("size", 4L, "align", 1L), "t_Fxn", Global.newObject("size", 8L, "align", 1L), "t_Int", Global.newObject("size", 4L, "align", 1L), "t_Int8", Global.newObject("size", 1L, "align", 1L), "t_Int16", Global.newObject("size", 2L, "align", 1L), "t_Int32", Global.newObject("size", 4L, "align", 1L), "t_Int64", Global.newObject("size", 8L, "align", 1L), "t_Long", Global.newObject("size", 4L, "align", 1L), "t_LLong", Global.newObject("size", 8L, "align", 1L), "t_LDouble", Global.newObject("size", 8L, "align", 1L), "t_Ptr", Global.newObject("size", 8L, "align", 1L), "t_Short", Global.newObject("size", 2L, "align", 1L), "t_Size", Global.newObject("size", 8L, "align", 1L)), "rh");
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("microsoft.targets.Win64$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("microsoft.targets.Win64$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("microsoft.targets.Win64$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
                fxn = Global.get(cap, "archive");
                if (fxn != null) po.addFxn("archive", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$archive", "microsoft.targets"), fxn);
                fxn = Global.get(cap, "compile");
                if (fxn != null) po.addFxn("compile", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$compile", "microsoft.targets"), fxn);
                fxn = Global.get(cap, "scompile");
                if (fxn != null) po.addFxn("scompile", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$scompile", "microsoft.targets"), fxn);
                fxn = Global.get(cap, "link");
                if (fxn != null) po.addFxn("link", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$link", "microsoft.targets"), fxn);
                fxn = Global.get(cap, "getVersion");
                if (fxn != null) po.addFxn("getVersion", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$getVersion", "microsoft.targets"), fxn);
                fxn = Global.get(cap, "getISAChain");
                if (fxn != null) po.addFxn("getISAChain", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$getISAChain", "microsoft.targets"), fxn);
                fxn = Global.get(cap, "findSuffix");
                if (fxn != null) po.addFxn("findSuffix", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$findSuffix", "microsoft.targets"), fxn);
                fxn = Global.get(cap, "selectSuffix");
                if (fxn != null) po.addFxn("selectSuffix", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$selectSuffix", "microsoft.targets"), fxn);
                fxn = Global.get(cap, "genConstCustom");
                if (fxn != null) po.addFxn("genConstCustom", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genConstCustom", "microsoft.targets"), fxn);
                fxn = Global.get(cap, "genVisibleData");
                if (fxn != null) po.addFxn("genVisibleData", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genVisibleData", "microsoft.targets"), fxn);
                fxn = Global.get(cap, "genVisibleFxns");
                if (fxn != null) po.addFxn("genVisibleFxns", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genVisibleFxns", "microsoft.targets"), fxn);
                fxn = Global.get(cap, "genVisibleLibFxns");
                if (fxn != null) po.addFxn("genVisibleLibFxns", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genVisibleLibFxns", "microsoft.targets"), fxn);
                fxn = Global.get(cap, "getRawVersion");
                if (fxn != null) po.addFxn("getRawVersion", (Proto.Fxn)om.findStrict("xdc.bld.ITarget3$$getRawVersion", "microsoft.targets"), fxn);
                fxn = Global.get(cap, "setPath");
                if (fxn != null) po.addFxn("setPath", (Proto.Fxn)om.findStrict("microsoft.targets.ITarget$$setPath", "microsoft.targets"), fxn);
        // struct Win64.SDKPath
        po = (Proto.Obj)om.findStrict("microsoft.targets.Win64$$SDKPath", "microsoft.targets");
        po.init("microsoft.targets.Win64.SDKPath", null);
                po.addFld("$hostonly", $$T_Num, 1, "r");
                po.addFld("includePath", new Proto.Arr($$T_Str, false), $$DEFAULT, "w");
                po.addFld("libraryPath", new Proto.Arr($$T_Str, false), $$DEFAULT, "w");
                po.addFld("libs", $$T_Str, $$UNDEF, "w");
    }

    void Net32$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        po = (Proto.Obj)om.findStrict("microsoft.targets.Net32.Module", "microsoft.targets");
        po.init("microsoft.targets.Net32.Module", om.findStrict("microsoft.targets.ITarget.Module", "microsoft.targets"));
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("name", $$T_Str, "Net32", "rh");
        po.addFld("os", $$T_Str, "Windows", "rh");
        po.addFld("suffix", $$T_Str, "86N", "rh");
        po.addFld("isa", $$T_Str, "x86", "rh");
        po.addFld("model", (Proto)om.findStrict("xdc.bld.ITarget.Model", "microsoft.targets"), Global.newObject("endian", "little"), "rh");
        po.addFld("profiles", new Proto.Map((Proto)om.findStrict("xdc.bld.ITarget.OptionSet", "microsoft.targets")), Global.newArray(new Object[]{Global.newArray(new Object[]{"debug", Global.newObject("compileOpts", Global.newObject("copts", "-Z7 -Od -MTd", "defs", "-D_DEBUG=1"), "linkOpts", "libcmt.lib kernel32.lib")}), Global.newArray(new Object[]{"release", Global.newObject("compileOpts", Global.newObject("copts", "-Oxi -MT"), "linkOpts", "libcmt.lib kernel32.lib")})}), "wh");
        po.addFld("rts", $$T_Str, "microsoft.targets.rts", "rh");
        po.addFld("platform", $$T_Str, "host.platforms.PC", "wh");
        po.addFld("execExt", $$T_Str, ".exe", "wh");
        po.addFld("ar", (Proto)om.findStrict("xdc.bld.ITarget2.Command", "microsoft.targets"), Global.newObject("cmd", "$(rootDir)/bin/link.exe -lib -nologo", "opts", ""), "rh");
        po.addFld("cc", (Proto)om.findStrict("xdc.bld.ITarget2.Command", "microsoft.targets"), Global.newObject("cmd", "$(rootDir)/bin/cl", "opts", "-nologo -c"), "rh");
        po.addFld("asm", (Proto)om.findStrict("xdc.bld.ITarget2.Command", "microsoft.targets"), Global.newObject("cmd", "$(rootDir)/bin/cl", "opts", "-nologo -c"), "rh");
        po.addFld("lnk", (Proto)om.findStrict("xdc.bld.ITarget2.Command", "microsoft.targets"), Global.newObject("cmd", "$(rootDir)/bin/link -nologo", "opts", ""), "rh");
        po.addFld("ccOpts", (Proto)om.findStrict("xdc.bld.ITarget2.Options", "microsoft.targets"), Global.newObject("prefix", "-G5 -W3"), "wh");
        po.addFld("lnkOpts", (Proto)om.findStrict("xdc.bld.ITarget2.Options", "microsoft.targets"), Global.newObject("prefix", "-libpath:$(rootDir)/lib", "suffix", "-map:$(XDCCFGDIR)/$@.map -pdb:$(XDCCFGDIR)/$@.pdb -mapinfo:exports -machine:ix86 -nodefaultlib -incremental:no"), "wh");
        po.addFld("includeOpts", $$T_Str, "-I$(rootDir)/include", "wh");
        po.addFld("stdTypes", (Proto)om.findStrict("xdc.bld.ITarget.StdTypes", "microsoft.targets"), Global.newObject("t_IArg", Global.newObject("size", 4L, "align", 4L), "t_Char", Global.newObject("size", 1L, "align", 1L), "t_Double", Global.newObject("size", 8L, "align", 8L), "t_Float", Global.newObject("size", 4L, "align", 4L), "t_Fxn", Global.newObject("size", 4L, "align", 4L), "t_Int", Global.newObject("size", 4L, "align", 4L), "t_Int8", Global.newObject("size", 1L, "align", 1L), "t_Int16", Global.newObject("size", 2L, "align", 2L), "t_Int32", Global.newObject("size", 4L, "align", 4L), "t_Int64", Global.newObject("size", 8L, "align", 8L), "t_Long", Global.newObject("size", 4L, "align", 4L), "t_LDouble", Global.newObject("size", 8L, "align", 8L), "t_Ptr", Global.newObject("size", 4L, "align", 4L), "t_Short", Global.newObject("size", 2L, "align", 2L), "t_Size", Global.newObject("size", 4L, "align", 4L)), "rh");
                po.addFxn("archive", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$archive", "microsoft.targets"), $$UNDEF);
                po.addFxn("compile", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$compile", "microsoft.targets"), $$UNDEF);
                po.addFxn("scompile", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$scompile", "microsoft.targets"), $$UNDEF);
                po.addFxn("link", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$link", "microsoft.targets"), $$UNDEF);
                po.addFxn("getVersion", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$getVersion", "microsoft.targets"), $$UNDEF);
                po.addFxn("getISAChain", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$getISAChain", "microsoft.targets"), $$UNDEF);
                po.addFxn("findSuffix", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$findSuffix", "microsoft.targets"), $$UNDEF);
                po.addFxn("selectSuffix", (Proto.Fxn)om.findStrict("xdc.bld.ITarget$$selectSuffix", "microsoft.targets"), $$UNDEF);
                po.addFxn("genConstCustom", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genConstCustom", "microsoft.targets"), $$UNDEF);
                po.addFxn("genVisibleData", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genVisibleData", "microsoft.targets"), $$UNDEF);
                po.addFxn("genVisibleFxns", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genVisibleFxns", "microsoft.targets"), $$UNDEF);
                po.addFxn("genVisibleLibFxns", (Proto.Fxn)om.findStrict("xdc.bld.ITarget2$$genVisibleLibFxns", "microsoft.targets"), $$UNDEF);
                po.addFxn("getRawVersion", (Proto.Fxn)om.findStrict("xdc.bld.ITarget3$$getRawVersion", "microsoft.targets"), $$UNDEF);
                po.addFxn("setPath", (Proto.Fxn)om.findStrict("microsoft.targets.ITarget$$setPath", "microsoft.targets"), $$UNDEF);
    }

    void ITarget$$ROV()
    {
    }

    void Win32$$ROV()
    {
    }

    void Win64$$ROV()
    {
    }

    void Net32$$ROV()
    {
    }

    void $$SINGLETONS()
    {
        pkgP.init("microsoft.targets.Package", (Proto.Obj)om.findStrict("xdc.IPackage.Module", "microsoft.targets"));
        Scriptable cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "microsoft/targets/package.xs");
        om.bind("xdc.IPackage$$capsule", cap);
        Object fxn;
                fxn = Global.get(cap, "init");
                if (fxn != null) pkgP.addFxn("init", (Proto.Fxn)om.findStrict("xdc.IPackage$$init", "microsoft.targets"), fxn);
                fxn = Global.get(cap, "close");
                if (fxn != null) pkgP.addFxn("close", (Proto.Fxn)om.findStrict("xdc.IPackage$$close", "microsoft.targets"), fxn);
                fxn = Global.get(cap, "validate");
                if (fxn != null) pkgP.addFxn("validate", (Proto.Fxn)om.findStrict("xdc.IPackage$$validate", "microsoft.targets"), fxn);
                fxn = Global.get(cap, "exit");
                if (fxn != null) pkgP.addFxn("exit", (Proto.Fxn)om.findStrict("xdc.IPackage$$exit", "microsoft.targets"), fxn);
                fxn = Global.get(cap, "getLibs");
                if (fxn != null) pkgP.addFxn("getLibs", (Proto.Fxn)om.findStrict("xdc.IPackage$$getLibs", "microsoft.targets"), fxn);
                fxn = Global.get(cap, "getSects");
                if (fxn != null) pkgP.addFxn("getSects", (Proto.Fxn)om.findStrict("xdc.IPackage$$getSects", "microsoft.targets"), fxn);
        pkgP.bind("$capsule", cap);
        pkgV.init2(pkgP, "microsoft.targets", Value.DEFAULT, false);
        pkgV.bind("$name", "microsoft.targets");
        pkgV.bind("$category", "Package");
        pkgV.bind("$$qn", "microsoft.targets.");
        pkgV.bind("$vers", Global.newArray(1, 0, 2, 0));
        Value.Map atmap = (Value.Map)pkgV.getv("$attr");
        atmap.seal("length");
        imports.clear();
        pkgV.bind("$imports", imports);
        StringBuilder sb = new StringBuilder();
        sb.append("var pkg = xdc.om['microsoft.targets'];\n");
        sb.append("if (pkg.$vers.length >= 3) {\n");
            sb.append("pkg.$vers.push(Packages.xdc.services.global.Vers.getDate(xdc.csd() + '/..'));\n");
        sb.append("}\n");
        sb.append("if ('microsoft.targets$$stat$base' in xdc.om) {\n");
            sb.append("pkg.packageBase = xdc.om['microsoft.targets$$stat$base'];\n");
            sb.append("pkg.packageRepository = xdc.om['microsoft.targets$$stat$root'];\n");
        sb.append("}\n");
        sb.append("pkg.build.libraries = [\n");
        sb.append("];\n");
        sb.append("pkg.build.libDesc = [\n");
        sb.append("];\n");
        Global.eval(sb.toString());
    }

    void ITarget$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("microsoft.targets.ITarget", "microsoft.targets");
        po = (Proto.Obj)om.findStrict("microsoft.targets.ITarget.Module", "microsoft.targets");
        vo.init2(po, "microsoft.targets.ITarget", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Interface");
        vo.bind("$capsule", om.findStrict("microsoft.targets.ITarget$$capsule", "microsoft.targets"));
        vo.bind("$package", om.findStrict("microsoft.targets", "microsoft.targets"));
        tdefs.clear();
        proxies.clear();
        inherits.clear();
        vo.bind("Model", om.findStrict("xdc.bld.ITarget.Model", "microsoft.targets"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.Model", "microsoft.targets"));
        vo.bind("DebugGen", om.findStrict("xdc.bld.ITarget.DebugGen", "microsoft.targets"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.DebugGen", "microsoft.targets"));
        vo.bind("Extension", om.findStrict("xdc.bld.ITarget.Extension", "microsoft.targets"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.Extension", "microsoft.targets"));
        vo.bind("CompileOptions", om.findStrict("xdc.bld.ITarget.CompileOptions", "microsoft.targets"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.CompileOptions", "microsoft.targets"));
        vo.bind("OptionSet", om.findStrict("xdc.bld.ITarget.OptionSet", "microsoft.targets"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.OptionSet", "microsoft.targets"));
        vo.bind("CompileGoal", om.findStrict("xdc.bld.ITarget.CompileGoal", "microsoft.targets"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.CompileGoal", "microsoft.targets"));
        vo.bind("LinkGoal", om.findStrict("xdc.bld.ITarget.LinkGoal", "microsoft.targets"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.LinkGoal", "microsoft.targets"));
        vo.bind("ArchiveGoal", om.findStrict("xdc.bld.ITarget.ArchiveGoal", "microsoft.targets"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.ArchiveGoal", "microsoft.targets"));
        vo.bind("CommandSet", om.findStrict("xdc.bld.ITarget.CommandSet", "microsoft.targets"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.CommandSet", "microsoft.targets"));
        vo.bind("StringArray", om.findStrict("xdc.bld.ITarget.StringArray", "microsoft.targets"));
        vo.bind("TypeInfo", om.findStrict("xdc.bld.ITarget.TypeInfo", "microsoft.targets"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.TypeInfo", "microsoft.targets"));
        vo.bind("StdTypes", om.findStrict("xdc.bld.ITarget.StdTypes", "microsoft.targets"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.StdTypes", "microsoft.targets"));
        vo.bind("Command", om.findStrict("xdc.bld.ITarget2.Command", "microsoft.targets"));
        tdefs.add(om.findStrict("xdc.bld.ITarget2.Command", "microsoft.targets"));
        vo.bind("Options", om.findStrict("xdc.bld.ITarget2.Options", "microsoft.targets"));
        tdefs.add(om.findStrict("xdc.bld.ITarget2.Options", "microsoft.targets"));
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        inherits.add("xdc.bld");
        inherits.add("xdc.bld");
        inherits.add("xdc.bld");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$interfaces")).add(vo);
        pkgV.bind("ITarget", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("ITarget");
        vo.seal(null);
        if (vo.getProto().lookupFld("$used") != null) {
            vo.unseal("$used");
        }
    }

    void Win32$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("microsoft.targets.Win32", "microsoft.targets");
        po = (Proto.Obj)om.findStrict("microsoft.targets.Win32.Module", "microsoft.targets");
        vo.init2(po, "microsoft.targets.Win32", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("microsoft.targets.Win32$$capsule", "microsoft.targets"));
        vo.bind("$package", om.findStrict("microsoft.targets", "microsoft.targets"));
        tdefs.clear();
        proxies.clear();
        mcfgs.clear();
        icfgs.clear();
        inherits.clear();
        vo.bind("Model", om.findStrict("xdc.bld.ITarget.Model", "microsoft.targets"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.Model", "microsoft.targets"));
        vo.bind("DebugGen", om.findStrict("xdc.bld.ITarget.DebugGen", "microsoft.targets"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.DebugGen", "microsoft.targets"));
        vo.bind("Extension", om.findStrict("xdc.bld.ITarget.Extension", "microsoft.targets"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.Extension", "microsoft.targets"));
        vo.bind("CompileOptions", om.findStrict("xdc.bld.ITarget.CompileOptions", "microsoft.targets"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.CompileOptions", "microsoft.targets"));
        vo.bind("OptionSet", om.findStrict("xdc.bld.ITarget.OptionSet", "microsoft.targets"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.OptionSet", "microsoft.targets"));
        vo.bind("CompileGoal", om.findStrict("xdc.bld.ITarget.CompileGoal", "microsoft.targets"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.CompileGoal", "microsoft.targets"));
        vo.bind("LinkGoal", om.findStrict("xdc.bld.ITarget.LinkGoal", "microsoft.targets"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.LinkGoal", "microsoft.targets"));
        vo.bind("ArchiveGoal", om.findStrict("xdc.bld.ITarget.ArchiveGoal", "microsoft.targets"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.ArchiveGoal", "microsoft.targets"));
        vo.bind("CommandSet", om.findStrict("xdc.bld.ITarget.CommandSet", "microsoft.targets"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.CommandSet", "microsoft.targets"));
        vo.bind("StringArray", om.findStrict("xdc.bld.ITarget.StringArray", "microsoft.targets"));
        vo.bind("TypeInfo", om.findStrict("xdc.bld.ITarget.TypeInfo", "microsoft.targets"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.TypeInfo", "microsoft.targets"));
        vo.bind("StdTypes", om.findStrict("xdc.bld.ITarget.StdTypes", "microsoft.targets"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.StdTypes", "microsoft.targets"));
        vo.bind("Command", om.findStrict("xdc.bld.ITarget2.Command", "microsoft.targets"));
        tdefs.add(om.findStrict("xdc.bld.ITarget2.Command", "microsoft.targets"));
        vo.bind("Options", om.findStrict("xdc.bld.ITarget2.Options", "microsoft.targets"));
        tdefs.add(om.findStrict("xdc.bld.ITarget2.Options", "microsoft.targets"));
        vo.bind("SDKPath", om.findStrict("microsoft.targets.Win32.SDKPath", "microsoft.targets"));
        tdefs.add(om.findStrict("microsoft.targets.Win32.SDKPath", "microsoft.targets"));
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("microsoft.targets");
        inherits.add("xdc.bld");
        inherits.add("xdc.bld");
        inherits.add("xdc.bld");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "microsoft.targets")).add(vo);
        vo.bind("$$instflag", 0);
        vo.bind("$$iobjflag", 1);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 1);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        Proto.Str ps = (Proto.Str)vo.find("Module_State");
        if (ps != null) vo.bind("$object", ps.newInstance());
        vo.bind("$$meta_iobj", om.has("microsoft.targets.Win32$$instance$static$init", null) ? 1 : 0);
        vo.bind("$$fxntab", Global.newArray());
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.setElem("", "xdc/bld/stddefs.xdt");
        atmap.seal("length");
        pkgV.bind("Win32", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("Win32");
    }

    void Win64$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("microsoft.targets.Win64", "microsoft.targets");
        po = (Proto.Obj)om.findStrict("microsoft.targets.Win64.Module", "microsoft.targets");
        vo.init2(po, "microsoft.targets.Win64", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("microsoft.targets.Win64$$capsule", "microsoft.targets"));
        vo.bind("$package", om.findStrict("microsoft.targets", "microsoft.targets"));
        tdefs.clear();
        proxies.clear();
        mcfgs.clear();
        icfgs.clear();
        inherits.clear();
        vo.bind("Model", om.findStrict("xdc.bld.ITarget.Model", "microsoft.targets"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.Model", "microsoft.targets"));
        vo.bind("DebugGen", om.findStrict("xdc.bld.ITarget.DebugGen", "microsoft.targets"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.DebugGen", "microsoft.targets"));
        vo.bind("Extension", om.findStrict("xdc.bld.ITarget.Extension", "microsoft.targets"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.Extension", "microsoft.targets"));
        vo.bind("CompileOptions", om.findStrict("xdc.bld.ITarget.CompileOptions", "microsoft.targets"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.CompileOptions", "microsoft.targets"));
        vo.bind("OptionSet", om.findStrict("xdc.bld.ITarget.OptionSet", "microsoft.targets"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.OptionSet", "microsoft.targets"));
        vo.bind("CompileGoal", om.findStrict("xdc.bld.ITarget.CompileGoal", "microsoft.targets"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.CompileGoal", "microsoft.targets"));
        vo.bind("LinkGoal", om.findStrict("xdc.bld.ITarget.LinkGoal", "microsoft.targets"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.LinkGoal", "microsoft.targets"));
        vo.bind("ArchiveGoal", om.findStrict("xdc.bld.ITarget.ArchiveGoal", "microsoft.targets"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.ArchiveGoal", "microsoft.targets"));
        vo.bind("CommandSet", om.findStrict("xdc.bld.ITarget.CommandSet", "microsoft.targets"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.CommandSet", "microsoft.targets"));
        vo.bind("StringArray", om.findStrict("xdc.bld.ITarget.StringArray", "microsoft.targets"));
        vo.bind("TypeInfo", om.findStrict("xdc.bld.ITarget.TypeInfo", "microsoft.targets"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.TypeInfo", "microsoft.targets"));
        vo.bind("StdTypes", om.findStrict("xdc.bld.ITarget.StdTypes", "microsoft.targets"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.StdTypes", "microsoft.targets"));
        vo.bind("Command", om.findStrict("xdc.bld.ITarget2.Command", "microsoft.targets"));
        tdefs.add(om.findStrict("xdc.bld.ITarget2.Command", "microsoft.targets"));
        vo.bind("Options", om.findStrict("xdc.bld.ITarget2.Options", "microsoft.targets"));
        tdefs.add(om.findStrict("xdc.bld.ITarget2.Options", "microsoft.targets"));
        vo.bind("SDKPath", om.findStrict("microsoft.targets.Win64.SDKPath", "microsoft.targets"));
        tdefs.add(om.findStrict("microsoft.targets.Win64.SDKPath", "microsoft.targets"));
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("microsoft.targets");
        inherits.add("xdc.bld");
        inherits.add("xdc.bld");
        inherits.add("xdc.bld");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "microsoft.targets")).add(vo);
        vo.bind("$$instflag", 0);
        vo.bind("$$iobjflag", 1);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 1);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        Proto.Str ps = (Proto.Str)vo.find("Module_State");
        if (ps != null) vo.bind("$object", ps.newInstance());
        vo.bind("$$meta_iobj", om.has("microsoft.targets.Win64$$instance$static$init", null) ? 1 : 0);
        vo.bind("$$fxntab", Global.newArray());
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.setElem("", "xdc/bld/stddefs.xdt");
        atmap.seal("length");
        pkgV.bind("Win64", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("Win64");
    }

    void Net32$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("microsoft.targets.Net32", "microsoft.targets");
        po = (Proto.Obj)om.findStrict("microsoft.targets.Net32.Module", "microsoft.targets");
        vo.init2(po, "microsoft.targets.Net32", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", $$UNDEF);
        vo.bind("$package", om.findStrict("microsoft.targets", "microsoft.targets"));
        tdefs.clear();
        proxies.clear();
        mcfgs.clear();
        icfgs.clear();
        inherits.clear();
        vo.bind("Model", om.findStrict("xdc.bld.ITarget.Model", "microsoft.targets"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.Model", "microsoft.targets"));
        vo.bind("DebugGen", om.findStrict("xdc.bld.ITarget.DebugGen", "microsoft.targets"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.DebugGen", "microsoft.targets"));
        vo.bind("Extension", om.findStrict("xdc.bld.ITarget.Extension", "microsoft.targets"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.Extension", "microsoft.targets"));
        vo.bind("CompileOptions", om.findStrict("xdc.bld.ITarget.CompileOptions", "microsoft.targets"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.CompileOptions", "microsoft.targets"));
        vo.bind("OptionSet", om.findStrict("xdc.bld.ITarget.OptionSet", "microsoft.targets"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.OptionSet", "microsoft.targets"));
        vo.bind("CompileGoal", om.findStrict("xdc.bld.ITarget.CompileGoal", "microsoft.targets"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.CompileGoal", "microsoft.targets"));
        vo.bind("LinkGoal", om.findStrict("xdc.bld.ITarget.LinkGoal", "microsoft.targets"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.LinkGoal", "microsoft.targets"));
        vo.bind("ArchiveGoal", om.findStrict("xdc.bld.ITarget.ArchiveGoal", "microsoft.targets"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.ArchiveGoal", "microsoft.targets"));
        vo.bind("CommandSet", om.findStrict("xdc.bld.ITarget.CommandSet", "microsoft.targets"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.CommandSet", "microsoft.targets"));
        vo.bind("StringArray", om.findStrict("xdc.bld.ITarget.StringArray", "microsoft.targets"));
        vo.bind("TypeInfo", om.findStrict("xdc.bld.ITarget.TypeInfo", "microsoft.targets"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.TypeInfo", "microsoft.targets"));
        vo.bind("StdTypes", om.findStrict("xdc.bld.ITarget.StdTypes", "microsoft.targets"));
        tdefs.add(om.findStrict("xdc.bld.ITarget.StdTypes", "microsoft.targets"));
        vo.bind("Command", om.findStrict("xdc.bld.ITarget2.Command", "microsoft.targets"));
        tdefs.add(om.findStrict("xdc.bld.ITarget2.Command", "microsoft.targets"));
        vo.bind("Options", om.findStrict("xdc.bld.ITarget2.Options", "microsoft.targets"));
        tdefs.add(om.findStrict("xdc.bld.ITarget2.Options", "microsoft.targets"));
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("microsoft.targets");
        inherits.add("xdc.bld");
        inherits.add("xdc.bld");
        inherits.add("xdc.bld");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "microsoft.targets")).add(vo);
        vo.bind("$$instflag", 0);
        vo.bind("$$iobjflag", 1);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 1);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        Proto.Str ps = (Proto.Str)vo.find("Module_State");
        if (ps != null) vo.bind("$object", ps.newInstance());
        vo.bind("$$meta_iobj", om.has("microsoft.targets.Net32$$instance$static$init", null) ? 1 : 0);
        vo.bind("$$fxntab", Global.newArray());
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.setElem("", "xdc/bld/stddefs.xdt");
        atmap.seal("length");
        pkgV.bind("Net32", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("Net32");
    }

    void $$INITIALIZATION()
    {
        Value.Obj vo;

        if (isCFG) {
        }//isCFG
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("microsoft.targets.Win32", "microsoft.targets"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("microsoft.targets.Win64", "microsoft.targets"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("microsoft.targets.Net32", "microsoft.targets"));
        Global.callFxn("init", pkgV);
        ((Value.Obj)om.getv("microsoft.targets.ITarget")).bless();
        ((Value.Obj)om.getv("microsoft.targets.Win32")).bless();
        ((Value.Obj)om.getv("microsoft.targets.Win64")).bless();
        ((Value.Obj)om.getv("microsoft.targets.Net32")).bless();
        ((Value.Arr)om.findStrict("$packages", "microsoft.targets")).add(pkgV);
    }

    public void exec( Scriptable xdcO, Session ses )
    {
        this.xdcO = xdcO;
        this.ses = ses;
        om = (Value.Obj)xdcO.get("om", null);

        Object o = om.geto("$name");
        String s = o instanceof String ? (String)o : null;
        isCFG = s != null && s.equals("cfg");
        isROV = s != null && s.equals("rov");

        $$IMPORTS();
        $$OBJECTS();
        ITarget$$OBJECTS();
        Win32$$OBJECTS();
        Win64$$OBJECTS();
        Net32$$OBJECTS();
        ITarget$$CONSTS();
        Win32$$CONSTS();
        Win64$$CONSTS();
        Net32$$CONSTS();
        ITarget$$CREATES();
        Win32$$CREATES();
        Win64$$CREATES();
        Net32$$CREATES();
        ITarget$$FUNCTIONS();
        Win32$$FUNCTIONS();
        Win64$$FUNCTIONS();
        Net32$$FUNCTIONS();
        ITarget$$SIZES();
        Win32$$SIZES();
        Win64$$SIZES();
        Net32$$SIZES();
        ITarget$$TYPES();
        Win32$$TYPES();
        Win64$$TYPES();
        Net32$$TYPES();
        if (isROV) {
            ITarget$$ROV();
            Win32$$ROV();
            Win64$$ROV();
            Net32$$ROV();
        }//isROV
        $$SINGLETONS();
        ITarget$$SINGLETONS();
        Win32$$SINGLETONS();
        Win64$$SINGLETONS();
        Net32$$SINGLETONS();
        $$INITIALIZATION();
    }
}
