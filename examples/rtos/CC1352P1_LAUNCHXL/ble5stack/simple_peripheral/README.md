# Simple Peripheral

## Table of Contents

* [Introduction](#Introduction)
* [Hardware Prerequisites](#Hardware Prerequisites)
* [Software Prerequisites](#Software Prerequisites)
* [Service/Profile Table](#Service/Profile Table)
* [Usage](#Usage)
    * [Characteristic 1 (UUID: FFF1)](#char1)
    * [Characteristic 2 (UUID: FFF2)](#char2)
    * [Characteristic 3 (UUID: FFF3)](#char3)
    * [Characteristic 4 (UUID: FFF4)](#char4)
    * [Characteristic 5 (UUID: FFF5)](#char5)

## <a name="Introduction"></a>Introduction

The `simple_peripheral` project implements a simple Bluetooth low energy
peripheral device with GATT services and demonstrates the TI Simple
Profile. This project can be a framework for developing many different
peripheral-role applications. The Simple Peripheral project is used as the
baseline for explaining the stack in the **BLE5-Stack User's Guide**.

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

The default Simple Peripheral board configuration uses the
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

This project contains 1 service, which has 5 characteristics.

Note that BLE Scanner may report 128 bit UUIDs, but the project is generating
16 bit UUIDs.


Characterisitic | UUID | Format         | Properties | Profile Source
----------------|------|----------------|------------|-----------------------------------
1               | FFF1 | 1 Byte Integer | Read/Write | simple_peripheral.c/simple_gatt_profile.c
2               | FFF2 | 1 Byte Integer | Read       | simple_peripheral.c/simple_gatt_profile.c
3               | FFF3 | 1 Byte Integer | Write      | simple_peripheral.c/simple_gatt_profile.c
4               | FFF4 | 1 Byte Integer | Notify     | simple_peripheral.c/simple_gatt_profile.c
5               | FFF5 | 5 Byte Integer | Read       | simple_peripheral.c/simple_gatt_profile.c

## <a name="Usage"></a>Usage

This application uses the UART peripheral to provide an
interface for the application. The UART is only used for display messages.

This document will use a terminal program (Tera Term, PuTTY, etc.) to serve as
the display for the output of the
LaunchPad, and it will use a BLE phone app, such as BLE Scanner (Android)
or LightBlue Explorer (iOS), that will act as the central device that will connect
to the Simple Peripheral device. This document will be using Tera Term and BLE Scanner.
Note that any other serial terminal
and smartphone BLE application can be used. The
following default parameters are used for the UART peripheral for display:

  UART Param     |Default Values
  -------------- |----------------
  Baud Rate      |115200
  Data length    |8 bits
  Parity         |None
  Stop bits      |1 bit
  Flow Control   |None

Once the Simple Peripheral sample application starts, the output to the terminal
will report it's address and the fact that it is advertising. further more there is an option to set the peripheral to be a AutoConnect node as shown below:

<img src="resource/sbp_advertising.png"/>
Advertising is enabled by default upon startup, so there is no need to
physically press a button to start advertising.
if `AutoConnect` selected, the user will have to choose which group to be associated with Group A , Group B or Disable (none as default).
Selecting a specific group will change the value of the  Shortened local name (opcode 0x07) in the adv data. Group A will be set to `GA`  Group B will be set to `GB` and the default if not setting any group or choosing Disable will be set to `SP`.
<img src="resource/sbp_advertising_group.png"/>

Using BLE Scanner to scan for the
device, you should see a device with the name of SimplePeripheral (note that
the navigation might be slightly different depending on your mobile
application):

<img src="resource/sbp_figure2.png" width="300" height="533" />

If there is more than 1 device called SimplePeripheral, the RSSI value in the
colored circles on the left can be used to find which device you are trying to
connect to. Press the "connect" button to connect to the device. Once connected,
the terminal window will show the address of the connected device (smartphone):

<img src="resource/sbp_connected.png"/>

After a connection is formed, you will also have the option to switch which physical
layer (PHY) preference is used. Use the left button (BTN-1) to move to the next option and
the right button (BTN-2) to select an option. For more information on the
differences between the PHY options, see the PHY section of
the **BLE5-Stack User's Guide**.

<img src="resource/sbp_PhyExample.png"/>

In BLE Scanner, you should be able to see various information about the Simple
Peripheral device:

<img src="resource/sbp_figure4.png" width="300" height="533" />

You should see 1 service, which is labeled as "CUSTOM SERVICE" (this may be
slightly different for other smartphone applications). It will also provide its
UUID. Clicking on this service will display the 5 characteristics that it
contains, with information about each characteristic:

<img src="resource/sbp_figure5.png" width="300" height="533" />

Scrolling down should show all 5 characteristics. The following sections detail
the functions and operations of each of the characteristics.

### <a name="char1"></a>Characteristic 1 (UUID: FFF1)

This characteristic demonstrates both read and write properties, and it contains
a 1-byte-long value. As shown below, there are 2 circles, one with an "R" and
one with a "W", next to "CUSTOM CHARACTERISTIC":

<img src="resource/sbp_figure6.png" width="300" height="533" />

These are the buttons used for read and write operations. The value for this
characteristic is set to "01" by default, and it can be changed by pressing the
write button. Upon pressing the write button, you will see the following menu:

<img src="resource/sbp_figure7.png" width="300" height="533" />

To write a new byte value to it, click on "Text" to change it to "Byte Array",
and then enter the value that you want to write to the characteristic (in
hexadecimal) in the field below. Press ok, and then you will be able to see that
the value has changed by looking at the terminal window. The example below shows
a write of `0x42` which is decimal for 66:

<img src="resource/sbp_figure8.png" />

Note that the terminal window will print the value in decimal. The value will
also be updated in BLE Scanner:

<img src="resource/sbp_figure9.png" width="300" height="533" />

The read button can be used to read a value. If the Simple Peripheral device has
changed the value of the characteristic (i.e. by using a button press), then
pressing the read button will show the updated value in BLE scanner as shown
above.

There is also a circle with an "R" next to "Characteristic User Description".
Pressing this will show the name of the characteristic, as shown below:

<img src="resource/sbp_figure10.png" width="300" height="533" />

### <a name="char2"></a>Characteristic 2 (UUID: FFF2)

This characteristic has read property only. Its value can be read following
the same procedure as is detailed for characteristic 1. It has a default value
of 0x02.

### <a name="char3"></a>Characteristic 3 (UUID: FFF3)

This characteristic has write property only. Its value can be written to
following the same procedure as is detailed for characteristic 1. It has a
default value of 0x03.

### <a name="char4"></a>Characteristic 4 (UUID: FFF4)

This characteristic has notify property only. It is a single byte, and it is set
to whatever value is contained in characteristic 3 (this value can be changed by
writing to characteristic 3). Next to "CUSTOM CHARACTERISTIC" there is a circle
with an "N". Pressing this button will enable notifications, and this
characteristic will be updated and will display the value contained in
characteristic 3:

<img src="resource/sbp_figure11.png" width="300" height="533" />

Before pressing the notify button, the read button next to "Client
Characteristic Configuration" can be pressed to show whether notifications (or
indications) have been enabled:

<img src="resource/sbp_figure12.png" width="300" height="533" />

Once notifications have been enabled, this will be displayed:

<img src="resource/sbp_figure13.png" width="300" height="533" />

### <a name="char5"></a>Characteristic 5 (UUID: FFF5)

This characteristic has read property only. It is 5 bytes, and its default value
is 0x0102030405. It differs from the other characteristics in that it requires
authentication. This is done by pairing the smartphone with the Simple
Peripheral device. The process of doing this varies depending on the smartphone
application and the operating system that is being used. Please see
documentation on the operating system that you are using for more details. Some
documentation for iOS can be found
[**here**](https://support.apple.com/en-us/HT204091), and some for Android can
be found [**here**](https://support.google.com/nexus/answer/2819579?hl=en). If
the Simple Peripheral device and the smartphone have not been paired, you will
not see anything when you press the read button (this includes reading the
characteristic user description).

When prompted for a passcode on the phone to
pair, the passcode will be displayed on the terminal:
<img src="resource/sbp_passcode.png" />

Once the devices are paired, pressing the read
button will show the characteristic's value as shown below:

<img src="resource/sbp_figure14.png" width="300" height="533" />
