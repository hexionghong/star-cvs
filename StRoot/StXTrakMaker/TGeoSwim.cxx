
// $Id: TGeoSwim.cxx,v 1.3 2016/06/01 01:05:27 perev Exp $
//
//
// Class StTGeoHelper
// ------------------



#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string>
#include <map>
#include <assert.h>
#include "TString.h"
#include "TMath.h"
#include "TGeoManager.h"
#include "TGeoNavigator.h"
#include "TGeoNode.h"
#include "TGeoVolume.h"
#include "TGeoShape.h"
#include "TGeoBBox.h"
#include "TGeoNode.h"
#include "TGeoMedium.h"
#include "TGeoMaterial.h"
#include "TGeoSwim.h"
#include "THelixTrack.h"
ClassImp(TGeoSwim)
//_____________________________________________________________________________
TGeoSwim::TGeoSwim(const char *name):TNamed(name,"TGeoSwim")
{
  fSmax = 5;
  fRmax = 1000;
  fZmin =-1000;
  fZmax = 1000;
  fHelx[0] = 0;
  fHelx[1] = 0;
  fNode[0] = 0;
  fNode[1] = 0;
  fMag = 0;  
  fLoss= 0;
  fEnd = 0;  
  fInOutLen[0] = 0;  
  fInOutLen[1] = 0;  
  fC  = 0;
  fP  = 0;  			//momentum  loss(GeV) 
  fPt = 0;  			//momentum  loss(GeV) 
  fPLoss = 0;  			//momentum  loss(GeV) 
  fTimeFly = 0;  		//time in seconds 
  memset(fB,0,sizeof(fB));
}
//_____________________________________________________________________________
void TGeoSwim::Set(double Rmax,double Zmin,double zMax,double sMax)
{
  fRmax = Rmax;
  fZmin = Zmin;
  fZmax = zMax;
  fSmax = sMax;
}
//_____________________________________________________________________________
int TGeoSwim::Set(THelixTrack *inHelx,THelixTrack *otHelx)
{
 fHelx[0]=inHelx; fHelx[1]=otHelx;
 gGeoManager->SetCurrentPoint    (fHelx[0]->Pos());
 gGeoManager->SetCurrentDirection(fHelx[0]->Dir());
 fNode[0] = gGeoManager->FindNode();
 if (!fNode[0]) return 1;
 return 0;
}

//_____________________________________________________________________________
int TGeoSwim::Set(const double* pos,const double* dir, double curv)
{
  fPt=0;fP=0;fPLoss=0;fTimeFly=0; memset(fB,0,sizeof(fB));
  fC = curv;
  int ans = Set(new THelixTrack(pos,dir,curv),new THelixTrack()); 
  if (fMag) {
    (*fMag)(pos,fB);
    double Pti = fHelx[0]->GetRho()*fB[2];
    fPt = (fabs(Pti)>1e-6) ? 1./Pti:1E6;
    fP  = fabs(fPt/fHelx[0]->GetCos());
  }
  return ans;
}
//_____________________________________________________________________________
const double *TGeoSwim::GetPos  (int idx) const      {return fHelx[idx]->Pos();}
//_____________________________________________________________________________
const double *TGeoSwim::GetDir  (int idx) const      {return fHelx[idx]->Dir();}
//_____________________________________________________________________________
const char *TGeoSwim::GetPath  () const      {return gGeoManager->GetPath();}
//_____________________________________________________________________________
int TGeoSwim::OutScene(const double *x) const
{
  if (x[2]<fZmin  || x[2] > fZmax) 	return 1;
  if (x[0]*x[0]+x[1]*x[1] > fRmax*fRmax)return 2;
  return 0;
}
//_____________________________________________________________________________
const TGeoMaterial *TGeoSwim::GetMate () const      
{
  if (!fNode[0]) return 0;
  return fNode[0]->GetMedium()->GetMaterial();
}
//_____________________________________________________________________________
//_____________________________________________________________________________
int TGeoSwim::Swim(double maxLenP)
{
enum {kMaxIter=100};
enum {kInside  = 1, kOutside  = 0};

  const TGeoNode **nodes=fNode;
  double   *inout= fInOutLen;
  THelixTrack** th=fHelx;

  const double *poz = fHelx[0]->Pos();
  double range = fabs(poz[0])+fabs(poz[1])+fabs(poz[2]);
  double myMicron = 1e-4+range*1e-4;

  double dP,dC,dPt,pos[3],dir[3],cosTh2,cosTh,lenxy2,lenxy;

  *fHelx[1] = *fHelx[0];
  fNode[0] = gGeoManager->GetCurrentNode();
  double myRad =1./(fabs(fHelx[0]->GetRho())+1e-10)/fHelx[0]->GetCos();
  double maxLen = (maxLenP>3*myRad)? 3*myRad: maxLenP;
  fInOutLen[0]=0; fInOutLen[1]=maxLen;
  double step=0,maxStep=0;
  while(1) {
    const TGeoMaterial *gmate = GetMate();
    double myLen = fInOutLen[0];
    int kase = kInside;  	
    for (int iter=0;iter<kMaxIter; iter++)  {
      switch (kase) {

	case kInside: {		//Inside 
	  if (step>0) {
            fHelx[1]->Move(step); fInOutLen[0]+=step;
            gGeoManager->SetCurrentPoint    (fHelx[1]->Pos());
            gGeoManager->SetCurrentDirection(fHelx[1]->Dir());
            fNode[1] = gGeoManager->FindNode();
          }
          maxStep = fInOutLen[1]-fInOutLen[0];
	  if (maxStep>0.3*myRad) maxStep=0.3*myRad;
          if ( fInOutLen[0]+maxStep > maxLen) maxStep = maxLen-fInOutLen[0];
          if (maxStep<=0) break;
	  gGeoManager->FindNextBoundary(maxStep);
	  step = gGeoManager->GetStep()*0.99;
	  break;}

	case kOutside : {		// outside & quality bad
	  if (fInOutLen[1]>fInOutLen[0]+step){ fInOutLen[1]=fInOutLen[0]+step;}
          if (fInOutLen[1]>maxLen) fInOutLen[1]=maxLen; 
	  if (fInOutLen[0]< fInOutLen[1]/2)  { step = (fInOutLen[1]-fInOutLen[0])*0.9;}
	  else 				   { step = (fInOutLen[1]-fInOutLen[0])*0.5;}
	  break;}

	default: assert(0 || "Wrong case");

      }
      if (maxStep<=0) 			return 1;
      range = fInOutLen[1]-fInOutLen[0];
      if (range<myMicron)	break;
      if (step<myMicron) 	{ 	//if step is tiny try to change upper limit
	step = myMicron;
	if (step> range/2) step = range/2;			
      }
      fHelx[1]->Eval(step,pos,dir); 
      kase = (gGeoManager->IsSameLocation(pos[0],pos[1],pos[2]))? kInside:kOutside;
    }

    range = fInOutLen[1]-fInOutLen[0];
    if (range>myMicron) 		return 13; 	//no convergency at all
    fHelx[1]->Move(range);

    if (OutScene(fHelx[1]->Pos()))	return 2;
    myLen = fInOutLen[1]-myLen;
    if (fLoss) { // Account of energy loss

      dP = (*fLoss)(gmate,fP,myLen,0);
//    ================================

      memcpy(pos,fHelx[1]->Pos(),sizeof(pos));
      memcpy(dir,fHelx[1]->Dir(),sizeof(dir));
      cosTh2 = (1.-dir[2])*(1.+dir[2]);
      cosTh  = sqrt(cosTh2);
      for (int j=0;j<3;j++){ dir[j]/=cosTh;}
      dPt = fPt/fP*dP;
      fP -= dP; fPt-= dPt;
      if (fabs(fPt)<1e-2) { return 1313; }
      fPLoss += dP;
      lenxy2 = cosTh2*myLen*myLen;
      lenxy  = cosTh*myLen;

      if (fMag) { //We have mag field 
	(*fMag)(pos,fB);
//      ===============
        dC  = 1./fPt/fB[2] - fC;
      } else {
	 dC = -fC*dPt/fPt;
      }
      fC -=dC;
      double dPhi = dC*lenxy /2.;
      double dH   = dC*lenxy2/6.;
      pos[0]+=-dir[1]*dH;
      pos[1]+= dir[0]*dH;
      dir[0] += -dir[1]*dPhi;
      dir[1] +=  dir[0]*dPhi;
      fHelx[1]->Set(pos,dir,fC);       
    }
    gGeoManager->SetCurrentPoint    ((double*)fHelx[1]->Pos());
    gGeoManager->SetCurrentDirection((double*)fHelx[1]->Dir());
    fNode[1] = gGeoManager->FindNode();
    if (!fNode[1]) 		return 99;
    if (!fEnd || (*fEnd)())	return 0;

    fNode[0] = fNode[1];
  }
  return 131313;
}
