# Host Test

## Table of Contents

* [Introduction](#Introduction)
* [Hardware Prerequisites](#Hardware Prerequisites)
* [Software Prerequisites](#Software Prerequisites)
* [Usage](#Usage)

## <a name="Introduction"></a>Introduction

The `host_test` project implements a pure Bluetooth low energy network
processor for use with an external microcontroller or a PC software
application such as BTool (provided with the SDK). Host Test supports all GAP roles: Central, Peripheral,
Broadcaster and Observer in addition to multi-role combinations of these roles.
A network processor incorporates the TI BLE Host and Controller components of the
protocol stack on the wireless MCU with the Bluetooth application residing on
the external microcontroller. Communication with the network processor occurs
through a serial interface (UART or SPI) using the Host Controller Interface
(HCI) protocol. The external application uses TI Vendor Specific HCI commands
and a limited subset of Bluetooth LE HCI commands/events to implement a
Bluetooth application. By placing the Host and Controller on the wireless MCU,
the memory requirements on the application processor are reduced since only the
application is required.

Note: It is not possible to use an external Bluetooth Host stack, such as BlueZ,
with Host Test since the network processor implements the Host component.
External applications must use the TI Vendor Specific HCI commands for
implementing GAP, GATT, Pairing/Bonding, etc., functionality.

This project has the following app configurations available:

|App Build Configuration         | Description                                                       |
|--------------------------------|-------------------------------------------------------------------|
|FlashROM_Release (default)      | All TI-RTOS debug features disabled. <br> The application uses the config file `ble_release.cfg` and the defines are in `<app name>_FlashROM_Release.opt`.|
|FlashROM_Debug                  | All TI-RTOS debug features enabled. <br> The application uses the config file `ble_debug.cfg` and the defines are in `<app name>_FlashROM_Debug.opt`.|
|FlashROM_RCOSC_Release          | This is for a 32-kHz crystal-less device configuration. <br> All TI-RTOS debug features disabled. <br> The application uses the config file `ble_release.cfg` and the defines are in `<app name>_FlashROM_Release.opt`. <br> This configuration is further discussed in the RCOSC section (Using 32-kHz Crystal-Less Mode) of the **BLE5-Stack User's Guide**.|

All application configurations use the stack library configuration,
FlashROM_Library. This build configuration will yield additional flash footprint
optimizations by the linker since the application and stack can share
contiguous flash pages. This configuration is further discussed in the Stack
Library Configuration section of the **BLE5-Stack User's Guide** provided in the SDK.

The Simple NP (Simple Network Processor) sample application is another project
that acts as a network processor. SimpleNP allows the application processor to
be responsible for maintaining fewer aspects of the BLE stack. Note, though,
that SimpleNP can only function as a peripheral or broadcaster device.

## <a name="Hardware Prerequisites"></a>Hardware Prerequisites

The default Host Test board configuration uses the
device's LaunchPad development kit. This hardware
configuration is shown in the below image:

<img src="resource/hardware_setup.jpg"/>

For custom hardware, see the Running the SDK on Custom Boards section of the
**BLE5-Stack User's Guide**.

## <a name="Software Prerequisites"></a>Software Prerequisites

For information on what versions of Code Composer Studio and IAR Embedded
Workbench to use, see the Release Notes file provided in the SDK. For
information on how to import this project into your IDE workspace and
build/run, please refer to the device's Platform section in the **BLE5-Stack User's Guide**.


## <a name="Usage"></a>Usage

This application uses the UART peripheral without `POWER_SAVING` by
default to communicate with the host processor. The project also
supports UART and SPI interfaces with power management enabled.

If `POWER_SAVING` is not defined and UART is being used, the MRDY and SRDY pins
are defined by default as in the table below. MRDY
and SRDY are configurable GPIO pins. They can be changed and set by the
application. They are set in `npi_config.h`. Note that MRDY and SRDY are required
to use SPI to allow for full duplex communication, regardless of whether power
management is enabled.

The following table includes the default pins for serial communication in this
project:

PIN          | LaunchPad Pins
-------------|----------------
MRDY         | IOID_13
SRDY         | IOID_14
UART_RX      | IOID_2
UART_TX      | IOID_3
SPI_MISO     | IOID_8
SPI_MOSI     | IOID_9
SPI_CLK      | IOID_10
SPI_FLASH_CS | IOID_20

See the [**NPI wiki**](http://processors.wiki.ti.com/index.php/NPI) for more
information on power management and setting the necessary pins.

Host Test can be used with an external microcontroller or a PC application such
as BTool (provided with the SDK) or
[**HCI Tester**](http://processors.wiki.ti.com/index.php/LPRF_BLE_HCITester).

Information on HCI and details and descriptions of available HCI commands can be
found in the TI BLE Vendor Specific HCI Guide (provided with the SDK).

**Note:** Host Test does not automatically respond to Connection Parameter Update Requests
and instead allows the application to specify the response parameters. Responses
must manually be sent within 40 seconds of reception of the
`GAP_LinkParamUpdateRequest` event (0x0612) using
`HCI_LERemoteConnectionParameterRequestReply` or
`HCI_LERemoteConnectionParameterRequestNegativeReply`.
You must do this to avoid a connection timeout.
See the log below for an example of handling this request using BTool:

<img src="resource/Btool_log.png"  width="600" height="480"/>