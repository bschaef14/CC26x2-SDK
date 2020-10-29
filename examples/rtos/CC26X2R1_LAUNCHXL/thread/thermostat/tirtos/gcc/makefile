SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR ?= $(abspath ../../../../../../..)

include $(SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/imports.mak

XDCPATH = $(SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/source;$(SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/kernel/tirtos/packages
XDCTARGET = gnu.targets.arm.M4F
PLATFORM = ti.platforms.simplelink:CC2652R1F
.PRECIOUS: $(CONFIGPKG)/compiler.opt $(CONFIGPKG)/linker.cmd

CC = "$(GCC_ARMCOMPILER)/bin/arm-none-eabi-gcc"
LNK = "$(GCC_ARMCOMPILER)/bin/arm-none-eabi-gcc"

OBJECTS = platform_alarm.obj platform_alarm_micro.obj platform_diag.obj platform_entropy.obj platform_misc.obj platform_settings.obj platform_system.obj platform_radio.obj platform_uart.obj platform_debug_uart.obj platform_crypto_aes_alt.obj platform_crypto_sha256_alt.obj platform_crypto_ecjpake_alt.obj platform_crypto_crypto_misc.obj platform_nv_crc.obj platform_nv_nvocmp.obj tiop_config.obj ti_devices_config.obj ti_radio_config.obj ti_drivers_config.obj main.obj otstack.obj otsupport_otrtosapi.obj thermostat.obj images.obj tiop_ui.obj cui.obj graphicext.obj disp_utils.obj CC26X2R1_LAUNCHXL_fxns.obj missing_cxxhelpers.obj

CONFIGPKG = thermostat

NAME = thermostat

CFLAGS = -I../.. \
    -I. \
    -fdata-sections \
    -ffunction-sections \
    -Os \
    -g \
    -std=c99 \
    -fno-exceptions \
    -fno-rtti \
    -fno-common \
    -DHAVE_CONFIG_H \
    -DSIMPLELINK_OPENTHREAD_SDK_BUILD=1 \
    -DSIMPLELINK_OPENTHREAD_CONFIG_CC2652=1 \
    -DBoard_EXCLUDE_NVS_EXTERNAL_FLASH \
    -DNVOCMP_POSIX_MUTEX \
    -DNVOCMP_NVPAGES=2 \
    -DNDEBUG \
    -DBOARD_DISPLAY_USE_UART=1 \
    -DMBEDTLS_CONFIG_FILE='"mbedtls-config-cc2652-gcc.h"' \
    -I${WORKSPACE_LOC}/libmbedcrypto_CC26X2R1_LAUNCHXL_tirtos_gcc/config \
    -IOPENTHREAD_MAKE_PATH/examples/platforms \
    -IOPENTHREAD_MAKE_PATH/include \
    -IOPENTHREAD_MAKE_PATH/src/core \
    -IOPENTHREAD_MAKE_PATH/third_party/mbedtls/repo/include \
    -IC:/.conan/54f80b/1/external/source \
    -I${PROJECT_ROOT}/platform/crypto \
    -I${WORKSPACE_LOC}/libopenthread_ftd_CC26X2R1_LAUNCHXL_tirtos_gcc/config \
    -DOPENTHREAD_CONFIG_FILE='"openthread-config-cc2652-gcc-ftd.h"' \
    -DOPENTHREAD_PROJECT_CORE_CONFIG_FILE='"openthread-core-cc2652-config-ide.h"' \
    -DTIOP_CUI=1 \
    -DBOARD_DISPLAY_USE_LCD=1 \
    -DDeviceFamily_CC26X2 \
    "-I$(SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/source" \
    "-I$(SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/source/ti/devices/cc13x2_cc26x2" \
    "-I$(SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/source/ti/posix/gcc" \
    -mcpu=cortex-m4 \
    -march=armv7e-m \
    -mthumb \
    -mfloat-abi=hard \
    -mfpu=fpv4-sp-d16 \
    -gstrict-dwarf \
    -Wall \
    "-I$(SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/kernel/tirtos/packages/gnu/targets/arm/libs/install-native/arm-none-eabi/include/newlib-nano" \
    "-I$(SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/kernel/tirtos/packages/gnu/targets/arm/libs/install-native/arm-none-eabi/include" \
    "-I$(GCC_ARMCOMPILER)/arm-none-eabi/include"

LFLAGS = -nostartfiles \
    -Wl,--gc-sections \
    --defsym=NVOCMP_NVPAGES=2 \
    -L${WORKSPACE_LOC}/libopenthread_ftd_CC26X2R1_LAUNCHXL_tirtos_gcc/OptimizeSize \
    -lopenthread_ftd \
    -L${WORKSPACE_LOC}/libopenthread_platform_utils_ftd_CC26X2R1_LAUNCHXL_tirtos_gcc/OptimizeSize \
    -lopenthread_platform_utils_ftd \
    -L${WORKSPACE_LOC}/libmbedcrypto_CC26X2R1_LAUNCHXL_tirtos_gcc/OptimizeSize \
    -lmbedcrypto \
    -Wl,-T,../../tirtos/gcc/CC26X2R1_LAUNCHXL_TIRTOS.lds \
    "-Wl,-Map,$(NAME).map" \
     -l${PROJECT_BUILD_DIR}/syscfg/ti_utils_build_linker.cmd.genlibs \
    "-L$(SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/source" \
    -LC:/.conan/54f80b/1/external/source \
    "-L$(SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/kernel/tirtos/packages" \
    "-Wl,-T,$(NAME)/linker.cmd" \
    -l:ti/devices/cc13x2_cc26x2/driverlib/bin/gcc/driverlib.lib \
    -march=armv7e-m \
    -mthumb \
    -mfloat-abi=hard \
    -mfpu=fpv4-sp-d16 \
    -static \
    "-L$(SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/kernel/tirtos/packages/gnu/targets/arm/libs/install-native/arm-none-eabi/lib/thumb/v7e-m/hard" \
    -lgcc \
    -lc \
    -lm \
    -lnosys \
    --specs=nano.specs

all: $(NAME).out

cflags.opt: makefile $(SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/imports.mak
	@ echo --compileOptions $(CFLAGS) > $@

$(CONFIGPKG)/compiler.opt: $(CONFIGPKG)/linker.cmd

$(CONFIGPKG)/linker.cmd: ../../release.cfg cflags.opt
	@ echo Running Configuro...
	@ $(XDC_INSTALL_DIR)/xs --xdcpath="$(XDCPATH)" xdc.tools.configuro -c "$(GCC_ARMCOMPILER)" -t $(XDCTARGET) -p $(PLATFORM) -@cflags.opt -o "$(CONFIGPKG)" ../../release.cfg

.INTERMEDIATE: syscfg
tiop_config.c ti_devices_config.c ti_radio_config.c ti_drivers_config.c tiop_config.h ti_radio_config.h ti_drivers_config.h: syscfg
	@ echo generation complete

syscfg: ../../thermostat.syscfg
	@ echo Generating configuration files...
	@ $(SYSCONFIG_TOOL) --compiler gcc --product $(SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/.metadata/product.json --output $(@D) $<

platform_alarm.obj: ../../platform/alarm.c tiop_config.h ti_radio_config.h ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -c @$(CONFIGPKG)/compiler.opt -o $@

platform_alarm_micro.obj: ../../platform/alarm_micro.c tiop_config.h ti_radio_config.h ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -c @$(CONFIGPKG)/compiler.opt -o $@

platform_diag.obj: ../../platform/diag.c tiop_config.h ti_radio_config.h ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -c @$(CONFIGPKG)/compiler.opt -o $@

platform_entropy.obj: ../../platform/entropy.c tiop_config.h ti_radio_config.h ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -c @$(CONFIGPKG)/compiler.opt -o $@

platform_misc.obj: ../../platform/misc.c tiop_config.h ti_radio_config.h ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -c @$(CONFIGPKG)/compiler.opt -o $@

platform_settings.obj: ../../platform/settings.c tiop_config.h ti_radio_config.h ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -c @$(CONFIGPKG)/compiler.opt -o $@

platform_system.obj: ../../platform/system.c tiop_config.h ti_radio_config.h ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -c @$(CONFIGPKG)/compiler.opt -o $@

platform_radio.obj: ../../platform/radio.c tiop_config.h ti_radio_config.h ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -c @$(CONFIGPKG)/compiler.opt -o $@

platform_uart.obj: ../../platform/uart.c tiop_config.h ti_radio_config.h ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -c @$(CONFIGPKG)/compiler.opt -o $@

platform_debug_uart.obj: ../../platform/debug_uart.c tiop_config.h ti_radio_config.h ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -c @$(CONFIGPKG)/compiler.opt -o $@

platform_crypto_aes_alt.obj: ../../platform/crypto/aes_alt.c tiop_config.h ti_radio_config.h ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -c @$(CONFIGPKG)/compiler.opt -o $@

platform_crypto_sha256_alt.obj: ../../platform/crypto/sha256_alt.c tiop_config.h ti_radio_config.h ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -c @$(CONFIGPKG)/compiler.opt -o $@

platform_crypto_ecjpake_alt.obj: ../../platform/crypto/ecjpake_alt.c tiop_config.h ti_radio_config.h ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -c @$(CONFIGPKG)/compiler.opt -o $@

platform_crypto_crypto_misc.obj: ../../platform/crypto/crypto_misc.c tiop_config.h ti_radio_config.h ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -c @$(CONFIGPKG)/compiler.opt -o $@

platform_nv_crc.obj: ../../platform/nv/crc.c tiop_config.h ti_radio_config.h ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -c @$(CONFIGPKG)/compiler.opt -o $@

platform_nv_nvocmp.obj: ../../platform/nv/nvocmp.c tiop_config.h ti_radio_config.h ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -c @$(CONFIGPKG)/compiler.opt -o $@

main.obj: ../../main.c tiop_config.h ti_radio_config.h ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -c @$(CONFIGPKG)/compiler.opt -o $@

otstack.obj: ../../otstack.c tiop_config.h ti_radio_config.h ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -c @$(CONFIGPKG)/compiler.opt -o $@

otsupport_otrtosapi.obj: ../../otsupport/otrtosapi.c tiop_config.h ti_radio_config.h ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -c @$(CONFIGPKG)/compiler.opt -o $@

thermostat.obj: ../../thermostat.c tiop_config.h ti_radio_config.h ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -c @$(CONFIGPKG)/compiler.opt -o $@

images.obj: ../../images.c tiop_config.h ti_radio_config.h ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -c @$(CONFIGPKG)/compiler.opt -o $@

tiop_ui.obj: ../../tiop_ui.c tiop_config.h ti_radio_config.h ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -c @$(CONFIGPKG)/compiler.opt -o $@

cui.obj: ../../cui.c tiop_config.h ti_radio_config.h ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -c @$(CONFIGPKG)/compiler.opt -o $@

graphicext.obj: ../../graphicext.c tiop_config.h ti_radio_config.h ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -c @$(CONFIGPKG)/compiler.opt -o $@

disp_utils.obj: ../../disp_utils.c tiop_config.h ti_radio_config.h ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -c @$(CONFIGPKG)/compiler.opt -o $@

CC26X2R1_LAUNCHXL_fxns.obj: ../../CC26X2R1_LAUNCHXL_fxns.c tiop_config.h ti_radio_config.h ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -c @$(CONFIGPKG)/compiler.opt -o $@

missing_cxxhelpers.obj: ../../missing/cxxhelpers.c tiop_config.h ti_radio_config.h ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -c @$(CONFIGPKG)/compiler.opt -o $@

tiop_config.obj: tiop_config.c tiop_config.h ti_radio_config.h ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -c @$(CONFIGPKG)/compiler.opt -o $@

ti_devices_config.obj: ti_devices_config.c tiop_config.h ti_radio_config.h ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -c @$(CONFIGPKG)/compiler.opt -o $@

ti_radio_config.obj: ti_radio_config.c tiop_config.h ti_radio_config.h ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -c @$(CONFIGPKG)/compiler.opt -o $@

ti_drivers_config.obj: ti_drivers_config.c tiop_config.h ti_radio_config.h ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -c @$(CONFIGPKG)/compiler.opt -o $@

$(NAME).out: $(OBJECTS) $(CONFIGPKG)/linker.cmd
	@ echo linking $@
	@ $(LNK)  $(OBJECTS)  $(LFLAGS) -o $(NAME).out

clean:
	@ echo Cleaning...
	@ $(RM) $(OBJECTS) > $(DEVNULL) 2>&1
	@ $(RM) $(NAME).out > $(DEVNULL) 2>&1
	@ $(RM) $(NAME).map > $(DEVNULL) 2>&1
	@ $(RMDIR) $(NAME) > $(DEVNULL) 2>&1
	@ $(RM) cflags.opt > $(DEVNULL) 2>&1
	@ $(RM) tiop_config.c ti_devices_config.c ti_radio_config.c ti_drivers_config.c tiop_config.h ti_radio_config.h ti_drivers_config.h > $(DEVNULL) 2>&1
