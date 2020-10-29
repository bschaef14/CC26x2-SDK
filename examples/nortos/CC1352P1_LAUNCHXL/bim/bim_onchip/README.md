# Boot Image manager for OAD On-chip

## Table of Contents

* [Introduction](#Introduction)
* [Hardware Prerequisites](#HardwarePrerequisites)
* [Software Prerequisites](#SoftwarePrerequisites)
* [Usage](#Usage)

## <a name="Introduction"></a>Introduction

The Boot Image manager (BIM) project demonstrates how to select, copy and run an
image located in the internal flash.

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

**Note: The ``bim_onchip`` project must be flashed to the device when
using on-chip OAD or the device will not function correctly, after flashing the
persistent application and the user application images, in the same order that is
mentioned here**

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