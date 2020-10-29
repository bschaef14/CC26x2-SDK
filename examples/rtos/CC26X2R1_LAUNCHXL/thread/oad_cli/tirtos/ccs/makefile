SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR ?= $(abspath ../../../../../../..)

include $(SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/imports.mak

XDCPATH = $(SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/source;$(SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/kernel/tirtos/packages
XDCTARGET = ti.targets.arm.elf.M4F
PLATFORM = ti.platforms.simplelink:CC2652R1F
.PRECIOUS: $(CONFIGPKG)/compiler.opt $(CONFIGPKG)/linker.cmd

CC = "$(CCS_ARMCOMPILER)/bin/armcl"
LNK = "$(CCS_ARMCOMPILER)/bin/armcl"

OBJECTS = platform_alarm.obj platform_alarm_micro.obj platform_diag.obj platform_entropy.obj platform_misc.obj platform_settings.obj platform_system.obj platform_radio.obj platform_uart.obj platform_debug_uart.obj platform_crypto_aes_alt.obj platform_crypto_sha256_alt.obj platform_crypto_ecjpake_alt.obj platform_crypto_crypto_misc.obj platform_nv_crc.obj platform_nv_nvocmp.obj tiop_config.obj ti_radio_config.obj ti_drivers_config.obj oad_image_header.obj main.obj otstack.obj otsupport_otrtosapi.obj cli.obj CC26X2R1_LAUNCHXL_fxns.obj bim_oad_debug_image_bin.obj

CONFIGPKG = oad_cli

NAME = oad_cli

CFLAGS = -I../.. \
    -I. \
    -O4 \
    -mv7M3 \
    --opt_for_speed=0 \
    --abi=eabi \
    -me \
    -g \
    --c99 \
    --gcc \
    --printf_support=nofloat \
    --gen_func_subsections=on \
    --gen_data_subsections=on \
    -DHAVE_CONFIG_H \
    -DSIMPLELINK_OPENTHREAD_SDK_BUILD=1 \
    -DSIMPLELINK_OPENTHREAD_CONFIG_CC2652=1 \
    -DBoard_EXCLUDE_NVS_EXTERNAL_FLASH \
    -DNVOCMP_POSIX_MUTEX \
    -DNVOCMP_NVPAGES=2 \
    -DNDEBUG \
    -DBOARD_DISPLAY_USE_UART=1 \
    -DMBEDTLS_CONFIG_FILE='"mbedtls-config-cc2652-ccs.h"' \
    -i${WORKSPACE_LOC}/libmbedcrypto_CC26X2R1_LAUNCHXL_tirtos_ccs/config \
    -iOPENTHREAD_MAKE_PATH/examples/platforms \
    -iOPENTHREAD_MAKE_PATH/include \
    -iOPENTHREAD_MAKE_PATH/src/core \
    -iOPENTHREAD_MAKE_PATH/third_party/mbedtls/repo/include \
    -iC:/.conan/54f80b/1/external/source \
    -i${PROJECT_ROOT}/platform/crypto \
    -i${WORKSPACE_LOC}/libopenthread_ftd_CC26X2R1_LAUNCHXL_tirtos_ccs/config \
    -DOPENTHREAD_CONFIG_FILE='"openthread-config-cc2652-ccs-ftd.h"' \
    -DOPENTHREAD_PROJECT_CORE_CONFIG_FILE='"openthread-core-cc2652-config-ide.h"' \
    -DENABLE_OAD=1 \
    -DSECURITY \
    -DTIOP_ENABLE_UART=1 \
    -iOPENTHREAD_MAKE_PATH/src/cli \
    -DDeviceFamily_CC26X2 \
    "-I$(SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/source" \
    "-I$(SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/source/ti/devices/cc13x2_cc26x2" \
    "-I$(SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/source/ti/posix/ccs" \
    --silicon_version=7M4 \
    --code_state=16 \
    --little_endian \
    --display_error_number \
    --diag_warning=255 \
    --diag_wrap=off \
    --float_support=FPv4SPD16 \
    --symdebug:dwarf \
    "-I$(CCS_ARMCOMPILER)/include"

LFLAGS = --unused_section_elimination=on \
    --heap_size=0 \
    -i${WORKSPACE_LOC}/libopenthread_cli_ftd_CC26X2R1_LAUNCHXL_tirtos_ccs/OptimizeSize \
    -llibopenthread_cli_ftd.a \
    -i${WORKSPACE_LOC}/libopenthread_ftd_CC26X2R1_LAUNCHXL_tirtos_ccs/OptimizeSize \
    -llibopenthread_ftd.a \
    -i${WORKSPACE_LOC}/libopenthread_platform_utils_ftd_CC26X2R1_LAUNCHXL_tirtos_ccs/OptimizeSize \
    -llibopenthread_platform_utils_ftd.a \
    -i${WORKSPACE_LOC}/libmbedcrypto_CC26X2R1_LAUNCHXL_tirtos_ccs/OptimizeSize \
    -llibmbedcrypto.a \
     -l${PROJECT_BUILD_DIR}/syscfg/ti_utils_build_linker.cmd.genlibs \
    "-i$(SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/source" \
    -iC:/.conan/54f80b/1/external/source \
    "-i$(SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/kernel/tirtos/packages" \
    -lti/devices/cc13x2_cc26x2/driverlib/bin/ccs/driverlib.lib \
    ../../tirtos/ccs/CC26X2R1_LAUNCHXL_TIRTOS_OAD.cmd \
    "-m$(NAME).map" \
    --warn_sections \
    --display_error_number \
    --diag_wrap=off \
    --rom_model \
    "-i$(CCS_ARMCOMPILER)/lib" \
    -llibc.a

all: $(NAME).out

cflags.opt: makefile $(SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/imports.mak
	@ echo --compileOptions $(CFLAGS) > $@

$(CONFIGPKG)/compiler.opt: $(CONFIGPKG)/linker.cmd

$(CONFIGPKG)/linker.cmd: ../../release_oad.cfg cflags.opt
	@ echo Running Configuro...
	@ $(XDC_INSTALL_DIR)/xs --xdcpath="$(XDCPATH)" xdc.tools.configuro -c "$(CCS_ARMCOMPILER)" -t $(XDCTARGET) -p $(PLATFORM) -@cflags.opt -o "$(CONFIGPKG)" ../../release_oad.cfg

.INTERMEDIATE: syscfg
tiop_config.c ti_radio_config.c ti_drivers_config.c tiop_config.h ti_radio_config.h ti_drivers_config.h: syscfg
	@ echo generation complete

syscfg: ../../oad_cli.syscfg
	@ echo Generating configuration files...
	@ $(SYSCONFIG_TOOL) --compiler ccs --product $(SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/.metadata/product.json --output $(@D) $<

platform_alarm.obj: ../../platform/alarm.c tiop_config.h ti_radio_config.h ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< --cmd_file=$(CONFIGPKG)/compiler.opt --output_file=$@

platform_alarm_micro.obj: ../../platform/alarm_micro.c tiop_config.h ti_radio_config.h ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< --cmd_file=$(CONFIGPKG)/compiler.opt --output_file=$@

platform_diag.obj: ../../platform/diag.c tiop_config.h ti_radio_config.h ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< --cmd_file=$(CONFIGPKG)/compiler.opt --output_file=$@

platform_entropy.obj: ../../platform/entropy.c tiop_config.h ti_radio_config.h ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< --cmd_file=$(CONFIGPKG)/compiler.opt --output_file=$@

platform_misc.obj: ../../platform/misc.c tiop_config.h ti_radio_config.h ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< --cmd_file=$(CONFIGPKG)/compiler.opt --output_file=$@

platform_settings.obj: ../../platform/settings.c tiop_config.h ti_radio_config.h ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< --cmd_file=$(CONFIGPKG)/compiler.opt --output_file=$@

platform_system.obj: ../../platform/system.c tiop_config.h ti_radio_config.h ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< --cmd_file=$(CONFIGPKG)/compiler.opt --output_file=$@

platform_radio.obj: ../../platform/radio.c tiop_config.h ti_radio_config.h ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< --cmd_file=$(CONFIGPKG)/compiler.opt --output_file=$@

platform_uart.obj: ../../platform/uart.c tiop_config.h ti_radio_config.h ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< --cmd_file=$(CONFIGPKG)/compiler.opt --output_file=$@

platform_debug_uart.obj: ../../platform/debug_uart.c tiop_config.h ti_radio_config.h ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< --cmd_file=$(CONFIGPKG)/compiler.opt --output_file=$@

platform_crypto_aes_alt.obj: ../../platform/crypto/aes_alt.c tiop_config.h ti_radio_config.h ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< --cmd_file=$(CONFIGPKG)/compiler.opt --output_file=$@

platform_crypto_sha256_alt.obj: ../../platform/crypto/sha256_alt.c tiop_config.h ti_radio_config.h ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< --cmd_file=$(CONFIGPKG)/compiler.opt --output_file=$@

platform_crypto_ecjpake_alt.obj: ../../platform/crypto/ecjpake_alt.c tiop_config.h ti_radio_config.h ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< --cmd_file=$(CONFIGPKG)/compiler.opt --output_file=$@

platform_crypto_crypto_misc.obj: ../../platform/crypto/crypto_misc.c tiop_config.h ti_radio_config.h ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< --cmd_file=$(CONFIGPKG)/compiler.opt --output_file=$@

platform_nv_crc.obj: ../../platform/nv/crc.c tiop_config.h ti_radio_config.h ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< --cmd_file=$(CONFIGPKG)/compiler.opt --output_file=$@

platform_nv_nvocmp.obj: ../../platform/nv/nvocmp.c tiop_config.h ti_radio_config.h ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< --cmd_file=$(CONFIGPKG)/compiler.opt --output_file=$@

oad_image_header.obj: ../../oad_image_header.c tiop_config.h ti_radio_config.h ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< --cmd_file=$(CONFIGPKG)/compiler.opt --output_file=$@

main.obj: ../../main.c tiop_config.h ti_radio_config.h ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< --cmd_file=$(CONFIGPKG)/compiler.opt --output_file=$@

otstack.obj: ../../otstack.c tiop_config.h ti_radio_config.h ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< --cmd_file=$(CONFIGPKG)/compiler.opt --output_file=$@

otsupport_otrtosapi.obj: ../../otsupport/otrtosapi.c tiop_config.h ti_radio_config.h ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< --cmd_file=$(CONFIGPKG)/compiler.opt --output_file=$@

cli.obj: ../../cli.c tiop_config.h ti_radio_config.h ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< --cmd_file=$(CONFIGPKG)/compiler.opt --output_file=$@

CC26X2R1_LAUNCHXL_fxns.obj: ../../CC26X2R1_LAUNCHXL_fxns.c tiop_config.h ti_radio_config.h ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< --cmd_file=$(CONFIGPKG)/compiler.opt --output_file=$@

bim_oad_debug_image_bin.obj: ../../bim_oad_debug_image_bin.c tiop_config.h ti_radio_config.h ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< --cmd_file=$(CONFIGPKG)/compiler.opt --output_file=$@

tiop_config.obj: tiop_config.c tiop_config.h ti_radio_config.h ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< --cmd_file=$(CONFIGPKG)/compiler.opt --output_file=$@

ti_radio_config.obj: ti_radio_config.c tiop_config.h ti_radio_config.h ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< --cmd_file=$(CONFIGPKG)/compiler.opt --output_file=$@

ti_drivers_config.obj: ti_drivers_config.c tiop_config.h ti_radio_config.h ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< --cmd_file=$(CONFIGPKG)/compiler.opt --output_file=$@

$(NAME).out: $(OBJECTS) $(CONFIGPKG)/linker.cmd
	@ echo linking $@
	@ $(LNK) -z $(OBJECTS) -l$(CONFIGPKG)/linker.cmd $(LFLAGS) -o $(NAME).out

clean:
	@ echo Cleaning...
	@ $(RM) $(OBJECTS) > $(DEVNULL) 2>&1
	@ $(RM) $(NAME).out > $(DEVNULL) 2>&1
	@ $(RM) $(NAME).map > $(DEVNULL) 2>&1
	@ $(RMDIR) $(NAME) > $(DEVNULL) 2>&1
	@ $(RM) cflags.opt > $(DEVNULL) 2>&1
	@ $(RM) tiop_config.c ti_radio_config.c ti_drivers_config.c tiop_config.h ti_radio_config.h ti_drivers_config.h > $(DEVNULL) 2>&1
