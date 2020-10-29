import struct
import logging
import argparse
import platform
from struct import pack
import pkg_resources

import win32pipe
import win32file

from typing import Dict, List

from tilogger.interface import LogPacket, TransportABC, LogFormatterABC, LogSubscriberABC
from tilogger.tracedb import Opcode, TraceDB, ElfString

logger = logging.getLogger("TI Logger")

def _load_entry_point(entry_point_name: str):
    entry_point: pkg_resources.EntryPoint
    return {
        entry_point.name: entry_point.load()
        for entry_point in pkg_resources.iter_entry_points(entry_point_name)
    }


def _build_value(buf):
    """
    Helper function: Turn an iterable into a little-endian integer
    """
    value = 0
    for idx, val in enumerate(buf):
        value += val << (idx * 8)
    return value


class Logger:
    PCAP_MAGIC_NUMBER: int = 0xA1B2C3D4
    PCAP_MAJOR_VER: int = 2
    PCAP_MINOR_VER: int = 4
    PCAP_LINK_TYPE: int = 147

    def __init__(self, transport_class, args):
        if args.verbose:
            logging.basicConfig(level="DEBUG")

        # Locate all transports/formatters/subscribers by entry points
        # Note that any duplicate formatters are resolved to a single formatter last-come-first-served
        self._formatters: Dict[str, LogFormatterABC] = {
            entry_point.name: entry_point.load()
            for entry_point in pkg_resources.iter_entry_points("tilogger.formatter")
        }

        self._subscribers: Dict[str, List[LogSubscriberABC]] = {}

        # Subscribers are slightly more difficult, because we need to handle duplicates explicitly
        for entry_point in pkg_resources.iter_entry_points("tilogger.subscriber"):
            self._subscribers.setdefault(entry_point.name, [])
            self._subscribers[entry_point.name].append(entry_point.load())


        # Instantiate a TransportClass instance
        # This may throw an error from the ABC module if the user has not implemented all methods
        # If so, just let this bubble out
        self._transport: TransportABC = transport_class()

        self._db = TraceDB(args.out_file, args.repickle, "")

        logger.critical("Waiting for Wireshark to be ready..")
        self._pipe = None
        self._setup_pipe()

        # This function never returns.
        self._transport.start(self, args, self._db)


    def log(self, packet: LogPacket):
        logger.debug("Handling %s", packet)

        # Special handling for Log.h opcodes; these all have elf strings
        # and may need to be expanded before further handling
        if packet.opcode < 10:
            self.expand_str(packet)

        # Duplicate formatters for a module are not permitted
        if packet.module in self._formatters:
            packet._str_data = self._formatters[packet.module].format_log(packet._str_data, packet)

        # Log this packet before notification, so badly behaved subscribers can't break the datapath
        self._send(packet)

        # Duplicate subscribers are permitted
        if packet.module in self._subscribers:
            for subscriber in self._subscribers[packet.module]:
                subscriber.notify_packet(bytes(packet).decode("UTF-8"), packet)


    def _send(self, packet: LogPacket):
        if packet.timestamp:
            timestamp = self._transport.timestamp_to_seconds(packet.timestamp)
            timestamp_seconds = int(timestamp)
            timestamp_microsec = int(1000000 * (timestamp - timestamp_seconds))
        else:
            timestamp_seconds = 0
            timestamp_microsec = 0

        payload = bytes(packet)
        header = struct.pack(
            "IIII",
            timestamp_seconds,
            timestamp_microsec,
            len(payload),
            len(payload)
        )

        data = header + payload
        self._send_internal(data)


    def _send_internal(self, data: bytes):
        if platform.system() == "Windows":
            win32file.WriteFile(self._pipe, data)
        else:
            raise NotImplementedError("No support for Linux in this beta")


    def _setup_pipe(self):
        # pipename should be of the form \\.\pipe\mypipename
        self._pipe = win32pipe.CreateNamedPipe(
            "\\\\.\\pipe\\tilogger-wireshark",
            win32pipe.PIPE_ACCESS_OUTBOUND,
            win32pipe.PIPE_TYPE_MESSAGE | win32pipe.PIPE_WAIT,
            1, 65536, 65536,
            300,
            None
        )

        win32pipe.ConnectNamedPipe(self._pipe, None)

        # Header format (rows are 32-bit words):
        # Magic number
        # Major (16b) and minor (16b) numbers
        # Reserved write 0
        # Reserved write 0
        # Max Packet Length, in bytes
        # LinkType

        header = struct.pack(
            "IHHIIII",
            self.PCAP_MAGIC_NUMBER,
            self.PCAP_MAJOR_VER,
            self.PCAP_MINOR_VER,
            0,
            0,
            256,
            self.PCAP_LINK_TYPE
        )
        self._send_internal(header)


    def _close(self):
        logger.info("Closing pipe")
        win32file.CloseHandle(self._pipe)


    def expand_str(self, packet: LogPacket) -> None:
        if len(packet.data) < 4:
            logger.error("Packet with less than 4 bytes of data! \n%s", packet)
            return

        address: int = _build_value(packet.data[:4])

        if address not in self._db.traceDB:
            logger.error("Packet header points to %d but this address does not map to the .out file!", address)
            return

        elf_str: ElfString = self._db.traceDB[address]

        # We don't use EVENT elfstrings except to search for their relevant EVENT_CONSTRUCT entry
        # Note that EVENT below is replaced with EVENT_CONSTRUCT
        if elf_str.opcode == Opcode.EVENT:
            elf_str = self._db.eventDB[elf_str.module_id + elf_str.event]

        # We have used the first 32-bit word already, so strip it off
        data = packet.data[4:]

        if elf_str.opcode in [Opcode.EVENT_CONSTRUCT, Opcode.FORMATTED_TEXT]:
            values = []
            while data:
                # Build the first four bytes into an int32
                values.append(_build_value(data[:4]))
                # Trim the first four bytes from data
                data = data[4:]

            if elf_str.opcode == Opcode.EVENT_CONSTRUCT:
                # TODO: TEMPORARY FIX FOR ALWAYS-SEND-4-ARGS
                values = values[:elf_str.string.count('%')]

            try:
                # Convert the list of values into a tuple so it is a valid argument to the % command
                packet._str_data = elf_str.string % tuple(values)
            except TypeError as exc:
                logger.error(
                    "Log.h elf string formatting failed: %s\nFormat string: %s, args: %s",
                    exc, elf_str.string, values
                )
                return

        elif elf_str.opcode == Opcode.BUFFER:
            # Format buffers as a list of space-sparated hex bytes
            data_str = " ".join(["0x%x" % byte for byte in data])
            # Join them with the user's elf string
            packet._str_data = elf_str.string + data_str


def _find_transport(transport) -> TransportABC:
    all_transports: Dict[str, TransportABC] = _load_entry_point("tilogger.transport")

    # Check our nominated transport is present in the list of transports
    if transport not in all_transports:
        raise SystemExit(
            f"Error: Transport '{transport}' is not available. "
            f"Registered transports: {list(all_transports.keys())}"
        )

    # Confirm that our transport derives from the correct class
    transport_class: TransportABC = all_transports[transport]
    if not TransportABC in transport_class.__bases__:
        raise SystemExit(
            f"Error: Specified transport {transport_class} must derive from tilogger.interface:TransportABC!"
        )

    return transport_class


def main():
    """
    Generates argparse parsers, reads command line arguments and starts a Logger.
    """
    # This is our official parser that we will use to consume the user's command line invocation
    parser = argparse.ArgumentParser(
        description="Provides shortcuts for test management"
    )

    # NOTE: We could attempt to improve this: look for transports before setting up the parser and
    # register using subcommands. However what we have works for now.

    # However, we want to provide good command line help for arguments defined in code we don't control
    # To do this we build a second parser that only looks for a transport
    transport_parser = argparse.ArgumentParser(
        description="Provides shortcuts for test management",

        # It is critical we do not have a default --help argument for this parser
        # Otherwise we will never be able to print help for all the arguments;
        # The first parse_known_args call will see --help, print help and SystemExit.
        add_help=False
    )
    # To reduce duplicate code, we add this argument to both parsers
    for temp_parser in [parser, transport_parser]:
        temp_parser.add_argument(
            "transport",
            type=str,
            help="Provide the name of a transport layer. This must correspond to an installed python package "
            "with a tilogger.transport entry point of the same name - see the documentation for details. "
            "Provide a valid transport name and run tilogger [transport] --help again to see "
            "transport-specific arguments."
        )

    # We wrap this and catch SystemExit (a usage error). In that case print help and exit
    try:
        # Try and parse sys.argv looking only for a single positional argument: transport
        # This returns a tuple, but we only care about the first value
        temp_args, _ = transport_parser.parse_known_args()
    except SystemExit:
        transport_parser.print_help()
        raise

    # Additional arguments needed for the Logger top level interface:
    parser.add_argument(
        "out_file",
        type=str,
        help="Path to the .out file running on the target."
    )
    # TODO: For now we're not exposing this configuration
    # parser.add_argument(
    #     "--pipe-name",
    #     type=str,
    #     nargs="?",
    #     default="tilogger_pipe",
    #     help="Choose the pipe name, which must be in the format '\\.\pipe\your-name'"
    # )
    parser.add_argument(
        "--verbose", "-v",
        action="store_true",
        help="Print significantly more information about ITM traces and incoming packets."
    )
    parser.add_argument(
        "--repickle", "-r",
        action="store_true",
        help="Ignore any existing information about this .out file and force tilogger to scan it for trace data again."
    )

    # Validate that our nominated transport exists and is registered
    # Note that this will just print an error message and SystemExit if the transport is not available
    transport_class = _find_transport(temp_args.transport)

    # Allow our transport class to register additional arguments
    transport_class.register_args(parser)

    # Now if the user has passed --help, all arguments for the correct transport
    args = parser.parse_args()

    # Finally, kick off the actual logging class
    Logger(transport_class, args)
