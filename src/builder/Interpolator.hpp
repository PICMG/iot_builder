/////////////////////////////////////////////////////////////////////////////////////////////////////////
// COPYRIGHT (C) 2020 TRIPLE RING TECHNOLOGIES, INC
//                    ALL RIGHTS RESERVED
//
//   File Name:	Interpolator.hpp
// Module Name: PICMG IIoT uSAM
//     Project:	TRT03/IIoT uSAM
//
// Description:	Performs interpolation (and extrapolation) from a data set.
//
//				This module comprises a base-class (generic) interpolator.  The base-class can be used
//				to generate other interpolators.
//
//				The base-class interpolator implements a linear interpolation by default.  Derive new
//				classes from it to create more complex interpolators.
//
// Rev History:	15 October 2020		A.P. Lowell
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
#ifndef		Interpolator_hpp
#define		Interpolator_hpp


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


///////// POST-INCLUDE PRE-PROCESSOR DIRECTIVES /////////////////////////////////////////////////////////





///////// MNEMONICS /////////////////////////////////////////////////////////////////////////////////////

	///////// DEFINES ///////////////////////////////////////////////////////////////////////////////////


	///////// ENUMS /////////////////////////////////////////////////////////////////////////////////////


///////// FORWARD STRUCT REFERENCES /////////////////////////////////////////////////////////////////////


///////// FORWARD CLASS REFERENCES //////////////////////////////////////////////////////////////////////

class CUInterpolator;


///////// TYPEDEFS //////////////////////////////////////////////////////////////////////////////////////

typedef CUInterpolator				CULinterp;	// alias this to a 'linear interpolator' class





///////// MACROS ////////////////////////////////////////////////////////////////////////////////////////


///////// FUNCTION PROTOTYPES ///////////////////////////////////////////////////////////////////////////





///////// STRUCT DECLARATIONS ///////////////////////////////////////////////////////////////////////////


///////// CLASS DECLARATIONS ////////////////////////////////////////////////////////////////////////////

class CUInterpolator {
		/*
			This implements both a linear interpolator AND a base-class for creating other interpolators
			that use different interpolation algorithms.  We presume that, at heart, all interpolators
			must have a basic configuration table of mappings between the independent and dependent
			variable values at some set of discrete points; this class maintains that and makes it
			available to derived classes for them to use in implementing their own interpolations.

			Various methods that extract 'derived' configurations from the basic configuration table
			and that perform the interpolations are made 'virtual' so that they can be overloaded in
			derived classes and then accessed in a generic way via a base-class reference/pointer.
		*/

	// public API
	public:
		// type definitions
			/*
				NOTE:	The following represents a function that maps an independent variable value
						to a dependent variable value.  It may be used to generate a configuration
						set algorithmically.

						NOTE that the 'context' variable is provided to allow you to pass some context
						to the mapping function.  That context may be some data structure that defines
						a parameterization for the mapping; or, if the 'map' function is defined as a
						static method within a class, it may be an object of that class through which
						the mapping function operates.
			*/
		typedef FloatDP /*dependent value*/
									FNMAP (
										FloatDP fIndependentValue,
										stdPointer pContext );

		// structures
		struct SUConfigurationTableEntry {
			FloatDP					fIndependentValue;
			FloatDP					fDependentValue;
		};


		// methods
			// construction/destruction/initialization
				/*
					See notes on 'configure' methods for argument formats
				*/
				/*
					:<SUBTLE>:
					Part of the configuration is to allocate the derived-class buffers, which is done
					via a virtual method ('allocateDerivedBuffers()'), and to extract the associated
					configurations, which is also done via a virtual method ('deriveConfiguration()').
					
					NEITHER OF THOSE WILL WORK CORRECTLY IF THE CONFIGURATION IS DONE WITHIN THE BASE-
					CLASS CONSTRUCTOR, BECAUSE THE VIRTUAL FUNCTION TABLE THAT POINTS TO THE DERIVED-
					CLASS VIRTUAL OVERLOADS WILL NOT YET HAVE BEEN ESTABLISHED!

					To overcome that limitation, DERIVED CLASSES MUST ALWAYS INSTANTIATE THE BASE-CLASS
					OBJECT USING THE DEFAULT BASE-CLASS CONSTRUCTOR and then explicitly configure the
					entire object by calling the appropriate 'configuration()' method!  DO NOT CALL THE
					BASE-CLASS CONSTRUCTORS THAT AUTOMATICALLY DO CONFIGURATION FROM WITHIN A DERIVED-
					CLASS CONSTRUCTOR!  IT WILL NOT WORK CORRECTLY IF YOU NEED TO ALLOCATE YOUR OWN
					DERIVED-CLASS BUFFERS AND/OR ESTABLISH DERIVED-CLASS CONFIGURATIONS VIA THE
					VIRTUAL METHODS!
				*/
				// default constructor (no configuration)
		CUInterpolator ( void );
				// configure from a table
		CUInterpolator ( UNativeInt uTableEntries,
				SUConfigurationTableEntry const *paConfigTable );
				// configure from separate arrays of independent and dependent values
		CUInterpolator ( UNativeInt uTableEntries,
				FloatDP const *pafIndependentValues, FloatDP const *pafDependentValues );
				// configure from an array of dependent values and a range of independent values
		CUInterpolator ( UNativeInt uTableEntries,
				FloatDP fIndependentValueMin, FloatDP fIndependentValueMax,
				FloatDP const *pafDependentValues );
				// configure from a mapping function and a range of independent values
		CUInterpolator ( UNativeInt uTableEntries,
				FloatDP fIndependentValueMin, FloatDP fIndependentValueMax,
				FNMAP *pfnMap, stdPointer pMapContext );
				// copy from another interpolator object
		CUInterpolator ( CUInterpolator const &Ref );
#if		defined ( __cplusplus ) && ( __cplusplus >= 201101L )			// C++11 or later
				// move from another interpolator object
		CUInterpolator ( CUInterpolator &&Ref );
#endif	// C++11

		virtual ~CUInterpolator();

				// report whether or not we have a valid configuration table from which to interpolate
				//
		inline stdBool tableIsInitialized ( void ) const { return ( 0 != m_uEntries ); }

		inline void copy ( CUInterpolator const &Ref ) { doCopy ( Ref ); }

				// configure the interpolator
				//
				/*
					NOTE:	A configuration set is an indexed list of independent and dependent
							variable pairs.  The 'independent' variable is the 'input' to the spline --
							what you specify when you want to interpolate; the 'dependent' variable is
							the output from the spline -- what is returned from the interpolation.

							It is not required that the independent variables be listed in any particular
							order, nor is it required that there be no dupicated entries -- the
							configuration method will take care of re-ordering things and/or removing
							duplicates as necessary.

							It is also not required that the interval between independent variable
							values from one index to the next be regular.  However, providing regularly-
							spaced intervals will, in general, make interpolation go faster because it
							minimizes the time required to find the appropriate place in the table
							from which to interpolate.  That said, this implementation provides a
							mechanism for optimizing that search process IFF consecutive values submitted
							to the interpolator are 'near' each other, as they would be if you were
							generating a complete curve by indexing from one end to the other, or as they
							would be if you were interpolating samples taken from a data acquisition
							system measuring a slowly-varying signal.
							
							This will work correctly for curves in any orientation, including curves that
							double back on themselves (i.e. there are multiple independent variable
							values that generate the same dependent variable value).
				*/
		void configure ( UNativeInt uTableEntries,
				SUConfigurationTableEntry const *paConfigTable );
		void configure ( UNativeInt uTableEntries,
				FloatDP const *pafIndependentValues, FloatDP const *pafDependentValues );
				/*
					NOTE:	The following synthesizes the independent values on a regularly-spaced
							interval based on the specified range of values.  It presumes that the
							'dependent' variable values have ALSO been sampled on that interval!
				*/
		void configure ( UNativeInt uTableEntries,
				FloatDP fIndependentValueMin, FloatDP fIndependentValueMax,
				FloatDP const *pafDependentValues );
				/*
					NOTE:	The following synthesizes the independent values on a regularly-spaced
							interval based on the specified range of values, and then synthesizes
							the corresponding dependent variable values using the specified mapping
							function.

							Yes, in general, if you have the mapping function then why would you bother
							creating an interpolation?  However, if the mapping function is compute-
							intensive, using the interpolator may reduce the computational load when
							you need to do a lot of calculations on an ongoing basis.
				*/
		void configure ( UNativeInt uTableEntries,
				FloatDP fIndependentValueMin, FloatDP fIndependentValueMax,
				FNMAP *pfnMap, stdPointer pMapContext );


		// accessors
		inline UNativeInt tableEntries ( void ) const { return m_uEntries; }

		UNativeInt /*# of entries*/ extractTable (
				SUConfigurationTableEntry *paTable, UNativeInt uMaxEntries ) const;

			// configuration table bounds
			//
				/*
					:<SUBTLE:>
					The following return the values from the lowest/highest table index, not the
					minimum/maximum values!

					Since we arrange for the 'independent variable' in the table to be ordered from
					minimum to maximum, the 'independent value' methods will return values that also
					correspond to the minimum/maximum independent variable values.  HOWEVER, the
					'dependent value' table is not guaranteed to be ordered in any particular sense
					(and may, in fact, change direction), so the values returned from the 'dependent
					value' methods may NOT correspond to the minimum/maximum dependent variable values.
				*/
		inline FloatDP lowestIndependentValue ( void ) const { return lowestXval(); }
		inline FloatDP highestIndependentValue ( void ) const { return highestXval(); }
		inline FloatDP lowestDependentValue ( void ) const { return lowestYval(); }
		inline FloatDP highestDependentValue ( void ) const { return highestYval(); }


			// operations
				// interpolate from an independent variable value to get a dependent variable value
				//
				/*
					:<SUBTLE>:
					The 'interpolate' function will also extrapolate beyond the ends of the
					configuration table based on the conditions observed at the table end-points.
					HOWEVER, extrapolation will quickly become inaccurate as you move farther from the
					end-points for any curve with a non-constant first derivative!

					Be warned....
				*/
				/*
					:<SUBTLE>:
					The 'interpolate' function is not declared 'const' becuase we keep track internally
					of the table index from which the last interpolation was taken, and that 'tracking'
					creates a change to the object as a result of the call to 'interpolate()'.  However,
					that internal change to the object affects ONLY the efficiency with which an
					interpolation may be done; it doesn't change the interpolated result or change the
					underlying configuration for the interpolator.
					
					We use the tracking information to  optimize table access in cases where the
					independent table entries are not equally spaced and we need to search for the
					correct table location from which to interpolate.  The optimization works when
					sequential calls to 'interpolate' are done with independent values that are "close"
					together, as they would be if they are being generated to sweep through a curve from
					one end to the other, or as samples taken from a slowly-varying physical channel.
				*/
				/*
					NOTE:	If no configuration has been established for the interpolator, then this
							method will simply return the independent variable value!
				*/
		FloatDP interpolate ( FloatDP fIndependentValue );


			// operators
		inline CUInterpolator const& operator= ( CUInterpolator const &Ref ) {
			copy ( Ref ); return (*this);
		}
//#if		defined ( __cplusplus ) && ( __cplusplus >= 201101L )			// C++11 or later
//		inline CUInterpolator const& operator= ( CUInterpolator &&Ref ) {
//			move ( Ref ); return (*this);
//		}
//#endif	// C++11

		inline FloatDP operator() ( FloatDP fIndependentValue ) {
			return interpolate ( fIndependentValue );
		}





	// protected API
	protected:
		// methods
			// construction/destruction/initialization
				/*
					NOTE:	Derived classes MAY overload the following to allocate additional buffers
							required for their customized operations.  By default, this is a noop
							(i.e. it doens't allocate any additional buffering).

							As with the 'allocateTable()' method, this should release any allocated
							buffers when the specified number of table entries is 0.

							This should throw an exception of type 'std::bad_alloc()' if it can't
							allocate its buffers!
				*/
		virtual void allocateDerivedBuffers ( UNativeInt uTableEntries ) {}

//#if		defined ( __cplusplus ) && ( __cplusplus >= 201101L )			// C++11 or later
//		inline void move ( CUInterpolator &&Ref ) { doMove ( Ref ); }
//#endif	// C++11

				/*
					NOTE:	derived classes MUST overload the following copy/move methods IF they add
							encapsulated data fields that must be copied/initialized!  The overloaded
							methods MUST call the base-class methods to do the base-class copy/move!
				*/
		virtual void doCopy ( CUInterpolator const &Ref );
#if		defined ( __cplusplus ) && ( __cplusplus >= 201101L )			// C++11 or later
		virtual void doMove ( CUInterpolator &&Ref );
#endif	// C++11


			// accessors
		inline SUConfigurationTableEntry const* tablePtr ( void ) const { return m_paTable; }

				/*
					NOTE:	The following accessors return the values from the specified configuration
							table entry.  IN THE INTEREST OF EFFICIENCY, THEY DO NOT VALIDATE THE
							SPECIFIED TABLE INDEX!  It is the responsibility of the coder using this
							class to do so before calling these methods!
					
							To keep things simple (i.e. so you don't have to keep typing 'independent'
							and 'dependent' in your code), we use the following short-hand nomenclature
							to refer to the dependent and and independent varibles in the configuration
							table:

								indpedendent variable = 'x'
								dependent variable = 'y'
				*/
				// table entry values
				//
					/*
						NOTE:	We don't allow you to change these once they have been initialized!
								They represent our "ground truth" configuration.
					*/
		inline FloatDP const& xval ( UNativeInt uIndex ) const {
			return m_paTable[uIndex].fIndependentValue;
		}
		inline FloatDP const& yval ( UNativeInt uIndex ) const {
			return m_paTable[uIndex].fDependentValue;
		}

				// first derivative values
				//
					/*
						NOTE:	We give you write access to these, in case you want to update them with
								some more refined estimate for a specific algorithm.  By default, they
								are intialized with the first-difference from the ending table entries.
					*/
					// low end of the independent variable value range
		inline FloatDP const& dydx_Low ( void ) const { return m_fdydx_L; }
		inline FloatDP& dydx_Low ( void ) { return m_fdydx_L; }
					// high end of the independent variable value range
		inline FloatDP const& dydx_High ( void ) const { return m_fdydx_H; }
		inline FloatDP& dydx_High ( void ) { return m_fdydx_H; }

				// bounding table values
				//
		inline UNativeInt lowestTableIndex ( void ) const { return 0; }
		inline UNativeInt highestTableIndex ( void ) const { return ( m_uEntries - 1 ); }

					/*
						:<SUBTLE:>
						The following return the values from the lowest/highest table index, not the
						minimum/maximum values!

						Since we arrange for the 'x' table to be ordered from minimum to maximum, the
						'x' methods will return values that also correspond to the minimum/maximum
						independent variable values.  HOWEVER, the 'y' table is not guaranteed to be
						ordered in any particular sense (and may, in fact, change direction), so the
						values returned from the 'y' methods may NOT correspond to the minimum/maximum
						dependent variable values.
					*/
		inline FloatDP lowestXval ( void ) const { return xval ( lowestTableIndex() ); }
		inline FloatDP highestXval ( void ) const { return xval ( highestTableIndex() ); }
		inline FloatDP lowestYval ( void ) const { return yval ( lowestTableIndex() ); }
		inline FloatDP highestYval ( void ) const { return yval ( highestTableIndex() ); }


			// operators
				// Return the table entry index for the table entry that contains the next-lowest
				// independent value -- that is, the largest table entry index for which:
				//
				//		<table x value>[index] <= fxval
				//
				// In common parlance, this returns the index for the table entry to the 'left' of the
				// specified x value.
				//
					/*
						NOTE:	How this is done depends on whether or not the independent values in the
								table are spaced at regular intervals and whether or not the current
								'x' values is close to or far away from the previous 'x' value.  Derived
								classes should use this method to locate themselves within the table,
								rather than assuming some relationship between the table entries.

								Derived classes may, of course, explicitly do the search for the proper
								table location themselves.  This, method is, however, designed to do that
								search for them in an optimal fashion...
					*/
					/*
						:<SUBTLE>:
						The interpolation method will call the 'extrapolation' implementation methods
						when the specified 'xval' is AT one end or the other of the table.  Hence, if
						you are working within the 'doInterpolate()' method (below), a call to this
						index mapper is guaranteed to NOT return the index for the last entry in the
						table -- the returned index will always be lower than that.  Hence, in that
						case, it is safe to add 1 to the index returned by this method and use that
						incremented index to access a table entry (i.e. to access the table entry to
						the 'right' of the specified 'xval').
					*/
		UNativeInt mapTableIndex ( FloatDP fxval );


			// implementation
				/*
					NOTE:	The following may be overloaded by a derived class that has some special
							requirement for the number of table entries it can accept for configuration
							(like it must have a least 5 entries).  By default, the requirement imposed
							is that a table must have at least 2 entries so that there is some range
							between then from which to interpolate.

							A value of 0 for the number of table entries is ALWAYS valid, since that
							is how we release the table for destruction.  Hence, that is validated
							separately and doesn't need to be included in this method -- assume this
							method is called because we already know the number is not 0.
				*/
		virtual stdBool tableSizeIsValid ( UNativeInt uTableEntries ) { return ( uTableEntries >= 2 ); }

				/*
					NOTE:	The following should be overloaded by a derived class that must intialize
							more than only the actual configuration table.  The default implementation
							is a noop, since the linear interpolator doesn't require any more
							configuration than that.
				*/
				/*
					:<SUBTLE>:
					This method assumes that all other configuration inputs have been established within
					the object data.  If there are 'extra' configurations for a given interploator (like
					whether or not to use a 'natural' spline for a cublic-spline interpolation), those
					must be reflected in the object data before this is called!

					In particular, since this is called as the last step of 'configuration' using any of
					the 'configuration()' methods, all such parameter values must have been established
					prior to calling 'configure()' using the standard base-class configuration APIs!
				*/
		virtual void deriveConfiguration ( void ) {}


			// operations
				/*
					NOTE:	The following MAY be overloaded by a derived class to customize the
							extrapolation/interpolation behavior.  By default, we do a linear
							extrapolation from the end-points using the calculated first derivatives,
							and a linear interpolation between table elements.

							If, for example, your derived interpolator will still do a linear
							extrpolation despite providing a different interpolation method, then you
							could keep the default extrapolators and only overload the 'doInterpolate()'
							method.
				*/
		virtual FloatDP doExtrapolateLow ( FloatDP fxval );
		virtual FloatDP doExtrapolateHigh ( FloatDP fxval );
		virtual FloatDP doInterpolate ( FloatDP fxval, UNativeInt uEntry );





	// private API
	private:
		// methods
			// construction/destruction/initialization
		stdBool /*true=we have valid buffers*/ allocateTable ( UNativeInt uTableEntries );





	// encapsulated data
	private:
		// specified configuration table
		UNativeInt					m_uAllocatedEntries;
		UNativeInt					m_uEntries;
		SUConfigurationTableEntry*	m_paTable;

		// derived configuration values
		FloatDP						m_fdydx_L;			// first derivative, low end of the x range
		FloatDP						m_fdydx_H;			// first derivative, high end of the x range

		FloatDP						m_fXStep;			// 0.0 if the x spacing is NOT regular!

		// state information
		UNativeInt					m_uLastEntry;
};





///////// CONSTANTS /////////////////////////////////////////////////////////////////////////////////////


///////// VARIABLES /////////////////////////////////////////////////////////////////////////////////////





///////// POST-CODE COMPILER DIRECTIVES /////////////////////////////////////////////////////////////////

#endif		/* Interpolator_hpp */
