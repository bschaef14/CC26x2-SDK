TI 15.4-Stack Sensor SM Example
=======================

Example Summary
---------------

The sensor secure manager (SM) example application demonstrates how to implement a sensor network device using TI 15.4-Stack with secure commissioning. A SM-enabled TI 15.4-Stack based star network consists of two types of logical devices: the SM-enabled PAN-Coordinator and the SM-enabled network devices, e.g. the Collector SM and Sensor SM applications, respectively. These devices function like their regular counterparts, but include provisions for authentication and secure device-key generation.

The SM-enabled PAN-Coordinator initiates the secure manager process and controls whether a device is accepted into the network or rejected and disassociated in the event of a failure. To be accepted, a network device must be verified using a mutually supported authentication method, and successfully generate a unique device key to be used for unicast communication. Broadcast communication will still be encrypted with the pre-shared key.

Key refreshments are also handled by the SM-enabled PAN-Coordinator and occur on a timeout basis.

The example applications in TI 15.4-Stack are developed for the CC13x2 Launchpad platform. In addition, the Linux example applications for the external host (AM335x + MAC Coprocessor) are located in the [**TI 15.4-Stack Gateway SDK**](http://www.ti.com/tool/ti-15.4-stack-gateway-linux-sdk).

> The project names for CC1352 and CC2652 platforms are referred to as CC13x2 or CC26x2. Replace x with either **1** or **5** depending on the specific wireless MCU being used.

> Note that this also includes the CC1352P-X boards, where the X represents which board subset is used, and the power amplification range.


> Although the application code for the sensor SM example is the same for each platform, there are different example projects, named as sensor_sm_x. Replace x with the device name of the specific Launchpad used.

Peripherals Exercised
---------------------

> To trigger various events, buttons can be used as well as the configurable user interface.
> The Example Usage section of this document explains how to use the user interface, although both the button presses and the UART perform the
> same actions.

* `CONFIG_GPIO_RLED` - Turns on after the sensor connects to the collector.
* `CONFIG_GPIO_BTN1` - Press to initialize the sensor application.
* `CONFIG_GPIO_BTN2` - Press to disassociate the device from the network.

> If `CONFIG_GPIO_BTN2` is held while power is applied to the Launchpad, NV Flash will be erased.

Resources & Jumper Settings
---------------------------
The following hardware is required to run TI 15.4-Stack Out of Box (OOB) example applications:
* Two [**CC13x2 Launchpad development kits**](http://www.ti.com/tool/launchxl-cc1352r1) or two [**CC26x2 Launchpad development kits**](http://www.ti.com/tool/launchxl-cc26x2r1)

> If you're using an IDE (such as CCS or IAR), please refer to `Board.html` in
your project directory for resources used and board-specific jumper settings.
Otherwise, you can find `Board.html` in the directory
&lt;SDK_INSTALL_DIR&gt;/source/ti/boards/&lt;BOARD&gt;.

Example Usage
-------------
This example project implements a SM-enabled sensor end device: one of potentially many network devices in a secure PAN. This end device must first be authenticated and generate an encryption key for unicast communication before being allowed into the network. Once commissioned, the sensor reads sensor information and sends it to the PAN-coordinator at a configured interval. This example assumes a second Launchpad is running the default collector SM application code to act as the SM-enabled PAN-coordinator.

The example output can be viewed through the UART terminal.

* Open a serial session (e.g. [`PuTTY`](http://www.putty.org/ "PuTTY's
Homepage"), etc.) to the appropriate COM port with the following settings.

* Note that if you are using Tera Term, by default, the Backspace key will be replaced with the delete key.
If you go to Setup->Keyboard There is a section called `Transmitting DEL by:`
Make sure that the backspace character is checked as well.

> The COM port can be determined via Device Manager in Windows or via
`ls /dev/tty*` in Linux.

* Upon example start, the connection will have the following settings:
```
    Baud-rate:     115200
    Data bits:          8
    Stop bits:          1
    Parity:          None
    Flow Control:    None
```
and initially display the following text on the UART terminal.


Main Menu:
```
 TI Sensor
 Press Enter for Help
<      HELP      >

Security Status: --
Status: Waiting...
```

The configurable user interface is provided to allow you to make changes at runtime, as follows:
```
    0xFFFF

<      SET PANID      >

Security Status: --
Status: Waiting...
```

```
 00 00 0F 00 00 00 00 00 00 00 00 00 00 00 00 00 00

<    SET CHAN MASK    >

Security Status: --
Status: Waiting...
```
```
 12 34 56 78 9A BC DE F0 00 00 00 00 00 00 00 00

<     SET NWK KEY     >

Security Status: --
Status: Waiting...
```
```
  DEFAULT CODE

<     AUTH METHOD     >

Security Status: --
Status: Waiting...
```

Note that these changes will only take effect if the sensor is in a waiting state.
Keys 0-F can be used to change the value when in edit mode, and left/right keys can be used for navigating the digits.
Once the sensor is started, the settings can only be changed if it is restarted.

> If the **AUTO_START** symbol is defined in your application, then the application will automatically configure itself on startup.
> This is not enabled by default with the project, but it can be configured as described below.

**AUTO_START** can be defined by removing the x in the .opts file under the defines folder.
-DxAUTO_START -> -DAUTO_START

> If the **AUTO_START** symbol is defined in your application, then the application will automatically configure itself on startup,
and the sensor will display Starting... instead of Waiting...

> If the **AUTO_START** symbol is not defined pressing `CONFIG_GPIO_BTN1` will initialize the sensor application until the sensor has connected to a network.  Alternatively, the sensor can also be started through the user interface, as shown below.
Note that the sensor will not join a network unless it is started.

To ASSOCIATE to a network and start the sensor:

```
 TI Sensor

<      ASSOCIATE      >

Security Status: --
Status: Starting...
```

To DISASSOCIATE from a network:

```
 TI Sensor

<     DISASSOCIATE    >

Security Status: --
Status: Starting...
```

> The joining device state variable information can be seen in the Joining Device Logical Link Controller's header file (`jdllc.h`), within the `Jdllc_states_t` structure.

* Wait for the sensor device to associate with the coordinator, after which the output will be updated with the channel number and device Id of the sensor that was started. After association `CONFIG_GPIO_RLED` will be set.
```
     TI Sensor
     Press Enter for Help
    <      HELP      >

    Security Status: Commissioned: 0x01
    Status: Joined--Mode=NBCN, Addr=0x0001, PanId=0x0001, Ch=0
```

> In order for the network device to associate, it must have either the generic PAN Id (0xFFFF, default configuration) or the same PAN Id as the collector. These settings can be found in the application's SysConfig dashboard.

* The secure commissioning process will then commence and informational messages will be displayed on the 'Security Status' update line.
```
    Security Status: Commissioning Started: 0x01
    Security Status: Commissioned: 0x01
```

> If the passkey authentication method is chosen, the user will be guided with prompts to input a chosen 6 numerical digit passkey through UART.

* After the sensor node has successfully joined the network, it receives a configuration request message from the collector SM application. The node then configures the time interval on how often to report the sensor data to the collector SM application, and how often to poll for buffered messages in case of sleepy devices. After receiving the configuration request message, the green LED (`CONFIG_GPIO_GLED`) toggles whenever the device sends the message. In the event of a failure during the SM process, the sensor node will be disassociated by the coordinator.

* Key refreshments will occur based on a timeout period set on the collector SM application, and the Security Status line will update accordingly.
```
    Security Status: Key Refreshment Started: 0x01
    Security Status: Key Refreshed: 0x01
```

```
     TI Sensor
     Press Enter for Help
    <      HELP      >

    Security Status: Key Refreshed: 0x01
    Status: Joined--Mode=NBCN, Addr=0x0001, PanId=0x0001, Ch=0
```

Analyzing Network Traffic
-------------------------

TI 15.4-Stack provides the means to analyze over-the-air traffic by including a packet sniffer firmware image. With an additional CC13x2 or CC26x2 Launchpad, users can set up a packet sniffer with the software provided in the SDK. More information about this can be found in the TI 15.4-Stack documentation under **Packet Sniffer**.


Viewing Device PER (Packet Error Rate)
--------------------------------------

PER is a simple value with the following equation
* PER = 100 * (Number of Failed Packets / (Number of Successful Packets + Number of Failed Packets))

This value can be used to judge how well the network doing.

If you would like to see the stats on the Number of Failed and Successful Packets then simply define **DISPLAY_PER_STATS**. This will add the code necessary to update and display the stats to the UART Display.


Project Configuration
--------------------------

The System Configuration (SysConfig) tool is a graphical interface for configuring your TI 15.4-Stack project. Based on the parameters configured in the SysConfig dashboard, C source files and header files are generated. Further advanced parameters can be located in `advanced_config.h`.

Some important settings in the TI 15.4-Stack module include:

| **Parameter**              | **SysConfig Location**            | **Description**                                               |
|----------------------------|-----------------------------------|---------------------------------------------------------------|
| Mode                       | Top of TI-15.4 Stack module       | Configures the mode of network operation                      |
| MAC Beacon Order           | MAC group within Network category | Configures how often the coordinator transmits a beacon       |
| MAC Super Frame Order      | MAC group within Network category | Configures the length of the active portion of the superframe |
| Channel Mask               | Network category                  | Configures channels to be scanned                             |
| Security Level             | Security category                 | Configures network security level                             |
| Pre-Shared Network Key     | Security category                 | Configures pre-shared network key                             |
| Authentication Method      | Security category                 | Configures authentication method supported by the sensor      |
| Default Authentication Code| Security category                 | Configures pre-defined code used for authentication           |

> SysConfig generated files are dynamically generated upon build, and any manual changes to them will be overwritten.

More information about the configuration and feature options can be found in the TI 15.4-Stack documentation under **Example Applications > Configuration Parameters**.

### Disabling Common User Interface

The common user interface (CUI) is a UART based interface that allows users to control and receive updates regarding the application. For various reasons, including reducing the memory footprint, the user is able to disable the common user interface (CUI). To disable the CUI, the following variable must be defined in the project-specific .opts file:

```
-DCUI_DISABLE
```

> Please Note: particular features that are dependednt on the CUI wil be unavailable when this feature is enabled.

### Multi-Page NV Configuration

By default, this project is configured to use two pages of NV. A maximum of five NV pages are supported. In order to modify the NV pages, update the following:
* `NVOCMP_NVPAGES=2` in the project-specific .opts file
* `NVOCMP_NVPAGES=2` in the linker options
* SysConfig NVS module:
   * Set Region Size based on the formula `NVOCMP_NVPAGES * 0x2000`
   * Set Region Base based on the formula `0x56000 - (NVOCMP_NVPAGES * 0x2000)`

A detailed description of the application architecture can be found in your installation within the
TI-15.4 Stack Getting Started Guide's Application Overview section.

&lt;SDK_INSTALL_DIR&gt;/docs/ti154stack/ti154stack-getting-started-guide.html.