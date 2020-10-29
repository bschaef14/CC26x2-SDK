/* 
 *  Copyright (c) 2008 Texas Instruments and others.
 *  All rights reserved. This program and the accompanying materials
 *  are made available under the terms of the Eclipse Public License v1.0
 *  which accompanies this distribution, and is available at
 *  http://www.eclipse.org/legal/epl-v10.html
 * 
 *  Contributors:
 *      Texas Instruments - initial implementation
 * 
 * */
/*! @_nodoc
 *  ======== ScalarStructs ========
 *  Structures used to read scalar types from an arbitrary address
 *
 *  The first field of a structure is guarenteed to be at
 *  a zero offset from the address of the structure itself.
 *
 *  From C99 Section 6.7.2.1 bullet point 13:
 *  @p(blist)
 *  - Within a structure object, the non-bit-field members and the units in
 *  which bit-fields reside have addresses that increase in the order in
 *  which they are declared. A pointer to a structure object, suitably
 *  converted, points to its initial member (or if that member is a bit-field,
 *  then to the unit in which it resides), and vice versa. There may be
 *  unnamed padding within a structure object, but not at its beginning.
 *  @p
 *  
 *  As a result, it's possible to use the structures below together with
 *  `Program.fetchStruct()` to fetch scalar values; e.g., the following can
 *  be used to read a 32-bit value from address `addr`:
 *
 *  @p(code)
 *      var v; // value located at address specified by addr 
 *      v = Program.fetchStruct(ScalarStructs.S_Bits32$fetchDesc, addr, false)
 *  @p
 */
module ScalarStructs {
	
    /*! @_nodoc */
    struct S_Bits8 {
	Bits8 elem;
    };

    /*! @_nodoc */
    struct S_Bits16 {
	Bits16 elem;
    };

    /*! @_nodoc */
    struct S_Bits32 {
	Bits32 elem;
    };

    /*! @_nodoc */
    struct S_Char {
	Char elem;
    };

    /*! @_nodoc */
    struct S_UChar {
	UChar elem;
    };

    /*! @_nodoc */
    struct S_Double {
	Double elem;
    };

    /*! @_nodoc */
    struct S_Float {
	Float elem;
    };

    /*! @_nodoc */
    struct S_Fxn {
	Fxn elem;
    };

    /*! @_nodoc */
    struct S_IArg {
	IArg elem;
    };

    /*! @_nodoc */
    struct S_UArg {
	UArg elem;
    };

    /*! @_nodoc */
    struct S_Int {
	Int elem;
    };

    /*! @_nodoc */
    struct S_UInt {
	UInt elem;
    };

    /*! @_nodoc */
    struct S_Int8 {
	Int8 elem;
    };

    /*! @_nodoc */
    struct S_UInt8 {
	UInt8 elem;
    };

    /*! @_nodoc */
    struct S_Int16 {
	Int16 elem;
    };

    /*! @_nodoc */
    struct S_UInt16 {
	UInt16 elem;
    };

    /*! @_nodoc */
    struct S_Int32 {
	Int32 elem;
    };

    /*! @_nodoc */
    struct S_UInt32 {
	UInt32 elem;
    };

    /*! @_nodoc */
    struct S_Long {
	Long elem;
    };

    /*! @_nodoc */
    struct S_ULong {
	ULong elem;
    };

    /*! @_nodoc */
    struct S_LLong {
	LLong elem;
    };

    /*! @_nodoc */
    struct S_ULLong {
	ULLong elem;
    };

    /*! @_nodoc */
    struct S_LDouble {
	LDouble elem;
    };

    /*! @_nodoc */
    struct S_Ptr {
	Ptr elem;
    };

    /*! @_nodoc */
    struct S_SizeT {
	SizeT elem;
    };

    /*! @_nodoc */
    struct S_Short {
	Short elem;
    };

    /*! @_nodoc */
    struct S_UShort {
	UShort elem;
    };

    /*! @_nodoc */
    struct S_String {
	String elem;
    };

    /*! @_nodoc */
    struct S_VaList {
	VaList elem;
    };

}
/*
 *  @(#) xdc.rov.support; 1, 0, 0,0; 4-17-2020 14:55:31; /db/ztree/library/trees/xdc/xdc-I11/src/packages/
 */

