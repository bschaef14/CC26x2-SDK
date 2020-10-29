onmessage = function(e) {
    var expr = e.data[0];
    var args = [];
    if (e.data.length > 1) {
        args = e.data.splice(1);
    }
    try {
        var val = eval(expr);
        postMessage([val, null].concat(args));
    }
    catch (ev) {
        postMessage(['error', ev.message].concat(args));
    }
}


