/*****************************************************************
 *
 * $Id: StTrsDeDx.hh,v 1.3 1999/01/23 18:47:22 fisyak Exp $
 *
 * Author: brian Nov 20, 1997
 *
 *****************************************************************
 * Description:  calculates dE/dx using a given parmeterization
 *               based on CERN/NA49 work.  Cross checks in
 *               progress with GEANT, system test and P10 data
 *               He/C3H8 parameterization coming soon...
 *
 *****************************************************************
 *
 * $Log: StTrsDeDx.hh,v $
 * Revision 1.3  1999/01/23 18:47:22  fisyak
 * Cleanup for SL98l
 *
 * Revision 1.3  1999/01/23 18:47:22  fisyak
 * Cleanup for SL98l
 *
 * Revision 1.2  1999/01/23 05:04:18  lasiuk
 * provide a default constructor
 *
 * Revision 1.1  1998/11/10 17:12:09  fisyak
 * Put Brian trs versin into StRoot
 *
 * Revision 1.9  1998/11/08 17:29:52  lasiuk
 * allocators for SUN
 *
 * Revision 1.8  1998/11/02 22:48:13  lasiuk
 * attachment coefficient as data member
 *
 * Revision 1.7  1998/11/01 17:37:35  lasiuk
 * added diffusion coefficients (need them for transporter)
 *
 * Revision 1.6  1998/10/31 14:12:28  lasiuk
 * add mMeanFreePath data member for nextInteraction member function;
 * return energy in secondary member function;
 * use SystemOfUnits in default padLength;
 * name space considerations
 *
 * Revision 1.5  1998/10/22 14:56:16  lasiuk
 * Incorporate 1/E^n distribution for primaries
 *
 * Revision 1.4  1998/10/22 00:23:21  lasiuk
 * Oct 22
 *
 * Revision 1.3  1998/08/12 17:50:26  lasiuk
 * incorporate units/update Bethe-Bloch
 *
 * Revision 1.2  1998/08/10 15:05:55  lasiuk
 * random generators/engines static
 *
 * Revision 1.1  1998/06/04 23:32:20  lasiuk
 * Initial Revision
 *
 ******************************************************************/
#ifndef __sun
#ifndef ST_TRS_DEDX_HH
#endif
#define ST_TRS_DEDX_HH

#include <string>
#include <vector>

// SCL
#include "SystemOfUnits.h"
#ifndef ST_NO_NAMESPACES
using namespace units;
#endif
#include "Randomize.h"

class StTrsDeDx {
public:
        enum StElectron{
	primaries,
	secondaries,
	total,
    StTrsDeDx(); // should be made private!! Do not use!!
#ifndef __sun
	numberOfElectrons};
#endif
    
public:
    StTrsDeDx(const string&, double = 1.95*centimeter);
    ~StTrsDeDx();

    // access functions
    double  W()         const;
    double  padLength() const;
    
    void   setPadLength(double);

    double transverseDiffusionCoefficient()    const;
    double longitudinalDiffusionCoefficient()  const;
    double attachmentCoefficient()             const;
    
    // member functions
    double nextInteraction()                    const;
    int    primary(double bg=3)                 const;
    int    secondary(double*)                   const;
    double betheBloch(double)                   const;
#ifndef ST_NO_TEMPLATE_DEF_ARGS
    void   electrons(vector<int>&, double bg=3) const;
#else
    void   electrons(vector<int, allocator<int> >&, double bg=3) const;
#endif

    // DIAGNOSTIC

protected:
    StTrsDeDx(); // should be made private!! Do not use!!
#ifndef __sun
    void doInitialization();
#else
    char*  mGas;
#endif

protected:
    string mGas;          // Label
    double mPairs;        // e-ion pairs per unit length
    double mMeanFreePath; // mean free path (1/mPairs)
    double mIonize;       // ionization potential
    double mW;            // ave energy to produce e-ion pair
    double mEndPoint;     // Cut-off energy
    double mExponent;     // 1/E^n energy distribution of primaries
    double mEReduced;     // reduced Exponent (1-mExponent)
    double mEE;           // function of mIonize
    double mDensity;      // density of gas
    double mZa;           // Z/A for gas

    double mSigmaTransverse;   // Transverse diffusion sigma 
    double mSigmaLongitudinal; // Longitudinal diffusion sigma
    double mAttachment;
    
    double mPadLength;    // Sample Length
    
    // Calculated Values
    double mKonstant;
    double mAlfat;

    static HepJamesRandom  mEngine;
    static RandFlat        mFlatDistribution;
    static RandPoisson     mPoissonDistribution;
    static RandExponential mExponentialDistribution;
};

inline double StTrsDeDx::W() const {return mW;}
inline double StTrsDeDx::padLength() const {return mPadLength;}
inline double StTrsDeDx::transverseDiffusionCoefficient() const {return mSigmaTransverse;}
inline double StTrsDeDx::longitudinalDiffusionCoefficient() const {return mSigmaLongitudinal;}
inline double StTrsDeDx::attachmentCoefficient() const {return mAttachment;}
#endif
