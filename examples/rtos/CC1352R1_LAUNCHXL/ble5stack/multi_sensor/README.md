# Multi Sensor

## Table of Contents

* [Introduction](#Introduction)
* [Hardware Prerequisites](#Hardware-Prerequisites)
* [Software Prerequisites](#Software-Prerequisites)
* [Usage](#Usage)
    * [UART Log](#UART-Log)
    * [Mobile App](#Mobile-App)
    * [Booting](#Booting)
    * [Advertising](#Advertising)
    * [Connecting](#Connecting)
* [GATT Services](#GATT-Services)
    * [Mandatory BLE Services](Mandatory-BLE-Services)
    * [OAD Service](OAD-Service)
    * [Temperature Service](Temperature-Service)
    * [Humidity Service](Humidity-Service)
    * [Light Service](Light-Service)
    * [LED Service](LED-Service)
    * [Button Service](Button-Service)
    * [Battery Service](Battery-Service)
    * [Accelerometer Service](Accelerometer-Service)
    * [Hall Service](Hall-Service)
* [Known Limitations](#Known-Limitations)

## Introduction

The `multi_sensor` project demonstrates the capabilities of the
`Launchpad Sensortag (LPSTK)` development kit functioning as a Bluetooth Low
Energy (BLE) peripheral device. It includes GATT services for each sensor that
can be interacted with via a mobile application. This project can be a framework
for developing many different peripheral-role applications on the LPSTK.

## Hardware Prerequisites

### Minimal Setup

This project uses the LPSTK development kit. The LPSTK comes shipped with the
multi_sensor project loaded onto the device. This is the minimum hardware required
to run the project. To do so, just power on the device. Note that it is still
possible to update the firmware by performing and `Over-the-Air Download (OAD)`. See the
[OAD Service](#OAD-Service).

### Additional Configuration

The LPSTK can be connected to a Launchpad for programming new firmware via JTAG and / or
accessing the application's UART log. This can be done by making the following
connections:

 Launchpad   |  LPSTK
-------------|----------------------------------
 XDS110 Out  | JTAG (Use ARM 10-pin Debug cable)
 TXD         | 13 (TX)

This is pictured here:

<img src="resource/connections.jpg" alt="drawing" width="500"/>

## Software Prerequisites

Multi_sensor provides the capability to perform an off-chip OAD to update the firmware.
Therefore, it is dependent on a `Boot Image Manager (BIM)` loaded in the last page of
flash. By default, this is an "secure" BIM.  For more information
on the OAD / BIM environment see the OAD section of the **BLE5-Stack User's Guide**.

For information on what versions of Code Composer Studio and IAR Embedded
Workbench to use, see the Release Notes file provided in the SDK. For
information on how to import this project into your IDE workspace and
build/run, please refer to the device's Platform section in the **BLE5-Stack User's Guide**.

## Usage

### UART Log

This application uses the UART peripheral to provide an interface for the
application. The UART is only used for display messages. Any documentation
referencing the UART log is only applicable if the TX pin has been connected as
stated in [Additional Configuration](#Additional-Configuration)

We recommend that you use a terminal program that can parse ANSI/VT100 color
codes such TeraTerm, PuTTY, Code Composer Studio Terminal, etc. to to monitor
the LaunchPad UART output.

The following default parameters are used for the UART peripheral for
display:

  UART Param     |Default Values
  -------------- |----------------
  Baud Rate      |115200
  Data length    |8 bits
  Parity         |None
  Stop bits      |1 bit
  Flow Control   |None

### Mobile App

To interact with the application over Bluetooth you may use any device that is
able to act as a BLE Central device, such as:

* iOS with the SimpleLink Starter or LightBlue
* Android with the apps SimpleLink Starter or BLEScanner
* A CC26xx/CC13xx LaunchPad running the sample app HostTest from a compatible
  SDK with the PC tool BTool from the same SDK.

This README will assume that the SimpleLink Starter app is being used. For more
information on interfacing with the multi_sensor's GATT services directly, see
[GATT Services](#GATT-Services).

### Booting

Once the multi_sensor sample application starts, the output to the terminal
will report its address, the status of sensor initialization, and the fact that
it is advertising, as shown below:

**Note: The first boot will take several seconds before the log starts as the
  internal image is being copied to the factory image**

<img src="resource/booting.gif"/>

During boot/application init, the following activities related to the BIM
 and the external flash take place:

* The call to `OAD_open` will check if a factory image exists in external
  flash. If none exists, the running image will be copied to external flash
  and marked as factory image.

* The combination `BTN-1` and `BTN-2` is checked. If both buttons are held
  down for 5 seconds, the external Flash will be erased. This is not recommended
  to do unless the user has capability of flashing a new BIM / multi_sensor image.

* `BTN-1`, the left button, is checked. If it is held down from reset, a 5
  second timeout elapses before the running image marks itself as invalid and
  reboots into BIM. The purpose of this is to make BIM revert to the factory
  image to internal Flash.

While either of the button / button combinations described above are held down
during reset, the LED's will blink alternating colors.

If the buttons are released during the 5 second timeout, the device will continue booting
normally.

After application initialization both buttons will operate normally. That is, they
will be used during a connection for the [Button Service](Button-Service).

**NOTE:** After erasing the external flash it is NOT recommended to perform the
          `BTN-1 + Reset` revert procedure as this leave the Boot Image Manager
          with no valid image in internal Flash and no factory image in external
          Flash. At this point a debugger like the XDS110 must be used
          to download a new internal image to the device.
          Pressing only `Reset` after erasing external Flash will invoke the
          `OAD_open` self-backup functionality on boot and revert will function
          normally again.

### Advertising

After the device has booted, it will begin advertising two advertising sets:
  1. Legacy Connectable Scannable Advertisements
  1. Long-range advertisements on the Coded PHY with S2-coding

The Blue LED will blink to indicate that the device is advertising.

A connection can be formed using either advertising set. Multi_sensor only supports
one connection so both advertising sets will be disabled when a connection is formed.
When the connection drops, both advertising sets will be restarted.

### Connecting

To Connect to the multi_sensor, select the device from the SimpleLink Starter's
list of devices and choose "Sensor View":

<img src="resource/connection_app.gif"/>

When a connection is formed,the Blue led will stop blinking and the Green LED will blink
once to indicate that the connection is formed. After this all LED's will be turned off
so that they can be used by the [LED Service](LED-Service).

When a device connects, the address of the device (i.e. phone) will be shown in the
console. Log messages will then indicate the various sensors as they are enabled and read:

<img src="resource/connection_log.gif"/>

## GATT Services

This section will describe how to interface over-the-air with the various GATT services
available in the multi_sensor project. From an embedded perspective, the interface to these
services is abstracted through the common Sensors module. This module  will automatically
expose GATT services for each sensor as the sensor is initialized successfully at reset. If
a sensor fails to initialize for some reason, the service will not be available. For more
information on the common Sensors module, see the Doxygen documentation of
`$SDK_INSTALL$source\ti\ble5stack\profiles\lpstk\sensor_common.h`.

### Mandatory BLE Services

These services are required by the Bluetooth Core Spec or are required to communicate with
mobile devices. All characteristic information is read-only.

#### Generic Access Profile

| UUID    | Description                                                                  | Value                   | Properties |
|---------|------------------------------------------------------------------------------|-------------------------|------------|
| 0x2800  | GATT Primary Service Declaration (Generic Access Profile)                    | 00:18                   | Read       |
| 0x2803  | GATT Characteristic Declaration (Device Name)                                | 02:03:00:00:2A          | Read       |
| 0x2A00  | Device Name                                                                  | Multi-Sensor            | Read       |
| 0x2803  | GATT Characteristic Declaration (Appearance)                                 | 02:05:00:01:2A          | Read       |
| 0x2A01  | Appearance                                                                   | 00:00                   | Read       |
| 0x2803  | GATT Characteristic Declaration (Peripheral Preferred Connection Parameters) | 02:07:00:04:2A          | Read       |
| 0x2A04  | Peripheral Preferred Connection Parameters                                   | 50:00:A0:00:00:00:E8:03 | Read       |
| 0x2803  | GATT Characteristic Declaration (Central Address Resolution)                 | 02:09:00:A6:2A          | Read       |
| 0x2AA6  | Central Address Resolution                                                   | 01                      | Read       |
| 0x2803  | GATT Characteristic Declaration (Resolvable Private Address Only)            | 02:0B:00:C9:2A          | Read       |
| 0x2AC9  | Resolvable Private Address Only                                              | 00                      | Read       |

#### Generic Attribute Profile

| UUID     | Description                                                  | Value  | Properties |
|--------- |--------------------------------------------------------------|--------|------------|
| 0x2800   | GATT Primary Service Declaration (Generic Attribute Profile) | 00:18  | Read       |

#### Device Information Service

| UUID    | Description                                               | Value                                     | Properties |
|-------- |-----------------------------------------------------------|-------------------------------------------|------------|
| 0x2800  | GATT Primary Service Declaration (Device Information)     | 0A:18                                     | Read       |
| 0x2803  | GATT Characteristic Declaration (System ID)               | 02:0F:00:23:2A                            | Read       |
| 0x2A23  | System ID                                                 | F3:29:2B:00:00:B0:6F:80                   | Read       |
| 0x2803  | GATT Characteristic Declaration (Model Number)            | 02:11:00:24:2A                            | Read       |
| 0x2A24  | Model Number String                                       | Model Number                              | Read       |
| 0x2803  | GATT Characteristic Declaration (Serial Number)           | 02:13:00:25:2A                            | Read       |
| 0x2A25  | Serial Number String                                      | Serial Number                             | Read       |
| 0x2803  | GATT Characteristic Declaration (Firmware Revision)       | 02:15:00:26:2A                            | Read       |
| 0x2A26  | Firmware Revision String                                  | Firmware Revision                         | Read       |
| 0x2803  | GATT Characteristic Declaration (Hardware Revision)       | 02:17:00:27:2A                            | Read       |
| 0x2A27  | Hardware Revision String                                  | Hardware Revision                         | Read       |
| 0x2803  | GATT Characteristic Declaration (Software Revision)       | 02:19:00:28:2A                            | Read       |
| 0x2A28  | Software Revision String                                  | Software Revision                         | Read       |
| 0x2803  | GATT Characteristic Declaration (Manufacturer Name)       | 02:1B:00:29:2A                            | Read       |
| 0x2A29  | Manufacturer Name String                                  | Manufacturer Name                         | Read       |
| 0x2803  | GATT Characteristic Declaration (Certification Data List) | 02:1D:00:2A:2A                            | Read       |
| 0x2A2A  | IEEE 11073-20601 Regulatory Certification Data List       | FE:00:65:78:70:65:72:69:6D:65:6E:74:61:6C | Read       |
| 0x2803  | GATT Characteristic Declaration (PnP ID)                  | 02:1F:00:50:2A                            | Read       |
| 0x2A50  | PnP ID                                                    | 01:0D:00:00:00:10:01                      | Read       |

The SimpleLink Starter app will display the Device Information Service information as such:

<img src="resource/device_info.png" alt="drawing" width="500"/>

### OAD Service

The OAD Service can be used to upload a new image to the multi_sensor. More information
on the OAD Procedure can be found in the OAD Section of the **BLE5-Stack User's Guide**.

| UUID                                   | Description                                      | Value                                                     |  Properties                             |
|--------------------------------------- |--------------------------------------------------|-----------------------------------------------------------|-----------------------------------------|
| 0x2800                                 | GATT Primary Service Declaration (OAD)           | 00:00:00:00:00:00:00:B0:00:40:51:04:C0:FF:00:F0           | Read                                    |
| 0x2803                                 | GATT Characteristic Declaration (Image Identify) | 1C:22:00:00:00:00:00:00:00:00:B0:00:40:51:04:C1:FF:00:F0  | Write without Response / Write / Notify |
| 0xF000FFC104514000B000000000000000     | Image Identify                                   |                                                           | Write without Response / Write / Notify |
| 0x2902                                 | Client Characteristic Configuration              | 00:00                                                     | Read / Write                            |
| 0x2901                                 | Characteristic User Description                  | Img Identify                                              | Read                                    |
| 0x2803                                 | GATT Characteristic Declaration (Image Block)    | 1C:26:00:00:00:00:00:00:00:00:B0:00:40:51:04:C2:FF:00:F0  | Write without Response / Write / Notify |
| 0xF000FFC204514000B000000000000000     | Image Block                                      |                                                           | Write without Response / Write / Notify |
| 0x2902                                 | Client Characteristic Configuration              | 00:00                                                     | Read / Write                            |
| 0x2901                                 | Characteristic User Description                  | Img Block                                                 | Read                                    |
| 0x2803                                 | GATT Characteristic Declaration (Image Control)  | 14:2A:00:00:00:00:00:00:00:00:B0:00:40:51:04:C5:FF:00:F0  | Write without Response / Notify         |
| 0xF000FFC504514000B000000000000000     | Image Control                                    |                                                           | Write without Response / Notify         |
| 0x2902                                 | Client Characteristic Configuration              | 00:00                                                     | Read / Write                            |
| 0x2901                                 | Characteristic User Description                  | OAD Extended Control                                      | Read                                    |

### Usage (with SimpleLink Start App)

First, it is necessary to email a binary (.bin) image so that it can
be opened by the mobile device in the SimpleLink Starter App.
The OAD can then be started as such:

<img src="resource/oad.gif"/>

When the OAD is complete, the app will show:

<img src="resource/oad_complete.jpg" alt="drawing" width="300"/>

### Temperature Service

The Temperature Service uses the HDC2010 Sensor to read the temperature periodically at a period set by
the period characteristic.

#### Attribute Table

| UUID                                   | Description                                     | Value                                                        |  Properties   |
|--------------------------------------- |-------------------------------------------------|--------------------------------------------------------------|---------------|
| 0x2800                                 | GATT Primary Service Declaration (Temperature)  | 00:00:00:00:00:00:00:B0:00:40:51:04:00:AA:00:F0              | Read          |
| 0x2803                                 | GATT Characteristic Declaration (Data)          | 12:2F:00:00:00:00:00:00:00:00:B0:00:40:51:04:01:AA:00:F0     | Read / Notify |
| 0xF000AA0104514000B000000000000000     | Temperature Data                                | 00:00:00:00                                                  | Read / Notify |
| 0x2902                                 | Client Characteristic Configuration             | 00:00                                                        | Read / Write  |
| 0x2901                                 | Characteristic User Description                 | Temperature Data                                             | Read          |
| 0x2803                                 | GATT Characteristic Declaration (Configuration) | 0A:33:00:00:00:00:00:00:00:00:B0:00:40:51:04:02:AA:00:F0     | Read / Write  |
| 0xF000AA0204514000B000000000000000     | Temperature Config                              | 00                                                           | Read / Write  |
| 0x2901                                 | Characteristic User Description                 | Temperature Conf.                                            | Read          |
| 0x2803                                 | GATT Characteristic Declaration (Period)        | 0A:36:00:00:00:00:00:00:00:00:B0:00:40:51:04:03:AA:00:F0     | Read / Write  |
| 0xF000AA0304514000B000000000000000     | Temperature Period                              | 64                                                           | Read / Write  |
| 0x2901                                 | Characteristic User Description                 | Temperature Period                                           | Read          |

#### Usage

1. If desired, set the sensor reading period (in ms) by writing to the Period Characteristic
1. Enable Temperature Data Notifications by writing 01:00 to the Temperature Data Client Characteristic
   Configuration Attribute
1. Enable the Temperature Service by writing 01 to the Temperature Config Characteristic
1. Temperature reading notifications in degrees Celsius will now be sent periodically as 32-bit floats

### Humidity Service

The Humidity Service uses the HDC2010 Sensor to read the humidity periodically at a period set by
the period characteristic.

#### Attribute Table

| UUID                                   | Description                                           | Value                                                        |  Properties   |
|--------------------------------------- |-------------------------------------------------------|--------------------------------------------------------------|---------------|
| 0x2800                                 | GATT Primary Service Declaration (Humidity)           | 00:00:00:00:00:00:00:B0:00:40:51:04:20:AA:00:F0              | Read          |
| 0x2803                                 | GATT Characteristic Declaration (Data)                | 12:3A:00:00:00:00:00:00:00:00:B0:00:40:51:04:21:AA:00:F0     | Read / Notify |
| 0xF000AA2104514000B000000000000000     | Humidity Data                                         | 00:00:00:00                                                  | Read / Notify |
| 0x2902                                 | Client Characteristic Configuration                   | 00:00                                                        | Read / Write  |
| 0x2901                                 | Characteristic User Description                       | Humidity Data                                                | Read          |
| 0x2803                                 | GATT Characteristic Declaration (Configuration)       | 0A:3E:00:00:00:00:00:00:00:00:B0:00:40:51:04:22:AA:00:F0     | Read / Write  |
| 0xF000AA2204514000B000000000000000     | Humidity Config                                       | 00                                                           | Read / Write  |
| 0x2901                                 | Characteristic User Description                       | Humidity Conf.                                               | Read          |
| 0x2803                                 | GATT Characteristic Declaration (Period)              | 0A:41:00:00:00:00:00:00:00:00:B0:00:40:51:04:23:AA:00:F0     | Read / Write  |
| 0xF000AA2304514000B000000000000000     | Humidity Period                                       | 64                                                           | Read / Write  |
| 0x2901                                 | Characteristic User Description                       | Humidity Period                                              | Read          |

#### Usage

1. If desired, set the sensor reading period (in ms) by writing to the Period Characteristic
1. Enable Humidity Data Notifications by writing 01:00 to the Humidity Data Client Characteristic
   Configuration Attribute
1. Enable the Humidity Service by writing 01 to the Humidity Config Characteristic
1. Humidity reading notifications in percent relative humidity will now be sent periodically as 32-bit floats

### Light Service

The Light Service uses the OPT3010 Sensor to read the lux periodically at a period set by
the period characteristic.

#### Attribute Table

| UUID                                   | Description                                           | Value                                                        |  Properties   |
|--------------------------------------- |-------------------------------------------------------|--------------------------------------------------------------|-------------- |
| 0x2800                                 | GATT Primary Service Declaration (Light)              | 00:00:00:00:00:00:00:B0:00:40:51:04:70:AA:00:F0              | Read          |
| 0x2803                                 | GATT Characteristic Declaration (Data)                | 12:45:00:00:00:00:00:00:00:00:B0:00:40:51:04:71:AA:00:F0     | Read / Notify |
| 0xF000AA7104514000B000000000000000     | Light Data                                            | 00:00:00:00                                                  | Read / Notify |
| 0x2902                                 | Client Characteristic Configuration                   | 00:00                                                        | Read / Write  |
| 0x2901                                 | Characteristic User Description                       | Light Data                                                   | Read          |
| 0x2803                                 | GATT Characteristic Declaration (Configuration)       | 0A:49:00:00:00:00:00:00:00:00:B0:00:40:51:04:72:AA:00:F0     | Read / Write  |
| 0xF000AA7204514000B000000000000000     | Light Config                                          | 00                                                           | Read / Write  |
| 0x2901                                 | Characteristic User Description                       | Light Conf.                                                  | Read          |
| 0x2803                                 | GATT Characteristic Declaration (Period)              | 0A:4C:00:00:00:00:00:00:00:00:B0:00:40:51:04:73:AA:00:F0     | Read / Write  |
| 0xF000AA7304514000B000000000000000     | Light Period                                          | 64                                                           | Read / Write  |
| 0x2901                                 | Characteristic User Description                       | Light Period                                                 | Read          |

#### Usage

1. If desired, set the sensor reading period (in ms) by writing to the Period Characteristic
1. Enable Light Data Notifications by writing 01:00 to the Light Data Client Characteristic
   Configuration Attribute
1. Enable the Light Service by writing 01 to the Light Config Characteristic
1. Light reading notifications in lux will now be sent periodically as 32-bit floats

### LED Service

#### Attribute Table

The LED service is used to control the three LED's on the LPSTK (red, blue, green).

| UUID                                   | Description                                           | Value                                                        |  Properties                           |
|--------------------------------------- |-------------------------------------------------------|--------------------------------------------------------------|---------------------------------------|
| 0x2800                                 | GATT Primary Service Declaration (LED)                | 00:00:00:00:00:00:00:B0:00:40:51:04:10:11:00:F0              | Read                                  |
| 0x2803                                 | GATT Characteristic Declaration (LED 0)               | 0E:50:00:00:00:00:00:00:00:00:B0:00:40:51:04:11:11:00:F0     | Read / Write without Response / Write |
| 0xF000111104514000B000000000000000     | LED0 State                                            | 00                                                           | Read / Write without Response / Write |
| 0x2901                                 | Characteristic User Description                       | LED 0                                                        | Read                                  |
| 0x2803                                 | GATT Characteristic Declaration (LED 1)               | 0E:53:00:00:00:00:00:00:00:00:B0:00:40:51:04:12:11:00:F0     | Read / Write without Response / Write |
| 0xF000111204514000B000000000000000     | LED1 State                                            | 00                                                           | Read / Write without Response / Write |
| 0x2901                                 | Characteristic User Description                       | LED 1                                                        | Read                                  |
| 0x2803                                 | GATT Characteristic Declaration (LED 2)               | 0E:56:00:00:00:00:00:00:00:00:B0:00:40:51:04:13:11:00:F0     | Read / Write without Response / Write |
| 0xF000111304514000B000000000000000     | LED2 State                                            |                                                              | Read / Write without Response / Write |
| 0x2901                                 | Characteristic User Description                       | LED 2                                                        | Read                                  |

#### Usage

1. Write a 1 or 0 to the relevant LED State Characteristic to turn the LED
   on / off respectively

### Button Service

The button service is used to notify the value of the LPSTK buttons when they are pressed / released.

#### Attribute Table

| UUID                                   | Description                                           | Value                                                        |  Properties   |
|--------------------------------------- |-------------------------------------------------------|--------------------------------------------------------------|---------------|
| 0x2800                                 | GATT Primary Service Declaration (Button)             | 00:00:00:00:00:00:00:B0:00:40:51:04:20:11:00:F0              | Read          |
| 0x2803                                 | GATT Characteristic Declaration (Button 0)            | 12:5A:00:00:00:00:00:00:00:00:B0:00:40:51:04:21:11:00:F0     | Read / Notify |
| 0xF000112104514000B000000000000000     | Button 0 State                                        | 00                                                           | Read / Notify |
| 0x2902                                 | Client Characteristic Configuration                   | 00:00                                                        | Read / Write  |
| 0x2901                                 | Characteristic User Description                       | Button 0                                                     | Read          |
| 0x2803                                 | GATT Characteristic Declaration (Button 1)            | 12:5E:00:00:00:00:00:00:00:00:B0:00:40:51:04:22:11:00:F0     | Read / Notify |
| 0xF000112204514000B000000000000000     | Button 1 State                                        | 00                                                           | Read / Notify |
| 0x2902                                 | Client Characteristic Configuration                   | 00:00                                                        | Read / Write  |
| 0x2901                                 | Characteristic User Description                       | Button 1                                                     | Read          |

#### Usage

1. Write 01:00 to the respective Button Client Characteristic Configuration attribute to
   enable notifications for the given button
1. Button State notifications will be sent for each button as its value changes to indicate
   1 / 0 for on / off

This will be displayed in the SimpleLink Starter app as such:

<img src="resource/buttons_app.gif"/>

The UART log will show the following:

<img src="resource/buttons_log.gif"/>

### Battery Service

The battery service is used to display the percentage battery remaining. The voltage level is set
to account for a 3V coin cell battery so other batteries will not display accurately.

#### Attribute Table

| UUID                                   | Description                                           | Value                                                        |  Properties   |
|--------------------------------------- |-------------------------------------------------------|--------------------------------------------------------------|---------------|
| 0x2800                                 | GATT Primary Service Declaration (Battery)            | 00:00:00:00:00:00:00:B0:00:40:51:04:0F:18:00:F0              | Read          |
| 0x2803                                 | GATT Characteristic Declaration (Percentage)          | 12:63:00:00:00:00:00:00:00:00:B0:00:40:51:04:19:2A:00:F0     | Read / Notify |
| 0xF0002A1904514000B000000000000000     | Battery Percentage                                    | 64                                                           | Read / Notify |
| 0x2902                                 | Client Characteristic Configuration                   | 00:00                                                        | Read / Write  |
| 0x2901                                 | Characteristic User Description                       | Battery Data                                                 | Read          |

#### Usage

1. Write 01:00 to the Battery Percentage Client Characteristic Configuration attribute to
   enable notifications
1. Battery Percentage State notifications will be sent as unsigned 8-bit values

### Accelerometer Service

The Accelerometer Service uses the ADXL362 Sensor to read the accelerometer coordinates. It implements
the default SPI Accelerometer example included with Sensor Controller Studio which does the following:
  - The accelerometer samples autonomously at 100 Hz, and generates interrupts to the Sensor Controller.
  - The Sensor Controller reads the accelerometer and performs simple filtering and tilt detection.
  - Tilt detection is communicated to the Accelerometer Service which will send the updated coordinates
    over the air.

**Note: Accelerometer coordinates are NOT sent periodically. They are only sent when a tilt is
   detected**

**Note: The Range characteristic is not currently implemented**

#### Attribute Table

| UUID                                   | Description                                           | Value                                                        |  Properties  |
|--------------------------------------- |-------------------------------------------------------|--------------------------------------------------------------|--------------|
| 0x2800                                 | GATT Primary Service Declaration (Accelerometer)      | 00:00:00:00:00:00:00:B0:00:40:51:04:A0:FF:00:F0              | Read         |
| 0x2803                                 | GATT Characteristic Declaration (Enable)              | 0A:68:00:00:00:00:00:00:00:00:B0:00:40:51:04:A1:FF:00:F0     | Read / Write |
| 0xF000FFA104514000B000000000000000     | Enable                                                | 00                                                           | Read / Write |
| 0x2901                                 | Characteristic User Description                       | Accel Enable                                                 | Read         |
| 0x2803                                 | GATT Characteristic Declaration (Range)               | 02:6B:00:00:00:00:00:00:00:00:B0:00:40:51:04:A2:FF:00:F0     | Read         |
| 0xF000FFA204514000B000000000000000     | Range                                                 | 14:00                                                        | Read         |
| 0x2901                                 | Characteristic User Description                       | Accel Range                                                  | Read         |
| 0x2803                                 | GATT Characteristic Declaration (X-Coordinate)        | 10:6E:00:00:00:00:00:00:00:00:B0:00:40:51:04:A3:FF:00:F0     | Notify       |
| 0xF000FFA304514000B000000000000000     | X-Coordinate                                          |                                                              | Notify       |
| 0x2902                                 | Client Characteristic Configuration                   | 00:00                                                        | Read / Write |
| 0x2901                                 | Characteristic User Description                       | Accel X-Coordinate                                           | Read         |
| 0x2803                                 | GATT Characteristic Declaration (Y-Coordinate)        | 10:72:00:00:00:00:00:00:00:00:B0:00:40:51:04:A4:FF:00:F0     | Notify       |
| 0xF000FFA404514000B000000000000000     | Y-Coordinate                                          |                                                              | Notify       |
| 0x2902                                 | Client Characteristic Configuration                   | 00:00                                                        | Read / Write |
| 0x2901                                 | Characteristic User Description                       | Accel Y-Coordinate                                           | Read         |
| 0x2803                                 | GATT Characteristic Declaration (Z-Coordinate)        | 10:76:00:00:00:00:00:00:00:00:B0:00:40:51:04:A5:FF:00:F0     | Notify       |
| 0xF000FFA504514000B000000000000000     | Z-Coordinate                                          |                                                              | Notify       |
| 0x2902                                 | Client Characteristic Configuration                   | 00:00                                                        | Read / Write |
| 0x2901                                 | Characteristic User Description                       | Accel Z-Coordinate                                           | Read         |

#### Usage

1. Enable Accelerometer Data Notifications by writing 01:00 to the Accelerometer Data Client
   Characteristic Configuration Attributes for the X, Y, and Z coordinates
1. Enable the Accelerometer Service by writing 01 to the Accelerometer Enable Characteristic
1. Accelerometer reading notifications will be sent for each coordinate when a tilt is
   detected as signed 16-bit values

### Hall Service

The Hall Service uses the HDC2010 Sensor to read the magnetic flux periodically at a period set by
the period characteristic.

#### Attribute Table

| UUID                                   | Description                                           | Value                                                        |  Properties   |
|--------------------------------------- |-------------------------------------------------------|--------------------------------------------------------------|---------------|
| 0x2800                                 | GATT Primary Service Declaration (Hall)               | 00:00:00:00:00:00:00:B0:00:40:51:04:90:AA:00:F0              | Read          |
| 0x2803                                 | GATT Characteristic Declaration (Data)                | 12:7B:00:00:00:00:00:00:00:00:B0:00:40:51:04:91:AA:00:F0     | Read / Notify |
| 0xF000AA9104514000B000000000000000     | Data                                                  | 00:00:00:00                                                  | Read / Notify |
| 0x2902                                 | Client Characteristic Configuration                   | 00:00                                                        | Read / Write  |
| 0x2901                                 | Characteristic User Description                       | Hall Data                                                    | Read          |
| 0x2803                                 | GATT Characteristic Declaration (Configuration)       | 0A:7F:00:00:00:00:00:00:00:00:B0:00:40:51:04:92:AA:00:F0     | Read / Write  |
| 0xF000AA9204514000B000000000000000     | Configuration                                         | 00                                                           | Read / Write  |
| 0x2901                                 | Characteristic User Description                       | Hall Conf.                                                   | Read          |
| 0x2803                                 | GATT Characteristic Declaration (Period)              | 0A:82:00:00:00:00:00:00:00:00:B0:00:40:51:04:93:AA:00:F0     | Read / Write  |
| 0xF000AA9304514000B000000000000000     | Period                                                | 64                                                           | Read / Write  |
| 0x2901                                 | Characteristic User Description                       | Hall Period                                                  | Read          |

#### Usage

1. If desired, set the sensor reading period (in ms) by writing to the Period Characteristic
1. Enable Hall Data Notifications by writing 01:00 to the Hall Data Client Characteristic
   Configuration Attribute
1. Enable the Hall Service by writing 01 to the Hall Config Characteristic
1. Hall reading notifications in magnetic flux will now be sent periodically as 32-bit floats

## Known Limitations

- For any services that read data periodically, this is the only way to initiate a sensor reading.
   That is, simply reading the data characteristic will only read the last value read from the periodic
   reading; it will not perform a new reading.
- The Hall Service is not enabled as the SAIL driver was designed for a different sensor.
- When reverting to factory image, the device will need to be reset (toggle LPSTK power off/on)
  after invalidating the internal flash image. After the reset, it will then jump to the external
  flash image (copied into internal flash). This is a known issue in the BLE5-Stack: see the BLE5-Stack
  Release Notes for more information.
- The range characteristic of the accelerometer service is not implemented and should not be written to.