COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR ?= $(abspath ../../../../../../..)

include $(COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/imports.mak

KERNEL_BUILD := $(COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/kernel/tirtos/builds/cc13x2_cc26x2/release

CC = "$(CCS_ARMCOMPILER)/bin/armcl"
LNK = "$(CCS_ARMCOMPILER)/bin/armcl"

OBJECTS = rfPacketRx.obj RFQueue.obj ti_radio_config.obj ti_drivers_config.obj ti_devices_config.obj tirtos_main_tirtos.obj

CONFIGPKG = $(KERNEL_BUILD)/ccs

NAME = rfPacketRx

CFLAGS = -I../.. \
    -I. \
    -DDeviceFamily_CC13X2 \
    "-I$(COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/source" \
    "-I$(COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/source/ti/posix/ccs" \
    --silicon_version=7M4 \
    --code_state=16 \
    --little_endian \
    --display_error_number \
    --diag_warning=255 \
    --diag_wrap=off \
    --gen_func_subsections=on \
    --float_support=FPv4SPD16 \
    --symdebug:dwarf \
    "-I$(CCS_ARMCOMPILER)/include"

LFLAGS = "-i$(COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/source" \
    "-i$(COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/kernel/tirtos/packages" \
    ti_utils_build_linker.cmd.genlibs \
    "-l$(KERNEL_BUILD)/ccs/linker.cmd" \
    -lti/devices/cc13x2_cc26x2/driverlib/bin/ccs/driverlib.lib \
    ../../tirtos/ccs/cc13x2_cc26x2_tirtos.cmd \
    "-m$(NAME).map" \
    --warn_sections \
    --display_error_number \
    --diag_wrap=off \
    --rom_model \
    "-i$(CCS_ARMCOMPILER)/lib" \
    -llibc.a

all: $(NAME).out

$(CONFIGPKG)/linker.cmd $(CONFIGPKG)/compiler.opt:
	@ $(ECHOBLANKLINE)
	@ echo $(CONFIGPKG) is not built.
	@ echo You can build it by issuing $(MAKE) in $(CONFIGPKG).
	@ $(ECHOBLANKLINE)

.INTERMEDIATE: syscfg
ti_radio_config.c ti_drivers_config.c ti_devices_config.c ti_utils_build_linker.cmd.genlibs ti_radio_config.h ti_drivers_config.h: syscfg
	@ echo generation complete

syscfg: ../../tirtos/rfPacketRx.syscfg
	@ echo Generating configuration files...
	@ $(SYSCONFIG_TOOL) --compiler ccs --product $(COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/.metadata/product.json --output $(@D) $<

rfPacketRx.obj: ../../rfPacketRx.c ti_radio_config.h ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< --cmd_file=$(CONFIGPKG)/compiler.opt --output_file=$@

RFQueue.obj: ../../RFQueue.c ti_radio_config.h ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< --cmd_file=$(CONFIGPKG)/compiler.opt --output_file=$@

tirtos_main_tirtos.obj: ../../tirtos/main_tirtos.c ti_radio_config.h ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< --cmd_file=$(CONFIGPKG)/compiler.opt --output_file=$@

ti_radio_config.obj: ti_radio_config.c ti_radio_config.h ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< --cmd_file=$(CONFIGPKG)/compiler.opt --output_file=$@

ti_drivers_config.obj: ti_drivers_config.c ti_radio_config.h ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< --cmd_file=$(CONFIGPKG)/compiler.opt --output_file=$@

ti_devices_config.obj: ti_devices_config.c ti_radio_config.h ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< --cmd_file=$(CONFIGPKG)/compiler.opt --output_file=$@

$(NAME).out: $(OBJECTS) $(CONFIGPKG)/linker.cmd
	@ echo linking $@
	@ $(LNK) -z $(OBJECTS)  $(LFLAGS) -o $(NAME).out

clean:
	@ echo Cleaning...
	@ $(RM) $(OBJECTS) > $(DEVNULL) 2>&1
	@ $(RM) $(NAME).out > $(DEVNULL) 2>&1
	@ $(RM) $(NAME).map > $(DEVNULL) 2>&1
	@ $(RM) ti_radio_config.c ti_drivers_config.c ti_devices_config.c ti_utils_build_linker.cmd.genlibs ti_radio_config.h ti_drivers_config.h > $(DEVNULL) 2>&1
