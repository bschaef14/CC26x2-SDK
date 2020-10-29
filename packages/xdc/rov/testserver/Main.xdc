/* --COPYRIGHT--,EPL
 *  Copyright (c) 2016-2020 Texas Instruments Incorporated - http://www.ti.com
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * --/COPYRIGHT--*/

/*!
 *  ======== Main.xdc ========
 *  The xdc.rov.testserver displays ROV views to clients on the command line.
 *  The server expects that a CCS debugger is already started and an application
 *  is loaded.
 *
 *  The DSlite port must be known at the time the server is started. Therefore,
 *  when starting CCS, the environment variable TI_DS_WEBSOCKET_PORT must be set
 *  and that same port number must be supplied to xdc.rov.testserver.
 *
 *  Commands:
 *
 *  Start the server -
 *      xs xdc.rov.testserver --exec <full path> --port <port_number>
 *
 *  Get a list of Modules and Tabs
 *      l
 *
 *  Get a Module's View
 *      v Module View
 *
 *  Issue the command "Run" to the debugger
 *      r
 *
 *  Issue the command "Pause" to the debugger
 *      p
 *
 *  Evaluate an XDCScript or a JavaScript expression
 *      > expression
 *
 *  Exit xdc.rov.testserver
 *      e
 *
 */
metaonly module Main inherits xdc.tools.ICmd {

    override config String usage[] = [
        ' ',
        'Usage',
        '[--exec <executable>]',
        '[--port <DSLite Websocket port>]',
        '[-v <debug level>]',
        '[--help]',
        ' ',
    ];

instance:

    /*!
     *  ======== exec ========
     *
     *  Executable to run ROV on
     *
     */
    @CommandOption('exec')
    config String executable = "";

    /*!
     *  ======== port ========
     *
     *  DSLite port
     *
     *  Corresponds to the value in the environment variable
     *  TI_DS_WEBSOCKET_PORT. The variable must be set at the time CCS is
     *  started.
     */
    @CommandOption('port')
    config UInt port;

    /*!
     *  ======== verbose ========
     *
     *  Defines the amount of debug info
     *
     */
    @CommandOption('v')
    config UInt verbose = 0;
}
