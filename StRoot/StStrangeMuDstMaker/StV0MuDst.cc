/***********************************************************************
 *
 * $Id: StV0MuDst.cc,v 1.1 2000/03/29 03:10:08 genevb Exp $
 *
 * Authors: Gene Van Buren, UCLA, 24-Mar-2000
 *          Peter G. Jones, University of Birmingham, 04-Jun-1999
 *
 ***********************************************************************
 *
 * Description: V0 micro dst class
 *
 ***********************************************************************
 *
 * $Log: StV0MuDst.cc,v $
 * Revision 1.1  2000/03/29 03:10:08  genevb
 * Introduction of Strangeness Micro DST package
 *
 *
 ***********************************************************************/
#include "phys_constants.h"
#include "StV0MuDst.hh"
#include "StTrack.h"
#include "StTrackFitTraits.h"
#include "StV0Vertex.h"
#include "StStrangeEvMuDst.hh"
ClassImp(StV0MuDst)

StV0MuDst::StV0MuDst() { 
}

void StV0MuDst::Fill(StV0Vertex* v0Vertex,
                       StStrangeEvMuDst* event) {
  mEvent = event;
  
  mDecayVertexV0[0] = v0Vertex->position().x();
  mDecayVertexV0[1] = v0Vertex->position().y();
  mDecayVertexV0[2] = v0Vertex->position().z();
  mDcaV0Daughters = v0Vertex->dcaDaughters();
  mDcaV0ToPrimVertex = v0Vertex->dcaParentToPrimaryVertex();
  mDcaPosToPrimVertex = v0Vertex->dcaDaughterToPrimaryVertex(positive);
  mDcaNegToPrimVertex = v0Vertex->dcaDaughterToPrimaryVertex(negative);
  mMomNeg[0] = v0Vertex->momentumOfDaughter(negative).x();
  mMomNeg[1] = v0Vertex->momentumOfDaughter(negative).y();
  mMomNeg[2] = v0Vertex->momentumOfDaughter(negative).z();
  mMomPos[0] = v0Vertex->momentumOfDaughter(positive).x();
  mMomPos[1] = v0Vertex->momentumOfDaughter(positive).y();
  mMomPos[2] = v0Vertex->momentumOfDaughter(positive).z();

  mTpcHitsPos =
    v0Vertex->daughter(positive)->fitTraits().numberOfFitPoints(kTpcId);
  mTpcHitsNeg =
    v0Vertex->daughter(negative)->fitTraits().numberOfFitPoints(kTpcId);
  }

void StV0MuDst::Clear() {
  mEvent = 0;
}

StV0MuDst::~StV0MuDst() {
}

Float_t StV0MuDst::decayLengthV0() {
     if (mEvent)
       return sqrt(pow(mDecayVertexV0[0] - mEvent->primaryVertex()[0],2) +
                   pow(mDecayVertexV0[1] - mEvent->primaryVertex()[1],2) +
                   pow(mDecayVertexV0[2] - mEvent->primaryVertex()[2],2));
     return 0.;
}

Float_t StV0MuDst::Ptot2Pos() {
     return (mMomPos[0]*mMomPos[0] +
	     mMomPos[1]*mMomPos[1] +
	     mMomPos[2]*mMomPos[2]);
}

Float_t StV0MuDst::Ptot2Neg() {
     return (mMomNeg[0]*mMomNeg[0] +
             mMomNeg[1]*mMomNeg[1] +
             mMomNeg[2]*mMomNeg[2]);
}

Float_t StV0MuDst::MomV0(int n) {
     return (mMomPos[n] + mMomNeg[n]);
}

Float_t StV0MuDst::Pt2V0() {
     Float_t mMomV0_0 = MomV0(0);
     Float_t mMomV0_1 = MomV0(1);
     return (mMomV0_0*mMomV0_0 + mMomV0_1*mMomV0_1);
}

Float_t StV0MuDst::Ptot2V0() {
     Float_t mMomV0_2 = MomV0(2);
     return (Pt2V0() + mMomV0_2*mMomV0_2);
}

Float_t StV0MuDst::MomPosAlongV0() {
     Float_t mPtot2V0 = Ptot2V0();
     if (mPtot2V0)
       return (mMomPos[0]*MomV0(0) + 
               mMomPos[1]*MomV0(1) +
               mMomPos[2]*MomV0(2)) / sqrt(mPtot2V0);
     return 0.;
}

Float_t StV0MuDst::MomNegAlongV0() {
     Float_t mPtot2V0 = Ptot2V0();
     if (mPtot2V0)
       return (mMomNeg[0]*MomV0(0) + 
               mMomNeg[1]*MomV0(1) +
               mMomNeg[2]*MomV0(2)) / sqrt(mPtot2V0);
     return 0.;
}

Float_t StV0MuDst::alphaV0() {
  Float_t mMomPosAlongV0 = MomPosAlongV0();
  Float_t mMomNegAlongV0 = MomNegAlongV0();
  return (mMomPosAlongV0-mMomNegAlongV0)/
         (mMomPosAlongV0+mMomNegAlongV0);
}

Float_t StV0MuDst::ptArmV0() {
  Float_t mMomPosAlongV0 = MomPosAlongV0();
  return sqrt(Ptot2Pos() - mMomPosAlongV0*mMomPosAlongV0);
}

Float_t StV0MuDst::eLambda() {
  return sqrt(Ptot2V0()+M_LAMBDA*M_LAMBDA);
}

Float_t StV0MuDst::eK0Short() {
  return sqrt(Ptot2V0()+M_KAON_0_SHORT*M_KAON_0_SHORT);
}

Float_t StV0MuDst::ePosProton() {
  return sqrt(Ptot2Pos()+M_PROTON*M_PROTON);
}

Float_t StV0MuDst::eNegProton() {
  return sqrt(Ptot2Neg()+M_ANTIPROTON*M_ANTIPROTON);
}

Float_t StV0MuDst::ePosPion() {
  return sqrt(Ptot2Pos()+M_PION_PLUS*M_PION_PLUS);
}

Float_t StV0MuDst::eNegPion() {
  return sqrt(Ptot2Neg()+M_PION_MINUS*M_PION_MINUS);
}

Float_t StV0MuDst::massLambda() {
  return sqrt(pow(ePosProton()+eNegPion(),2)-Ptot2V0());
}

Float_t StV0MuDst::massAntiLambda() {
  return sqrt(pow(eNegProton()+ePosPion(),2)-Ptot2V0());
}

Float_t StV0MuDst::massK0Short() {
  return sqrt(pow(ePosPion()+eNegPion(),2)-Ptot2V0());
}

Float_t StV0MuDst::rapLambda() {
  Float_t ela = eLambda();
  Float_t mMomV0_2 = MomV0(2);
  return 0.5*log((ela+mMomV0_2)/(ela-mMomV0_2));
}

Float_t StV0MuDst::rapK0Short() {
  Float_t ek0 = eK0Short();
  Float_t mMomV0_2 = MomV0(2);
  return 0.5*log((ek0+mMomV0_2)/(ek0-mMomV0_2));
}

Float_t StV0MuDst::cTauLambda() {
  return massLambda()*decayLengthV0()/sqrt(Ptot2V0());
}

Float_t StV0MuDst::cTauK0Short() {
  return massK0Short()*decayLengthV0()/sqrt(Ptot2V0());
}

Float_t StV0MuDst::ptPos() {
  return sqrt(Ptot2Pos()-mMomPos[2]*mMomPos[2]);
}

Float_t StV0MuDst::ptotPos() {
  return sqrt(Ptot2Pos());
}

Float_t StV0MuDst::ptNeg() {
  return sqrt(Ptot2Neg()-mMomNeg[2]*mMomNeg[2]);
}

Float_t StV0MuDst::ptotNeg() {
  return sqrt(Ptot2Neg());
}

Float_t StV0MuDst::ptV0() {
  return sqrt(Pt2V0());
}

Float_t StV0MuDst::ptotV0() {
  return sqrt(Ptot2V0());
}
