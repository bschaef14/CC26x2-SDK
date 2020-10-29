## Example Summary

This application moves into and out of shutdown. The state is controlled
by buttons. `CONFIG_PIN_BUTTON_1` will bring the device into shutdown, while
`CONFIG_PIN_BUTTON_0` will wake-up the device. A special LED toggling sequence
is used when waking from shutdown to demonstrate the get-reset-source
functionality.

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

* `CONFIG_PIN_LED_0` -  Lit when the device is active, not lit when in shutdown.
* `CONFIG_PIN_LED_1` -  Will blink 2 times when coming out of shutdown.

## BoosterPacks, Board Resources & Jumper Settings

For board specific jumper settings, resources and BoosterPack modifications,
refer to the __Board.html__ file.

> If you're using an IDE such as Code Composer Studio (CCS) or IAR, please
refer to Board.html in your project directory for resources used and
board-specific jumper settings.

The Board.html can also be found in your SDK installation:

        <SDK_INSTALL_DIR>/source/ti/boards/<BOARD>

## Example Usage

* Run the example. Use the `CONFIG_PIN_BUTTON_1` and `CONFIG_PIN_BUTTON_0`
buttons to shutdown and wake-up the device. Also, use the RESET button
to compare the different start-up sequences used.

> It is not possible to do a proper shutdown sequence with the
debugger connected. For correct behaviour, this example must be run with the
debugger disconnected by resetting or performing a power-cycle of the device.

## Application Design Details

* The example consists of one task which is waiting for a semaphore
that is posted in the `CONFIG_PIN_BUTTON_1` interrupt handle. Since no other
resources are requested by the application, it will enter standby
in active mode when waiting for a posting of the semaphore. The
non-default initialization table used is equal to the default one.
It is included to show how a non-default initialization when waking
from shutdown can be used to avoid glitches on IOs.

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
