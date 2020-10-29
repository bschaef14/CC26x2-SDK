# Multi Role

## Table of Contents

* [Introduction](#Introduction)
* [Hardware Prerequisites](#Hardware Prerequisites)
* [Software Prerequisites](#Software Prerequisites)
* [Assumptions/Considerations](#Assumptions/Considerations)
* [Usage](#Usage)
    * [Advertise](#Advertise)
    * [Discover Devices](#Discover Devices)
    * [Connect](#Connect)
    * [GATT Read](#GATT Read)
    * [GATT Write](#GATT Write)
    * [Connection Update](#Connection Update)
    * [Disconnect](#Disconnect)

## <a name="Introduction"></a>Introduction

The `multi_role` project demonstrates the capabilities of the
Texas Instruments BLE stack to function in multiple simultaneous
connection roles.

Specifically, the project is capable of functioning as
a master and a slave simultaneously, in any combination, while
maintaining up to the amount of connections defined by `MAX_NUM_BLE_CONNS`.
Check on this preprocessor define for the default value.

The project is capable of connecting to any central/peripheral device.
Any type of legacy pairing/bonding is possible. Parameter updates are
supported. Connections can be established in any role. The project supports
multiple simultaneous GATT discoveries and/or pairings/bondings.

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

## <a name="Hardware Prerequisites"></a>Hardware Prerequisites

The default Multi Role board configuration uses the
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

## <a name="Assumptions/Considerations"></a>Assumptions/Considerations

In this document, the terms master/central and slave/peripheral are used
synonymously. It is assumed that the master/central devices are GATT clients and
slave/peripheral devices are GATT servers. Once the connection limit (set with
the `MAX_NUM_BLE_CONNS` preprocessor define) is reached, the `multi_role` device
won’t be allowed to advertise/scan until there is a disconnection.

When connected in the master role, the `multi_role` device will query the slave
device for the simpleGATTProfile service in order to demo some basic GATT
procedures (read/write characteristic). However, it is not necessary for the
slave device to contain this service in order for a connection to be
established. That is, it is possible for the peripheral devices in the system to
have differing attribute tables since the `multi_role` device will perform a
service/characteristic discovery after each connection is formed.

By default, the `multi_role` application is configured to filter
and connect to peripheral devices with the TI Simple Profile UUID,
such as devices with the `simple_peripheral` project. To modify this
behavior, set `Discover devices by service UUID` to `FALSE` in SysConfig
(under `Observer Configuration`).

If the project is configured for too many connections (via the
`MAX_NUM_BLE_CONNS` preprocessor define) and also security, it is possible for
heap allocation failures to occur which will break the stack. Therefore, the
project should be stress-tested for its intended use case to verify that there
are no heap issues by including the `HEAPMGR_METRICS` preprocessor define. See the
Debugging section of the **BLE5-Stack User's Guide** for more information on how to do this.

When at least one connection is already formed, in order to allow enough
processing time to scan for a new connection, the minimum possible connection
interval (in milliseconds) that can be used is:

```12.5 + 5*n```

where n is the amount of current connections. For example, if there are
currently four connections, all four connections must use a minimum connection
interval of ```12*5 + 5*4 = 32.5 ms``` in order to allow scanning to occur to
establish a new connection.

## <a name="Usage"></a>Usage

The `multi_role` project uses the two buttons on the LaunchPad to accept
user input and outputs information through the UART.
This application uses the UART peripheral to provide an
interface for the application. The UART is only used for display messages.
This document will use a terminal program (Tera Term, PuTTY, etc.) to serve as
the display for the output of the LaunchPad. Note that any other serial terminal
can be used. The following default parameters are used for the UART peripheral
for display:

  UART Param     |Default Values
  -------------- |----------------
  Baud Rate      |115200
  Data length    |8 bits
  Parity         |None
  Stop bits      |1 bit
  Flow Control   |None

Upon powering on or resetting the device, the user will see the main
menu. When in the main menu, the left button will scroll through the
possible actions, which are:

* Discover Devices
* Advertise

Other options will become available once devices are scanned, and again once a
connection is formed.

The device will report upon startup that it is initialized, advertising, and
connected to 0 devices:

<img src="resource/mr_figure1.png"/>

The right button is used to select one of the available options. Each option
will be covered in the following sections.

### <a name="Advertise"></a>Advertise

Selecting this option will toggle advertising on and off. It will display
"Adv Set 0 Disabled" when advertising is off and "Adv Set 0 Enabled" when
advertising is on. By default, advertising is on which will send out
connectable advertisements from the device.

### <a name="Discover Devices"></a>Discover Devices

Selecting this option will start a device discovery to search for advertising
devices. It will then display the number of devices found:

<img src="resource/mr_figure2.png"/>

### <a name="Connect"></a>Connect

After selecting `Discover Devices`, the option to connect to a device will
now become available. Selecting the `+Connect` option to allow you to
scroll through the discovered devices:

<img src="resource/mr_figure3.png"/>


A device can then be selected to initiate a connection. The connect option can
be chosen again to connect with additional peripheral devices. The terminal
window will display the number of devices that are connected to the Multi Role
device. With a device now connected, select `+Work with` and select the appropriate
connected device.

<img src="resource/mr_figure4.png"/>

At this point, the following options are present to be selected:

* GATT Read
* GATT Write
* Connection Update
* Disconnect

It is also possible to select "Upper Menu" to return to the main menu.

### <a name="GATT Read"></a>GATT Read

This option is used to send read requests to the peripheral device.
Pressing the right button once will send a read request to the
connected device and will print the returned value of the characteristic.
Note that this characteristic can be written to with the `GATT Write` option:

<img src="resource/mr_figure5.png"/>

### <a name="GATT Write"></a>GATT Write

This option is used to write predetermined values (0x00, 0x55, 0xAA, etc.) to the
device's characteristic. Note that this characteristic can be read from with the
`GATT Read` option:

<img src="resource/mr_figure6.png"/>

### <a name="Connection Update"></a>Connection Update

This option updates the connection parameters based on the values set by
the following constants in `multi_role.c`:
* DEFAULT\_UPDATE\_MIN\_CONN\_INTERVAL
* DEFAULT\_UPDATE\_MAX\_CONN\_INTERVAL
* DEFAULT\_UPDATE\_SLAVE\_LATENCY
* DEFAULT\_UPDATE\_CONN\_TIMEOUT

The update was successful if it states that it has been "Updated" and
displays the new connection timeout value.

<img src="resource/mr_figure7.png"/>

### <a name="Disconnect"></a>Disconnect

This option will send a link termination request to terminate the connection
with the connected device. The following shows a successful
disconnection:

<img src="resource/mr_figure8.png"/>