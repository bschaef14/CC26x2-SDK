SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR ?= $(abspath ../../../../../../..)

include $(SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/imports.mak

XDCPATH = $(SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/source;$(SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/kernel/tirtos/packages
XDCTARGET = iar.targets.arm.M4F
PLATFORM = ti.platforms.simplelink:CC2652R1F
.PRECIOUS: $(CONFIGPKG)/compiler.opt $(CONFIGPKG)/linker.cmd

CC = "$(IAR_ARMCOMPILER)/bin/iccarm"
LNK = "$(IAR_ARMCOMPILER)/bin/ilinkarm"

OBJECTS = platform_alarm.obj platform_alarm_micro.obj platform_diag.obj platform_entropy.obj platform_misc.obj platform_settings.obj platform_system.obj platform_radio.obj platform_uart.obj platform_debug_uart.obj platform_crypto_aes_alt.obj platform_crypto_sha256_alt.obj platform_crypto_ecjpake_alt.obj platform_crypto_crypto_misc.obj platform_nv_crc.obj platform_nv_nvocmp.obj tiop_config.obj ti_devices_config.obj ti_radio_config.obj ti_drivers_config.obj main.obj otstack.obj otsupport_otrtosapi.obj empty.obj tiop_ui.obj cui.obj disp_utils.obj CC26X2R1_LAUNCHXL_fxns.obj

CONFIGPKG = empty_mtd

NAME = empty_mtd

CFLAGS = -I../.. \
    -I. \
    -DHAVE_CONFIG_H \
    -DSIMPLELINK_OPENTHREAD_SDK_BUILD=1 \
    -DSIMPLELINK_OPENTHREAD_CONFIG_CC2652=1 \
    -DBoard_EXCLUDE_NVS_EXTERNAL_FLASH \
    -DNVOCMP_POSIX_MUTEX \
    -DNVOCMP_NVPAGES=2 \
    -DNDEBUG \
    -DBOARD_DISPLAY_USE_UART=1 \
    -DMBEDTLS_CONFIG_FILE="mbedtls-config-cc2652-iar.h" \
    -I$COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR$/source/ti/thread/rtos/LP_CC2652RSIP/thread/libmbedcrypto/tirtos/iar/config \
    -I$COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR$/source/third_party/openthread/examples/platforms \
    -I$COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR$/source/third_party/openthread/include \
    -I$COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR$/source/third_party/openthread/src/core \
    -I$COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR$/source/third_party/openthread/third_party/mbedtls/repo/include \
    -I$SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR$/kernel/tirtos/packages \
    -I$COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR$/source \
    -I$XDCROOT$/packages \
    -I$COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR$/source/ti/thread/rtos/LP_CC2652RSIP/thread/libopenthread_mtd/tirtos/iar/config \
    -DOPENTHREAD_CONFIG_FILE="openthread-config-cc2652-iar-mtd.h" \
    -DOPENTHREAD_PROJECT_CORE_CONFIG_FILE="openthread-core-cc2652-config-ide.h" \
    -DTIOP_CUI=1 \
    -DDeviceFamily_CC26X2 \
    "-I$(SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/source" \
    "-I$(SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/source/ti/devices/cc13x2_cc26x2" \
    "-I$(SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/source/ti/posix/iar" \
    --debug \
    --silent \
    -e \
    --aeabi \
    --thumb \
    --diag_suppress=Pa050

LFLAGS = $COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR$/source/ti/thread/rtos/LP_CC2652RSIP/thread/libopenthread_mtd/tirtos/iar/OptimizeSize/libopenthread_mtd.a  \
    $COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR$/source/ti/thread/rtos/LP_CC2652RSIP/thread/libopenthread_platform_utils_mtd/tirtos/iar/OptimizeSize/libopenthread_platform_utils_mtd.a  \
    $COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR$/source/ti/thread/rtos/LP_CC2652RSIP/thread/libmbedcrypto/tirtos/iar/OptimizeSize/libmbedcrypto.a  \
    " -f$PROJ_DIR$/ti_utils_build_linker.cmd.genlibs -L$(SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/source" \
    " -L$(SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/kernel/tirtos/packages" \
    "$(SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/source/ti/devices/cc13x2_cc26x2/driverlib/bin/iar/driverlib.lib" \
    --config ../../tirtos/iar/CC26X2R1_LAUNCHXL_TIRTOS.icf \
    --map "$(NAME).map" \
    --silent \
    --cpu=Cortex-M4F \
    --semihosting=iar_breakpoint

all: $(NAME).out

cflags.opt: makefile $(SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/imports.mak
	@ echo --compileOptions $(CFLAGS) > $@

$(CONFIGPKG)/compiler.opt: $(CONFIGPKG)/linker.cmd

$(CONFIGPKG)/linker.cmd: ../../release.cfg cflags.opt
	@ echo Running Configuro...
	@ $(XDC_INSTALL_DIR)/xs --xdcpath="$(XDCPATH)" xdc.tools.configuro -c "$(IAR_ARMCOMPILER)" -t $(XDCTARGET) -p $(PLATFORM) -@cflags.opt -o "$(CONFIGPKG)" ../../release.cfg

.INTERMEDIATE: syscfg
tiop_config.c ti_devices_config.c ti_radio_config.c ti_drivers_config.c tiop_config.h ti_radio_config.h ti_drivers_config.h: syscfg
	@ echo generation complete

syscfg: ../../empty_mtd.syscfg
	@ echo Generating configuration files...
	@ $(SYSCONFIG_TOOL) --compiler iar --product $(SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/.metadata/product.json --output $(@D) $<

platform_alarm.obj: ../../platform/alarm.c tiop_config.h ti_radio_config.h ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -f $(CONFIGPKG)/compiler.opt -o $@

platform_alarm_micro.obj: ../../platform/alarm_micro.c tiop_config.h ti_radio_config.h ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -f $(CONFIGPKG)/compiler.opt -o $@

platform_diag.obj: ../../platform/diag.c tiop_config.h ti_radio_config.h ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -f $(CONFIGPKG)/compiler.opt -o $@

platform_entropy.obj: ../../platform/entropy.c tiop_config.h ti_radio_config.h ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -f $(CONFIGPKG)/compiler.opt -o $@

platform_misc.obj: ../../platform/misc.c tiop_config.h ti_radio_config.h ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -f $(CONFIGPKG)/compiler.opt -o $@

platform_settings.obj: ../../platform/settings.c tiop_config.h ti_radio_config.h ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -f $(CONFIGPKG)/compiler.opt -o $@

platform_system.obj: ../../platform/system.c tiop_config.h ti_radio_config.h ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -f $(CONFIGPKG)/compiler.opt -o $@

platform_radio.obj: ../../platform/radio.c tiop_config.h ti_radio_config.h ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -f $(CONFIGPKG)/compiler.opt -o $@

platform_uart.obj: ../../platform/uart.c tiop_config.h ti_radio_config.h ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -f $(CONFIGPKG)/compiler.opt -o $@

platform_debug_uart.obj: ../../platform/debug_uart.c tiop_config.h ti_radio_config.h ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -f $(CONFIGPKG)/compiler.opt -o $@

platform_crypto_aes_alt.obj: ../../platform/crypto/aes_alt.c tiop_config.h ti_radio_config.h ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -f $(CONFIGPKG)/compiler.opt -o $@

platform_crypto_sha256_alt.obj: ../../platform/crypto/sha256_alt.c tiop_config.h ti_radio_config.h ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -f $(CONFIGPKG)/compiler.opt -o $@

platform_crypto_ecjpake_alt.obj: ../../platform/crypto/ecjpake_alt.c tiop_config.h ti_radio_config.h ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -f $(CONFIGPKG)/compiler.opt -o $@

platform_crypto_crypto_misc.obj: ../../platform/crypto/crypto_misc.c tiop_config.h ti_radio_config.h ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -f $(CONFIGPKG)/compiler.opt -o $@

platform_nv_crc.obj: ../../platform/nv/crc.c tiop_config.h ti_radio_config.h ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -f $(CONFIGPKG)/compiler.opt -o $@

platform_nv_nvocmp.obj: ../../platform/nv/nvocmp.c tiop_config.h ti_radio_config.h ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -f $(CONFIGPKG)/compiler.opt -o $@

main.obj: ../../main.c tiop_config.h ti_radio_config.h ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -f $(CONFIGPKG)/compiler.opt -o $@

otstack.obj: ../../otstack.c tiop_config.h ti_radio_config.h ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -f $(CONFIGPKG)/compiler.opt -o $@

otsupport_otrtosapi.obj: ../../otsupport/otrtosapi.c tiop_config.h ti_radio_config.h ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -f $(CONFIGPKG)/compiler.opt -o $@

empty.obj: ../../empty.c tiop_config.h ti_radio_config.h ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -f $(CONFIGPKG)/compiler.opt -o $@

tiop_ui.obj: ../../tiop_ui.c tiop_config.h ti_radio_config.h ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -f $(CONFIGPKG)/compiler.opt -o $@

cui.obj: ../../cui.c tiop_config.h ti_radio_config.h ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -f $(CONFIGPKG)/compiler.opt -o $@

disp_utils.obj: ../../disp_utils.c tiop_config.h ti_radio_config.h ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -f $(CONFIGPKG)/compiler.opt -o $@

CC26X2R1_LAUNCHXL_fxns.obj: ../../CC26X2R1_LAUNCHXL_fxns.c tiop_config.h ti_radio_config.h ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -f $(CONFIGPKG)/compiler.opt -o $@

tiop_config.obj: tiop_config.c tiop_config.h ti_radio_config.h ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -f $(CONFIGPKG)/compiler.opt -o $@

ti_devices_config.obj: ti_devices_config.c tiop_config.h ti_radio_config.h ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -f $(CONFIGPKG)/compiler.opt -o $@

ti_radio_config.obj: ti_radio_config.c tiop_config.h ti_radio_config.h ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -f $(CONFIGPKG)/compiler.opt -o $@

ti_drivers_config.obj: ti_drivers_config.c tiop_config.h ti_radio_config.h ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -f $(CONFIGPKG)/compiler.opt -o $@

$(NAME).out: $(OBJECTS) $(CONFIGPKG)/linker.cmd
	@ echo linking $@
	@ $(LNK)  $(OBJECTS) -f $(CONFIGPKG)/linker.cmd $(LFLAGS) -o $(NAME).out

clean:
	@ echo Cleaning...
	@ $(RM) $(OBJECTS) > $(DEVNULL) 2>&1
	@ $(RM) $(NAME).out > $(DEVNULL) 2>&1
	@ $(RM) $(NAME).map > $(DEVNULL) 2>&1
	@ $(RMDIR) $(NAME) > $(DEVNULL) 2>&1
	@ $(RM) cflags.opt > $(DEVNULL) 2>&1
	@ $(RM) tiop_config.c ti_devices_config.c ti_radio_config.c ti_drivers_config.c tiop_config.h ti_radio_config.h ti_drivers_config.h > $(DEVNULL) 2>&1
