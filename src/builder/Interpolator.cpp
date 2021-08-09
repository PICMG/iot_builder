/////////////////////////////////////////////////////////////////////////////////////////////////////////
// COPYRIGHT (C) 2020 TRIPLE RING TECHNOLOGIES, INC
//                    ALL RIGHTS RESERVED
//
//   File Name:	Interpolator.cpp
// Module Name: PICMG IIoT uSAM
//     Project:	TRT03/IIoT uSAM
//
// Description:	Performs interpolation (and extrapolation) from a data set.
//
//				This module comprises a base-class (generic) interpolator.  The base-class can be used
//				to generate other interpolators.
//
//				The base-class interpolator implements a linear interpolation by default.
//
// Rev History:	15 October 2020		A.P. Lowell
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
#define		Interpolator_cpp


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
	// class CUInterpolator
	//

stdBool /*true=we have valid buffers*/ CUInterpolator::allocateTable ( UNativeInt uTableEntries ) {
	// are we releasing the table or has the size of the table increased?
	if ( ( 0 == uTableEntries ) || ( uTableEntries > m_uAllocatedEntries ) ) {
		// yes -- is there an old buffer to release?
		if ( 0 != m_uAllocatedEntries ) {
			// yes -- release the old buffer
			delete [] m_paTable;
			m_paTable = NULL;
			m_uEntries = m_uAllocatedEntries = 0;
		}

		// do we need to allocate a new buffer?
		if ( 0 == uTableEntries ) {
			// no -- we are done
				// release any extra buffers we have allocated...
			allocateDerivedBuffers ( 0 );
			return FALSE;
		}

		// allocate a new buffer
			/*
				:<SUBTLE>:
				The following will trap a failure to allocate whether the 'new' method throws an
				exception or simply returns a null pointer.  We assume we can't count on one
				behavior or the other...
			*/
		try { m_paTable = new SUConfigurationTableEntry [ uTableEntries ]; }
		catch ( ... ) { m_paTable = NULL; }
		if ( NULL == m_paTable ) { throw std::bad_alloc(); }

		m_uAllocatedEntries = uTableEntries;

		// allocate any additional buffers required for derived classes
		try { allocateDerivedBuffers ( m_uAllocatedEntries ); }
		catch ( ... ) {
			// we failed to allocate the derived buffers -- release our own buffer and fail!
			delete [] m_paTable;
			m_paTable = NULL;
			m_uEntries = m_uAllocatedEntries = 0;
			throw;
		}
	}
	// if we got here, we have a valid set of buffers of non-zero length!
	m_uEntries = uTableEntries;

	return TRUE;
}


///////// LOCAL FUNCTIONS ///////////////////////////////////////////////////////////////////////////////


///////// PROTECTED/PUBLIC METHODS //////////////////////////////////////////////////////////////////////

	/////////////////////////////////////////////////////////////////////////////////////////////////////
	// class CUInterpolator
	//

CUInterpolator::CUInterpolator ( void )
			:	m_uAllocatedEntries ( 0 ),
				m_uEntries ( 0 ),
				m_paTable ( NULL ) {
}

CUInterpolator::CUInterpolator ( UNativeInt uTableEntries,
		SUConfigurationTableEntry const *paConfigTable )
			:	m_uAllocatedEntries ( 0 ),
				m_uEntries ( 0 ),
				m_paTable ( NULL ) {
	try { configure ( uTableEntries, paConfigTable ); }
	catch ( ... ) { throw std::bad_alloc(); }
}


CUInterpolator::CUInterpolator ( UNativeInt uTableEntries,
		FloatDP const *pafIndependentValues, FloatDP const *pafDependentValues )
			:	m_uAllocatedEntries ( 0 ),
				m_uEntries ( 0 ),
				m_paTable ( NULL ) {
	try { configure ( uTableEntries, pafIndependentValues, pafDependentValues ); }
	catch ( ... ) { throw std::bad_alloc(); }
}

CUInterpolator::CUInterpolator ( UNativeInt uTableEntries,
		FloatDP fIndependentValueMin, FloatDP fIndependentValueMax,
		FNMAP *pfnMap, stdPointer pMapContext )
			:	m_uAllocatedEntries ( 0 ),
				m_uEntries ( 0 ),
				m_paTable ( NULL ) {
	try {
		configure ( uTableEntries, fIndependentValueMin, fIndependentValueMax,
			pfnMap, pMapContext );
	}
	catch ( ... ) { throw std::bad_alloc(); }
}

CUInterpolator::CUInterpolator ( CUInterpolator const &Ref )
			:	m_uAllocatedEntries ( 0 ),
				m_uEntries ( 0 ),
				m_paTable ( NULL ) {
	try { copy ( Ref ); }
	catch ( ... ) { throw std::bad_alloc(); }
}

/*
#if		defined ( __cplusplus ) && ( __cplusplus >= 201101L )			// C++11 or later

CUInterpolator::CUInterpolator ( CUInterpolator &&Ref )
			:	m_uAllocatedEntries ( 0 ),
				m_uEntries ( 0 ),
				m_paTable ( NULL ) {
	try { move ( Ref ); }
	catch ( ...) { throw std::bad_alloc(); }
}

#endif	// C++11
*/

CUInterpolator::~CUInterpolator() {
	(void)allocateTable ( 0 );
}





void CUInterpolator::configure ( UNativeInt uTableEntries,
		SUConfigurationTableEntry const *paConfigTable ) {
	UNativeInt				ui, uj;
	SUConfigurationTableEntry
							Temp;
	FloatDP					fInterval, fTolerance;

	if ( 0 != uTableEntries && NULL == paConfigTable ) {
		// THERE IS NO TABLE FROM WHICH TO INITIALIZE!
		throw std::invalid_argument ( "No Table Init" );
	}
	if ( 0 != uTableEntries && !tableSizeIsValid ( uTableEntries ) ) {
		// THE REQUESTED TABLE SIZE IS INVALID!
		throw std::invalid_argument ( "Invalid Table Length" );
	}
	if ( !allocateTable ( uTableEntries ) ) {
		// buffer has been deallocated -- nothing to do
		return;
	}
	// if we got here, we have a valid buffer of non-zero length!

	// copy the specified configuration table into the internal table
	memcpy ( (void*)m_paTable, (void const *)paConfigTable, sizeof(m_paTable[0]) * m_uEntries );

	// sort the table into increasing order of the independent variable
	for ( ui=0 ; ui < ( m_uEntries - 1 ) ; ++ui ) {
		for ( uj=(ui+1) ; uj < m_uEntries ; ++uj ) {
			if ( xval(uj) < xval(ui) ) {
				Temp = m_paTable[uj];
				m_paTable[uj] = m_paTable[ui];
				m_paTable[ui] = Temp;
			}
		}
	}

	// remove any duplicate entries from the table
	for ( ui=0 ; ui < ( m_uEntries - 1 ) ; ++ui ) {
		if ( xval(ui+1) != xval(ui) ) {
			// this is not a duplicate...
			continue;
		}
		// we found a duplicate entry -- remove it!
		memmove ( (void*)( m_paTable + ui ), (void const *)( m_paTable + ui + 1 ),
			sizeof(m_paTable[0]) * ( m_uEntries - ( ui + 1 ) ) );
		--m_uEntries;
	}
	if ( !tableSizeIsValid ( m_uEntries ) ) {
		// removing the duplicates has left the table too small!
		allocateTable ( 0 );
		throw std::invalid_argument ( "Invalid Table Length" );
	}

	// determine whether the independent values are spaced regularly within the table
		// establish the nominal X step
#if 0
	m_fXStep = ( highestXval() - lowestXval() ) / (FloatDP)( m_uEntries - 1 );
		// establish a tolerance for 'irregularity'
	fTolerance = 1.0e-6 * m_fXStep;
		// check the intervals for irregularities
	for ( ui=1 ; ui < ( m_uEntries - 1 ) ; ++ui ) {
		fInterval = xval(ui+1) - xval(ui);
		if ( fabs ( fInterval - m_fXStep ) > fTolerance ) {
			// this doesn't appear to be regularly-spaced!
			m_fXStep = 0.0;
			break;
		}
	}
#else
	m_fXStep = 0.0;
#endif

	// establish the first derivatives
	m_fdydx_L = ( yval(lowestTableIndex()+1) - yval(lowestTableIndex()) )
		/ ( xval(lowestTableIndex()+1) - xval(lowestTableIndex()) );
	m_fdydx_H = ( yval(highestTableIndex()) - yval(highestTableIndex()-1) )
		/ ( xval(highestTableIndex()) - xval(highestTableIndex()-1) );

	// derive any other configurations from the configuration table
	deriveConfiguration();

	// initialize the 'last entry' to the center of the table (for lack of a better guess)
		/*
			NOTE:	Starting in the center should optimize the first search if the first independent
					value from which we expect to interpolate will be randomly placed within the
					range of independent values.

					Starting at the left end of the table should optimize the first search if we
					expect the interpolator to be used to synthesize an interpolation curve in a
					systematic way by sweeping through the independent values from low to high.

					Your guess is as good as mine as to which it will be.  I have chosen to optimize
					for an application in which we are interpolating from samples that could begin at
					any abritrary value...
		*/
	m_uLastEntry = ( m_uEntries - 1 ) / 2;
}

void CUInterpolator::configure ( UNativeInt uTableEntries,
		FloatDP const *pafIndependentValues, FloatDP const *pafDependentValues ) {
	SUConfigurationTableEntry
							*paTable;
	UNativeInt				uEntry;

	if ( 0 != uTableEntries
			&& ( NULL == pafIndependentValues || NULL == pafDependentValues ) ) {
		// THERE IS NO TABLE FROM WHICH TO INITIALIZE!
		throw std::invalid_argument ( "No Table Init" );
	}
	if ( 0 != uTableEntries && !tableSizeIsValid ( uTableEntries ) ) {
		// THE REQUESTED TABLE SIZE IS INVALID!
		throw std::invalid_argument ( "Invalid Table Length" );
	}

	try { paTable = new SUConfigurationTableEntry [ uTableEntries ]; }
	catch ( ... ) { paTable = NULL; }
	if ( NULL == paTable ) { throw std::bad_alloc(); }

	try {
		for ( uEntry=0 ; uEntry < uTableEntries ; ++uEntry ) {
			paTable[uEntry].fIndependentValue = pafIndependentValues[uEntry];
			paTable[uEntry].fDependentValue = pafDependentValues[uEntry];
		}

		configure ( uTableEntries, paTable );

		delete [] paTable;
	}
	catch ( ... ) {
		delete [] paTable;
		throw;
	}
}

void CUInterpolator::configure ( UNativeInt uTableEntries,
		FloatDP fIndependentValueMin, FloatDP fIndependentValueMax,
		FloatDP const *pafDependentValues ) {
	FloatDP					fDRange, fIRange;
	SUConfigurationTableEntry
							*paTable;
	UNativeInt				uEntry;

	if ( 0 != uTableEntries && NULL == pafDependentValues ) {
		// THERE IS NO TABLE FROM WHICH TO INITIALIZE!
		throw std::invalid_argument ( "No Table Init" );
	}
	if ( 0 != uTableEntries && !tableSizeIsValid ( uTableEntries ) ) {
		// THE REQUESTED TABLE SIZE IS INVALID!
		throw std::invalid_argument ( "Invalid Table Length" );
	}

	if ( fIndependentValueMax < fIndependentValueMin ) {
		fDRange = fIndependentValueMin;
		fIndependentValueMin = fIndependentValueMax;
		fIndependentValueMin = fDRange;
	}
	if ( fIndependentValueMax == fIndependentValueMin ) {
		// THERE IS NO RANGE OVER WHICH TO INITIALIZE!
		throw std::invalid_argument ( "No Table Range" );
	}

	try { paTable = new SUConfigurationTableEntry [ uTableEntries ]; }
	catch ( ... ) { paTable = NULL; }
	if ( NULL == paTable ) { throw std::bad_alloc(); }

	try {
		// initialize the buffers
			// generate the independent values
		fIRange = (FloatDP)( m_uEntries - 1 );
		fDRange = fIndependentValueMax - fIndependentValueMin;
		for ( uEntry=0 ; uEntry < ( uTableEntries - 1 ) ; ++uEntry ) {
			paTable[uEntry].fIndependentValue =
				fIndependentValueMin + fDRange * ( (FloatDP)uEntry / fIRange );
			paTable[uEntry].fDependentValue = pafDependentValues[uEntry];
		}
		paTable[uEntry].fIndependentValue = fIndependentValueMax;
		paTable[uEntry].fDependentValue = pafDependentValues[uEntry];

		configure ( uTableEntries, paTable );

		delete [] paTable;
	}
	catch ( ... ) {
		delete [] paTable;
		throw;
	}
}

void CUInterpolator::configure ( UNativeInt uTableEntries,
		FloatDP fIndependentValueMin, FloatDP fIndependentValueMax,
		FNMAP *pfnMap, stdPointer pMapContext ) {
	FloatDP					fDRange, fIRange;
	SUConfigurationTableEntry
							*paTable;
	UNativeInt				uEntry;

	if ( 0 != uTableEntries && NULL == pfnMap ) {
		// THERE IS NO MAPPING FUNCTION FROM WHICH TO INITIALIZE!
		throw std::invalid_argument ( "No Table Init" );
	}
	if ( 0 != uTableEntries && !tableSizeIsValid ( uTableEntries ) ) {
		// THE REQUESTED TABLE SIZE IS INVALID!
		throw std::invalid_argument ( "Invalid Table Length" );
	}

	if ( fIndependentValueMax < fIndependentValueMin ) {
		fDRange = fIndependentValueMin;
		fIndependentValueMin = fIndependentValueMax;
		fIndependentValueMin = fDRange;
	}
	if ( fIndependentValueMax == fIndependentValueMin ) {
		// THERE IS NO RANGE OVER WHICH TO INITIALIZE!
		throw std::invalid_argument ( "No Table Range" );
	}

	try { paTable = new SUConfigurationTableEntry [ uTableEntries ]; }
	catch ( ... ) { paTable = NULL; }
	if ( NULL == paTable ) { throw std::bad_alloc(); }

	try {
		// initialize the buffers
			// generate the independent values
		fIRange = (FloatDP)( m_uEntries - 1 );
		fDRange = fIndependentValueMax - fIndependentValueMin;
		for ( uEntry=0 ; uEntry < ( uTableEntries - 1 ) ; ++uEntry ) {
			paTable[uEntry].fIndependentValue =
				fIndependentValueMin + fDRange * ( (FloatDP)uEntry / fIRange );
			paTable[uEntry].fDependentValue = pfnMap ( paTable[uEntry].fIndependentValue, pMapContext );
		}
		paTable[uEntry].fIndependentValue = fIndependentValueMax;
		paTable[uEntry].fDependentValue = pfnMap ( paTable[uEntry].fIndependentValue, pMapContext );

		configure ( uTableEntries, paTable );

		delete [] paTable;
	}
	catch ( ... ) {
		delete [] paTable;
		throw;
	}
}





UNativeInt /*# of entries*/ CUInterpolator::extractTable (
		SUConfigurationTableEntry *paTable, UNativeInt uMaxValues ) const {
	UNativeInt				uEntry;

	if ( 0 == m_uEntries ) { return 0; }

	if ( uMaxValues > m_uEntries ) { uMaxValues = m_uEntries; }
	if ( NULL != paTable ) {
		for ( uEntry=0 ; uEntry < uMaxValues ; ++uEntry ) {
			*(paTable++) = m_paTable[uEntry];
		}
	}

	return uMaxValues;
}




		
FloatDP CUInterpolator::interpolate ( FloatDP fIndependentValue ) {
	UNativeInt				uL;

	if ( 0 == m_uEntries ) {
		// no interpolation!
		return fIndependentValue;
	}
	else if ( fIndependentValue <= lowestXval() ) {
		// we are extrapolating to the left!
		m_uLastEntry = lowestTableIndex();
		return doExtrapolateLow ( fIndependentValue );
	}
	else if ( fIndependentValue >= highestXval() ) {
		// we are extrapolating to the right!
		m_uLastEntry = highestTableIndex();
		return doExtrapolateHigh ( fIndependentValue );
	}
	else {
		// we are interpolating
		uL = mapTableIndex ( fIndependentValue );
		m_uLastEntry = uL;
		return doInterpolate ( fIndependentValue, uL );
	}
}





void CUInterpolator::doCopy ( CUInterpolator const &Ref ) {
	configure ( Ref.m_uEntries, Ref.m_paTable );
}


#if		defined ( __cplusplus ) && ( __cplusplus >= 201101L )			// C++11 or later

void CUInterpolator::doMove ( CUInterpolator &&Ref ) {
	m_uAllocatedEntries = Ref.m_uAllocatedEntries;
	m_uEntries = Ref.m_uEntries;
	m_paTable = Ref.m_paTable;

	Ref.m_uAllocatedEntries = 0;
	Ref.m_uEntries = 0;
	Ref.m_paTable = NULL;

	m_fdydx_L = Ref.m_fdydx_L;
	m_fdydx_H = Ref.m_fdydx_H;

	m_fXStep = Ref.m_fXStep;
	m_uLastEntry = Ref.m_uLastEntry;
}

#endif	// C++11





/*
#if		defined ( __cplusplus ) && ( __cplusplus >= 201101L )			// C++11 or later

void CUInterpolator::move ( CUInterpolator &&Ref ) {
	m_uEntries = Ref.m_uEntries;
	m_paTable = Ref.m_paTable;

	Ref.m_uEntries = 0;
	Ref.m_paTable = NULL;

	m_fdydx_L = Ref.m_fdydx_L;
	m_fdydx_H = Ref.m_fdydx_H;
	m_fXStep = Ref.m_fXStep;

	m_uLastEntry = Ref.m_uLastEntry;
}

#endif	// C++11
*/




UNativeInt CUInterpolator::mapTableIndex ( FloatDP fxval ) {
	UNativeInt				uL, uR, uTest;

	// find the proper table location from which to interpolate
		/*
			:<SUBTLE>:
			If the table is marked as having regular X spacing, then we can simply calculate the
			proper table location directly from that X spacing.

			If the table is NOT marked as having regular X spacing, then we need to search for the
			proper place in the table.

			We record the place in the table from which we took the LAST sample so we can start our
			search from there.  If we are calling this to work from a data pattern -- for example,
			run through an increasing sequence of values or to interpolate from a sampled data
			stream that represents a slowly-changing physical parameter -- then that should minimize
			the search time since new samples are likely to fall within the same local area of the
			table as old samples.  However, if this is being called on randomly varying values, then
			we are stuck doing a brute-force search most of the time....
		*/
		/*
			NOTE:	Since we use the 'extrapolation' methods to handle values that are AT the
					table boundaries, we know a priori that the argument to this method is
					somewhere BETWEEN the table boundaries!
		*/
	if ( 0.0 != m_fXStep ) {
		// the X values are regularly spaced -- just calculate the proper table position directly
		uL = (UNativeInt)floor ( ( fxval - xval(0) ) / m_fXStep );
			/*
				:<SUBTLE>:
				The following step is here to deal with the potential for arcane rounding errors...
			*/
		if ( uL >= highestTableIndex() ) { --uL; }
	}
	else {
		// the X values are NOT regularly spaced -- we need to search for the proper table location...
		do {
			// start by assuming we need to be where we were last time
			uL = m_uLastEntry;
			if ( uL == highestTableIndex() ) { --uL; }
			uR = uL + 1;
			if ( ( fxval >= xval(uL) ) && ( fxval < xval(uR) ) ) {
				// we are still within the same range as last time -- we can use it!
				break;
			}
			// we are no longer where we were -- where do we need to go?
			if ( fxval > xval(uL) ) {
				// we need to start our search above where we were in the table
				
				// start by checking whether the next entry up will work...
				++uL; ++uR;
				if ( ( fxval >= xval(uL) ) && ( fxval < xval(uR) ) ) {
					// yes -- this is the range
					break;
				}
				// no, that didn't work -- we'll need to search for it...
				uR = highestTableIndex();
			}
			else {
				// we need to start our search below where we were in the table

				// start by checking whether the next entry down will work...
				--uL; --uR;
				if ( ( fxval >= xval(uL) ) && ( fxval < xval(uR) ) ) {
					// yes -- this is the range
					break;
				}
				// no, that didn't work -- we'll need to search for it...
				uL = lowestTableIndex();
			}

			while ( ( uR - uL ) > 1 ) {
				uTest = ( uR + uL ) >> 1;
				if ( xval(uTest) > fxval ) {
					uR = uTest;
				}
				else {
					uL = uTest;
				}
			}
		} while ( FALSE );
	}
	// we now have the bounding table locations

	return uL;
}





FloatDP CUInterpolator::doExtrapolateLow ( FloatDP fxval ) {
	UNativeInt				uL;

	// linear extrapolation using the first derivative

	uL = lowestTableIndex();
	return ( yval(uL) -	( xval(uL) - fxval ) * m_fdydx_L );
}


FloatDP CUInterpolator::doExtrapolateHigh ( FloatDP fxval ) {
	UNativeInt				uR;

	// linear extrapolation using the first derivative

	uR = highestTableIndex();
	return ( yval(uR) + ( fxval - xval(uR) ) * m_fdydx_H );
}


FloatDP CUInterpolator::doInterpolate ( FloatDP fxval, UNativeInt uEntry ) {
	UNativeInt				uL, uR;
	FloatDP					fwL, fwR;

	// linear interpolation

	uL = uEntry;
	uR = uL + 1;
	fwL = ( xval(uR) - fxval ) / ( xval(uR) - xval(uL) );
	fwR = 1.0 - fwL;

	return ( fwL * yval(uL) + fwR * yval(uR) );
}


///////// EXTERNAL FUNCTIONS ////////////////////////////////////////////////////////////////////////////





///////// POST-CODE COMPILER DIRECTIVES /////////////////////////////////////////////////////////////////

// end Interpolator_cpp
