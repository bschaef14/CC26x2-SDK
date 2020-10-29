Polymer({ is : "ti-rov-data",
    properties: {
        pendingRequests : {
          type: Array,
          value: []
        },
        requestTimeout : {
          type : Number,
          value: 10000
        },
        inTimeout : {
          type: Boolean,
          value: false
        },
        connectTimeout : {
          type : Number,
          value: 20000
        },
        connectWaiting : {
          type: Boolean,
          value: false
        }
    },

    cancelAllPendingRequests : function () {
        for (var i = 0; i < this.pendingRequests.length; i++) {
            var pendingReqObj = this.pendingRequests[i];
            var handle = Number(pendingReqObj.timeoutHandle);
            pendingReqObj.timeoutHandle = null;
            this.cancelAsync(handle);
            var names = this.getModuleAndViewNames(pendingReqObj);
            pendingReqObj.reqMod.getViewCallback('cancelled', null, names.module, names.view, handle);
        }
        this.pendingRequests = [];
    },
    cancelConnectWait : function (reqObj) {
        reqObj.rovData.connectWaiting = false;
        reqObj.reqMod.setExecutableCallback({resStr: 'Connect cancelled'});
    },
    debuggerEvent : function(detail, reqObj) {
        reqObj.reqMod.debuggerEvent(detail);
    },
    discoverAddOns : function (reqMod) {
        var reqObj = {reqMod: reqMod, rovData : this};
        reqObj.timeoutHandle = String(this.async(function() {return (this.discoverAddOnsTimeoutCallback(reqObj));}, this.requestTimeout));
        return(discoverAddons(reqObj, this.discoverAddOnsCallback));
    },
    discoverAddOnsCallback :  function (res, addOns, reqObj) {
        if (reqObj.timeoutHandle != null) {
            var handle = Number(reqObj.timeoutHandle);
            reqObj.timeoutHandle = null;
            reqObj.rovData.cancelAsync(handle);
            reqObj.reqMod.discoverAddOnsCallback(res, addOns);
        }
    },
    discoverAddOnsTimeoutCallback : function(reqObj) {
        reqObj.timeoutHandle = null;
        reqObj.reqMod.discoverAddOnsCallback('Timeout discovering ROV add ons', null);
    },
    getModuleAndViewNames : function (reqObj) {
        if (reqObj.reqMod.moduleName) {
            var module = reqObj.reqMod.moduleName;
            var view =  reqObj.reqMod.viewName;
        }
        else {
            var module = reqObj.module;
            var view =  reqObj.view;
        }
        return ({module: module, view: view});
    },
    getView: function(module, view, reqMod) {
        if (this.hasPendingRequest(module, view, reqMod)){
            return (null);
        }
        var reqObj = {reqMod: reqMod, rovData : this, timeoutHandle : null, module: module, view: view};
        reqObj.timeoutHandle = String(this.async(function() {return (this.getViewTimeoutCallback(reqObj));}, this.requestTimeout));
        this.pendingRequests.push(reqObj);
        var handle = reqObj.timeoutHandle;
        getView(module, view, reqObj, this.getViewCallback);
        return (handle);
    },
    getViewCallback: function(error, res, module, view, reqObj) {
        if (!reqObj.reqMod.isAttached && reqObj.reqMod.parentElement == null) {
            reqObj.rovData.removePendingRequest(reqObj);    /* view has been closed */
            return;
        }
        if (!reqObj.rovData.inTimeout && reqObj.rovData.pendingRequests.length > 0 &&
            reqObj.timeoutHandle != null) {
            var handle = Number(reqObj.timeoutHandle);
            reqObj.timeoutHandle = null;
            reqObj.rovData.cancelAsync(handle);
            /* Remove from request list */
            for (var i = 0; i <  reqObj.rovData.pendingRequests.length; i++) {
                if (reqObj.rovData.pendingRequests[i] == reqObj) {
                    reqObj.rovData.pendingRequests.splice(i, 1);
                    break;
                }
            }
            reqObj.reqMod.getViewCallback(error, res, module, view, handle);
        }
    },
    getViewList : function(reqMod) {
        var reqObj = {reqMod: reqMod, rovData : this, timeoutHandle : null};
        reqObj.timeoutHandle = String(this.async(function() {return (this.getViewListTimeoutCallback(reqObj));}, this.requestTimeout));
        return(getViewList(reqObj, this.getViewListCallback));
    },
    getViewListCallback: function(error, viewList, reqObj) {
        if (reqObj.timeoutHandle != null) {
            var handle = Number(reqObj.timeoutHandle);
            reqObj.timeoutHandle = null;
            reqObj.rovData.cancelAsync(handle);
            reqObj.reqMod.getViewListCallback(error, viewList);
        }
    },
    getViewListTimeoutCallback: function(reqObj) {
        if (!confirm('Server is not responding.\nContinue to wait?')) {
            reqObj.timeoutHandle = null;
            reqObj.reqMod.getViewListCallback('Timeout getting list of ROV modules', null);
        }
        else {
            /* Reset timeout for this and keep waiting */
            reqObj.timeoutHandle = String(this.async(function() {return (this.getViewListTimeoutCallback(reqObj));}, this.requestTimeout));
        }
    },
    getViewTimeoutCallback : function(reqObj) {
        if (this.inTimeout || this.pendingRequests.length == 0) {
            return;
        }
        if (!reqObj.reqMod.isAttached && reqObj.reqMod.parentElement == null) {
            reqObj.rovData.removePendingRequest(reqObj);    /* view has been closed */
            return;
        }
        this.inTimeout = true;
        var name = reqObj.reqMod.moduleName ? (reqObj.reqMod.moduleName + '  ' + reqObj.reqMod.viewName) : reqObj.reqMod.viewName;
        if (!confirm('\n    The update request for\n\n' + '    ' + name +
            '\n\n    has timed out.  To continue waiting, click OK.\n    Click Cancel to stop waiting for this update.\n\n')) {
            /* User has killed request(s). Cancel all pending request timeouts
               and send timeout error to all views with pending requests */
            var handle = reqObj.timeoutHandle;
            reqObj.timeoutHandle = null;
            var names = this.getModuleAndViewNames(reqObj);
            reqObj.reqMod.getViewCallback('Data request timeout', null, names.module, names.view, handle);
            for (var i = 0; i < this.pendingRequests.length; i++) {
                if (this.pendingRequests[i] != reqObj) {
                    var pendingReqObj = this.pendingRequests[i];
                    handle = Number(pendingReqObj.timeoutHandle);
                    pendingReqObj.timeoutHandle = null;
                    this.cancelAsync(handle);
                    names = this.getModuleAndViewNames(pendingReqObj);
                    pendingReqObj.reqMod.getViewCallback('Data request timeout', null, names.module, names.view, handle);
                }
            }
            this.pendingRequests = [];
        }
        else {
            /* Reset timeout for this and all remaining pending requests */
            reqObj.timeoutHandle = String(this.async(function() {return (this.getViewTimeoutCallback(reqObj));}, this.requestTimeout));
            for (var i = 0; i < this.pendingRequests.length; i++) {
                if (this.pendingRequests[i] != reqObj) {
                    var pendingReqObj = this.pendingRequests[i];
                    var handle = Number(pendingReqObj.timeoutHandle);
                    pendingReqObj.timeoutHandle = null;
                    this.cancelAsync(handle);
                    pendingReqObj.timeoutHandle = String(this.async(function() {return (this.getViewTimeoutCallback(pendingReqObj));}, this.requestTimeout));
                }
            }
        }
        this.inTimeout = false;
    },
    hasPendingRequest : function (module, view, reqMod) {
        for (var i = 0; i < this.pendingRequests.length; i++) {
            var pendingReqObj = this.pendingRequests[i];
            if (pendingReqObj.module == module && pendingReqObj.view == view &&
                pendingReqObj.reqMod.id == reqMod.id) {
                return (true);
            }
        }
        return (false);
    },
    removePendingRequest : function (reqObj) {
        for (var i = 0; i <  reqObj.rovData.pendingRequests.length; i++) {
            if (reqObj.rovData.pendingRequests[i] == reqObj) {
                reqObj.rovData.pendingRequests.splice(i, 1);
                break;
            }
        }
    },
    setConnectTimeout : function (timeout) {
        this.connectTimeout = timeout;
    },
    setExecutable : function(execPath, reqMod) {
        this.connectWaiting = false;
        var reqObj = {reqMod: reqMod, rovData : this};
        reqObj.timeoutHandle = String(this.async(function() {return (this.setExecutableTimeoutCallback(reqObj));}, this.connectTimeout));
        return(setExecutable(execPath, reqObj, this.setExecutableCallback, this.setExecutableProgressCallback,
                             this.debuggerEvent));
    },
    setExecutableCallback : function(res, reqObj) {
        if (reqObj.timeoutHandle != null) {
            var handle = Number(reqObj.timeoutHandle);
            reqObj.timeoutHandle = null;
            reqObj.rovData.cancelAsync(handle);
            reqObj.reqMod.setExecutableCallback(res);
        }
    },
    setExecutableProgressCallback : function(res, reqObj) {
        var waitingRes = res.match(/^waiting/i) !=  null;
        if (!reqObj.rovData.connectWaiting && waitingRes) {
            if (reqObj.timeoutHandle != null) {
                var handle = Number(reqObj.timeoutHandle);
                reqObj.timeoutHandle = null;
                reqObj.rovData.cancelAsync(handle);
                reqObj.rovData.connectWaiting = true;
                reqObj.reqMod.setExecutableProgressCallback(res, reqObj);
            }
        }
        else if (reqObj.rovData.connectWaiting && !waitingRes) {
            reqObj.timeoutHandle = String(reqObj.rovData.async(function() {return (reqObj.rovData.setExecutableTimeoutCallback(reqObj));}, reqObj.rovData.connectTimeout));
            reqObj.rovData.connectWaiting = false;
            reqObj.reqMod.setExecutableProgressCallback(res, reqObj);
        }
        else if (reqObj.rovData.connectWaiting && waitingRes) {
            reqObj.reqMod.setExecutableProgressCallback(res, reqObj);
        }
        else if (reqObj.timeoutHandle != null) {
            reqObj.reqMod.setExecutableProgressCallback(res, reqObj);
        }
    },
    setExecutableTimeoutCallback : function(reqObj) {
        if (!confirm('Target is not responding and may need to be reset.\nContinue to wait?')) {
            reqObj.timeoutHandle = null;
            reqObj.reqMod.setExecutableCallback({resStr: 'Connect timeout'});
        }
        else {
            /* Reset timeout for this and keep waiting */
            reqObj.timeoutHandle = String(this.async(function() {return (this.setExecutableTimeoutCallback(reqObj));}, this.connectTimeout));
        }
    },
    setRequestTimeout : function (timeout) {
        this.requestTimeout = timeout;
    }
});
