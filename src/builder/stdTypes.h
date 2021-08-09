/////////////////////////////////////////////////////////////////////////////////////////////////////////
// COPYRIGHT (C) 2020 TRIPLE RING TECHNOLOGIES, INC
//                    ALL RIGHTS RESERVED
//
//   File Name:	stdTypes.h
// Module Name: PICMG IIoT
//     Project:	TRT03/IIoT
//
// Description:	useful 'C' mnemonics and types
//
//				This header is designed to be useable across platforms to enable sharing of library
//				sources across platforms.  The intent is to provide the same set of mnemonics and type
//				definitions across all platforms, both so that the libraries will compile/link on any
//				platform and in the broader interest of standardizing code development.  In general,
//				this will need be adapted to the processor/development platform you are using, because:
//
//					-	Different processors have different native integer and pointer sizes, and use
//						different byte-ordering to encode multi-byte values
//
//					-	Different development platforms map the 'C/C++' integer types to different word
//						sizes differently
//
//				To that extent, this file is a template for creating a customized header as much as it
//				a header for direct use.  You will see various 'adaptation' switches in this template
//				by which it tries to detect the 'proper' configuration.  Those are attuned to the
//				particular processors/development platforms I happened to be using when I first created
//				it and are in no way either complete or wholly reliable -- it is likely that, at some
//				point, you will need to do your own customization.
//
// Rev History:	16 October 2020		A.P. Lowell
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
#ifndef		stdTypes_h
#define		stdTypes_h


///////// PRE-INCLUDE PRE-PROCESSOR DIRECTIVES //////////////////////////////////////////////////////////

		/*
			Customize to the platform by bringing in the platform-specific adaptations.  THE PLATFORM-
			SPECIFIC ADAPATATION FILE INCLUDED HERE MUST BE IN THE BUILD PATH FOR THE SPECIFIC
			DEVELOPMENT TOOL/PROJECT YOU ARE USING TO DO THE BUILD!

			It should #define the following mnemonics that identify how the platform works:

				STD_BIG_ENDIAN					#define ONE of these to specify byte ordering for
				STD_LITTLE_ENDIAN				multi-byte values.  Applications can use this to
												customize code that must access individual bytes/
												words within a larger word.

				SIZE_ALIGNED						#define as 16, 32, or 64 to indicate the size (in
												bits) of the 'native' alignment for optimal bus
												access.  Applications can use this INDIRECTLY to
												allocate space in 'aligned' chunks by making use
												of the 'SAlignedInteger' and 'UAlignedInteger'
												types, and/or by using the 'STD_ALIGNED_SIZE'
												macro.

				SIZE_POINTER						#define as 16, 32, or 64 to indicate the size of a
												pointer (in bits) or, more accurately, to indicate
												the standard integer size that must be used to hold
												a pointer.  Applications can use this IDIRECTLY to
												manage conversion between pointers and integers by
												using the 'intptr_t' and 'uintptr_t' types.

				TYPES_SUPPORTS_SIZED_INTEGERS	#define this to indicate that the development tool
												supports the C/C++ fixed-width integer types; see
												the note, below.  If the tool does NOT support those,
												then we provide definitions for them in this header
												so that they are guaranteed to be available for use.
												To do that, we need to #define the following platform
												mappings:

				TYPE_SINT8						#define to resolve to the type that the development
												tool uses to create a signed 8-bit integer.

				TYPE_UINT8						#define to resolve to the type that the development
												tool uses to create an unsigned 8-bit integer.

				TYPE_SINT16						#define to resolve to the type that the development
												tool uses to create a signed 16-bit integer.

				TYPE_UINT16						#define to resolve to the type that the development
												tool uses to create an unsigned 16-bit integer.

				TYPE_SINT32						#define to resolve to the type that the development
												tool uses to create a signed 32-bit integer.

				TYPE_UINT32						#define to resolve to the type that the development
												tool uses to create an unsigned 32-bit integer.

				TYPE_SINT64						#define to resolve to the type that the development
												tool uses to create a signed 64-bit integer; #undef
												to indicate that the tool doesn't support use of 64-
												bit integers.

				TYPE_UINT64						#define to resolve to the type that the development
												tool uses to create an unsigned 64-bit integer;
												#undef to indicate that the tool doesn't support use
												of 64-bit integers.

				LIM_SINT8_MIN					#define to resolve to the minimum value representable
												using a signed 8-bit integer.

				LIM_SINT8_MAX					#define to resolve to the maximum value representable
												using a signed 8-bit integer.

				LIM_UINT8_MAX					#define to resolve to the maximum value representable
												using an unsigned 8-bit integer.

				LIM_SINT16_MIN					#define to resolve to the minimum value representable
												using a signed 16-bit integer.

				LIM_SINT16_MAX					#define to resolve to the maximum value representable
												using a signed 16-bit integer.

				LIM_UINT16_MAX					#define to resolve to the maximum value representable
												using an unsigned 16-bit integer.

				LIM_SINT32_MIN					#define to resolve to the minimum value representable
												using a signed 32-bit integer.

				LIM_SINT32_MAX					#define to resolve to the maximum value representable
												using a signed 32-bit integer.

				LIM_UINT32_MAX					#define to resolve to the maximum value representable
												using an unsigned 32-bit integer.

				LIM_SINT64_MIN					#define to resolve to the minimum value representable
												using a signed 64-bit integer; #undef to omit that
													definition.
													
				LIM_SINT64_MAX					#define to resolve to the maximum value representable
												using a signed 64-bit integer; #undef to omit that
												definition.

				LIM_UINT64_MAX					#define to resolve to the maximum value representable
												using an unsigned 64-bit integer; #undef to omit that
												definition.
		*/
#include "platform_Adapter.h"





///////// INCLUDES //////////////////////////////////////////////////////////////////////////////////////


// standard 'C' libraries
#ifdef	TYPES_SUPPORTS_SIZED_INTEGERS
#include <stdint.h>			// include the fixed-width integer types
#endif
#include <limits.h>			// include integer limits (ANSI)
#include <float.h>			// include floating-point limits (ANSI)


///////// POST-INCLUDE PRE-PROCESSOR DIRECTIVES /////////////////////////////////////////////////////////





///////// MNEMONICS /////////////////////////////////////////////////////////////////////////////////////

	///////// DEFINES ///////////////////////////////////////////////////////////////////////////////////

		// mnemonics for data types

			// integer types
#define TYPES_SUPPORTS_SIZED_INTEGERS
#if		!defined ( TYPES_SUPPORTS_SIZED_INTEGERS )
			/*
				The standard length-specific types are not available, so we replicate them.
			*/
#define INT_LEAST8_MIN			LIM_SINT8_MIN
#define INT_LEAST8_MAX			LIM_SINT8_MAX
#define UINT_LEAST8_MAX			LIM_UINT8_MAX
#define INT_LEAST16_MIN			LIM_SINT16_MIN
#define INT_LEAST16_MAX			LIM_SINT16_MAX
#define UINT_LEAST16_MAX		LIM_UINT16_MAX
#define INT_LEAST32_MIN			LIM_SINT32_MIN
#define INT_LEAST32_MAX			LIM_SINT32_MAX
#define UINT_LEAST32_MAX		LIM_UINT32_MAX
#if			defined ( LIM_SINT64_MIN )
#define INT_LEAST64_MIN			LIM_SINT64_MIN
#endif		// LIM_SINT64_MIN
#if			defined ( LIM_SINT64_MAX )
#define INT_LEAST64_MAX			LIM_SINT64_MAX
#endif		// LIM_SINT64_MAX
#if			defined ( LIM_UINT64_MAX )
#define UINT_LEAST64_MAX		LIM_UINT64_MAX
#endif		// LIM_UINT64_MAX


#define INT8_MIN				INT_LEAST8_MIN
#define INT8_MAX				INT_LEAST8_MAX
#define UINT8_MAX				UINT_LEAST8_MAX
#define INT16_MIN				INT_LEAST16_MIN
#define INT16_MAX				INT_LEAST16_MAX
#define UINT16_MAX				UINT_LEAST16_MAX
#define INT32_MIN				INT_LEAST32_MIN
#define INT32_MAX				INT_LEAST32_MAX
#define UINT32_MAX				UINT_LEAST32_MAX
#if			defined ( LIM_SINT64_MIN )
#define INT64_MIN				INT_LEAST32_MIN
#endif		// LIM_SINT64_MIN
#if			defined ( LIM_SINT64_MAX )
#define INT64_MAX				INT_LEAST32_MAX
#endif		// LIM_SINT64_MAX
#if			defined ( LIM_UINT64_MAX )
#define UINT64_MAX				UINT_LEAST32_MAX
#endif		// LIM_UINT64_MAX


#if			( 8 == SIZE_ALIGNED )
#define INT_FAST8_MIN			INT_LEAST8_MIN
#define INT_FAST8_MAX			INT_LEAST8_MAX
#define UINT_FAST8_MAX			UINT_LEAST8_MAX
#define INT_FAST16_MIN			INT_LEAST16_MIN
#define INT_FAST16_MAX			INT_LEAST16_MAX
#define UINT_FAST16_MAX			UINT_LEAST16_MAX
#define INT_FAST32_MIN			INT_LEAST32_MIN
#define INT_FAST32_MAX			INT_LEAST32_MAX
#define UINT_FAST32_MAX			UINT_LEAST32_MAX
#elif		( 16 == SIZE_ALIGNED )
#define INT_FAST8_MIN			INT_LEAST16_MIN
#define INT_FAST8_MAX			INT_LEAST16_MAX
#define UINT_FAST8_MAX			UINT_LEAST16_MAX
#define INT_FAST16_MIN			INT_LEAST16_MIN
#define INT_FAST16_MAX			INT_LEAST16_MAX
#define UINT_FAST16_MAX			UINT_LEAST16_MAX
#define INT_FAST32_MIN			INT_LEAST32_MIN
#define INT_FAST32_MAX			INT_LEAST32_MAX
#define UINT_FAST32_MAX			UINT_LEAST32_MAX
#elif		( 32 == SIZE_ALIGNED )
#define INT_FAST8_MIN			INT_LEAST32_MIN
#define INT_FAST8_MAX			INT_LEAST32_MAX
#define UINT_FAST8_MAX			UINT_LEAST32_MAX
#define INT_FAST16_MIN			INT_LEAST32_MIN
#define INT_FAST16_MAX			INT_LEAST32_MAX
#define UINT_FAST16_MAX			UINT_LEAST32_MAX
#define INT_FAST32_MIN			INT_LEAST32_MIN
#define INT_FAST32_MAX			INT_LEAST32_MAX
#define UINT_FAST32_MAX			UINT_LEAST32_MAX
#else		// ( 64 == SIZE_ALIGNED )
#define INT_FAST8_MIN			INT_LEAST64_MIN
#define INT_FAST8_MAX			INT_LEAST64_MAX
#define UINT_FAST8_MAX			UINT_LEAST64_MAX
#define INT_FAST16_MIN			INT_LEAST64_MIN
#define INT_FAST16_MAX			INT_LEAST64_MAX
#define UINT_FAST16_MAX			UINT_LEAST64_MAX
#define INT_FAST32_MIN			INT_LEAST64_MIN
#define INT_FAST32_MAX			INT_LEAST64_MAX
#define UINT_FAST32_MAX			UINT_LEAST64_MAX
#endif		// SIZE_ALIGNED
#if			defined ( LIM_SINT64_MIN )
#define INT_FAST64_MIN			INT_LEAST64_MIN
#endif		// LIM_SINT64_MIN
#if			defined ( LIM_SINT64_MAX )
#define INT_FAST64_MAX			INT_LEAST64_MAX
#endif		// LIM_SINT64_MAX
#if			defined ( LIM_UINT64_MAX )
#define UINT_FAST64_MAX			UINT_LEAST64_MAX
#endif		// LIM_UINT64_MAX


#if			defined ( LIM_SINT64_MIN )
#define INTMAX_MIN				INT_LEAST64_MIN
#else		// !defined ( LIM_SINT64_MIN )
#define INTMAX_MIN				INT_LEAST32_MIN
#endif		// LIM_SINT64_MIN
#if			defined ( LIM_SINT64_MAX )
#define INTMAX_MAX				INT_LEAST64_MAX
#else		// !defined ( LIM_SINT64_MAX )
#define INTMAX_MAX				INT_LEAST32_MAX
#endif		//LIM_SINT64_MAX
#if			defined ( LIM_UINT64_MAX )
#define UINTMAX_MAX				UINT_LEAST64_MAX
#else		// !defined ( LIM_UINT64_MAX )
#define UINTMAX_MAX				UINT_LEAST32_MAX
#endif		//LIM_UINT64_MAX


#if			( 16 == SIZE_POINTER )
#define INTPTR_MIN				INT_LEAST16_MIN
#define INTPTR_MAX				INT_LEAST16_MAX
#define UINTPTR_MAX				UINT_LEAST16_MAX
#elif		( 32 == SIZE_POINTER )
#define INTPTR_MIN				INT_LEAST32_MIN
#define INTPTR_MAX				INT_LEAST32_MAX
#define UINTPTR_MAX				UINT_LEAST32_MAX
#else		// ( 64 == SIZE_POINTER )
#define INTPTR_MIN				INT_LEAST64_MIN
#define INTPTR_MAX				INT_LEAST64_MAX
#define UINTPTR_MAX				UINT_LEAST64_MAX
#endif		// SIZE_POINTER

#endif	// TYPES_SUPPORTS_SIZED_INTEGERS





			// convenient aliases that are easier to type and won't conflict with any other definitions
				/*
					NOTE:	Since the C/C++ standard doesn't guarantee that types like 'int32_t' will be
							defined (they only guarantee that, for example, 'int_least32_t' will be
							defined), you can't rely on being able to use 'int32_t' as a type in your
							code.  Since typing 'int_least32_t' is long and awkward, we provide these
							short-hand alternatives, like UInt32...
				*/
#define SNativeInt_MIN			INT_MIN				// native signed integer (platform-determined size)
#define SNativeInt_MAX			INT_MAX				// native signed integer (platform-determined size)
#define UNativeInt_MAX			UINT_MAX			// native unsigned integer ( platform-determined size)

#define SInt8_MIN				INT_LEAST8_MIN		// signed 8-bit integer
#define SInt8_MAX				INT_LEAST8_MAX		// signed 8-bit integer
#define UInt8_MAX				UINT_LEAST8_MAX		// unsigned 8-bit integer
#define SInt16_MIN				INT_LEAST16_MIN		// signed 16-bit integer
#define SInt16_MAX				INT_LEAST16_MAX		// signed 16-bit integer
#define UInt16_MAX				UINT_LEAST16_MAX	// unsigned 16-bit integer
#define SInt32_MIN				INT_LEAST32_MIN		// signed 32-bit integer
#define SInt32_MAX				INT_LEAST32_MAX		// signed 32-bit integer
#define UInt32_MAX				UINT_LEAST32_MAX	// unsigned 32-bit integer
#if			defined ( LIM_SINT64_MIN )
#define SInt64_MIN				INT_LEAST64_MIN		// signed 64-bit integer
#endif		// LIM_SINT64_MIN
#if			defined ( LIM_SINT64_MAX )
#define SInt64_MAX				INT_LEAST64_MAX		// signed 64-bit integer
#endif		// LIM_SINT64_MAX
#if			defined ( LIM_UINT64_MAX )
#define UInt64_MAX				UINT_LEAST64_MAX	// unsigned 64-bit integer
#endif		// LIM_UINT64_MAX





		// floating-point types
#define FloatSP_DIG 			FLT_DIG				// single-precision floating-point
#define FloatSP_MIN	 			FLT_MIN
#define FloatSP_MAX				FLT_MAX
#define FloatSP_EPSILON			FLT_EPSILON

#define FloatDP_DIG				DBL_DIG				// double-precision floating-point
#define FloatDP_MIN				DBL_MIN
#define FloatDP_MAX				DBL_MAX
#define FloatDP_EPSILON			DBL_EPSILON





			// Pointer values
#if		!defined ( NULL )
#define NULL					(0)
#endif


			// logical state mnemonics
				// boolean logic values
#define B_FALSE					((stdBool)0)
#define B_TRUE					((stdBool)1)
#define B_LOW					B_FALSE
#define B_HIGH					B_TRUE
#define B_CLEAR					B_FALSE
#define B_RESET					B_FALSE
#define B_SET					B_TRUE
#define B_DEASSERTED			B_FALSE
#define B_ASSERTED				B_TRUE
#define B_INACTIVE				B_FALSE
#define B_ACTIVE				B_TRUE
#define B_UNSIGNALED			B_FALSE
#define B_SIGNALED				B_TRUE

#if		!defined ( FALSE )
#define FALSE					B_FALSE
#endif	// FALSE
#if		!defined ( TRUE )
#define TRUE					B_TRUE
#endif	// TRUE

				// switch states (stdBool, stdState)
#define SW_OFF 					B_FALSE
#define SW_ON					B_TRUE
#define SW_OPEN					SW_OFF		
#define SW_CLOSED				SW_ON

				// decision states
				// (stdDecision, stdState)
#define DEC_NO					(0)
#define DEC_MAYBE				(-1)
#define DEC_YES					(1)
#define DEC_STOP				DEC_NO
#define DEC_GO					DEC_YES
#define DEC_NOGO				DEC_NO

#define STS_BAD					DEC_NO
#define STS_GOOD				DEC_YES
#define STS_INDIFFERENT			DEC_MAYBE
#define STS_FAIL				STS_BAD
#define STS_PASS				STS_GOOD
#define STS_NOTOK				STS_BAD
#define STS_OK					STS_GOOD
#define STS_INVALID				STS_BAD
#define STS_VALID				STS_GOOD

				// number classification (stdState)
#define NUM_MINUS_ONE			(-1)
#define NUM_ZERO				(0)
#define NUM_PLUS_ONE			(1)
#define NUM_NEG_ONE				NUM_MINUS_ONE
#define NUM_POS_ONE				NUM_PLUS_ONE
#define NUM_NEGATIVE			NUM_MINUS_ONE
#define NUM_NEUTRAL				NUM_ZERO
#define NUM_POSITIVE			NUM_PLUS_ONE
#define NUM_MINUS				NUM_NEGATIVE
#define NUM_PLUS				NUM_POSITIVE
#define NUM_DECREMENT			NUM_MINUS_ONE
#define NUM_HOLD				NUM_ZERO
#define NUM_INCREMENT			NUM_PLUS_ONE

				// result states (stdResult)
	// include 'stdResult.h' for detailed result state definitions


	///////// ENUMS /////////////////////////////////////////////////////////////////////////////////////


///////// FORWARD STRUCT REFERENCES /////////////////////////////////////////////////////////////////////


///////// TYPEDEFS //////////////////////////////////////////////////////////////////////////////////////

		// generic data types
		//
#if		!defined ( TYPES_SUPPORTS_SIZED_INTEGERS )
			/*
				The standard length-specific types are not available, so we replicate them.
			*/

typedef TYPE_SINT8				int_least8_t;
typedef TYPE_UINT8				uint_least8_t;
typedef TYPE_SINT16				int_least16_t;
typedef TYPE_UINT16				uint_least16_t;
typedef TYPE_SINT32				int_least32_t;
typedef TYPE_UINT32				uint_least32_t;
#if			defined ( TYPE_SINT64 )
typedef TYPE_SINT64				int_least64_t;
#endif		// TYPE_SINT64
#if			defined ( TYPE_UINT64 )
typedef TYPE_UINT64				uint_least64_t;
#endif		// TYPE_UINT64


typedef int_least8_t			int8_t;
typedef uint_least8_t			uint8_t;
typedef int_least16_t			int16_t;
typedef uint_least16_t			uint16_t;
typedef int_least32_t			int32_t;
typedef uint_least32_t			uint32_t;
#if			defined ( TYPE_SINT64 )
typedef int_least64_t			int64_t;
#endif		// TYPE_SINT64
#if			defined ( TYPE_UINT64 )
typedef uint_least64_t			uint64_t;
#endif		// TYPE_UINT64


#if			( 8 == SIZE_ALIGNED )
typedef int_least8_t			int_fast8_t;
typedef uint_least8_t			uint_fast8_t;
typedef int_least16_t			int_fast16_t;
typedef uint_least16_t			uint_fast16_t;
typedef int_least32_t			int_fast32_t;
typedef uint_least32_t			uint_fast32_t;
#elif		( 16 == SIZE_ALIGNED )
typedef int_least16_t			int_fast8_t;
typedef uint_least16_t			uint_fast8_t;
typedef int_least16_t			int_fast16_t;
typedef uint_least16_t			uint_fast16_t;
typedef int_least32_t			int_fast32_t;
typedef uint_least32_t			uint_fast32_t;
#elif		( 32 == SIZE_ALIGNED )
typedef int_least32_t			int_fast8_t;
typedef uint_least32_t			uint_fast8_t;
typedef int_least32_t			int_fast16_t;
typedef uint_least32_t			uint_fast16_t;
typedef int_least32_t			int_fast32_t;
typedef uint_least32_t			uint_fast32_t;
#else		// ( 64 == SIZE_ALIGNED )
typedef int_least64_t			int_fast8_t;
typedef uint_least64_t			uint_fast8_t;
typedef int_least64_t			int_fast16_t;
typedef uint_least64_t			uint_fast16_t;
typedef int_least64_t			int_fast32_t;
typedef uint_least64_t			uint_fast32_t;
#endif		// SIZE_ALIGNED
#if			defined ( TYPE_SINT64 )
typedef int_least64_t			int_fast64_t;
#endif		// TYPE_SINT64
#if			defined ( TYPE_UINT64 )
typedef uint_least64_t			uint_fast64_t;
#endif		// TYPE_UINT64


#if			defined ( TYPE_SINT64 )
typedef int_least64_t			int_max_t;
#else		// !defined ( TYPE_SINT64 )
typedef int_least32_t			int_max_t;
#endif		// TYPE_SINT64
#if			defined ( TYPE_UINT64 )
typedef uint_least64_t			uint_max_t;
#else		// !defined ( TYPE_UINT64 )
typedef uint_least32_t			uint_max_t;
#endif		// TYPE_UINT64


#if			( 16 == SIZE_POINTER )
typedef	int_least16_t			intptr_t;
typedef uint_least16_t			uintptr_t;
#elif		( 32 == SIZE_POINTER )
typedef	int_least32_t			intptr_t;
typedef uint_least32_t			uintptr_t;
#else		// ( 64 == SIZE_POINTER )
typedef	int_least64_t			intptr_t;
typedef uint_least64_t			uintptr_t;
#endif		// SIZE_POINTER

#endif	// TYPES_SUPPORTS_SIZED_INTEGERS





			/*
				NOTE:	The following flags may be used by application modules to decide whether or
						not they may use 64-bit integers for things.
			*/
#if		defined ( INT_LEAST64_MAX )
#define STD_INT64_SUPPORTED					// signal that signed 64-bit integers are available
#endif	// INT_LEAST64_MAX
#if		defined ( UINT_LEAST64_MAX )
#define STD_UINT64_SUPPORTED				// signal that unsigned 64-bit integers are available
#endif	// UINT_LEAST64_MAX

#if		defined ( STD_INT64_SUPPORTED ) || defined ( STD_UINT64_SUPPORTED )
#define STD_xINT64_SUPPORTED				// signal that SOME kind of 64-bit integers are available
#endif	// STD_INT64_SUPPORTED, STD_UINT64_SUPPORTED





		// 'aligned' integer types -- integers of the minimum size guaranteed to be aligned for
		// optimal bus access
		//
#if		( 8 == SIZE_ALIGNED )
typedef int_least8_t			SAlignedInteger;
typedef uint_least8_t			UAlignedInteger;
#elif	( 16 == SIZE_ALIGNED )
typedef int_least16_t			SAlignedInteger;
typedef uint_least16_t			UAlignedInteger;
#elif	( 32 == SIZE_ALIGNED )
typedef int_least32_t			SAlignedInteger;
typedef uint_least32_t			UAlignedInteger;
#else	// ( 64 == SIZE_ALIGNED )
typedef int_least64_t			SAlignedInteger;
typedef uint_least64_t			UAlignedInteger;
#endif	// SIZE_ALIGNED





			// character types
typedef uint_least8_t			ascChar;				// ASCII character
typedef uint_least16_t			uniChar;				// Unicode character





			// integer types
				// convenient aliases that are easier to type and won't conflict with any other
				// definitions
					/*
						NOTE:	Since the C/C++ standard doesn't guarantee that types like 'int32_t'
								will be defined (they only guarantee that, for example, 'int_least32_t'
								will be defined), you can't rely on being able to use 'int32_t' as a
								type in your code.  Since typing 'int_least32_t' is long and awkward, we
								provide these short-hand alternatives, like UInt32...
					*/
typedef int						SNativeInt;			// native signed integer (platform-determined size)
typedef unsigned int			UNativeInt;			// native unsigned integer (platform-determined size)


typedef int_least8_t			SInt8;				// signed 8-bit integer
typedef uint_least8_t			UInt8;				// unsigned 8-bit integer
typedef int_least16_t			SInt16;				// signed 16-bit integer
typedef uint_least16_t			UInt16;				// unsigned 16-bit integer
typedef int_least32_t			SInt32;				// signed 32-bit integer
typedef uint_least32_t			UInt32;				// unsigned 32-bit integer
#if			defined ( STD_INT64_SUPPORTED )
typedef int_least64_t			SInt64;				// signed 64-bit integer
#endif		// TYPE_SINT64
#if			defined ( STD_UINT64_SUPPORTED )
typedef uint_least64_t			UInt64;				// unsigned 64-bit integer
#endif		// TYPE_UINT64





			// floating-point types
typedef float					FloatSP;			// single-precision float
typedef double					FloatDP;			// double-precision float





			// null and pointer types
typedef void*	 				stdPointer;			// generic pointer
typedef stdPointer*				stdHandle;			// generic handle





			// logical state types
typedef uint_least8_t			stdBool;			// Boolean value

typedef int_least16_t			stdState;			// useful states; see mnemonics, above

typedef int_least16_t  			stdDecision;		// result of a decision; see mnemonics, above

typedef int_least16_t 			stdResult;  		// result of an action; see 'stdResult.h'





///////// MACROS ////////////////////////////////////////////////////////////////////////////////////////

		/*
			NOTE:	The following increases the specified size (in bytes) as necessary to make the
					adjusted size encompass exactly an integer number of 'aligned' integers.  You can use
					it to ensure that things you allocate end up properly aligned for optimum bus access.
		*/
#define STD_ALIGNED_SIZE(bytes)	(sizeof(UAlignedInteger)										\
									*((bytes)+(sizeof(UAlignedInteger)-1)/sizeof(UAlignedInteger)))


///////// FUNCTION PROTOTYPES ///////////////////////////////////////////////////////////////////////////





///////// STRUCT DECLARATIONS ///////////////////////////////////////////////////////////////////////////




///////// CONSTANTS /////////////////////////////////////////////////////////////////////////////////////


///////// VARIABLES /////////////////////////////////////////////////////////////////////////////////////





///////// POST-CODE COMPILER DIRECTIVES /////////////////////////////////////////////////////////////////

	// clean up our local definitions so the can't conflict with anything else...
#undef	LIM_UINT64_MAX
#undef	LIM_SINT64_MAX
#undef	LIM_SINT64_MIN
#undef	LIM_UINT32_MAX
#undef	LIM_SINT32_MAX
#undef	LIM_SINT32_MIN
#undef	LIM_UINT16_MAX
#undef	LIM_SINT16_MAX
#undef	LIM_SINT16_MIN
#undef	LIM_UINT8_MAX
#undef	LIM_SINT8_MAX
#undef	LIM_SINT8_MIN
#undef	TYPE_UINT64
#undef	TYPE_SINT64
#undef	TYPE_UINT32
#undef	TYPE_SINT32
#undef	TYPE_UINT16
#undef	TYPE_SINT16
#undef	TYPE_UINT8
#undef	TYPE_SINT8
#undef	SIZE_POINTER
#undef	SIZE_ALIGNED
#undef	TYPES_SUPPORTS_SIZED_INTEGERS

#endif		/* stdTypes_h */
