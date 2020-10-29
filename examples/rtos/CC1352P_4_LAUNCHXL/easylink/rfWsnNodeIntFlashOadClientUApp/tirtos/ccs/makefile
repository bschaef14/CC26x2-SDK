COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR ?= $(abspath ../../../../../../..)

include $(COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/imports.mak

XDCPATH = $(COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/source;$(COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/kernel/tirtos/packages
XDCTARGET = ti.targets.arm.elf.M4F
PLATFORM = ti.platforms.simplelink:CC1352P1F3
.PRECIOUS: $(CONFIGPKG)/compiler.opt $(CONFIGPKG)/linker.cmd

CC = "$(CCS_ARMCOMPILER)/bin/armcl"
LNK = "$(CCS_ARMCOMPILER)/bin/armcl"
HEX = "$(CCS_ARMCOMPILER)/bin/armobjcopy"

OBJECTS = NodeRadioTask.obj NodeTask.obj NodeStorage.obj oad_native_oad_oad_client.obj oad_native_oad_oad_protocol.obj oad_native_oad_oad_storage.obj oad_native_oad_oad_image_header_app.obj oad_ble_oad_oad_switch.obj common_cc26xx_flash_interface_internal_flash_interface_int_rtos_NVS.obj common_cc26xx_crc_crc32.obj rfWsnNode.obj SceAdc.obj easylink_EasyLink.obj sce_scif_framework.obj sce_scif_osal_tirtos.obj ti_easylink_config.obj ti_radio_config.obj ti_drivers_config.obj sce_scif.obj

CONFIGPKG = rfWsnNodeIntFlashOadClientUApp

NAME = rfWsnNodeIntFlashOadClientUApp

CFLAGS = -I../.. \
    -I. \
    -DFEATURE_BLE_OAD \
    -DCC13X2 \
    -DOAD_ONCHIP \
    -DOAD_U_APP \
    -O4 \
    --opt_for_speed=1 \
    "-I$(COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/source/ti/common/cc26xx/flash_interface" \
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

LFLAGS = --define=OAD_ONCHIP \
    --define=OAD_U_APP \
    --diag_suppress=10325-D \
    "-i$(COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/source" \
    "-i$(COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/kernel/tirtos/packages" \
    ti_utils_build_linker.cmd.genlibs \
    -lti/devices/cc13x2_cc26x2/driverlib/bin/ccs/driverlib.lib \
    ../../tirtos/ccs/CC1352P_4_LAUNCHXL_TIRTOS_OAD.cmd \
    "-m$(NAME).map" \
    --warn_sections \
    --display_error_number \
    --diag_wrap=off \
    --rom_model \
    "-i$(CCS_ARMCOMPILER)/lib" \
    -llibc.a

all: $(NAME).hex

cflags.opt: makefile $(COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/imports.mak
	@ echo --compileOptions $(CFLAGS) > $@

$(CONFIGPKG)/compiler.opt: $(CONFIGPKG)/linker.cmd

$(CONFIGPKG)/linker.cmd: ../../rfWsnNodeIntFlashOadClientUApp.cfg cflags.opt
	@ echo Running Configuro...
	@ $(XDC_INSTALL_DIR)/xs --xdcpath="$(XDCPATH)" xdc.tools.configuro -c "$(CCS_ARMCOMPILER)" -t $(XDCTARGET) -p $(PLATFORM) -@cflags.opt -o "$(CONFIGPKG)" ../../rfWsnNodeIntFlashOadClientUApp.cfg

.INTERMEDIATE: syscfg
ti_easylink_config.c ti_radio_config.c ti_drivers_config.c ti_utils_build_linker.cmd.genlibs ti_easylink_config.h ti_easylink_oad_config.h ti_radio_config.h ti_drivers_config.h: syscfg
	@ echo generation complete

syscfg: ../../tirtos/rfWsnNodeIntFlashOadClient.syscfg
	@ echo Generating configuration files...
	@ $(SYSCONFIG_TOOL) --compiler ccs --product $(COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/.metadata/product.json --output $(@D) $<

NodeRadioTask.obj: ../../NodeRadioTask.c ti_easylink_config.h ti_easylink_oad_config.h ti_radio_config.h ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< --cmd_file=$(CONFIGPKG)/compiler.opt --output_file=$@

NodeTask.obj: ../../NodeTask.c ti_easylink_config.h ti_easylink_oad_config.h ti_radio_config.h ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< --cmd_file=$(CONFIGPKG)/compiler.opt --output_file=$@

NodeStorage.obj: ../../NodeStorage.c ti_easylink_config.h ti_easylink_oad_config.h ti_radio_config.h ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< --cmd_file=$(CONFIGPKG)/compiler.opt --output_file=$@

oad_native_oad_oad_client.obj: ../../oad/native_oad/oad_client.c ti_easylink_config.h ti_easylink_oad_config.h ti_radio_config.h ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< --cmd_file=$(CONFIGPKG)/compiler.opt --output_file=$@

oad_native_oad_oad_protocol.obj: ../../oad/native_oad/oad_protocol.c ti_easylink_config.h ti_easylink_oad_config.h ti_radio_config.h ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< --cmd_file=$(CONFIGPKG)/compiler.opt --output_file=$@

oad_native_oad_oad_storage.obj: ../../oad/native_oad/oad_storage.c ti_easylink_config.h ti_easylink_oad_config.h ti_radio_config.h ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< --cmd_file=$(CONFIGPKG)/compiler.opt --output_file=$@

oad_native_oad_oad_image_header_app.obj: ../../oad/native_oad/oad_image_header_app.c ti_easylink_config.h ti_easylink_oad_config.h ti_radio_config.h ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< --cmd_file=$(CONFIGPKG)/compiler.opt --output_file=$@

oad_ble_oad_oad_switch.obj: ../../oad/ble_oad/oad_switch.c ti_easylink_config.h ti_easylink_oad_config.h ti_radio_config.h ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< --cmd_file=$(CONFIGPKG)/compiler.opt --output_file=$@

common_cc26xx_flash_interface_internal_flash_interface_int_rtos_NVS.obj: ../../common/cc26xx/flash_interface/internal/flash_interface_int_rtos_NVS.c ti_easylink_config.h ti_easylink_oad_config.h ti_radio_config.h ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< --cmd_file=$(CONFIGPKG)/compiler.opt --output_file=$@

common_cc26xx_crc_crc32.obj: ../../common/cc26xx/crc/crc32.c ti_easylink_config.h ti_easylink_oad_config.h ti_radio_config.h ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< --cmd_file=$(CONFIGPKG)/compiler.opt --output_file=$@

rfWsnNode.obj: ../../rfWsnNode.c ti_easylink_config.h ti_easylink_oad_config.h ti_radio_config.h ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< --cmd_file=$(CONFIGPKG)/compiler.opt --output_file=$@

SceAdc.obj: ../../SceAdc.c ti_easylink_config.h ti_easylink_oad_config.h ti_radio_config.h ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< --cmd_file=$(CONFIGPKG)/compiler.opt --output_file=$@

easylink_EasyLink.obj: ../../easylink/EasyLink.c ti_easylink_config.h ti_easylink_oad_config.h ti_radio_config.h ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< --cmd_file=$(CONFIGPKG)/compiler.opt --output_file=$@

sce_scif_framework.obj: ../../sce/scif_framework.c ti_easylink_config.h ti_easylink_oad_config.h ti_radio_config.h ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< --cmd_file=$(CONFIGPKG)/compiler.opt --output_file=$@

sce_scif_osal_tirtos.obj: ../../sce/scif_osal_tirtos.c ti_easylink_config.h ti_easylink_oad_config.h ti_radio_config.h ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< --cmd_file=$(CONFIGPKG)/compiler.opt --output_file=$@

sce_scif.obj: ../../sce/scif.c ti_easylink_config.h ti_easylink_oad_config.h ti_radio_config.h ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< --cmd_file=$(CONFIGPKG)/compiler.opt --output_file=$@

ti_easylink_config.obj: ti_easylink_config.c ti_easylink_config.h ti_easylink_oad_config.h ti_radio_config.h ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< --cmd_file=$(CONFIGPKG)/compiler.opt --output_file=$@

ti_radio_config.obj: ti_radio_config.c ti_easylink_config.h ti_easylink_oad_config.h ti_radio_config.h ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< --cmd_file=$(CONFIGPKG)/compiler.opt --output_file=$@

ti_drivers_config.obj: ti_drivers_config.c ti_easylink_config.h ti_easylink_oad_config.h ti_radio_config.h ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< --cmd_file=$(CONFIGPKG)/compiler.opt --output_file=$@

$(NAME).out: $(OBJECTS) $(CONFIGPKG)/linker.cmd
	@ echo linking $@
	@ $(LNK) -z $(OBJECTS) -l$(CONFIGPKG)/linker.cmd $(LFLAGS) -o $(NAME).out

$(NAME).hex: $(NAME).out
	@ echo building $@
	@ $(HEX) -O ihex  $? $@

clean:
	@ echo Cleaning...
	@ $(RM) $(OBJECTS) > $(DEVNULL) 2>&1
	@ $(RM) $(NAME).out > $(DEVNULL) 2>&1
	@ $(RM) $(NAME).map > $(DEVNULL) 2>&1
	@ $(RMDIR) $(NAME) > $(DEVNULL) 2>&1
	@ $(RM) cflags.opt > $(DEVNULL) 2>&1
	@ $(RM) $(NAME).hex > $(DEVNULL) 2>&1
	@ $(RM) ti_easylink_config.c ti_radio_config.c ti_drivers_config.c ti_utils_build_linker.cmd.genlibs ti_easylink_config.h ti_easylink_oad_config.h ti_radio_config.h ti_drivers_config.h > $(DEVNULL) 2>&1
