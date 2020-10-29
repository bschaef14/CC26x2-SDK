COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR ?= $(abspath ../../../../../../..)

include $(COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/imports.mak

KERNEL_BUILD := $(COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/kernel/tirtos/builds/cc13x2_cc26x2/release

CC = "$(GCC_ARMCOMPILER)/bin/arm-none-eabi-gcc"
LNK = "$(GCC_ARMCOMPILER)/bin/arm-none-eabi-gcc"

OBJECTS = ConcentratorRadioTask.obj ConcentratorTask.obj rfWsnConcentrator.obj oad_native_oad_oad_server.obj oad_native_oad_oad_protocol.obj oad_native_oad_oad_storage.obj common_cc26xx_flash_interface_external_flash_interface_ext_rtos_NVS.obj common_cc26xx_crc_crc32.obj easylink_EasyLink.obj ti_easylink_config.obj ti_radio_config.obj ti_drivers_config.obj ti_devices_config.obj

CONFIGPKG = $(KERNEL_BUILD)/gcc

NAME = rfWsnConcentratorOadServer

CFLAGS = -I../.. \
    -I. \
    -DFEATURE_OAD_SERVER_ONLY \
    -DCC13X2 \
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

LFLAGS = -Wl,-T,../../tirtos/gcc/cc13x2_cc26x2_tirtos.lds \
    "-Wl,-Map,$(NAME).map" \
    "-L$(COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/source" \
    "-L$(COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/kernel/tirtos/packages" \
    ti_utils_build_linker.cmd.genlibs \
    "-Wl,-T,$(KERNEL_BUILD)/gcc/linker.cmd" \
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

all: $(NAME).out

$(CONFIGPKG)/linker.cmd $(CONFIGPKG)/compiler.opt:
	@ $(ECHOBLANKLINE)
	@ echo $(CONFIGPKG) is not built.
	@ echo You can build it by issuing $(MAKE) in $(CONFIGPKG).
	@ $(ECHOBLANKLINE)

.INTERMEDIATE: syscfg
ti_easylink_config.c ti_radio_config.c ti_drivers_config.c ti_devices_config.c ti_utils_build_linker.cmd.genlibs ti_easylink_config.h ti_easylink_oad_config.h ti_radio_config.h ti_drivers_config.h: syscfg
	@ echo generation complete

syscfg: ../../tirtos/rfWsnConcentratorOadServer.syscfg
	@ echo Generating configuration files...
	@ $(SYSCONFIG_TOOL) --compiler gcc --product $(COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/.metadata/product.json --output $(@D) $<

ConcentratorRadioTask.obj: ../../ConcentratorRadioTask.c ti_easylink_config.h ti_easylink_oad_config.h ti_radio_config.h ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -c @$(CONFIGPKG)/compiler.opt -o $@

ConcentratorTask.obj: ../../ConcentratorTask.c ti_easylink_config.h ti_easylink_oad_config.h ti_radio_config.h ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -c @$(CONFIGPKG)/compiler.opt -o $@

rfWsnConcentrator.obj: ../../rfWsnConcentrator.c ti_easylink_config.h ti_easylink_oad_config.h ti_radio_config.h ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -c @$(CONFIGPKG)/compiler.opt -o $@

oad_native_oad_oad_server.obj: ../../oad/native_oad/oad_server.c ti_easylink_config.h ti_easylink_oad_config.h ti_radio_config.h ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -c @$(CONFIGPKG)/compiler.opt -o $@

oad_native_oad_oad_protocol.obj: ../../oad/native_oad/oad_protocol.c ti_easylink_config.h ti_easylink_oad_config.h ti_radio_config.h ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -c @$(CONFIGPKG)/compiler.opt -o $@

oad_native_oad_oad_storage.obj: ../../oad/native_oad/oad_storage.c ti_easylink_config.h ti_easylink_oad_config.h ti_radio_config.h ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -c @$(CONFIGPKG)/compiler.opt -o $@

common_cc26xx_flash_interface_external_flash_interface_ext_rtos_NVS.obj: ../../common/cc26xx/flash_interface/external/flash_interface_ext_rtos_NVS.c ti_easylink_config.h ti_easylink_oad_config.h ti_radio_config.h ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -c @$(CONFIGPKG)/compiler.opt -o $@

common_cc26xx_crc_crc32.obj: ../../common/cc26xx/crc/crc32.c ti_easylink_config.h ti_easylink_oad_config.h ti_radio_config.h ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -c @$(CONFIGPKG)/compiler.opt -o $@

easylink_EasyLink.obj: ../../easylink/EasyLink.c ti_easylink_config.h ti_easylink_oad_config.h ti_radio_config.h ti_drivers_config.h $(CONFIGPKG)/compiler.opt
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

ti_devices_config.obj: ti_devices_config.c ti_easylink_config.h ti_easylink_oad_config.h ti_radio_config.h ti_drivers_config.h $(CONFIGPKG)/compiler.opt
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
	@ $(RM) ti_easylink_config.c ti_radio_config.c ti_drivers_config.c ti_devices_config.c ti_utils_build_linker.cmd.genlibs ti_easylink_config.h ti_easylink_oad_config.h ti_radio_config.h ti_drivers_config.h > $(DEVNULL) 2>&1
