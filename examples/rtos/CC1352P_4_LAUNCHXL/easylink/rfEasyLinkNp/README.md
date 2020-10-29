# rfEasyLinkNp

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
The EasyLink API has been exposed over an AT Command Interface such that it can
be exercised by Host SW (running on an PC, MPU or MCU) or by a human using a
serial terminal emulator.

For more information on the EasyLink API and usage refer to the [Proprietary RF User's guide](http://dev.ti.com/tirex/#/?link=Software%2FSimpleLink%20CC13x2%2026x2%20SDK%2FDocuments%2FProprietary%20RF%2FProprietary%20RF%20User's%20Guide)

Peripherals Exercised
---------------------
* `CONFIG_DISPLAY_UART` - Used to access the AT interface from a host serial session

Resources & Jumper Settings
---------------------------
> If you're using an IDE (such as CCS or IAR), please refer to Board.html in your project
directory for resources used and board-specific jumper settings. Otherwise, you can find
Board.html in the directory &lt;SDK_INSTALL_DIR&gt;/source/ti/boards/&lt;BOARD&gt;.

Board Specific Settings
-----------------------
1. If the Np example is initialized to the default mode, i.e. the custom setting,
the center frequency is set to:
    - 433.92 MHz for the CC1350-LAUNCHXL-433
    - 433.92 MHz for the CC1352P-4-LAUNCHXL
    - 2440 MHz on the CC2640R2-LAUNCHXL and CC26X2R1_LAUNCHXL
    - 868.0 MHz for other launchpads
In order to change frequency, modify the ti_radio_config.c file. This can be
done using the code export feature in Smart RF Studio, or directly in the file
2. On the CC1352P1 the high PA is enabled (high output power) for all
Sub-1 GHz modes by default.
3. On the CC1352P-2 and CC1352P-4 the high PA operation for Sub-1 GHz modes is not supported
4. The CC2640R2 is setup to run all proprietary physical modes at a center
frequency of 2440 MHz, at a data rate of 250 Kbps

Example Usage
-------------
Open a serial session (e.g. HyperTerminal,puTTY, etc.) to the appropriate COM
port. Note: the COM port can be determine via Device Manager in Windows or via
ls /dev/tty* in Linux.

The connection should have the following settings:

    Baud-rate:    115200
    Data bits:    8
    Stop bits:    1
    Parity:       None
    Flow Control: None

If the serial session is started before the target completes initialization,
the reset indication and version number is displayed:
"RESET:vxx.xx.xx"

By default the target echoes back any character that is typed in the serial session.

Note: Two boards are recommended in order to see the effect of these commands
    by running both the rfEasyLinkNp and the rfEasyLinkRx examples.


AT API
-------------

The AT Command Interface uses ASCII characters so that a terminal emulator can
send the commands, but also uses framing so that SW can format and parse
the AT commands.

The frame format is shown below

    | Start of Frame | Command Type  | Command ID  | Parameters | End Of Frame |
    |================|===============|=============|============|==============|
    | "AT"           | 'P'/'+'       | "i"         | "0001"     |  &lt;CR>       |
    |----------------|---------------|-------------|------------|--------------|

An example command is:

        AT+I 0001<CR>

The EasyLink AT Command Interface uses 2 command types:

1. Pxx: Parameters
1. +x: Control Commands

AT Parameters
---------------

Parameters offer set and get functionality.

The format of a parameter read command to get the TxPower parameter is:

        ATP00?<CR>

The response to a read will be of the format shown below, depending on the
parameter it will be hex or decimal:

        -10<CR>

The format of an AT command to set the Frequency Parameter to 868MHz is:

        ATPFR=868000000<CR>

The response of a write will be of the format:

        OK<CR>

The registers exposed for the EasyLink API are:

    | Param | R/W | Description                      | Parameter(s)               |
    |=======|=====|==================================|============================|
    |  ST   |  R  | Read The last EasyLink status    | EasyLink status in 4B:     |
    |       |     | returned                         | hex<br> Success = 0000     |
    |       |     |                                  | Config_Error = 0001        |
    |       |     |                                  | Param_Error = 0002         |
    |       |     |                                  | Mem_Error = 0003           |
    |       |     |                                  | Cmd_Error = 0004           |
    |       |     |                                  | Tx_Error = 0005            |
    |       |     |                                  | Rx_Error = 0006            |
    |       |     |                                  | Rx_Timeout = 0007          |
    |       |     |                                  | Rx_Buffer_Error = 0008     |
    |       |     |                                  | Busy_Error = 0009          |
    |       |     |                                  | Aborted = 000a             |
    |-------|-----|----------------------------------|----------------------------|
    |  AE   | R/W | UART Echo Enable                 | 0 or 1 to enable/disable   |
    |       |     |                                  | echo                       |
    |-------|-----|----------------------------------|----------------------------|
    |  FR   | R/W | Read/Write frequency in kHz      | Frequency in 1B hex        |
    |-------|-----|----------------------------------|----------------------------|
    |  PW   | R/W | Read/Write tx power in dBm       | Power in decimal between   |
    |       |     |                                  | -10 to 14dBm (20dBm on     |
    |       |     |                                  | 1352P). Note ccfg changes  |
    |       |     |                                  | are required for 14dBm     |
    |       |     |                                  | output power; this applies |
    |       |     |                                  | to CC12x2P running the     |
    |       |     |                                  | default PA                 |
    |-------|-----|----------------------------------|----------------------------|
    |  BM   | R/W | Read/Write data mode for Tx/Rx   | Mode in 1B hex 0:ASCII     |
    |       |     | data                             | 1:Binary                   |
    |-------|-----|----------------------------------|----------------------------|
    |  IE   |  R  | Read IEEE address                | None                       |
    |-------|-----|----------------------------------|----------------------------|
    |  AS   |  R  | Read/Write address size in Bytes | Address is restricted to   |
    |       |     |                                  | 0, 1, 2, 4 or 8 bytes. 0B  |
    |       |     |                                  | disables address filtering |
    |       |     |                                  | on the Rx side, prevents Tx|
    |       |     |                                  | from inserting address     |
    |       |     |                                  | bytes in the payload       |
    |-------|-----|----------------------------------|----------------------------|
    |  TA   | R/W | Read/Write Tx address            | 1-8B Tx address in hex     |
    |-------|-----|----------------------------------|----------------------------|
    |  RT   |  R  | Read current radio time          | None                       |
    |-------|-----|----------------------------------|----------------------------|
    |  TY   | R/W | Read/Write Time Type             | Time in 1B hex 0:Absolute  |
    |       |     |                                  | Time 1:Relative Time       |
    |-------|-----|----------------------------------|----------------------------|
    |  TT   | R/W | Absolute or relative (based on   | Absolute/relative time in  |
    |       |     | Time Type) radio time to Tx a    | units of 4MHz ticks in     |
    |       |     | packet                           | decimal OR 0 for immediate |
    |-------|-----|----------------------------------|----------------------------|
    |  TR   | R/W | Absolute or relative (based      | Absolute/relative time in  |
    |       |     | on Time Type) radio time to Rx a | units of 4MHz ticks in     |
    |       |     | packet                           | decimal OR 0 for immediate |
    |-------|-----|----------------------------------|----------------------------|
    |  RO   | R/W | Relative time for Rx timeout     | Relative time in units of  |
    |       |     |                                  | 4MHz ticks in decimal OR 0 |
    |       |     |                                  | for never                  |
    |-------|-----|----------------------------------|----------------------------|
    |  LA   |  R  | Destination address of last Rx'ed| None                       |
    |       |     | message                          |                            |
    |-------|-----|----------------------------------|----------------------------|
    |  LT   |  R  | Read absolute radio time of last | None                       |
    |       |     | Rx'ed message                    |                            |
    |-------|-----|----------------------------------|----------------------------|
    |  CR   |  R  | Read Current RSSI when the radio | None                       |
    |       |     | is in the receiver mode          |                            |
    |-------|-----|----------------------------------|----------------------------|
    |  LR   |  R  | Read RSSI of last Rx'ed message  | None                       |
    |-------|-----|----------------------------------|----------------------------|
    |  F0   | R/W | Read/Write address filter 0      |  1-8B address in hex       |
    |-------|-----|----------------------------------|----------------------------|
    |  F1   | R/W | Read/Write address filter 1      |  1-8B address in hex       |
    |-------|-----|----------------------------------|----------------------------|
    |  F2   | R/W | Read/Write address filter 2      |  1-8B address in hex       |
    |-------|-----|----------------------------------|----------------------------|
    |  TM   | R/W | Read/Write test mode             | Test mode in 1B hex        |
    |       |     |                                  | 0:None/Cancel              |
    |       |     |                                  | 1:Tone/Carrier Wave        |
    |       |     |                                  | 2:Modulated Signal         |
    |       |     |                                  | 3:PER Tx                   |
    |       |     |                                  | 4:PER Rx                   |
    |       |     |                                  | 5:Tone/Carrier Wave Rx     |
    |-------|-----|----------------------------------|----------------------------|
    |  PI   | R/W | Read/Write PER Tx Bursts Interval| 1B time between PER bursts |
    |       |     |                                  | in units of ms in Decimal  |
    |-------|-----|----------------------------------|----------------------------|
    |  PB   | R/W | Read/Write PER Tx Burst Size     | 1B Tx Burst Size in Hex    |
    |-------|-----|----------------------------------|----------------------------|
    |  PP   | R/W | Read/Write Number of PER Tx/Rx   | 1B Tx/Rx Packets in Hex    |
    |       |     | Packets                          |                            |
    |-------|-----|----------------------------------|----------------------------|
    |  PL   | R/W | Read/Write PER Tx/Rx Packet      | 1B Tx/Rx Packet Length in  |
    |       |     | Length                           | Hex                        |
    |-------|-----|----------------------------------|----------------------------|
    |GM00-03| R/W | Read/Write GPIO Mode             | 0:1 GPIO Value input/output|
    |-------|-----|----------------------------------|----------------------------|
    |GV00-03| R/W | Read/Write GPIO Value            | 0:1 GPIO Value             |
    |-------|-----|----------------------------------|----------------------------|


It is important to note that most Register commands call underlying EasyLink
functions that require the Initialize Command "AT+I x" to be run.

AT Control Commands
-------------------

The format of Control Commands are:

        AT+I 00<CR>

The response will be similar to:

        OK<CR>

The Commands exposed for the EasyLink AT API are:

    | Command  | Description                        | Parameter(s)                |
    |==========|====================================|=============================|
    |   i/I    | Initialize the Radio               | 00: For User defined from   |
    |          |                                    | ti_radio_config.c/h         |
    |          |                                    | 01: For 50kbps 2-GFSK Mode (|
    |          |                                    | CC13xx only)                |
    |          |                                    | 02: For 625bps Long Range   |
    |          |                                    | ti_radio_config.c/h         |
    |          |                                    | Mode (CC13xx only)          |
    |          |                                    | 03: For 200kbps 2.4G 2-GFSK |
    |          |                                    | Mode (CC2650 only)          |
    |          |                                    | 04: For 5kbps SimpleLink    |
    |          |                                    | Long Range Mode             |
    |          |                                    | 05: For 100kbps 2.4G 2-GFSK |
    |          |                                    | Mode (CC1352 and CC26X2)    |
    |          |                                    | 06: For 250kbps 2.4G 2-GFSK |
    |          |                                    | Mode (CC1352 and CC26X2)    |
    |          |                                    | 07: For 200kbps 2-GFSK Mode |
    |          |                                    | (CC13X2 only)               |
    |----------|------------------------------------|-----------------------------|
    |  tx/TX   | Send a message to Tx Address       | x Data Bytes                |
    |          | (PARAM PTA) at Tx Time (Param PTT) |                             |
    |          | or Immediate if 0 or expired       |                             |
    |----------|------------------------------------|-----------------------------|
    |  rx/RX   | Turn on Radio to Receive Data at   | None                        |
    |          | Rx Time set by Parameter `PRT` (or |                             |
    |          | Immediate if 0), with timeout set  |                             |
    |          | by Parameter "PRO"                 |                             |
    |----------|------------------------------------|-----------------------------|
    |  rs/RS   | Reset the CC13xx                   | None                        |
    |----------|------------------------------------|-----------------------------|

__Command Responses__

Response for Register Write and Control Commands are formatted as:

    | Response       | Description                                                |
    |================|============================================================|
    | OK<CR>         | Command or Register write successful                       |
    |----------------|------------------------------------------------------------|
    | Error 0001<CR> | Command or register read/write failed due to bad formatting|
    |----------------|------------------------------------------------------------|
    | Error 0002<CR> | Command or register read/write failed due to bad length    |
    |----------------|------------------------------------------------------------|
    | Error 0003<CR> | Command or register write failed due to a parameter Error  |
    |----------------|------------------------------------------------------------|
    | Error 0004<CR> | Command or register write failed due to a Memory Error     |
    |----------------|------------------------------------------------------------|
    | Error 0005<CR> | Command or register write failed due to Error From         |
    |                | EasyLink API (EasyLink error is stored in Parameter "ST")  |
    |----------------|------------------------------------------------------------|

The response to the "rx"\"RX" (receive) Command is of the format:

- For ASCII Data Mode:

        RX: Hello World<CR>
        OK<CR>

- For Binary Data Mode:

        RX: 2fbb1aa8ec84045fb0c3e5236cb8cc5b3c<CR>
        OK<CR>

The response to the "rs"\"RX" (reset) Command is of format:

- For ASCII Data Mode:

        RESET:vxx.xx.xx<CR>

Where vxx.xx.xx is the version number of the EasyLink API

## Test Modes

The EasyLink AT interface supports the following test modes:

- Carrier Wave
- Modulated Signal
- PER Tx
- PER Rx
- Tone/Carrier Wave Rx

Before running these commands you must first initialize the device using:

        AT+I x<CR>

where x indicate the Phy settings used.

### Carrier Wave:

This is enabled by Setting test mode to 1. To configure the Carrier Wave
test mode use:

        ATPTM=1<CR>

To exit the Carrier Wave test mode:

        ATPTM=0<CR>

### Modulated Signal:

This is enabled by Setting test mode to 2. To configure the Modulated Signal
test mode use:

        ATPTM=2<CR>

To exit the Modulated Signal test mode:

        ATPTM=0<CR>

Note, that the Modulated Signal test mode can only run once. For correct
operation setting the Modulated Signal for a second time requires a reset command:

        AT+RS<CR>

### PER Tx:

This is enabled by setting test mode to 3, it will run for the number of
packets configure in the `PP` Parameter, or indefinitely number of packets
is set to 0. To configure PER Tx test mode to run for 100 packets:

        ATPPP=100<CR>
        ATPTM=3<CR>

While the test is running the below responses should be expected:

        TPER: 00
        TPER: 01
        TPER: 02
        ...

Once the test completes the below response should be expected.

        TPER: Done OK<CR>

Related Parameters to this test are:

- PPL: PER Tx Packet Length
- PPB: PER Tx Burst size
- PPI: Interval in ms between bursts

### PER Rx:

This is enabled by setting test mode to 4, it will run for the number of
packets configure in the `PP` Parameter, or indefinitely number of packets
is set to 0. To configure PER Rx test mode to run for 100 packets:

        ATPPP=100<CR>
        ATPTM=4<CR>

While the test is running the below responses should be expected:

        RPER: 00, 0001, 0000, -31
        RPER: 01, 0002, 0000, -31
        RPER: 02, 0003, 0000, -31
        ...

The columns are:

        RPER: <Sequence Number>, <Pass Count>, <Fail Count>, <RSSI>

The sequence number is sent from the Tx PER, and is expected to increment by
1 in each packet. Pass Count increments every time a correct sequence number
is received. Fail Count increments every time an incorrect sequence number
is received.

Once the test completes the below response should be expected.

        <RPER>: Done OK

### Tone/Carrier Wave Rx:

This is enabled by Setting test mode to 5. To configure the Tone/Carrier Wave
Rx test mode use:

        ATPTM=5<CR>

The user may request the RSSI data from the radio by polling the CR (Current
RSSI), while the radio is running in receive mode, by

        ATPCR?

A value of -128 indicates an error in the RSSI value.

To exit this test mode:

        ATPTM=0<CR>


Note for IAR users: When using the CC1310DK, the TI XDS110v3 USB Emulator must
be selected. For the CC1310_LAUNCHXL, select TI XDS110 Emulator. In both cases,
select the cJTAG interface.
