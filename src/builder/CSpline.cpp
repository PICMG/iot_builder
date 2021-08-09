/////////////////////////////////////////////////////////////////////////////////////////////////////////
// COPYRIGHT (C) 2020 TRIPLE RING TECHNOLOGIES, INC
//                    ALL RIGHTS RESERVED
//
//   File Name:	CSpline.cpp
// Module Name: PICMG IIoT uSAM
//     Project:	TRT03/IIoT uSAM
//
// Description:	Performs cubic sline interpolation from a data set.  This is a customization of the
//				general 'Interpolator' class.
//
//				This implementation is derived from the prototype implementation defined in 'Numerical
//				Recipes in C', second edition (Press, Teukolsky, Vetterling, and Flannery).
//
// Rev History:	16 October 2020		A.P. Lowell
//				Initial Release
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
#define		CSpline_cpp


///////// PRE-INCLUDE PRE-PROCESSOR DIRECTIVES //////////////////////////////////////////////////////////


///////// INCLUDES //////////////////////////////////////////////////////////////////////////////////////

		// standard 'C' libraries
//#include <stdlib.h>
#include <cstdlib>
#include <stdexcept>
#include <stdio.h>
#include <string.h>
#include <float.h>
#include <math.h>

		// standard mnemonics and types
#include "stdTypes.h"		// standard data types and mnemonics
#include "stdMacros.h"		// useful macros

		// BSP/HAL headers

		// OS headers

		// library headers

		// project headers
#include "CSpline.hpp"
#include "Interpolator.hpp"


///////// POST-INCLUDE PRE-PROCESSOR DIRECTIVES /////////////////////////////////////////////////////////





///////// MNEMONICS /////////////////////////////////////////////////////////////////////////////////////

	///////// DEFINES ///////////////////////////////////////////////////////////////////////////////////


	///////// ENUMS /////////////////////////////////////////////////////////////////////////////////////


///////// FORWARD STRUCT REFERENCES /////////////////////////////////////////////////////////////////////


///////// TYPEDEFS //////////////////////////////////////////////////////////////////////////////////////





///////// MACROS ////////////////////////////////////////////////////////////////////////////////////////


///////// FUNCTION PROTOTYPES ///////////////////////////////////////////////////////////////////////////





///////// STRUCT DECLARATIONS ///////////////////////////////////////////////////////////////////////////





///////// PRIVATE AND LOCAL STATIC CONSTANTS ////////////////////////////////////////////////////////////


///////// LOCAL CONSTANTS ///////////////////////////////////////////////////////////////////////////////


///////// PROTECTED AND PUBLIC CONSTANTS ////////////////////////////////////////////////////////////////


///////// EXTERNAL CONSTANTS ////////////////////////////////////////////////////////////////////////////


///////// PRIVATE AND LOCAL INITIALIZED VARIABLES ///////////////////////////////////////////////////////


///////// LOCAL VARIABLES ///////////////////////////////////////////////////////////////////////////////


///////// PROTECTED AND PUBLIC INITIALIZED VARIABLES ////////////////////////////////////////////////////


///////// EXTERNAL VARIABLES ////////////////////////////////////////////////////////////////////////////





///////// PRIVATE AND LOCAL METHODS /////////////////////////////////////////////////////////////////////

	/////////////////////////////////////////////////////////////////////////////////////////////////////
	// class CUCSpline
	//

void CUCSpline::allocateDerivedBuffers ( UNativeInt uTableEntries ) {
	if ( ( 0 == uTableEntries ) || ( uTableEntries > m_uAllocatedEntries ) ) {
		// is there an old buffer to release?
		if ( 0 != m_uAllocatedEntries ) {
			// yes -- release the old buffer
			delete [] m_pafd2ydx2;
			m_uAllocatedEntries = 0;
			m_pafd2ydx2 = NULL;
		}

		// do we need to allocate a new buffer?
		if ( 0 == uTableEntries ) {
			// no -- we are done
		}
		// allocate a new buffer
			/*
				:<SUBTLE>:
				The following will trap a failure to allocate whether the 'new' method throws an
				exception or simply returns a null pointer.  We assume we can't count on one
				behavior or the other...
			*/
		try { m_pafd2ydx2 = new FloatDP [ uTableEntries ]; }
		catch ( ... ) { m_pafd2ydx2 = NULL; }
		if ( NULL == m_pafd2ydx2 ) { throw std::bad_alloc(); }

		m_uAllocatedEntries = uTableEntries;
	}
	// if we got here, we have a valid set of buffers of non-zero length!
}





void CUCSpline::doCopy ( CUInterpolator const &Ref ) {
	CUCSpline const			&CRef = (CUCSpline const&)Ref;

	// set the natural spline configuration first -- the rest of the configuration depends on it!
	m_bNaturalSpline = CRef.m_bNaturalSpline;

	// copy by configuring from the reference configuration table
	configure ( CRef.tableEntries(), CRef.tablePtr() );
}


/*
#if		defined ( __cplusplus ) && ( __cplusplus >= 201101L )			// C++11 or later

void CUCSpline::doMove ( CUInterpolator &&Ref ) {
	CUCSpline const			&CRef = (CUCSpline const&)Ref;

	// move the base-class elements
	CUInterpolator::doMove ( Ref );

	// move the added elements
	m_uAllocatedEntries = CRef.m_uAllocatedEntries;
	m_pafd2ydx2 = CRef.m_pafd2ydx2;

	CRef.m_uAllocatedEntries = 0;
	CRef.m_pafd2ydx2 = NULL;

	// copy the rest of the added elements
	m_bNaturalSpline = CRef.m_bNaturalSpline;
}

#endif	// C++11
*/

void CUCSpline::deriveConfiguration ( void ) {
		/*
			:<SUBTLE>:
			The algorithm that estimates the second derivatives for the cubic spline requires us to
			stipulate a priori what we think the first derivatives are at the ends of the interval
			represented by the configuration table.  However, since we don't have an actual function
			for the curve from which to calculate those, we don't know what they are and must estimate
			them.
			
			For purposes of estimating the cubic spline parameters, the simple 1st-difference at the
			table end-points seems to be the best estimate we have of the derivative at the end-points.
			Fancier methods that make adjustments (e.g. based on the 2nd difference) don't work as well
			-- by which we mean that they tend to introduce a significant degree of 'extra' curvature
			(assessed visually and by comparison to 'real' curves generated from functional forms) into
			the extrapolations within one or two table-entry intervals beyond the end of the table.

				Example of a correction:
					Technically, the 1st difference is nominally an estimate of the derivative halfway
					between the 2 entries at the end of the table, not at the actual end of the table.
					We might presume we would get a better result projecting that to the actual end of
					the table by accounting for the rate of change in the derivative during that half-
					entry interval, based on our estimate of the second derivative.

					However, our best estimate of the second derivative is, by it nature, from even
					farther back inside the table and, for many curves, applying that as a correction
					ends up over- or under-estimating the derivative at the end point by by more than
					the error in the original "simple" 1st-difference estimate...
		*/
		/*
			NOTE:	The base-class already estimated the first derivative using the simple first
					difference.  We don't need to do anything else!
		*/

	// extract the formal estimates of the second derivatives...
	deriveSecondDerivatives();
}


void CUCSpline::deriveSecondDerivatives ( void ) {
	FloatDP					fSig, fTemp;
	FloatDP					fqn, fun;
	UNativeInt				ui;
	FloatDP					*pafBuffer;

	// allocate a working buffer
	try { pafBuffer = new FloatDP [ tableEntries() ]; }
	catch ( ... ) { pafBuffer = NULL; }
	if ( NULL == pafBuffer ) { throw std::bad_alloc(); }

	// extract the formal estimates of the second derivatives...

		/*
			NOTE:	This formulation comes from the prototype implementation defined in 'Numerical
					Recipes in C', second edition (Press, Teukolsky, Vetterling, and Flannery).
		*/

		// initialize the low boundary
	if ( m_bNaturalSpline ) {
		m_pafd2ydx2[0] = 0.0;
		pafBuffer[0] = 0.0;
	}
	else {
		m_pafd2ydx2[0] = -0.5;
		pafBuffer[0] = ( 3.0 / ( xval(1) - xval(0) ) )
			* ( ( yval(1) - yval(0) ) / ( xval(1) - xval(0) ) - dydx_Low() );
	}

		// do a tridiagonal decomposition; m_pafd2ydx2 and pafBuffer are used for temporary storage
	for ( ui=1 ; ui < highestTableIndex() ; ++ui ) {
		fSig = ( xval(ui) - xval(ui-1) ) / ( xval(ui+1) - xval(ui-1) );
		fTemp = fSig * m_pafd2ydx2[ui-1] + 2.0;
		m_pafd2ydx2[ui] = ( fSig - 1.0 ) / fTemp;
		pafBuffer[ui] = ( yval(ui+1) - yval(ui) ) / ( xval(ui+1) - xval(ui) )
			- ( yval(ui) - yval(ui-1) ) / ( xval(ui) - xval(ui-1) );
		pafBuffer[ui] =
			( 6.0 * pafBuffer[ui] / ( xval(ui+1) - xval(ui-1) ) - fSig * pafBuffer[ui-1] ) / fTemp;
	}

		// initialize the upper boundary values
	if ( m_bNaturalSpline ) {
		fqn = fun = 0.0;
	}
	else {
		fqn = 0.5;
		fun = ( 3.0 / ( xval(highestTableIndex()) - xval(highestTableIndex()-1) ) )
			* ( dydx_High() - ( yval(highestTableIndex()) - yval(highestTableIndex()-1) )
				/ ( xval(highestTableIndex()) - xval(highestTableIndex()-1) ) );
	}

	m_pafd2ydx2[highestTableIndex()] = ( fun - fqn * pafBuffer[highestTableIndex()-1] )
		/ ( fqn * m_pafd2ydx2[highestTableIndex()-1] + 1.0 );

		// do the tridiagonal back-substutution
	for ( ui=(highestTableIndex()) ; ui > 0 ; ) {
		--ui;
		m_pafd2ydx2[ui] = m_pafd2ydx2[ui] * m_pafd2ydx2[ui+1] + pafBuffer[ui];
	}

	// release the working buffer
	delete [] pafBuffer;
}



FloatDP CUCSpline::doExtrapolateLow ( FloatDP fxval ) {
	UNativeInt				uL;
	FloatDP					fInterval, fValue;

	uL = lowestTableIndex();
	fInterval = xval(uL) - fxval;
	fValue =
			yval(uL)
		-	fInterval * dydx_Low()
		-	0.5 * fInterval * fInterval * m_pafd2ydx2[uL];

	return fValue;
}


FloatDP CUCSpline::doExtrapolateHigh ( FloatDP fxval ) {
	UNativeInt				uR;
	FloatDP					fInterval, fValue;

	uR = highestTableIndex();
	fInterval = fxval - xval(uR);
	fValue =
			yval(uR)
		+	fInterval * dydx_High()
		+	0.5 * fInterval * fInterval * m_pafd2ydx2[uR];

	return fValue;
}


FloatDP CUCSpline::doInterpolate ( FloatDP fxval, UNativeInt uEntry ) {
	UNativeInt				uL, uR;
	FloatDP					fSpan;
	FloatDP					fwL, fwR;
	FloatDP					fy;

		/*
			NOTE:	This formulation was adapted from the prototype implementation defined in
					'Numerical Recipes in C', second edition (Press, Teukolsky, Vetterling, and
					Flannery).
		*/

	// do the interpolation
	uL = uEntry;
	uR = uL + 1;

	fSpan = xval(uR) - xval(uL);
	fwL = ( xval(uR) - fxval ) / fSpan;
	fwR = 1.0 - fwL;

	fy = fwL * yval(uL) + fwR * yval(uR)
#if		1
		+ ( ( ( fwL * fwL * fwL - fwL ) * m_pafd2ydx2[uL] + ( fwR * fwR * fwR - fwR ) * m_pafd2ydx2[uR] )
			* ( fSpan * fSpan ) ) / 6.0;
#else
		;
#endif	// 0

	return fy;
}



///////// LOCAL FUNCTIONS ///////////////////////////////////////////////////////////////////////////////


///////// PROTECTED/PUBLIC METHODS //////////////////////////////////////////////////////////////////////

	/////////////////////////////////////////////////////////////////////////////////////////////////////
	// class CUCSpline
	//

		/*
			:<SUBTLE>:
			Part of the configuration is to allocate the derived-class buffers, which is done via a
			virtual method ('allocateDerivedBuffers()'), and to extract the associated configurations,
			which is also done via a virtual method ('deriveConfiguration()').
					
			NEITHER OF THOSE WILL WORK CORRECTLY IF THE CONFIGURATION IS DONE WITHIN THE BASE-CLASS
			CONSTRUCTOR, BECAUSE THE VIRTUAL FUNCTION TABLE THAT POINTS TO THE DERIVED-CLASS VIRTUAL
			OVERLOADS WILL NOT YET HAVE BEEN ESTABLISHED!

			To overcome that limitation, WE INSTANTIATE THE BASE-CLASS OBJECT USING THE DEFAULT BASE-
			CLASS CONSTRUCTOR and then explicitly configure the entire object by calling the appropriate
			'configuration()' method!
		*/

CUCSpline::CUCSpline ( stdBool bNaturalSpline )
			:	m_bNaturalSpline ( bNaturalSpline ),
				m_uAllocatedEntries ( 0 ),
				m_pafd2ydx2 ( NULL ) {
}

CUCSpline::CUCSpline ( UNativeInt uTableEntries,
		SUConfigurationTableEntry const *paConfigTable,
		stdBool bNaturalSpline )
			:	m_bNaturalSpline ( bNaturalSpline ),
				m_uAllocatedEntries ( 0 ),
				m_pafd2ydx2 ( NULL ) {
	try { configure ( uTableEntries, paConfigTable ); }
	catch ( ... ) { throw std::bad_alloc(); }
}


CUCSpline::CUCSpline ( UNativeInt uTableEntries,
		FloatDP const *pafIndependentValues, FloatDP const *pafDependentValues,
		stdBool bNaturalSpline )
			:	m_bNaturalSpline ( bNaturalSpline ),
				m_uAllocatedEntries ( 0 ),
				m_pafd2ydx2 ( NULL ) {
	try { configure ( uTableEntries, pafIndependentValues, pafDependentValues ); }
	catch ( ... ) { throw std::bad_alloc(); }
}

CUCSpline::CUCSpline ( UNativeInt uTableEntries,
		FloatDP fIndependentValueMin, FloatDP fIndependentValueMax,
		FNMAP *pfnMap, stdPointer pMapContext,
		stdBool bNaturalSpline )
			:	m_bNaturalSpline ( bNaturalSpline ),
				m_uAllocatedEntries ( 0 ),
				m_pafd2ydx2 ( NULL ) {
	try {
		configure ( uTableEntries, fIndependentValueMin, fIndependentValueMax,
			pfnMap, pMapContext );
	}
	catch ( ... ) { throw std::bad_alloc(); }
}

CUCSpline::CUCSpline ( CUCSpline const &Ref )
			:	m_bNaturalSpline ( TRUE ),
				m_uAllocatedEntries ( 0 ),
				m_pafd2ydx2 ( NULL ) {
	try { copy ( Ref ); }
	catch ( ... ) { throw std::bad_alloc(); }
}

/*
#if		defined ( __cplusplus ) && ( __cplusplus >= 201101L )			// C++11 or later

CUCSpline::CUCSpline ( CUCSpline &&Ref )
			:	m_bNaturalSpline ( TRUE ),
				m_uAllocatedEntries ( 0 ),
				m_pafd2ydx2 ( NULL ) {
	try { move ( Ref ); }
	catch ( ...) { throw std::bad_alloc(); }
}

#endif	// C++11
*/

CUCSpline::~CUCSpline() {
	allocateDerivedBuffers ( 0 );
}





void CUCSpline::configureNaturalSpline ( stdBool bUseNaturalSpline ) {
	if ( m_bNaturalSpline != bUseNaturalSpline ) {
		// configuration has changed...

		// change the state of the 'natural spline' flag and re-derive the configuration
		m_bNaturalSpline = bUseNaturalSpline;
		deriveConfiguration();
	}
}


///////// EXTERNAL FUNCTIONS ////////////////////////////////////////////////////////////////////////////





///////// POST-CODE COMPILER DIRECTIVES /////////////////////////////////////////////////////////////////

// end CSpline_cpp
