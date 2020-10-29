## Introduction

This subfolder contains the host-side logger tooling, used to capture and
interpret log messages and send them to Wireshark to be displayed. If you would
just like to view arbitrary packets in an ITM stream, see the section at the end
of this document.

Please note there is no support for Linux in this beta, but it will be supported
in the GA release.

## Quick Start Guide

Install Python (3.7.x) from __[python.org][python-web]__  and Wireshark from
__[wireshark.org][wireshark-web]__.

There are three steps to displaying logs: installing the python modules using
the setup script, configuring Wireshark to display the logs correctly, and
running the tools.

### Install Python Modules

**NOTE:** If you are  behind a VPN and have not configured http_proxy (i.e. you
usually must provide a --proxy argument to `pip`), use the
`setup_with_proxy.bat` script. By default this is set for the TI internal
network, but you can customise it for your own VPN by passing an argument:
`setup_with_proxy.bat www.myproxy.com`.

`python` must be available on PATH for these scripts to work.

If pip can access pypi with no proxy argument, you can just use `setup.bat`.

Run the `setup.bat` or `setup_with_proxy.bat` script. This will:
 - Generate or update a Python Virtual Environment
 - Install the `tilogger` and `tilogger-itm-transport` packages
 - Install any user modules from `plugins/`

### Configure Wireshark

Copy `transport/tilog_dissector.lua` to `[Wireshark install
dir]/plugins/2.2.6/tilog_dissector.lua`. This will teach Wireshark to understand
the TI Logger's packet format.

Other configuration is handled by the `sharkbait.bat` script.

### Extracting some Logs

The tools must be started in sequence:
 - Start a terminal session in the `tools/log` folder
 - `> .venv/Scripts/activate` to get into the virtual environment
 - `> tilogger itm [out_file] 115200 [COM_PORT]` to launch the logger tool, scan
   your .out file for log items and wait for Wireshark to pick up the pipe. Use
   Device Manager to choose the correct COM port and be aware that you must use
   the "Auxiliary Data Port" not the "Application/User UART".
 - Double click `sharkbait.bat` to launch Wireshark with the correct
   configuration and immediately open the pipe. If `sharkbait` fails to run,
   please check that the path to Wireshark is correct.
 - Flash your logged firmware or reset the device. `tilogger` will not begin
   recording until it has seen a reset frame, which is currently only generated
   on startup.

If all goes well, your logs should begin to appear in the Wireshark window.

`sharkbait.bat` adds some columns to the default view for Level, Module and
Format string. To modify these or add new columns, right click the column
headers and choose `Column Preferences`. Note that the field type should be
'Custom'.

### User format modules
User format modules are functional in this beta, but there are no examples
provided. You will need to create a python package with a class that inherits
from one of the ABCs in tilogger.interface. Your package must register an entry
point (this is documented in the interface classes). If placed in a subfolder of
modules/, it will be installed into the virtual environment automatically when
you call one of the setup scripts.

## Viewing RAW ITM Streams
The `tilogger_itm_transport` package also provides a raw ITM viewer, which can
for example support the use of TI's `ITM` driver directly. Follow the setup
guidelines above to create an environment and install the ITM package. You can
ignore any instructions for Wireshark.

The command line for the ITM viewer is `> tilogger_itm_viewer 115200
[COM_PORT]`.

[python-web]: https://www.python.org/downloads/ [wireshark-web]:
https://www.wireshark.org/download.html
