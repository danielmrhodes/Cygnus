#ifndef PointCoulEx_h
#define PointCoulEx_h

#include "StatisticalTensor.h"
#include "Nucleus.h"
#include "Reaction.h"
#include "TVectorD.h"
#include "TMatrixD.h"
#include <vector>
#include <array>
#include <algorithm>
#include <complex>
#include <iostream>
#include <fstream>
#include <ctime>
#include <chrono>

#include "StatisticalTensor.h"
#include "Nucleus.h"
#include "Reaction.h"
#include "MiscFunctions.h"
#include "Connection.h"
#include "State.h"
#include "Substate.h"

class Nucleus;
class Reaction;

///
///	\class PointCoulEx
///
///	\brief Calculate point Coulomb excitation amplitudes and associated
///	statistical tensors
///
///	This class does the majority of the heavy lifting, calculating the 
///	Coulomb-excitation amplitudes and probabilities. The syntax borrows 
///	heavily from both GOSIA and CLX methods, but is organized rather 
///	differently.
///
///	Connection information, rather than being stored in a number of 
///	matrices (as in CLX and GOSIA) is collected in a vector of Substate
///	objects. 
///
///	The program then cycles through these substates during amplitude 
///	calculation.
///
///	The numerical integration of the coupled-differential equations is 
///	performed using the CLX method:\n
///	A Runge-Kutta algorithm is used to determine four initial points, 
///	before the Adams-Moulton method is employed. This differs from GOSIA,
/// 	in which a pertubative technique is used to determine initial points. 
///	In practice, differences between methods are insignificant.
///
///	The class also calculates the statistical tensors in both the reaction
///	frame and the laboratory frame and stores them in StatisticalTensor 
///	objects (fTensors and fTensorsB).
///

class PointCoulEx 
{

	public : 
			
		PointCoulEx();
		PointCoulEx(Nucleus*, Reaction*);			/*!< Constructor, passing Nucleus and Reactions immediately */
		PointCoulEx(const PointCoulEx& p);			/*!< Copy constructor */
		PointCoulEx& operator = (const PointCoulEx& p);		/*!< Assignment operator */
		~PointCoulEx() {;}

		TVectorD		PointProbabilities(double);	/*!< Calculate the point probabilities and immediately pass them */
		void			CalculatePointProbabilities(double);	/*!< Calculate the point probabilities */

		TVectorD		GetProbabilitiesVector() const	{ return Probabilities;	}	/*!< Returns previously calculated point probabilities */

		void			SetNucleus(Nucleus* nucl) 	{ fNucleus = *nucl;	}	/*!< Define the Nucleus to be used in the point calculation */
		void			SetReaction(Reaction* reac)	{ fReaction = *reac;	}	/*!< Define the Reaction kinematics for the point calculation */

		void			SetProjectileExcitation(bool b = true)	{ 
										projectileExcitation = b; 	
										PrepareConnections();	
									}				/*!< Define whether projectile or target excitation is being calculated */

		void			SetTargetDetection(bool b = true)	{ bTargetDetection = b;		}	/*!< Define whether the beam or target nucleus was detected */
	
		Nucleus*		GetNucleus()			{ return &fNucleus;	}	/*!< Return the Nucleus used in the calculation */
		Reaction*		GetReaction()			{ return &fReaction;	}	/*!< Return the Reaction used in the calculation */

		void			SetVerbose(bool b = true)	{ verbose = b;		}	/*!< Define the verbocity of the calculation */

		void			PrepareConnections();						/*!< Before performing the calculation, set up the connections between the substates */

		void			SetAccuracy(double acc)		{ fAccuracy = acc;	}	/*!< Define the accuracy of the point calculation */

		void			Print();							/*!< Simple print function */

		void			WriteDetailsToFile(const char* outfilename = "CoulEx_CalculationDetails.txt");	/*!< Write the calculation details to a text file for debugging purposes  */
		void			WriteMatrix(std::ofstream&, TMatrixD);				/*!< Write a well formatted TMatrixD */
		void			WriteConnections(std::ofstream&);				/*!< Write the connections between substates in a well formatted manner */
		void                    WriteTensorsToFile(const char* outfilename, std::ios_base::openmode mode = std::ios_base::out);                    /*!< Write LAB frame tensors to a text file  */
		void                    WriteTensorsBToFile(const char* outfilename, std::ios_base::openmode mode = std::ios_base::out);                   /*!< Write Excitation frame tensors to a text file  */

		TMatrixD		GetFinalRealAmplitude()	const	{ return FinalRealAmplitude;	}	/*!< Return the real components of the final amplitudes  */
		TMatrixD		GetFinalImagAmplitude()	const	{ return FinalImagAmplitude;	}	/*!< Return the imaginary components of the final amplitudes */

		void			CalculateTensors();							/*!< Calculate the statistical tensors based on the final amplitudes */
		StatisticalTensor	GetTensors()			{ return fTensors;		}	/*!< Return the calculated statistical tensors */
		StatisticalTensor	GetTensorsB()			{ return fTensorsB;		}	/*!< Return the calculated statistical tensors */
	
		void			TrackReaction(bool b = true)	{ fTrack = b;			}	/*!< Track the reaction, step-by-step. Off by default. */
		bool			Tracking()		const	{ return fTrack;		}	/*!< Is the reaction being tracker? */

		double					GetEpsilon()		const	{ return fEpsilon;		}	/*!< Return the epsilon (theta proxy) of the reaction */
		std::vector<double>			GetOmega()		const	{ return fOmegaTracking;	}	/*!< Return a vector of the omega (time proxy) values during the reaction */
		std::vector<std::vector<double>> 	GetProbabilityTrack()	const	{ return fStateProbTracking;	}	/*!< Return the probabilities, step-by-step, during the reaction*/

		double			GetTheta()				const	{ return fTheta;		}	/*!< Return the theta value */

		void			FixStep(bool b = true)		{ fUseFixedStep = b;		}	/*!< Set whether a fixed step (dOmega = 0.03, accuracy 1e-6) can be used - approx. equivalent to GOSIA INT,1000 */
		bool			UseFixedStep()		const	{ return fUseFixedStep;		}	/*!< Return whether a fixed step will be used for the CoulEx calculation */

		void			SetUseSymmetry(bool b = true)	{ fUseSymmetry = b;		}	/*!< Sets whether symmetry arguments can be used to speed up the calculation (default = true) */
		bool			UseSymmetry()		const	{ return fUseSymmetry;		}	/*!< Returns whether symmetry arguments can be used to speed up the calculation */             	

	private :

		bool			fUseFixedStep;
		bool			fUseSymmetry;	

		double			fAccuracy;

		std::vector<Substate>	fSubstates;
		std::vector<State>	fStates;

		double			fTheta;

		double			fA1;
		double			fA2;
		double			fZ1;
		double			fZ2;

		std::vector<int>	IFAC;

		double 			XiMax;

		bool			verbose;
		bool 			debug;
		bool			projectileExcitation;
		bool			bTargetDetection;

		Nucleus			fNucleus;
		Reaction		fReaction;

		TVectorD		fSubStateProbabilities;

		TVectorD		Integration(double);

		int			LMax;

		std::vector< std::complex< double > > 	CollisionFunction(int, double, double);

		std::vector<TMatrixD>	ComputeAmpDerivativeMatrices(std::vector<TMatrixD>, double, double);

		TVectorD		Probabilities;
		TMatrixD		FinalRealAmplitude;
		TMatrixD		FinalImagAmplitude;
	
		double 			ElectricDipoleNormalization(); 	
		double 			E1PolFactor; 		

		double			NormalizationFactor;
		int 			IntegerSpin; 	

		void			CalculateTensorsExcitationFrame();
		void			CalculateTensorsLabFrame();

		StatisticalTensor	fTensorsB;
		StatisticalTensor	fTensors;

		double			GetATS(int Ne = 41);	
		void			XSTATIC(int, double, int&, int&, double&, double&, double&);	
		std::array<double,7>	GKK(const int, const int, const double, const double, const double);

		bool 					fTrack;
		double					fEpsilon;
		std::vector< double>			fOmegaTracking;
		std::vector< std::vector < double > >	fStateProbTracking;
		std::vector< std::vector < std::vector < double > > > fStateMultTracking;

};
#endif
