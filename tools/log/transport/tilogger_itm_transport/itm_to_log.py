"""
Parse serial data using the Log Protocol as defined here:
https://confluence.itg.ti.com/display/OsloFW/ITM+Log+Protocol
"""

import logging
import enum
from collections import deque
import math
from abc import ABC, abstractmethod
import os
import sys
from dataclasses import *

from typing import Optional

from tilogger.interface import LogPacket
from tilogger.tracedb import ElfString, Opcode

from tilogger_itm_transport.itm_framer import ITMFrame, ITMSourceSWFrame, ITMOpcode, build_value, ITMStimulusPort

logger = logging.getLogger("ItmPacketiser")

SWIT_SIZE = 4
RESET_TOKEN = bytes([0xBB, 0xBB, 0xBB, 0xBB])


class ItmLogPacketData:
    def __init__(self, elf_string: ElfString, header: ITMSourceSWFrame, timestamp: Optional[int] = None):
        self.remaining_length: int
        self.elf_string = elf_string
        self.timestamp: Optional[int] = timestamp
        self.data: bytes = bytes(header.data)
        self.next_frame_has_length: bool = False

        if self.elf_string.opcode in [
                Opcode.FORMATTED_TEXT,
                Opcode.EVENT_CONSTRUCT,
                Opcode.EVENT
        ]:
            self.remaining_length = int(elf_string.nargs) * SWIT_SIZE

        elif self.elf_string.opcode == Opcode.BUFFER:
            self.remaining_length = 1024
            self.next_frame_has_length = True

        else:
            self.remaining_length = 0


    def append(self, itm_frame: ITMSourceSWFrame) -> None:
        if self.next_frame_has_length:
            # Set remaining length to the data frame and clear the read-length flag
            self.next_frame_has_length = False
            self.remaining_length = build_value(itm_frame.data)
            logger.debug('FRAMING: Frame length set to %d', self.remaining_length)

        else:
            # Adjust remaining length and append data
            self.remaining_length -= len(itm_frame)
            self.data += bytes(itm_frame.data)


    def to_log_packet(self) -> LogPacket:
        logger.debug('FRAMING: Handing off %s Packet with %d bytes of data', self.elf_string.opcode, len(self.data))
        return LogPacket.from_elf_string(
            self.elf_string,
            self.data,
            self.timestamp
        )


def rat_from_rtc(rtc_s):
    """
    Turn a real-time clock value into a radio time value

    Args:
      rtc_s: real-time-clock in seconds

    Returns:
        rat_s: radio time in seconds
        rat_t: radio time in ticks

    """
    # Updated assumed RAT tick based on RTC value (magic magic)
    # Doing the same assumptions as done inside the RF  (0x100000000LL/32768)
    # RTC in ticks like on our devices
    rtc_sec = int((math.floor(rtc_s) * 32768))
    rtc_subsec = int((rtc_s - rtc_sec) * 2 ** 32)
    new_rat = (rtc_sec << 32) + rtc_subsec
    # Conservatively assume that we are just about to increment
    # the RTC Scale with the 4 MHz that the RAT is running
    # Add the RAT offset for RTC == 0 * /
    new_rat += 4294967296 / 32768
    new_rat *= 4000000  # Scale to 4 MHz ticks
    new_rat = new_rat / 4294967296
    # Store as ticks
    rat_t = new_rat
    # Store as time
    rat_s = new_rat / 4000000
    return rat_s, rat_t


class ITMPacketiser():
    """
    Manages parsing ITM frames into LogPackets

    Stores a sorted dictionary of frames as they are being parsed.
    Stores a dictionary of watchpoint strings to match to corresponding watchpoints.
    Stores a trace database for use by members.

    Args:
        db: trace database
        clock: clock speed of embedded device

    """

    def __init__(self, db=None, clock=48000000, baud=12000000):
        self._trace_db = db
        self._event_sets = {}
        self._watchpoints = [None] * 4
        self._rat_t = 0
        self._rat_s = 0
        self._rtc_s = 0
        self.clock = clock
        self.offset = 0
        self.baudrate = int(baud)
        self.time_sync_subseconds = False

        self._current_packet: Optional[ItmLogPacketData] = None


    def parse(self, itm_frame: ITMFrame) -> Optional[LogPacket]:
        """
        The top-level ITMFrame parser.

        Will directly build all frames besides software source frames (these are build by build_sw_source_frame).
        When a timestamp is received from ITM, the running time values will be updated.
        After all frames are built, completed() will be called.

        Args:
          itm_frame: input ITMFrame

        Returns:

        """
        try:
            if itm_frame.opcode == ITMOpcode.TIMESTAMP:
                # Update running timestamps then discard the frame
                self._rtc_s += itm_frame.ts_counter / self.clock
                self._rat_s, self._rat_t = rat_from_rtc(self._rtc_s)
                self.offset = 0
                return None

            packet = None
            self.offset += (len(itm_frame) + 1) * 1 / self.baudrate

            if itm_frame.opcode == ITMOpcode.SOURCE_SW:
                assert isinstance(itm_frame, ITMSourceSWFrame)

                # This may not be the entire frame. Try to build it.
                # packet will be None unless the whole frame is completed
                packet = self.append_packet(itm_frame, self.offset)

                if packet:
                    return packet.to_log_packet()

        except Exception as exc:
            exc_type, _, exc_tb = sys.exc_info()
            if exc_tb:
                fname = os.path.split(exc_tb.tb_frame.f_code.co_filename)[1]
                logger.error("{} @ {} {}: ".format(exc_type, fname, exc_tb.tb_lineno) + str(exc))

        return None


    def append_packet(self, itm_frame: ITMSourceSWFrame, time_offset) -> Optional[ItmLogPacketData]:

        if itm_frame.port == ITMStimulusPort.STIM_HEADER:
            # Get elf data from header
            header = build_value(itm_frame.data)

            if header in self._trace_db.traceDB:
                elf_string = self._trace_db.traceDB[header]
                self._current_packet = ItmLogPacketData(elf_string, itm_frame, timestamp=self._rtc_s + time_offset)
                logger.debug('FRAMING: New Frame with len %d header 0x%x', self._current_packet.remaining_length, header)
            else:
                # This address does not exist in the trace database
                logger.warning("FRAMING: corruption: no trace database information at 0x%x", header)
                self._current_packet = None

            if self._current_packet.remaining_length == 0:
                packet = self._current_packet
                self._current_packet = None

                return packet

        elif itm_frame.port == ITMStimulusPort.STIM_TRACE:
            if not self._current_packet:
                # Fall through and return None
                logger.warning("Unexpected trace packet with no header! Discarding.")
            else:
                # Append the incoming data to the currently assembling packet
                self._current_packet.append(itm_frame)
                logger.debug(
                    'FRAMING: %d bytes added, remaining: %d',
                    len(itm_frame),
                    self._current_packet.remaining_length
                )

                # Note that < 0 here means we somehow appended a trace packet to a header that said it had no data
                # This should not happen as long as all opcodes are correctly configured
                if self._current_packet.remaining_length <= 0:
                    packet = self._current_packet
                    self._current_packet = None

                    return packet

        return None


    def reset(self):
        """Handle reset frame."""
