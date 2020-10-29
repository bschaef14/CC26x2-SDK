class RtlsUtilException(Exception):
    def __init__(self, *args, **kwargs):
        Exception.__init__(self, *args, **kwargs)


class RtlsUtilTimeoutException(RtlsUtilException):
    pass


class RtlsUtilMasterNotFoundException(RtlsUtilException):
    pass


class RtlsUtilPassiveNotFoundException(RtlsUtilException):
    pass


class RtlsUtilNodesNotIdentifiedException(RtlsUtilException):
    def __init__(self, msg, not_indentified_nodes):
        RtlsUtilException.__init__(self, msg)
        self.not_indentified_nodes = not_indentified_nodes


class RtlsUtilEmbeddedFailToStopScanException(RtlsUtilException):
    pass


class RtlsUtilScanNoResultsException(RtlsUtilException):
    pass


class RtlsUtilScanSlaveNotFoundException(RtlsUtilException):
    pass


class RtlsUtilFailToConnectException(RtlsUtilException):
    pass
