/////////////////////////////////////////////////////////////////////////////////////////////////////////
// COPYRIGHT (C) 2020 TRIPLE RING TECHNOLOGIES, INC
//                    ALL RIGHTS RESERVED
//
//   File Name:	stdMacros.h
// Module Name: PICMG IIoT
//     Project:	TRT03/IIoT
//
// Description:	useful 'C' mnemonics and types
//				NOTE:	If the compiler you are using supports C99 or later, then the size-specific
//						integer type definitions are guaranteed to be valid.
//
//						OTHERWISE, THEY MUST BE ADAPTED TO THE COMPILER YOU ARE USING!
//
//						Prior to that version, the C standard did not define standard types that
//						were guaranteed to be of a certain size.  In many implementations, 'char'
//						is one byte (8 bits), 'short' is 2 bytes (16 bits), and 'long' is 4 bytes
//						(32 bits), but that is not required by the standard and some compilers do
//						it differently.
//
//						Since not all compilers are required to implement all possible integer
//						sizes, the most we can guarantee is that an integer defined with a specific
//						size is AT LEAST big enough to hold a value of that size. On most modern
//						compilers, all sizes will be available, so the size-specific types will,
//						indeed, be sized exactly as intended.  However, if you are absolutely
//						depending on that, you will need to check the underlying compiler
//						implementation to make sure....
//
// Rev History:	26 April 2020		A.P. Lowell
//					Initial release
//
// This is the unpublished proprietary source code of TRIPLE RING TECHNOLOGIES. The copyright notice
// above does not evidence any actual or intended publication of such source code.
//
// This source code module is provided to speed up and simplify the development process and is
// intended for incorporation into and/or linkage with other design elements. Use of this source code
// is governed by the Triple Ring Technologies Platform License (TRTPL), as follows:
//
//		-	For purposes of the TRTPL, any element(s) covered by the TRTPL are designated "licensed
//			materials"; a person or entity which has been granted a license to make use of licensed
//			materials is designated a "licensee".
//
//		-	For purposes of the TRTPL, product design(s) which incorporate the licensed materials and/or
//			link to element(s) which incorporate the licensed materials shall be deemed to have "made use
//			of" the licensed materials and are designated "license-derived" product design(s).  It is NOT
//			required that license-derived product design(s) and associated product(s) actively report a
//			TRT copyright notice as part of their operation.
//
//		-	License-derived product design(s) may be held as proprietary and/or protected by copyright,
//			patent, or trademark without impediment PROVIDED they do not assert or try to enforce such
//			protections on the licensed materials per se.
//
//		-	Licensee(s) may sell and/or distribute product(s) based on license-derived product design(s)
//			without impediment and/or further obligation EXCEPT as defined in any supplemental agreements
//			between the licensee(s) and TRT stipulating additional licensing terms.
//
//		-	Licensee(s) may make use of the licensed materials for maintenance of and upgrades to the
//			license-derived product design(s) without impediment and/or further obligation.
//
//		-	Licensee(s) may modify the licensed materials to fix defects and/or remove/modify/add
//			features PROVIDED they do not claim or try to enforce their own copyright, patent, and/or
//			trademark on the pre-existing materials AND PROVIDED they retain the original TRT copyright
//			notice(s) within any elements containing the pre-existing materials.
//
//		-	Licensed materials are provided without any warranty, INCLUDING, WITHOUT LIMITATION, any
//			implied warranty of merchantability or fitness for a particular purpose or infringement of
//			third party intellectual property rights, EXCEPT AS OTHERWISE SPECIFICALLY STIPULATED in
//			an agreement executed by Triple Ring Technologies.
//
// Contractual recipients of product design(s) developed by Triple Ring Technologies under a Triple Ring
// Technologies Professional Services Agreement, and which make use of materials licensed under the
// TRTPL, are AUTOMATICALLY granted the right to make use of the licensed materials as part of the
// delivered product design(s) subject to the following supplemental rights and constraints:
//
//		-	Licensee(s) may make use of the licensed materials for new product design(s) that extend
//			their own product line without impediment and/or further obligation.  For purposes of TRTPL
//			licensing, such new license-derived product design(s) will have the same status as the
//			original TRT-developed product design(s) and associated TRTPL licensing shall be considered
//			as having been granted as an extension of the original TRT Professional Services Agreement.
//
//		-	Unless such usage rights are explicitly granted by a supplemental agreement between the
//			licensee(s) and TRT, the licensee(s) may NOT make use of the licensed materials for
//			product design(s) developed on behalf of a 3rd party.
//
//		-	EXCEPT for the following explicit restrictions, licensee(s) may transfer ownership of
//			license-derived product design(s), and associated rights to the licensed materials, to a 3rd
//			party without impediment and/or further obligation.
//
//				-	Unless additional usage rights are explicitly granted by a supplemental agreement
//					between the licensee(s) and TRT, rights to the licensed materials will adhere to
//					the license-derived product design(s) and will transfer to the 3rd party along with
//					ownership of the license-derived product design(s); the original licensee(s) will
//					NOT retain any rights to the licensed materials after the transfer of license-
//					derived product design ownership.
//
//				-	Licensee(s) may NOT place rights to the licensed materials into the public domain
//					by a transfer of license-derived product design ownership into the public domain.
//
//				-	Licensee(s) may NOT grant or transfer rights to the licensed materials to a 3rd party
//					WITHOUT a transfer of ownership of the associated license-derived product design(s).
//
//	SUPPLEMENTAL LICENSING TERMS AND RIGHTS: This module has been provided to the PCI Industrial Computer
//	Manufacturer's Group (PICMG) as part of a reference design for their IIoT standard set.  As such, it
//	has been provided to PICMG specifically for distribution as freeware and may be considered a part of
//	the public domain, with no restrictions on use and/or re-distribution and without licensing fees.
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef		stdMacros_h
#define		stdMacros_h


///////// PRE-INCLUDE PRE-PROCESSOR DIRECTIVES //////////////////////////////////////////////////////////


///////// INCLUDES //////////////////////////////////////////////////////////////////////////////////////

#include "stdTypes.h"


///////// POST-INCLUDE PRE-PROCESSOR DIRECTIVES /////////////////////////////////////////////////////////





///////// MNEMONICS /////////////////////////////////////////////////////////////////////////////////////

	///////// DEFINES ///////////////////////////////////////////////////////////////////////////////////

		// numbered bit masks
#define BIT0_8						BIT_8(0)
#define BIT1_8						BIT_8(1)
#define BIT2_8						BIT_8(2)
#define BIT3_8						BIT_8(3)
#define BIT4_8						BIT_8(4)
#define BIT5_8						BIT_8(5)
#define BIT6_8						BIT_8(6)
#define BIT7_8						BIT_8(7)
#define BIT0_16						BIT_16(0)
#define BIT1_16						BIT_16(1)
#define BIT2_16						BIT_16(2)
#define BIT3_16						BIT_16(3)
#define BIT4_16						BIT_16(4)
#define BIT5_16						BIT_16(5)
#define BIT6_16						BIT_16(6)
#define BIT7_16						BIT_16(7)
#define BIT8_16						BIT_16(8)
#define BIT9_16						BIT_16(9)
#define BIT10_16					BIT_16(10)
#define BIT11_16					BIT_16(11)
#define BIT12_16					BIT_16(12)
#define BIT13_16					BIT_16(13)
#define BIT14_16					BIT_16(14)
#define BIT15_16					BIT_16(15)
#define BIT0_32						BIT_32(0)
#define BIT1_32						BIT_32(1)
#define BIT2_32						BIT_32(2)
#define BIT3_32						BIT_32(3)
#define BIT4_32						BIT_32(4)
#define BIT5_32						BIT_32(5)
#define BIT6_32						BIT_32(6)
#define BIT7_32						BIT_32(7)
#define BIT8_32						BIT_32(8)
#define BIT9_32						BIT_32(9)
#define BIT10_32					BIT_32(10)
#define BIT11_32					BIT_32(11)
#define BIT12_32					BIT_32(12)
#define BIT13_32					BIT_32(13)
#define BIT14_32					BIT_32(14)
#define BIT15_32					BIT_32(15)
#define BIT16_32					BIT_32(16)
#define BIT17_32					BIT_32(17)
#define BIT18_32					BIT_32(18)
#define BIT19_32					BIT_32(19)
#define BIT20_32					BIT_32(20)
#define BIT21_32					BIT_32(21)
#define BIT22_32					BIT_32(22)
#define BIT23_32					BIT_32(23)
#define BIT24_32					BIT_32(24)
#define BIT25_32					BIT_32(25)
#define BIT26_32					BIT_32(26)
#define BIT27_32					BIT_32(27)
#define BIT28_32					BIT_32(28)
#define BIT29_32					BIT_32(29)
#define BIT30_32					BIT_32(30)
#define BIT31_32					BIT_32(31)
#if		defined ( STD_UINT64_SUPPORTED )
#define BIT0_64						BIT_64(0)
#define BIT1_64						BIT_64(1)
#define BIT2_64						BIT_64(2)
#define BIT3_64						BIT_64(3)
#define BIT4_64						BIT_64(4)
#define BIT5_64						BIT_64(5)
#define BIT6_64						BIT_64(6)
#define BIT7_64						BIT_64(7)
#define BIT8_64						BIT_64(8)
#define BIT9_64						BIT_64(9)
#define BIT10_64					BIT_64(10)
#define BIT11_64					BIT_64(11)
#define BIT12_64					BIT_64(12)
#define BIT13_64					BIT_64(13)
#define BIT14_64					BIT_64(14)
#define BIT15_64					BIT_64(15)
#define BIT16_64					BIT_64(16)
#define BIT17_64					BIT_64(17)
#define BIT18_64					BIT_64(18)
#define BIT19_64					BIT_64(19)
#define BIT20_64					BIT_64(20)
#define BIT21_64					BIT_64(21)
#define BIT22_64					BIT_64(22)
#define BIT23_64					BIT_64(23)
#define BIT24_64					BIT_64(24)
#define BIT25_64					BIT_64(25)
#define BIT26_64					BIT_64(26)
#define BIT27_64					BIT_64(27)
#define BIT28_64					BIT_64(28)
#define BIT29_64					BIT_64(29)
#define BIT30_64					BIT_64(30)
#define BIT31_64					BIT_64(31)
#define BIT32_64					BIT_64(32)
#define BIT33_64					BIT_64(33)
#define BIT34_64					BIT_64(34)
#define BIT35_64					BIT_64(35)
#define BIT36_64					BIT_64(36)
#define BIT37_64					BIT_64(37)
#define BIT38_64					BIT_64(38)
#define BIT39_64					BIT_64(39)
#define BIT40_64					BIT_64(40)
#define BIT41_64					BIT_64(41)
#define BIT42_64					BIT_64(42)
#define BIT43_64					BIT_64(43)
#define BIT44_64					BIT_64(44)
#define BIT45_64					BIT_64(45)
#define BIT46_64					BIT_64(46)
#define BIT47_64					BIT_64(47)
#define BIT48_64					BIT_64(48)
#define BIT49_64					BIT_64(49)
#define BIT50_64					BIT_64(50)
#define BIT51_64					BIT_64(51)
#define BIT52_64					BIT_64(52)
#define BIT53_64					BIT_64(53)
#define BIT54_64					BIT_64(54)
#define BIT55_64					BIT_64(55)
#define BIT56_64					BIT_64(56)
#define BIT57_64					BIT_64(57)
#define BIT58_64					BIT_64(58)
#define BIT59_64					BIT_64(59)
#define BIT60_64					BIT_64(60)
#define BIT61_64					BIT_64(61)
#define BIT62_64					BIT_64(62)
#define BIT63_64					BIT_64(63)
#endif	// STD_UINT64_SUPPORTED


	///////// ENUMS /////////////////////////////////////////////////////////////////////////////////////


///////// FORWARD STRUCT REFERENCES /////////////////////////////////////////////////////////////////////


///////// TYPEDEFS //////////////////////////////////////////////////////////////////////////////////////





///////// MACROS ////////////////////////////////////////////////////////////////////////////////////////

	// bit definitions
		// general typed bit mask definition
#define BIT(n,type)					(((type)1)<<(n))

		// 8-bit bit-mask
#define BIT_8(n)					BIT(n,UInt8)
		// 16-bit bit-mask
#define BIT_16(n)					BIT(n,UInt16)
		// 32-bit bit-mask
#define BIT_32(n)					BIT(n,UInt32)
#if		defined ( STD_UINT64_SUPPORTED )
		// 64-bit bit-mask
#define BIT_64(n)					BIT(n,UInt64)
#endif	// STD_UINT64_SUPPORTED


			/*
				NOTE:	The following macros provide parameterized access to individual bits and bit
						fields within an integer variable.  Bits are numbered LSB to MSB, starting with
						0 (LSB).

						NOTE these use shift operators, so they do not depend on byte-ordering.

						For bit fields, masks and shift specifications are used to identify the field.
			*/
#define _bitIsSet(data,bit,type)	( ( (data) & BIT(bit,type) ) != 0 )
#define _bitIsClear(data,bit,type)	( !_bitIsSet ( data, bit, type ) )

#define _bitIsSet_8(data,bit)		_bitIsSet(data,bit,UInt8)
#define _bitIsClear_8(data,bit)		_bitIsClear(data,bit,UInt8)
#define _bitIsSet_16(data,bit)		_bitIsSet(data,bit,UInt16)
#define _bitIsClear_16(data,bit)	_bitIsClear(data,bit,UInt16)
#define _bitIsSet_32(data,bit)		_bitIsSet(data,bit,UInt32)
#define _bitIsClear_32(data,bit)	_bitIsClear(data,bit,UInt32)
#if		defined ( STD_UINT64_SUPPORTED )
#define _bitIsSet_64(data,bit)		_bitIsSet(data,bit,UInt64)
#define _bitIsClear_64(data,bit)	_bitIsClear(data,bit,UInt64)
#endif	//STD_UINT64_SUPPORTED

#define _setBit(data,bit,type)		( ( data ) |= BIT ( bit, type ) )
#define _clearBit(data,bit,type)	( ( data ) &= (~BIT ( bit, type )) )
#define _invertBit(data,bit,type)	( ( data ) ^= BIT ( bit, type ) )

#define _setBit_8(data,bit)			_setBit(data,bit,UInt8)
#define _clearBit_8(data,bit)		_clearBit(data,bit,UInt8)
#define _invertBit_8(data,bit)		_invertBit(data,bit,UInt8)
#define _setBit_16(data,bit)		_setBit(data,bit,UInt16)
#define _clearBit_16(data,bit)		_clearBit(data,bit,UInt16)
#define _invertBit_16(data,bit)		_invertBit(data,bit,UInt16)
#define _setBit_32(data,bit)		_setBit(data,bit,UInt32)
#define _clearBit_32(data,bit)		_clearBit(data,bit,UInt32)
#define _invertBit_32(data,bit)		_invertBit(data,bit,UInt32)
#if		defined ( STD_UINT64_SUPPORTED )
#define _setBit_64(data,bit)		_setBit(data,bit,UInt64)
#define _clearBit_64(data,bit)		_clearBit(data,bit,UInt64)
#define _invertBit_64(data,bit)		_invertBit(data,bit,UInt64)
#endif	// STD_UINT64_SUPPORTED

			/*
				NOTE:	For the following macros, we ASSUME the 'data' and 'mask' values are of the same
						type!  If they are not, odd things may happen....
			*/
#define _anyBitsAreSet(data,mask)	( ( (data) & (mask) ) != 0 )
#define _allBitsAreSet(data,mask)	( ( (data) & (mask) ) == (mask) )
#define _anyBitsAreClear(data,mask)	( !_allBitsAreSet ( data, mask ) )
#define _allBitsAreClear(data,mask)	( !_anyBitsAreSet ( data, mask ) )

#define _setBits(data,mask)		   	( ( data ) |= ( mask ) )
#define _clearBits(data,mask)		( ( data ) &= ~( mask ) )
#define _invertBits(data,mask)  	( ( data ) ^= ( mask ) )

				/*
					NOTE:	For the 'field' macros, the 'mask' is presumed to represent the mask for the
							field RELATIVE TO THE LSB.  That is:

								-	For the 'get' macro, the mask is applied AFTER the field has been
									shifted to the right to that its LSB sits at the WORD LSB
								-	For the 'set' macros, the mask is applied to the value BEFORE it
									is shifted left to its position within the WORD

							Hence, for two different fields of the same size, the masks would be the
							same, independent of the positions at which those fields were placed.
				*/
#define _getField(data,shift,mask)	\
									( ( (data) >> (shift) ) & (mask) )
#define _setField(data,mask,shift,value) \
									( ( (data) & ~( (mask) << (shift) ) ) \
										| ( ( (value) & (mask) ) << (shift) ) )


#define _u16_lo8(d)					((UInt8)(((UInt16)(d))&0xff))
#define _u16_hi8(d)					((UInt8)((((UInt16)(d))>>8)&0xff))

#define _u32_lo16(d)				((UInt16)(((UInt32)(d))&0xffff))
#define _u32_hi16(d)				((UInt16)((((UInt32)(d))>>16)&0xffff))

#if		defined ( STD_UINT64_SUPPORTED )
#define _u64_lo32(d)				((UInt32)(((UInt64)(d))&0xffffffff))
#define _u64_hi32(d)				((UInt32)((((UInt64)(d))>>32)&0xffffffff))
#endif	// STD_UINT64_SUPPORTED

#define _make_u16(l,h)				((((UInt16)(l))&0xff)|((((UInt16)(h))&0xff)<<8))
#define _make_u32(l,h)				((((UInt32)(l))&0xffff)|((((UInt32)(h))&0xffff)<<16))
#if		defined ( STD_UINT64_SUPPORTED )
#define _make_u64(l,h)				((((UInt64)(l))&0xffffffff)|((((UInt64)(h))&0xffffffff)<<32))
#endif	// STD_UINT64_SUPPORTED


///////// FUNCTION PROTOTYPES ///////////////////////////////////////////////////////////////////////////





///////// STRUCT DECLARATIONS ///////////////////////////////////////////////////////////////////////////




///////// CONSTANTS /////////////////////////////////////////////////////////////////////////////////////


///////// VARIABLES /////////////////////////////////////////////////////////////////////////////////////





///////// POST-CODE COMPILER DIRECTIVES /////////////////////////////////////////////////////////////////

#endif		/* stdMacros_h */
