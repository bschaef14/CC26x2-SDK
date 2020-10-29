# rfEasyLinkListenBeforeTalk

---

Project Setup using the System Configuration Tool (SysConfig)
-------------------------
The purpose of SysConfig is to provide an easy to use interface for configuring
drivers, RF stacks, and more. The .syscfg file provided with each example
project has been configured and tested for that project. Changes to the .syscfg
file may alter the behavior of the example away from default. Some parameters
configured in SysConfig may require the use of specific APIs or additional
modifications in the application source code. More information can be found in
SysConfig by hovering over a configurable and clicking the question mark (?)
next to it's name.

### EasyLink Stack Configuration
Many parameters of the EasyLink stack can be configured using SysConfig
including RX, TX, Radio, and Advanced settings. More information can be found in
SysConfig by hovering over a configurable and clicking the question mark (?)
next to it's name. Alternatively, refer to the System Configuration Tool
(SysConfig) section of the Proprietary RF User's guide found in
&lt;SDK_INSTALL_DIR&gt;/docs/proprietary-rf/proprietary-rf-users-guide.html.

Example Summary
---------------
This example includes the EasyLink API and uses it to configure the RF driver to
assess the communication channel before transmitting any packets, i.e.  Listen
Before Talk (LBT); the radio will first enter RX mode to assess the channel, if
it is idle (RSSI below a set threshold) for 5ms it proceeds to transmit a
packet. If the channel is busy (RSSI above the set threshold), at any point in
the channel sense window, the radio will back off for a random period before
assessing the channel again; it will do this a certain number of times before
failing altogether.

This project can be run in conjunction with the rfEasyLinkRx project; a third
board, managed by SmartRF Studio, emitting a continuous carrier wave may be
used as a jammer to simulate high load networks.

For more information on the EasyLink API and usage refer to the [Proprietary RF User's guide](http://dev.ti.com/tirex/#/?link=Software%2FSimpleLink%20CC13x2%2026x2%20SDK%2FDocuments%2FProprietary%20RF%2FProprietary%20RF%20User's%20Guide)


Peripherals Exercised
---------------------
* `CONFIG_PIN_GLED` - Indicates that a packet has been transmitted
* `CONFIG_PIN_RLED` - Indicates that the CCA algorithm had failed to detect an
  idle channel during which to transmit a given packet
 `CONFIG_PIN_GLED` & `CONFIG_PIN_RLED` indicate an error

Resources & Jumper Settings
---------------------------
> If you're using an IDE (such as CCS or IAR), please refer to Board.html in your project
directory for resources used and board-specific jumper settings. Otherwise, you can find
Board.html in the directory &lt;SDK_INSTALL_DIR&gt;/source/ti/boards/&lt;BOARD&gt;.

Example Usage
-------------
The example code can be tested with a receiver managed by SmartRF Studio,
or one board running the rfEasyLinkRx example as the receiver, and another board
connected to SmartRF Studio, acting as a jammer at 868.0 MHz (433.92 MHz for
the CC1350-LAUNCHXL-433 board).

The transmitter will attempt to transmit a packet every 100 ms. CONFIG_PIN_GLED
will toggle each time a packet is successfully transmitted. CONFIG_PIN_RLED will
toggle if the Clear Channel Assessment (CCA) fails and the packet is not
transmitted. If the macro RFEASYLINKLBT_RETRANSMIT_PACKETS is set to 1 the
application will attempt to retransmit any failed packets indefinitely; if set
to 0 it will skip the failed packet and move on to the next packet in sequence.
This cycle will continue.

Before running this application you should first start the rfEasyLinkRx on a
second board to see that the transmitted packets are received.

Application Design Details
--------------------------
This example shows how to use the EasyLink API to access the RF driver, set the
frequency, assess the channel and transmit packets. The TX API used is
asynchronous (non-blocking).

The rfEasyLinkListenBeforeTalk example will transmit a packet every 100 ms if
the channel is idle. The TX API used will run a Clear Channel Assessment (CCA)
algorithm to determine if the channel is idle for a specified time window
(EASYLINK_CHANNEL_IDLE_TIME_US); it does this by comparing the channel RSSI
against a set threshold, if it is lower than the threshold the
channel is considered to be idle, else it is busy. If the channel is busy, at
any point in the channel sense window, the radio will back off for a certain
time period before reassessing the channel, it does this a certain number of
times before giving up and declaring a "channel busy" failure. CONFIG_PIN_RLED
will indicate when this condition occurs. For every successful transmission
CONFIG_PIN_GLED will be toggled. An error has occurred if both CONFIG_PIN_GLED
and CONFIG_PIN_RLED are toggled.

A single task, "rfEasyLinkLbtFnx", configures the RF driver through the EasyLink
API and transmits messages.

EasyLink API
-------------------------
### Overview
The EasyLink API should be used in application code. The EasyLink API is
intended to abstract the RF Driver in order to give a simple API for
customers to use as is or extend to suit their application[Use Cases]
(@ref USE_CASES).

### General Behavior
Before using the EasyLink API:

  - The EasyLink Layer is initialized by calling EasyLink_init(). This
    initializes and opens the RF driver and configures a modulation scheme
    passed to EasyLink_init.
  - The RX and TX can operate independently of each other.

The following is true for receive operation:

  - RX is enabled by calling EasyLink_receive() or EasyLink_receiveAsync()
  - Entering RX can be immediate or scheduled
  - EasyLink_receive() is blocking and EasyLink_receiveAsync() is non-blocking
  - The EasyLink API does not queue messages so calling another API function
    while in EasyLink_receiveAsync() will return EasyLink_Status_Busy_Error
  - An Async operation can be cancelled with EasyLink_abort()

The following apply for transmit operation:

  - TX is enabled by calling EasyLink_transmit(), EasyLink_transmitAsync()
    or EasyLink_transmitCcaAsync()
  - TX can be immediate or scheduled
  - EasyLink_transmit() is blocking and EasyLink_transmitAsync(),
    EasyLink_transmitCcaAsync() are non-blocking
  - EasyLink_transmit() for a scheduled command, or if TX cannot start
  - The EasyLink API does not queue messages so calling another API function
    while in either EasyLink_transmitAsync() or EasyLink_transmitCcaAsync()
    will return EasyLink_Status_Busy_Error
  - An Async operation can be cancelled with EasyLink_abort()

### Error Handling
The EasyLink API will return EasyLink_Status containing success or error
  code. The EasyLink_Status codes are:

   - EasyLink_Status_Success
   - EasyLink_Status_Config_Error
   - EasyLink_Status_Param_Error
   - EasyLink_Status_Mem_Error
   - EasyLink_Status_Cmd_Error
   - EasyLink_Status_Tx_Error
   - EasyLink_Status_Rx_Error
   - EasyLink_Status_Rx_Timeout
   - EasyLink_Status_Busy_Error
   - EasyLink_Status_Aborted

### Power Management
The power management framework will try to put the device into the most
power efficient mode whenever possible. Please see the technical reference
manual for further details on each power mode.

The EasyLink Layer uses the power management offered by the RF driver Refer to the RF
Driver documentation for more details.

### Supported Functions
    | Generic API function          | Description                                        |
    |-------------------------------|----------------------------------------------------|
    | EasyLink_init()               | Init's and opens the RF driver and configures the  |
    |                               | specified settings based on EasyLink_Params struct |
    | EasyLink_transmit()           | Blocking Transmit                                  |
    | EasyLink_transmitAsync()      | Non-blocking Transmit                              |
    | EasyLink_transmitCcaAsync()   | Non-blocking Transmit with Clear Channel Assessment|
    | EasyLink_receive()            | Blocking Receive                                   |
    | EasyLink_receiveAsync()       | Non-blocking Receive                                |
    | EasyLink_abort()              | Aborts a non blocking call                         |
    | EasyLink_enableRxAddrFilter() | Enables/Disables RX filtering on the Addr          |
    | EasyLink_getIeeeAddr()        | Gets the IEEE Address                              |
    | EasyLink_setFrequency()       | Sets the frequency                                 |
    | EasyLink_getFrequency()       | Gets the frequency                                 |
    | EasyLink_setRfPower()         | Sets the Tx Power                                  |
    | EasyLink_getRfPower()         | Gets the Tx Power                                  |
    | EasyLink_getRssi()            | Gets the RSSI                                      |
    | EasyLink_getAbsTime()         | Gets the absolute time in RAT ticks                |
    | EasyLink_setCtrl()            | Set RF parameters, test modes or EasyLink options  |
    | EasyLink_getCtrl()            | Get RF parameters or EasyLink options              |
    | EasyLink_getIeeeAddr()        | Gets the IEEE address                              |

### Frame Structure
The EasyLink implements a basic header for transmitting and receiving data. This header supports
addressing for a star or point-to-point network with acknowledgements.

Packet structure:

     _________________________________________________________
    |           |                   |                         |
    | 1B Length | 1-64b Dst Address |         Payload         |
    |___________|___________________|_________________________|


Note for IAR users: When using the CC1310DK, the TI XDS110v3 USB Emulator must
be selected. For the CC1310_LAUNCHXL, select TI XDS110 Emulator. In both cases,
select the cJTAG interface.
