## Example Summary

This example will cycle the device between active and standby states. The CPU
will toggle the LEDs each time it wakes up from standby, using the PIN driver,
and then immediately return to standby.

## SysConfig Usage

Configuring the PIN driver through the SysConfig GUI is not supported. For new
development, please use the GPIO driver.

## Peripherals & Pin Assignments

When this project is built, the SysConfig tool will generate the TI-Driver
configurations into the __ti_drivers_config.c__ and __ti_drivers_config.h__
files. Information on pins and resources used is present in both generated
files. Additionally, the System Configuration file (\*.syscfg) present in the
project may be opened with SysConfig's graphical user interface to determine
pins and resources used.

* `CONFIG_PIN_LED_0` - Toggled by Device Under Test
* `CONFIG_PIN_LED_1` - Toggled by Device Under Test

## BoosterPacks, Board Resources & Jumper Settings

For board specific jumper settings, resources and BoosterPack modifications,
refer to the __Board.html__ file.

> If you're using an IDE such as Code Composer Studio (CCS) or IAR, please
refer to Board.html in your project directory for resources used and
board-specific jumper settings.

The Board.html can also be found in your SDK installation:

        <SDK_INSTALL_DIR>/source/ti/boards/<BOARD>


## Example Usage

* Run the example.

* The device wakes up and toggles the LEDs at periodic intervals.
Modify the `standbyDuration` to change how long the device stays in standby,
the default is 5s.

>If the `standbyDuration` is set to < 1ms, the power policy will
choose to enter idle instead of standby, since it won't have sufficient time
to enter/exit standby.

## Application Design Details

* The example consists of one task which periodically sleeps and wakes.
Since no other work is scheduled in the application, the power policy
(run when the CPU is idle) will choose to put the device into standby during
the duration of the task sleep.

>If you are measuring power consumption during standby, it will be higher when
the LEDs are on, and will be much lower when the LEDs are off.

TI-RTOS:

* When building in Code Composer Studio, the kernel configuration project will
be imported along with the example. The kernel configuration project is
referenced by the example, so it will be built first. The "release" kernel
configuration is the default project used. It has many debug features disabled.
These feature include assert checking, logging and runtime stack checks. For a
detailed difference between the "release" and "debug" kernel configurations and
how to switch between them, please refer to the SimpleLink MCU SDK User's
Guide. The "release" and "debug" kernel configuration projects can be found
under &lt;SDK_INSTALL_DIR&gt;/kernel/tirtos/builds/&lt;BOARD&gt;/(release|debug)/(ccs|gcc).

FreeRTOS:

* Please view the `FreeRTOSConfig.h` header file for example configuration
information.
