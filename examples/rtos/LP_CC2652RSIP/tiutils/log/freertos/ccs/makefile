SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR ?= $(abspath ../../../../../../..)

include $(SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/imports.mak

KERNEL_BUILD := $(SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/kernel/freertos/builds/cc13x2_cc26x2/release

CC = "$(CCS_ARMCOMPILER)/bin/armcl"
LNK = "$(CCS_ARMCOMPILER)/bin/armcl"

OBJECTS = log.obj freertos_main_freertos.obj ti_utils_osal_config.obj ti_utils_runtime_config.obj ti_loggers_utils_config.obj ti_loggers_drivers_config.obj ti_drivers_config.obj ti_devices_config.obj

NAME = log

CFLAGS = -I../.. \
    -I. \
    -Dti_utils_runtime_Log_ENABLE_PPO \
    -Dti_utils_runtime_Log_ENABLE_INFO1 \
    -Dti_utils_runtime_Log_ENABLE_INFO2 \
    -Dti_utils_runtime_Log_ENABLE_Log_ERROR \
    -DDeviceFamily_CC26X2 \
    "-I$(SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/source" \
    "-I$(SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/source/ti/posix/ccs" \
    "-I$(FREERTOS_INSTALL_DIR)/FreeRTOS/Source/include" \
    "-I$(FREERTOS_INSTALL_DIR)/FreeRTOS/Source/portable/CCS/ARM_CM4F" \
    "-I$(KERNEL_BUILD)" \
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

LFLAGS = "-i$(SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/source" \
    --diag_suppress=10063 \
    ti_utils_build_linker.cmd.genlibs \
    "-l$(KERNEL_BUILD)/ccs/freertos.lib" \
    -lti/devices/cc13x2_cc26x2/driverlib/bin/ccs/driverlib.lib \
    ../../freertos/ccs/cc13x2_cc26x2_freertos.cmd \
    "-m$(NAME).map" \
    --warn_sections \
    --display_error_number \
    --diag_wrap=off \
    --rom_model \
    "-i$(CCS_ARMCOMPILER)/lib" \
    -llibc.a

all: $(NAME).out

$(KERNEL_BUILD)/ccs/freertos.lib:
	@ $(ECHOBLANKLINE)
	@ echo $@ is not built.
	@ echo You can build it by issuing $(MAKE) in $(dir $@).
	@ $(ECHOBLANKLINE)

.INTERMEDIATE: syscfg
ti_utils_osal_config.c ti_utils_runtime_config.c ti_loggers_utils_config.c ti_loggers_drivers_config.c ti_drivers_config.c ti_devices_config.c ti_utils_build_linker.cmd.genlibs ti_utils_osal_config.h ti_utils_runtime_config.h ti_loggers_utils_config.h ti_loggers_drivers_config.h ti_drivers_config.h: syscfg
	@ echo generation complete

syscfg: ../../freertos/log_freertos.syscfg
	@ echo Generating configuration files...
	@ $(SYSCONFIG_TOOL) --compiler ccs --product $(SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/.metadata/product.json --output $(@D) $<

log.obj: ../../log.c ti_utils_osal_config.h ti_utils_runtime_config.h ti_loggers_utils_config.h ti_loggers_drivers_config.h ti_drivers_config.h
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< --output_file=$@

freertos_main_freertos.obj: ../../freertos/main_freertos.c ti_utils_osal_config.h ti_utils_runtime_config.h ti_loggers_utils_config.h ti_loggers_drivers_config.h ti_drivers_config.h
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< --output_file=$@

ti_utils_osal_config.obj: ti_utils_osal_config.c ti_utils_osal_config.h ti_utils_runtime_config.h ti_loggers_utils_config.h ti_loggers_drivers_config.h ti_drivers_config.h
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< --output_file=$@

ti_utils_runtime_config.obj: ti_utils_runtime_config.c ti_utils_osal_config.h ti_utils_runtime_config.h ti_loggers_utils_config.h ti_loggers_drivers_config.h ti_drivers_config.h
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< --output_file=$@

ti_loggers_utils_config.obj: ti_loggers_utils_config.c ti_utils_osal_config.h ti_utils_runtime_config.h ti_loggers_utils_config.h ti_loggers_drivers_config.h ti_drivers_config.h
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< --output_file=$@

ti_loggers_drivers_config.obj: ti_loggers_drivers_config.c ti_utils_osal_config.h ti_utils_runtime_config.h ti_loggers_utils_config.h ti_loggers_drivers_config.h ti_drivers_config.h
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< --output_file=$@

ti_drivers_config.obj: ti_drivers_config.c ti_utils_osal_config.h ti_utils_runtime_config.h ti_loggers_utils_config.h ti_loggers_drivers_config.h ti_drivers_config.h
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< --output_file=$@

ti_devices_config.obj: ti_devices_config.c ti_utils_osal_config.h ti_utils_runtime_config.h ti_loggers_utils_config.h ti_loggers_drivers_config.h ti_drivers_config.h
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< --output_file=$@

$(NAME).out: $(OBJECTS) $(KERNEL_BUILD)/ccs/freertos.lib
	@ echo linking $@
	@ $(LNK) -z $(OBJECTS)  $(LFLAGS) -o $(NAME).out

clean:
	@ echo Cleaning...
	@ $(RM) $(OBJECTS) > $(DEVNULL) 2>&1
	@ $(RM) $(NAME).out > $(DEVNULL) 2>&1
	@ $(RM) $(NAME).map > $(DEVNULL) 2>&1
	@ $(RM) ti_utils_osal_config.c ti_utils_runtime_config.c ti_loggers_utils_config.c ti_loggers_drivers_config.c ti_drivers_config.c ti_devices_config.c ti_utils_build_linker.cmd.genlibs ti_utils_osal_config.h ti_utils_runtime_config.h ti_loggers_utils_config.h ti_loggers_drivers_config.h ti_drivers_config.h > $(DEVNULL) 2>&1
