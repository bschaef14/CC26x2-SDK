PROJECT_LOC = $(abspath ../../)
PROJECT_ROOT = $(PROJECT_LOC)
COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR ?= $(abspath ../../../../../../..)

include $(COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/imports.mak

XDCPATH = $(COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/source;$(COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/kernel/tirtos/packages;
SYSCONFIG_PRODUCT := $(COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/.metadata/product.json

CC = "$(TICLANG_ARMCOMPILER)/bin/tiarmclang"
LNK = "$(TICLANG_ARMCOMPILER)/bin/tiarmclang"

XDCTARGET = ti.targets.arm.clang.M4F
PLATFORM = ti.platforms.simplelink:CC2652R1F

OBJECTS = ti_drivers_config.obj ti_devices_config.obj ti_radio_config.obj mac_user_config.obj mt.obj mt_ext.obj mt_mac.obj mt_sys.obj mt_util.obj board_led.obj crc.obj nvocmp.obj  mac_cfg.obj mac_settings.obj macTask.obj api_mac.obj npi_frame_mt.obj npi_rxbuf.obj npi_task.obj npi_tl.obj npi_tl_uart.obj main.obj mcp.obj CC26X2R1_LAUNCHXL_fxns.obj

CONFIGPKG = coprocessor

NAME = coprocessor

CFLAGS = -I../.. \
    -I. \
    "@${PROJECT_LOC}/application/defines/coprocessor.opts" \
    "-I${PROJECT_ROOT}/application/coprocessor/mt" \
    "-I${PROJECT_ROOT}/application/coprocessor" \
    "-I${PROJECT_ROOT}/application/2_4g" \
    "-I${PROJECT_ROOT}/software_stack/ti15_4stack/mac/mac_settings" \
    "-I${PROJECT_ROOT}/software_stack/ti15_4stack/radio_configuration/radio_config" \
    -Oz \
    -std=gnu9x \
    -munaligned-access \
    -fshort-enums \
    -fcommon \
    -funsigned-char \
    -DTIMAC_ROM_IMAGE_BUILD \
    "-I${PROJECT_ROOT}" \
    "-I${PROJECT_ROOT}/application" \
    "-I${PROJECT_ROOT}/application/launch_pad" \
    "-I${PROJECT_ROOT}/software_stack/ti15_4stack/osal" \
    "-I${COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR}/source/ti/ti154stack/apps" \
    "-I${COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR}/source/ti/ti154stack/common" \
    "-I${COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR}/source/ti/ti154stack/common/boards" \
    "-I${COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR}/source/ti/ti154stack/common/osal_port" \
    "-I${COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR}/source/ti/ti154stack/npi/src" \
    "-I${COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR}/source/ti/ti154stack/npi/src/inc" \
    "-I${COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR}/source/ti/ti154stack/common/util" \
    "-I${COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR}/source/ti/ti154stack/common/inc" \
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
    "-I${COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR}/source/ti/ti154stack/apps/coprocessor" \
    "-I${COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR}/source/ti/ti154stack/apps/coprocessor/mt" \
    "-I${COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR}/source/ti/devices/cc13x2_cc26x2" \
    "-I${COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR}/source/ti/devices/cc13x2_cc26x2/inc" \
    "-I${COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR}/source/ti/devices/cc13x2_cc26x2/driverlib" \
    "-I${COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR}/source/ti/common/nv" \
    -DBoard_EXCLUDE_NVS_EXTERNAL_FLASH \
    -DDeviceFamily_CC26X2 \
    "-I$(COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/source" \
    "-I$(COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/source/ti/posix/ticlang" \
    -gdwarf-3 \
    -mcpu=cortex-m4 \
    -march=armv7e-m \
    -mthumb \
    -mfloat-abi=hard \
    -mfpu=fpv4-sp-d16

LFLAGS = "-l${COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR}/source/ti/ti154stack/library/tirtos/ticlang/bin/maclib_secure_cc26x2_2_4g.a" \
    "-l${COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR}/source/ti/ti154stack/library/tirtos/ticlang/bin/maclib_osal_tirtos_cc13x2_26x2.a" \
    "-l${COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR}/source/ti/ti154stack/library/tirtos/ticlang/timac_rom_PG2_0_rom_api_linker.cmd" \
    --define=NVOCMP_NVPAGES=4 \
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
    ../../cc26x2lp.cmd \
    "-Wl,-m,$(NAME).map" \
    -Wl,-c \
    "-Wl,-i$(TICLANG_ARMCOMPILER)/lib" \
    -llibc.a

.PRECIOUS: $(CONFIGPKG)/compiler.opt $(CONFIGPKG)/linker.cmd

all: $(NAME).out

$(CONFIGPKG)/compiler.opt: $(CONFIGPKG)/linker.cmd

$(CONFIGPKG)/linker.cmd: $(COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/source/ti/ti154stack/apps/../../../../source/ti/ti154stack/common/boards/CC26X2R1_LAUNCHXL/app_cop.cfg
	@ echo Running Configuro...
	@ $(XDC_INSTALL_DIR)/xs --xdcpath="$(XDCPATH)" xdc.tools.configuro -c "$(TICLANG_ARMCOMPILER)" -t $(XDCTARGET) -p $(PLATFORM) --compileOptions "$(CFLAGS)" -o "$(CONFIGPKG)" $(COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/source/ti/ti154stack/apps/../../../../source/ti/ti154stack/common/boards/CC26X2R1_LAUNCHXL/app_cop.cfg

.INTERMEDIATE: syscfg
ti_drivers_config.c ti_drivers_config.h ti_devices_config.c: syscfg
	@ echo generation complete

syscfg: ../../coprocessor.syscfg
	@ echo Generating configuration files...
	@ $(SYSCONFIG_TOOL) --product $(SYSCONFIG_PRODUCT) --compiler ticlang --output $(@D) $<

ti_drivers_config.obj: ti_drivers_config.c $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) -c $< @$(CONFIGPKG)/compiler.opt -o $@

ti_radio_config.obj: ../../software_stack/ti15_4stack/radio_configuration/radio_config/ti_radio_config.c ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) -c $< @$(CONFIGPKG)/compiler.opt -o $@

mac_user_config.obj: $(COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/source/ti/ti154stack/apps/../../../../source/ti/ti154stack/common/boards/mac_user_config.c ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) -c $< @$(CONFIGPKG)/compiler.opt -o $@

mt.obj: $(COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/source/ti/ti154stack/apps/coprocessor/mt/mt.c ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) -c $< @$(CONFIGPKG)/compiler.opt -o $@

mt_ext.obj: $(COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/source/ti/ti154stack/apps/coprocessor/mt/mt_ext.c ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) -c $< @$(CONFIGPKG)/compiler.opt -o $@

mt_mac.obj: $(COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/source/ti/ti154stack/apps/coprocessor/mt/mt_mac.c ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) -c $< @$(CONFIGPKG)/compiler.opt -o $@

mt_sys.obj: $(COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/source/ti/ti154stack/apps/coprocessor/mt/mt_sys.c ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) -c $< @$(CONFIGPKG)/compiler.opt -o $@

mt_util.obj: $(COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/source/ti/ti154stack/apps/coprocessor/mt/mt_util.c ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) -c $< @$(CONFIGPKG)/compiler.opt -o $@

board_led.obj: $(COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/source/ti/ti154stack/apps/../../../../source/ti/ti154stack/common/util/launchpad/board_led.c ti_drivers_config.h $(CONFIGPKG)/compiler.opt
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

npi_frame_mt.obj: $(COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/source/ti/ti154stack/apps/../../../../source/ti/ti154stack/npi/src/npi_frame_mt.c ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) -c $< @$(CONFIGPKG)/compiler.opt -o $@

npi_rxbuf.obj: $(COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/source/ti/ti154stack/apps/../../../../source/ti/ti154stack/npi/src/npi_rxbuf.c ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) -c $< @$(CONFIGPKG)/compiler.opt -o $@

npi_task.obj: $(COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/source/ti/ti154stack/apps/../../../../source/ti/ti154stack/npi/src/npi_task.c ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) -c $< @$(CONFIGPKG)/compiler.opt -o $@

npi_tl.obj: $(COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/source/ti/ti154stack/apps/../../../../source/ti/ti154stack/npi/src/npi_tl.c ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) -c $< @$(CONFIGPKG)/compiler.opt -o $@

npi_tl_uart.obj: $(COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/source/ti/ti154stack/apps/../../../../source/ti/ti154stack/npi/src/npi_tl_uart.c ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) -c $< @$(CONFIGPKG)/compiler.opt -o $@

main.obj: $(COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/source/ti/ti154stack/apps/coprocessor/main.c ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) -c $< @$(CONFIGPKG)/compiler.opt -o $@

mcp.obj: $(COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/source/ti/ti154stack/apps/coprocessor/mcp.c ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) -c $< @$(CONFIGPKG)/compiler.opt -o $@

CC26X2R1_LAUNCHXL_fxns.obj: ../../CC26X2R1_LAUNCHXL_fxns.c ti_drivers_config.h $(CONFIGPKG)/compiler.opt
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
	@ $(RM) ti_drivers_config.c ti_drivers_config.h ti_devices_config.c > $(DEVNULL) 2>&1
