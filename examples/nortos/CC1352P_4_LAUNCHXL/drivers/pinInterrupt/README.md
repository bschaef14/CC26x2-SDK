## Example Summary

Application that toggles board LEDs using a Pin interrupt.

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

* `CONFIG_PIN_LED_0` - Toggled by CONFIG_PIN_BUTTON_0
* `CONFIG_PIN_LED_1` - Toggled by CONFIG_PIN_BUTTON_1
* `CONFIG_PIN_BUTTON_0` - Toggles CONFIG_PIN_LED_0
* `CONFIG_PIN_BUTTON_1` - Toggles CONFIG_PIN_LED_1

## BoosterPacks, Board Resources & Jumper Settings

For board specific jumper settings, resources and BoosterPack modifications,
refer to the __Board.html__ file.

> If you're using an IDE such as Code Composer Studio (CCS) or IAR, please
refer to Board.html in your project directory for resources used and
board-specific jumper settings.

The Board.html can also be found in your SDK installation:

        <SDK_INSTALL_DIR>/source/ti/boards/<BOARD>


## Example Usage

Run the example.

1. `CONFIG_PIN_LED_0` is toggled by pushing `CONFIG_PIN_BUTTON_0`.

2. `CONFIG_PIN_LED_1` is toggled by pushing `CONFIG_PIN_BUTTON_1`.

* Each key's interrupt is configured for a falling edge of the pulse. A small
delay has been added to provide button de-bounce logic.

## Application Design Details

* The `buttonCallbackFxn` function is defined in the *pinInterrupt.c* file.
This function is called in the context of the pin interrupt (Hwi).

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
