SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR ?= $(abspath ../../../../../..)

include $(SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/imports.mak

CC = "$(TICLANG_ARMCOMPILER)/bin/tiarmclang"
LNK = "$(TICLANG_ARMCOMPILER)/bin/tiarmclang"

OBJECTS = uart2echo.obj main_nortos.obj ti_drivers_config.obj ti_devices_config.obj

NAME = uart2echo

CFLAGS = -I.. \
    -I. \
    -Oz \
    -DDeviceFamily_CC26X2 \
    "-I$(SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/source" \
    "-I$(SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/kernel/nortos" \
    "-I$(SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/kernel/nortos/posix" \
    -gdwarf-3 \
    -mcpu=cortex-m4 \
    -march=armv7e-m \
    -mthumb \
    -mfloat-abi=hard \
    -mfpu=fpv4-sp-d16

LFLAGS = "-L$(SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/source" \
     -lti/grlib/lib/ticlang/m4f/grlib.a \
     -lthird_party/spiffs/lib/ticlang/m4f/spiffs.a \
     -lti/drivers/lib/ticlang/m4f/drivers_cc26x2.a \
    "-L$(SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/kernel/nortos" \
    ti_utils_build_linker.cmd.genlibs \
    -lti/devices/cc13x2_cc26x2/driverlib/bin/ticlang/driverlib.lib \
    ../ticlang/cc13x2_cc26x2_nortos.cmd \
    "-Wl,-m,$(NAME).map" \
    -Wl,-c \
    -Wl,-w \
    "-L$(TICLANG_ARMCOMPILER)/lib" \
    -llibc.a

all: $(NAME).out

.INTERMEDIATE: syscfg
ti_drivers_config.c ti_devices_config.c ti_utils_build_linker.cmd.genlibs ti_drivers_config.h: syscfg
	@ echo generation complete

syscfg: ../uart2echo.syscfg
	@ echo Generating configuration files...
	@ $(SYSCONFIG_TOOL) --compiler ticlang --product $(SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/.metadata/product.json --output $(@D) $<

uart2echo.obj: ../uart2echo.c ti_drivers_config.h
	@ echo Building $@
	@ $(CC) $(CFLAGS) -c $< -o $@

main_nortos.obj: ../main_nortos.c ti_drivers_config.h
	@ echo Building $@
	@ $(CC) $(CFLAGS) -c $< -o $@

ti_drivers_config.obj: ti_drivers_config.c ti_drivers_config.h
	@ echo Building $@
	@ $(CC) $(CFLAGS) -c $< -o $@

ti_devices_config.obj: ti_devices_config.c ti_drivers_config.h
	@ echo Building $@
	@ $(CC) $(CFLAGS) -c $< -o $@

$(NAME).out: $(OBJECTS)
	@ echo linking $@
	@ $(LNK) -Wl,-u,_c_int00 $(OBJECTS)  $(LFLAGS) -o $(NAME).out

clean:
	@ echo Cleaning...
	@ $(RM) $(OBJECTS) > $(DEVNULL) 2>&1
	@ $(RM) $(NAME).out > $(DEVNULL) 2>&1
	@ $(RM) $(NAME).map > $(DEVNULL) 2>&1
	@ $(RM) ti_drivers_config.c ti_devices_config.c ti_utils_build_linker.cmd.genlibs ti_drivers_config.h > $(DEVNULL) 2>&1
