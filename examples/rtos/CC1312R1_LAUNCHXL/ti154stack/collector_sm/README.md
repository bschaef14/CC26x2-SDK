TI 15.4-Stack Collector SM Example
=======================

Example Summary
---------------

The collector secure manager (SM) example application demonstrates how to implement a Personal Area Network (PAN) Coordinator using TI 15.4-Stack with secure commissioning. A SM-enabled TI 15.4-Stack based star network consists of two types of logical devices: the SM-enabled PAN-Coordinator and the SM-enabled network devices, e.g. the Collector SM and Sensor SM applications, respectively. These devices function like their regular counterparts, but include provisions for authentication and secure device-key generation.

The SM-enabled PAN-Coordinator initiates the secure manager process and controls whether a device is accepted into the network or rejected and disassociated in the event of a failure. To be accepted, a network device must be verified using a mutually supported authentication method, and successfully generate a unique device key to be used for unicast communication. Broadcast communication will still be encrypted with the pre-shared key.

Key refreshments are also handled by the SM-enabled PAN-Coordinator and occur on a timeout basis.

The example applications in TI 15.4-Stack are developed for the CC13x2 Launchpad platform. In addition, the Linux example applications for the external host (AM335x + MAC Coprocessor) are located in the [**TI 15.4-Stack Gateway SDK**](http://www.ti.com/tool/ti-15.4-stack-gateway-linux-sdk).

> The project names for CC1352 and CC2652 platforms are referred to as CC13x2 or CC26x2. Replace x with either **1** or **5** depending on the specific wireless MCU being used.

> Note that this also includes the CC1352P-X boards, where the X represents which board subset is used, and the power amplification range.

> Although the application code for the collector SM example is the same for each platform, there are different example projects, named as collector_sm_x. Replace x with the device name of the specific Launchpad used.

Peripherals Exercised
---------------------

> To trigger various events, buttons can be used as well as the configurable user interface.
> The Example Usage section of this document explains how to use the user interface, although both the button presses and the UART perform the
> same actions.

* `CONFIG_GPIO_RLED` - Set when the collector application is initialized. Flashes when the network is open for joining.
* `CONFIG_GPIO_BTN1` - Press to initialize the collector application.
* `CONFIG_GPIO_BTN2` - Press to allow new devices to join the network. While the network is open for joining, `CONFIG_GPIO_RLED` will flash. Press again to disallow joining.

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
This example project implements a SM-enabled collector device: the PAN-Coordinator for the secure network. This device creates a TI 15.4-Stack secure network, commissions sensor devices, collects sensor information sent by devices running the sensor SM example application, and tracks if the devices are on the network or not by periodically sending tracking request messages. This example assumes a second Launchpad is running the default sensor SM application code.

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
 TI Collector
 Press Enter for Help
<      HELP      >

Security Status: --
Status: Waiting...
Device Status: --
Number of Joined Devices: --
```

The configurable user interface is provided to allow you to make changes at runtime, as follows:
```
    0xFFFF

<      SET PANID      >

Security Status: --
Status: Waiting...
Device Status: --
Number of Joined Devices: --
```

```
 00 00 0F 00 00 00 00 00 00 00 00 00 00 00 00 00 00

<    SET CHAN MASK    >

Security Status: --
Status: Waiting...
Device Status: --
Number of Joined Devices: --
```
```
 12 34 56 78 9A BC DE F0 00 00 00 00 00 00 00 00

<     SET NWK KEY     >

Security Status: --
Status: Waiting...
Device Status: --
Number of Joined Devices: --
```
```
  DEFAULT CODE

<     AUTH METHOD     >

Security Status: --
Status: Waiting...
Device Status: --
Number of Joined Devices: --
```

Note that these changes will only take effect if the collector is in a waiting state.
Keys 0-F can be used to change the value when in edit mode, and left/right keys can be used for navigating the digits.
Once the collector is started, the settings can only be changed if it is restarted.

> If the **AUTO_START** symbol is defined in your application, then the application will automatically configure itself on startup.
> This is not enabled by default with the project, but it can be configured as described below.

**AUTO_START** can be defined by removing the x in the .opts file under the defines folder.
-DxAUTO_START -> -DAUTO_START

> If **AUTO_START** is defined, the collector will display Starting... instead of Waiting...
After the collector has been started, permit join must be turned on, and this can be done either with the right button press on the collector, or through the user interface as shown below.

> If **AUTO_START** is not defined, then the collector will not open a network unless it is started with the left button press, and permit join is turned on with the right button press.
Alternatively, the configurable user interface will allow you to start the collector, and open the network for sensors to join using the following menu options under the NETWORK ACTIONS tab:

To FORM Network:
```
 TI Collector

<       FORM NWK      >

Security Status: --
Status: Waiting...
Device Status: --
Number of Joined Devices: --
```

To OPEN Network:

```
 TI Collector

<       OPEN NWK      >

Security Status: --
Status: Waiting...
Device Status: --
Number of Joined Devices: --
```

To CLOSE Network:

```
 TI Collector

<       CLOSE NWK     >

Security Status: --
Status: Waiting...
Device Status: --
Number of Joined Devices: --
```

* Start the application by pressing `CONFIG_GPIO_BTN1` or selecting `FORM NWK` under the `NETWORK ACTIONS` tab.
```
 TI Collector
 Press Enter for Help
<      HELP      >

Security Status: --
Status: Starting...
Device Status: --
Number of Joined Devices: 0
```

After starting, sensor specific application information will be displayed, such as the devices current state.
```
 TI Collector

<   NETWORK ACTIONS   >

Status: Started--Mode=NBCN, Addr=0xaabb, PanId=0x0001, Ch=0, PermitJoin=Off
Device Status: --
Number of Joined Devices: 0
```

Now turn on permit join using `CONFIG_GPIO_BTN2` or by selecting OPEN NWK under the `NETWORK ACTIONS` tab.
Once the network is started, and sensors begin to join, each of the status lines will update accordingly.
The network coordinator (Collector) will update the Security Status line with the Commissioning progress:

Security Status: Commissioning Started: 0x0001
Security Status: Commissioned: 0x0001

and in the case of any commissioning failures:
Security Status: Commissioning Failed: 0x0001

```
 TI Collector

<   NETWORK ACTIONS   >

Security Status: Commissioned: 0x0001
Status: Started--Mode=NBCN, Addr=0xaabb, PanId=0x0001, Ch=0, PermitJoin=On
Device Status: Sensor - Addr=0001, Temp=29, RSSI=-14
Number of Joined Devices: 1
```


* Key refreshments will occur based on a timeout period set on the collector SM application.
The Security Status line will be updated with one of the following:
```
    Key Refreshment Started: 0x1
    Key Refreshed: 0x1
```
* The network coordinator (Collector) needs to keep two unique keys per device (Seed Key and Device Key). Due to the limit of NV space as of today, all of the keys cannot be stored to the NV space. The NV storage only keeps the sensor device information, not key information.
For real products, it is recommended to store all the keys to internal flash or external flash. Note that the key may need to be encrypted if an external flash space is used for key storage.
The key information in the RAM memory will be lost after power cycle or reset. So, to improve the out of box experience for the secure commission feature, the collector is expected to work normally even after reset or power cycle and following solution has been applied:
The collector recommissions all the devices that have been in its network before it experienced a power glitch. In FH mode of operation, the sensor quickly loses sync with cordinator when the cordinator experiences a power glitch and hence, might not receive the packets for recommissioning sent by co-ordinator and hence the collector prints the following message.
```
    Commissioning Postponed: 0x1
```
In this case, it is expected to wait for a time where the sensor recognizes it got orphaned and joins the network back, when commissioning will happen again and communication between sensor and co-ordinator is restored.
As far as the sesnor is concerned, the device stores the key info of the co-ordinator alone and hence, the same is stored in its NV.

Analyzing Network Traffic
-------------------------

TI 15.4-Stack provides the means to analyze over-the-air traffic by including a packet sniffer firmware image. With an additional CC13x2 or CC26x2 Launchpad, users can set up a packet sniffer with the software provided in the SDK. More information about this can be found in the TI 15.4-Stack documentation under **Packet Sniffer**.

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
| Authentication Methods     | Security category                 | Configures authentication methods supported by the collector  |
| Default Authentication Code| Security category                 | Configures pre-defined code used for authentication           |
| Key Refresh Timeout        | Security category                 | Configures time in secs between sensor key refreshments       |

> SysConfig generated files are dynamically generated upon build, and any manual changes to them will be overwritten.

More information about the configuration and feature options can be found in the TI 15.4-Stack documentation under **Example Applications > Configuration Parameters**.

### Disabling Common User Interface

The common user interface (CUI) is a UART based interface that allows users to control and receive updates regarding the application. For various reasons, including reducing the memory footprint, the user is able to disable the common user interface (CUI). To disable the CUI, the following variable must be defined in the project-specific .opts file:

```
-DCUI_DISABLE
```

> Please Note: particular features that are dependednt on the CUI wil be unavailable when this feature is enabled.

### Multi-Page NV Configuration

By default, this project is configured to use four pages of NV. A maximum of five NV pages are supported. In order to modify the NV pages, update the following:
* `NVOCMP_NVPAGES=4` in the project-specific .opts file
* `NVOCMP_NVPAGES=4` in the linker options
* SysConfig NVS module:
   * Set Region Size based on the formula `NVOCMP_NVPAGES * 0x2000`
   * Set Region Base based on the formula `0x56000 - (NVOCMP_NVPAGES * 0x2000)`

A detailed description of the application architecture can be found in your installation within the
TI-15.4 Stack Getting Started Guide's Application Overview section.

&lt;SDK_INSTALL_DIR&gt;/docs/ti154stack/ti154stack-getting-started-guide.html.