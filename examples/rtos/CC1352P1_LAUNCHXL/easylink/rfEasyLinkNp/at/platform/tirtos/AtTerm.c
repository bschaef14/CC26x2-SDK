/*
 * Copyright (c) 2015-2016, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
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
 */

#include <string.h>
#include <stdio.h>

#include <xdc/runtime/System.h>
#include <ti/drivers/UART.h>
#include <ti/sysbios/knl/Clock.h>

#include "ti_drivers_config.h"

#include "at/platform/inc/AtTerm.h"
#include "at/AtProcess.h"

static UART_Handle uartHdl;

int32_t AtTerm_init(void)
{
    UART_Params uartParams;

    /* Create a UART with data processing off. */
    UART_Params_init(&uartParams);
    uartParams.writeDataMode = UART_DATA_BINARY;
    uartParams.readDataMode = UART_DATA_BINARY;
    uartParams.readReturnMode = UART_RETURN_FULL;
    uartParams.readTimeout = UART_WAIT_FOREVER; //(10000 / Clock_tickPeriod); //return after blocking for 10ms
    uartParams.readEcho = UART_ECHO_OFF;
    uartParams.baudRate = 115200;
    uartHdl = UART_open(CONFIG_DISPLAY_UART, &uartParams);

    if (uartHdl == NULL)
    {
        System_abort("Error opening the UART");
    }

    return 0;
}

int32_t AtTerm_getChar(char* ch)
{
    char c = '\r';
    uint32_t bytes = 0;

    //System_printf("a+\n");
    //System_flush();

    bytes = UART_read(uartHdl, &c, 1);

    *ch = c;
    //System_printf("a-(%c:%d)\n", c, bytes);
    return bytes;
}

void AtTerm_putChar(char ch)
{
    UART_write(uartHdl, &ch, 1);
}

void AtTerm_sendStringUi8Value(char *string, uint8_t value, uint8_t format)
{
    char strVal[128] = { 0 };

    if (format == 10)
    {
        sprintf(strVal, "%s%02d", (char*) string, value);
    } else
    {
        sprintf(strVal, "%s%02x", (char*) string, value);
    }

    UART_write(uartHdl, strVal, strlen(strVal));
}

void AtTerm_sendStringI8Value(char *string, int8_t value, uint8_t format)
{
    char strVal[128] = { 0 };

    if (format == 10)
    {
        sprintf(strVal, "%s%02d", (char*) string, value);
    } else
    {
        sprintf(strVal, "%s%02x", (char*) string, value);
    }

    UART_write(uartHdl, strVal, strlen(strVal));
}

void AtTerm_sendStringUi16Value(char *string, uint16_t value, uint8_t format)
{
    char strVal[128] = { 0 };

    if (format == 10)
    {
        sprintf(strVal, "%s%04d", (char*) string, value);
    } else
    {
        sprintf(strVal, "%s%04x", (char*) string, value);
    }

    UART_write(uartHdl, strVal, strlen(strVal));
}

void AtTerm_sendStringI16Value(char *string, int16_t value, uint8_t format)
{
    char strVal[128] = { 0 };

    if (format == 10)
    {
        sprintf(strVal, "%s%04d", (char*) string, value);
    } else
    {
        sprintf(strVal, "%s%04x", (char*) string, value);
    }

    UART_write(uartHdl, strVal, strlen(strVal));
}

void AtTerm_sendStringUi32Value(char *string, uint32_t value, uint8_t format)
{
    char strVal[128] = { 0 };

    if (format == 10)
    {
        sprintf(strVal, "%s%08u", (char*) string, (unsigned int)value);
    } else
    {
        sprintf(strVal, "%s%08x", (char*) string, (unsigned int)value);
    }

    UART_write(uartHdl, strVal, strlen(strVal));
}

void AtTerm_sendStringI32Value(char *string, int32_t value, uint8_t format)
{
    char strVal[128] = { 0 };

    if (format == 10)
    {
        sprintf(strVal, "%s%08d", (char*) string, (int)value);
    } else
    {
        sprintf(strVal, "%s%08x", (char*) string, (unsigned int)value);
    }

    UART_write(uartHdl, strVal, strlen(strVal));
}

void AtTerm_sendString(char *string)
{
    uint32_t len = strlen(string);
    UART_write(uartHdl, string, len);
}
