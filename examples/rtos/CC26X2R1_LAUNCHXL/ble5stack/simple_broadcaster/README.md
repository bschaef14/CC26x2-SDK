# Simple Broadcaster

## Table of Contents

* [Introduction](#Introduction)
* [Hardware Prerequisites](#Hardware Prerequisites)
* [Software Prerequisites](#Software Prerequisites)
* [Usage](#Usage)

## <a name="Introduction"></a>Introduction

The `simple_broadcaster` project demonstrates non-connectable
beacon applications, such as Apple iBeacon™ and Google Eddystone™. See
Bluetooth low energy Beacons Application Note
([SWRA475](http://www.ti.com/lit/pdf/swra475)) for more information
about Bluetooth low energy beacons.

This project has the following app configurations available:

|App Build Configuration         | Description                                                       |
|--------------------------------|-------------------------------------------------------------------|
|FlashROM_Release (default)      | All TI-RTOS debug features disabled but application-logging to UART remains enabled. <br> The application uses the config file `ble_release.cfg` and the defines are in `<app name>_FlashROM_Release.opt`.|
|FlashROM_Debug                  | All TI-RTOS debug features enabled. <br> The application uses the config file `ble_debug.cfg` and the defines are in `<app name>_FlashROM_Debug.opt`.|
|FlashROM_RCOSC_Release          | This is for a 32-kHz crystal-less device configuration. <br> All TI-RTOS debug features disabled but application-logging to UART remains enabled.<br> The application uses the config file `ble_release.cfg` and the defines are in `<app name>_FlashROM_Release.opt`. <br> This configuration is further discussed in the RCOSC section (Using 32-kHz Crystal-Less Mode) of the **BLE5-Stack User's Guide**.|

All application configurations use the stack library configuration,
FlashROM_Library. This build configuration will yield additional flash footprint
optimizations by the linker since the application and stack can share
contiguous flash pages. This configuration is further discussed in the Stack
Library Configuration section of the **BLE5-Stack User's Guide** provided in the SDK.

## <a name="Hardware Prerequisites"></a>Hardware Prerequisites

The default Simple Broadcaster board configuration uses the
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

## <a name="Service/Profile Table"></a>Service/Profile Table

## <a name="Usage"></a>Usage

This application uses the UART peripheral to display messages. The UART is only
used to display messages.

This application is configured to broadcast sample advertising data out of the
box. It sends non-connectable advertisements.

This document will use PuTTY to serve as the display for the output of the
LaunchPad, and it will use BLE Scanner as the phone app that will scan
the device to show that it is advertising. Note that any other serial terminal
and smart phone BLE application can be used. The following default parameters
are used for the UART peripheral for display:

  UART Param     |Default Values
  -------------- |----------------
  Baud Rate      |115200
  Data length    |8 bits
  Parity         |None
  Stop bits      |1 bit
  Flow Control   |None

Once the Simple Broadcaster sample application starts, the output to the terminal
will report its address and the fact that it is advertising, as shown below:

<img src="resource/sbb_figure1.png" width="637" height="400" />

Advertising is enabled by default upon startup, so there is no need to
physically press a button to start advertising. Using BLE Scanner to scan for the
device, you should see a device with the name of SimpleBLEBroadcaster (note that
the navigation might be slightly different depending on your mobile
application):

<img src="resource/sbb_figure2.png" width="300" height="533" />

If there is more than 1 device called SimpleBLEBroadcaster, the RSSI value in
the colored circles on the left can be used to find which device you are looking
for. The device name is being sent from the Simple Broadcaster device in the
scan response data that is sent when the device is scanned by BLE Scanner. The
Simple Broadcaster device is sending out non-connectable advertisements, so it
cannot be connected to by the smartphone application (or any other central
device). Pressing "connect" in BLE Scanner will not do anything.

TI's [**SmartRF Protocol Packet Sniffer**](http://www.ti.com/tool/packet-sniffer)
can be used to see the advertising data that is being broadcasted:

<img src="resource/sbb_figure3.png" />

The information in the "AdvData" field shows the advertising data that is being
broadcasted. This data can be changed by making changes to advertData in
simple_broadcaster.c. Also, note that defining BEACON\_FEATURE in the project
will change the data that is being advertised.

The information in the "AdvA" field shows the address of the Simple Broadcaster
device. This will match what was shown in BLE Scanner.

The information in the "Adv PDU Type" shows that the advertisements are
discoverable and non-connectable.
