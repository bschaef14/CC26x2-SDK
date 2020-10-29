# Simple Peripheral OAD Off-chip

## Table of Contents

* [Introduction](#Introduction)
* [Hardware Prerequisites](#Hardware Prerequisites)
* [Software Prerequisites](#Software Prerequisites)
* [Service/Profile Table](#Service/Profile Table)
* [Usage](#Usage)

## <a name="Introduction"></a>Introduction

The `simple_peripheral_oad_offchip` project demonstrates off-chip OAD
functionality with the `simple_peripheral` application. The application behavior
is the same as the stock `simple_peripheral` project,
with the exception that OAD is added.
For guidance on this out of the box demo for OAD refer to the Off-chip OAD
section of the **BLE5-Stack User's Guide**.

**Note: The device's BIM project included in the workspace must also be flashed
to the device when using off-chip OAD or the device will not function
correctly. Please refer to the information on BIM build configuration for more details**

This project has the following app configurations available:

|App Build Configuration         | Description                                                       |
|--------------------------------|-------------------------------------------------------------------|
|FlashROM_Release (default)      | All TI-RTOS debug features disabled but application-logging to UART remains enabled. <br> The application uses the config file `ble_release.cfg` and the defines are in `<app name>_FlashROM_Release.opt`.|
|FlashROM_Debug                  | All TI-RTOS debug features enabled. <br> The application uses the config file `ble_debug.cfg` and the defines are in `<app name>_FlashROM_Debug.opt`.|
|FlashROM_Release_unsecure       | All TI-RTOS debug features disabled but application-logging to UART remains enabled. <br> The application uses the config file `ble_release.cfg` and the defines are in `<app name>_FlashROM_Release_unsecure.opt`.|
|FlashROM_Debug_unsecure         | All TI-RTOS debug features enabled. <br> The application uses the config file `ble_debug.cfg` and the defines are in `<app name>_FlashROM_Debug_unsecure.opt`.|

The debug configuration of the application additionally enables the OAD debug
mode. The OAD debug mode can be entered via the two button menu and offers the
following functionality:

  * Erase User images in external flash (except factory image)
  * Erase entire external flash
  * Erase image by index
  * Print external flash image information summary
  * Print detailed external flash image information by index
  * Create factory image from active image

The difference between the secure (Release/Debug) and the unsecure variant is the
addition of security related features in the OAD process i.e. authentication of the
OAD image and authentication of the some of the OAD commands from the distributor.

All application configurations use the stack library configuration,
FlashROM_Library. This build configuration will yield additional flash footprint
optimizations by the linker since the application and stack can share
contiguous flash pages. This configuration is further discussed in the Stack
Library Configuration section of the **BLE5-Stack User's Guide** provided in
the SDK.

This document seeks to extend the `simple_peripheral` project readme
documentation that is already in place, and explain how `simple_peripheral` can
be used to demonstrate off-chip OAD. For an advanced discussion of OAD, and
an in-depth explanation of this project, please see the Off-chip OAD User's
Guide section in the **BLE5-Stack User's Guide**.

The BIM project (included in the workspace) has following build configuartions available:
|BIM project Build Configuration | Description                                                       |
|--------------------------------|-------------------------------------------------------------------|
|Release (default)               | OAD security related features enabled and debug features disabled. <br> This configuration uses the pre-defined symbols from cc26x2r1lp_bim_offchip_Release.opt|
|Debug                           | OAD security related features enabled and debug features disabled. <br> This configuration uses the pre-defined symbols from cc26x2r1lp_bim_offchip_Debug.opt|
|Release_unsecure                | OAD security related features disabled and debug features disabled. <br> This configuration uses the pre-defined symbols from cc26x2r1lp_bim_offchip_Release_unsecure.opt|
|Debug_unsecure                  | OAD security related features disabled and debug features enabled. <br> This configuration uses the pre-defined symbols from cc26x2r1lp_bim_offchip_Debug_unsecure.opt|

Debug features in BIM module include LED on/off indications and also provisions for downloading the .hex image of the aaplication and debugging through JTAG.
Always use the same combination of features in the BIM with that of the application. For example, if the application uses "FlashROM_Debug_unsecure", the BIM must be built with "Debug_unsecure" configuration.

Below table captures the description of MACROS used in BIM project

|BIM pre-defined symbols | Description                                                       |
|------------------------|-------------------------------------------------------------------|
|SECURITY                | Enable OAD security related features i.e. Image authentication and
authentication of certain OAD related commands from distributor|
|FLASH_DEVICE_ERASE      | Enable this macro and build BIM to get an image that can clear all the content in the external flash.
This macro should never be enabled in the BIM project when OAD needs to be done.
|LED_DEBUG               | Enable visual indication when errors occur in the BIM module. This also enables the toggling of LED when external flash is being erased by the special BIM image generated by enabling the FLASH_DEVICE_ERASE macro.
|JTAG_DEBUG              | Enables the JTAG debug of the OAD process in the BIM context. When the .hex file of the application is downloaded into the internal flash using the debugger, it does not have the length of the image and the CRC computed. When this macro is enabled, for the image in the internal flash, the length of the image is calculated and populated by BIM code. Also, the CRC check is skipped on this image. Similarly, the BIM also calculates and updates the length of the image in the external flash (if present) when this macro is enabled.|


## <a name="Hardware Prerequisites"></a>Hardware Prerequisites

The default Simple Peripheral OAD-offchip board configuration uses the
device's LaunchPad development kit. This hardware
configuration is shown in the below image:

<img src="resource/hardware_setup.jpg"/>

For custom hardware, see the Running the SDK on Custom Boards section of the
**BLE5-Stack User's Guide**.

## <a name="Software Prerequisites"></a>Software Prerequisites

For information on what versions of Code Composer Studio and IAR Embedded
Workbench to use, see the Release Notes file provided in the SDK. For
information on how to import this project into your IDE workspace and
build/run, please refer to the device's Platform section in the
**BLE5-Stack User's Guide**.

## <a name="Service/Profile Table"></a>Service/Profile Table

This project contains the simple service, which has 5 characteristics.

Note that BLE Scanner may report 128 bit UUIDs, but the project is generating
16 bit UUIDs.


Characterisitic | UUID | Format         | Properties | Profile Source
----------------|------|----------------|------------|----------------------
1               | FFF1 | 1 Byte Integer | Read/Write | `simple_gatt_profile.c`
2               | FFF2 | 1 Byte Integer | Read       | `simple_gatt_profile.c`
3               | FFF3 | 1 Byte Integer | Write      | `simple_gatt_profile.c`
4               | FFF4 | 1 Byte Integer | Notify     | `simple_gatt_profile.c`
5               | FFF5 | 5 Byte Integer | Read       | `simple_gatt_profile.c`

Additionally, this project contains the OAD profile, which has 5
characteristics.

See OAD Service Description in the **BLE5-Stack User's Guide** for more
information on the OAD service


## <a name="Usage"></a>Usage

This application uses the UART peripheral to provide an
interface for the application. The UART is only used for display messages.

This document will use a terminal program (Tera Term, PuTTY, etc.) to serve as
the display for the output of the
LaunchPad, and it will use a BLE phone app, such as BLE Scanner (Android)
or LightBlue Explorer (iOS), that will act as the central device that will
connect
to the Simple Peripheral device. This document will be using Tera Term and
BLE Scanner. Note that any other serial terminal and smart-phone BLE application
can be used. The following default parameters are used for the UART peripheral
for display:

  UART Param     |Default Values
  -------------- |----------------
  Baud Rate      |115200
  Data length    |8 bits
  Parity         |None
  Stop bits      |1 bit
  Flow Control   |None
