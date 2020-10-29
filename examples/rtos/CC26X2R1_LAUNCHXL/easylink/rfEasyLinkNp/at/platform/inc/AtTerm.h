/*
 * Copyright (c) 2015, Texas Instruments Incorporated
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

//!
//! \defgroup atPlatform

//!
//! \ingroup atPlatform
//@{

#ifndef atTerm__include
#define atTerm__include

#ifdef __cplusplus
extern "C"
{
#endif

#include "at/platform/tirtos/PlatformTypes.h"

//*****************************************************************************
//
//! \brief Initializes the AT terminal
//!
//! This function configures the UART for use as an AT Terminal
//!
//! \param none
//!
//! \return UART specific status, 0 for success -1 for error
//
//*****************************************************************************
extern int32_t AtTerm_init( void );

//*****************************************************************************
//
//! \brief Gets a char from the AT Terminal
//!
//! This function reads a character from the AT Terminal
//!
//! \param ch - pointer to a Uint8 where the character will be read in to
//!
//! \return bytes read (0 or 1)
//
//*****************************************************************************
extern int32_t AtTerm_getChar( char* ch );

//*****************************************************************************
//
//! \brief Puts a char to the AT Terminal
//!
//! This function writes a character to the AT Terminal
//!
//! \param ch - the char to be written
//!
//! \return bytes read (0 or 1)
//
//*****************************************************************************
extern void AtTerm_putChar( char ch );

//*****************************************************************************
//
//! \brief Sends a string to the AT Terminal
//!
//! This function writes a string to the AT Terminal
//!
//! \param string - the null terminated string to be written
//!
//! \return none
//
//*****************************************************************************
extern void AtTerm_sendString( char* string);

//*****************************************************************************
//
//! \brief Sends a string and a Uint8 to the AT Terminal
//!
//! This function writes a string to the AT Terminal
//!
//! \param string - the null terminated string to be written
//! \param value - the Uint8 value to be written
//! \param format - base 10 or 16 (Dec or Hex)
//!
//! \return none
//
//*****************************************************************************
extern void AtTerm_sendStringUi8Value( char *string, uint8_t value, uint8_t
        format);

//*****************************************************************************
//
//! \brief Sends a string and a Int8 to the AT Terminal
//!
//! This function writes a string to the AT Terminal
//!
//! \param string - the null terminated string to be written
//! \param value - the Int8 value to be written
//! \param format - base 10 or 16 (Dec or Hex)
//!
//! \return none
//
//*****************************************************************************
extern void AtTerm_sendStringI8Value(char *string, int8_t value,
        uint8_t format);

//*****************************************************************************
//
//! \brief Sends a string and a Uint16 to the AT Terminal
//!
//! This function writes a string to the AT Terminal
//!
//! \param string - the null terminated string to be written
//! \param value - the Uint16 value to be written
//! \param format - base 10 or 16 (Dec or Hex)
//!
//! \return none
//
//*****************************************************************************
extern void AtTerm_sendStringUi16Value( char* string, uint16_t value, uint8_t
        format);

//*****************************************************************************
//
//! \brief Sends a string and a Int16 to the AT Terminal
//!
//! This function writes a string to the AT Terminal
//!
//! \param string - the null terminated string to be written
//! \param value - the Int16 value to be written
//! \param format - base 10 or 16 (Dec or Hex)
//!
//! \return none
//
//*****************************************************************************
extern void AtTerm_sendStringI16Value(char *string, int16_t value, uint8_t
        format);

//*****************************************************************************
//
//! \brief Sends a string and a Uint32 to the AT Terminal
//!
//! This function writes a string to the AT Terminal
//!
//! \param string - the null terminated string to be written
//! \param value - the Uint32 value to be written
//! \param format - base 10 or 16 (Dec or Hex)
//!
//! \return none
//
//*****************************************************************************
extern void AtTerm_sendStringUi32Value( char *string, uint32_t value, uint8_t
        format);

//*****************************************************************************
//
//! \brief Sends a string and a Int32 to the AT Terminal
//!
//! This function writes a string to the AT Terminal
//!
//! \param string - the null terminated string to be written
//! \param value - the Int32 value to be written
//! \param format - base 10 or 16 (Dec or Hex)
//!
//! \return none
//
//*****************************************************************************
extern void AtTerm_sendStringI32Value(char *string, int32_t value, uint8_t
        format);

#ifdef __cplusplus
}
#endif

#endif /* atTerm__include */
