# Project Zero

## Table of Contents

* [Introduction](#Introduction)
* [Hardware Prerequisites](#Hardware Prerequisites)
* [Software Prerequisites](#Software Prerequisites)
* [Service/Profile Table](#Service/Profile Table)
    * [LED Service](#LED Service)
    * [Button Service](#Button Service)
    * [Data Service](#Data Service)
* [Usage](#Usage)
    * [Booting](#Booting)
    * [Connecting](#Connecting)


## <a name="Introduction"></a>Introduction

The `project_zero` project implements a simple Bluetooth low energy peripheral
device with GATT services and demonstrates how to integrate Over the Air
Download and three custom services. This project can be a framework for
developing many different peripheral-role applications.

This project has the following app configurations available:

|App Build Configuration         | Description                                                       |
|--------------------------------|-------------------------------------------------------------------|
|FlashROM_Release (default)      | All TI-RTOS debug features disabled but application-logging to UART remains enabled. <br> The application uses the config file `ble_release.cfg` and the defines are in `<app name>_FlashROM_Release.opt`.|
|FlashROM_Debug                  | All TI-RTOS debug features enabled. <br> The application uses the config file `ble_debug.cfg` and the defines are in `<app name>_FlashROM_Debug.opt`.|

All application configurations use the stack library configuration,
FlashROM_Library. This build configuration will yield additional flash footprint
optimizations by the linker since the application and stack can share
contiguous flash pages. This configuration is further discussed in the Stack
Library Configuration section of the **BLE5-Stack User's Guide** provided in the SDK.

**Note on the Boot Image Manager**
<br>This project is configured to be managed and launched by the `BIM` or `Boot Image Manager` example app. This means that this project does not include a `CCFG` section to configure the device at boot, and therefore requires that the BIM project is programmed into the device first. <br/><br/>The application supports secure OAD i.e. Image authentication needs to be done before executing the newly tranferred image. This is done by the BIM example app. A security header is added to this application image in order to support this.<br/><br/>If BIM is not present in the device internal Flash, the project can usually be started from the IDE but will not be executed after a pin reset. Read more about BIM and OAD in the **BLE5-Stack User's Guide**.

## <a name="Hardware Prerequisites"></a>Hardware Prerequisites

The default Project Zero board configuration uses the
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

This project contains 3 services in addition to GAP, GATT, Device Information
and OAD.

### <a name="LED Service"></a>LED Service

This service exposes two readable and writeable characteristics, `LED0` and
`LED1` which both expect `1 byte` of data. Writing 0 to either of the
characteristic value attributes will turn off the corresponding LED on the
Launchpad. Writing anything other than 0 will turn the LED on.


 Description     |  UUID                                    | Format                 | Properties
-----------------|------------------------------------------|------------------------|-----------------------
 LED Service     | F000**1110**-0451-4000-B000-000000000000 | *Service declaration*  | Read
 LED0 State      | F000**1111**-0451-4000-B000-000000000000 | 1 byte uint8_t         | Read/Write
 LED1 State      | F000**1112**-0451-4000-B000-000000000000 | 1 byte uint8_t         | Read/Write


### <a name="Button Service"></a>Button Service

This service exposes to readable and notifiable characteristics, `BUTTON0` and
`BUTTON1` which correspond to the hardware switches `BTN-1` and `BTN-2`
respectively. The state of each is updated when the hardware switch is
pressed, and this can be read out via BLE. If notifications are enabled on
these characteristics the button states will be sent out as notifications.

 Description     |  UUID                                    | Format                 | Properties
-----------------|------------------------------------------|------------------------|-----------------------
 Button Service  | F000**1120**-0451-4000-B000-000000000000 | *Service declaration*  | Read
 BUTTON0 State   | F000**1121**-0451-4000-B000-000000000000 | 1 byte uint8_t         | Read/Notify
 BUTTON1 State   | F000**1122**-0451-4000-B000-000000000000 | 1 byte uint8_t         | Read/Notify

### <a name="Data Service"></a>Data Service

This service exposes a long (longer than legacy MTU size) characteristic
`String char` and a characteristic that is meant to exemplify streaming,
`Stream char`. In order to read the entire contents of `String char`, a client
must either have negiotated a longer MTU size than the default 27 bytes, or
must use the `GATT Long Read` procedure.

 Description     |  UUID                                    | Format                 | Properties
-----------------|------------------------------------------|------------------------|-----------------------
 Data Service    | F000**1130**-0451-4000-B000-000000000000 | *Service declaration*  | Read
 String char     | F000**1131**-0451-4000-B000-000000000000 | Up to 40 byte uint8_t  | Read/Write/WriteNoRsp
 Stream char     | F000**1132**-0451-4000-B000-000000000000 | Up to 20 byte uint8_t  | WriteNoRsp/Notify


## <a name="Usage"></a>Usage

This application uses the UART peripheral to provide an interface for the
application. The UART is only used for display messages.

We recommend that you use a terminal program that can parse ANSI/VT100 color
codes such Tera Term, PuTTY, Code Composer Studio Terminal, etc. to to monitor
the LaunchPad UART output.

To interact with the application over Bluetooth you may use any device that is
able to act as a BLE Central device, such as

* iOS with the apps SimpleLink Starter or LightBlue

* Android with the apps SimpleLink Starter or BLEScanner

* A CC26xx/CC13xx LaunchPad running the sample app HostTest from a compatible
  SDK with the PC tool BTool from the same SDK.

Please see the Readme for `simple_peripheral` for an example of how
to connect with a smartphone.

The following default parameters are used for the UART peripheral for
display:

  UART Param     |Default Values
  -------------- |----------------
  Baud Rate      |115200
  Data length    |8 bits
  Parity         |None
  Stop bits      |1 bit
  Flow Control   |None

### <a name="Booting"></a>Booting

Once the Project Zero sample application starts, the output to the terminal
will report its address and the fact that it is advertising, as shown below:

<img src="resource/boot.gif"/>

During boot/application init some activities related to `Boot Image Mananger
(BIM)` and the external flash also take place:

* The call to `OAD_open` will check if a factory image exists in external
  flash. If none exists, the running image will be copied to external flash
  and marked as factory image.

* `BTN-1`, the left button, is checked. If it is held down from reset, a 5
  second timeout elapses before the running image marks itself as invalid and
  reboots into BIM. The purpose of this is to make BIM revert to the factory
  image to internal Flash.

* The combination `BTN-1` and `BTN-2` is checked. If both buttons are held
  down for 5 seconds, the external Flash will be erased.

After application init both buttons will operate normally. If the buttons are
released during the 5 second timeout, the device will continue booting
normally.

**NOTE:** After erasing the external flash it is not recommended to perform the
          `BTN-1 + Reset` revert procedure as this leave the Boot Image Manager
          with no valid image in internal Flash and no factory image in external
          Flash. At this point a debugger like the on-board XDS110 must be used
          to download a new internal image to the device.
          Pressing only `Reset` after erasing external Flash will invoke the
          `OAD_open` self-backup functionality on boot and revert will function
          normally again.


### <a name="Connecting"></a>Connecting

When a device connects, the address of the device will be shown in the
console. After a while you will see the automatic parameter update request
sent from the device being applied.

<img src="resource/connect.gif"/>