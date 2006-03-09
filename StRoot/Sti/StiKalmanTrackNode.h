#ifndef StiKalmanTrackNode_H
#define StiKalmanTrackNode_H 1
#define STI_NODE_DEBUG

#include <Stiostream.h>
#include <stdlib.h>
#include <stdexcept>
#include <math.h>
#include "StiTrackNode.h"
#include "StThreeVector.hh"
#include "StThreeVectorF.hh"
#include "StiKalmanTrackFinderParameters.h"
#include "StiShape.h"
#include "StiPlanarShape.h"
#include "StiCylindricalShape.h"
#include "StiPlacement.h"
#include "StiHit.h"
#include "StiMaterial.h"
class StiDetector;
class StiMaterial;
class Messenger;
class StiElossCalculator;
  
typedef enum {
  kFailed = -1,         // could not find intersection
  kHit,                                
  kEdgePhiPlus, kEdgeZminus, kEdgePhiMinus, kEdgeZplus, 
  kMissPhiPlus, kMissZminus, kMissPhiMinus, kMissZplus
} StiIntersection;
  
class StiNodePars {
public:	
void reset(){memset(this,0,sizeof(StiNodePars));}

  /// sine and cosine of cross angle
  double _cosCA;
  double _sinCA;
  double _x;   
  /// local Y-coordinate of this track (reference plane)           
  double _y; 
  /// local Z-coordinate of this track (reference plane)
  double _z;
  /// (signed curvature)*(local Xc of helix axis - X current point on track)
  double _eta;
  /// signed curvature [sign = sign(-qB)]
  double _curv;  
  /// tangent of the track momentum dip angle
  double _tanl;
};
class StiNodeMtx {
public:	
void reset(){memset(this,0,sizeof(StiNodeMtx));}
  double A[kNPars][kNPars];
};


class StiNodeStat {
public:	
  StiNodeStat(){reset();}
void reset(){memset(this,0,sizeof(StiNodeStat));}
  double dx, dy, dl0, dl;
  double x1,y1,cosCA1,sinCA1;
  double x2,y2,cosCA2,sinCA2;
  double sumSin, sumCos;
};

class StiNodeErrs {
public:	
void reset(){memset(this,0,sizeof(StiNodeErrs));}

union{double A[1];double _cXX;};
  double _cYX,_cYY;                       
  double _cZX,_cZY, _cZZ;                 
  double _cEX,_cEY, _cEZ, _cEE;           
  double _cCX,_cCY, _cCZ, _cCE, _cCC;     
  double _cTX,_cTY, _cTZ, _cTE, _cTC, _cTT;
};  

class StiNodeExt {
public:
void reset(){mPP.reset();mPE.reset();mMtx.reset();}
void unset(){;}
public:
  StiNodePars mPP; 
  StiNodeMtx  mMtx;
  StiNodeErrs mPE;
};





/*! \class StiKalmanTrackNode
  Work class used to handle Kalman filter information while
  constructing track nodes.  A node may or may not own a hit
  depending whether it lies on a measurement layer where a hit
  was found. A node can have 0, 1, or many children. 
  Nodes are nominally sequenced outside-in i.e. with decreasing 
  radius (or independent variable). The order can however be reversed.
  In anycase, the order should always be monotonically increasing 
  or decreasing.
  \author Claude A Pruneau
*/
class StiKalmanTrackNode : public StiTrackNode 
{
friend class StiTrackNodeHelper;
public:
  StiKalmanTrackNode(){_ext=0; reset();}
  StiKalmanTrackNode(const StiKalmanTrackNode &node);
  virtual ~StiKalmanTrackNode(){reduce();_Kount=-1;};
  const StiKalmanTrackNode& operator=(const StiKalmanTrackNode &node);  
  
  double mcs2(double relRadThickness, double beta2, double p2);
  /// Resets the node to a "null" un-used state
  void reset();
  void unset(){reduce();}
  /// Resets errors for refit
  void resetError(double fak=0);
  /// Initialize this node with the given hit information
  void initialize(StiHit*h,double alpha, double eta, double curvature, double tanl);
  
  /// Sets the Kalman state of this node equal to that of the given node. 
  void setState(const StiKalmanTrackNode * node);
  /// Extract state information from this node.
  void get(double& alpha, double& xRef, double x[kNPars], double cc[kNErrs], double& chi2);

  /// Extract state information from this node in Radial representation.
  void getGlobalRadial(double  x[6],double  e[15]);

  /// Extract state information from this node in TPT representation.
  void getGlobalTpt   (float   x[6],float   e[15]);

  /// Get the charge (sign) of the track at this node
  int getCharge() const;
  /// Convenience Method that returns the track momentum at this node
  StThreeVectorF getMomentumF() const;
  /// Convenience Method that returns the track momentum at this node
  /// in global coordinates.
  StThreeVectorF getGlobalMomentumF() const;
  StThreeVector<double> getMomentum() const;
  StThreeVector<double> getGlobalMomentum() const;
  /// Calculates and returns the momentum and error of the track at this node. The momentum is 
  /// in the local reference frame of this node.
  void getMomentum(double p[3], double e[6]=0) const;
  /// Calculates and returns the tangent of the track pitch angle at this node.
  double getCurvature() const;
  void setCurvature(double curvature);
  double getDipAngle() const;
  double getTanL() const;
  /// Calculates and returns the momentum of the track at this node.
  double getP() const;
  /// Calculates and returns the transverse momentum of the track at this node.
  double getPt() const;
    
  double x_g() const;
  double y_g() const;
  double z_g() const;
    
  double getX() const 			{ return mFP._x ;}
  double getY() const 			{ return mFP._y;}  
  double getZ() const 			{ return mFP._z;}
  
  double getEta  () const 		{return mFP._eta;   }
  double getSin  () const 		{return mFP._sinCA;}
  double getCos  () const 		{return mFP._cosCA;}
  double getAlpha() const 		{return _alpha;}
  double getEyy()   const 		{return eyy;}
  double getEzz()   const 		{return ezz;}
  double getCyy()   const 		{return mFE._cYY;}
  double getCzz()   const 		{return mFE._cZZ;}
  double const *getPars()const          {return (&mFP._x);}
  double getDiag(int idx)const          {return mFE.A[(idx*(idx+3))/2];}
  int    getHitCount () const		{return hitCount;}
  int    getNullCount() const       	{return nullCount;}
  int    getContigHitCount () const 	{return contiguousHitCount ;}
  int    getContigNullCount() const 	{return contiguousNullCount;}
  char  &getHitCount () 		{return hitCount;}
  char  &getNullCount()        		{return nullCount;}
  char  &getContigHitCount ()  		{return contiguousHitCount ;}
  char  &getContigNullCount()  		{return contiguousNullCount;}

  static void Break(int kase);
  static void PrintStep();
  StThreeVector<double>getPoint() const;
  StThreeVector<double>getGlobalPoint() const;
  /// Calculates and returns the momentum and error of the track at this node in global coordinates.
  void getGlobalMomentum(double p[3], double e[6]=0) const;
  int  isEnded() const;	
  
  /// Propagates a track encapsulated by the given node "p" to the given detector "tDet".
  int  propagate(StiKalmanTrackNode *p, const StiDetector * tDet, int dir);	//throw (Exception);
  
  /// Propagates a track encapsulated by the given node "p" to the given vertex
  bool propagate(const StiKalmanTrackNode *p, StiHit * vertex, int dir);

  bool propagateToBeam(const StiKalmanTrackNode *p, int dir);
  int  propagateToRadius(StiKalmanTrackNode *pNode, double radius,int dir);

  /// Evaluates, stores and returns the dedx associated with this node.
  /// Possible returned values are:
  /// > 0 : value of dedx
  /// -1  : pathlength was invalid or less than "0"
  /// -2  : no hit is associated with the node.
  /// -3  : invalid eloss data for this node.
  double  evaluateDedx();
  
  int  locate(StiPlacement*place,StiShape*sh);
  int  propagate(double x,int option,int dir);
  void propagateMtx();
  void propagateError();
  int  testError(double *emx,int begend);
  void numeDeriv(double val,int kind,int shape=0,int dir=0);
  int  testDeriv(double *der);
  void propagateMCS(StiKalmanTrackNode * previousNode, const StiDetector * tDet);
  
  /// Extrapolate the track parameters to radial position "x"  and return a point global coordinates along
  /// the track at that point.
  StThreeVector<double> getPointAt(double xk) const;
  
  int nudge(StiHit *hit=0);
  double evaluateChi2(const StiHit *hit); 
  int updateNode(); 
  int rotate(double alpha); 
  double getField()   const;
  int    getHelicity()const;
  double getPhase()   const;
  double getPsi()     const;
  double getWindowY();
  double getWindowZ();
  double pitchAngle() const;
  double crossAngle() const;
  double sinCrossAngle() const;
  double pathlength() const;
  double pathLToNode(const StiKalmanTrackNode * const oNode);
  StThreeVectorD* getLengths(StiKalmanTrackNode *nextNode);

  double length(const StThreeVector<double>& delta, double curv);
  double getDedx() const;
  static double nice(double angle);
  /// Return center of helix circle in global coordinates
  StThreeVector<double> getHelixCenter() const;
  void setHitErrors(double yErr,double zErr);
  void setHitErrors();
  static void   setParameters(StiKalmanTrackFinderParameters *parameters);
  friend ostream& operator<<(ostream& os, const StiKalmanTrackNode& n);

  double getX0() const;
  double getGasX0() const;
  double getDensity() const;
  double getGasDensity() const;

  void   extend();
  void   reduce();
  Int_t  debug() const {return _debug;}
  void   setDebug(Int_t m) {_debug = m;}
  void   PrintpT(Char_t *opt="");
  static void   ResetComment(Char_t *m = "") {comment = m; commentdEdx = "";}
  static const Char_t *Comment() {return comment.Data();}
  /// rotation angle of local coordinates wrt global coordinates
  void static saveStatics(double *sav);
  void static backStatics(double *sav);
  void   print(const char *opt) const;
  static void setErrFactor(double ef)	{fgErrFactor=ef;}
  
 private:   
  static StiNodeExt *nodeExtInstance();
  void getHitErrors(const StiHit *hit,double ss[3]) const;

//  Extended members 
 public:
 
  const StiNodePars &mPP() const                      {return _ext->mPP; } 
        StiNodePars &mPP()       {if (!_ext) extend(); return _ext->mPP; } 
  const StiNodeErrs &mPE() const                      {return _ext->mPE; } 
        StiNodeErrs &mPE()       {if (!_ext) extend(); return _ext->mPE; } 
  const StiNodeMtx &mMtx() const                      {return _ext->mMtx;} 
        StiNodeMtx &mMtx()       {if (!_ext) extend(); return _ext->mMtx;} 

 protected:   

  char _beg[1];  
  double _alpha;
  StiNodePars mFP; 
  /// covariance matrix of the track parameters
  StiNodeErrs mFE;
  float  eyy,ezz;
  char hitCount;
  char nullCount;
  char contiguousHitCount;
  char contiguousNullCount;
  char   _end[1];
  StiNodeExt *_ext;
  static StiKalmanTrackFinderParameters * pars;

//??  static int counter;
//??  static Messenger &  _messenger;

//??  static int   shapeCode;
//??  static const StiDetector * det;
//??  static const StiPlanarShape * planarShape;
//??  static const StiCylindricalShape * cylinderShape;
//??  static StiMaterial * gas;
//??  static StiMaterial * prevGas;
//??  static StiMaterial * mat;
//??  static StiMaterial * prevMat;
  static StiNodeStat  mgP;
//??  static double radThickness, density;
//??  static double gasDensity,matDensity,gasRL,matRL;
  static bool   useCalculatedHitError;
  static double fgErrFactor;
  static int fgRefit;
//  debug variables
  static int    fDerivTestOn;   
  static double fDerivTest[kNPars][kNPars];   
  static int   _debug;
  static TString comment;
  static TString commentdEdx;
public:
  int _Kount;  //for debug only 
};


inline double StiKalmanTrackNode::nice(double angle)
{ 
  if (angle <= -M_PI) angle += 2*M_PI;
  if (angle >   M_PI) angle -= 2*M_PI;
  return angle;
}

inline double StiKalmanTrackNode::getCurvature() const
{
  return mFP._curv;
}

inline double StiKalmanTrackNode::getDipAngle() const
{
  return atan(mFP._tanl);
}

inline StThreeVector<double> StiKalmanTrackNode::getMomentum() const
{
  double pt = getPt();
  return StThreeVector<double>(pt*mFP._cosCA,pt*mFP._sinCA,pt*mFP._tanl);
}

inline StThreeVectorF StiKalmanTrackNode::getMomentumF() const
{
  double pt = getPt();
  return StThreeVectorF(pt*mFP._cosCA,pt*mFP._sinCA,pt*mFP._tanl);
}

inline StThreeVector<double> StiKalmanTrackNode::getGlobalMomentum() const
{
  StThreeVector<double> p = getMomentum();
  p.rotateZ(_alpha);
  return p;
}

inline StThreeVectorF StiKalmanTrackNode::getGlobalMomentumF() const
{
  StThreeVectorF p = getMomentumF();
  p.rotateZ(_alpha);
  return p;
}

inline int StiKalmanTrackNode::getCharge() const
{
  return (pars->field*mFP._curv > 0) ? -1 : 1;
}

inline double StiKalmanTrackNode::getTanL() const
{
  return mFP._tanl;
}

inline int StiKalmanTrackNode::getHelicity()  const
{
  return (mFP._curv < 0) ? -1 : 1;
}


inline double StiKalmanTrackNode::pitchAngle() const
{
  return atan(mFP._tanl);
}

inline double StiKalmanTrackNode::sinCrossAngle() const
{
  return mFP._sinCA;
}

inline double StiKalmanTrackNode::crossAngle() const
{
  return asin(mFP._sinCA);
}

inline void StiKalmanTrackNode::setHitErrors(double ey,double ez)
{
  eyy = ey; ezz = ez;
}

/*! Calculate/return the track transverse momentum
  <p>
  Calculate the track transverse momentum in GeV/c based on this node's track parameters.
  <p>
  The momentum is calculated based on the track curvature held by this node. A minimum
  curvature of 1e-12 is allowed. 
*/
inline double StiKalmanTrackNode::getPt() const
{
  double curvature;
  curvature = fabs(mFP._curv);
  if (pars->field) {
    if (curvature<1e-12) 
      return 0.003e12*fabs(pars->field);
    else
      return 0.00299792458*fabs(pars->field/curvature);
  } else return 1e3;
}

/*! Calculate/return the track momentum
  <p>
  Calculate the track  momentum in GeV/c based on this node's track parameters.
  <p>
  The momentum is calculated based on the track curvature held by this node. A minimum
  curvature of 1e-12 is allowed. 
*/
inline double StiKalmanTrackNode::getP() const
{
  return (getPt()*::sqrt(1.+mFP._tanl*mFP._tanl));
}

inline double StiKalmanTrackNode::mcs2(double relRadThickness, double beta2, double p2)
{
  return 14.1*14.1*relRadThickness/(beta2*p2*1e6);
}

//stl helper functor

struct StiKTNXLessThan
{
    bool operator()(const StiKalmanTrackNode& lhs, const StiKalmanTrackNode& rhs) const;
};

struct StreamX 
{
  void operator()(const StiKalmanTrackNode& node) 
  {
    cout <<node.getX()<<endl;
  }
};

inline StThreeVector<double> StiKalmanTrackNode::getPoint() const
{
  return StThreeVector<double>(mFP._x,mFP._y,mFP._z);
}

inline StThreeVector<double> StiKalmanTrackNode::getGlobalPoint() const
{
  return StThreeVector<double>(cos(_alpha)*mFP._x-sin(_alpha)*mFP._y, sin(_alpha)*mFP._x+cos(_alpha)*mFP._y, mFP._z);
}

inline  double StiKalmanTrackNode::x_g() const
{
  return cos(_alpha)*mFP._x-sin(_alpha)*mFP._y;
}

inline  double StiKalmanTrackNode::y_g() const
{
  return sin(_alpha)*mFP._x+cos(_alpha)*mFP._y;
}

inline  double StiKalmanTrackNode::z_g() const
{
  return mFP._z;
}

///Calculate and returns pathlength within detector volume
///associated with this node. Returns 0 if no detector is 
///associated.
inline double StiKalmanTrackNode::pathlength() const
{
  const StiDetector * det = getDetector();
  if (!det) return 0.; 
  double thickness = det->getShape()->getThickness();
  return (thickness*::sqrt(1.+mFP._tanl*mFP._tanl)) / mFP._cosCA;
}

///Return the radiation length (in cm) of the 
///the detector volume at this node.
inline double StiKalmanTrackNode::getX0() const
{
  const StiDetector * det = getDetector();
  if (!det)
    return 0.;
  return det->getMaterial()->getX0();
}

///Return the radiation length (in cm) of the gas
///surrounding the detector volume at this node.
inline double StiKalmanTrackNode::getGasX0() const
{
  const StiDetector * det = getDetector();
  if (!det)
    return 0.;
  return det->getGas()->getX0();
}

inline double StiKalmanTrackNode::getDensity() const
{
  const StiDetector * det = getDetector();
  if (!det)
    return 0.;
  return det->getMaterial()->getDensity();
}

inline double StiKalmanTrackNode::getGasDensity() const
{
  const StiDetector * det = getDetector();
  if (!det)
    return 0.;
  return det->getGas()->getDensity();
}


inline StThreeVectorD* StiKalmanTrackNode::getLengths(StiKalmanTrackNode* nextNode)
{
  double x1=pathlength()/2.;
  double x3=nextNode->pathlength()/2.;
  double x2=pathLToNode(nextNode);
  if (x2> (x1+x3)) x2=x2-x1-x3;
  else x2=0;

  return new StThreeVectorD(x1/getX0(),
			    x2/getDetector()->getMaterial()->getX0(), 
			    x3/nextNode->getX0());
}

inline double StiKalmanTrackNode::getDedx() const
{

  StiHit *hit = getHit();
  if (!hit) return -1;
  double de=hit->getEloss();
  double dx=pathlength();
  if(dx>0 && de>0) return de/dx;
  return -1;
}

inline void StiKalmanTrackNode::setCurvature(double curvature)
{
  mFP._curv=curvature;
}


#endif

