//StiKalmanTrack.cxx
/*
 * $Id: StiKalmanTrackNode.cxx,v 2.83 2005/06/02 17:27:41 perev Exp $
 *
 * /author Claude Pruneau
 *
 * $Log: StiKalmanTrackNode.cxx,v $
 * Revision 2.83  2005/06/02 17:27:41  perev
 * More weak assert in nudge()
 *
 * Revision 2.82  2005/05/31 16:47:56  perev
 * technical reorganization
 *
 * Revision 2.81  2005/05/12 18:10:04  perev
 * dL/dCurv more accurate
 *
 * Revision 2.80  2005/05/04 19:33:00  perev
 * Supress assert
 *
 * Revision 2.79  2005/04/30 20:45:18  perev
 * Less strong test for assert in propagateError
 *
 * Revision 2.78  2005/04/25 20:20:25  fisyak
 * replace assert by print out
 *
 * Revision 2.77  2005/04/12 14:35:39  fisyak
 * Add print out for dE/dx
 *
 * Revision 2.76  2005/04/11 22:48:30  perev
 * assert removed
 *
 * Revision 2.75  2005/04/11 17:33:55  perev
 * Wrong sorting accounted, check for accuracy inctreased
 *
 * Revision 2.74  2005/04/11 14:32:18  fisyak
 * Use gdrelx from GEANT for dE/dx calculation with accouning density effect
 *
 * Revision 2.73  2005/03/30 21:01:43  perev
 * asserts replaced to prints
 *
 * Revision 2.72  2005/03/28 05:52:40  perev
 * Reorganization of node container
 *
 * Revision 2.71  2005/03/24 19:28:35  perev
 * Switch off DerivTest
 *
 * Revision 2.70  2005/03/24 18:05:07  perev
 * Derivatives and their test fixed to eta==Psi model
 *
 * Revision 2.69  2005/03/19 00:20:33  perev
 * Assert for zero determinant ==> print
 *
 * Revision 2.68  2005/03/18 17:35:38  perev
 * some asserts removed
 *
 * Revision 2.67  2005/03/18 17:13:07  perev
 * assert in rotate fix
 *
 * Revision 2.66  2005/03/17 06:24:52  perev
 * A lot of changes. _eta now is Psi
 *
 * Revision 2.65  2005/02/25 17:05:41  perev
 * Scaling for errors added
 *
 * Revision 2.64  2005/02/19 20:23:37  perev
 * Cleanup
 *
 * Revision 2.63  2005/02/18 19:02:55  fisyak
 * Add debug print out for extendToVertex
 *
 * Revision 2.62  2005/02/17 23:19:02  perev
 * NormalRefangle + Error reseting
 *
 * Revision 2.61  2005/02/17 19:58:06  fisyak
 * Add debug print out flags
 *
 * Revision 2.60  2005/02/16 17:47:16  perev
 * assert in nudge 1==>5
 *
 * Revision 2.59  2005/02/07 18:33:42  fisyak
 * Add VMC dead material
 *
 * Revision 2.58  2005/01/20 16:51:32  perev
 * Remove redundant print
 *
 * Revision 2.57  2005/01/17 01:31:25  perev
 * New parameter model
 *
 * Revision 2.56  2005/01/06 00:59:41  perev
 * Initial errors tuned
 *
 * Revision 2.55  2005/01/04 01:37:47  perev
 * minor bug fix
 *
 * Revision 2.54  2004/12/23 18:15:46  perev
 * Cut for -ve cosCA added
 *
 * Revision 2.53  2004/12/14 17:10:17  perev
 * Propagate for 0 not called
 *
 * Revision 2.52  2004/12/13 22:52:23  perev
 * Off testError
 *
 * Revision 2.51  2004/12/13 20:01:38  perev
 * old version of testError temporary activated
 *
 * Revision 2.50  2004/12/12 01:34:24  perev
 * More smart testError, partial error reset
 *
 * Revision 2.49  2004/12/11 22:17:49  pruneau
 * new eloss calculation
 *
 * Revision 2.48  2004/12/11 04:31:36  perev
 * set of bus fixed
 *
 * Revision 2.47  2004/12/10 15:51:44  fisyak
 * Remove fudge factor from eloss calculation, add more debug printout and tests, reorder calculation of cov. matrix for low triangular form
 *
 * Revision 2.46  2004/12/08 16:56:16  fisyak
 * Fix sign in dE/dx; move from upper to lower triangular matrix convention (StEvent) for px,py,pz
 *
 * Revision 2.45  2004/12/05 00:39:07  fisyak
 * Add test suit for matrix manipulation debugging under overall CPPFLAGS=-DSti_DEBUG
 *
 * Revision 2.44  2004/12/01 14:04:57  pruneau
 * z propagation fix
 *
 * Revision 2.43  2004/11/24 17:59:26  fisyak
 * Set ionization potential for Ar in eloss calculateion instead 5
 *
 * Revision 2.42  2004/11/22 19:43:06  pruneau
 * commented out offending cout statement
 *
 * Revision 2.41  2004/11/22 19:23:20  pruneau
 * minor changes
 *
 * Revision 2.40  2004/11/10 21:46:02  pruneau
 * added extrapolation function; minor change to updateNode function
 *
 * Revision 2.39  2004/11/08 15:32:54  pruneau
 * 3 sets of modifications
 * (1) Changed the StiPlacement class to hold keys to both the radial and angle placement. Propagated the use
 * of those keys in StiSvt StiTpc StiSsd and all relevant Sti classes.
 * (2) Changed the StiKalmanTrackFinder::find(StiTrack*) function's algorithm for the navigation of the
 * detector volumes. The new code uses an iterator to visit all relevant volumes. The code is now more robust and compact
 * as well as much easier to read and maintain.
 * (3) Changed the chi2 calculation in StiKalmanTrack::getChi2 and propagated the effects of this change
 * in both StiTrackingPlots and StiStEventFiller classes.
 *
 * Revision 2.38  2004/10/27 03:25:49  perev
 * Version V3V
 *
 * Revision 2.37  2004/10/26 21:53:23  pruneau
 * No truncation but bad hits dropped
 *
 * Revision 2.36  2004/10/26 06:45:37  perev
 * version V2V
 *
 * Revision 2.35  2004/10/25 14:15:56  pruneau
 * various changes to improve track quality.
 *
 * Revision 2.34  2004/03/24 22:01:07  pruneau
 * Removed calls to center representation and replaced by normal representation
 *
 * Revision 2.33  2004/03/17 21:01:53  andrewar
 * Trapping for negative track error (^2) values _cYY and _cZZ. This should
 * be a temporary fix until the root of the problem is found. Problem seems
 * localized to trackNodes without hits.
 * Also trapping for asin(x), x>1 in ::length; point to point cord length
 * on the helix is greater than twice radius of curvature. This should also be
 * resovled.
 *
 * Revision 2.32  2004/01/30 21:40:21  pruneau
 * some clean up of the infinite checks
 *
 * Revision 2.31  2003/09/02 17:59:41  perev
 * gcc 3.2 updates + WarnOff
 *
 * Revision 2.30  2003/08/13 21:04:21  pruneau
 * transfered relevant tracking pars to detector builders
 *
 * Revision 2.29  2003/08/02 08:23:10  pruneau
 * best performance so far
 *
 * Revision 2.28  2003/07/30 19:18:58  pruneau
 * sigh
 *
 * Revision 2.26  2003/07/15 13:56:19  andrewar
 * Revert to previous version to remove bug.
 *
 * Revision 2.24  2003/05/22 18:42:33  andrewar
 * Changed max eloss correction from 1% to 10%.
 *
 * Revision 2.23  2003/05/09 22:07:57  pruneau
 * Added protection to avoid 90deg tracks and ill defined eloss
 *
 * Revision 2.22  2003/05/09 14:57:20  pruneau
 * Synching
 *
 * Revision 2.21  2003/05/08 18:49:09  pruneau
 * fudge=1
 *
 * Revision 2.20  2003/05/07 03:01:39  pruneau
 * *** empty log message ***
 *
 * Revision 2.19  2003/05/03 14:37:22  pruneau
 * *** empty log message ***
 *
 * Revision 2.18  2003/05/01 20:46:47  pruneau
 * changed error parametrization
 *
 * Revision 2.17  2003/04/22 21:20:17  pruneau
 * Added hit filter
 * Tuning og finder pars
 * Tuning of KalmanTrackNode
 *
 * Revision 2.16  2003/04/17 22:49:36  andrewar
 * Fixed getPhase function to conform to StHelixModel convention.
 *
 * Revision 2.15  2003/03/31 17:18:56  pruneau
 * various
 *
 * Revision 2.14  2003/03/13 21:21:27  pruneau
 * getPhase() fixed. MUST inclde -helicity()*pi/2
 *
 * Revision 2.13  2003/03/13 18:59:13  pruneau
 * various updates
 *
 * Revision 2.12  2003/03/12 17:57:31  pruneau
 * Elss calc updated.
 *
 * Revision 2.11  2003/03/04 21:31:05  pruneau
 * Added getX0() and getGasX0() conveninence methods.
 *
 * Revision 2.10  2003/03/04 18:41:27  pruneau
 * Fixed StiHit to use global coordinates as well as locals.
 * Fixed Logic Bug in StiKalmanTrackFinder
 *
 * Revision 2.9  2003/03/04 15:25:48  andrewar
 * Added several functions for radlength calculation.
 *
 */

#include <Stiostream.h>
#include <stdexcept>
#include <math.h>
#include <stdio.h>
using namespace std;

#include "StiHit.h"
#include "StiDetector.h"
#include "StiPlacement.h"
#include "StiMaterial.h"
#include "StiShape.h"
#include "StiPlanarShape.h"
#include "StiCylindricalShape.h"
#include "StiKalmanTrackNode.h"
#include "StiElossCalculator.h"
#include "StiTrackingParameters.h"
#include "StiKalmanTrackFinderParameters.h"
#include "StiHitErrorCalculator.h"
#include "TString.h"
#include "TRMatrix.h"
#include "TRVector.h"
#define PrP(A)    cout << "\t" << (#A) << " = \t" << ( A )
#define PrPP(A,B) {cout << "=== StiKalmanTrackNode::" << (#A); PrP((B)); cout << endl;}
// Local Track Model
//
// x[0] = y  coordinate
// x[1] = z  position along beam axis
// x[2] = (Psi)
// x[3] = C  (local) curvature of the track
// x[4] = tan(l) 

static const double kMaxEta = 1.;
static const double kMaxCur = 0.2;


StiKalmanTrackFinderParameters * StiKalmanTrackNode::pars = 0;
bool StiKalmanTrackNode::recurse = false;

int    StiKalmanTrackNode::shapeCode = 0;
StiNodeStat StiKalmanTrackNode::mgP;
double StiKalmanTrackNode::density = 0;
double StiKalmanTrackNode::gasDensity= 0;
double StiKalmanTrackNode::matDensity= 0;
double StiKalmanTrackNode::gasRL= 0;
double StiKalmanTrackNode::matRL= 0;
double StiKalmanTrackNode::radThickness= 0;
const StiDetector * StiKalmanTrackNode::det = 0;
const StiPlanarShape *StiKalmanTrackNode::planarShape = 0;
const StiCylindricalShape *StiKalmanTrackNode::cylinderShape = 0;


static const int    idx33[3][3] = {{0,1,3},{1,2,4},{3,4,5}};
static const int    idx55[5][5] = 
  {{0,1,3,6,10},{1,2,4,7,11},{3,4,5, 8,12},{6,7, 8, 9,13},{10,11,12,13,14}};
static const int    idx55tpt[5][5] = 
  {{0,1,2,3, 4},{1,5,6,7, 8},{2,6,9,10,11},{3,7,10,12,13},{ 4, 8,11,13,14}};

static const int    idx66[6][6] =
  {{ 0, 1, 3, 6,10,15},{ 1, 2, 4, 7,11,16},{ 3, 4, 5, 8,12,17}
  ,{ 6, 7, 8, 9,13,18},{10,11,12,13,14,19},{15,16,17,18,19,20}};

StiMaterial * StiKalmanTrackNode::gas = 0;
StiMaterial * StiKalmanTrackNode::prevGas = 0;
StiMaterial * StiKalmanTrackNode::mat = 0;
StiMaterial * StiKalmanTrackNode::prevMat = 0;
bool StiKalmanTrackNode::useCalculatedHitError = true;
#define MESSENGER *(Messenger::instance(MessageType::kNodeMessage))
TString StiKalmanTrackNode::comment("");
TString StiKalmanTrackNode::commentdEdx(""); 
int StiKalmanTrackNode::counter = 0;
//debug vars
//#define STI_ERROR_TEST
//#define STI_DERIV_TEST
#ifdef STI_DERIV_TEST
int    StiKalmanTrackNode::fDerivTestOn=0;   
#endif
#ifndef STI_DERIV_TEST
int    StiKalmanTrackNode::fDerivTestOn=-10;   
#endif

double StiKalmanTrackNode::fDerivTest[kNPars][kNPars];   
int gCurrShape=0;
double StiKalmanTrackNode::fgErrFactor=1;

void StiKalmanTrackNode::Break(int kase)
{
static int myBreak=-2005;
if (kase!=myBreak) return;
  printf("*** Break(%d) ***\n",kase);
}		
/* bit mask for debug printout  
   0   => 1 - covariance and propagate matrices 
   1   => 2 - hit associated with the node
   2   => 4 - test matrix manipulation
   3   => 8 - test locate
 */
int StiKalmanTrackNode::_debug = 0;

//______________________________________________________________________________
void StiKalmanTrackNode::reset()
{ 

  StiTrackNode::reset();
  memset(_beg,0,_end-_beg+1);
  _cosAlpha = 1.;
  resetError();
  hitCount=nullCount=contiguousHitCount=contiguousNullCount = 0;
static int myCount=0;
  _Kount = ++myCount; 
  Break(_Kount);
}
//______________________________________________________________________________
void StiKalmanTrackNode::resetError(double fak)
{ 
static const double DY=0.3,DZ=0.3,DEta=0.03,DRho=0.01,DTan=0.05;

  if (!fak) {
    mFE.reset();
    mFE._cYY=DY*DY;
    mFE._cZZ=DZ*DZ;
    mFE._cEE=DEta*DEta;
    mFE._cCC=DRho*DRho;
    mFE._cTT=DTan*DTan;
  } else {
    for (int i=0;i<kNErrs;i++) mFE.A[i] *=fak;
  }  
  mPE = mFE;
}
//_____________________________________________________________
/// Set the Kalman state of this node to be identical 
/// to that of the given node.
/// This method is useful to initial the state of a node
/// while propagating a track.
//______________________________________________________________________________
void StiKalmanTrackNode::setState(const StiKalmanTrackNode * n)
{
  _state   = n->_state;
  _alpha    = n->_alpha;
  _cosAlpha = n->_cosAlpha;
  _sinAlpha = n->_sinAlpha;
  mFP = n->mFP;
  _refX  = n->_refX;
  _layerAngle  = n->_layerAngle;
  mFE = n->mFE;
  nullCount = n->nullCount;
  contiguousHitCount = n->contiguousHitCount;
  contiguousNullCount = n->contiguousNullCount;
  setChi2(1e62);  
}

/**
   returns the node information
   double& alpha : angle of the local reference frame
   double& xRef  : refence position of this node in the local frame
   double x[6],  : state, for a definition, see the top of this file
   double cc[21] : error matrix of the state "x"
   double& chi2) : chi2 of the track at this node
*/
//______________________________________________________________________________
void StiKalmanTrackNode::get(double& alpha,
			     double& xRef,
			     double  x[kNPars], 
			     double  e[kNErrs], 
			     double& chi2)
{
  alpha = _alpha;
  xRef  = _refX;
  memcpy(x,&mFP,sizeof(mFP));
  memcpy(e,&mFE,sizeof(mFE));
  chi2 = getChi2();
}

/*! Calculate/return track 3-momentum and error.
  <p>
  Calculate the 3-momentum of the track in the local reference frame.
  <P>
    
  <h3>Momentum Representation</h3>
  <TABLE BORDER="0" CELLPADDING="2" CELLSPACING="0" WIDTH="100%">
  <TR>
  <TD WIDTH="10%">p[0]</TD>
  <TD WIDTH="10%">px</TD>
  <TD WIDTH="50%">outward</TD>
  </TR>
  <TR>
  <TD WIDTH="10%">p[1]</TD>
  <TD WIDTH="10%">py</TD>
  <TD WIDTH="50%">along detector plane</TD>
  </TR>
  <TR>
  <TD WIDTH="10%">p[2]</TD>
  <TD WIDTH="10%">pz</TD>
  <TD WIDTH="50%">along beam direction</TD>
  </TR>
  </TABLE>
  <h3>Notes:</h3>
  <ol>
  <li>Throws runtime_error exception if |sin(phi)^2|>1.</li>
  <li>Bypasses error calculation if error array "e" is a null pointer.</li>
  </ol>
*/
//______________________________________________________________________________
void StiKalmanTrackNode::getMomentum(double p[3], double e[6]) const
{	
//	keep in mind that _eta == CA
//	keep in mind that pt == SomeCoef/rho
enum {jX=0,jY,jZ,jE,jC,jT};

  double pt = getPt();
  p[0] = pt*mFP._cosCA;
  p[1] = pt*mFP._sinCA;
  p[2] = pt*mFP._tanl;

// 		if e==0, error calculation is not needed, then return
  if (!e) return;

  double rho = mFP._curv; 
  if (fabs(rho) <1.e-12) rho = 1.e-12;
  double F[3][kNPars]; memset(F,0,sizeof(F));
  double dPtdRho = -fabs(pt/rho);
  F[jX][jE] = pt*mFP._sinCA;
  F[jX][jC] = dPtdRho*mFP._cosCA;
  F[jX][jT] = 0;

  F[jY][jE] =  -pt*mFP._cosCA;
  F[jY][jC] = dPtdRho*mFP._sinCA;
  F[jY][jT] =  0;
  
  F[jZ][jE] =  0;
  F[jZ][jC] = dPtdRho*mFP._tanl;
  F[jZ][jT] = pt;
  
  
  
  memset(e,0,sizeof(*e)*kNPars);
  for (int j1=jE;j1<kNPars;j1++) {
  for (int j2=jE;j2<kNPars;j2++) {
    double cc = mFE.A[idx66[j1][j2]];    
    if(!cc) continue;
    for (int k1=0;k1<= 2;k1++){
    for (int k2=0;k2<=k1;k2++){
      e[idx33[k1][k2]]+= cc*F[k1][j1]*F[k2][j2];
  }}}}    
}
//______________________________________________________________________________
/**
   returns the node information
   double x[6],  : state, for a definition, in radial implementation
                   rad  - radius at start (cm). See also comments
                   phi  - azimuthal angle  (in rad)      
                   z    - z-coord. (cm)                 
                   psi  - azimuthal angle of pT vector (in rads)     
                   tanl - tan(dip) =pz/pt               
                   curv - Track curvature (1/cm) 
   double cc[15] : error matrix of the state "x" rad is fixed
                       code definition adopted here, where:
   PhiPhi;
   ZPhi     ,ZZ;                       
   TanlPhi  ,TanlZ ,TanlTanl,                 
   PsiPhi   ,PsiZ  ,PsiTanl , PsiPsi ,           
   CurvPhi  ,CurvZ ,CurvTanl, CurvPsi, CurvCurv     

*/
//______________________________________________________________________________
void StiKalmanTrackNode::getGlobalRadial(double  x[6],double  e[15])
{
  enum {jRad=0,jPhi,jZ,jTan,jPsi,jCur, kX=0,kY,kZ,kE,kC,kT};
  double alpha,xRef,chi2;
  double xx[kNPars],ee[kNErrs];

  get(alpha,xRef,xx,ee,chi2);
  
  x[jRad] = sqrt(pow(xx[kX],2)+pow(xx[kY],2));
  x[jPhi] = atan2(xx[kY],xx[kX]) + alpha;
  x[jZ  ] = xx[kZ];
  x[jTan] = xx[kT];
  x[jPsi] = xx[kE] + alpha;
  x[jCur] = xx[kC];
  if (!e) return;

  double F[kNErrs][kNErrs]; memset(F,0,sizeof(F));
  F[jPhi][kX] = -1e5;
  F[jPhi][kY] =  1e5;
  if (fabs(xx[kY])>1e-5)  F[jPhi][kX] = -1./(xx[kY]);
  if (fabs(xx[kX])>1e-5)  F[jPhi][kY] =  1./(xx[kX]);
  F[jZ][kZ]   = 1.;
  F[jTan][kT] = 1;
  F[jPsi][kE] = 1;
  F[jCur][kC] = 1;
  memset(e,0,sizeof(*e)*15);
  for (int k1=0;k1<kNPars;k1++) {
  for (int k2=0;k2<kNPars;k2++) {
    double cc = mFE.A[idx66[k1][k2]];    
    for (int j1=jPhi;j1<= 5;j1++){
    for (int j2=jPhi;j2<=j1;j2++){
      e[idx55[j1-1][j2-1]]+= cc*F[j1][k1]*F[j2][k2];
  }}}}    
  
}
//______________________________________________________________________________
/**
   returns the node information in TPT representation
   double x[6],  : state, for a definition, in radial implementation
                   rad  - radius at start (cm). See also comments
                   phi  - azimuthal angle  (in rad)      
                   z    - z-coord. (cm)                 
                   psi  - azimuthal angle of pT vector (in rads)     
                   tanl - tan(dip) =pz/pt               
                   q/pt -  
   double cc[15] : error matrix of the state "x" rad is fixed
                       code definition adopted here, where:

                                                 Units
                       ______|________________|____________
                       phi*R |  0  1  2  3  4 |  deg*cm
                        z0   |  1  5  6  7  8 |    cm
                       tanl  |  2  6  9 10 11 |    1         covar(i)
                        psi  |  3  7 10 12 13 |   deg
                       q/pt  |  4  8 11 13 14 | e*1/(GeV/c)
                       -----------------------------------

                       and where phi  = atan2(y0,x0)*(180 deg/pi)
                                 R    = sqrt(x0*x0 + y0*y0)
                                 q/pt = icharge*invpt; (This is what the 
                                        radius of curvature actually
                                        determines)

*/
//______________________________________________________________________________
void StiKalmanTrackNode::getGlobalTpt(float  x[6],float  e[15])
{
  enum {jRad=0,jPhi,jZ,jTan,jPsi,jCur,jPt=jCur};
static const double DEG = 180./M_PI;
static       double fak[6] = {1,0,1,1,DEG,0};

  double xx[6],ee[15];
  getGlobalRadial(xx,ee);
  double pt = getPt();
  fak[jPhi] = DEG*xx[jRad];
  fak[jPt] = (double(getCharge())/pt)/xx[jCur];

  for (int i=0;i<6;i++) {x[i] = (float)(fak[i]*xx[i]);}
  if (!e) return;

  for (int j1=jPhi;j1<= 5;j1++){
  for (int j2=jPhi;j2<=j1;j2++){
    e[idx55tpt[j1-1][j2-1]] = (float)fak[j1]*fak[j2]*ee[idx55[j1-1][j2-1]];
  }}

}


//______________________________________________________________________________
double StiKalmanTrackNode::getField()  const
{
  return pars->field;
}

//______________________________________________________________________________
double StiKalmanTrackNode::getPhase() const
{
  //! This function translates between ITTF helix parameters and
  //! StHelixModel phi. It is only used to fill StTrackGeometry.
  //! For a StPhysicalHelix, phi must be transformed by -h*pi/2.
  return getPsi()-getHelicity()*M_PI/2;

}
//______________________________________________________________________________
double StiKalmanTrackNode::getPsi() const
{
  return getGlobalMomentum().phi();
}

//______________________________________________________________________________
/// returns momentum and its error matrix 
/// in cartesian coordinates in the _global_
/// ref frame of the experiment
/// p[0] = px
/// p[1] = py
/// p[2] = pz
/// Use lower triangular matrix
/// e[0] = px-px
/// e[1] = px-py
/// e[2] = py-py
/// e[3] = px-pz
/// e[4] = py-pz
/// e[5] = pz-pz

//______________________________________________________________________________
void StiKalmanTrackNode::getGlobalMomentum(double p[3], double e[6]) const
{	
  // first get p & e in the local ref frame
  enum {jXX=0,jXY,jYY};
  
  getMomentum(p,e);
  // now rotate the p & e in the global ref frame
  // for the time being, assume an azimuthal rotation 
  // by alpha is sufficient.
  // transformation matrix - needs to be set
  double px=p[0];
  double py=p[1];
  p[0] = _cosAlpha*px - _sinAlpha*py;
  p[1] = _sinAlpha*px + _cosAlpha*py;
  if (e==0) return;

    // original error matrix

  double cXX = e[jXX];
  double cXY = e[jXY];
  double cYY = e[jYY];
  double cc = _cosAlpha*_cosAlpha;
  double ss = _sinAlpha*_sinAlpha;
  double cs = _cosAlpha*_sinAlpha;
  e[jXX] = cc*cXX -   2.*cs*cXY + ss*cYY;
  e[jYY] = ss*cXX +   2.*cs*cXY + cc*cYY;
  e[jXY] = cs*cXX + (cc-ss)*cXY - cs*cYY;
}


//______________________________________________________________________________
/*! Steering routine that propagates the track encapsulated by the given node "pNode" to the given detector "tDet". 
	<p>
	The propagation involves the following steps.
 <OL>
 <LI>Extrapolation of the existing track to the next layer, by "transporting" the
     track a smaller radius.</LI>
 <LI>Determine if the extrapolation actually intersects an existing volume.</LI>
 <LI>Exit with status code if no intersection is found.</LI>
 <LI>Transport the error matrix to the new radius.</LI>
 <LI>If mcsCalculated==true, proceed to calculate MCS effects on the error matrix.</LI>
 <LI>if elossCalculated==true, proceed to calculate Eloss effects on the track parameters.</LI>
 </OL>
 <p>Currently, propagate can handle kPlanar and kCylindrical geometries only. An exception is thrown if other geometry shape are used.
*/
//______________________________________________________________________________
int StiKalmanTrackNode::propagate(StiKalmanTrackNode *pNode, 
				  const StiDetector * tDet,int dir)
{
static int nCall=0; nCall++;
Break(nCall);
  det = tDet;
  int position = 0;
  setState(pNode);
  setDetector(tDet);
  if (debug()) ResetComment(::Form("%30s ",tDet->getName().c_str()));

//StiPlacement * plaze = pNode->getDetector()->getPlacement();
//double pLayerRadius  = plaze->getLayerRadius ();
//double pNormalRadius = plaze->getNormalRadius();

  StiPlacement * place = tDet->getPlacement();
  double nLayerRadius  = place->getLayerRadius ();
  double nNormalRadius = place->getNormalRadius();

  StiShape * sh = tDet->getShape();
  int shapeCode = sh->getShapeCode();
  _refX = nLayerRadius;
  _layerAngle = place->getLayerAngle();
  double endVal,dAlpha;
  switch (shapeCode) {

  case kPlanar: endVal = nNormalRadius;
    { //flat volume
      dAlpha = place->getNormalRefAngle();
      dAlpha = nice(dAlpha - _alpha);
      // bail out if the rotation fails...
      position = rotate(dAlpha);
      if (position) 			return -10;
    }
    					break;
  case kDisk:  							
  case kCylindrical: endVal = nNormalRadius;
    {
      double xy[4];
      position = cylCross(endVal,&mFP._cosCA,mFP._curv,xy);
      if (position) 			return -11;
      dAlpha = atan2(xy[1],xy[0]);
      position = rotate(dAlpha);
      if (position) 			return -11;
    }
   					break;
  default: assert(0);
  }

  position = propagate(endVal,shapeCode,dir); 
  if (position<0)  return position;

  position = locate(place,sh); 
  if (position>kEdgeZplus || position<0) return position;
  propagateError();
  if (debug() & 8) { PrintpT("E");}

  // Multiple scattering
  if (pars->mcsCalculated && fabs(pars->field)>0 )  propagateMCS(pNode,tDet);
  if (debug() & 8) { PrintpT("M");}
  return position;
}

//______________________________________________________________________________
/*! Propagate the track encapsulated by pNode to the given vertex. Use this node
	to represent the track parameters at the vertex.
  <p>
  This method propagates the track from the given parent node
  "pNode" to the given vertex effectively calculating the
  location (x,y,z) of the track near the given vertex. It use "this" node
 to represent/hold the track parameters at the vertex.
 return true when the propagation is successfull and false otherwise.
<p>
*/
bool StiKalmanTrackNode::propagate(const StiKalmanTrackNode *parentNode, StiHit * vertex,int dir)
{
  setState(parentNode);
  if (debug()) ResetComment(::Form("Vtx:%8.3f %8.3f %8.3f",vertex->x(),vertex->y(),vertex->z()));
  //double locVx = _cosAlpha*vertex->x() + _sinAlpha*vertex->y();
  if (propagate(vertex->x(),kPlanar,dir) < 0)    return false; // track does not reach vertex "plane"
  propagateError();
  if (debug() & 8) { PrintpT("V");}
  setHit(vertex);
  setDetector(0);
  return true;
}

//______________________________________________________________________________
///Propagate track from the given node to the beam line with x==0.
///Set the hit and detector pointers to null to manifest this is an extrapolation
bool StiKalmanTrackNode::propagateToBeam(const StiKalmanTrackNode *parentNode,int dir)
{
  setState(parentNode);
  if (debug()) ResetComment(::Form("%30s ",parentNode->getDetector()->getName().c_str()));
  if (propagate(0., kPlanar,dir) < 0) return false; // track does not reach vertex "plane"
  propagateError();
  if (debug() & 8) { PrintpT("B");}
  setHit(0);
  setDetector(0);
  return true;
}

//______________________________________________________________________________
///Extrapolate the track defined by the given node to the given radius.
///Return a negative value if the operation is impossible.
int StiKalmanTrackNode::propagateToRadius(StiKalmanTrackNode *pNode, double radius,int dir)
{
  int position = 0;
  setState(pNode);
  if (debug()) ResetComment(::Form("%30s ",pNode->getDetector()->getName().c_str()));
  _refX = radius;
  position = propagate(radius,kCylindrical,dir);
  if (position<0) return position;
  propagateError();
  if (debug() & 8) { PrintpT("R");}
  _detector = 0;
  return position;
}


//______________________________________________________________________________
/*! Work method used to perform the tranport of "this" node from 
  its current "_x" position to the given position "xk". 
  Returns -1 if the propagation cannot be carried out, i.e.
  if the track curvature is such it cannot reach the desired 
  location.
  option == 0 Planar
  option == 1 Cylinder
 */
int  StiKalmanTrackNode::propagate(double xk, int option,int dir)
{
  static int nCalls=0;
  nCalls++;
  assert(fDerivTestOn!=-10 || _state==kTNRotEnd ||_state>=kTNReady);
  _state = kTNProBeg;
  numeDeriv(xk,1,option,dir);
  mgP.x1=mFP._x;  mgP.y1=mFP._y; mgP.cosCA1 =mFP._cosCA; mgP.sinCA1 =mFP._sinCA;
  double rho = mFP._curv;
  mgP.x2 = xk;

  mgP.dx=mgP.x2-mgP.x1;  
  double test = (dir)? mgP.dx:-mgP.dx;  
//   	if track is coming back stop tracking
//VP  if (test<0) return -3; //Unfortunatelly correct order not garanteed
//   propagation is not needed, return;
//  if (fabs(mgP.dx) < 1.e-6) { _state = kTNProEnd; return 0;}

  double dsin = mFP._curv*mgP.dx;
  mgP.sinCA2=mgP.sinCA1 + dsin; 
//	Orientation is bad. Fit is non reliable
  if (fabs(mgP.sinCA2)>0.95) return -4;
  mgP.cosCA2   = ::sqrt((1.-mgP.sinCA2)*(1.+mgP.sinCA2));
//	Check what sign of cosCA2 must be
  test = (2*dir-1)*mFP._curv*dsin*mgP.cosCA1;
  if (test<0) mgP.cosCA2 = -mgP.cosCA2;
  if (mgP.cosCA2 <0) {// -ve cos.
//	There is a problem: Track is going outside or wrong order of detectors
//      There is no clear way to distinguish. Use HACK. (VP)
     if (fabs(mgP.sinCA1-mgP.sinCA2)<0.5 && mgP.cosCA1<0) return -5;
     mgP.cosCA2 = -mgP.cosCA2; 
  }
  mgP.sumSin   = mgP.sinCA1+mgP.sinCA2;
  mgP.sumCos   = mgP.cosCA1+mgP.cosCA2;
  mgP.dy = mgP.dx*(mgP.sumSin/mgP.sumCos);
  mgP.y2 = mgP.y1+mgP.dy;
  mgP.dl0 = mgP.cosCA1*mgP.dx+mgP.sinCA1*mgP.dy;
  double sind = mgP.dl0*rho;
  
  if (fabs(dsin) < 0.02 && mgP.cosCA1 >0) { //tiny angle
    mgP.dl = mgP.dl0*(1.+sind*sind/6);
  } else {
    double cosd = mgP.cosCA2*mgP.cosCA1+mgP.sinCA2*mgP.sinCA1;
    mgP.dl = atan2(sind,cosd)/rho;
  }

  mFP._z += mgP.dl*mFP._tanl;
  mFP._y = mgP.y2;
  mFP._eta = nice(mFP._eta+rho*mgP.dl);  					/*VP*/
  // sanity check - to abandon the track
  if (fabs(mFP._y)>200. || fabs(mFP._z)>200. ) return -6;
  mFP._x       = mgP.x2;
  mFP._sinCA   = mgP.sinCA2;
  mFP._cosCA   = mgP.cosCA2;
//??  assert(fabs(mFP._sinCA-sin(mFP._eta))<1e-5);
  assert(fabs(mFP._x )<1.e+10);
  assert(fabs(mFP._y )<1.e+10);
  assert(fabs(mFP._sinCA)<1.0001);
  assert(fabs(mFP._cosCA)<1.0001);
  mPP = mFP;
  _state = kTNProEnd;
  return 0;
}

//______________________________________________________________________________
int StiKalmanTrackNode::nudge(StiHit *hit)
{
  assert(fDerivTestOn!=-10 || _state==kTNProEnd || _state>=kTNReady);
  _state = kTNNudBeg;
  if (!hit) hit = getHit();
  double deltaX = hit->x()-mFP._x;
  if (fabs(deltaX) <1.e-3) 	{_state = kTNNudEnd;return  0;}
  assert(fabs(deltaX) < 3.);
  double deltaS = mFP._curv*(deltaX);
  double sCA2 = mFP._sinCA + deltaS;
  if (fabs(sCA2)>0.95) 		return -7;
  double cCA2= sqrt((1.-sCA2)*(1.+sCA2));
  if (cCA2 <  0.2) 		return -8;
  if (cCA2 >  1.0) 		cCA2 = 1.;
  double deltaY = deltaX*(mFP._sinCA+sCA2)/(mFP._cosCA+cCA2);
  double deltaL = deltaX*mFP._cosCA + deltaY*mFP._sinCA;
  double sind = deltaL*mFP._curv;
  if (fabs(sind)<0.02) { deltaL = deltaL*(1.+sind*sind/6);}
  else                 { deltaL = asin(sind)/mFP._curv;       }

  double deltaZ = mFP._tanl*(deltaL);
  mFP._sinCA    = mgP.sinCA2 = sCA2;
  mFP._cosCA    = mgP.cosCA2 = cCA2;
  mgP.sumSin   = mgP.sinCA1+mgP.sinCA2;
  mgP.sumCos   = mgP.cosCA1+mgP.cosCA2;
  mFP._x   += deltaX;
  mFP._y   += deltaY;
  mFP._z   += deltaZ;
  mFP._eta += deltaL*mFP._curv;
  mgP.dx   += deltaX;
  mgP.dy   += deltaY;
  mgP.dl0  += deltaL;
  mgP.dl   += deltaL;


//??  assert(fabs(mFP._sinCA-sin(mFP._eta))<1e-5);
  assert(fabs(mFP._sinCA) <  1.);
  assert(fabs(mFP._cosCA) <= 1.);
  mPP = mFP;
  _state = kTNNudEnd;
  return 0;
}
//______________________________________________________________________________
/// Make propagation matrix 
/// \note This method must be called ONLY after a call to the propagate method.
void StiKalmanTrackNode::propagateMtx()
{  
//  	fYE == dY/dEta
  double fYE= mgP.dx*(1.+mgP.cosCA1*mgP.cosCA2+mgP.sinCA1*mgP.sinCA2)/(mgP.sumCos*mgP.cosCA2);
//	fEC == dEta/dRho
  double fEC = mgP.dx/mgP.cosCA2;
//	fYC == dY/dRho
  double fYC=(mgP.dy*mgP.sinCA2+mgP.dx*mgP.cosCA2)/mgP.sumCos*fEC;
//	fZE == dZ/dEta
  double dLdEta = mgP.dy/mgP.cosCA2;
  double fZE =  mFP._tanl*dLdEta;

// 	fZC == dZ/dRho
  double dang = mgP.dl*mFP._curv;
  double C2LDX = mgP.dl*mgP.dl*(
               0.5*mgP.sinCA2*pow((1+pow(dang/2,2)*sinX(dang/2)),2) +
                   mgP.cosCA2*dang*sinX(dang));

  double fZC = mFP._tanl*C2LDX/mgP.cosCA2;

//  	fZT == dZ/dTanL; 
  double fZT= mgP.dl; 

  double ca =1, sa=0;
  if (mMtx.A[0][0]) { ca = mMtx.A[0][0]+1.;sa = mMtx.A[0][1];}
  mMtx.reset();
//  X related derivatives
  mMtx.A[0][0] = -1;
  mMtx.A[1][0] = -mgP.sinCA2/mgP.cosCA2; 
  mMtx.A[2][0] = -mFP._tanl /mgP.cosCA2;
  mMtx.A[3][0] = -mFP._curv /mgP.cosCA2;

  mMtx.A[1][3]=fYE; mMtx.A[1][4]=fYC; mMtx.A[2][3]=fZE;
  mMtx.A[2][4]=fZC; mMtx.A[2][5]=fZT; mMtx.A[3][4]=fEC;
  if (sa) {
    double fYX = mMtx.A[1][0]; 
    mMtx.A[1][0] = fYX*ca-sa;
    mMtx.A[1][1] = fYX*sa+ca-1;
  }
}



//______________________________________________________________________________
/// Propagate the track error matrix
/// \note This method must be called ONLY after a call to the propagate method.
void StiKalmanTrackNode::propagateError()
{  
  static int nCall=0; nCall++;
  Break(nCall);
  assert(fDerivTestOn!=-10 || _state==kTNProEnd);
  
  if (debug() & 1) 
    {
      counter++;
      cout << "Prior Error:"
	   << "c00:"<<mFE._cYY<<endl
	   << "c10:"<<mFE._cZY<<" c11:"<<mFE._cZZ<<endl
	   << "c20:"<<mFE._cEY<<" c21:"<<mFE._cEZ<<endl
	   << "c30:"<<mFE._cCY<<" c31:"<<mFE._cCZ<<endl
	   << "c40:"<<mFE._cTY<<" c41:"<<mFE._cTZ<<endl;
    }
  propagateMtx();
  errPropag6(mFE.A,mMtx.A,kNPars);
  int smallErr = !(mFE._cYY>1e-20 && mFE._cZZ>1e-20 && mFE._cEE>1e-20&& mFE._cCC>1.e-30&& mFE._cTT>1.e-20);
  if (smallErr) {
    printf("***SmallErr: cYY=%g cZZ=%g cEE=%g cCC=%g cTT=%g\n"
          ,mFE._cYY,mFE._cZZ,mFE._cEE,mFE._cCC,mFE._cTT);
    assert(mFE._cYY>0 && mFE._cZZ>0 && mFE._cEE>0 && mFE._cCC>0 && mFE._cTT>0);
  }
  assert(fabs(mFE._cXX)<1.e-6);
  assert(mFE._cYY*mFE._cZZ-mFE._cZY*mFE._cZY>0);
  mFE._cXX = mFE._cYX= mFE._cZX = mFE._cEX = mFE._cCX = mFE._cTX = 0;
  
#ifdef Sti_DEBUG
  if (debug() & 4) {
    PrPP(propagateError,C);
    TRMatrix F(kNPars,kNPars,f[0]); PrPP(propagateError,F);
    // C^k-1_k = F_k * C_k-1 * F_kT + Q_k
    C = TRSymMatrix(F,TRArray::kAxSxAT,C); PrPP(propagateError,C);
    TRSymMatrix C1(kNPars,mFE.A);   PrPP(propagateError,C1);
    C1.Verify(C);//,1e-7,2);
  }
#endif
  if (debug() & 1) 
    {
      cout << "Post Error:"
	   << "cYY:"<<mFE._cYY<<endl
	   << "cZY:"<<mFE._cZY<<" cZZ:"<<mFE._cZZ<<endl
	   << "cEY:"<<mFE._cEY<<" cEZ:"<<mFE._cEZ<<endl
	   << "cCY:"<<mFE._cCY<<" cCZ:"<<mFE._cCZ<<endl
	   << "cTY:"<<mFE._cTY<<" cTZ:"<<mFE._cTZ<<endl;
    }
// now set hiterrors
   setHitErrors();

// set state node is ready
  mPE = mFE;
  _state = kTNReady;
}

//______________________________________________________________________________
/*! Calculate the effect of MCS on the track error matrix.
  <p>
  The track is assumed to propagate from (x0,y0,z0) to (mgP.x1,y1,z1). The calculation
  is performed for the given mass hypothesis which given a momentum determines the
  speed "beta" of the particle. The calculation of the average scattering angle
  is delegated to the function "mcs2". The calculation of energy loss is done
  by the function eloss.
 */

/*!Calulates length between center of this node and provided node, which
  is assumed to be on the same helix. Have to use global coords, since 
  nodes may not be in the same detector volume.

  \returns (double) length
*/
//delta(mgP.dx,dy,dz) = here - there
double StiKalmanTrackNode::pathLToNode(const StiKalmanTrackNode * const oNode)
{
  const StThreeVector<double> delta = 
    getGlobalPoint() - oNode->getGlobalPoint();
  double R = getCurvature();
  // s = 2c * asin( t/(2c)); t=::sqrt(mgP.dx^2+dy^2+dz^2)
  return length(delta, R);
}

//______________________________________________________________________________
inline double StiKalmanTrackNode::length(const StThreeVector<double>& delta, double curv)
{
  
  double m = delta.perp();
  double as = 0.5*m*curv;
  double lxy=0;
  if (fabs(as)<0.01) { lxy = m*(1.+as*as/24);}
  else               { lxy = 2.*asin(as)/curv;}
  return sqrt(lxy*lxy+delta.z()*delta.z());
}

#if 0
//______________________________________________________________________________
StThreeVector<double> StiKalmanTrackNode::getPointAt(double xk) const
{
  assert(0);
  double cosCA1, sinCA1, cosCA2, sinCA2, 
  mgP.x1=mFP._x;  mgP.y1=mFP._y; cosCA1=mFP._cosCA; sinCA1=mFP._sinCA;
  mgP.x2=mgP.x1+(xk-mgP.x1);
  mgP.dx=mgP.x2-mgP.x1;
  sinCA2=mFP._curv*mgP.dx- mFP._eta;			/*VP*/
  if (fabs(sinCA2)>1.) throw runtimemP._error("SKTN::getPointAt() -W- fabs(sinCA2)>1.");
  cosCA2=::sqrt(1.- sinCA2*sinCA2);
  double sumSin = sinCA1+sinCA2;
  double yy = mFP._y + mgP.dx*sumSin/(cosCA1+cosCA2);
  double sinCA1plusCA2  = sinCA1*cosCA2 + sinCA2*cosCA1;
  if (sinCA1plusCA2==0) throw runtime_error("SKTN::getPointAt() -W- sinCA1plusCA2==0.");
  return StThreeVector<double>(_cosAlpha*mgP.x2-_sinAlpha*yy, _sinAlpha*mgP.x2+_cosAlpha*yy, mFP._z+mgP.dx*mFP._tanl*sumSin/sinCA1plusCA2);
}
#endif //0
//______________________________________________________________________________
/*! Calculate the increment of chi2 caused by the addition of this node to the track.
  <p>
  Uses the track extrapolation to "_x", and hit position to evaluate and return the 
  increment to the track chi2.
  The chi2 is not stored internally in this node. 
  <p>
  <h3>Notes</h3>
  <ol>
  <li>Use full error matrices.</li>
  <li>Return increment in chi2 implied by the node/hit assocition.</li>
  <li>Throws an exception if numerical problems arise.</li>
  </ol>
*/
double StiKalmanTrackNode::evaluateChi2(const StiHit * hit) 
{
  double r00, r01,r11;
  //If required, recalculate the errors of the detector hits.
  //Do not attempt this calculation for the main vertex.
  if (!hit)throw runtime_error("SKTN::evaluateChi2(const StiHit &) - hit==0");
  double dsin =mFP._curv*(hit->x()-mFP._x);
  if (fabs(mFP._sinCA+dsin)>0.99   )	return 1e41;
  if (fabs(mFP._eta)       >kMaxEta) 	return 1e41;
  if (fabs(mFP._curv)      >kMaxCur)    	return 1e41;

  const StiDetector * detector = hit->detector();
  if (useCalculatedHitError && detector)
    {
      if (eyy <= 0) {
	cout << "eyy " << eyy << " reject" << endl;
	return 1e41;
      }
      r00=mFE._cYY*fgErrFactor+eyy;
      r01=mFE._cZY*fgErrFactor;     r11=mFE._cZZ*fgErrFactor+ezz;
    }
  else
    {
      double ss[3];
      getHitErrors(hit,ss);

      r00=ss[0]+mFE._cYY;
      r01=ss[1]+mFE._cZY;  
      r11=ss[2]+mFE._cZZ;
    }
  TRSymMatrix R(2,
		r00,
		r01, r11);
  double det=r00*r11 - r01*r01;
  //if (mFE._cYY<=0 || mFE._cZZ<=0 || det<=0)
  //  cout << endl << "evalChi2 c00:"<<mFE._cYY<< " c10:"<<mFE._cZY<<" c11:"<<mFE._cZZ<<" det:"<<det<< " eyy:"<<eyy<<" ezz:"<<ezz<<endl;
  if (det<1.e-10) {
    printf("StiKalmanTrackNode::evalChi2 *** zero determinant %g\n",det);
    return 1e60;
  }
  double tmp=r00; r00=r11; r11=tmp; r01=-r01;  
  double dyt=hit->y()-mFP._y;
  double dzt=hit->z()-mFP._z;
  double cc= (dyt*r00*dyt + 2*r01*dyt*dzt + dzt*r11*dzt)/det;
  if (debug() & 4) {
    TRSymMatrix G(R,TRArray::kInverted);
    TRVector r(2,hit->y()-mFP._y,hit->z()-mFP._z);
    Double_t chisq = G.Product(r,TRArray::kATxSxA);
    Double_t diff = chisq - cc;
    Double_t sum  = chisq + cc;
    if (diff > 1e-7 || (sum > 2. && (2 * diff ) / sum > 1e-7)) {
      cout << "Failed:\t" << chisq << "\t" << cc << "\tdiff\t" << diff << endl;
    }
  }
  if (debug() & 8) {comment += Form(" chi2 = %6.2f",cc);}
  return cc;
}
//______________________________________________________________________________
int StiKalmanTrackNode::isEnded() const
{

   if(fabs(mFP._eta )>kMaxEta) return 1;
   if(fabs(mFP._curv)>kMaxCur) return 2;
   return 0;   
}		
		
//______________________________________________________________________________
/*! Calculate the effect of MCS on the track error matrix.
  <p>
  The track is assumed to propagate from (x0,y0,z0) to (x1,y1,z1). The calculation
  is performed for the given mass hypothesis which given a momentum determines the
  speed "beta" of the particle. The calculation of the average scattering angle
  is delegated to the function "mcs2". The calculation of energy loss is done
  by the function eloss.
 */
void StiKalmanTrackNode::propagateMCS(StiKalmanTrackNode * previousNode, const StiDetector * tDet)
{  
  double relRadThickness;
  // Half path length in previous node
  double pL1,pL2,pL3,d1,d2,d3,dxEloss;
#ifdef STI_ERROR_TEST
  testError(mFE.A,0);
#endif // STI_ERROR_TEST
  pL1=previousNode->pathlength()/2.;
  // Half path length in this node
  pL3=pathlength()/2.;
  // Gap path length
  pL2= pathLToNode(previousNode);
  if (pL1<0) pL1=0;
  if (pL2<0) pL2=0;
  if (pL3<0) pL3=0;
  double x0p =-1;
  double x0Gas=-1;
  double x0=-1;
  d1    = previousNode->getDensity();
  x0p   = previousNode->getX0();
  d3    = tDet->getMaterial()->getDensity();
  x0    = tDet->getMaterial()->getX0();


  if (pL2> (pL1+pL3)) 
    {
      pL2=pL2-pL1-pL3;
      if (mgP.dx>0)
				{
					x0Gas = tDet->getGas()->getX0();
					d2    = tDet->getGas()->getDensity();
				}
      else
				{
					x0Gas = previousNode->getGasX0(); 
					d2    = previousNode->getGasDensity();
				}
      relRadThickness = 0.;
      dxEloss = 0;
      if (x0p>0.) 
	{
	  relRadThickness += pL1/x0p;
	  dxEloss += d1*pL1;
	}
      if (x0Gas>0.)
				{
					relRadThickness += pL2/x0Gas;
					dxEloss += d2*pL2;
				}
      if (x0>0.)
				{
					relRadThickness += pL3/x0;
					dxEloss += d3*pL3;
				}
    }
  else 
    {
      relRadThickness = 0.; 
      dxEloss = 0;
      if (x0p>0.) 
				{
					relRadThickness += pL1/x0p;
					dxEloss += d1*pL1;
				}
      if (x0>0.)
				{
					relRadThickness += pL3/x0;
					dxEloss += d3*pL3;
				}
    }
  double pt = getPt();
  double p2=(1.+mFP._tanl*mFP._tanl)*pt*pt;
  double m=pars->massHypothesis;
  double m2=m*m;
  double e2=p2+m2;
  double beta2=p2/e2;
  //cout << " m2:"<<m2<<" p2:"<<p2<<" beta2:"<<beta2;
  double theta2=mcs2(relRadThickness,beta2,p2);
  //cout << " theta2:"<<theta2;
 double rho = mFP._curv, tanl = mFP._tanl; 

 double cos2Li = (1.+ tanl*tanl);  // 1/cos(lamda)**2
 
 mFE._cEE += cos2Li 		*theta2;
 mFE._cCC += tanl*tanl*rho*rho	*theta2;
 mFE._cTC += rho*tanl*cos2Li	*theta2;
 mFE._cTT += cos2Li*cos2Li		*theta2;

#ifdef STI_ERROR_TEST
  testError(mFE.A,1);
#endif // STI_ERROR_TEST
  double dE=0;
  double sign = (mgP.dx>0)? 1:-1;

//  const static double I2Ar = (15.8*18) * (15.8*18) * 1e-18; // GeV**2
  StiElossCalculator * calculator = tDet->getElossCalculator();
  double eloss = calculator->calculate(1.,m, beta2);
  dE = sign*dxEloss*eloss;
  if(!finite(dxEloss) || !finite(beta2) || !finite(m) || m==0 || !finite(eloss) || !finite(mFP._curv) || p2==0 )
    {
      cout << "STKN::propagate() -E- Null or Infinite values detected" << endl
					 << "     beta2 : " << beta2
					 << "   dxEloss : " << dxEloss
					 << "         m : " << m
					 << "     eloss : " << mFP._curv
					 << "        p2 : " << p2
					 << "  Logic error => ABORT" << endl;
      throw logic_error("StiKalmanTrackNode::propagate() -F- Infinite values detected. dxEloss!=finite");
    }
  if (fabs(dE)>0)
    {
      if (debug()) commentdEdx = Form("%6.3g cm %6.3g keV %6.3f GeV ",mgP.dx,1e6*dE,TMath::Sqrt(e2)-m); 
      double correction =1. + ::sqrt(e2)*dE/p2;
      if (correction>1.1) correction = 1.1;
      else if (correction<0.9) correction = 0.9;
      mFP._curv = mFP._curv *correction;
    }
    mPP = mFP; mPE = mFE;

}

//______________________________________________________________________________
/*! Update the track parameters using this node.
  <p>
  This method uses the hit contained by node to update the track 
  parameters contained by this node and thus complete the propagation
  of this track to the location x=_x.
  <p>
  <OL>
  <li>Throw a runtime_error exception if no hit is actually associated with this node.</li>
  <li>Compute the measurement error matrix "r". Invert it.
  <li>Update the measurement matrix "k" and calculate updated curvature, eta, and pitch.
  <li>Update track error matrix.</li>
  </OL>
  <p>
  <h3>Notes</h3>
  <ol>
  <li>Throw logic_error if no hit is associated with this node.</li>
  <li>Throw runtime_error if determinent of "r" matrix is null.
  </ol>
*/
int StiKalmanTrackNode::updateNode() 
{
static int nCall=0; nCall++;
  assert(fDerivTestOn!=-10 || _state>=kTNReady);
  _state = kTNFitBeg;
#ifdef STI_ERROR_TEST
  testError(mFE.A,0);
#endif //STI_ERROR_TEST
  assert(mFE._cXX<1e-8);
  double r00,r01,r11;
  const StiDetector* detector = getDetector();
  double v00 = eyy;
  double v10 =  0.;
  double v11 = ezz;
  if (! (useCalculatedHitError && detector))
    {
      double ss[3];
      getHitErrors(getHit(),ss);
      v00 = ss[0]; v10 = ss[1]; v11 = ss[2];
    }  
  r00=v00+mFE._cYY;
  r01=v10+mFE._cZY;  r11=v11+mFE._cZZ;
#ifdef Sti_DEBUG
  TRSymMatrix V(2,v00,
		  v10, v11);  
  TRSymMatrix R1(2,r00,
		   r01, r11);
  static const TRMatrix H(2,5, 1., 0., 0., 0., 0.,
			       0., 1., 0., 0., 0.);
#endif
  _det=r00*r11 - r01*r01;
  if (!finite(_det) || _det<1.e-10) {
    printf("StiKalmanTrackNode::updateNode *** zero determinant %g\n",_det);
    return -11;
  }
  // inverse matrix
  double tmp=r00; r00=r11/_det; r11=tmp/_det; r01=-r01/_det;
  // update error matrix
  double k00=mFE._cYY*r00+mFE._cZY*r01, k01=mFE._cYY*r01+mFE._cZY*r11;
  double k10=mFE._cZY*r00+mFE._cZZ*r01, k11=mFE._cZY*r01+mFE._cZZ*r11;
  double k20=mFE._cEY*r00+mFE._cEZ*r01, k21=mFE._cEY*r01+mFE._cEZ*r11;
  double k30=mFE._cCY*r00+mFE._cCZ*r01, k31=mFE._cCY*r01+mFE._cCZ*r11;
  double k40=mFE._cTY*r00+mFE._cTZ*r01, k41=mFE._cTY*r01+mFE._cTZ*r11;
  double dyt  = getHit()->y() - mFP._y;
  double dzt  = getHit()->z() - mFP._z;
  double dp3  = k30*dyt + k31*dzt;
  double dp2  = k20*dyt + k21*dzt;
  double dp4  = k40*dyt + k41*dzt;
#ifdef Sti_DEBUG
  double dp0  = k00*dyt + k01*dz;
  double dp1  = k10*dyt + k11*dz;
  if (debug() & 4) {
    PrPP(updateNode,R1);
    PrPP(updateNode,V);
  }
  TRSymMatrix C(kNPars,mFE.A);  
  TRSymMatrix R(H,TRArray::kAxSxAT,C);
  R += V;
  TRSymMatrix G(R,TRArray::kInverted); 
  if (debug() & 4) {
    PrPP(updateNode,C);
    PrPP(updateNode,R);
    PrPP(updateNode,G);
  }
  // K = C * HT * G
  TRMatrix T(C,TRArray::kSxAT,H); 
  TRMatrix K(T,TRArray::kAxS,G);  
  TRMatrix K1(5,2,
	      k00, k01,
	      k10, k11,
	      k20, k21,
	      k30, k31,
	      k40, k41);   
  if (debug() & 4) {
    PrPP(updateNode,T);
    PrPP(updateNode,K1);
    PrPP(updateNode,K);
    K1.Verify(K);
  }
  TRVector dR(2,dyt, dzt);
  TRVector dP1(5, dp0, dp1, dp2, dp3, dp4);
  TRVector dP(K,TRArray::kAxB,dR);
  if (debug() & 4) dP1.Verify(dP);//,1e-7,2);
#endif
  double eta  = nice(mFP._eta + dp2);
  if (fabs(eta)>kMaxEta) return -14;
  double cur  = mFP._curv + dp3;
  if (fabs(cur)>kMaxCur) return -16;
  assert(finite(cur));
  double tanl = mFP._tanl + dp4;
  // Check if any of the quantities required to pursue the update
  // are infinite. If so, it means the tracks cannot be update/propagated
  // any longer and should therefore be abandoned. Just return. This is 
  // not a big but rather a feature of the fact a helicoidal tracks!!!
  if (!finite(mFE._cYY)||!finite(mFE._cZZ)||!finite(k30)||!finite(k31))  return -11;
  // update Kalman state
   double p0 = mFP._y + k00*dyt + k01*dzt;
//VP  mFP._y += k00*dy + k01*dz;
  if (fabs(p0)>200.) 
    {
      cout << "updateNode()[1] -W- _y:"<<mFP._y<<" _z:"<<mFP._z<<endl;
      return -12;
    }
  double p1 = mFP._z + k10*dyt + k11*dzt;
  if (fabs(p1)>200.) 
    {
      cout << "updateNode()[2] -W- _y:"<<mFP._y<<" _z:"<<mFP._z<<endl;
      return -13;
    }
  //mFP._tanl += k40*dyt + k41*dzt;
  double sinCA  =  sin(eta);
  // The following test introduces a track propagation error but happens
  // only when the track should be aborted so we don't care...
  mFP._y  = p0;
  mFP._z  = p1;
  mFP._eta  = eta;
  mFP._curv  = cur;
  mFP._tanl  = tanl;
  mFP._sinCA = sinCA;
  mFP._cosCA = ::sqrt((1.-mFP._sinCA)*(1.+mFP._sinCA)); 
  mFP = mFP;
  // update error matrix
  double c00=mFE._cYY;                       
  double c10=mFE._cZY, c11=mFE._cZZ;                 
  double c20=mFE._cEY, c21=mFE._cEZ;//, c22=mFE._cEE;           
  double c30=mFE._cCY, c31=mFE._cCZ;//, c32=mFE._cCE, c33=mFE._cCC;     
  double c40=mFE._cTY, c41=mFE._cTZ;//, c42=mFE._cTE, c43=mFE._cTC, c44=mFE._cTT;
  mFE._cYY-=k00*c00+k01*c10;
  mFE._cZY-=k10*c00+k11*c10;mFE._cZZ-=k10*c10+k11*c11;
  mFE._cEY-=k20*c00+k21*c10;mFE._cEZ-=k20*c10+k21*c11;mFE._cEE-=k20*c20+k21*c21;
  mFE._cCY-=k30*c00+k31*c10;mFE._cCZ-=k30*c10+k31*c11;mFE._cCE-=k30*c20+k31*c21;mFE._cCC-=k30*c30+k31*c31;
  mFE._cTY-=k40*c00+k41*c10;mFE._cTZ-=k40*c10+k41*c11;mFE._cTE-=k40*c20+k41*c21;mFE._cTC-=k40*c30+k41*c31;mFE._cTT-=k40*c40+k41*c41;

  if (mFE._cYY >= v00 || mFE._cZZ >= v11) {
    printf("StiKalmanTrackNode::updateNode *** _cYY >= v00 || _cZZ >= v11 %g %g %g %g \n"
          ,mFE._cYY,v00,mFE._cZZ,v11);
    return -14;
  }
  if (!(mFE._cYY>0 && mFE._cZZ >0 && mFE._cEE>0 && mFE._cCC>0 && mFE._cTT>0)) {
    printf("StiKalmanTrackNode::updateNode *** negative errors  %g %g %g %g %g\n"
          ,mFE._cYY,mFE._cZZ,mFE._cEE,mFE._cCC,mFE._cTT);
    return -14;
  }
  assert (mFE._cYY*mFE._cZZ-mFE._cZY*mFE._cZY>0);

#ifdef STI_ERROR_TEST
  testError(mFE.A,1);
#endif // STI_ERROR_TEST
#ifdef Sti_DEBUG
  TRSymMatrix W(H,TRArray::kATxSxA,G); 
  TRSymMatrix C0(C);
  C0 -= TRSymMatrix(C,TRArray::kRxSxR,W);
  TRSymMatrix C1(kNPars,mFE.A);  
  if (debug() & 4) {
    PrPP(updateNode,W); 
    PrPP(updateNode,C0);
    PrPP(updateNode,C1);
    C1.Verify(C0);
  }
  //   update of the covariance matrix:
  //    C_k = (I - K_k * H_k) * C^k-1_k * (I - K_k * H_k)T + K_k * V_k * KT_k
  // P* C^k-1_k * PT
  TRMatrix A(K,TRArray::kAxB,H);
  TRMatrix P(TRArray::kUnit,kNPars);
  P -= A;
  TRSymMatrix C2(P,TRArray::kAxSxAT,C); 
  TRSymMatrix Y(K,TRArray::kAxSxAT,V);  
  C2 += Y;  
  if (debug() & 4) {
    PrPP(updateNode,C2); PrPP(updateNode,Y); PrPP(updateNode,C2);
    C2.Verify(C0);
    C2.Verify(C1);
  }
#endif
  if (debug() & 8) {
    Double_t dpTOverpT = 100*TMath::Sqrt(mFE._cCC/(mFP._curv*mFP._curv));
    if (dpTOverpT > 9999.999) dpTOverpT = 9999.999;
    if (debug() & 8) PrintpT("U");
    //    cout << "StiKalmanTrackNode::updateNode pT " << getPt() << "+-" << dpTOverpT << endl;
  } 
  _state = kTNFitEnd;
  return 0; 
}

//______________________________________________________________________________
/*! Rotate this node track representation azymuthally by given angle.
  <p>
  This method rotates by an angle alpha the track representation 
  held by this node. 
  <h3>Notes</h3>
  <ol>
  <li>The rotation is bound between -M_PI and M_PI.</li>
  <li>Throws runtime_error if "(_y-y0)*_curv>=0" in order to avoid math exception.</li>
  <li>Avoid undue rotations as they are CPU intensive...</li>
  </ol>
*/
int StiKalmanTrackNode::rotate (double alpha) //throw ( Exception)
{
  assert(fDerivTestOn!=-10 || _state>=kTNReady);
  mMtx.A[0][0]=0;
  if (fabs(alpha)<1.e-6) return 0;
  _state = kTNRotBeg;
  _alpha += alpha;
  _alpha = nice(_alpha);
    //cout << "    new  _alpha:"<< 180.*_alpha/3.1415927<<endl;

  double xt1=mFP._x; 
  double yt1=mFP._y; 
  mgP.sinCA1 = mFP._sinCA;
  mgP.cosCA1 = mFP._cosCA;
  double ca = cos(alpha);
  double sa = sin(alpha);
  mFP._x = xt1*ca + yt1*sa;
  mFP._y= -xt1*sa + yt1*ca;
  mFP._cosCA =  mgP.cosCA1*ca+mgP.sinCA1*sa;
  mFP._sinCA = -mgP.cosCA1*sa+mgP.sinCA1*ca;
   double nor = 0.5*(mFP._sinCA*mFP._sinCA+mFP._cosCA*mFP._cosCA +1);
  mFP._cosCA /= nor;
  mFP._sinCA /= nor;

  mFP._eta= nice(mFP._eta-alpha); /*VP*/
  mFP._sinCA = sin(mFP._eta);
  mFP._cosCA = cos(mFP._eta);
#ifdef Sti_DEBUG  
  TRSymMatrix C(kNPars,mFE.A);
  if (debug() & 4) {PrPP(rotate,C);}
#endif
//cout << " mFP._sinCA:"<<mFP._sinCA<<endl;
  assert(fabs(mFP._sinCA)<=1.);
  assert(fabs(mFP._cosCA)<=1.);
  memset(mMtx.A,0,sizeof(mMtx));
  mMtx.A[0][0]= ca-1;
  mMtx.A[0][1]= sa;
  mMtx.A[1][0]=-sa;
  mMtx.A[1][1]= ca-1;

  double oldC = _cosAlpha;
  double oldS = _sinAlpha;
 _cosAlpha=oldC*ca - oldS*sa; 
 _sinAlpha=oldC*sa + oldS*ca; 
  _state = kTNRotEnd;
  mPP = mFP;
  return 0;
}


//_____________________________________________________________________________
/// print to the ostream "os" the parameters of this node 
/// and all its children recursively
ostream& operator<<(ostream& os, const StiKalmanTrackNode& n)
{
  const StiDetector *det = n.getDetector();
  if (det) os  <<"Det:"<<n.getDetector()->getName();
  else     os << "Det:UNknown";
  os << " a:" << 180*n._alpha/M_PI<<" degs"
     << " refX:" << n._refX
     << " refAngle:" << n._layerAngle <<endl
     << "\tx:" << n.mFP._x
     << " p0:" << n.mFP._y 
     << " p1:" << n.mFP._z 
     << " p2:" << n.mFP._eta 
      << " p3:" << n.mFP._curv 
     << " p4:" << n.mFP._tanl
     << " c00:" <<n.mFE._cYY<< " c11:"<<n.mFE._cZZ 
     << " pT:" << n.getPt() << endl;
  if (n.debug() & 2) {
    StiHit * hit = n.getHit();
    if (hit) os << "\thas hit with chi2 = " << n.getChi2()
		<< " n:"<<n.hitCount
		<< " null:"<<n.nullCount
		<< endl<<"\t hit:"<<*hit;
  }
  else os << endl;
  return os;
}

//______________________________________________________________________________
double StiKalmanTrackNode::getWindowY()
{	  
  const StiDetector * detector = getDetector();
  const StiTrackingParameters * parameters = detector->getTrackingParameters();
  double searchWindowScale = parameters->getSearchWindowScale();
  double minSearchWindow   = parameters->getMinSearchWindow();
  double maxSearchWindow   = parameters->getMaxSearchWindow();

  const StiHitErrorCalculator * calc = detector->getHitErrorCalculator();
  double myEyy,myEzz;
  calc->calculateError(this,myEyy,myEzz);

 

    double window = searchWindowScale*::sqrt(mFE._cYY+myEyy);
  if (window<minSearchWindow)
    window = minSearchWindow;
  else if (window>maxSearchWindow)
    window = maxSearchWindow;
  return window;
}

//_____________________________________________________________________________
double StiKalmanTrackNode::getWindowZ()
{	 
  const StiDetector * detector = getDetector();
  const StiTrackingParameters * parameters = detector->getTrackingParameters();
  double searchWindowScale = parameters->getSearchWindowScale();
  double minSearchWindow   = parameters->getMinSearchWindow();
  double maxSearchWindow   = parameters->getMaxSearchWindow();

  const StiHitErrorCalculator * calc = detector->getHitErrorCalculator();
  double myEyy,myEzz;
  calc->calculateError(this,myEyy,myEzz);

  
  double window = searchWindowScale*::sqrt(mFE._cZZ+myEzz); 
  if (window<minSearchWindow)
    window = minSearchWindow;
  else if (window>maxSearchWindow)
    window = maxSearchWindow;
  return window;
}

//______________________________________________________________________________
StThreeVector<double> StiKalmanTrackNode::getHelixCenter() const
{
  if (mFP._curv==0) throw logic_error("StiKalmanTrackNode::getHelixCenter() -F- _curv==0 ");
  double xt0 = mFP._x-mFP._sinCA/mFP._curv;   /*VP*/
  double yt0 = mFP._y+mFP._cosCA/(mFP._curv);
  double zt0 = mFP._z+mFP._tanl*asin(mFP._sinCA)/mFP._curv;
  return (StThreeVector<double>(_cosAlpha*xt0-_sinAlpha*yt0,_sinAlpha*xt0+_cosAlpha*yt0,zt0));
}

//______________________________________________________________________________
void StiKalmanTrackNode::setParameters(StiKalmanTrackFinderParameters *parameters)
{
  pars = parameters;
}


//______________________________________________________________________________
int StiKalmanTrackNode::locate(StiPlacement *place,StiShape *sh)
{
  enum {kNStd = 5};
  int position;
  double yOff, yAbsOff, detHW, detHD,edge,innerY, outerY, innerZ, outerZ, zOff, zAbsOff;
  //fast way out for projections going out of fiducial volume
  if (fabs(mFP._z)>200. || fabs(mFP._y)>200. ) position = -1;
  edge  = 2.;
  if (mFP._x<50.)      edge  = 0.3;  
  edge = 0; //VP the meaning of edge is not clear
  Int_t shapeCode  = sh->getShapeCode();
  switch (shapeCode) {
  case kDisk:
  case kCylindrical: // cylinder
    yOff    = nice(_alpha - place->getLayerAngle());
    yAbsOff = fabs(yOff);
    yAbsOff -=kNStd*sqrt((mFE._cXX+mFE._cYY)/(mFP._x*mFP._x+mFP._y*mFP._y));
    if (yAbsOff<0) yAbsOff=0;
    detHW = ((StiCylindricalShape *) sh)->getOpeningAngle()/2.;
    innerY = outerY = detHW;
    break;
  case kPlanar: 
  default:
    yOff = mFP._y - place->getNormalYoffset();
    yAbsOff = fabs(yOff) - kNStd*sqrt(mFE._cYY);
    if (yAbsOff<0) yAbsOff=0;
    detHW = sh->getHalfWidth();
    innerY = detHW - edge;
    //outerY = innerY + 2*edge;
    //outerZ = innerZ + 2*edge;
    outerY = innerY + edge;
    break;
  }
  zOff = mFP._z - place->getZcenter();
  zAbsOff = fabs(zOff);
  detHD = sh->getHalfDepth();
  innerZ = detHD - edge;
  outerZ = innerZ + edge;
  if (yAbsOff<innerY && zAbsOff<innerZ)
    position = kHit; 
  else if (yAbsOff>outerY && (yAbsOff-outerY)>(zAbsOff-outerZ))
    // outside detector to positive or negative y (phi)
    // if the track is essentially tangent to the plane, terminate it.
    if (fabs(mFP._sinCA)>0.9999 || mFP._tanl>57.2)
      return -16;
    else
      position = yOff>0 ? kMissPhiPlus : kMissPhiMinus;
  else if (zAbsOff>outerZ && (zAbsOff-outerZ)>(yAbsOff-outerY))
    // outside detector to positive or negative z (west or east)
    position = zOff>0 ? kMissZplus : kMissZminus;
  else if ((yAbsOff-innerY)>(zAbsOff-innerZ))
    // positive or negative phi edge
    position = yOff>0 ? kEdgePhiPlus : kEdgePhiMinus;
  else
    // positive or negative z edge
    position = zOff>0 ? kEdgeZplus : kEdgeZminus;
  if (debug() & 8) {
    comment += ::Form("R %8.3f y/z %8.3f/%8.3f", 
		      mFP._x, mFP._y, mFP._z);
    if (position>kEdgeZplus || position<0)  
      comment += ::Form(" missed %2d y0/z0 %8.3f/%8.3f dY/dZ %8.3f/%8.3f",
			position, yOff, zOff, detHW, detHD);
  }
  return position;
 }
//______________________________________________________________________________

void StiKalmanTrackNode::initialize(StiHit *h,double alpha, double XcRho, double curvature, double tanl)
{
  //cout << "StiKalmanTrackNode::initialize(...) -I- Started"<<endl;
  reset();
  setHit(h);
  //_refX    = h->detector()->getPlacement()->getNormalRadius();
  _refX    = h->detector()->getPlacement()->getLayerRadius();
  _layerAngle= h->detector()->getPlacement()->getLayerAngle();
  mFP._x       = h->x();
  _alpha   = alpha;
  _cosAlpha = cos(alpha);
  _sinAlpha = sin(alpha);
  mFP._y      = h->y();
  mFP._z      = h->z();
  mFP._eta      = XcRho-mFP._x*curvature;
  mFP._curv      = curvature;
  mFP._tanl      = tanl;
  mFP._sinCA = 999.;
  if (fabs(mFP._eta)>1.)   
      throw runtime_error("SKTN::initialize() - ERROR - fabs(_sinCA)>1.");
  mFP._sinCA   = -mFP._eta;
  mFP._cosCA   = ::sqrt((1.-mFP._sinCA)*(1+mFP._sinCA));
//		Changing eta -sinCA ==>> arcsin(sinCA)
   mFP._eta = asin(mFP._sinCA);
  //cout << "StiKalmanTrackNode::initialize(...) -I- Done"<<endl;
  mPP = mFP;
  setDetector(h->detector());
  setHitErrors();
  _state = kTNInit;
  setChi2(0.1);
}


//______________________________________________________________________________
const StiKalmanTrackNode& StiKalmanTrackNode::operator=(const StiKalmanTrackNode &n)
{
  StiTrackNode::operator=(n);
  memcpy(_beg,n._beg,_end-_beg+1);
  return *this;
}
//______________________________________________________________________________
void StiKalmanTrackNode::setHitErrors()
{
  setHitErrors(0.,0.);
  const StiDetector * detector = getDetector();
  if (!detector) return;
  const StiHitErrorCalculator * calc = detector->getHitErrorCalculator();
  if (!calc) return;
  double myEyy,myEzz;
  calc->calculateError(this,myEyy,myEzz);
  setHitErrors(myEyy,myEzz);
}
//______________________________________________________________________________
void StiKalmanTrackNode::getHitErrors(const StiHit *hit,double ss[3]) const
{
  double syy = hit->syy();
  double syz = hit->syz();
  double szz = hit->szz();
  double sxx = hit->sxx();
  if (sxx < 1e-10) {// no X errors
     ss[0] = syy; ss[1] = syz; ss[2] = szz;}
  else             {//account uncertaincy in X for primary mainly
    double sxy = hit->sxy();
    double sxz = hit->sxy();
    double kY = mFP._sinCA; 
    double kZ = mFP._tanl*sqrt(1.+kY*kY);
    ss[0] = syy + 2.*kY*sxy + kY*kY*sxx;
    ss[1] = syz + kY*sxz    + kZ*sxy + kY*kZ*sxx;
    ss[2] = szz + 2.*kZ*sxz + kZ*kZ*sxx;
  }	
}	


#if 1
//______________________________________________________________________________
int StiKalmanTrackNode::testError(double *emx, int begend)
{
// Test and correct error matrix. Output : number of fixes
// DO NOT IMPROVE weird if() here. This accounts NaN


  static int nCall=0; nCall++;
  static const double dia[6] = { 1000.,1000., 1000.,1000.,1000,1000.};
  static double emxBeg[kNErrs];
//return 0;
  if (!begend) { memcpy(emxBeg,emx,sizeof(emxBeg));}
  int ians=0,j1,j2,jj;
  for (j1=0; j1<5;j1++){
    jj = idx55[j1][j1];
    if (!(emx[jj]>0)) {;
      printf("<StiKalmanTrackNode::testError> Negative diag %g[%d][%d]\n",emx[jj],j1,j1);
      				continue;}
    if (emx[jj]<=10*dia[j1] )	continue;
    assert(finite(emx[jj]));
    printf("<StiKalmanTrackNode::testError> Huge diag %g[%d][%d]\n",emx[jj],j1,j1);
    				continue;
//    ians++; emx[jj]=dia[j1];
//    for (j2=0; j2<5;j2++){if (j1!=j2) emx[idx55[j1][j2]]=0;}
  }
  for (j1=0; j1< 5;j1++){
  for (j2=0; j2<j1;j2++){
    jj = idx55[j1][j2];
    assert(finite(emx[jj]));
    double cormax = emx[idx55[j1][j1]]*emx[idx55[j2][j2]];
    if (emx[jj]*emx[jj]<cormax) continue;
    cormax= sqrt(cormax);
//    ians++;emx[jj]= (emx[jj]<0) ? -cormax:cormax;
  }}
  return ians;
}
#endif//0
//______________________________________________________________________________
void StiKalmanTrackNode::numeDeriv(double val,int kind,int shape,int dir)
{
   double maxStep[kNPars]={0.1,0.1,0.1,0.01,0.001,0.01};
   if (fDerivTestOn<0) return;
   gCurrShape = shape;
   fDerivTestOn=-1;
   double save[20];
   StiKalmanTrackNode myNode;
   double *pars = &myNode.mFP._x;
   int state=0;
   saveStatics(save);
   if (fabs(mFP._curv)> 0.02) goto FAIL;
   int ipar;
   for (ipar=1;ipar<kNPars;ipar++)
   {
     for (int is=-1;is<=1;is+=2) {
       myNode = *this;
       backStatics(save);
       double step = 0.1*sqrt((mFE.A)[idx66[ipar][ipar]]);
       if (step>maxStep[ipar]) step = maxStep[ipar];
//       if (step>0.1*fabs(pars[ipar])) step = 0.1*pars[ipar];
//       if (fabs(step)<1.e-7) step = 1.e-7;
       pars[ipar] +=step*is;
// 		Update sinCA & cosCA       
       myNode.mFP._sinCA = sin(myNode.mFP._eta);
       if (fabs(myNode.mFP._sinCA) > 0.9) goto FAIL;
       myNode.mFP._cosCA = cos(myNode.mFP._eta);
       
       switch (kind) {
	 case 1: //propagate
	   state = myNode.propagate(val,shape,dir); break;
	 case 2: //rotate
	   state = myNode.rotate(val);break;
	 default: assert(0);  
       }  
       if(state  ) goto FAIL;

       for (int jpar=1;jpar<kNPars;jpar++) {
	 if (is<0) {
	   fDerivTest[jpar][ipar]= pars[jpar];
	 } else      {
	   double tmp = fDerivTest[jpar][ipar];
	   fDerivTest[jpar][ipar] = (pars[jpar]-tmp)/(2.*step);
	   if (ipar==jpar) fDerivTest[jpar][ipar]-=1.;
         }
       }
     }
   }
   fDerivTestOn=1;backStatics(save);return;
FAIL: 
   fDerivTestOn=0;backStatics(save);return;
}
//______________________________________________________________________________
int StiKalmanTrackNode::testDeriv(double *der)
{
   if (fDerivTestOn!=1) return 0;
   double *num = fDerivTest[0];
   int nerr = 0;
   for (int i=1;i<kNErrs;i++) {
     int ipar = i/kNPars;
     int jpar = i%kNPars;
     if (ipar==jpar)					continue;
     if (ipar==0)					continue;
     if (jpar==0)					continue;
     double dif = fabs(num[i]-der[i]);
     if (fabs(dif) <= 1.e-5) 				continue;
     if (fabs(dif) <= 0.2*0.5*fabs(num[i]+der[i]))	continue;
     nerr++;
     printf ("***Wrong deriv [%d][%d] = %g %g %g\n",ipar,jpar,num[i],der[i],dif);
  }
  fDerivTestOn=0;
  return nerr;
}
//______________________________________________________________________________
void StiKalmanTrackNode::saveStatics(double *sav)
{  
  sav[ 0]=mgP.x1;
  sav[ 1]=mgP.x2; 
  sav[ 2]=mgP.y1; 
  sav[ 3]=mgP.y2; 
  sav[ 5]=mgP.dx; 
  sav[ 6]=mgP.cosCA1; 
  sav[ 7]=mgP.sinCA1; 
  sav[ 8]=mgP.cosCA2; 
  sav[ 9]=mgP.sinCA2; 
  sav[10]=mgP.sumSin; 
  sav[11]=mgP.sumCos; 
  sav[14]=mgP.dl; 
  sav[15]=mgP.dl0; 
  sav[16]=mgP.dy; 
}  
//______________________________________________________________________________
void StiKalmanTrackNode::backStatics(double *sav)
{  
  mgP.x1=             sav[ 0];
  mgP.x2= 		  sav[ 1]; 
  mgP.y1= 		  sav[ 2]; 
  mgP.y2= 		  sav[ 3]; 
  mgP.dx= 	  sav[ 5]; 
  mgP.cosCA1= 	  sav[ 6]; 
  mgP.sinCA1= 	  sav[ 7]; 
  mgP.cosCA2= 	  sav[ 8]; 
  mgP.sinCA2= 	  sav[ 9]; 
  mgP.sumSin= 	  sav[10]; 
  mgP.sumCos= 	  sav[11]; 
  mgP.dl=             sav[14];
  mgP.dl0=            sav[15];
  mgP.dy=             sav[16];
}
//________________________________________________________________________________
void   StiKalmanTrackNode::PrintpT(Char_t *opt) {
  Double_t dpTOverpT = 100*TMath::Sqrt(mFE._cCC/(mFP._curv*mFP._curv));
  if (dpTOverpT > 9999.9) dpTOverpT = 9999.9;
  comment += ::Form(" %s pT %8.3f+-%6.1f",opt,getPt(),dpTOverpT);
}
//________________________________________________________________________________
void StiKalmanTrackNode::PrintStep() {
  cout << comment << "\t" << commentdEdx << endl;
  ResetComment();
}
//________________________________________________________________________________
void   StiKalmanTrackNode::print(const char *opt) const
{
  const char *txt = "xyzect";
  if (!opt || !opt[0]) opt = txt;
  TString ts;
  if (!isValid()) ts+="*";
  if (  getHit()) {ts+="h";if (getChi2()>1e3) ts+="@";}
  printf("%p(%s)",(void*)this,ts.Data());
  for (int j=0;txt[j];j++) {
    if (!strchr(opt,txt[j])) continue;
    printf("\t%c=%g ",txt[j],(&mFP._x)[j]);
  }
    printf("\n");
}    
    
    
    
    
    



