COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR ?= $(abspath ../../../../../..)

include $(COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/imports.mak

CC = "$(IAR_ARMCOMPILER)/bin/iccarm"
LNK = "$(IAR_ARMCOMPILER)/bin/ilinkarm"

OBJECTS = ti_easylink_config.obj ti_radio_config.obj ti_drivers_config.obj ti_devices_config.obj rfEasylinkListenBeforeTalk_nortos.obj easylink_EasyLink_nortos.obj main_nortos.obj

NAME = rfEasyLinkListenBeforeTalk

CFLAGS = -I.. \
    -I. \
    -DDeviceFamily_CC13X2 \
    "-I$(COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/source" \
    "-I$(COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/kernel/nortos" \
    "-I$(COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/kernel/nortos/posix" \
    --cpu=Cortex-M4F \
    --debug \
    --silent \
    -e \
    --aeabi \
    --thumb \
    --diag_suppress=Pa050

LFLAGS = "-L$(COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/source" \
    "-L$(COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/kernel/nortos" \
    -fti_utils_build_linker.cmd.genlibs \
    "$(COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/source/ti/devices/cc13x2_cc26x2/driverlib/bin/iar/driverlib.lib" \
    --config ../iar/cc13x2_cc26x2_nortos.icf \
    --map "$(NAME).map" \
    --silent \
    --cpu=Cortex-M4F \
    --semihosting=iar_breakpoint

all: $(NAME).out

.INTERMEDIATE: syscfg
ti_easylink_config.c ti_radio_config.c ti_drivers_config.c ti_devices_config.c ti_utils_build_linker.cmd.genlibs ti_easylink_config.h ti_radio_config.h ti_drivers_config.h: syscfg
	@ echo generation complete

syscfg: ../rfEasyLinkListenBeforeTalk.syscfg
	@ echo Generating configuration files...
	@ $(SYSCONFIG_TOOL) --compiler iar --product $(COM_TI_SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/.metadata/product.json --output $(@D) $<

rfEasylinkListenBeforeTalk_nortos.obj: ../rfEasylinkListenBeforeTalk_nortos.c ti_easylink_config.h ti_radio_config.h ti_drivers_config.h
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -o $@

easylink_EasyLink_nortos.obj: ../easylink/EasyLink_nortos.c ti_easylink_config.h ti_radio_config.h ti_drivers_config.h
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -o $@

main_nortos.obj: ../main_nortos.c ti_easylink_config.h ti_radio_config.h ti_drivers_config.h
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -o $@

ti_easylink_config.obj: ti_easylink_config.c ti_easylink_config.h ti_radio_config.h ti_drivers_config.h
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -o $@

ti_radio_config.obj: ti_radio_config.c ti_easylink_config.h ti_radio_config.h ti_drivers_config.h
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -o $@

ti_drivers_config.obj: ti_drivers_config.c ti_easylink_config.h ti_radio_config.h ti_drivers_config.h
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -o $@

ti_devices_config.obj: ti_devices_config.c ti_easylink_config.h ti_radio_config.h ti_drivers_config.h
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -o $@

$(NAME).out: $(OBJECTS)
	@ echo linking $@
	@ $(LNK)  $(OBJECTS)  $(LFLAGS) -o $(NAME).out

clean:
	@ echo Cleaning...
	@ $(RM) $(OBJECTS) > $(DEVNULL) 2>&1
	@ $(RM) $(NAME).out > $(DEVNULL) 2>&1
	@ $(RM) $(NAME).map > $(DEVNULL) 2>&1
	@ $(RM) ti_easylink_config.c ti_radio_config.c ti_drivers_config.c ti_devices_config.c ti_utils_build_linker.cmd.genlibs ti_easylink_config.h ti_radio_config.h ti_drivers_config.h > $(DEVNULL) 2>&1
