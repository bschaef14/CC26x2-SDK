"""
This module describes the host-side interfaces for developers of transport implementations or plugins.
"""
from enum import Enum
from argparse import ArgumentParser

from typing import Optional, NoReturn, List, TYPE_CHECKING
from abc import ABC, abstractmethod

from tilogger.tracedb import ElfString, Opcode, TraceDB
from tilogger_itm_transport.itm_framer import build_value

if TYPE_CHECKING:
    from tilogger.logger import Logger


class LogLevel(Enum):
    Log_ERROR = 0
    Log_WARN = 1
    Log_INFO1 = 3
    Log_INFO2 = 4
    Log_INFO3 = 5
    Log_INFO4 = 6
    Log_INFO5 = 7
    Log_INFO6 = 8
    Log_DEBUG = 9


class LogPacket:
    """
    LogPacket is a data class for describing log records.

    Some notes on data representation:
     - module (str):        This string should match one of the module names used on the target.

     - opcode (int):        This opcode represents the "logging operation" this packet represents. Opcodes
                            0-9 are RESERVED for use only by Log.h APIs. Any custom log packets (e.g.
                            DWT watchpoint packets, or program counter traces) must use opcodes 10 and up.
                            If your embedded code defines specific opcodes, you should also provide a Python
                            module implementing the LogFormatterABC (below) that converts the representation
                            of those new opcodes into user-facing strings.

     - timestamp (int):     A 32-bit integer defining a timestamp. A transport layer should be able to resolve
       (optional)           this timestamp into real-time seconds, to tolerable accuracy, and (barring
                            rollovers) it must be monotonically increasing. Examples of good timestamps:
                            a timer derived directly or some division of the system clock, an RTC, or a
                            hardware timer. A 'message number' incremented with each packet would not be a
                            good timestamp because it cannot be converted into seconds.

     - data (bytes):        The data bytes for this packet, including the header that contains the ELF
                            pointer if applicable.

     - level (LogLevel):    What LogLevel the call was made with.

     - _str_data (bytes):   Internal use only. Holds the string representation of this log packet once
                            it has been through the formatting mechanism. Do not modify this value.

     - _final_str_data:     Internal use only. Holds the string representation of this log packet as it will
       (bytes)              be sent over the pipe, including the module and opcode values. Do not modify this value.
    """

    def __init__(self, module: str, opcode: int, level: LogLevel, timestamp: Optional[int], data: bytes):
        """
        Create a new LogPacket instance.

        Args:
            module (str): Name of the Module the user has logged from
            opcode (int): Integer 0-9 for TI Log APIs or a module-specific opcode >= 10
            level (LogLevel): One of the values from the LogLevel enum
            timestamp (int): Transport-dependent timestamp code
            data (bytes): The data bytes for this packet
        """
        self.module: str = module
        self.opcode: int = opcode
        self.timestamp: Optional[int] = timestamp
        self.level: LogLevel = level
        self.data: bytes = data

        self._str_data: str = ""
        self._final_str_data: str = ""


    @classmethod
    def from_elf_string(cls, elf_string: ElfString, data: bytes, timestamp: Optional[int] = None):
        """
        Generates a LogPacket instance from an ElfString and packet data.

        Args:
            elf_string (ElfString): The ElfString that corresponds to this log site.
            data (bytes): The data bytes associated with this packet.
            timestamp (int, optional): A timestamp value (see class docstring). Defaults to None.

        Returns:
            LogPacket: A completed LogPacket.
        """
        level = LogLevel[elf_string.level]
        return cls(elf_string.module_id, elf_string.opcode.value, level, timestamp, data)


    def __bytes__(self) -> bytes:
        """
        Convert the final string representation of the packet and to UTF-8 bytes which will be added to the pipe.
        """
        self._final_str_data = \
            f"{str(Opcode(self.opcode).name)}||{self.module}||{str(self.level.name)}||{self._str_data}"

        return self._final_str_data.encode("UTF-8", "strict")


    def __str__(self) -> str:
        """
        This is not normally used, but builds a human-readable packet representation for debugging or verbose mode.
        """
        elf_ptr = build_value(self.data[:4])
        data_str = " ".join(["0x%x" % byte for byte in self.data[4:]])
        return f"""\
LogPacket:
  Module: {self.module}
  Opcode: {self.opcode}
  Timestamp: {self.timestamp}
  ELF Pointer: 0x{elf_ptr:X}
  Data: {data_str}
  Formatted String: {self._str_data}"""


class TransportABC(ABC):
    """
    This interface serves as a base class for transport layers.
    All methods marked @abstractmethod must be implemented by a transport layer.
    """

    @classmethod
    @abstractmethod
    def register_args(cls, parser: ArgumentParser):
        pass

    @abstractmethod
    def start(self, logger: 'Logger', argv: List[str], tracedb: TraceDB) -> NoReturn:
        """
        start is called shortly after the 'logger' CLI is invoked. It should never return, unless
        the program indicates completion or encounters an error.

        Args:
            argv (List[str]): All command line arguments to the program. This can be interpreted by
                              argparse or another arguments library. You may throw exceptions here if
                              mandatory arguments are not provided.

                              Implementations MUST print CLI documentation if start is invoked with
                              the -h or --help arguments. In this case, no other action should be taken.

        Returns:
            NoReturn: This function should retain execution control and never return unless the program exits.
        """

    @abstractmethod
    def reset(self):
        """
        Handle a device or protocol reset, clearing module state as needed.
        """


    @abstractmethod
    def timestamp_to_seconds(self, timestamp: int) -> float:
        """
        Converts the input timestamp, in implementation-dependent units, into decimal seconds.

        Note that it is not elapsed time that is requested; if the clock source is an RTC and it
        happens to currently read 4 days and a few minutes, then that value should be returned (in seconds).

        Args:
            timestamp (int): A timestamp, provided as part of a LogPacket.

        Returns:
            float: An equivalent timestamp, in seconds.
        """


class LogFormatterABC(ABC):
    """
    This interface should be implemented by packages looking to provide rich log formatting.

    If you do not provide a plugin for your module, then logs will be formatted using a default
    implementation. For format_log, this simply prints the resolved format string with inserted arguments.
    For format_packet, the individual bytes are printed in hex, space-separated.

    Both format_log and format_packet must be implemented. See the notes under each function for when they
    will be called. For example, if your module provides no custom opcodes and does not use LogBuf, then
    format_packet will never be called and it can simply return an empty string.

    LogFormatter implemeters must provide an entry point formatted like this:
        entry_points={
            "tilogger.formatter": [
                "[MODULE_NAME] = YOUR_PACKAGE.YOUR_MODULE:FORMATTER_CLASS",
            ],
        },
    """
    @abstractmethod
    def format_log(self, logged_str: str, packet: LogPacket) -> str:
        """
        This method will be called for the following Log.h APIs: LogEvent, ************

        Args:
            logged_str (str): This contains the fully resolved format string including provided arguments.
            packet (LogPacket): This is the packet that generated this event, as the transport provided it
                                to Logger.Send. Please note that while packet.data is technically valid,
                                plugins are not provided with direct access to the .out file so cannot
                                perform their own format string resolution. Please consider parsing the
                                string e.g. with a regex if you need to reformat the arguments.

        Returns:
            str: The string representation of the *data* for this packet, as it should be presented to the
                 user. Do not include the module name, timestamp or opcode in this string; they are added
                 automatically by Logger.

        Default Implementation:
            The default implementation returns logged_str.
        """

    def format_packet(self, packet: LogPacket) -> str:
        """
        This method will be called when a user calls the LogBuf API from Log.h, or when a module packet
        is received with a custom opcode (>=10). These packets are completely untouched; this function must
        use packet.data to build a meaningful string representation for the user.

        Args:
            packet (LogPacket): This is the packet that generated this event, as the transport provided it
                                to Logger.Send.

        Returns:
            str: The string representation of the *data* for this packet, as it should be presented to the
                 user. Do not include the module name, timestamp or opcode in this string; they are added
                 automatically by Logger.
        """


class LogSubscriberABC(ABC):
    """
    This interface can be implemented by packages who wish to subscribe to packets logged by another module.
    A subscriber is notified about matching packets, but cannot influence their representation or prevent
    them from being emitted.

    Hooks in LogFormatter-derived classes will be called before LogSubscribers are notified.

    LogSubscriber implementers must provide an entry point formatted like this:
        entry_points={
            "tilogger.subscriber": [
                "[MODULE_NAME] = YOUR_PACKAGE.YOUR_MODULE:FORMATTER_CLASS",
            ],
        },
    """
    @abstractmethod
    def notify_packet(self, final_string: str, packet: LogPacket) -> None:
        """
        This method is called when a log packet matching the 'watched' module is emitted.
        It is called just prior to the log's emission over the pipe.

        Args:
            final_string (str): The string representation of the packet, either generated by Logger or
                                by a module plugin implementing LogFormatterABC. See the Logger
                                documentation for a description of the string format here (it is identical
                                to the string format in the pipe).
            packet (LogPacket): The original packet passed to Logger.
        """
