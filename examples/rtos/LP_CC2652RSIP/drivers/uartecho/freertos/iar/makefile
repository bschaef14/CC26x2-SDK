SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR ?= $(abspath ../../../../../../..)

include $(SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/imports.mak

KERNEL_BUILD := $(SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/kernel/freertos/builds/cc13x2_cc26x2/release

CC = "$(IAR_ARMCOMPILER)/bin/iccarm"
LNK = "$(IAR_ARMCOMPILER)/bin/ilinkarm"

OBJECTS = uartecho.obj freertos_main_freertos.obj ti_drivers_config.obj ti_devices_config.obj

NAME = uartecho

CFLAGS = -I../.. \
    -I. \
    -DDeviceFamily_CC26X2 \
    "-I$(SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/source" \
    "-I$(SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/source/ti/posix/iar" \
    "-I$(FREERTOS_INSTALL_DIR)/FreeRTOS/Source/include" \
    "-I$(FREERTOS_INSTALL_DIR)/FreeRTOS/Source/portable/IAR/ARM_CM4F" \
    "-I$(KERNEL_BUILD)" \
    --cpu=Cortex-M4F \
    --debug \
    --silent \
    -e \
    --aeabi \
    --thumb \
    --diag_suppress=Pa050

LFLAGS = "-L$(SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/source" \
    --threaded_lib \
    -fti_utils_build_linker.cmd.genlibs \
    "$(KERNEL_BUILD)/iar/freertos.lib" \
    "$(SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/source/ti/devices/cc13x2_cc26x2/driverlib/bin/iar/driverlib.lib" \
    --config ../../freertos/iar/cc13x2_cc26x2_freertos.icf \
    --map "$(NAME).map" \
    --silent \
    --cpu=Cortex-M4F \
    --semihosting=iar_breakpoint

all: $(NAME).out

$(KERNEL_BUILD)/iar/freertos.lib:
	@ $(ECHOBLANKLINE)
	@ echo $@ is not built.
	@ echo You can build it by issuing $(MAKE) in $(dir $@).
	@ $(ECHOBLANKLINE)

.INTERMEDIATE: syscfg
ti_drivers_config.c ti_devices_config.c ti_utils_build_linker.cmd.genlibs ti_drivers_config.h: syscfg
	@ echo generation complete

syscfg: ../../freertos/uartecho.syscfg
	@ echo Generating configuration files...
	@ $(SYSCONFIG_TOOL) --compiler iar --product $(SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/.metadata/product.json --output $(@D) $<

uartecho.obj: ../../uartecho.c ti_drivers_config.h
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -o $@

freertos_main_freertos.obj: ../../freertos/main_freertos.c ti_drivers_config.h
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -o $@

ti_drivers_config.obj: ti_drivers_config.c ti_drivers_config.h
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -o $@

ti_devices_config.obj: ti_devices_config.c ti_drivers_config.h
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -o $@

$(NAME).out: $(OBJECTS) $(KERNEL_BUILD)/iar/freertos.lib
	@ echo linking $@
	@ $(LNK)  $(OBJECTS)  $(LFLAGS) -o $(NAME).out

clean:
	@ echo Cleaning...
	@ $(RM) $(OBJECTS) > $(DEVNULL) 2>&1
	@ $(RM) $(NAME).out > $(DEVNULL) 2>&1
	@ $(RM) $(NAME).map > $(DEVNULL) 2>&1
	@ $(RM) ti_drivers_config.c ti_devices_config.c ti_utils_build_linker.cmd.genlibs ti_drivers_config.h > $(DEVNULL) 2>&1
