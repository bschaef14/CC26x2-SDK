/* --COPYRIGHT--,EPL
 *  Copyright (c) 2008-2019 Texas Instruments Incorporated - http://www.ti.com
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

/*
 *  ======== microsoft/targets/std.h ========
 */

#ifndef microsoft_targets_STD_
#define microsoft_targets_STD_

/* include target-specific "portable" macros */
#if defined(xdc_target_name__) & !defined(xdc_target_macros_include__)
#include xdc__local_include(xdc_target_name__)
#endif

#define inline __inline

/*
 * xdc__LONGLONG__ indicates if compiler supports 'long long' type
 * xdc__BITS<n> __ indicates if compiler supports 'uint<n>_t' type
 */
#undef xdc__LONGLONG__
#define xdc__BITS8__
#define xdc__BITS16__
#define xdc__BITS32__
#define xdc__BITS64__
#define xdc__INT64__

/*
 *  ======== [U]Int<n> ========
 */
typedef signed char	xdc_Int8;
typedef unsigned char	xdc_UInt8;
typedef short		xdc_Int16;
typedef unsigned short	xdc_UInt16;
typedef long		xdc_Int32;
typedef unsigned long	xdc_UInt32;

typedef __int64          xdc_Int64;
typedef unsigned __int64 xdc_UInt64;
typedef __int64          xdc_LLong;
typedef unsigned __int64 xdc_ULLong;

/*
 *  ======== Bits<n> ========
 */
typedef unsigned char	xdc_Bits8;
typedef unsigned short	xdc_Bits16;
typedef unsigned int	xdc_Bits32;

typedef unsigned __int64 xdc_Bits64;

/*
 *  ======== Arg ========
 */
typedef intptr_t  xdc_IArg;
typedef uintptr_t xdc_UArg;

#define xdc__ARG__
typedef xdc_IArg xdc_Arg;	/* deprecated, but compatible with BIOS 5.x */

#endif /* microsoft_targets_STD_ */
