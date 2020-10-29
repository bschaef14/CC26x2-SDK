import sys
import atexit
import argparse
import queue

from typing import Optional, NoReturn, List

from tilogger.interface import LogPacket, TransportABC
from tilogger.logger import Logger

from tilogger_itm_transport.serial_rx import SerialRx
from tilogger_itm_transport.itm_framer import ITMFramer
from tilogger_itm_transport.itm_to_log import ITMPacketiser

class ITM_Transport(TransportABC):

    def __init__(self):
        super().__init__()

        self._com_port: int = 0
        self._baud_rate: int = 0
        self.serial: Optional[SerialRx] = None


    def start(self, logger: Optional[Logger], args, tracedb) -> NoReturn:
        self._com_port = args.com_port
        self._baud_rate = args.baud_rate
        self.serial = SerialRx(self._com_port)

        atexit.register(self.serial.close)

        frame_queue: queue.Queue = queue.Queue()
        framer = ITMFramer(frame_queue)

        # Note we use "Logger == None" as our 'ITM only mode' flag
        if logger:
            packetiser = ITMPacketiser(tracedb)
        else:
            packetiser = None

        rx_data: bytearray = bytearray()
        while True:
            rx_data.extend(self.serial.receive())
            rx_data = framer.parse(rx_data)

            try:
                result = frame_queue.get(block=False)
            except queue.Empty:
                continue

            if logger:
                packet: Optional[LogPacket] = packetiser.parse(result)
                if packet:
                    logger.log(packet)
            else:
                print(result)


    def reset(self):
        pass


    def timestamp_to_seconds(self, timestamp: int) -> float:
        return float(timestamp)


    @classmethod
    def register_args(cls, parser: argparse.ArgumentParser):
        parser.add_argument(
            'baud_rate',
            type=int,
            help='Choose a baud rate for serial communication. This should be 115200 for the XDS110 Launchpad debugger.'
        )
        parser.add_argument(
            'com_port',
            type=str,
            help='Choose the COM port for the device to be debugged. Note that this must be the Auxiliary COM port.'
        )
        return parser


def itm_parser_main():
    parser = argparse.ArgumentParser(prog="itm_transport")
    ITM_Transport.register_args(parser)

    args = parser.parse_args()

    transport = ITM_Transport()
    transport.start(None, args, None)
