/////////////////////////////////////////////////////////////////////////////////////////////////////////
// COPYRIGHT (C) 2020 TRIPLE RING TECHNOLOGIES, INC
//                    ALL RIGHTS RESERVED
//
//   File Name:	CSpline.hpp
// Module Name: PICMG IIoT uSAM
//     Project:	TRT03/IIoT uSAM
//
// Description:	Performs cubic sline interpolation from a data set.  This is a customization of the
//				general 'Interpolator' class.
//
// Rev History:	16 October 2020		A.P. Lowell
//				initial release
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
#ifndef		CSpline_hpp
#define		CSpline_hpp


///////// PRE-INCLUDE PRE-PROCESSOR DIRECTIVES //////////////////////////////////////////////////////////


///////// INCLUDES //////////////////////////////////////////////////////////////////////////////////////

		// standard 'C' libraries

		// standard mnemonics and types
#include "stdTypes.h"		// standard data types and mnemonics
#include "stdMacros.h"		// useful macros

		// BSP/HAL headers

		// OS headers

		// library headers

		// project headers
#include "Interpolator.hpp"


///////// POST-INCLUDE PRE-PROCESSOR DIRECTIVES /////////////////////////////////////////////////////////





///////// MNEMONICS /////////////////////////////////////////////////////////////////////////////////////

	///////// DEFINES ///////////////////////////////////////////////////////////////////////////////////


	///////// ENUMS /////////////////////////////////////////////////////////////////////////////////////


///////// FORWARD STRUCT REFERENCES /////////////////////////////////////////////////////////////////////


///////// FORWARD CLASS REFERENCES //////////////////////////////////////////////////////////////////////

class CUCSpline;


///////// TYPEDEFS //////////////////////////////////////////////////////////////////////////////////////





///////// MACROS ////////////////////////////////////////////////////////////////////////////////////////


///////// FUNCTION PROTOTYPES ///////////////////////////////////////////////////////////////////////////





///////// STRUCT DECLARATIONS ///////////////////////////////////////////////////////////////////////////


///////// CLASS DECLARATIONS ////////////////////////////////////////////////////////////////////////////

class CUCSpline : public CUInterpolator {
	// public API
	public:
		// methods
			// construction/destruction/initialization
				/*
					See notes on base-class 'configure' methods, and on the 'natural spline'
					configuration method (below), for argument formats
				*/
				// default constructor (no configuration)
		CUCSpline ( stdBool bNaturalSpline = TRUE );
				// configure from a table
		CUCSpline ( UNativeInt uTableEntries,
				SUConfigurationTableEntry const *paConfigTable,
				stdBool bNaturalSpline = TRUE );
				// configure from separate arrays of independent and dependent values
		CUCSpline ( UNativeInt uTableEntries,
				FloatDP const *pafIndependentValues, FloatDP const *pafDependentValues,
				stdBool bNaturalSpline = TRUE );
				// configure from an array of dependent values and a range of independent values
		CUCSpline ( UNativeInt uTableEntries,
				FloatDP fIndependentValueMin, FloatDP fIndependentValueMax,
				FloatDP const *pafDependentValues,
				stdBool bNaturalSpline = TRUE );
				// configure from a mapping function and a range of independent values
		CUCSpline ( UNativeInt uTableEntries,
				FloatDP fIndependentValueMin, FloatDP fIndependentValueMax,
				FNMAP *pfnMap, stdPointer pMapContext,
				stdBool bNaturalSpline = TRUE );
				// copy from another cubic spline object
		CUCSpline ( CUCSpline const &Ref );
#if		defined ( __cplusplus ) && ( __cplusplus >= 201101L )			// C++11 or later
				// move from another cubic spline object
		CUCSpline ( CUCSpline &&Ref );
#endif	// C++11

		virtual ~CUCSpline();

		inline void copy ( CUCSpline const &Ref ) {
			CUInterpolator::copy ( (CUInterpolator const&)Ref );
		}

				// configure the interpolator
				//
				/*
					:<SUBTLE>:
					If you specify a 'natural' spline, then the second derivative is set to zero at the
					ends of the configuration table and extrapolation beyond the ends of the table is
					done linearly.  That has the advantage that it remains accurately reversible over a
					wide range of extrapolated values (i.e. if you create a second cubic spline by
					reversing the dependent and independent variables during intialization, and then use
					the pair of splines to map forward and then backward -- independent value ->
					dependent value -> independent value -- the input and output of that process will
					match to a high degree of accuracy even when you get reasonably far from the values
					at the ends of the table).  It ALSO has the advantage that it remains highly
					accurate inside the table within the last few entries on either end.

					If you do NOT specify a 'natural' spline, the algorithm attempts to estimate a valid
					second derivative for the ends of the table and uses that for extrapolation.  The
					advantage is that you might eke out a bit more accuracy for extrapolations slightly
					beyond the ends of the table, but the forward and backward splines will begin fairly
					quickly to diverge once you get very far from the ends of the table.  IT ALSO MEANS
					THAT THE ACCURACY WILL BEGIN TO SLOWLY DEGRADE WITHIN THE BOUNDS OF THE LAST FEW
					TABLE ENTRIES -- that is, you trade a bit better accuracy shortly beyond the bounds
					of the table for a bit worse accuracy shortly within the bounds of the table.

					Pick your poison...
				*/
		void configureNaturalSpline ( stdBool bUseNaturalSpline );

				/*
					NOTE:	We use the standard interpolator configuration API, so we don't overload
							the 'configure()' methods -- use the base-class methods...
				*/


			// operators
		inline CUCSpline const& operator= ( CUCSpline const &Ref ) {
			copy ( Ref ); return (*this);
		}
/*
#if		defined ( __cplusplus ) && ( __cplusplus >= 201101L )			// C++11 or later
		inline CUCSpline const& operator= ( CUCSpline &&Ref ) {
			move ( Ref ); return (*this);
		}
#endif	// C++11
*/




	// private API
	private:
		// methods
			// construction/destruction/initialization
				// overload to allocate the d2ydx2 table
				//
		virtual void allocateDerivedBuffers ( UNativeInt uTableEntries );
/*
#if		defined ( __cplusplus ) && ( __cplusplus >= 201101L )			// C++11 or later
		inline void move ( CUCSpline &&Ref ) {
			CUInterpolator::move ( (CUInterpolator&&)Ref );
		}
#endif	// C++11
*/
				// overloads for copy/move methods -- these accont for the CSpline-specific data!
				//
		virtual void doCopy ( CUInterpolator const &Ref );
/*
#if		defined ( __cplusplus ) && ( __cplusplus >= 201101L )			// C++11 or later
		virtual void doMove ( CUInterpolator &&Ref );
#endif	// C++11
*/
				// overload for table size validation method
				//
					/*
						NOTE:	We need at least 3 table entries to do a cubic spline!
					*/
		virtual stdBool tableSizeIsValid ( UNativeInt uTableEntries ) { return ( uTableEntries >= 3 ); }

				// overload for configuration method -- does CSpline-specific configuration
				//
		virtual void deriveConfiguration ( void );

				// estimate the second-derivatives from the configuration table entries
				//
		void deriveSecondDerivatives ( void );


			// operations
				// overloads for extrapolation/interpolation methods -- implements the CSpline algorithm
				//
		virtual FloatDP doExtrapolateLow ( FloatDP fxval );
		virtual FloatDP doExtrapolateHigh ( FloatDP fxval );
		virtual FloatDP doInterpolate ( FloatDP fxval, UNativeInt uEntry );





	// encapsulated data
	private:
		// configuration flags
		stdBool						m_bNaturalSpline;
		// derived configuration tables
		UNativeInt					m_uAllocatedEntries;
		FloatDP*					m_pafd2ydx2;
};





///////// CONSTANTS /////////////////////////////////////////////////////////////////////////////////////


///////// VARIABLES /////////////////////////////////////////////////////////////////////////////////////





///////// POST-CODE COMPILER DIRECTIVES /////////////////////////////////////////////////////////////////

#endif		/* CSpline_hpp */
