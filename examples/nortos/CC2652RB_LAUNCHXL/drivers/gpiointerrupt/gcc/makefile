SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR ?= $(abspath ../../../../../..)

include $(SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/imports.mak

CC = "$(GCC_ARMCOMPILER)/bin/arm-none-eabi-gcc"
LNK = "$(GCC_ARMCOMPILER)/bin/arm-none-eabi-gcc"

OBJECTS = gpiointerrupt.obj main_nortos.obj ti_drivers_config.obj ti_devices_config.obj

NAME = gpiointerrupt

CFLAGS = -I.. \
    -I. \
    -DDeviceFamily_CC26X2 \
    "-I$(SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/source" \
    "-I$(SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/kernel/nortos" \
    "-I$(SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/kernel/nortos/posix" \
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
    "-I$(GCC_ARMCOMPILER)/arm-none-eabi/include/newlib-nano" \
    "-I$(GCC_ARMCOMPILER)/arm-none-eabi/include"

LFLAGS = -Wl,-T,../gcc/cc13x2_cc26x2_nortos.lds \
    "-Wl,-Map,$(NAME).map" \
    "-L$(SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/source" \
    -eresetISR \
    "-L$(SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/kernel/nortos" \
    ti_utils_build_linker.cmd.genlibs \
    -l:ti/devices/cc13x2_cc26x2/driverlib/bin/gcc/driverlib.lib \
    -march=armv7e-m \
    -mthumb \
    -mfloat-abi=hard \
    -mfpu=fpv4-sp-d16 \
    -nostartfiles \
    -static \
    -Wl,--gc-sections \
    "-L$(GCC_ARMCOMPILER)/arm-none-eabi/lib/thumb/v7e-m/hard" \
    -lgcc \
    -lc \
    -lm \
    -lnosys \
    --specs=nano.specs

all: $(NAME).out

.INTERMEDIATE: syscfg
ti_drivers_config.c ti_devices_config.c ti_utils_build_linker.cmd.genlibs ti_drivers_config.h: syscfg
	@ echo generation complete

syscfg: ../gpiointerrupt.syscfg
	@ echo Generating configuration files...
	@ $(SYSCONFIG_TOOL) --compiler gcc --product $(SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/.metadata/product.json --output $(@D) $<

gpiointerrupt.obj: ../gpiointerrupt.c ti_drivers_config.h
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -c -o $@

main_nortos.obj: ../main_nortos.c ti_drivers_config.h
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -c -o $@

ti_drivers_config.obj: ti_drivers_config.c ti_drivers_config.h
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -c -o $@

ti_devices_config.obj: ti_devices_config.c ti_drivers_config.h
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -c -o $@

$(NAME).out: $(OBJECTS)
	@ echo linking $@
	@ $(LNK)  $(OBJECTS)  $(LFLAGS) -o $(NAME).out

clean:
	@ echo Cleaning...
	@ $(RM) $(OBJECTS) > $(DEVNULL) 2>&1
	@ $(RM) $(NAME).out > $(DEVNULL) 2>&1
	@ $(RM) $(NAME).map > $(DEVNULL) 2>&1
	@ $(RM) ti_drivers_config.c ti_devices_config.c ti_utils_build_linker.cmd.genlibs ti_drivers_config.h > $(DEVNULL) 2>&1
