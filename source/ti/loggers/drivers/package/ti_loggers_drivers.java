/*
 *  Do not modify this file; it is automatically 
 *  generated and any modifications will be overwritten.
 *
 * @(#) xdc-I11
 */
import java.util.*;
import org.mozilla.javascript.*;
import xdc.services.intern.xsr.*;
import xdc.services.spec.Session;

public class ti_loggers_drivers
{
    static final String VERS = "@(#) xdc-I11\n";

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
    }

    void $$OBJECTS()
    {
        pkgP = (Proto.Obj)om.bind("ti.loggers.drivers.Package", new Proto.Obj());
        pkgV = (Value.Obj)om.bind("ti.loggers.drivers", new Value.Obj("ti.loggers.drivers", pkgP));
    }

    void $$SINGLETONS()
    {
        pkgP.init("ti.loggers.drivers.Package", (Proto.Obj)om.findStrict("xdc.IPackage.Module", "ti.loggers.drivers"));
        pkgP.bind("$capsule", $$UNDEF);
        pkgV.init2(pkgP, "ti.loggers.drivers", Value.DEFAULT, false);
        pkgV.bind("$name", "ti.loggers.drivers");
        pkgV.bind("$category", "Package");
        pkgV.bind("$$qn", "ti.loggers.drivers.");
        pkgV.bind("$vers", Global.newArray(1, 0, 0));
        Value.Map atmap = (Value.Map)pkgV.getv("$attr");
        atmap.seal("length");
        imports.clear();
        pkgV.bind("$imports", imports);
        StringBuilder sb = new StringBuilder();
        sb.append("var pkg = xdc.om['ti.loggers.drivers'];\n");
        sb.append("if (pkg.$vers.length >= 3) {\n");
            sb.append("pkg.$vers.push(Packages.xdc.services.global.Vers.getDate(xdc.csd() + '/..'));\n");
        sb.append("}\n");
        sb.append("if ('ti.loggers.drivers$$stat$base' in xdc.om) {\n");
            sb.append("pkg.packageBase = xdc.om['ti.loggers.drivers$$stat$base'];\n");
            sb.append("pkg.packageRepository = xdc.om['ti.loggers.drivers$$stat$root'];\n");
        sb.append("}\n");
        sb.append("pkg.build.libraries = [\n");
            sb.append("'lib/ticlang/m4/loggers.a',\n");
            sb.append("'lib/loggers.am4',\n");
            sb.append("'lib/ticlang/m4f/loggers.a',\n");
            sb.append("'lib/loggers.am4f',\n");
            sb.append("'lib/ccs/m4/loggers.a',\n");
            sb.append("'lib/loggers.aem4',\n");
            sb.append("'lib/ccs/m4f/loggers.a',\n");
            sb.append("'lib/loggers.aem4f',\n");
            sb.append("'lib/gcc/m4/loggers.a',\n");
            sb.append("'lib/loggers.am4g',\n");
            sb.append("'lib/gcc/m4f/loggers.a',\n");
            sb.append("'lib/loggers.am4fg',\n");
            sb.append("'lib/iar/m4/loggers.a',\n");
            sb.append("'lib/loggers.arm4',\n");
            sb.append("'lib/iar/m4f/loggers.a',\n");
            sb.append("'lib/loggers.arm4f',\n");
        sb.append("];\n");
        sb.append("pkg.build.libDesc = [\n");
            sb.append("['lib/ticlang/m4/loggers.a', {target: 'ti.targets.arm.clang.M4', suffix: 'm4'}],\n");
            sb.append("['lib/loggers.am4', {target: 'ti.targets.arm.clang.M4', suffix: 'm4'}],\n");
            sb.append("['lib/ticlang/m4f/loggers.a', {target: 'ti.targets.arm.clang.M4F', suffix: 'm4f'}],\n");
            sb.append("['lib/loggers.am4f', {target: 'ti.targets.arm.clang.M4F', suffix: 'm4f'}],\n");
            sb.append("['lib/ccs/m4/loggers.a', {target: 'ti.targets.arm.elf.M4', suffix: 'em4'}],\n");
            sb.append("['lib/loggers.aem4', {target: 'ti.targets.arm.elf.M4', suffix: 'em4'}],\n");
            sb.append("['lib/ccs/m4f/loggers.a', {target: 'ti.targets.arm.elf.M4F', suffix: 'em4f'}],\n");
            sb.append("['lib/loggers.aem4f', {target: 'ti.targets.arm.elf.M4F', suffix: 'em4f'}],\n");
            sb.append("['lib/gcc/m4/loggers.a', {target: 'gnu.targets.arm.M4', suffix: 'm4g'}],\n");
            sb.append("['lib/loggers.am4g', {target: 'gnu.targets.arm.M4', suffix: 'm4g'}],\n");
            sb.append("['lib/gcc/m4f/loggers.a', {target: 'gnu.targets.arm.M4F', suffix: 'm4fg'}],\n");
            sb.append("['lib/loggers.am4fg', {target: 'gnu.targets.arm.M4F', suffix: 'm4fg'}],\n");
            sb.append("['lib/iar/m4/loggers.a', {target: 'iar.targets.arm.M4', suffix: 'rm4'}],\n");
            sb.append("['lib/loggers.arm4', {target: 'iar.targets.arm.M4', suffix: 'rm4'}],\n");
            sb.append("['lib/iar/m4f/loggers.a', {target: 'iar.targets.arm.M4F', suffix: 'rm4f'}],\n");
            sb.append("['lib/loggers.arm4f', {target: 'iar.targets.arm.M4F', suffix: 'rm4f'}],\n");
        sb.append("];\n");
        Global.eval(sb.toString());
    }

    void $$INITIALIZATION()
    {
        Value.Obj vo;

        if (isCFG) {
        }//isCFG
        Global.callFxn("init", pkgV);
        ((Value.Arr)om.findStrict("$packages", "ti.loggers.drivers")).add(pkgV);
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
        if (isROV) {
        }//isROV
        $$SINGLETONS();
        $$INITIALIZATION();
    }
}
