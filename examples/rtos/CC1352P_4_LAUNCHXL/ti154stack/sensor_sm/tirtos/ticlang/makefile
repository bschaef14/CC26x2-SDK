PROJECT_LOC = $(abspath ../../)
PROJECT_ROOT = $(PROJECT_LOC)
COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR ?= $(abspath ../../../../../../..)

include $(COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/imports.mak

XDCPATH = $(COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/source;$(COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/kernel/tirtos/packages;
SYSCONFIG_PRODUCT := $(COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/.metadata/product.json

CC = "$(TICLANG_ARMCOMPILER)/bin/tiarmclang"
LNK = "$(TICLANG_ARMCOMPILER)/bin/tiarmclang"

XDCTARGET = ti.targets.arm.clang.M4F
PLATFORM = ti.platforms.simplelink:CC1352P1F3

OBJECTS = ti_drivers_config.obj ti_radio_config.obj ti_devices_config.obj jdllc.obj mac_user_config.obj cui.obj sm_ti154.obj sm_ecc_ti154.obj sm_toolbox.obj crc.obj nvocmp.obj  mac_cfg.obj mac_settings.obj macTask.obj api_mac.obj main.obj sensor.obj ssf.obj CC1352P_4_LAUNCHXL_fxns.obj

CONFIGPKG = sensor_sm

NAME = sensor_sm

CFLAGS = -I../.. \
    -I. \
    "@${PROJECT_LOC}/application/defines/sensor.opts" \
    "-I${PROJECT_ROOT}/application/sensor" \
    -Oz \
    -std=gnu9x \
    -munaligned-access \
    -fshort-enums \
    -fcommon \
    -funsigned-char \
    -mlittle-endian \
    -fdiagnostics-show-option \
    -ffunction-sections \
    -g \
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
    "-I${COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR}/source/ti/ti154stack/apps/sensor" \
    "-I${COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR}/source/ti/devices/cc13x2_cc26x2" \
    "-I${COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR}/source/ti/devices/cc13x2_cc26x2/inc" \
    "-I${COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR}/source/ti/devices/cc13x2_cc26x2/driverlib" \
    "-I${COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR}/source/ti/common/nv" \
    "-I${COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR}/source/ti/common/cui" \
    -DBoard_EXCLUDE_NVS_EXTERNAL_FLASH \
    -DDeviceFamily_CC13X2 \
    "-I$(COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/source" \
    "-I$(COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/source/ti/posix/ticlang" \
    -gdwarf-3 \
    -mcpu=cortex-m4 \
    -march=armv7e-m \
    -mthumb \
    -mfloat-abi=hard \
    -mfpu=fpv4-sp-d16

LFLAGS = "-l${COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR}/source/ti/ti154stack/library/tirtos/ticlang/timac_rom_PG2_0_rom_api_linker.cmd" \
    --define=NVOCMP_NVPAGES=2 \
    "-l${PROJECT_BUILD_DIR}/syscfg/ti_utils_build_linker.cmd.genlibs" \
    "-i$(COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/source" \
    "-i$(COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/kernel/tirtos/packages" \
    --unused_section_elimination=on \
    --diag_wrap=off \
    "--xml_link_info=${ProjName}_linkInfo.xml" \
    -x \
    --display_error_number \
    --rom_model \
    --warn_sections \
    "-l$(COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/source/ti/devices/cc13x2_cc26x2/driverlib/bin/ticlang/driverlib.lib" \
    ../../cc13x2lp.cmd \
    "-Wl,-m,$(NAME).map" \
    -Wl,-c \
    "-Wl,-i$(TICLANG_ARMCOMPILER)/lib" \
    -llibc.a

.PRECIOUS: $(CONFIGPKG)/compiler.opt $(CONFIGPKG)/linker.cmd

all: $(NAME).out

$(CONFIGPKG)/compiler.opt: $(CONFIGPKG)/linker.cmd

$(CONFIGPKG)/linker.cmd: $(COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/source/ti/ti154stack/apps/../../../../source/ti/ti154stack/common/boards/CC13X2_LAUNCHXL/app.cfg
	@ echo Running Configuro...
	@ $(XDC_INSTALL_DIR)/xs --xdcpath="$(XDCPATH)" xdc.tools.configuro -c "$(TICLANG_ARMCOMPILER)" -t $(XDCTARGET) -p $(PLATFORM) --compileOptions "$(CFLAGS)" -o "$(CONFIGPKG)" $(COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/source/ti/ti154stack/apps/../../../../source/ti/ti154stack/common/boards/CC13X2_LAUNCHXL/app.cfg

.INTERMEDIATE: syscfg
ti_drivers_config.c ti_drivers_config.h ti_154stack_config.h ti_154stack_features.h ti_radio_config.c ti_radio_config.h ti_devices_config.c: syscfg
	@ echo generation complete

syscfg: ../../sensor.syscfg
	@ echo Generating configuration files...
	@ $(SYSCONFIG_TOOL) --product $(SYSCONFIG_PRODUCT) --compiler ticlang --output $(@D) $<

ti_drivers_config.obj: ti_drivers_config.c $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) -c $< @$(CONFIGPKG)/compiler.opt -o $@

jdllc.obj: $(COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/source/ti/ti154stack/apps/../../../../source/ti/ti154stack/common/jdllc/jdllc.c ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) -c $< @$(CONFIGPKG)/compiler.opt -o $@

mac_user_config.obj: $(COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/source/ti/ti154stack/apps/../../../../source/ti/ti154stack/common/boards/mac_user_config.c ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) -c $< @$(CONFIGPKG)/compiler.opt -o $@

cui.obj: $(COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/source/ti/ti154stack/apps/../../../../source/ti/common/cui/cui.c ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) -c $< @$(CONFIGPKG)/compiler.opt -o $@

sm_ti154.obj: $(COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/source/ti/ti154stack/apps/../../../../source/ti/ti154stack/common/sm/sm_ti154.c ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) -c $< @$(CONFIGPKG)/compiler.opt -o $@

sm_ecc_ti154.obj: $(COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/source/ti/ti154stack/apps/../../../../source/ti/ti154stack/common/sm/sm_ecc_ti154.c ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) -c $< @$(CONFIGPKG)/compiler.opt -o $@

sm_toolbox.obj: $(COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/source/ti/ti154stack/apps/../../../../source/ti/ti154stack/common/sm/sm_toolbox.c ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) -c $< @$(CONFIGPKG)/compiler.opt -o $@

crc.obj: $(COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/source/ti/ti154stack/apps/../../../../source/ti/common/nv/crc.c ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) -c $< @$(CONFIGPKG)/compiler.opt -o $@

nvocmp.obj: $(COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/source/ti/ti154stack/apps/../../../../source/ti/common/nv/nvocmp.c ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) -c $< @$(CONFIGPKG)/compiler.opt -o $@

mac_cfg.obj: $(COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/source/ti/ti154stack/apps/../../../../source/ti/ti154stack/high_level/mac_cfg.c ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) -c $< @$(CONFIGPKG)/compiler.opt -o $@

mac_settings.obj: $(COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/source/ti/ti154stack/apps/../../../../source/ti/ti154stack/low_level/cc13xx/mac_settings.c ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) -c $< @$(CONFIGPKG)/compiler.opt -o $@

macTask.obj: $(COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/source/ti/ti154stack/apps/../../../../source/ti/ti154stack/common/osal_port/osal_port_tirtos/macTask.c ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) -c $< @$(CONFIGPKG)/compiler.opt -o $@

api_mac.obj: $(COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/source/ti/ti154stack/apps/../../../../source/ti/ti154stack/common/osal_port/osal_port_tirtos/api_mac.c ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) -c $< @$(CONFIGPKG)/compiler.opt -o $@

main.obj: $(COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/source/ti/ti154stack/apps/sensor/main.c ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) -c $< @$(CONFIGPKG)/compiler.opt -o $@

sensor.obj: $(COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/source/ti/ti154stack/apps/sensor/sensor.c ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) -c $< @$(CONFIGPKG)/compiler.opt -o $@

ssf.obj: $(COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/source/ti/ti154stack/apps/sensor/ssf.c ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) -c $< @$(CONFIGPKG)/compiler.opt -o $@

CC1352P_4_LAUNCHXL_fxns.obj: ../../CC1352P_4_LAUNCHXL_fxns.c ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) -c $< @$(CONFIGPKG)/compiler.opt -o $@

ti_radio_config.obj: ti_radio_config.c ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) -c $< @$(CONFIGPKG)/compiler.opt -o $@

ti_devices_config.obj: ti_devices_config.c ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) -c $< @$(CONFIGPKG)/compiler.opt -o $@

$(NAME).out: $(OBJECTS) $(CONFIGPKG)/linker.cmd
	@ echo linking $@
	@ $(LNK) $(CFLAGS) -Wl,-u,_c_int00 $(OBJECTS) -l$(CONFIGPKG)/linker.cmd $(LFLAGS) -o $(NAME).out

clean:
	@ echo Cleaning...
	@ $(RM) $(OBJECTS) > $(DEVNULL) 2>&1
	@ $(RM) $(NAME).out > $(DEVNULL) 2>&1
	@ $(RM) $(NAME).map > $(DEVNULL) 2>&1
	@ $(RMDIR) $(NAME) > $(DEVNULL) 2>&1
	@ $(RM) ti_drivers_config.c ti_drivers_config.h ti_154stack_config.h ti_154stack_features.h ti_radio_config.c ti_radio_config.h ti_devices_config.c > $(DEVNULL) 2>&1
