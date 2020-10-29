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

/***************************************************************************************************
 * INCLUDES
 ***************************************************************************************************/
#include "at/platform/inc/AtTerm.h"
#include "at/AtProcess.h"
#include "at/AtControl.h"
#include "at/AtParams.h"
#include "at/platform/tirtos/DbgPrint.h"

#include <string.h> /* memset */

/***************************************************************************************************
 * CONSTANTS
 ***************************************************************************************************/
#define ATPROCESS_STATE_A         0
#define ATPROCESS_STATE_T         1
#define ATPROCESS_STATE_CMD       2

static void processCmd( char* cmdBuffer, uint8_t cmdLen )
{
    AtProcess_Status status = AtProcess_Status_CmdIdError;

    DbgPrint_printf("processCmd: %s\n", cmdBuffer);

    switch(cmdBuffer[0])
    {
        case '+':
            //process RF command
            status = AtControl_parseIncoming(&(cmdBuffer[1]), cmdLen-1);
            break;
        case '&':
            //process peripheral command
            break;
        case 'P':
        case 'p':
            //process Parameter command
            status = AtParams_parseIncoming(&(cmdBuffer[1]), cmdLen-1);
            break;
            //start timer to return timeout error
    }

    if(status == AtProcess_Status_Success)
    {
        AtProcess_printSuccess();
    }
    else
    {
        AtProcess_printErrorCode(status);
    }

    return;
}

/***************************************************************************************************
 * INTERFACE FUNCTIONS
 ***************************************************************************************************/

void AtProcess_processingLoop( void )
{
    char ch;
    static char cmdBuffer[ATPROCESS_MAX_AT_MSG_LEN];
    static uint8_t state = ATPROCESS_STATE_A;
    static uint8_t cmdLen = 0;

    if( AtTerm_getChar(&ch) > 0)
    {
        if(AtParams_echoEnabled)
        {
            AtTerm_putChar(ch);
        }

        switch( state )
        {
            case ATPROCESS_STATE_A:
                memset(cmdBuffer, 0, ATPROCESS_MAX_AT_MSG_LEN);
                cmdLen = 0;
                if( (ch == 'a') || (ch == 'A') )
                {
                    state = ATPROCESS_STATE_T;
                }
                break;
            case ATPROCESS_STATE_T:
                if( (ch == 't') || (ch == 'T') )
                {
                    state = ATPROCESS_STATE_CMD;
                }
                break;
            case ATPROCESS_STATE_CMD:
                if((ch == '\n') || (ch == '\r'))
                {
                    //command entry complete, process it
                    processCmd(cmdBuffer, cmdLen);
                    state = ATPROCESS_STATE_A;
                    cmdLen = 0;
                }
                else
                {
                    if(cmdLen < ATPROCESS_MAX_AT_MSG_LEN)
                    {
                        //add this char to the command string
                        cmdBuffer[cmdLen++] = ch;
                    }
                    else
                    {
                        //The command entered was too long we have overrun the command
                        //buffer
                        AtProcess_printErrorCode(AtProcess_Status_CmdIdError);
                        state = ATPROCESS_STATE_A;
                        cmdLen = 0;
                    }
                }
                break;
            default:
                state = ATPROCESS_STATE_A;
                cmdLen = 0;
                break;
        }
    }
}

void AtProcess_printSuccess(void)
{
    AtTerm_sendString("OK\r");
}

void AtProcess_printErrorCode(AtProcess_Status errorCode)
{
    AtTerm_sendStringUi16Value("Error ", (uint32_t) errorCode, 16);
    AtTerm_sendString("\r");
}

uint8_t AtProcess_cmdCmp( char* cmdStr, char* inComingCmdStr )
{
    uint32_t cmdIdx;

    for(cmdIdx = 0; cmdIdx < ATPROCESS_MAX_AT_CMD_LEN; cmdIdx++)
    {
        //end of command will be signified by:
        //space (for params), = (for Register), \0, \n, \r
        if( (inComingCmdStr[cmdIdx] == ' ')  ||
                (inComingCmdStr[cmdIdx] == '>')  ||
                (inComingCmdStr[cmdIdx] == '\0') ||
                (inComingCmdStr[cmdIdx] == '\n') ||
                (inComingCmdStr[cmdIdx] == '\r') )
        {
            break;
        }

        //end reg of read or write
        if( (cmdStr[cmdIdx] == inComingCmdStr[cmdIdx]) &&
                ( (cmdStr[cmdIdx] == '=') ||
                        (cmdStr[cmdIdx] == '?') ) )
        {
            break;
        }

        if( cmdStr[cmdIdx] != inComingCmdStr[cmdIdx] )
        {
            cmdIdx = 0;
            break;
        }
    }

    if(cmdIdx == ATPROCESS_MAX_AT_CMD_LEN)
    {
        cmdIdx = 0;
    }

    //remove white spaces
    while(cmdStr[cmdIdx] == ' ')
    {
        cmdIdx++;
    }

    return cmdIdx;
}

/***************************************************************************************************
 * @fn      atCmpReg
 *
 * @brief   compare register access string
 *
 * @param   cmd1 command string 1
 *          cmd2 command string 2
 *
 * @return  None
 ***************************************************************************************************/
uint8_t atCmpReg( char* cmdStr, char* inComingCmdStr )
{
    uint32_t cmdIdx;

    for(cmdIdx = 0; cmdIdx < ATPROCESS_MAX_AT_CMD_LEN; cmdIdx++)
    {
        //end of command will be signified by:
        //space (for params), = (for Register), \0, \n, \r
        if( (inComingCmdStr[cmdIdx] == ' ')  ||
                (inComingCmdStr[cmdIdx] == '>')  ||
                (inComingCmdStr[cmdIdx] == '\0') ||
                (inComingCmdStr[cmdIdx] == '\n') ||
                (inComingCmdStr[cmdIdx] == '\r') )
        {
            break;
        }

        if( cmdStr[cmdIdx] != inComingCmdStr[cmdIdx] )
        {
            cmdIdx = 0;
            break;
        }
    }

    if(cmdIdx == ATPROCESS_MAX_AT_CMD_LEN)
    {
        cmdIdx = 0;
    }

    return cmdIdx;
}

/***************************************************************************************************
 ***************************************************************************************************/
