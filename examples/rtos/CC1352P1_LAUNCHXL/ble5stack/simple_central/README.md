# Simple Central

## Table of Contents

* [Introduction](#Introduction)
* [Hardware Prerequisites](#Hardware Prerequisites)
* [Software Prerequisites](#Software Prerequisites)
* [Usage](#Usage)
    * [GATT Read](#read)
    * [GATT Write](#write)
    * [Start RSSI Reading](#RSSI)
    * [Connection Update](#connection update)
    * [Set Conn PHY Preference](#phy preference)
    * [Disconnect](#disconnect)

## <a name="Introduction"></a>Introduction

The `simple_central` project implements a simple Bluetooth low energy
central device with GATT client functionality. The `simple_central`
application can be configured to filter and connect to peripheral
devices with the TI Simple Profile UUID, such as devices with the
`simple_peripheral` project. To enable this behavior, set
`Discover devices by service UUID` to `TRUE` in SysConfig
(under `Observer Configuration`).

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

The default Simple Central board configuration uses the
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

Once the Simple Central sample application starts, the output to the terminal
will report its address and that it is initialized and ready to begin
discovery, as shown below. On the menu, you can use the left button
(BTN-1) to move to the next option and the right button (BTN-2) to select an item:

<img src="resource/sbc_figure1.png"/>

The first option can be selected to set the Scanning PHY. For more information
on PHY refer to the PHY section in the **BLE5-Stack User's Guide**.

<img src="resource/sbc_figure2.png"/>

The second option can be selected to begin discovering devices that are
broadcasting BLE advertisements. This will report the number of discovered devices.
Select `Connect to` to view the list of devices and use the left button (BTN-1)
to go through the available devices:

<img src="resource/sbc_figure3.png"/>

When you have found the device that you are trying to connect to, press the right
button to connect. This will display information such as the connection timeout
and the address of the connected peripheral device.

<img src="resource/sbc_figure11.png"/>

The third option can be selected to select a group to which the central will
start connecting automatically to.
After selecting the group  (Group A or Group B) the menu will go back to
main menu. Then the user will have to choose `Discover Devices` to start scanning
and connecting to the group members. After all members are connected , the
`Work with` menu can be chosen to use one of the members. If one of the members
got disconnected along the way , as long as the AutoConnect is not disabled,
new `Discover Devices` can be initiated.

<img src="resource/sbc_figure12.png"/>
Basic assumptions that need to be taken into account:

* A peripheral node is associated with a group when it advertises a group name
 as part of its adv data. The peripheral should advertise the group name as
 its Shortened local name (opcode 0x07) and then `GA` for Group A or `GB`
 for Group B.
* The central won't start automatically connecting to the chosen group until
  all defined group members are successfully scanned and found.
  Number of group members are defined by `MAX_NUM_BLE_CONNS`. Hence the user
  have to pre-define the number of members that wanted to be connected to.
* if the user defined `MAX_NUM_BLE_CONNS` to be bigger then 8 ,
  then all update parameters requests will be rejected.

For further information regarding the limitation and settings of scheduling and memory,
please see the **BLE5-Stack User's Guide**.


Once connected select `Work with`
and select the connected device.

<img src="resource/sbc_figure4.png"/>

It will now present you with 6 options. These options will be described in the
following sections:

* GATT Read
* GATT Write
* Start RSSI Reading
* Connection Update
* Set Conn PHY Preference
* Disconnect


It is also possible to browse to and select "Upper Menu" to return to the
main menu.

### <a name="read"></a>Option: GATT Read

This option is used to send read requests to the peripheral device.
Pressing the right button once will send a read request to the
peripheral device and will print the returned value of the characteristic.
Note that this characteristic can be written to with the `GATT Write` option:

<img src="resource/sbc_figure5.png"/>

### <a name="write"></a>Option: GATT Write

This option is used to write predetermined values (0x00, 0x55, 0xAA, etc.) to the
device's characteristic. Note that this characteristic can be read from with the
`GATT Read` option:

<img src="resource/sbc_figure6.png"/>

### <a name="RSSI"></a>Option: Toggle Read RSSI

This option is used to get the RSSI (signal strength) of the connected
peripheral device. Selecting this option will toggle RSSI reads on and off.
When it is on, the RSSI value will be printed. The value will continue to be
updated as the RSSI changes:

<img src="resource/sbc_figure7.png"/>

### <a name="connection update"></a>Option: Connection Update

This option updates the connection parameters based on the values set by
the following constants in `simple_central.c`:
* DEFAULT\_UPDATE\_MIN\_CONN\_INTERVAL
* DEFAULT\_UPDATE\_MAX\_CONN\_INTERVAL
* DEFAULT\_UPDATE\_SLAVE\_LATENCY
* DEFAULT\_UPDATE\_CONN\_TIMEOUT

The update was successful if it states that it has been "Updated" and
displays the new connection timeout value.
<img src="resource/sbc_figure8.png"/>

### <a name="phy preference"></a>Option: Set Conn PHY Preference
This option is used to select the PHY preference for the current connection.
If the PHY preference is changed to include a "faster" PHY than before,
`simple_central` will attempt a PHY change.
You can choose from the following options:

* 1 Mbps
* 2 Mbps
* 1 & 2 Mbps
* Coded
* 1 & 2 Mbps, & Coded

For more information on PHY refer to the PHY section in the
**BLE5-Stack User's Guide**. The following shows a successful
update to Coded PHY:

<img src="resource/sbc_figure9.png"/>

### <a name="disconnect"></a>Option: Disconnect

This option will send a link termination request to terminate the connection
with the connected device. The following shows a successful
disconnection:

<img src="resource/sbc_figure10.png"/>

Note that this application also supports MTU exchanges. This is done
automatically by the Simple Central device after a connection is formed with a
peripheral device. A larger MTU can be negotiated by changing `MAX_PDU_SIZE`. More
information on this can be found in the
Configuring for Larger MTU Values section of the **BLE5-Stack User's Guide**.

Also note that the application will automatically request pairing once the
connection is established.
