#
#  Copyright (c) 2018-2019, Texas Instruments Incorporated
#  All rights reserved.
#
#  Redistribution and use in source and binary forms, with or without
#  modification, are permitted provided that the following conditions
#  are met:
#
#  *  Redistributions of source code must retain the above copyright
#     notice, this list of conditions and the following disclaimer.
#
#  *  Redistributions in binary form must reproduce the above copyright
#     notice, this list of conditions and the following disclaimer in the
#     documentation and/or other materials provided with the distribution.
#
#  *  Neither the name of Texas Instruments Incorporated nor the names of
#     its contributors may be used to endorse or promote products derived
#     from this software without specific prior written permission.
#
#  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
#  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
#  THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
#  PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
#  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
#  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
#  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
#  OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
#  WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
#  OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
#  EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#

import enum

from construct import Struct, Enum, Int8ul, Int8sl, Int32ul, Int16ul, Int16sl, Byte, this, Float64l, FlagsEnum, \
    GreedyRange, CString

from unpi.npirequest_mixins import AsyncReq, FromNwp, FromAp, SyncRsp, SyncReq
from unpi.serialnode import builder_class
from unpi.unpiparser import NpiSubSystem, NpiRequest, NpiSubSystems, NiceBytes, ReverseBytes


class Commands(enum.IntEnum):
    RTLS_CMD_IDENTIFY = 0x00
    RTLS_CMD_CONN_PARAMS = 0x02
    RTLS_CMD_CONNECT = 0x03
    RTLS_CMD_SCAN = 0x04
    RTLS_CMD_SCAN_STOP = 0x05
    RTLS_CMD_TOF_RESULT_DIST = 0x06
    RTLS_CMD_TOF_RESULT_STAT = 0x07
    RTLS_CMD_TOF_RESULT_RAW = 0x08
    RTLS_CMD_TOF_SET_SEC_SEED = 0x09
    RTLS_CMD_TOF_GET_SEC_SEED = 0x10
    RTLS_CMD_TOF_SET_PARAMS = 0x11
    RTLS_CMD_TOF_ENABLE = 0x12
    RTLS_CMD_AOA_SET_PARAMS = 0x13
    RTLS_CMD_AOA_ENABLE = 0x14
    RTLS_CMD_RESET_DEVICE = 0x20
    RTLS_CMD_TERMINATE_LINK = 0x22
    RTLS_CMD_AOA_RESULT_ANGLE = 0x23
    RTLS_CMD_AOA_RESULT_RAW = 0x24
    RTLS_CMD_AOA_RESULT_PAIR_ANGLES = 0x25
    RTLS_CMD_TOF_CALIBRATE = 0x26
    RTLS_CMD_CONN_INFO = 0x27
    RTLS_CMD_SET_RTLS_PARAM = 0x28
    RTLS_CMD_GET_RTLS_PARAM = 0x29
    RTLS_CMD_TOF_CALIB_NV_READ = 0x30
    RTLS_CMD_TOF_SWITCH_ROLE = 0x31
    RTLS_CMD_GET_ACTIVE_CONN_INFO = 0x32

    RTLS_EVT_ASSERT = 0x80
    RTLS_EVT_ERROR = 0x81
    RTLS_EVT_DEBUG = 0x82
    RTLS_EVT_CONN_INFO = 0x83


# Rtls param types for RTLS_CMD_SET_RTLS_PARAM command
class RtlsParamType(enum.IntFlag):
    RTLS_PARAM_CONNECTION_INTERVAL = 1
    RTLS_PARAM_2 = 2
    RTLS_PARAM_3 = 3


class Capabilities(enum.IntFlag):
    CM = 1
    AOA_TX = 2
    AOA_RX = 4
    TOF_SLAVE = 8
    TOF_PASSIVE = 16
    TOF_MASTER = 32
    RTLS_SLAVE = 64
    RTLS_MASTER = 128
    RTLS_PASSIVE = 256


RtlsStatus = Enum(Int8ul,
                  RTLS_SUCCESS=0,
                  RTLS_FAIL=1,
                  RTLS_LINK_ESTAB_FAIL=2,
                  RTLS_LINK_TERMINATED=3,
                  RTLS_OUT_OF_MEMORY=4,
                  RTLS_CONFIG_NOT_SUPPORTED=5,
                  RTLS_ILLEGAL_CMD=6,
                  )

AssertCause = Enum(Int8ul,
                   HAL_ASSERT_CAUSE_FALSE=0,
                   HAL_ASSERT_CAUSE_TRUE=1,
                   HAL_ASSERT_CAUSE_INTERNAL_ERROR=2,
                   HAL_ASSERT_CAUSE_HW_ERROR=3,
                   HAL_ASSERT_CAUSE_OUT_OF_MEMORY=4,
                   HAL_ASSERT_CAUSE_ICALL_ABORT=5,
                   HAL_ASSERT_CAUSE_ICALL_TIMEOUT=6,
                   HAL_ASSERT_CAUSE_WRONG_API_CALL=7,
                   HAL_ASSERT_CAUSE_HARDWARE_ERROR=8,
                   HAL_ASSERT_CAUSE_RF_DRIVER_ERROR=9,
                   )


class TofRole(enum.IntEnum):
    TOF_SLAVE = 0
    TOF_MASTER = 1
    TOF_PASSIVE = 2


class AoaRole(enum.IntEnum):
    AOA_SLAVE = 0
    AOA_MASTER = 1
    AOA_PASSIVE = 2


class TofResultMode(enum.IntEnum):
    TOF_MODE_DIST = 0
    TOF_MODE_STAT = 1
    TOF_MODE_RAW = 2


class TofRunMode(enum.IntEnum):
    TOF_MODE_CONT = 0
    TOF_MODE_AUTO = 1


class TofSecMode(enum.IntEnum):
    TOF_MODE_SINGLE_BUF = 0
    TOF_MODE_DBL_BUF = 1


class AoaResultMode(enum.IntEnum):
    AOA_MODE_ANGLE = 0
    AOA_MODE_PAIR_ANGLES = 1
    AOA_MODE_RAW = 2


class DeviceFamily(enum.IntEnum):
    DeviceFamily_ID_CC13X0 = 1
    DeviceFamily_ID_CC26X0 = 2
    DeviceFamily_ID_CC26X0R2 = 3
    DeviceFamily_ID_CC13X2 = 4
    DeviceFamily_ID_CC26X2 = 5


# noinspection PyPep8Naming
class RTLS(NpiSubSystem):
    type = NpiSubSystems.RTLS.value

    def __init__(self, sender):
        self.sender = sender

    #
    # Responses
    #
    class IdentifyRsp(NpiRequest, SyncRsp, FromNwp):
        command = Commands.RTLS_CMD_IDENTIFY
        struct = Struct(
            "capabilities" / FlagsEnum(Int16ul, Capabilities),
            "revNum" / Int16ul,
            "devId" / Enum(Int8ul, DeviceFamily),
            "identifier" / NiceBytes(ReverseBytes(Byte[6])),
        )

    class ConnRsp(NpiRequest, AsyncReq, FromNwp):
        command = Commands.RTLS_CMD_CONNECT
        struct = Struct(
            "connHandle" / Int16ul,
            "status" / RtlsStatus,
        )

    class AssertRsp(NpiRequest, AsyncReq, FromNwp):
        command = Commands.RTLS_EVT_ASSERT
        struct = Struct(
            "cause" / AssertCause,
        )

    class ErrorRsp(NpiRequest, AsyncReq, FromNwp):
        command = Commands.RTLS_EVT_ERROR
        struct = Struct(
            "status" / RtlsStatus,
        )

    class DebugRsp(NpiRequest, AsyncReq, FromNwp):
        command = Commands.RTLS_EVT_DEBUG
        struct = Struct(
            "debug_value" / Int32ul,
            "debug_string" / CString("utf8"),
        )

    class ConnInfoEvtRsp(NpiRequest, AsyncReq, FromNwp):
        command = Commands.RTLS_EVT_CONN_INFO
        struct = Struct(
            "connHandle" / Int16ul,
            "rssi" / Int8sl,
            "channel" / Int8ul,
        )

    class DeviceInfoRsp(NpiRequest, AsyncReq, FromNwp):
        command = Commands.RTLS_CMD_SCAN
        struct = Struct(
            "eventType" / Int8ul,
            "addrType" / Enum(Int8ul),
            "addr" / NiceBytes(ReverseBytes(Byte[6])),
            "rssi" / Int8sl,
            "dataLen" / Int8ul,
            "data" / NiceBytes(Byte[this.dataLen])
        )

    class ScanRsp(NpiRequest, SyncRsp, FromNwp):
        command = Commands.RTLS_CMD_SCAN
        struct = Struct(
            "status" / RtlsStatus,
        )

    class ResetDeviceRes(NpiRequest, SyncRsp, FromNwp):
        command = Commands.RTLS_CMD_RESET_DEVICE
        struct = Struct(
            "status" / RtlsStatus,
        )

    class ScanStopRsp(NpiRequest, AsyncReq, FromNwp):
        command = Commands.RTLS_CMD_SCAN_STOP
        struct = Struct(
            "status" / RtlsStatus,
        )

    class ConnectRsp(NpiRequest, SyncRsp, FromNwp):
        command = Commands.RTLS_CMD_CONNECT
        struct = Struct(
            "status" / RtlsStatus,
        )

    class SetConnParamsRsp(NpiRequest, SyncRsp, FromNwp):
        command = Commands.RTLS_CMD_CONN_PARAMS
        struct = Struct(
            "status" / RtlsStatus,
        )

    class GetActiveConnInfoRsp(NpiRequest, SyncRsp, FromNwp):
        command = Commands.RTLS_CMD_GET_ACTIVE_CONN_INFO
        struct = Struct(
            "status" / RtlsStatus,
        )

    class ConnParamsRsp(NpiRequest, AsyncReq, FromNwp):
        command = Commands.RTLS_CMD_CONN_PARAMS
        struct = Struct(
            "connHandle" / Int16ul,
            "accessAddress" / Int32ul,
            "connInterval" / Int16ul,
            "hopValue" / Int8ul,
            "mSCA" / Int16ul,
            "currChan" / Int8ul,
            "chanMap" / Byte[5],
            "crcInit" / Int32ul,
            "peerAddr" / NiceBytes(ReverseBytes(Byte[6])),
        )

    class AoaStartRsp(NpiRequest, SyncRsp, FromNwp):
        command = Commands.RTLS_CMD_AOA_ENABLE
        struct = Struct(
            "status" / RtlsStatus,
        )

    class AoaSetParamsRsp(NpiRequest, SyncRsp, FromNwp):
        command = Commands.RTLS_CMD_AOA_SET_PARAMS
        struct = Struct(
            "status" / RtlsStatus,
        )

    class AoaResultAngle(NpiRequest, AsyncReq, FromNwp):
        command = Commands.RTLS_CMD_AOA_RESULT_ANGLE
        struct = Struct(
            "connHandle" / Int16ul,
            "angle" / Int16sl,
            "rssi" / Int8sl,
            "antenna" / Int8ul,
            "channel" / Int8ul,
        )

    class AoaResultPairAngle(NpiRequest, AsyncReq, FromNwp):
        command = Commands.RTLS_CMD_AOA_RESULT_PAIR_ANGLES
        struct = Struct(
            "connHandle" / Int16ul,
            "rssi" / Int8sl,
            "antenna" / Int8ul,
            "channel" / Int8ul,
            "pairAngle" / Int16sl[3],
        )

    class AoaResultRaw(NpiRequest, AsyncReq, FromNwp):
        command = Commands.RTLS_CMD_AOA_RESULT_RAW
        struct = Struct(
            "connHandle" / Int16ul,
            "rssi" / Int8sl,
            "antenna" / Int8ul,
            "channel" / Int8ul,
            "offset" / Int16ul,
            "samplesLength" / Int16ul,
            "samples" / GreedyRange(Struct(
                "q" / Int16sl,
                "i" / Int16sl,
            )),
        )

    class TofStartRsp(NpiRequest, SyncRsp, FromNwp):
        command = Commands.RTLS_CMD_TOF_ENABLE
        struct = Struct(
            "status" / RtlsStatus,
        )

    class TofResultStatistics(NpiRequest, AsyncReq, FromNwp):
        command = Commands.RTLS_CMD_TOF_RESULT_STAT
        struct = Struct(
            "stats" / GreedyRange(Struct(
                "freq" / Int16ul,
                "tick" / Float64l,
                "tickVariance" / Float64l,
                "rssi" / Int8sl,
                "numOk" / Int32ul,
            )),
        )

    class TofResultDistance(NpiRequest, AsyncReq, FromNwp):
        command = Commands.RTLS_CMD_TOF_RESULT_DIST
        struct = Struct(
            "distance" / Float64l,
            "rssi" / Int8sl,
        )

    class TofResultRaw(NpiRequest, AsyncReq, FromNwp):
        command = Commands.RTLS_CMD_TOF_RESULT_RAW
        struct = Struct(
            "offset" / Int16ul,
            "samplesLength" / Int16ul,
            "samples" / GreedyRange(Struct(
                "tick" / Int32ul,
                "freqIdx" / Int8ul,
                "rssi" / Int8sl,
            )),
        )

    class TofSetParamsRsp(NpiRequest, SyncRsp, FromNwp):
        command = Commands.RTLS_CMD_TOF_SET_PARAMS
        struct = Struct(
            'status' / RtlsStatus,
        )

    class TofSetSecSeedRsp(NpiRequest, SyncRsp, FromNwp):
        command = Commands.RTLS_CMD_TOF_SET_SEC_SEED
        struct = Struct(
            "status" / RtlsStatus,
        )

    class TofGetSecSeedRsp(NpiRequest, SyncRsp, FromNwp):
        command = Commands.RTLS_CMD_TOF_GET_SEC_SEED
        struct = Struct(
            "seed" / NiceBytes(Int8ul[32]),
        )

    class TofCalibEnabledRsp(NpiRequest, SyncRsp, FromNwp):
        command = Commands.RTLS_CMD_TOF_CALIBRATE
        struct = Struct(
            "status" / RtlsStatus,
        )

    class TofCalibCompleteRsp(NpiRequest, AsyncReq, FromNwp):
        command = Commands.RTLS_CMD_TOF_CALIBRATE
        struct = Struct(
            "status" / RtlsStatus,
        )

    class TofCalibReadCompleteRsp(NpiRequest, AsyncReq, FromNwp):
        command = Commands.RTLS_CMD_TOF_CALIB_NV_READ
        struct = Struct(
            "calibParams" / Struct(
                "numFreq" / Int8ul,
                "calibDistance" / Int16ul,
            ),
            "calibVals" / GreedyRange(Struct(
                "freq" / Int16ul,
                "tick" / Float64l,
                "tickVariance" / Float64l,
                "numOk" / Int32ul,
            )),
        )

    class ConnInfoRsp(NpiRequest, SyncRsp, FromNwp):
        command = Commands.RTLS_CMD_CONN_INFO
        struct = Struct(
            "status" / RtlsStatus,
        )

    class SetRtlsParamRsp(NpiRequest, SyncRsp, FromNwp):
        command = Commands.RTLS_CMD_SET_RTLS_PARAM
        struct = Struct(
            "connHandle" / Int16ul,
            "rtlsParamType" / Enum(Int8ul, RtlsParamType),
            "status" / RtlsStatus,
        )

    class ReadCalFromNVRsp(NpiRequest, SyncRsp, FromNwp):
        command = Commands.RTLS_CMD_TOF_CALIB_NV_READ
        struct = Struct(
            "status" / RtlsStatus,
        )

    class TofRoleSwitchRsp(NpiRequest, SyncRsp, FromNwp):
        command = Commands.RTLS_CMD_TOF_SWITCH_ROLE
        struct = Struct(
            'status' / RtlsStatus,
        )

    #
    # Requests
    #

    class IdentifyReq(NpiRequest, SyncReq, FromAp):
        command = Commands.RTLS_CMD_IDENTIFY
        struct = None

    class ScanReq(NpiRequest, SyncReq, FromAp):
        command = Commands.RTLS_CMD_SCAN
        struct = None

    class ResetDeviceReq(NpiRequest, SyncReq, FromAp):
        command = Commands.RTLS_CMD_RESET_DEVICE
        struct = None

    class ConnectReq(NpiRequest, SyncReq, FromAp):
        command = Commands.RTLS_CMD_CONNECT
        struct = Struct(
            'addrType' / Enum(Int8ul),
            'peerAddr' / NiceBytes(ReverseBytes(Byte[6])),
            'connInterval' / Int16ul,
        )

    class GetActiveConnInfoReq(NpiRequest, SyncReq, FromAp):
        command = Commands.RTLS_CMD_GET_ACTIVE_CONN_INFO
        struct = Struct(
            'connHandle' / Int16ul,
        )

    class TerminateLinkReq(NpiRequest, SyncReq, FromAp):
        command = Commands.RTLS_CMD_TERMINATE_LINK
        struct = Struct(
            'connHandle' / Int16ul,
        )

    class AoaStartReq(NpiRequest, SyncReq, FromAp):
        command = Commands.RTLS_CMD_AOA_ENABLE
        struct = Struct(
            "connHandle" / Int16ul,
            "enable" / Int8ul,  # Enable/Disable
            "cteInterval" / Int16ul,  # 0 = run once, > 0 = sample CTE every cteInterval until told otherwise
            "cteLength" / Int8ul,  # Length of the CTE (2 - 20), used for AoA receiver
        )

    class AoaSetParamsReq(NpiRequest, SyncReq, FromAp):
        command = Commands.RTLS_CMD_AOA_SET_PARAMS
        struct = Struct(
            "aoaRole" / Enum(Int8ul, AoaRole),  # AOA_MASTER, AOA_SLAVE, AOA_PASSIVE
            "aoaResultMode" / Enum(Int8ul, AoaResultMode),  # AOA_MODE_ANGLE, AOA_MODE_PAIR_ANGLES, AOA_MODE_RAW
            "connHandle" / Int16ul,
            "slotDurations" / Int8ul,  # 1us/2us sampling slots
            "sampleRate" / Int8ul,  # 1Mhz (BT5.1 spec), 2Mhz, 3Mhz or 4Mhz - this enables oversampling
            "sampleSize" / Int8ul,  # 8 bit sample (as defined by BT5.1 spec), 16 bit sample (higher accuracy)
            "sampleCtrl" / Int8ul,  # sample control flags 0x00-default filtering, 0x01-RAW_RF no filtering
            "samplingEnable" / Int8ul,
            # 0 = mask CTE even if enabled, 1 = don't mask CTE, even if disabled (support Unrequested CTE)
            "numAnt" / Int8ul,  # Number of antennas in antenna array
            "antArray" / Int8ul[this.numAnt],  # GPIO's of antennas
        )

    class TofCalibReq(NpiRequest, SyncReq, FromAp):
        command = Commands.RTLS_CMD_TOF_CALIBRATE
        struct = Struct(
            "enable" / Int8ul,
            "samplesPerFreq" / Int16ul,
            "calibDistance" / Int8ul,
            "useCalibFromNV" / Int8ul,
        )

    class TofStartReq(NpiRequest, SyncReq, FromAp):
        command = Commands.RTLS_CMD_TOF_ENABLE
        struct = Struct(
            "enable" / Int8ul,
        )

    class TofSetParamsReq(NpiRequest, SyncReq, FromAp):
        command = Commands.RTLS_CMD_TOF_SET_PARAMS
        struct = Struct(
            'tofRole' / Enum(Int8ul, TofRole),
            'numSamples' / Int16ul,
            'numFreq' / Int8ul,
            'slaveLqiFilter' / Int8ul,
            'postProcessLqiThresh' / Int8ul,
            'postProcessMagnRatio' / Int16ul,
            'autoTofRssiThresh' / Int8sl,
            'resultMode' / Enum(Int8ul, TofResultMode),
            'runMode' / Enum(Int8ul, TofRunMode),
            'frequencies' / Int16ul[this.numFreq],
        )

    class SetConnInfoReq(NpiRequest, SyncReq, FromAp):
        command = Commands.RTLS_CMD_CONN_PARAMS
        struct = Struct(
            "connHandle" / Int16ul,
            "accessAddress" / Int32ul,
            "connInterval" / Int16ul,
            "hopValue" / Int8ul,
            "mSCA" / Int16ul,
            "currChan" / Int8ul,
            "chanMap" / Byte[5],
            "crcInit" / Int32ul,
        )

    class TofSetSecSeedReq(NpiRequest, SyncReq, FromAp):
        command = Commands.RTLS_CMD_TOF_SET_SEC_SEED
        struct = Struct(
            "seed" / NiceBytes(Int8ul[32]),
        )

    class TofGetSecSeedReq(NpiRequest, SyncReq, FromAp):
        command = Commands.RTLS_CMD_TOF_GET_SEC_SEED
        struct = None

    class GetConnInfoReq(NpiRequest, SyncReq, FromAp):
        command = Commands.RTLS_CMD_CONN_INFO
        struct = Struct(
            "connHandle" / Int16ul,
            "enable" / Int8ul,
        )

    class SetRtlsParamReq(NpiRequest, SyncReq, FromAp):
        command = Commands.RTLS_CMD_SET_RTLS_PARAM
        struct = Struct(
            "connHandle" / Int16ul,
            "rtlsParamType" / Enum(Int8ul, RtlsParamType),
            "len" / Int8ul,
            "data" / Byte[this.len]
        )

    class ReadCalibFromNVReq(NpiRequest, SyncReq, FromAp):
        command = Commands.RTLS_CMD_TOF_CALIB_NV_READ
        struct = None

    class TofRoleSwitchReq(NpiRequest, SyncReq, FromAp):
        command = Commands.RTLS_CMD_TOF_SWITCH_ROLE
        struct = Struct(
            "tofRole" / Enum(Int8ul, TofRole)
        )

    @builder_class(IdentifyReq)
    def identify(self): pass

    @builder_class(ScanReq)
    def scan(self): pass

    @builder_class(ConnectReq)
    def connect(self, addrType, peerAddr): pass

    @builder_class(TerminateLinkReq)
    def terminate_link(self, connHandle): pass

    @builder_class(TofStartReq)
    def tof_start(self, enable): pass

    @builder_class(TofSetParamsReq)
    def tof_set_params(self, tofRole, numSamples, numFreq, slaveLqiFilter, postProcessLqiThresh, postProcessMagnRatio,
                       autoTofRssiThresh, resultMode, runMode, frequencies): pass

    @builder_class(AoaStartReq)
    def aoa_start(self, connHandle, enable, cteInterval, cteLength): pass

    @builder_class(AoaSetParamsReq)
    def aoa_set_params(self, aoaRole, aoaResultMode, connHandle, slotDurations, sampleRate, sampleSize, sampleCtrl,
                       samplingEnable, numAnt, antArray): pass

    @builder_class(SetConnInfoReq)
    def set_ble_conn_info(self, connHandle, accessAddress, connInterval, hopValue, mSCA, currChan, chanMap,
                          crcInit): pass

    @builder_class(TofSetSecSeedReq)
    def tof_set_sec_seed(self, seed): pass

    @builder_class(TofGetSecSeedReq)
    def tof_get_sec_seed(self): pass

    @builder_class(TofCalibReq)
    def tof_calib(self, enable, samplesPerFreq, calibDistance, useCalibFromNV): pass

    @builder_class(ResetDeviceReq)
    def reset_device(self): pass

    @builder_class(GetConnInfoReq)
    def get_conn_info(self, connHandle, enable): pass

    @builder_class(SetRtlsParamReq)
    def set_rtls_param(self, connHandle, rtlsParamType, len, data): pass

    @builder_class(ReadCalibFromNVReq)
    def read_calib_from_NV(self): pass

    @builder_class(TofRoleSwitchReq)
    def tof_switch_role(self, tofRole): pass

    @builder_class(GetActiveConnInfoReq)
    def get_active_conn_info(self, connHandle): pass


class RtlsCC26X2(RTLS):
    type = NpiSubSystems.RTLS.value


class RtlsCC2640R2(RTLS):
    type = NpiSubSystems.RTLS.value

    #
    # Responses
    #
    class ConnRsp(NpiRequest, AsyncReq, FromNwp):
        command = Commands.RTLS_CMD_CONNECT
        struct = Struct(
            "status" / RtlsStatus,
        )

    class ConnInfoEvtRsp(NpiRequest, AsyncReq, FromNwp):
        command = Commands.RTLS_EVT_CONN_INFO
        struct = Struct(
            "rssi" / Int8sl,
            "channel" / Int8ul,
        )

    class ConnParamsRsp(NpiRequest, AsyncReq, FromNwp):
        command = Commands.RTLS_CMD_CONN_PARAMS
        struct = Struct(
            "accessAddress" / Int32ul,
            "connInterval" / Int16ul,
            "hopValue" / Int8ul,
            "mSCA" / Int16ul,
            "currChan" / Int8ul,
            "chanMap" / Byte[5],
            "crcInit" / Int32ul,
        )

    class AoaResultAngle(NpiRequest, AsyncReq, FromNwp):
        command = Commands.RTLS_CMD_AOA_RESULT_ANGLE
        struct = Struct(
            "angle" / Int16sl,
            "rssi" / Int8sl,
            "antenna" / Int8ul,
            "channel" / Int8ul,
        )

    class AoaResultPairAngle(NpiRequest, AsyncReq, FromNwp):
        command = Commands.RTLS_CMD_AOA_RESULT_PAIR_ANGLES
        struct = Struct(
            "rssi" / Int8sl,
            "antenna" / Int8ul,
            "channel" / Int8ul,
            "pairAngle" / Int16sl[3],
        )

    class AoaResultRaw(NpiRequest, AsyncReq, FromNwp):
        command = Commands.RTLS_CMD_AOA_RESULT_RAW
        struct = Struct(
            "rssi" / Int8sl,
            "antenna" / Int8ul,
            "channel" / Int8ul,
            "offset" / Int16ul,
            "samplesLength" / Int16ul,
            "samples" / GreedyRange(Struct(
                "q" / Int16sl,
                "i" / Int16sl,
            )),
        )

    class SetRtlsParamRsp(NpiRequest, SyncRsp, FromNwp):
        command = Commands.RTLS_CMD_SET_RTLS_PARAM
        struct = Struct(
            "rtlsParamType" / Enum(Int8ul, RtlsParamType),
            "status" / RtlsStatus,
        )

    #
    # Requests
    #

    class TerminateLinkReq(NpiRequest, SyncReq, FromAp):
        command = Commands.RTLS_CMD_TERMINATE_LINK
        struct = None

    class SetRtlsParamReq(NpiRequest, SyncReq, FromAp):
        command = Commands.RTLS_CMD_SET_RTLS_PARAM
        struct = Struct(
            "rtlsParamType" / Enum(Int8ul, RtlsParamType),
            "len" / Int8ul,
            "data" / Byte[this.len]
        )

    class AoaStartReq(NpiRequest, SyncReq, FromAp):
        command = Commands.RTLS_CMD_AOA_ENABLE
        struct = Struct(
            "enable" / Int8ul,
        )

    class AoaSetParamsReq(NpiRequest, SyncReq, FromAp):
        command = Commands.RTLS_CMD_AOA_SET_PARAMS
        struct = Struct(
            "aoaRole" / Enum(Int8ul, AoaRole),
            "aoaResultMode" / Enum(Int8ul, AoaResultMode),
            "cteScanOvs" / Int8ul,
            "cteOffset" / Int8ul,
            "cteTime" / Int16ul,
            "sampleCtrl" / Int8ul,
        )

    class SetConnInfoReq(NpiRequest, SyncReq, FromAp):
        command = Commands.RTLS_CMD_CONN_PARAMS
        struct = Struct(
            "accessAddress" / Int32ul,
            "connInterval" / Int16ul,
            "hopValue" / Int8ul,
            "mSCA" / Int16ul,
            "currChan" / Int8ul,
            "chanMap" / Byte[5],
            "crcInit" / Int32ul,
        )

    class GetConnInfoReq(NpiRequest, SyncReq, FromAp):
        command = Commands.RTLS_CMD_CONN_INFO
        struct = Struct(
            "enable" / Int8ul,
        )

    @builder_class(AoaStartReq)
    def aoa_start(self, enable): pass

    @builder_class(AoaSetParamsReq)
    def aoa_set_params(self, aoaRole, aoaResultMode, cteScanOvs, cteOffset, cteTime, sampleCtrl): pass

    @builder_class(TerminateLinkReq)
    def terminate_link(self): pass

    @builder_class(SetConnInfoReq)
    def set_ble_conn_info(self, accessAddress, connInterval, hopValue, mSCA, currChan, chanMap, crcInit): pass

    @builder_class(GetConnInfoReq)
    def get_conn_info(self, enable): pass

    @builder_class(SetRtlsParamReq)
    def set_rtls_param(self, rtlsParamType, len, data): pass
