COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR ?= $(abspath ../../../../../../..)

include $(COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/imports.mak

XDCPATH = $(COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/source;$(COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/kernel/tirtos/packages
XDCTARGET = iar.targets.arm.M4F
PLATFORM = ti.platforms.simplelink:CC1352P1F3
.PRECIOUS: $(CONFIGPKG)/compiler.opt $(CONFIGPKG)/linker.cmd

CC = "$(IAR_ARMCOMPILER)/bin/iccarm"
LNK = "$(IAR_ARMCOMPILER)/bin/ilinkarm"
HEX = "$(IAR_ARMCOMPILER)/bin/ielftool"

OBJECTS = NodeRadioTask.obj NodeTask.obj NodeStorage.obj oad_native_oad_oad_client.obj oad_native_oad_oad_protocol.obj oad_native_oad_oad_storage.obj oad_native_oad_oad_image_header_app.obj oad_ble_oad_oad_switch.obj common_cc26xx_flash_interface_internal_flash_interface_int_rtos_NVS.obj common_cc26xx_crc_crc32.obj rfWsnNode.obj SceAdc.obj easylink_EasyLink.obj sce_scif_framework.obj sce_scif_osal_tirtos.obj ti_easylink_config.obj ti_radio_config.obj ti_drivers_config.obj sce_scif.obj

CONFIGPKG = rfWsnNodeIntFlashOadClientUApp

NAME = rfWsnNodeIntFlashOadClientUApp

CFLAGS = -I../.. \
    -I. \
    -DFEATURE_BLE_OAD \
    -DCC13X2 \
    -DOAD_ONCHIP \
    -DOAD_U_APP \
    "-I$(COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/source/ti/common/cc26xx/flash_interface" \
    -DDeviceFamily_CC13X2 \
    "-I$(COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/source" \
    "-I$(COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/source/ti/posix/iar" \
    --debug \
    --silent \
    -e \
    --aeabi \
    --thumb \
    --diag_suppress=Pa050

LFLAGS = --config_def=OAD_ONCHIP=1 \
    --config_def=OAD_U_APP=1 \
    "-L$(COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/source" \
    "-L$(COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/kernel/tirtos/packages" \
    -fti_utils_build_linker.cmd.genlibs \
    "$(COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/source/ti/devices/cc13x2_cc26x2/driverlib/bin/iar/driverlib.lib" \
    --config ../../tirtos/iar/CC1352P1_LAUNCHXL_TIRTOS_OAD.icf \
    --map "$(NAME).map" \
    --silent \
    --cpu=Cortex-M4F \
    --semihosting=iar_breakpoint

all: $(NAME).hex

cflags.opt: makefile $(COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/imports.mak
	@ echo --compileOptions $(CFLAGS) > $@

$(CONFIGPKG)/compiler.opt: $(CONFIGPKG)/linker.cmd

$(CONFIGPKG)/linker.cmd: ../../rfWsnNodeIntFlashOadClientUApp.cfg cflags.opt
	@ echo Running Configuro...
	@ $(XDC_INSTALL_DIR)/xs --xdcpath="$(XDCPATH)" xdc.tools.configuro -c "$(IAR_ARMCOMPILER)" -t $(XDCTARGET) -p $(PLATFORM) -@cflags.opt -o "$(CONFIGPKG)" ../../rfWsnNodeIntFlashOadClientUApp.cfg

.INTERMEDIATE: syscfg
ti_easylink_config.c ti_radio_config.c ti_drivers_config.c ti_utils_build_linker.cmd.genlibs ti_easylink_config.h ti_easylink_oad_config.h ti_radio_config.h ti_drivers_config.h: syscfg
	@ echo generation complete

syscfg: ../../tirtos/rfWsnNodeIntFlashOadClient.syscfg
	@ echo Generating configuration files...
	@ $(SYSCONFIG_TOOL) --compiler iar --product $(COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/.metadata/product.json --output $(@D) $<

NodeRadioTask.obj: ../../NodeRadioTask.c ti_easylink_config.h ti_easylink_oad_config.h ti_radio_config.h ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -f $(CONFIGPKG)/compiler.opt -o $@

NodeTask.obj: ../../NodeTask.c ti_easylink_config.h ti_easylink_oad_config.h ti_radio_config.h ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -f $(CONFIGPKG)/compiler.opt -o $@

NodeStorage.obj: ../../NodeStorage.c ti_easylink_config.h ti_easylink_oad_config.h ti_radio_config.h ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -f $(CONFIGPKG)/compiler.opt -o $@

oad_native_oad_oad_client.obj: ../../oad/native_oad/oad_client.c ti_easylink_config.h ti_easylink_oad_config.h ti_radio_config.h ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -f $(CONFIGPKG)/compiler.opt -o $@

oad_native_oad_oad_protocol.obj: ../../oad/native_oad/oad_protocol.c ti_easylink_config.h ti_easylink_oad_config.h ti_radio_config.h ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -f $(CONFIGPKG)/compiler.opt -o $@

oad_native_oad_oad_storage.obj: ../../oad/native_oad/oad_storage.c ti_easylink_config.h ti_easylink_oad_config.h ti_radio_config.h ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -f $(CONFIGPKG)/compiler.opt -o $@

oad_native_oad_oad_image_header_app.obj: ../../oad/native_oad/oad_image_header_app.c ti_easylink_config.h ti_easylink_oad_config.h ti_radio_config.h ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -f $(CONFIGPKG)/compiler.opt -o $@

oad_ble_oad_oad_switch.obj: ../../oad/ble_oad/oad_switch.c ti_easylink_config.h ti_easylink_oad_config.h ti_radio_config.h ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -f $(CONFIGPKG)/compiler.opt -o $@

common_cc26xx_flash_interface_internal_flash_interface_int_rtos_NVS.obj: ../../common/cc26xx/flash_interface/internal/flash_interface_int_rtos_NVS.c ti_easylink_config.h ti_easylink_oad_config.h ti_radio_config.h ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -f $(CONFIGPKG)/compiler.opt -o $@

common_cc26xx_crc_crc32.obj: ../../common/cc26xx/crc/crc32.c ti_easylink_config.h ti_easylink_oad_config.h ti_radio_config.h ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -f $(CONFIGPKG)/compiler.opt -o $@

rfWsnNode.obj: ../../rfWsnNode.c ti_easylink_config.h ti_easylink_oad_config.h ti_radio_config.h ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -f $(CONFIGPKG)/compiler.opt -o $@

SceAdc.obj: ../../SceAdc.c ti_easylink_config.h ti_easylink_oad_config.h ti_radio_config.h ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -f $(CONFIGPKG)/compiler.opt -o $@

easylink_EasyLink.obj: ../../easylink/EasyLink.c ti_easylink_config.h ti_easylink_oad_config.h ti_radio_config.h ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -f $(CONFIGPKG)/compiler.opt -o $@

sce_scif_framework.obj: ../../sce/scif_framework.c ti_easylink_config.h ti_easylink_oad_config.h ti_radio_config.h ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -f $(CONFIGPKG)/compiler.opt -o $@

sce_scif_osal_tirtos.obj: ../../sce/scif_osal_tirtos.c ti_easylink_config.h ti_easylink_oad_config.h ti_radio_config.h ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -f $(CONFIGPKG)/compiler.opt -o $@

sce_scif.obj: ../../sce/scif.c ti_easylink_config.h ti_easylink_oad_config.h ti_radio_config.h ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -f $(CONFIGPKG)/compiler.opt -o $@

ti_easylink_config.obj: ti_easylink_config.c ti_easylink_config.h ti_easylink_oad_config.h ti_radio_config.h ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -f $(CONFIGPKG)/compiler.opt -o $@

ti_radio_config.obj: ti_radio_config.c ti_easylink_config.h ti_easylink_oad_config.h ti_radio_config.h ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -f $(CONFIGPKG)/compiler.opt -o $@

ti_drivers_config.obj: ti_drivers_config.c ti_easylink_config.h ti_easylink_oad_config.h ti_radio_config.h ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -f $(CONFIGPKG)/compiler.opt -o $@

$(NAME).out: $(OBJECTS) $(CONFIGPKG)/linker.cmd
	@ echo linking $@
	@ $(LNK)  $(OBJECTS) -f $(CONFIGPKG)/linker.cmd $(LFLAGS) -o $(NAME).out

$(NAME).hex: $(NAME).out
	@ echo building $@
	@ $(HEX) --ihex --verbose $? $@

clean:
	@ echo Cleaning...
	@ $(RM) $(OBJECTS) > $(DEVNULL) 2>&1
	@ $(RM) $(NAME).out > $(DEVNULL) 2>&1
	@ $(RM) $(NAME).map > $(DEVNULL) 2>&1
	@ $(RMDIR) $(NAME) > $(DEVNULL) 2>&1
	@ $(RM) cflags.opt > $(DEVNULL) 2>&1
	@ $(RM) $(NAME).hex > $(DEVNULL) 2>&1
	@ $(RM) ti_easylink_config.c ti_radio_config.c ti_drivers_config.c ti_utils_build_linker.cmd.genlibs ti_easylink_config.h ti_easylink_oad_config.h ti_radio_config.h ti_drivers_config.h > $(DEVNULL) 2>&1
