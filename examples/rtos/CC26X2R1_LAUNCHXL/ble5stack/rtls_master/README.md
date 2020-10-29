# RTLS System

## Table of Contents

* [General Information](#general-information)
* [Introduction](#Introduction)
* [Angle of Arrival - Setup](#aoa-setup)
	* [Software Setup](#aoa-software-setup)
	* [Hardware Setup](#aoa-hardware-setup)
	* [Running The Example](#aoa-running-the-example)

# <a name="general-informatio"></a>General Information

1. Detailed guide can be found on [SimpleLink Academy](http://dev.ti.com/tirex/explore/node?node=ADoNVRzIjRH1Ft0aHjErQg__pTTHBmu__LATEST)
2. To get usage information for the RTLS Node Manager scripts and GUI please refer to: `tools\ble5stack\rtls_agent\README.md`
3. In order to edit and view how the RTLS subsystem handles uNPI commands, please refer to the following file: `tools\ble5stack\rtls_agent\rtls\rtls\ss_rtls.py`
4. In order to see a usage example, please refer to `tools\ble5stack\rtls_agent\examples\rtls_example.py` which shows how to use a RTLS Master + RTLS Slave + RTLS Passive combination
5. To access GUI, please refer [TI Gallery](https://dev.ti.com/gallery/) and look for RTLS Monitor project.

# <a name="Introduction"></a>Introduction

This readme contains the following information:
1. How to setup AoA demo

# <a name="aoa-setup"></a>Angle of Arrival - Setup
## <a name="aoa-software-setup"></a>Software Setup
AoA is currently supported on all roles with the following caveats:

#### RTLS Master
* Master role will collect I/Q samples
* Master role supports the official BT5.1 Connected AoA implementation
* AoA API's can be accessed by using RTLS Services host module

#### RTLS Slave
* Slave will send out a constant tone at the end of connection packet and it will not collect I/Q samples
* Slave supports the official BT5.1 Connected AoA implementation
* AoA API's can be accessed by using RTLS Services host module
* The tone length is configured by the user using the CTE Time parameter (refer to `tools\ble5stack\rtls_agent\rtls\rtls\ss_rtls.py`). **Note that RTLS Passive only supports reception of a tone of length 20**

#### RTLS Passive
* Passive will collect I/Q samples with the following limitation: **CTE Time is always configured to 20, Sample Rate is always 4Mhz and Sample Size is always 16 bit**
* When using AoA raw output mode (AOA_MODE_RAW), consider the following:
    1. Connection interval of BLE must be at least 300ms to accomodate outputing all of the samples (a large 2k bytes chunk)
    2. Large amounts of heap will be used to support this mode


## <a name="aoa-hardware-setup"></a>Hardware Setup
**Note: Pin 29 will be held high on application initialization in order to enable a TX/RX antenna in cases where BOOSTXL-AOA is attached**

AoA requires 2 devices at minimum: RTLS Master/Slave.
The devices should be flashed with the rtls_master/rtls_slave applications as described above (with AoA flags).
Compile and flash your applications.

* RTLS Master - CC2642R1 LaunchPad with BOOSTXL-AOA

#### Optional for extra accuracy
Add an RTLS Passive to the setup - also equiped with BOOSTXL-AOA

**For the RTLS Master/Passive hardware setup, please take a look at**
[Angle of Arrival BoosterPack](http://dev.ti.com/tirex/#/?link=Development%20Tools%2FKits%20and%20Boards%2FAngle%20of%20Arrival%20BoosterPack)

## <a name="aoa-running-the-example"></a>Running The Example
The steps to run the out of box example is described in `tools\ble5stack\rtls_agent\README.html`

### RAW Mode
Note that this mode puts extra strain on the UART and the timing of the system since RAW mode involves outputing large chunks of data.

**When using RAW mode for both AoA**
* Minimum recommended latency (Connection Interval) is 300ms
* Maximal recommended latency is 800ms
