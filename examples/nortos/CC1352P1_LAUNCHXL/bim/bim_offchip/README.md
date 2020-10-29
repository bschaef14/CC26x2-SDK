# Boot Image manager for OAD Off-chip

## Table of Contents

* [Introduction](#Introduction)
* [Hardware Prerequisites](#HardwarePrerequisites)
* [Software Prerequisites](#SoftwarePrerequisites)
* [Usage](#Usage)

## <a name="Introduction"></a>Introduction

The Boot Image manager (BIM) project demonstrates how to select, copy and run an
image located in the internal flash or the external flash.

This project is independant of any protocol stack.

## <a name="HardwarePrerequisites"></a>Hardware Prerequisites

The default configuration uses the
[LAUNCHXL-CC1352R1](http://www.ti.com/tool/launchxl-cc1352r1).

For custom hardware, see the **Running the SDK on Custom Hardware** section of
the [BLE5-Stack User's
Guide](../../../../../docs/ble5stack/ble_user_guide/ble5stack-users-guide.html).

## <a name="SoftwarePrerequisites"></a>Software Prerequisites

For information on what versions of Code Composer Studio and IAR Embedded
Workbench to use, see the dependencies section of the Release Notes located
in the root folder of the SDK.

For information on how to import this project into your IDE workspace and
build/run, please refer to the device's Platform section in the [BLE5-Stack
Quick Start
Guide](../../../../../docs/simplelink_mcu_sdk/html/quickstart-guide/ble5-quick-start.html).

## <a name="Usage"></a>Usage

**Note: The ``bim_offchip`` project must be flashed to the device when
using off-chip OAD or the device will not function correctly**

This project is independent of any protocol stack.

Four build configurations are provided for on-chip BIM:

* Release_unsecure
* Debug_unsecure
* Release
* Debug

Release_unsecure is to be flashed when flashing the user application .bin and doing unsecure OAD
i.e. NO image authentication is done before executing the newly trasferred (OAD) image
Release is to be used when doing secure OAD i.e. Image authentication needs to be done before
executing the newly tranferred image.

Debug versions are provided for debugging purposes i.e. when the application image is
dowloaded using the debugger in the IDE. In this mode, the post build step of calculating the CRC
or signing of the image is not done and hence, the sign or CRC is not available in the header
of the image downloaded to internal flash with the debugger.
Additionally, in Debug configurations, LED indications are available
for visual feedback.

In Release version, when no valid image is found in either external flash or
internal flash, BIM will try booting to factory Image, if present.

FLASH_DEVICE_ERASE:
This is a macro provided in the BIM, which when enabled and built, this special image
can be used to erase external flash and remove any remnanents of images/data from previous
usage. If additionally, DEBUG macro is enabled, LEDs are flashed during the process of external
flash erase.

**Note: This MACRO is NEVER meant to be used for a regular OAD case but for this specific purpose
of having a special image for erasing exeternal flash.**

In other words, never enable the macro "FLASH_DEVICE_ERASE" in BIM if the device needs to boot to
new image after doing a successful OAD. Enabling this macro would mean that the newly downloaded OAD
Image will get erased from External Flash and then the BIM will only boot to the already existent
(older) Internal Flash image.