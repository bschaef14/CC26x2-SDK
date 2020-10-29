
## Example Summary

The WSN Node example illustrates how to create a Wireless Sensor Network Node device
which sends packets to a concentrator. This example is meant to be used with the WSN
Concentrator example to form a one-to-many network where the nodes send messages to
the concentrator.

This examples showcases the use of several Tasks, Semaphores, and Events to get sensor
updates and send packets with acknowledgement from the concentrator. For the radio
layer, this example uses the EasyLink API which provides an easy-to-use API for the
most frequently used radio operations.

## Peripherals Exercised
* `CONFIG_PIN_RLED` - Toggled when data is received over the RF interface
* `CONFIG_PIN_BTN1` - Select Node
* `CONFIG_PIN_BTN2` - Select Action

## Resources & Jumper Settings

> If you're using an IDE (such as CCS or IAR), please refer to Board.html in your project
directory for resources used and board-specific jumper settings. Otherwise, you can find
Board.html in the directory &lt;SDK_INSTALL_DIR&gt;/source/ti/boards/&lt;BOARD&gt;.

## Example Usage

* Run the example. On another board (or several boards) run the WSN Node example.
The LCD will show the discovered node(s). When the collector receives data from
a new node, it is given a new row on the display and the received value is shown.
If more than 7 nodes are detected, the device list rolls over, overriding
the first. Whenever an updated value is received from a node, it is updated on
the LCD display.

The example also supports Over The Air Download (OAD), where new FW can be transferred
from the concentrator to the node. There must be an OAD Server, which is included in
the concentrator project, and an OAD client, which is included in the node  project.

** See rfWsnNodeExtFlashOadClient project README for instructions on how to generate OAD images **

### Performing an OAD Image Transfer

To be safe the external flash of the Concentrator should be wiped before running the
example. To do this, program both LP boards with erase_storage_offchip_cc13x2lp.hex. The
program will flash the LEDs while erasing the external flash. Allow the application to
run until the LEDs stop flashing indicating the external flash has been erased.

The FW to erase the external flash can be found in below location and should be loaded
using Uniflash programmer:
`<SDK_DIR>/examples/rtos/CC1352R1_LAUNCHXL/easylink/hexfiles/offChipOad/ccs/erase_storage_offchip_cc13x2lp.hex`

#### OAD with Easylink

The Concentrator OAD Server and Node OAD Client FW should each be loaded into a
CC1312R1LP/CC1352R1LP using the Uniflash programmer:

- Load rfWsnConcentratorOadServer (.out) project into a CC1312R1LP/CC1352R1LP
- Load `<SDK_DIR>/examples/rtos/CC1352R1_LAUNCHXL/easylink/hexfiles/offChipOad/ccs/rfWsnNodeExtFlashOadClient_CC1352R1_LAUNCHXL_app_v1.hex` and `<SDK_DIR>/examples/rtos/CC1352R1_LAUNCHXL/easylink/hexfiles/offChipOad/ccs/cc13x2r1lp_bim_offchip.hex` into a CC1312R1LP/CC1352R1LP by selecting multiple
files in Uniflash

The Concentrator will display the below on the UART terminal:

```shell
Nodes   Value   SW    RSSI
*0x0b    0887    0    -080
 0xdb    1036    0    -079
 0x91    0940    0    -079
Action: Update available FW
Info: Available FW unknown
```

Use the node display to identify the corresponding node ID:

```shell
Node ID: 0x91
Node ADC Reading: 1196
```

The node OAD image can be loaded into the external flash of the Concentrator through
the UART with the oad_write_bin.py script. The action must first be selected using
BTN-2. Press BTN-2 until the Action is set to `Update available FW`, then press BTN-1
and BTN-2 simultaneously to execute the action.

When "Available FW" is selected the terminal will display:

```shell
    Waiting for Node FW update...
```

The UART terminal must be closed to free the COM port before the script is run. Then
the python script can be run using the following command:

```shell
    python <SDK>/tools/easylink/oad/oad_write_bin.py /dev/ttyS28 <SDK_DIR>/examples/rtos/CC1352R1_LAUNCHXL/easylink/hexfiles/offChipOad/ccs/rfWsnNodeExtFlashOadClient_CC1352R1_LAUNCHXL_app_v2.bin
```

After the download the UART terminal can be re-opened and the "Info" menu line will be
updated to reflect the new FW available for OAD to a node.

The current FW version running on the node can be requested using the `Send FW Ver Req`
action. This is done by pressing BTN-1 until the desired node is selected (indicated by
the *), then pressing BTN-2 until the Action is set to `Send FW Ver Req`. To execute the
action, press BTN-1 and BTN-2 simultaneously.

The next time the node sends data to the concentrator, the FW version of the selected
node will appear in the Info section.

```shell
Nodes   Value   SW    RSSI
 0x0b    0887    0    -080
 0xdb    1036    0    -079
*0x91    0940    0    -079
Action: Send FW Ver Req
Info: Node 0x91 FW sv:0001, bv:01
```
Where:
* `sv` is the FW version number
* `bv` is the version number of the BIM

The node FW can now be updated to the image stored on the external flash of the
concentrator. Press BTN-1 until the desired node is selected, then press BTN-2 until
the Action is set to `Update node FW`. To execute the action, press BTN-1 and BTN-2
simultaneously.


The next time the node sends data, the OAD sequence will begin. As the node requests
each image block from the concentrator the Concentrator display is updated to show the
progress of the image transfer.

```shell
Nodes   Value   SW    RSSI
 0x0b    0887    0    -080
 0xdb    1036    0    -079
*0x91    0940    0    -079
Action: Update node FW
Info: OAD Block 14 of 1089
```
The node display also updates to show the status of the image transfer.
```shell
Node ID: 0x91
Node ADC Reading: 3093
OAD Block: 14 of 1089
OAD Block Retries: 0
```


Once the OAD has completed, the concentrator will indicate that the transfer has
finished with an `OAD Complete` status. The node will reset itself with a new node ID.
If the device does not reset itself a manual reset may be necessary.

```shell
Nodes   Value   SW    RSSI
 0x0b    0887    0    -080
 0xdb    1036    0    -079
*0xe2    0940    0    -079
Action: Update node FW
Info: Node 0xe2 FW Unknown
```

A firmware version request can then be performed to verify the new image.


#### OAD with BLE

The BLE Host Application and BLE Simple Peripheral with OAD should each be loaded into a
CC1312R1LP/CC1352R1LP using the Uniflash programmer:

- Load `<SDK_DIR>/examples/rtos/CC1352R1_LAUNCHXL/ble5/hexfiles/ble5_host_test_cc13x2r1lp_app.hex`  into a CC1312R1LP/CC1352R1LP

- Load `<SDK_DIR>/examples/rtos/CC1352R1_LAUNCHXL/ble5/hexfiles/oad/ble5_simple_peripheral_oad_offchip_cc13x2r1lp_app_FlashROM_Release_oad.bin` and `<SDK_DIR>/examples/rtos/CC1352R1_LAUNCHXL/easylink/hexfiles/offChipOad/ccs/cc13x2r1lp_bim_offchip.hex` into a CC1312R1LP/CC1352R1LP

When BTN-1 Button is held down and the device is reset by pressing the RESET Button or
disconnecting/reconnecting the device to a power source the device will boot into the Factory Image located in external flash. If there is no Factory Image, the image stored in internal flash is copied to external flash as the factory image. After loading the BLE Simple Peripheral image on the device, perform a factory reset to copy the BLE Simple Peripheral image as the Factory Image.

- Replace the image on the BLE Simple Peripheral device by loading `<SDK_DIR>/examples/rtos/CC1352R1_LAUNCHXL/easylink/hexfiles/offChipOad/ccs/rfWsnNodeExtFlashOadClient_CC1352R1_LAUNCHXL_app_v1.hex` using Uniflash

A factory reset can be performed at any time to perform an OAD now. Perform a factory reset to continue.

Open up Btool which can be found in
- `<SDK_DIR>/tools/ble5stack/btool`

Select the UART port that corresponds to the device running the BLE Host application. All
other Serial Port Settings should be left as the defaults. Select the `Scan` button in the
GUI. When the scan has finished, select the address of the BLE Simple Peripheral device and
press the `Establish` button to connect the two devices.

** Note  the BLE Address can be seen by looking at UART output of the device or by using
Uniflash and navigating to Settings & Utilities &rarr; Read Primary BLE Address **

When the connection has been established, navigate to the `Over The Air Download` tab in
the top left hand corner of the GUI.

Select `Read Image File` and navigate to the Easylink rfWsnNodeExtFlashOad image created
earlier. Make sure to select the `OAD To External Flash` check box. Start the image
transfer by clicking the `Send` button.

When the OAD has completed, the device will boot up into the new rfWsnNodeExtFlashOad image and native OAD can now be performed as previously described.


## Application Design Details

This examples consists of two tasks, one application task and one radio
protocol task.

The ConcentratorRadioTask handles the radio protocol. This sets up the EasyLink
API and uses it to always wait for packets on a set frequency. When it receives
a valid packet, it sends an ACK and then forwards it to the ConcentratorTask.

The ConcentratorTask receives packets from the ConcentratorRadioTask, displays
the data on the LCD and toggles CONFIG_PIN_RLED.

* *RadioProtocol.h* can also be used to configure the PHY settings from the following
options: IEEE 802.15.4g 50kbit (default), Long Range Mode or custom settings. In the
case of custom settings, the *ti_radio_config.c* file is used. The configuration can
be changed by exporting a new ti_radio_config.c file from Smart RF Studio or
modifying the file directly.

## References
* For more information on the EasyLink API and usage refer to the [Proprietary RF User's guide](http://dev.ti.com/tirex/#/?link=Software%2FSimpleLink%20CC13x2%2026x2%20SDK%2FDocuments%2FProprietary%20RF%2FProprietary%20RF%20User's%20Guide)