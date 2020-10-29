COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR ?= $(abspath ../../../../../../..)

include $(COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/imports.mak

KERNEL_BUILD := $(COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/kernel/freertos/builds/cc13x2_cc26x2/release

CC = "$(CCS_ARMCOMPILER)/bin/armcl"
LNK = "$(CCS_ARMCOMPILER)/bin/armcl"

OBJECTS = rfDualModeTx.obj ti_radio_config.obj ti_drivers_config.obj ti_devices_config.obj freertos_main_freertos.obj

NAME = rfDualModeTx

CFLAGS = -I../.. \
    -I. \
    -DDeviceFamily_CC13X2 \
    "-I$(COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/source" \
    "-I$(COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/source/ti/posix/ccs" \
    "-I$(FREERTOS_INSTALL_DIR)/FreeRTOS/Source/include" \
    "-I$(FREERTOS_INSTALL_DIR)/FreeRTOS/Source/portable/CCS/ARM_CM4F" \
    "-I$(KERNEL_BUILD)" \
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
    --diag_suppress=10063 \
    ti_utils_build_linker.cmd.genlibs \
    "-l$(KERNEL_BUILD)/ccs/freertos.lib" \
    -lti/devices/cc13x2_cc26x2/driverlib/bin/ccs/driverlib.lib \
    ../../freertos/ccs/cc13x2_cc26x2_freertos.cmd \
    "-m$(NAME).map" \
    --warn_sections \
    --display_error_number \
    --diag_wrap=off \
    --rom_model \
    "-i$(CCS_ARMCOMPILER)/lib" \
    -llibc.a

all: $(NAME).out

$(KERNEL_BUILD)/ccs/freertos.lib:
	@ $(ECHOBLANKLINE)
	@ echo $@ is not built.
	@ echo You can build it by issuing $(MAKE) in $(dir $@).
	@ $(ECHOBLANKLINE)

.INTERMEDIATE: syscfg
ti_radio_config.c ti_drivers_config.c ti_devices_config.c ti_utils_build_linker.cmd.genlibs ti_radio_config.h ti_drivers_config.h: syscfg
	@ echo generation complete

syscfg: ../../freertos/rfDualModeTx.syscfg
	@ echo Generating configuration files...
	@ $(SYSCONFIG_TOOL) --compiler ccs --product $(COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/.metadata/product.json --output $(@D) $<

rfDualModeTx.obj: ../../rfDualModeTx.c ti_radio_config.h ti_drivers_config.h
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< --output_file=$@

freertos_main_freertos.obj: ../../freertos/main_freertos.c ti_radio_config.h ti_drivers_config.h
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< --output_file=$@

ti_radio_config.obj: ti_radio_config.c ti_radio_config.h ti_drivers_config.h
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< --output_file=$@

ti_drivers_config.obj: ti_drivers_config.c ti_radio_config.h ti_drivers_config.h
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< --output_file=$@

ti_devices_config.obj: ti_devices_config.c ti_radio_config.h ti_drivers_config.h
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< --output_file=$@

$(NAME).out: $(OBJECTS) $(KERNEL_BUILD)/ccs/freertos.lib
	@ echo linking $@
	@ $(LNK) -z $(OBJECTS)  $(LFLAGS) -o $(NAME).out

clean:
	@ echo Cleaning...
	@ $(RM) $(OBJECTS) > $(DEVNULL) 2>&1
	@ $(RM) $(NAME).out > $(DEVNULL) 2>&1
	@ $(RM) $(NAME).map > $(DEVNULL) 2>&1
	@ $(RM) ti_radio_config.c ti_drivers_config.c ti_devices_config.c ti_utils_build_linker.cmd.genlibs ti_radio_config.h ti_drivers_config.h > $(DEVNULL) 2>&1
