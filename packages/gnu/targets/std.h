/* 
 *  Copyright (c) 2014 Texas Instruments and others.
 *  All rights reserved. This program and the accompanying materials
 *  are made available under the terms of the Eclipse Public License v1.0
 *  which accompanies this distribution, and is available at
 *  http://www.eclipse.org/legal/epl-v10.html
 * 
 *  Contributors:
 *      Texas Instruments - initial implementation
 * 
 * */
/*
 *  ======== gnu/targets/std.h ========
 *
 *! Revision History
 *! ================
 *! 02-Jun-2014 ashish  use C99 types (stdint.h) to define xdc types
 *! 26-Oct-2011 sg      added 7EM for M4F
 *! 30-Aug-2011 sg      different UInt32 for 7M architecture
 *! 21-Apr-2006 kw      removed unsupported 'Arg' type
 *! 28-Jul-2005 kw      updated per new RTSC types
 */

#ifndef gnu_targets_STD_
#define gnu_targets_STD_

/* allow _TI_STD_TYPES like 'Uns' and 'Uint8' */
#ifndef xdc__deprecated_types
#define xdc__deprecated_types
#endif

/* include target-specific "portable" macros */
#if defined(xdc_target_name__) & !defined(xdc_target_macros_include__)
#include xdc__local_include(xdc_target_name__)
#endif

#include <stdint.h>	/* C99 standard integer types */

/*
 * xdc__LONGLONG__ indicates if compiler supports 'long long' type
 * xdc__BITS<n> __ indicates if compiler supports 'uint<n>_t' type
 */
#define xdc__LONGLONG__
#define xdc__BITS8__
#define xdc__BITS16__
#define xdc__BITS32__
#define xdc__BITS64__
#define xdc__INT64__

/*
 *  ======== [U]Int<n> ========
 */
typedef int_least8_t        xdc_Int8;
typedef uint_least8_t       xdc_UInt8;
typedef int_least16_t       xdc_Int16;
typedef uint_least16_t      xdc_UInt16;
typedef int_least32_t       xdc_Int32;
typedef uint_least32_t      xdc_UInt32;
#ifdef xdc__INT64__
typedef int_least64_t       xdc_Int64;
typedef uint_least64_t      xdc_UInt64;
#endif

/*
 *  ======== Bits<n> ========
 */
#ifdef xdc__BITS8__
typedef uint8_t             xdc_Bits8;
#endif
#ifdef xdc__BITS16__
typedef uint16_t            xdc_Bits16;
#endif
#ifdef xdc__BITS32__
typedef uint32_t            xdc_Bits32;
#endif
#ifdef xdc__BITS64__
typedef uint64_t            xdc_Bits64;
#endif

/*
 *  ======== [IU]Arg ========
 */
typedef intptr_t	        xdc_IArg;
typedef uintptr_t	        xdc_UArg;

/* deprecated, but compatible with BIOS 5.x */
#define xdc__ARG__
typedef xdc_IArg            xdc_Arg;

/*
 *  ======== restrict ========
 */
#define xdc__RESTRICT__

/*
 *  ======== xdc__META ========
 */
#if !defined(xdc_target__os_MacOS)
  #define xdc__META(n,s) \
    __attribute__ ((section ("xdc.meta"))) \
    const char (n)[] = {(s)}
#endif

#if (xdc_target__sizeof_IArg > xdc_target__sizeof_Ptr) \
    || (xdc_target__sizeof_IArg > xdc_target__sizeof_Fxn)
#define xdc__ARGTOPTR
#define xdc__ARGTOFXN
static inline xdc_Ptr xdc_iargToPtr(xdc_IArg a) { return ((xdc_Ptr)(int)a); }
static inline xdc_Ptr xdc_uargToPtr(xdc_UArg a) { return ((xdc_Ptr)(int)a); }
static inline xdc_Fxn xdc_iargToFxn(xdc_IArg a) { return ((xdc_Fxn)(int)a); }
static inline xdc_Fxn xdc_uargToFxn(xdc_UArg a) { return ((xdc_Fxn)(int)a); }
#endif

#endif /* gnu_targets_STD_ */
/*
 *  @(#) gnu.targets; 1, 0, 1,0; 4-17-2020 14:57:02; /db/ztree/library/trees/xdc/xdc-I11/src/targets/
 */

