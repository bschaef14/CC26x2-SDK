COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR ?= $(abspath ../../../../../../..)

include $(COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/imports.mak

XDCPATH = $(COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/source;$(COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/kernel/tirtos/packages
XDCTARGET = gnu.targets.arm.M4F
PLATFORM = ti.platforms.simplelink:CC1312R1F3
.PRECIOUS: $(CONFIGPKG)/compiler.opt $(CONFIGPKG)/linker.cmd

CC = "$(GCC_ARMCOMPILER)/bin/arm-none-eabi-gcc"
LNK = "$(GCC_ARMCOMPILER)/bin/arm-none-eabi-gcc"
HEX = "$(GCC_ARMCOMPILER)/bin/arm-none-eabi-objcopy"

OBJECTS = NodeRadioTask.obj NodeTask.obj NodeStorage.obj oad_native_oad_oad_client.obj oad_native_oad_oad_protocol.obj oad_native_oad_oad_storage.obj oad_native_oad_oad_image_header_app.obj oad_ble_oad_oad_switch.obj common_cc26xx_flash_interface_internal_flash_interface_int_rtos_NVS.obj common_cc26xx_crc_crc32.obj rfWsnNode.obj SceAdc.obj easylink_EasyLink.obj sce_scif_framework.obj sce_scif_osal_tirtos.obj ti_easylink_config.obj ti_radio_config.obj ti_drivers_config.obj sce_scif.obj

CONFIGPKG = rfWsnNodeIntFlashOadClientUApp

NAME = rfWsnNodeIntFlashOadClientUApp

CFLAGS = -I../.. \
    -I. \
    -DFEATURE_BLE_OAD \
    -DCC13X2 \
    -DOAD_ONCHIP \
    -DOAD_U_APP \
    -Os \
    "-I$(COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/source/ti/common/cc26xx/flash_interface" \
    -DDeviceFamily_CC13X2 \
    "-I$(COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/source" \
    "-I$(COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/source/ti/posix/gcc" \
    -mcpu=cortex-m4 \
    -march=armv7e-m \
    -mthumb \
    -std=c99 \
    -mfloat-abi=hard \
    -mfpu=fpv4-sp-d16 \
    -ffunction-sections \
    -fdata-sections \
    -g \
    -gstrict-dwarf \
    -Wall \
    "-I$(COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/kernel/tirtos/packages/gnu/targets/arm/libs/install-native/arm-none-eabi/include/newlib-nano" \
    "-I$(COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/kernel/tirtos/packages/gnu/targets/arm/libs/install-native/arm-none-eabi/include" \
    "-I$(GCC_ARMCOMPILER)/arm-none-eabi/include"

LFLAGS = -Wl,--defsym=OAD_ONCHIP=1 \
    -Wl,--defsym=OAD_U_APP=1 \
    -Wl,-T,../../tirtos/gcc/CC1312R1_LAUNCHXL_TIRTOS_OAD.lds \
    "-Wl,-Map,$(NAME).map" \
    "-L$(COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/source" \
    "-L$(COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/kernel/tirtos/packages" \
    ti_utils_build_linker.cmd.genlibs \
    "-Wl,-T,$(NAME)/linker.cmd" \
    -l:ti/devices/cc13x2_cc26x2/driverlib/bin/gcc/driverlib.lib \
    -march=armv7e-m \
    -mthumb \
    -mfloat-abi=hard \
    -mfpu=fpv4-sp-d16 \
    -nostartfiles \
    -static \
    -Wl,--gc-sections \
    "-L$(COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/kernel/tirtos/packages/gnu/targets/arm/libs/install-native/arm-none-eabi/lib/thumb/v7e-m/hard" \
    -lgcc \
    -lc \
    -lm \
    -lnosys \
    --specs=nano.specs

all: $(NAME).hex

cflags.opt: makefile $(COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/imports.mak
	@ echo --compileOptions $(CFLAGS) > $@

$(CONFIGPKG)/compiler.opt: $(CONFIGPKG)/linker.cmd

$(CONFIGPKG)/linker.cmd: ../../rfWsnNodeIntFlashOadClientUApp.cfg cflags.opt
	@ echo Running Configuro...
	@ $(XDC_INSTALL_DIR)/xs --xdcpath="$(XDCPATH)" xdc.tools.configuro -c "$(GCC_ARMCOMPILER)" -t $(XDCTARGET) -p $(PLATFORM) -@cflags.opt -o "$(CONFIGPKG)" ../../rfWsnNodeIntFlashOadClientUApp.cfg

.INTERMEDIATE: syscfg
ti_easylink_config.c ti_radio_config.c ti_drivers_config.c ti_utils_build_linker.cmd.genlibs ti_easylink_config.h ti_easylink_oad_config.h ti_radio_config.h ti_drivers_config.h: syscfg
	@ echo generation complete

syscfg: ../../tirtos/rfWsnNodeIntFlashOadClient.syscfg
	@ echo Generating configuration files...
	@ $(SYSCONFIG_TOOL) --compiler gcc --product $(COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/.metadata/product.json --output $(@D) $<

NodeRadioTask.obj: ../../NodeRadioTask.c ti_easylink_config.h ti_easylink_oad_config.h ti_radio_config.h ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -c @$(CONFIGPKG)/compiler.opt -o $@

NodeTask.obj: ../../NodeTask.c ti_easylink_config.h ti_easylink_oad_config.h ti_radio_config.h ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -c @$(CONFIGPKG)/compiler.opt -o $@

NodeStorage.obj: ../../NodeStorage.c ti_easylink_config.h ti_easylink_oad_config.h ti_radio_config.h ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -c @$(CONFIGPKG)/compiler.opt -o $@

oad_native_oad_oad_client.obj: ../../oad/native_oad/oad_client.c ti_easylink_config.h ti_easylink_oad_config.h ti_radio_config.h ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -c @$(CONFIGPKG)/compiler.opt -o $@

oad_native_oad_oad_protocol.obj: ../../oad/native_oad/oad_protocol.c ti_easylink_config.h ti_easylink_oad_config.h ti_radio_config.h ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -c @$(CONFIGPKG)/compiler.opt -o $@

oad_native_oad_oad_storage.obj: ../../oad/native_oad/oad_storage.c ti_easylink_config.h ti_easylink_oad_config.h ti_radio_config.h ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -c @$(CONFIGPKG)/compiler.opt -o $@

oad_native_oad_oad_image_header_app.obj: ../../oad/native_oad/oad_image_header_app.c ti_easylink_config.h ti_easylink_oad_config.h ti_radio_config.h ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -c @$(CONFIGPKG)/compiler.opt -o $@

oad_ble_oad_oad_switch.obj: ../../oad/ble_oad/oad_switch.c ti_easylink_config.h ti_easylink_oad_config.h ti_radio_config.h ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -c @$(CONFIGPKG)/compiler.opt -o $@

common_cc26xx_flash_interface_internal_flash_interface_int_rtos_NVS.obj: ../../common/cc26xx/flash_interface/internal/flash_interface_int_rtos_NVS.c ti_easylink_config.h ti_easylink_oad_config.h ti_radio_config.h ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -c @$(CONFIGPKG)/compiler.opt -o $@

common_cc26xx_crc_crc32.obj: ../../common/cc26xx/crc/crc32.c ti_easylink_config.h ti_easylink_oad_config.h ti_radio_config.h ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -c @$(CONFIGPKG)/compiler.opt -o $@

rfWsnNode.obj: ../../rfWsnNode.c ti_easylink_config.h ti_easylink_oad_config.h ti_radio_config.h ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -c @$(CONFIGPKG)/compiler.opt -o $@

SceAdc.obj: ../../SceAdc.c ti_easylink_config.h ti_easylink_oad_config.h ti_radio_config.h ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -c @$(CONFIGPKG)/compiler.opt -o $@

easylink_EasyLink.obj: ../../easylink/EasyLink.c ti_easylink_config.h ti_easylink_oad_config.h ti_radio_config.h ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -c @$(CONFIGPKG)/compiler.opt -o $@

sce_scif_framework.obj: ../../sce/scif_framework.c ti_easylink_config.h ti_easylink_oad_config.h ti_radio_config.h ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -c @$(CONFIGPKG)/compiler.opt -o $@

sce_scif_osal_tirtos.obj: ../../sce/scif_osal_tirtos.c ti_easylink_config.h ti_easylink_oad_config.h ti_radio_config.h ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -c @$(CONFIGPKG)/compiler.opt -o $@

sce_scif.obj: ../../sce/scif.c ti_easylink_config.h ti_easylink_oad_config.h ti_radio_config.h ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -c @$(CONFIGPKG)/compiler.opt -o $@

ti_easylink_config.obj: ti_easylink_config.c ti_easylink_config.h ti_easylink_oad_config.h ti_radio_config.h ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -c @$(CONFIGPKG)/compiler.opt -o $@

ti_radio_config.obj: ti_radio_config.c ti_easylink_config.h ti_easylink_oad_config.h ti_radio_config.h ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -c @$(CONFIGPKG)/compiler.opt -o $@

ti_drivers_config.obj: ti_drivers_config.c ti_easylink_config.h ti_easylink_oad_config.h ti_radio_config.h ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -c @$(CONFIGPKG)/compiler.opt -o $@

$(NAME).out: $(OBJECTS) $(CONFIGPKG)/linker.cmd
	@ echo linking $@
	@ $(LNK)  $(OBJECTS)  $(LFLAGS) -o $(NAME).out

$(NAME).hex: $(NAME).out
	@ echo building $@
	@ $(HEX) -O ihex --remove-section .vtable --remove-section .dmaSpi0RxControlTableEntry --remove-section .dmaSpi0TxControlTableEntry --remove-section .dmaSpi1RxControlTableEntry --remove-section .dmaSpi1TxControlTableEntry --remove-section .dmaSpi0RxAltControlTableEntry --remove-section .dmaSpi0TxAltControlTableEntry --remove-section .dmaSpi1RxAltControlTableEntry --remove-section .dmaSpi1TxAltControlTableEntry --gap-fill 0xff $? $@

clean:
	@ echo Cleaning...
	@ $(RM) $(OBJECTS) > $(DEVNULL) 2>&1
	@ $(RM) $(NAME).out > $(DEVNULL) 2>&1
	@ $(RM) $(NAME).map > $(DEVNULL) 2>&1
	@ $(RMDIR) $(NAME) > $(DEVNULL) 2>&1
	@ $(RM) cflags.opt > $(DEVNULL) 2>&1
	@ $(RM) $(NAME).hex > $(DEVNULL) 2>&1
	@ $(RM) ti_easylink_config.c ti_radio_config.c ti_drivers_config.c ti_utils_build_linker.cmd.genlibs ti_easylink_config.h ti_easylink_oad_config.h ti_radio_config.h ti_drivers_config.h > $(DEVNULL) 2>&1
