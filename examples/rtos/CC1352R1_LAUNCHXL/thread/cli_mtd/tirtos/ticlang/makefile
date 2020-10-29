SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR ?= $(abspath ../../../../../../..)

include $(SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/imports.mak

XDCPATH = $(SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/source;$(SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/kernel/tirtos/packages
XDCTARGET = ti.targets.arm.clang.M4F
PLATFORM = ti.platforms.simplelink:CC1352R1F3
.PRECIOUS: $(CONFIGPKG)/compiler.opt $(CONFIGPKG)/linker.cmd

CC = "$(TICLANG_ARMCOMPILER)/bin/tiarmclang"
LNK = "$(TICLANG_ARMCOMPILER)/bin/tiarmclang"

OBJECTS = platform_alarm.obj platform_alarm_micro.obj platform_diag.obj platform_entropy.obj platform_misc.obj platform_settings.obj platform_system.obj platform_radio.obj platform_uart.obj platform_debug_uart.obj platform_crypto_aes_alt.obj platform_crypto_sha256_alt.obj platform_crypto_ecjpake_alt.obj platform_crypto_crypto_misc.obj platform_nv_crc.obj platform_nv_nvocmp.obj tiop_config.obj ti_devices_config.obj ti_radio_config.obj ti_drivers_config.obj main.obj otstack.obj otsupport_otrtosapi.obj cli.obj CC1352R1_LAUNCHXL_fxns.obj

CONFIGPKG = cli_mtd

NAME = cli_mtd

CFLAGS = -I../.. \
    -I. \
    -Oz \
    -g \
    -gdwarf-3 \
    -gstrict-dwarf \
    -Wall \
    -Wno-ti-macros \
    -DHAVE_CONFIG_H \
    -DSIMPLELINK_OPENTHREAD_SDK_BUILD=1 \
    -DSIMPLELINK_OPENTHREAD_CONFIG_CC1352=1 \
    -DBoard_EXCLUDE_NVS_EXTERNAL_FLASH \
    -DNVOCMP_POSIX_MUTEX \
    -DNVOCMP_NVPAGES=2 \
    -DNDEBUG \
    -DBOARD_DISPLAY_USE_UART=1 \
    -DMBEDTLS_CONFIG_FILE='"mbedtls-config-cc1352-ticlang.h"' \
    -I${WORKSPACE_LOC}/libmbedcrypto_CC1352R1_LAUNCHXL_tirtos_ticlang/config \
    -IOPENTHREAD_MAKE_PATH/examples/platforms \
    -IOPENTHREAD_MAKE_PATH/include \
    -IOPENTHREAD_MAKE_PATH/src/core \
    -IOPENTHREAD_MAKE_PATH/src/cli \
    -IOPENTHREAD_MAKE_PATH/third_party/mbedtls/repo/include \
    -IC:/.conan/54f80b/1/external/source \
    -I${PROJECT_ROOT}/platform/crypto \
    -I${WORKSPACE_LOC}/libopenthread_mtd_CC1352R1_LAUNCHXL_tirtos_ticlang/config \
    -DOPENTHREAD_CONFIG_FILE='"openthread-config-cc1352-ticlang-mtd.h"' \
    -DOPENTHREAD_PROJECT_CORE_CONFIG_FILE='"openthread-core-cc1352-config-ide.h"' \
    -DTIOP_ENABLE_UART=1 \
    -DDeviceFamily_CC13X2 \
    "-I$(SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/source" \
    "-I$(SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/source/ti/devices/cc13x2_cc26x2" \
    "-I$(SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/source/ti/posix/ticlang" \
    -mcpu=cortex-m4 \
    -march=armv7e-m \
    -mthumb \
    -mfloat-abi=hard \
    -mfpu=fpv4-sp-d16

LFLAGS = --reread_libs \
    -Wl,-i${WORKSPACE_LOC}/libopenthread_cli_mtd_CC1352R1_LAUNCHXL_tirtos_ticlang/OptimizeSize \
    -Wl,-llibopenthread_cli_mtd.a \
    -Wl,-i${WORKSPACE_LOC}/libopenthread_mtd_CC1352R1_LAUNCHXL_tirtos_ticlang/OptimizeSize \
    -Wl,-llibopenthread_mtd.a \
    -Wl,-i${WORKSPACE_LOC}/libopenthread_platform_utils_mtd_CC1352R1_LAUNCHXL_tirtos_ticlang/OptimizeSize \
    -Wl,-llibopenthread_platform_utils_mtd.a \
    -Wl,-i${WORKSPACE_LOC}/libmbedcrypto_CC1352R1_LAUNCHXL_tirtos_ticlang/OptimizeSize \
    -Wl,-llibmbedcrypto.a \
    -l${PROJECT_BUILD_DIR}/syscfg/ti_utils_build_linker.cmd.genlibs \
    "-L$(SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/source" \
    -LC:/.conan/54f80b/1/external/source \
    "-L$(SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/kernel/tirtos/packages" \
    -lti/devices/cc13x2_cc26x2/driverlib/bin/ticlang/driverlib.lib \
    ../../tirtos/ticlang/CC1352R1_LAUNCHXL_TIRTOS.cmd \
    "-Wl,-m,$(NAME).map" \
    -Wl,-c \
    -Wl,-w \
    "-L$(TICLANG_ARMCOMPILER)/lib" \
    -llibc.a

all: $(NAME).out

cflags.opt: makefile $(SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/imports.mak
	@ echo --compileOptions $(CFLAGS) > $@

$(CONFIGPKG)/compiler.opt: $(CONFIGPKG)/linker.cmd

$(CONFIGPKG)/linker.cmd: ../../release.cfg cflags.opt
	@ echo Running Configuro...
	@ $(XDC_INSTALL_DIR)/xs --xdcpath="$(XDCPATH)" xdc.tools.configuro -c "$(TICLANG_ARMCOMPILER)" -t $(XDCTARGET) -p $(PLATFORM) -@cflags.opt -o "$(CONFIGPKG)" ../../release.cfg

.INTERMEDIATE: syscfg
tiop_config.c ti_devices_config.c ti_radio_config.c ti_drivers_config.c tiop_config.h ti_radio_config.h ti_drivers_config.h: syscfg
	@ echo generation complete

syscfg: ../../cli_mtd.syscfg
	@ echo Generating configuration files...
	@ $(SYSCONFIG_TOOL) --compiler ticlang --product $(SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/.metadata/product.json --output $(@D) $<

platform_alarm.obj: ../../platform/alarm.c tiop_config.h ti_radio_config.h ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) -c $< @$(CONFIGPKG)/compiler.opt -o $@

platform_alarm_micro.obj: ../../platform/alarm_micro.c tiop_config.h ti_radio_config.h ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) -c $< @$(CONFIGPKG)/compiler.opt -o $@

platform_diag.obj: ../../platform/diag.c tiop_config.h ti_radio_config.h ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) -c $< @$(CONFIGPKG)/compiler.opt -o $@

platform_entropy.obj: ../../platform/entropy.c tiop_config.h ti_radio_config.h ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) -c $< @$(CONFIGPKG)/compiler.opt -o $@

platform_misc.obj: ../../platform/misc.c tiop_config.h ti_radio_config.h ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) -c $< @$(CONFIGPKG)/compiler.opt -o $@

platform_settings.obj: ../../platform/settings.c tiop_config.h ti_radio_config.h ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) -c $< @$(CONFIGPKG)/compiler.opt -o $@

platform_system.obj: ../../platform/system.c tiop_config.h ti_radio_config.h ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) -c $< @$(CONFIGPKG)/compiler.opt -o $@

platform_radio.obj: ../../platform/radio.c tiop_config.h ti_radio_config.h ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) -c $< @$(CONFIGPKG)/compiler.opt -o $@

platform_uart.obj: ../../platform/uart.c tiop_config.h ti_radio_config.h ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) -c $< @$(CONFIGPKG)/compiler.opt -o $@

platform_debug_uart.obj: ../../platform/debug_uart.c tiop_config.h ti_radio_config.h ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) -c $< @$(CONFIGPKG)/compiler.opt -o $@

platform_crypto_aes_alt.obj: ../../platform/crypto/aes_alt.c tiop_config.h ti_radio_config.h ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) -c $< @$(CONFIGPKG)/compiler.opt -o $@

platform_crypto_sha256_alt.obj: ../../platform/crypto/sha256_alt.c tiop_config.h ti_radio_config.h ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) -c $< @$(CONFIGPKG)/compiler.opt -o $@

platform_crypto_ecjpake_alt.obj: ../../platform/crypto/ecjpake_alt.c tiop_config.h ti_radio_config.h ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) -c $< @$(CONFIGPKG)/compiler.opt -o $@

platform_crypto_crypto_misc.obj: ../../platform/crypto/crypto_misc.c tiop_config.h ti_radio_config.h ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) -c $< @$(CONFIGPKG)/compiler.opt -o $@

platform_nv_crc.obj: ../../platform/nv/crc.c tiop_config.h ti_radio_config.h ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) -c $< @$(CONFIGPKG)/compiler.opt -o $@

platform_nv_nvocmp.obj: ../../platform/nv/nvocmp.c tiop_config.h ti_radio_config.h ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) -c $< @$(CONFIGPKG)/compiler.opt -o $@

main.obj: ../../main.c tiop_config.h ti_radio_config.h ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) -c $< @$(CONFIGPKG)/compiler.opt -o $@

otstack.obj: ../../otstack.c tiop_config.h ti_radio_config.h ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) -c $< @$(CONFIGPKG)/compiler.opt -o $@

otsupport_otrtosapi.obj: ../../otsupport/otrtosapi.c tiop_config.h ti_radio_config.h ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) -c $< @$(CONFIGPKG)/compiler.opt -o $@

cli.obj: ../../cli.c tiop_config.h ti_radio_config.h ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) -c $< @$(CONFIGPKG)/compiler.opt -o $@

CC1352R1_LAUNCHXL_fxns.obj: ../../CC1352R1_LAUNCHXL_fxns.c tiop_config.h ti_radio_config.h ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) -c $< @$(CONFIGPKG)/compiler.opt -o $@

tiop_config.obj: tiop_config.c tiop_config.h ti_radio_config.h ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) -c $< @$(CONFIGPKG)/compiler.opt -o $@

ti_devices_config.obj: ti_devices_config.c tiop_config.h ti_radio_config.h ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) -c $< @$(CONFIGPKG)/compiler.opt -o $@

ti_radio_config.obj: ti_radio_config.c tiop_config.h ti_radio_config.h ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) -c $< @$(CONFIGPKG)/compiler.opt -o $@

ti_drivers_config.obj: ti_drivers_config.c tiop_config.h ti_radio_config.h ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) -c $< @$(CONFIGPKG)/compiler.opt -o $@

$(NAME).out: $(OBJECTS) $(CONFIGPKG)/linker.cmd
	@ echo linking $@
	@ $(LNK) -Wl,-u,_c_int00 $(OBJECTS) -l$(CONFIGPKG)/linker.cmd $(LFLAGS) -o $(NAME).out

clean:
	@ echo Cleaning...
	@ $(RM) $(OBJECTS) > $(DEVNULL) 2>&1
	@ $(RM) $(NAME).out > $(DEVNULL) 2>&1
	@ $(RM) $(NAME).map > $(DEVNULL) 2>&1
	@ $(RMDIR) $(NAME) > $(DEVNULL) 2>&1
	@ $(RM) cflags.opt > $(DEVNULL) 2>&1
	@ $(RM) tiop_config.c ti_devices_config.c ti_radio_config.c ti_drivers_config.c tiop_config.h ti_radio_config.h ti_drivers_config.h > $(DEVNULL) 2>&1
