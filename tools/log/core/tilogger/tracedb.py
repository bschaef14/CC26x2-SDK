import os
import logging
import pickle
import hashlib
import json
import enum

from pathlib import Path
from typing import Optional
from appdirs import AppDirs, user_data_dir
from elftools.dwarf.descriptions import describe_form_class
from elftools.elf.elffile import ELFFile, Section

# Set up logger
logger = logging.getLogger("TraceDB")

TRACE_SECTION_NAME = ".log_data"


class Opcode(enum.Enum):
    FORMATTED_TEXT = 0
    EVENT = 1
    BUFFER = 2
    RESET = 3
    EVENT_CONSTRUCT = 4

# String to opcode dictionary
log_string_to_opcode = {
    "LOG_OPCODE_FORMATED_TEXT": Opcode.FORMATTED_TEXT,
    "LOG_OPCODE_BUFFER": Opcode.BUFFER,
    "LOG_OPCODE_EVENT": Opcode.EVENT,
    "LOG_EVENT_CONSTRUCT": Opcode.EVENT_CONSTRUCT,
}


class ElfString:
    def __init__(self, value: str):
        # Get opcode and strip from value
        self.value = value
        opcode_str, value = value.split("\x1e", 1)

        try:
            self.opcode: Opcode = log_string_to_opcode[opcode_str]
        except KeyError:
            logger.critical("ElfString cannot be instantiated with unknown opcode %s", opcode_str)

        self.file: str
        self.line: str
        self.event: str
        self.string: str
        self.module_id: str
        self.nargs: int
        self.level: str

        if self.opcode == Opcode.EVENT_CONSTRUCT:
            self.file, self.line, self.event, self.module_id, self.string, tmp_nargs = value.split("\x1e")

        elif self.opcode == Opcode.EVENT:
            self.file, self.line, self.level, self.module_id, self.event, tmp_nargs = value.split("\x1e")

            # TODO: TEMPORARY FIX FOR ALWAYS-SEND-4-ARGS
            tmp_nargs = "4"

        elif self.opcode == Opcode.FORMATTED_TEXT:
            self.file, self.line, self.level, self.module_id, self.string, tmp_nargs = value.split("\x1e")

        elif self.opcode == Opcode.BUFFER:
            # Note that nargs == 0 for BUFFER strings - length is only known at runtime
            self.file, self.line, self.level, self.module_id, self.string, tmp_nargs = value.split("\x1e")

        else:
            raise Exception(f"Unexpected opcode {self.opcode}")

        self.nargs = int(tmp_nargs)


class TraceDB:
    def __init__(self, elf, repickle: bool, sdk_path=""):
        self.elf = elf
        self.sdk_path = sdk_path
        self.device = ""
        self.rom_sub_path = ""
        self.traceDB = {}
        self.eventDB = {}
        self.functionDB = {}
        self.module_db = {}

        # Locate and create %AppData% directories
        dirs = AppDirs("logger", "tilogger")
        user_data_dir = Path(dirs.user_data_dir)
        user_data_dir.mkdir(parents = True, exist_ok = True)

        trace_db_pickle_file = (user_data_dir / "trace_db.pkl").resolve()
        func_db_pickle_file = (user_data_dir / "func_db.pkl").resolve()
        event_db_pickle_file = (user_data_dir / "event_db.pkl").resolve()
        json_file = (user_data_dir / "trace_db.json").resolve()

        build_trace_db = True
        build_func_db = False if self.sdk_path is "" else True

        # Build current hash
        hasher = hashlib.md5()
        try:
            with open(self.elf, 'rb') as f:
                hasher.update(f.read())
        except FileNotFoundError as e:
            logger.error("Not able to open elf file " + self.elf)
            raise e

        current_hash = hasher.hexdigest()
        json_dict = {}

        try:
            # Read json data
            with open(json_file, 'rb') as f:
                json_dict = json.load(f)
            try:
                # Get hash of stored elf file
                last_hash = json_dict["hash"]
                # See whether hash's match
                if current_hash == last_hash:
                    build_trace_db = False
            except KeyError:
                logging.error("No previous hash found")
            # See if SDK path changed
            if build_func_db is True:
                try:
                    last_sdk_path = json_dict["sdk"]
                    if self.sdk_path == last_sdk_path:
                        build_func_db = False
                except KeyError:
                    logger.error("No SDK path in JSON file")
        except (json.JSONDecodeError, FileNotFoundError):
            logger.error("Can't open JSON file @ " + str(json_file))

        # Build and pickle databases if needed
        if build_trace_db is True or build_func_db is True or repickle is True:
            # Build elf information
            self.get_elf_db()
            # Pickle trace database
            with open(trace_db_pickle_file, "wb") as f:
                pickle.dump(self.traceDB, f)
            # Pickle event database
            with open(event_db_pickle_file, "wb") as f:
                pickle.dump(self.eventDB, f)
            # Store elf hash to json file
            json_dict["hash"] = current_hash
            with open(json_file, 'w') as f:
                json.dump(json_dict, f)
            logger.critical("TraceDB and EventDB have been pickled")
            # Pickle database
            with open(func_db_pickle_file, "wb") as f:
                pickle.dump(self.functionDB, f)
            # Store sdk path to json file
            json_dict["sdk"] = self.sdk_path
            with open(json_file, 'w') as f:
                json.dump(json_dict, f)
            logger.critical("FuncDB has been pickled")

        elif build_trace_db is False and build_func_db is False:
            # Load from pickled trace file
            try:
                with open(trace_db_pickle_file, 'rb') as f:
                    self.traceDB = pickle.load(f)
                with open(event_db_pickle_file, 'rb') as f:
                    self.eventDB = pickle.load(f)
                logger.critical("Pickled trace and event databases successfully loaded")
            except FileNotFoundError:
                logger.error("Pickled trace_db or event_db file not found")
            # Load from pickled func db file
            try:
                with open(func_db_pickle_file, 'rb') as f:
                    self.functionDB = pickle.load(f)
                logger.critical("Pickled function database successfully loaded")
            except FileNotFoundError:
                logger.error("Pickled trace_db file not found")
        logger.critical("Done configuring databases")


    def add_to_funcdb(self, elf_file):
        # Create ELF file object
        elf = ELFFile(elf_file)
        # Get debug info
        dwarf_info = elf.get_dwarf_info()
        # Go over all compiler units (CU)
        for CU in dwarf_info.iter_CUs():
            # Go over all debug information entries (DIE) in given CU
            for DIE in CU.iter_DIEs():
                # We only care about subprogram
                if DIE.tag == 'DW_TAG_subprogram':
                    if DIE.attributes.get('DW_AT_low_pc', None) is not None:
                        low_pc = DIE.attributes['DW_AT_low_pc'].value
                        if DIE.attributes.get('DW_AT_name', None) is not None:
                            fxn_name = DIE.attributes['DW_AT_name'].value
                            # DWARF v4 in section 2.17 describes how to interpret the
                            # DW_AT_high_pc attribute based on the class of its form.
                            # For class 'address' it's taken as an absolute address
                            # (similarly to DW_AT_low_pc); for class 'constant', it's
                            # an offset from DW_AT_low_pc.
                            high_pc_attr = DIE.attributes['DW_AT_high_pc']
                            high_pc_attr_class = describe_form_class(high_pc_attr.form)
                            if high_pc_attr_class == 'address':
                                high_pc = high_pc_attr.value
                            elif high_pc_attr_class == 'constant':
                                high_pc = low_pc + high_pc_attr.value
                            else:
                                logger.error('Error: invalid DW_AT_high_pc class:', high_pc_attr_class)
                                continue

                            # Get file
                            try:
                                file_index = DIE.attributes['DW_AT_decl_file'].value
                            except KeyError:
                                continue
                            else:
                                line_prog = dwarf_info.line_program_for_CU(CU)
                                file = line_prog['file_entry'][file_index - 1].name
                                # Get line
                                line = DIE.attributes['DW_AT_decl_line'].value
                                # Add to functionDB
                                self.functionDB[(low_pc, high_pc)] = [fxn_name, file, line]
                                # Print for debugging
                                logger.debug(
                                    "{}() @ {}:{}".format(fxn_name.decode("utf-8"), file.decode("utf-8"), line))


    def get_elf_db(self):
        with open(self.elf, 'rb') as f:
            self.get_log_db(f)


    def get_log_db(self, f):
        elf = ELFFile(f)
        # Find LOG section
        trace_sec: Optional[Section] = None
        for secnum, sec in enumerate(elf.iter_sections()):
            if TRACE_SECTION_NAME in sec.name:
                trace_sec = elf.get_section(secnum)
                break
        if trace_sec is None:
            raise ValueError("Trace sections not found in elf file. Ensure that the linker file is correct and that "
                             "there is at least one module and level enabled.")

        logger.critical("Creating dictionary of strings and functions from elf file...")
        logger.debug("LOG TRACE =======================================================")
        trace_base_addr = trace_sec.header['sh_addr']
        logger.debug("Located .log_data section at 0x%x", trace_base_addr)

        # Build LOG trace database by searching in symbol table
        for sym in elf.get_section_by_name('.symtab').iter_symbols():
            if sym.entry.st_value & trace_base_addr == trace_base_addr and "LogSymbol" in sym.name:
                # Find offset into section by subtracting section base address
                offset = trace_sec.header['sh_offset'] + (sym.entry.st_value - trace_base_addr)
                # Seek to offset in ELF
                trace_sec.stream.seek(offset)
                # Read until end of section
                value = trace_sec.stream.read(trace_sec.header['sh_size'] - (
                        offset - trace_sec.header['sh_offset']))

                # Truncate output at null character and remove quotes
                value = value.decode("utf-8").split("\0")[0].replace("\"", "")
                # Create new ElfString to store in dictionary
                elf_string = ElfString(value)
                # Add to relevant database
                if elf_string.opcode is Opcode.EVENT_CONSTRUCT:
                    self.eventDB[elf_string.module_id + elf_string.event] = elf_string
                else:
                    self.traceDB[sym.entry.st_value] = elf_string
                logger.debug("0x%x--> %s", sym.entry.st_value, value.replace("\x1e", ", "))

            elif sym.entry.st_value & trace_base_addr == trace_base_addr and "LogModule" in sym.name:
                # Find offset into section by subtracting section base address
                offset = trace_sec.header['sh_offset'] + (sym.entry.st_value - trace_base_addr)
                # Seek to offset in ELF
                trace_sec.stream.seek(offset)
                # Read until end of section
                value = trace_sec.stream.read(trace_sec.header['sh_size'] - (
                        offset - trace_sec.header['sh_offset']))

                # Truncate output at null character and remove quotes
                value = value.decode("utf-8").split("\0")[0].replace("\"", "")

                logger.critical("Found Module %x --> %s", sym.entry.st_value, value)
