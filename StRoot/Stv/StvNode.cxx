//StvKalmanTrack.cxx
/*
 * $Id: StvNode.cxx,v 1.7 2011/05/04 18:30:14 perev Exp $
 *
 * /author Victor Perev
 */

#include <Stiostream.h>
#include <math.h>
#include <stdio.h>

#include "Stv/StvNode.h"
#include "Stv/StvHit.h"
#include "TString.h"
#include "StarVMC/GeoTestMaker/StTGeoHelper.h"


//______________________________________________________________________________
void StvNode::reset()
{ 
static int myCount=0;
  memset(mBeg,0,mEnd-mBeg+1);
  mId = ++myCount; 
  mXi2[0] = 3e33;mXi2[1] = 3e33;mXi2[2] = 3e33;
}
//______________________________________________________________________________
void StvNode::SetPre(StvNodePars &par,StvFitErrs &err,int dir) 	
{
  mPP[dir]=par;mPE[dir]=err;
  mFP[dir]=par;mFE[dir]=err;
  mFP[  2]=par;mFE[  2]=err;
}
//______________________________________________________________________________
void StvNode::SetFit(StvNodePars &par,StvFitErrs &err,int dir) 	
{
  mFP[dir]=par;mFE[dir]=err;
  if (dir==2) return;
  mFP[2]=par;mFE[2]=err;
}
//______________________________________________________________________________
StDetectorId StvNode::GetDetId() const
{
  const StHitPlane *hp = GetHitPlane(); if (!hp) return kUnknownId;
  return hp->GetDetId();
} 
//________________________________________________________________________________
double StvNode::GetTime() const
{
  return 0;
} 
//________________________________________________________________________________
void StvNode::Print(const char *opt) const
{
static const char *txt = "XYZAPTCHREL";
static const char *hhh = "xyzre";
  if (!opt || !opt[0]) opt = "_";
  TString myOpt(opt);
  if (myOpt.Contains("_")) myOpt.ReplaceAll("_","2RZErze");
  double val,err[2];
  const StvNodePars &fp= GetFP();
  const StvFitErrs  &fe= GetFE();
  StvHit *hit = GetHit();
  TString ts;
  if (hit) {ts+=(GetXi2()>1e3)? "h":"H";}
  printf("%p(%s)",(void*)this,ts.Data());
  if (myOpt.Contains("2")) printf("\t%s=%g","Xi2",GetXi2());
  for (int i=0;txt[i];i++) {
    err[0]=-999;val=-999;
    if (myOpt.Index(TString(txt[i]))<0) continue;
    if (txt[i]=='R') 		{val = fp.getRxy();}
    else if (txt[i]=='P')	{val = fp.getPt() ;}
    else if (txt[i]=='E')	{err[0] = sqrt(fe.mHH); err[1] = sqrt(fe.mZZ);}
    else if (txt[i]=='L')	{val = GetLen();}
    else 			{val = fp[i]      ;}
    if (abs(val+999)>1e-6) 	{ printf("\t%c=%g",txt[i],val);}
    if (err[0]>-999)  		{ printf("\tHH=%7.2g ZZ=%7.2g",err[0],err[1]);}
  }//end for i

  if (hit) {
    for (int i=0;hit && hhh[i];i++) {
       err[0]=-999;val=-999;
      if (myOpt.Index(TString(hhh[i]))<0) continue;
      if (hhh[i]=='r') 		{val = hit->getRxy();}
      else if (hhh[i]=='e')	{err[0] = sqrt(mHrr[0]); err[1] = sqrt(mHrr[2]);}
      else 			{val = hit->x()[i];} 
      if (abs(val+999)>1e-6) 	{printf("\th%c=%g",hhh[i],val);}
      if (err[0]>-999)  	{ printf("\thh=%7.2g zz=%7.2g",err[0],err[1]);}
    } 
  }
  printf("\n");
  return;
}    
//________________________________________________________________________________
void StvNode::SetDer(const Mtx55D_t &der, int dir)
{
   memcpy(mDer[dir][0],der[0],sizeof(Mtx55D_t));
   mPP[dir].reverse(mDer[1-dir],der);
} 
//________________________________________________________________________________
void StvNode::SetHit(StvHit *hit)
{
   if (mHit) mHit->addTimesUsed(-1);
   mHit = hit;
   if (!mHit) return;
   assert(!mHit->timesUsed());
   mHit->addTimesUsed(1);
} 
//________________________________________________________________________________
void StvNode::UpdateDca()
{ 
  double dL = -( mFP[2]._x*mFP[2]._cosCA+mFP[2]._y*mFP[2]._sinCA);
  if (fabs(dL)<1e-6) return;
  do {
    if (fabs(dL)<1e-1) break;
    TCircle crk(&(mFP[2]._x),&(mFP[2]._cosCA),mFP[2]._curv);
    double zero[2]={0};
    dL = crk.Path(zero);
  } while(0);
  for (int i=0;i<5;i++) {mPP[i].move(dL);} 
}
//________________________________________________________________________________
int StvNode::Check(const char *tit, int dirs) const
{
  if (!tit) tit="";
  int nerr=0,ans;
  for (int k=0;k<5;k++) {
    TString ts;
    if (tit[0]) {ts=tit; ts+="/par["; ts+=k;ts+="]"; }
      int itst = (((1&k)+1)&dirs);
      if (k==4) itst = 1;
      if (!itst) continue;
      ans = mPP[k].check(ts); if (ans) nerr++;
      ans = mPE[k].Check(ts); if (ans) nerr++;
  }
  return nerr;
}

