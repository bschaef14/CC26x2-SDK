SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR ?= $(abspath ../../../../../../..)

include $(SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/imports.mak

KERNEL_BUILD := $(SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/kernel/freertos/builds/cc13x2_cc26x2/release

CC = "$(TICLANG_ARMCOMPILER)/bin/tiarmclang"
LNK = "$(TICLANG_ARMCOMPILER)/bin/tiarmclang"

OBJECTS = i2secho.obj AudioCodec.obj freertos_main_freertos.obj ti_drivers_config.obj ti_devices_config.obj

NAME = i2secho

CFLAGS = -I../.. \
    -I. \
    -Oz \
    -DDeviceFamily_CC13X2 \
    "-I$(SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/source" \
    "-I$(SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/source/ti/posix/ticlang" \
    "-I$(FREERTOS_INSTALL_DIR)/FreeRTOS/Source/include" \
    "-I$(FREERTOS_INSTALL_DIR)/FreeRTOS/Source/portable/GCC/ARM_CM4F" \
    "-I$(KERNEL_BUILD)" \
    -gdwarf-3 \
    -mcpu=cortex-m4 \
    -march=armv7e-m \
    -mthumb \
    -mfloat-abi=hard \
    -mfpu=fpv4-sp-d16

LFLAGS = "-L$(SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/source" \
     -lti/display/lib/ticlang/m4f/display.a \
     -lti/grlib/lib/ticlang/m4f/grlib.a \
     -lthird_party/spiffs/lib/ticlang/m4f/spiffs.a \
    ti_utils_build_linker.cmd.genlibs \
    "-l$(KERNEL_BUILD)/ticlang/freertos.lib" \
    -lti/devices/cc13x2_cc26x2/driverlib/bin/ticlang/driverlib.lib \
    ../../freertos/ticlang/cc13x2_cc26x2_freertos.cmd \
    "-Wl,-m,$(NAME).map" \
    -Wl,-c \
    -Wl,-w \
    "-L$(TICLANG_ARMCOMPILER)/lib" \
    -llibc.a

all: $(NAME).out

$(KERNEL_BUILD)/ticlang/freertos.lib:
	@ $(ECHOBLANKLINE)
	@ echo $@ is not built.
	@ echo You can build it by issuing $(MAKE) in $(dir $@).
	@ $(ECHOBLANKLINE)

.INTERMEDIATE: syscfg
ti_drivers_config.c ti_devices_config.c ti_utils_build_linker.cmd.genlibs ti_drivers_config.h: syscfg
	@ echo generation complete

syscfg: ../../freertos/i2secho.syscfg
	@ echo Generating configuration files...
	@ $(SYSCONFIG_TOOL) --compiler ticlang --product $(SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/.metadata/product.json --output $(@D) $<

i2secho.obj: ../../i2secho.c ti_drivers_config.h
	@ echo Building $@
	@ $(CC) $(CFLAGS) -c $< -o $@

AudioCodec.obj: ../../AudioCodec.c ti_drivers_config.h
	@ echo Building $@
	@ $(CC) $(CFLAGS) -c $< -o $@

freertos_main_freertos.obj: ../../freertos/main_freertos.c ti_drivers_config.h
	@ echo Building $@
	@ $(CC) $(CFLAGS) -c $< -o $@

ti_drivers_config.obj: ti_drivers_config.c ti_drivers_config.h
	@ echo Building $@
	@ $(CC) $(CFLAGS) -c $< -o $@

ti_devices_config.obj: ti_devices_config.c ti_drivers_config.h
	@ echo Building $@
	@ $(CC) $(CFLAGS) -c $< -o $@

$(NAME).out: $(OBJECTS) $(KERNEL_BUILD)/ticlang/freertos.lib
	@ echo linking $@
	@ $(LNK) -Wl,-u,_c_int00 $(OBJECTS)  $(LFLAGS) -o $(NAME).out

clean:
	@ echo Cleaning...
	@ $(RM) $(OBJECTS) > $(DEVNULL) 2>&1
	@ $(RM) $(NAME).out > $(DEVNULL) 2>&1
	@ $(RM) $(NAME).map > $(DEVNULL) 2>&1
	@ $(RM) ti_drivers_config.c ti_devices_config.c ti_utils_build_linker.cmd.genlibs ti_drivers_config.h > $(DEVNULL) 2>&1
