PROJECT_LOC = $(abspath ../../)
PROJECT_ROOT = $(PROJECT_LOC)
COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR ?= $(abspath ../../../../../../..)

include $(COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/imports.mak

XDCPATH = $(COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/source;$(COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/kernel/tirtos/packages;
SYSCONFIG_PRODUCT := $(COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/.metadata/product.json

CC = "$(CCS_ARMCOMPILER)/bin/armcl"
LNK = "$(CCS_ARMCOMPILER)/bin/armcl"

XDCTARGET = ti.targets.arm.elf.M4F
PLATFORM = ti.platforms.simplelink:CC1352R1F3

OBJECTS = ti_drivers_config.obj ti_radio_config.obj jdllc.obj mac_user_config.obj cui.obj oad_client.obj oad_protocol.obj oad_storage.obj oad_image_header_app.obj flash_interface_int_rtos_NVS.obj crc32.obj crc.obj nvocmp.obj  mac_cfg.obj mac_settings.obj macTask.obj api_mac.obj main.obj sensor.obj ssf.obj CC1352R1_LAUNCHXL_fxns.obj

CONFIGPKG = sensor_oad_onchip_persistent_2_4g_secure

NAME = sensor_oad_onchip_persistent_2_4g_secure

CFLAGS = -I../.. \
    -I. \
    "--cmd_file=${PROJECT_LOC}/application/defines/sensor_oad_onchip_persistent_2_4g.opts" \
    "-I${PROJECT_ROOT}/application/sensor/oad" \
    "-I${PROJECT_ROOT}/application/sensor" \
    -O4 \
    --c99 \
    --opt_for_speed=0 \
    --fp_reassoc=off \
    --unaligned_access=on \
    --embedded_constants=on \
    --wchar_t=16 \
    --enum_type=packed \
    --common=on \
    --sat_reassoc=off \
    --plain_char=unsigned \
    -DTIMAC_ROM_IMAGE_BUILD \
    "-I${PROJECT_ROOT}" \
    "-I${PROJECT_ROOT}/application" \
    "-I${PROJECT_ROOT}/software_stack/ti15_4stack/osal" \
    "-I${COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR}/source/ti/ti154stack/apps" \
    "-I${COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR}/source/ti/ti154stack/common" \
    "-I${COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR}/source/ti/ti154stack/common/boards" \
    "-I${COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR}/source/ti/ti154stack/common/osal_port" \
    "-I${COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR}/source/ti/ti154stack/common/util" \
    "-I${COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR}/source/ti/ti154stack/common/inc" \
    "-I${COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR}/source/ti/ti154stack/common/stack/src" \
    "-I${COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR}/source/ti/ti154stack/common/stack/tirtos/inc" \
    "-I${COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR}/source/ti/ti154stack/common/heapmgr" \
    "-I${COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR}/source/ti/ti154stack/services/saddr" \
    "-I${COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR}/source/ti/ti154stack/services/sdata" \
    "-I${COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR}/source/ti/ti154stack/hal/crypto" \
    "-I${COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR}/source/ti/ti154stack/hal/platform" \
    "-I${COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR}/source/ti/ti154stack/hal/rf" \
    "-I${COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR}/source/ti/ti154stack/fh" \
    "-I${COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR}/source/ti/ti154stack/high_level" \
    "-I${COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR}/source/ti/ti154stack/inc" \
    "-I${COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR}/source/ti/ti154stack/rom" \
    "-I${COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR}/source/ti/ti154stack/inc/cc13xx" \
    "-I${COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR}/source/ti/ti154stack/low_level/cc13xx" \
    "-I${COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR}/source/ti/ti154stack/tracer" \
    "-I${COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR}/source/ti/common/cc26xx/flash_interface" \
    "-I${COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR}/source/ti/common/cc26xx/crc" \
    "-I${COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR}/source/ti/ti154stack" \
    "-I${COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR}/source/ti/ti154stack/apps/common/oad" \
    "-I${COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR}/source/ti" \
    "-I${COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR}/source/ti/ti154stack/common/oad/CC13X2_CC26X2R1_LAUNCHXL" \
    "-I${COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR}/source/ti/ti154stack/apps/sensor_oad_onchip" \
    "-I${COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR}/source/ti/ti154stack/apps/sensor" \
    "-I${COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR}/source/ti/devices/cc13x2_cc26x2" \
    "-I${COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR}/source/ti/devices/cc13x2_cc26x2/inc" \
    "-I${COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR}/source/ti/devices/cc13x2_cc26x2/driverlib" \
    "-I${COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR}/source/ti/common/nv" \
    "-I${COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR}/source/ti/common/cui" \
    -DDeviceFamily_CC13X2 \
    -DSECURITY \
    -DOAD_IMG_A \
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

LFLAGS = "--library=${COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR}/source/ti/ti154stack/library/tirtos/ccs/timac_rom_PG2_0_rom_api_linker.cmd" \
    --define=NVOCMP_NVPAGES=2 \
    --define=OAD_IMG_A=1 \
    "-l${PROJECT_BUILD_DIR}/syscfg/ti_utils_build_linker.cmd.genlibs" \
    "-i$(COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/source" \
    "-i$(COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/kernel/tirtos/packages" \
    --unused_section_elimination=on \
    --diag_wrap=off \
    "--xml_link_info=${ProjName}_linkInfo.xml" \
    -x \
    "-l$(COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/source/ti/devices/cc13x2_cc26x2/driverlib/bin/ccs/driverlib.lib" \
    ../../cc13x2lp_oad_onchip.cmd \
    "-m$(NAME).map" \
    --warn_sections \
    --display_error_number \
    --rom_model \
    "-i$(CCS_ARMCOMPILER)/lib" \
    -llibc.a

.PRECIOUS: $(CONFIGPKG)/compiler.opt $(CONFIGPKG)/linker.cmd

all: $(NAME).out

$(CONFIGPKG)/compiler.opt: $(CONFIGPKG)/linker.cmd

$(CONFIGPKG)/linker.cmd: $(COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/source/ti/ti154stack/apps/../../../../source/ti/ti154stack/common/boards/CC13X2_LAUNCHXL/app_oad_onchip_persistent.cfg
	@ echo Running Configuro...
	@ $(XDC_INSTALL_DIR)/xs --xdcpath="$(XDCPATH)" xdc.tools.configuro -c "$(CCS_ARMCOMPILER)" -t $(XDCTARGET) -p $(PLATFORM) --compileOptions "$(CFLAGS)" -o "$(CONFIGPKG)" $(COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/source/ti/ti154stack/apps/../../../../source/ti/ti154stack/common/boards/CC13X2_LAUNCHXL/app_oad_onchip_persistent.cfg

.INTERMEDIATE: syscfg
ti_drivers_config.c ti_drivers_config.h ti_154stack_config.h ti_154stack_oad_config.h ti_154stack_features.h ti_radio_config.c ti_radio_config.h: syscfg
	@ echo generation complete

syscfg: ../../sensor.syscfg
	@ echo Generating configuration files...
	@ $(SYSCONFIG_TOOL) --product $(SYSCONFIG_PRODUCT) --compiler ccs --output $(@D) $<

ti_drivers_config.obj: ti_drivers_config.c $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< --cmd_file=$(CONFIGPKG)/compiler.opt --output_file=$@

jdllc.obj: $(COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/source/ti/ti154stack/apps/../../../../source/ti/ti154stack/common/jdllc/jdllc.c ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< --cmd_file=$(CONFIGPKG)/compiler.opt --output_file=$@

mac_user_config.obj: $(COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/source/ti/ti154stack/apps/../../../../source/ti/ti154stack/common/boards/mac_user_config.c ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< --cmd_file=$(CONFIGPKG)/compiler.opt --output_file=$@

cui.obj: $(COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/source/ti/ti154stack/apps/../../../../source/ti/common/cui/cui.c ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< --cmd_file=$(CONFIGPKG)/compiler.opt --output_file=$@

oad_client.obj: $(COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/source/ti/ti154stack/apps/../../../../source/ti/ti154stack/apps/common/oad/oad_client.c ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< --cmd_file=$(CONFIGPKG)/compiler.opt --output_file=$@

oad_protocol.obj: $(COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/source/ti/ti154stack/apps/../../../../source/ti/ti154stack/common/oad/CC13X2_CC26X2R1_LAUNCHXL/native_oad/oad_protocol.c ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< --cmd_file=$(CONFIGPKG)/compiler.opt --output_file=$@

oad_storage.obj: $(COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/source/ti/ti154stack/apps/../../../../source/ti/ti154stack/common/oad/CC13X2_CC26X2R1_LAUNCHXL/native_oad/oad_storage.c ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< --cmd_file=$(CONFIGPKG)/compiler.opt --output_file=$@

oad_image_header_app.obj: $(COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/source/ti/ti154stack/apps/../../../../source/ti/ti154stack/common/oad/CC13X2_CC26X2R1_LAUNCHXL/oad_image_header_app.c ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< --cmd_file=$(CONFIGPKG)/compiler.opt --output_file=$@

flash_interface_int_rtos_NVS.obj: $(COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/source/ti/ti154stack/apps/../../../../source/ti/common/cc26xx/flash_interface/internal/flash_interface_int_rtos_NVS.c ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< --cmd_file=$(CONFIGPKG)/compiler.opt --output_file=$@

crc32.obj: $(COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/source/ti/ti154stack/apps/../../../../source/ti/common/cc26xx/crc/crc32.c ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< --cmd_file=$(CONFIGPKG)/compiler.opt --output_file=$@

crc.obj: $(COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/source/ti/ti154stack/apps/../../../../source/ti/common/nv/crc.c ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< --cmd_file=$(CONFIGPKG)/compiler.opt --output_file=$@

nvocmp.obj: $(COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/source/ti/ti154stack/apps/../../../../source/ti/common/nv/nvocmp.c ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< --cmd_file=$(CONFIGPKG)/compiler.opt --output_file=$@

mac_cfg.obj: $(COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/source/ti/ti154stack/apps/../../../../source/ti/ti154stack/high_level/mac_cfg.c ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< --cmd_file=$(CONFIGPKG)/compiler.opt --output_file=$@

mac_settings.obj: $(COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/source/ti/ti154stack/apps/../../../../source/ti/ti154stack/low_level/cc13xx/mac_settings.c ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< --cmd_file=$(CONFIGPKG)/compiler.opt --output_file=$@

macTask.obj: $(COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/source/ti/ti154stack/apps/../../../../source/ti/ti154stack/common/osal_port/osal_port_tirtos/macTask.c ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< --cmd_file=$(CONFIGPKG)/compiler.opt --output_file=$@

api_mac.obj: $(COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/source/ti/ti154stack/apps/../../../../source/ti/ti154stack/common/osal_port/osal_port_tirtos/api_mac.c ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< --cmd_file=$(CONFIGPKG)/compiler.opt --output_file=$@

main.obj: $(COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/source/ti/ti154stack/apps/sensor/main.c ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< --cmd_file=$(CONFIGPKG)/compiler.opt --output_file=$@

sensor.obj: $(COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/source/ti/ti154stack/apps/sensor/sensor.c ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< --cmd_file=$(CONFIGPKG)/compiler.opt --output_file=$@

ssf.obj: $(COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/source/ti/ti154stack/apps/sensor/ssf.c ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< --cmd_file=$(CONFIGPKG)/compiler.opt --output_file=$@

CC1352R1_LAUNCHXL_fxns.obj: ../../CC1352R1_LAUNCHXL_fxns.c ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< --cmd_file=$(CONFIGPKG)/compiler.opt --output_file=$@

ti_radio_config.obj: ti_radio_config.c ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< --cmd_file=$(CONFIGPKG)/compiler.opt --output_file=$@

$(NAME).out: $(OBJECTS) $(CONFIGPKG)/linker.cmd
	@ echo linking $@
	@ $(LNK) $(CFLAGS) -z $(OBJECTS) -l$(CONFIGPKG)/linker.cmd $(LFLAGS) -o $(NAME).out

clean:
	@ echo Cleaning...
	@ $(RM) $(OBJECTS) > $(DEVNULL) 2>&1
	@ $(RM) $(NAME).out > $(DEVNULL) 2>&1
	@ $(RM) $(NAME).map > $(DEVNULL) 2>&1
	@ $(RMDIR) $(NAME) > $(DEVNULL) 2>&1
	@ $(RM) ti_drivers_config.c ti_drivers_config.h ti_154stack_config.h ti_154stack_oad_config.h ti_154stack_features.h ti_radio_config.c ti_radio_config.h > $(DEVNULL) 2>&1
