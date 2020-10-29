/*
 * ======== rov_server.js ========
 * This is a TI Cloud Agent (TICA) module that operates on the server-side.
 *
 * TICA modules MUST NOT be confused with NodeJS modules: a TICA module
 * is created and managed by the undocumented(?) TICA NodeJS module 
 * "ccs_base/cloudagent/src/module.js".  This means that TICA modules
 *     o can be created dynamically by any NodeJS module
 *     o need not have any representation in the filesystem
 *     o make the term "module" ambigous and their documentation read like
 *       a software license.
 *
 * A TICA module is specified by an object that declares a 'name' string 
 * and a create() method.  TICA modules are created by passing this
 * specification object to TICA's module.createModule() method.  A TICA
 * module's create() method defines a set of commands that can be invoked by 
 * a browser client.  All commands declared by a TICA module return promises.
 *
 * TICA handles the communication with browser clients, and delivers
 * commands to TICA modules by invoking commands specified by its
 * specification object.
 *
 * TICA module.createModule() is invoked either from within another TICA
 * module or at startup where all NodeJS modules contained in CCS's 
 * cloudagent/modules directory are loaded and their NodeJS module.exports
 * are used create the initial set of TICA modules.
 *
 * TICA modules are singletons: each is created just once (on first usage) 
 * and is reused until the last browser client closes.  Each TICA module is 
 * associated with a single web socket server which is used to "connect" to
 * browser-side views via a socket, one socket per browser view.  These
 * sockets are created and destroyed as browser views are opened and closed.
 *
 * The ROVServer module is started as part of TICA startup, and provides a
 * single method, createInstance, which creates a uniquely named TICA module
 * and "installs it as a "sub-module" of ROVServer.  Each ROV view in CCS
 * "connects" to ROVServer, and uses createInstance to create a new TICA
 * module, named "ROVServer:<uid>", which serves as bridge to an XDCtools 
 * ROV server process created when the browser client calls setExec() when
 * connecting to a target CPU.
 *
 * All ROVserver:<uid> instance commands serviced by an XDCtools ROV server
 * are maintained in a queue of pending promises.  When the XDCtools server
 * responds, the next queued promise is settled (i.e., fullfilled or 
 * rejected). The assumption is that the ROV server responds in the same
 * order it receives requests, which is ensured by the fact that the server
 * itself is synchronous and single-threaded.
 */

"use strict";

/* required 'core' node modules */
var spawn = require("child_process").spawn;
var os = require("os");
var fs = require('fs');

/*
 *  ======== module ========
 *  The nodejs module (not to be confused by a TICA "module")
 */
/* global module */

/*
 *  ======== createModule ========
 *  Imported TI cloud agent module "loader"
 *  
 *  @param name    - name of the module
 *  @param onClose - function called when module is closed (disconnected)
 */
var createModule = module.parent.require("../module").createModule;

/*
 *  ======== logger ========
 *  Imported TI cloud agent debug output logger
 */
var logger = module.parent.require("../logger");

/*
 *  ======== Q ========
 *  Imported promise modulesd used by TI cloud agent
 */
var Q = module.parent.require("q");

/*
 *  ======== shared state for all ROVServer:<uid> instances ========
 */

/* counter used to generate GUID for each instance */
var instanceId = 0; 

/* dslite web socket server port number (only one for all instances) */
var wssPort = undefined;

/* XDCTOOLS_JAVA_HOME originally set to 64-bit JRE */
var orig_java_home = undefined;

/* ROVServer module object */
var ROVServer; /* initialized by module.exports.create */

/*
 *  ======== _createInstance ========
 *  Create a new instance state object for ws (unless it already exists)
 */
function _createInstance(args)
{
    var name = "ROVServer:" + instanceId++;
    
    logger.info("ROV Module: _createInstance(): creating instance "
                + name);

    /* create an ROV instance state object */
    var inst = {
        /* instance (effective) GUID */
        name: name,

        /* the underlying xdctools ROV server */
        server: null,

        /* buffer of data from monserver to be sent to client */
        buffer: "",

        /* buffer of stderr output from monserver */
        stderrBuf: "",

        /* We access 'waitingPromise' as if it were an array, but it's
         * actually an object whose properties are integers. That allows
         * us to delete promises, and keep the object small.
         */
        waitingPromise: {},

        /* indexes to the next empty slot for a new promise, and to
         * the next pending promise to be resolved.
         */
        createId: 0,
        resolveId: 0,

        /* required TICA module "constructor" */
        create: function (onClose) {
            return createModule(name, onClose).then(
                function (agentInst) {
                    /* utility functions for the browser */
                    agentInst.commands.find = find;
                    agentInst.commands.discoverAddons = discoverAddons;

                    /* ROV server instance data requests */
                    agentInst.commands.getViews = list;
                    agentInst.commands.getProgress = getProgress;
                    agentInst.commands.getView = getView;
                    agentInst.commands.setExec = setExec;

                    /* ROV server instance (re)start/stop */
                    agentInst.commands.start = start;
                    agentInst.commands.exit = exit;

                    /* required TICA destructor */
                    agentInst.onclose = function () {
                        logger.info("closing instance " + name);
                         /* remove self from factory submodule cache */
                        delete ROVServer.subModules[name];
                        _exit(inst);
                    };

                    /* required TICA "constructor" return value */
                    return {
                        port: agentInst.getPort()
                    };
                }
            );
        }
    };

    /*
     *  ======== exit ========
     *  Stops the xdctools ROV server
     */
    function exit()
    {
        logger.info("ROV Module: exit(" + inst.name + ")");

        var locPromise = Q.defer();
        _exit(inst);
        locPromise.resolve();

        return (locPromise.promise);
    }

    /*
     *  ======== getProgress ========
     *  Reports progress messages during the ROV server startup
     */
    function getProgress()
    {
        logger.info("ROV Module: getProgress(" + inst.name + ")");

        var prom = Q.defer();
        inst.waitingPromise[inst.createId] = prom;

        var res = _checkBuffer(inst);
        if (res != null) {
            prom.resolve(res);
            return (prom.promise);
        }

        /* Only if the promise is not resolved yet, we increment createId++.
         * Otherwise, we let the next promise overwrite createId for this one.
         * This pattern is repeated in other functions that sometimes resolve
         * a promise immediately, and sometimes just return an unresolved 
         * promise.
         */
        inst.createId++;
        return (prom.promise);
    }

    /*
     *  ======== getView ========
     *  Returns the content of one tab
     */
    function getView(mod, view)
    {
        logger.info("ROV Module: getView(" + inst.name + ": "
                    + mod + ", " + view + ")");

        var prom = Q.defer();
        inst.waitingPromise[inst.createId] = prom;
        if (inst.server == null) {
            prom.reject("ROV server is not running.");
            return (prom.promise);
        }
        inst.createId++;
        inst.server.stdin.write("view:" + mod + "," + view + os.EOL);
        return (prom.promise);
    }

    /*
     *  ======== list ========
     *  Returns the list of all modules and their tabs
     */
    function list()
    {
        logger.info("ROV Module: list(" + inst.name + ")");

        var prom = Q.defer();
        inst.waitingPromise[inst.createId] = prom;

        if (inst.server == null) {
            prom.reject("ROV server instance is not running.");
            return (prom.promise);
        }
        inst.createId++;
        inst.server.stdin.write("list" + os.EOL);

        return (prom.promise);
    }

    /*
     *  ======== setExec ========
     *  Starts the server and supplies the path to the executable image
     *
     *  path is a string of the form:
     *      <exe_path>?comm=<comm_type>[:<comm_arg>]*[&<name=value>]*
     *  where,
     *      <exe_path>   is the full path to the executable
     *      <comm_type>  is "DSLite", "Serial", ...
     *      <comm_arg>   is a communication-specific value
     *      <name=value> is an arbitrary name-value pair that is
     *                   specified by the client
     */
    function setExec(path, xdcroot, xdcpath)
    {
        logger.info("ROV Module: setExec(" + inst.name + ": " + path
                    + ", " + xdcroot + ", ...)");

        var prom;

        if (inst.server == null) {
            prom = start.call(this, xdcroot, xdcpath);
            if (inst.server == null) {
                return (prom);  /* failed promise: e.g., can't find xs.exe */
            }
        }

        /* Here we create a promise, and return it unresolved (unless
         * there's an error). When 'data' is received from the ROV
         * server, the function waiting for it will resolve the promise.
         */
        prom = Q.defer();

        /* queue the promise */
        inst.waitingPromise[inst.createId] = prom;
        inst.createId++;

        /* if it exists, append dslite websocket server port num to exec cmd */
        var portArg = "";
        if (wssPort != null) {
            portArg = "&wsPort=" + wssPort;
        }

        /* send command to the server */
        inst.server.stdin.write("exec:" + path + portArg + os.EOL);

        return (prom.promise);
    }

    /*
     *  ======== start ========
     *  Starts the server and sets up event handlers
     */
    function start(xdcroot, xdcpath)
    {
        logger.info("ROV Module: start(" + inst.name + ": "
                    + xdcroot + ", " + xdcpath + ")");

        var locPromise = Q.defer();

        if (inst.server != null) {
            locPromise.resolve();
            return (locPromise.promise);
        }

        if (xdcroot == null) {
            xdcroot = process.env.XDCROOT;
        }

        var xs;
        if (xdcroot != null) {
            xs = xdcroot + "/xs";
            if (process.platform == "win32") {
                xs += ".exe";
            }
        }
        else {
            locPromise.reject("xdcroot is not supplied as a parameter or in"
                              + " the environment");
            return (locPromise.promise);
        }

        /* define XDCROOT_JAVA_HOME in case CCS doesn't (it should but ...) */
        if (process.env.XDCTOOLS_JAVA_HOME == null) {
            logger.info("warning: XDCTOOLS_JAVA_HOME was not set by CCS");
            process.env.XDCTOOLS_JAVA_HOME = _findJRE(xdcroot);
        }
        if (orig_java_home == undefined) {
            orig_java_home = process.env.XDCTOOLS_JAVA_HOME;
        }

        /* If this is Windows, and XDCtools older than 3.55, we have to look 
         * into a location with the 32-bit JRE. If there is nothing there, we
         * are not in CCS 9, so we will fall back to the standard JRE
         * locations.
         */
        if (process.platform == "win32"
            && fs.existsSync(__dirname + "/../../../utils/jre32")) {
            var m = xdcroot.match(/xdctools_3_(\d\d)/);
            if (m != null && m[1] != null && m[1] < "55") {
                if (fs.existsSync(xdcroot + "/jre")) {
                    process.env.XDCTOOLS_JAVA_HOME = xdcroot + "/jre";
                }
                else {
                    process.env.XDCTOOLS_JAVA_HOME =
                        __dirname + "/../../../utils/jre32";
                }
            }
            else {
                process.env.XDCTOOLS_JAVA_HOME = orig_java_home;
            }
        }

        /* check for MacOs /Contents/Home path additions */
        if (process.platform == 'darwin') {
            var jre = process.env.XDCTOOLS_JAVA_HOME;
            if (jre != null && jre.indexOf("/Contents/Home") == -1) {
                /* this should never be necessary, but ... */
                process.env.XDCTOOLS_JAVA_HOME += "/Contents/Home";
                logger.info(
                    "warning: added missing /Contents/Home to XDCTOOLS_JAVA_HOME");
            }
        }

        logger.info("XDCTOOLS_JAVA_HOME set to "
            + process.env.XDCTOOLS_JAVA_HOME);

        if (xdcpath == null) {
            xdcpath = process.env.XDCPATH;
        }

        var params = ["xdc.rov.monserver"];
        if (xdcpath != null) {
            params.unshift("--xdcpath=" + xdcpath);
        }

        /* create a common prefix for all monserver debug trace */
        var prefix = "ROV server " + params[params.length - 1] + ": ";

        inst.buffer = ""; /* ensure buffer is empty for new server */

        /* spawn xs */
        inst.server = spawn(xs, params, {detached: false, env: undefined});

        /* add error handler IMMEDIATELY otherwise node throws and aborts! */
        inst.server.on("error", function(msg) {
            logger.info(prefix + xs + " error: " + msg);
        });

        if (inst.server.pid == 0) {
            inst.server = null;
            var msg = prefix + "internal error: spawn of '" + xs + "' failed.";
            logger.info(msg);
            locPromise.reject(msg);
            return (locPromise.promise);
        }

        inst.server.stdout.setEncoding("utf8");

        inst.server.stdout.on("data", function(data) {
            /* accumulate output data from server */
            logger.info(prefix + "output: " + data);
            inst.buffer = inst.buffer + data;

            /* while there are outstanding promises, look for resolutions */
            while (inst.waitingPromise[inst.resolveId] != undefined) {
                if (inst.resolveId > 0) {
                    delete inst.waitingPromise[inst.resolveId - 1];
                }
                var message = _checkBuffer(inst);
                if (message == null) {
                    break;
                }

                logger.info("ROV Module: resolving promise "
                            + inst.resolveId);
                inst.waitingPromise[inst.resolveId].resolve(message);
                inst.resolveId++;
            }
        });
        inst.server.stdout.on("end", function() {
            logger.info(prefix + "ended stdout");
        });
        inst.server.stdout.on("close", function() {
            logger.info(prefix + "closed stdout");
            _exit(inst); /* in case, xs exits due to some startup error */
        });

        inst.server.stderr.on("data", function(data) {
            logger.info(prefix + "stderr: " + data);
            inst.stderrBuf = inst.stderrBuf + data;
        });
        inst.server.stderr.on("end", function() {
            logger.info(prefix + "ended stderr");
            if (inst.waitingPromise[inst.resolveId] != undefined) {
                if (inst.resolveId > 0) {
                    delete inst.waitingPromise[inst.resolveId - 1];
                }
                logger.info("ROV Module: rejecting promise " + inst.resolveId);
                inst.waitingPromise[inst.resolveId].reject(inst.stderrBuf);
                inst.resolveId++;
            }
        });

        locPromise.resolve();
        return (locPromise.promise);
    }

    return (inst);
}

/*
 *  ======== _exit ========
 *  Terminate the xdctools server for the specified inst
 */
function _exit(inst)
{
    if (inst.server != null) {
        logger.info("ROV Module: _exit(" + inst.name + ") pid = "
                    +  inst.server.pid);

        /* in case, the target monitor hangs, we kill rather than
         *     inst.server.stdin.write("exit" + os.EOL);
         */
        inst.server.kill('SIGTERM');
        inst.server = null;
    }
    else {
        logger.info("ROV Module: _exit(" + inst.name + ") server == null");
    }
}

/*
 *  ======== _findJRE ========
 *  Locate JRE
 */
function _findJRE(root)
{
    var tokens = [
        root + "/jre",
        root + "/../ccsv6/eclipse/jre",
        root + "/../ccsv7/eclipse/jre",
        __dirname + "/../../../../eclipse/jre"
    ];

    for (var i = 0; i < tokens.length; i++) {
        var tmp = tokens[i];

        /* TODO: update nodejs version so we can use fs.accessSync() */
        if (fs.existsSync(tmp)) {
            return (tmp);
        }
    }
    return (null);
}

/*
 *  ======== _findROV ========
 *  Locate latest version of the ROV plugin
 *
 *  Returns a path that's relative to the nodejs module path or, if
 *  the ROV plugin can't be found, null
 *
 *  Assumes that this file and one entry of the nodejs module path have a
 *  fixed relative path to the CCS "root" folder (ccsv7) _and_ the eclipse
 *  plugins folder is always located at <ccs_root>/eclipse/plugins
 *
 *  NODE_PATH could be used to add a controlled base directory for
 *  loading modules.  Setting NODE_PATH would prevent the user from
 *  spoofing this heuristic.
 *
 *  Installing node.exe in a fixed relative location to the eclipse plugins
 *  directory also works.
 *
 *  Currently, module.globalPaths =
 *      process.env.NODE_PATH
 *      $HOME/.node_modules
 *      $HOME/.node_libraries
 *      <ccs_root>/ccs_base/lib/node
 *
 *  The eclipse plugins directory is
 *      <ccs_root>/eclipse/plugins
 *
 *  This file is assumed to be located here:
 *      <ccs_root>/ccs_base/cloudagent/src/modules
 */
function _findROV()
{
    /* get absolute path to the CCS "root" folder (ccsv7) */
    var root = __dirname + "/../../../..";

    /* get a relative path from node's module path to CCS root */
    var prefix = "../../../";

    /* find all installed versions of the com.ti.rov plugin */
    var pdirs = [
        root + "/eclipse/dropins",
        root + "/eclipse/plugins"
    ];
    var plugins = [];
    for (var i = 0; i < pdirs.length; i++) {
        var dir = pdirs[i];
        logger.info("_findROV: looking in: " + dir);

        var list = [];
        try {
            list = fs.readdirSync(dir);
        }
        catch (x) {
            ; /* ignore non-existent directories */
        }

        for (var j = 0; j < list.length; j++) {
            var pname = list[j];
            if (pname.indexOf("com.ti.rov_") == 0) {
                logger.info("_findROV: found: " + dir + '/' + pname);
                plugins.push({dir: dir, pname: pname});
            }
        }
    }

    /* select latest version */
    plugins.sort(function (a, b) {
        return (a.pname > b.pname) ? -1 : ((a.pname < b.pname) ? 1 : 0);
    });
    var plugin = plugins[0];

    if (plugin == null) {
        return (null);
    }

    /* return a node module path relative path to plugin */
    return (prefix + plugin.dir.substr(root.length + 1) + '/' + plugin.pname);
}

/*
 * ======== _findXDCtools ========
 */
function _findXDCtools()
{
    /* get absolute path to the CCS "root" folder (ccsv7)/.. */
    var root = __dirname + "/../../../..";
    if (process.platform == 'darwin') {
        root += "/../../..";
    }

    var list = [];
    try {
        list = fs.readdirSync(root);
    }
    catch (x) {
        ; /* ignore non-existent directories */
    }

    var xdctools = [];
    for (var j = 0; j < list.length; j++) {
        var pname = list[j];
        logger.info("_findXDCtools: checking: " + pname);
        if (pname.indexOf("xdctools_") == 0) {
            logger.info("_findXDCtools: found: " + root + '/' + pname);
            xdctools.push({dir: root, pname: pname});
        }
    }

    /* select latest version */
    xdctools.sort(function (a, b) {
        return (a.pname > b.pname) ? -1 : ((a.pname < b.pname) ? 1 : 0);
    });

    logger.info("_findXDCtools: selected: "
                + (xdctools[0] ? xdctools[0].pname : null));

    /* return full path (or null) */
    return (xdctools[0] ? (root + '/' + xdctools[0].pname) : null);
}

/*
 *  ======== _checkBuffer ========
 *  Extracts the next message surrounded by <BOM> and <EOM> from buffer
 *
 *  If buffer is empty, the function returns null. This function should be
 *  called only after the caller verifies there are pending promises.
 *  Otherwise, the message will be dropped and, depending on the message,
 *  it could corrupt the communication between the GUI and the server.
 */
function _checkBuffer(inst)
{
    /* We need to have some kind of EOM signal to know when the ROV server
     * finished responding to a command. For now, we use "<EOM>" as a signal.
     */
    if (inst.buffer.indexOf("<EOM>") != -1) {
        var first = inst.buffer.indexOf("<BOM>");
        var last = inst.buffer.indexOf("<EOM>");
        /* There should be <BOM> at the beginning and <EOM> at the end. If
         * something went wrong, we'll dump the buffer to the log file and
         * shutdown the ROV server.
         */
        if (first == -1 || first > last) {
            logger.info("ROV Module: invalid buffer content: " + inst.buffer);
            _exit(inst);
            return (null);
        }
        var message = inst.buffer.slice(first + 5, last);
        inst.buffer = inst.buffer.substr(last + 5);
        return (message);
    }

    return (null);
}

/*
 *  ======== discoverAddons ========
 *  Return an array of server-side addon paths
 *
 *  discovery algorithm:
 *      var result = [];
 *      var roots = [<paths in XDC_ROV_HTTPROOTS>, <http initial root>];
 *      for (all http roots) {
 *          if (<root>/"addons.json" exists) {
 *              list = load(<root>/addons.json);
 *              for (all objs in list) {
 *                  if (obj.id and obj.path are globally unique) {
 *                      var addon = {path: obj.path, name: obj.name, root:root}
 *                      result.push(addon);
 *                  }
 *              }
 *          }
 *      }
 *      return (result);
 *
 *  Assumes: rov_server.js (this file) is at the initial root of the HTTP
 *           server
 */
function discoverAddons()
{
    var defer = Q.defer();

    var result = [];

    /* create search path from env and initial root */
    var roots = [];
    if (process.env.XDC_ROV_HTTPROOTS != null) {
        roots = process.env.XDC_ROV_HTTPROOTS.split(/;+/);
    }
    roots.push(__dirname + "/webcontents"); /* add initial http "/" */

    /* find "addons.json" in a root */
    var ids = {};
    var paths = {};
    for (var i = 0; i < roots.length; i++) {
        var root = roots[i];
        if (root != "") {
            try {
                /* load the addons.json file */
                var list = JSON.parse(
                    fs.readFileSync(root + "/addons.json", 'utf8'));

                /* use the list from addons.json, to locate/define addons */
                for (var j = 0; j < list.length; j++) {
                    var addon = list[j];
                    if (ids[addon.id] == null && paths[addon.path] == null) {
                        logger.info("ROV Module: discoverAddons(): adding " 
                                    + root + "/" + addon.path 
                                    + " (" + addon.name + ")");

                        ids[addon.id] = 1;
                        paths[addon.path] = 1;
                        result.push(
                            {path: addon.path, name: addon.name, id: addon.id, root: root});
                    }
                }
            }
            catch (e) {
                logger.info("ROV Module: discoverAddons(" + root 
                            + ") exception: " + e.message);
            }
        }
    }

    defer.resolve(JSON.stringify(result));

    return (defer.promise);
}

/*
 *  ======== find ========
 *  Locate name along package path
 */
function find(name, pathArray)
{
    var defer = Q.defer();

    if (pathArray == null || pathArray.length == 0) {
        var xdcpath = process.env.XDCPATH;
        pathArray = xdcpath == null ? [] : xdcpath.split(/;+/);
        pathArray.push(process.env.XDCROOT);
    }

    for (var i = 0; i < pathArray.length; i++) {
        var prefix = pathArray[i];
        if (prefix != "") {
            var tmp = prefix + '/' + name;
            logger.info("rov.find(): " + tmp + " ...");
            /* TODO: update nodejs version so we can use fs.accessSync() */
            if (fs.existsSync(tmp)) {
                defer.resolve(tmp);
                return (defer.promise);
            }
        }
    }

    defer.reject(new Error("can't find '" + name + "' along: " + pathArray));

    return (defer.promise);
}


/*
 *  ======== module.exports ========
 *  TICA module specification
 */
module.exports = {
    name: "ROVServer",

    /*
     *  ======== create ========
     *  TICA module initialization
     *
     *  @param closeCallback - cleanup function that should be called when
     *                         closing(?)
     *  @param agent         - create/get sibling module
     */
    create: function(closeCallback, agent) {
        /* get and cache this module's web socket server port number */
        agent("DS").then(
            function(response) {
                wssPort = response.port;
            }
        );

        logger.info("ROV Module: create(): NODE_PATH = "
                    + process.env.NODE_PATH);

        logger.info("ROV Module: create(): Module.globalPaths = "
                    + require("module").globalPaths);

        return createModule(this.name, closeCallback).then(
            function(modObj) {

                /* save ROVServer obj so we can add/delete ROVServer insts */
                ROVServer = modObj;
                
                /*
                 *  ======== createInstance ========
                 *  Creates an ROVServer instance
                 *
                 *  This function creates a unique ROVServer instance object
                 *  which is a newly created TICA module (with its 
                 *  own state and a unique name) that must be retrieved by
                 *  the browser in order to operate on the target.
                 *
                 *  @param args - a string passed from the browser
                 *  @returns    - the name of the new ROVServer instance to
                 *                the browser
                 */
                modObj.commands.createInstance = function (args) {
                    /* create & install a TICA module definition */
                    var instDef = _createInstance(args);
                    ROVServer.subModules[instDef.name] = instDef;

                    /* return the name of the TICA module to the browser */
                    return Q(instDef.name);
                };
                
                /* When all ROVServer clients are closed, 'onClose' is invoked
                 * just before closing the websocket server associated
                 * with this module
                 */
                modObj.onClose = function() {
                    /* issue: call closeCallback ???? */
                    logger.info("ROV Module: onClose() called.");
                };

                /* required return value for TICA module's create */
                return {
                    port: modObj.getPort()
                };
            }
        );
    }
};
