SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR ?= $(abspath ../../../../../../..)

include $(SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/imports.mak

KERNEL_BUILD := $(SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/kernel/tirtos/builds/cc13x2_cc26x2/release

CC = "$(IAR_ARMCOMPILER)/bin/iccarm"
LNK = "$(IAR_ARMCOMPILER)/bin/ilinkarm"

OBJECTS = log.obj tirtos_main_tirtos.obj ti_utils_osal_config.obj ti_utils_runtime_config.obj ti_loggers_utils_config.obj ti_loggers_drivers_config.obj ti_drivers_config.obj ti_devices_config.obj

CONFIGPKG = $(KERNEL_BUILD)/iar

NAME = log

CFLAGS = -I../.. \
    -I. \
    -Dti_utils_runtime_Log_ENABLE_PPO \
    -Dti_utils_runtime_Log_ENABLE_INFO1 \
    -Dti_utils_runtime_Log_ENABLE_INFO2 \
    -Dti_utils_runtime_Log_ENABLE_Log_ERROR \
    -DDeviceFamily_CC26X2 \
    "-I$(SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/source" \
    "-I$(SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/source/ti/posix/iar" \
    --debug \
    --silent \
    -e \
    --aeabi \
    --thumb \
    --diag_suppress=Pa050

LFLAGS = "-L$(SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/source" \
    -fti_utils_runtime_LogSite.cmd.genlibs \
    "-L$(SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/kernel/tirtos/packages" \
    -fti_utils_build_linker.cmd.genlibs \
    -f "$(KERNEL_BUILD)/iar/linker.cmd" \
    "$(SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/source/ti/devices/cc13x2_cc26x2/driverlib/bin/iar/driverlib.lib" \
    --config ../../tirtos/iar/cc13x2_cc26x2_tirtos.icf \
    --map "$(NAME).map" \
    --silent \
    --cpu=Cortex-M4F \
    --semihosting=iar_breakpoint

all: $(NAME).out

$(CONFIGPKG)/linker.cmd $(CONFIGPKG)/compiler.opt:
	@ $(ECHOBLANKLINE)
	@ echo $(CONFIGPKG) is not built.
	@ echo You can build it by issuing $(MAKE) in $(CONFIGPKG).
	@ $(ECHOBLANKLINE)

.INTERMEDIATE: syscfg
ti_utils_osal_config.c ti_utils_runtime_config.c ti_loggers_utils_config.c ti_loggers_drivers_config.c ti_drivers_config.c ti_devices_config.c ti_utils_build_linker.cmd.genlibs ti_utils_osal_config.h ti_utils_runtime_config.h ti_loggers_utils_config.h ti_loggers_drivers_config.h ti_drivers_config.h: syscfg
	@ echo generation complete

syscfg: ../../tirtos/log_tirtos.syscfg
	@ echo Generating configuration files...
	@ $(SYSCONFIG_TOOL) --compiler iar --product $(SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/.metadata/product.json --output $(@D) $<

log.obj: ../../log.c ti_utils_osal_config.h ti_utils_runtime_config.h ti_loggers_utils_config.h ti_loggers_drivers_config.h ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -f $(CONFIGPKG)/compiler.opt -o $@

tirtos_main_tirtos.obj: ../../tirtos/main_tirtos.c ti_utils_osal_config.h ti_utils_runtime_config.h ti_loggers_utils_config.h ti_loggers_drivers_config.h ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -f $(CONFIGPKG)/compiler.opt -o $@

ti_utils_osal_config.obj: ti_utils_osal_config.c ti_utils_osal_config.h ti_utils_runtime_config.h ti_loggers_utils_config.h ti_loggers_drivers_config.h ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -f $(CONFIGPKG)/compiler.opt -o $@

ti_utils_runtime_config.obj: ti_utils_runtime_config.c ti_utils_osal_config.h ti_utils_runtime_config.h ti_loggers_utils_config.h ti_loggers_drivers_config.h ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -f $(CONFIGPKG)/compiler.opt -o $@

ti_loggers_utils_config.obj: ti_loggers_utils_config.c ti_utils_osal_config.h ti_utils_runtime_config.h ti_loggers_utils_config.h ti_loggers_drivers_config.h ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -f $(CONFIGPKG)/compiler.opt -o $@

ti_loggers_drivers_config.obj: ti_loggers_drivers_config.c ti_utils_osal_config.h ti_utils_runtime_config.h ti_loggers_utils_config.h ti_loggers_drivers_config.h ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -f $(CONFIGPKG)/compiler.opt -o $@

ti_drivers_config.obj: ti_drivers_config.c ti_utils_osal_config.h ti_utils_runtime_config.h ti_loggers_utils_config.h ti_loggers_drivers_config.h ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -f $(CONFIGPKG)/compiler.opt -o $@

ti_devices_config.obj: ti_devices_config.c ti_utils_osal_config.h ti_utils_runtime_config.h ti_loggers_utils_config.h ti_loggers_drivers_config.h ti_drivers_config.h $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -f $(CONFIGPKG)/compiler.opt -o $@

$(NAME).out: $(OBJECTS) $(CONFIGPKG)/linker.cmd
	@ echo linking $@
	@ $(LNK)  $(OBJECTS)  $(LFLAGS) -o $(NAME).out

clean:
	@ echo Cleaning...
	@ $(RM) $(OBJECTS) > $(DEVNULL) 2>&1
	@ $(RM) $(NAME).out > $(DEVNULL) 2>&1
	@ $(RM) $(NAME).map > $(DEVNULL) 2>&1
	@ $(RM) ti_utils_osal_config.c ti_utils_runtime_config.c ti_loggers_utils_config.c ti_loggers_drivers_config.c ti_drivers_config.c ti_devices_config.c ti_utils_build_linker.cmd.genlibs ti_utils_osal_config.h ti_utils_runtime_config.h ti_loggers_utils_config.h ti_loggers_drivers_config.h ti_drivers_config.h > $(DEVNULL) 2>&1
