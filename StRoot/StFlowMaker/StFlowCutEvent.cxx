////////////////////////////////////////////////////////////////////////////
//
// $Id: StFlowCutEvent.cxx,v 1.35 2004/08/24 20:24:32 oldi Exp $
//
// Author: Art Poskanzer and Raimond Snellings, LBNL, Oct 1999
//          MuDst enabled by Kirill Filimonov, LBNL, Jun 2002
//
// Description:  Class for applying event cuts
//
////////////////////////////////////////////////////////////////////////////

#include <Stiostream.h>
#include <stdlib.h>
#include "StFlowCutEvent.h"
#include "StFlowCutTrack.h"
#include "StEvent.h"
#include "StFlowPicoEvent.h"
#include "StEventTypes.h"
#include "PhysicalConstants.h"
#include "SystemOfUnits.h"
#include "StThreeVectorF.hh"
#include "StFlowConstants.h"
#include "StMuDSTMaker/COMMON/StMuEvent.h"
#define PR(x) cout << "##### FlowCutEvent: " << (#x) << " = " << (x) << endl;

ClassImp(StFlowCutEvent)

//-----------------------------------------------------------------------

Int_t    StFlowCutEvent::mCentCuts[2]       = {0, 0};
Int_t    StFlowCutEvent::mMultCuts[2]       = {10, 10000};
Float_t  StFlowCutEvent::mVertexXCuts[2]    = {-1., 1.};
Float_t  StFlowCutEvent::mVertexYCuts[2]    = {-1., 1.};
Float_t  StFlowCutEvent::mVertexZCuts[2]    = {-75., 75.};
UInt_t   StFlowCutEvent::mEventN            = 0;     
UInt_t   StFlowCutEvent::mGoodEventN        = 0;
UInt_t   StFlowCutEvent::mCentCut           = 0;
UInt_t   StFlowCutEvent::mMultCut           = 0;
UInt_t   StFlowCutEvent::mVertexXCut        = 0;
UInt_t   StFlowCutEvent::mVertexYCut        = 0;
UInt_t   StFlowCutEvent::mVertexZCut        = 0;
Float_t  StFlowCutEvent::mEtaSymTpcCuts[2]  = {-3., 3.};
UInt_t   StFlowCutEvent::mEtaSymTpcCutN     = 0;     
Float_t  StFlowCutEvent::mEtaSymFtpcCuts[2] = {-5., 5.};
UInt_t   StFlowCutEvent::mEtaSymFtpcCutN    = 0;     
Float_t  StFlowCutEvent::mTriggerCut        = 0;
UInt_t   StFlowCutEvent::mTriggerCutN       = 0;

//-----------------------------------------------------------------------

StFlowCutEvent::StFlowCutEvent() {
  // To apply event cuts
}

//-----------------------------------------------------------------------

StFlowCutEvent::~StFlowCutEvent() {
}

//-----------------------------------------------------------------------

Bool_t StFlowCutEvent::CheckEvent(StEvent* pEvent) {
  // Returns kTRUE if StEvent survives all the cuts
  
  // Primary vertex
  Long_t nvtx = pEvent->numberOfPrimaryVertices();
  if (nvtx == 0) {
    //      cout << "FlowCutEvent: no Vertex " << endl;
    return kFALSE;
  }
  StPrimaryVertex* pVertex = pEvent->primaryVertex(0);
  if (!pVertex) return kFALSE;

  // Multiplicity
  Long_t mult = pVertex->numberOfDaughters();
  if (mMultCuts[1] > mMultCuts[0] && 
     (mult < mMultCuts[0] || mult >= mMultCuts[1])) {
    mMultCut++;
    return kFALSE;
  }
  
  if (!(pEvent->runInfo()->centerOfMassEnergy() > 60. && pEvent->runInfo()->centerOfMassEnergy() < 65.)) { // not 62 GeV
    // for 62 GeV this is within the triggerID cut
    if (pEvent->l3Trigger() && pEvent->l3Trigger()->l3EventSummary() &&
	!(pEvent->l3Trigger()->l3EventSummary()->unbiasedTrigger())) {
      // cout << "FlowCutEvent: L3 biased trigger event " << endl;
      return kFALSE;
    }
  }

  // update normal event counter
  mEventN++;

  const StThreeVectorF& vertex = pVertex->position();
 
  // Vertex x
  Float_t vertexX = vertex.x();
  if (mVertexXCuts[1] > mVertexXCuts[0] &&
     (vertexX < mVertexXCuts[0] || vertexX >= mVertexXCuts[1])) {
    mVertexXCut++;
    return kFALSE;
  }

  // Vertex y
  Float_t vertexY = vertex.y();
  if (mVertexYCuts[1] > mVertexYCuts[0] &&
     (vertexY < mVertexYCuts[0] || vertexY >= mVertexYCuts[1])) {
    mVertexYCut++;
    return kFALSE;
  }

  // Vertex z
  Float_t vertexZ = vertex.z();
  if (mVertexZCuts[1] > mVertexZCuts[0] &&
     (vertexZ < mVertexZCuts[0] || vertexZ >= mVertexZCuts[1])) {
    mVertexZCut++;
    return kFALSE;
  }

  // Trigger
  Float_t trigger = 0.;

  if (pEvent->runInfo()->centerOfMassEnergy() > 60. && pEvent->runInfo()->centerOfMassEnergy() < 65. ) { // 62 GeV
    Float_t ctbMult = 0.;
    StTriggerDetectorCollection *triggers = pEvent->triggerDetectorCollection();
    if (triggers) {
      StCtbTriggerDetector &CTB = triggers->ctb();
      // get CTB
      for (UInt_t slat = 0; slat < CTB.numberOfSlats(); slat++) {
	for (UInt_t tray = 0; tray < CTB.numberOfTrays(); tray++) {
	  ctbMult += CTB.mips(tray,slat,0);
	}
      }
    }
     
    UInt_t triggerId = 0;
    if (pEvent->triggerIdCollection()->nominal()->isTrigger(35004)) triggerId = 35004;
    else if (pEvent->triggerIdCollection()->nominal()->isTrigger(35007)) triggerId = 35007;
    else if (pEvent->triggerIdCollection()->nominal()->isTrigger(35001)) triggerId = 35001;
    else if (pEvent->triggerIdCollection()->nominal()->isTrigger(35009)) triggerId = 35009;
    
    if (!( (triggerId == 35004 || triggerId == 35007) ||
	  ((triggerId == 35001 || triggerId == 35009) && ctbMult > 15) )) {
      trigger = 10.; // no clue
    } else {
      trigger = 1.; // minbias
    }
  } 
  
  else {
    StL0Trigger* pTrigger = pEvent->l0Trigger();
    
    if (pTrigger) {
      UInt_t triggerWord = pTrigger->triggerWord();
      
      switch (triggerWord) {
      case 4096:  trigger = 1.;  break; // minbias
      case 4352:  trigger = 2.;  break; // central
      case 61952: trigger = 3.;  break; // laser
      default:    trigger = 10.; break; // no clue
      }
    }
  }
  
  if (mTriggerCut && trigger != mTriggerCut) {
    mTriggerCutN++;
    return kFALSE;
  }

  mGoodEventN++;
  return kTRUE;
}

//-----------------------------------------------------------------------

Bool_t StFlowCutEvent::CheckEvent(StFlowPicoEvent* pPicoEvent) {
  // Returns kTRUE if picoevent survives all the cuts
  
  if (!pPicoEvent) return kFALSE;

  // Multiplicity
  Int_t mult = pPicoEvent->OrigMult();
  if (mMultCuts[1] > mMultCuts[0] && 
     (mult < mMultCuts[0] || mult >= mMultCuts[1])) {
    mMultCut++;
    return kFALSE;
  }
   
  // Trigger
  UInt_t triggerWord = pPicoEvent->L0TriggerWord();
  Float_t trigger;

  if (pPicoEvent->CenterOfMassEnergy() > 60. && pPicoEvent->CenterOfMassEnergy() < 65. ) { // 62 GeV
    if (!( (triggerWord == 35004 || triggerWord == 35007) ||
	  ((triggerWord == 35001 || triggerWord == 35009) && pPicoEvent->CTB() > 15) )) {
      trigger = 10.; // no clue
    } else {
      trigger = 1.; // minbias
    }
  } 

  else {
    switch (triggerWord) {
    case 4096:  trigger = 1.;  break; // minbias
    case 4352:  trigger = 2.;  break; // central
    case 61952: trigger = 3.;  break; // laser
    default:    trigger = 10.; break; // no clue
    }    
  }

  if (mTriggerCut && trigger != mTriggerCut) {
    mTriggerCutN++;
    return kFALSE;
  }

  // Centrality
  // Centrality=0 is not retievable
  Int_t cent = pPicoEvent->CalcCentrality();
  if (mCentCuts[0] && mCentCuts[1] >= mCentCuts[0] && 
      (cent < mCentCuts[0] || cent > mCentCuts[1])) {
    mCentCut++;
    return kFALSE;
  }
  
  // update normal event counter
  mEventN++;

  // Vertex x
  Float_t vertexX = pPicoEvent->VertexX();
  if (mVertexXCuts[1] > mVertexXCuts[0] &&
     (vertexX < mVertexXCuts[0] || vertexX >= mVertexXCuts[1])) {
    mVertexXCut++;
    return kFALSE;
  }

  // Vertex y
  Float_t vertexY = pPicoEvent->VertexY();
  if (mVertexYCuts[1] > mVertexYCuts[0] &&
     (vertexY < mVertexYCuts[0] || vertexY >= mVertexYCuts[1])) {
    mVertexYCut++;
    return kFALSE;
  }

  // Vertex z
  Float_t vertexZ = pPicoEvent->VertexZ();
  if (mVertexZCuts[1] > mVertexZCuts[0] &&
     (vertexZ < mVertexZCuts[0] || vertexZ >= mVertexZCuts[1])) {
    mVertexZCut++;
    return kFALSE;
  }

  mGoodEventN++;
  return kTRUE;
}

//-----------------------------------------------------------------------

Bool_t StFlowCutEvent::CheckEvent(StMuEvent* pMuEvent) {
  // Returns kTRUE if muevent survives all the cuts
  
  if (!pMuEvent) return kFALSE;

  // Primary vertex
  // The following lines were introduced to get rid of events without a primary vertex.
  // These events are possible since the FTPC uses (0., 0., 0.) as a nominal vertex position
  // if no vertex is found. (This was requested by UPC.) If in such an event tracks are found 
  // they'll make it into the MuDst. Unfortunately a proper scheme to exclude those events is missing. 
  // By cutting on the vertex position we eliminated those events. 
  // THIS WILL REMOVE SIMPLE SIMULATED EVENTS AS WELL!
  if (TMath::Abs(pMuEvent->primaryVertexPosition().x()) < 1.e-5 &&
      TMath::Abs(pMuEvent->primaryVertexPosition().y()) < 1.e-5 &&
      TMath::Abs(pMuEvent->primaryVertexPosition().z()) < 1.e-5) {
    // cout << "FlowCutEvent: no Vertex " << endl;
    return kFALSE;
  }
  
  // Multiplicity 
  Int_t mult = pMuEvent->eventSummary().numberOfGoodPrimaryTracks(); //???
  if (mMultCuts[1] > mMultCuts[0] && 
      (mult < mMultCuts[0] || mult >= mMultCuts[1])) {
    mMultCut++;
    return kFALSE;
  }
   
  // Trigger
  Float_t trigger;
  if (pMuEvent->runInfo().centerOfMassEnergy() > 60. && pMuEvent->runInfo().centerOfMassEnergy() < 65. ) { // 62 GeV

    UInt_t triggerId = 0;
    if (pMuEvent->triggerIdCollection().nominal().isTrigger(35004)) triggerId = 35004;
    else if (pMuEvent->triggerIdCollection().nominal().isTrigger(35007)) triggerId = 35007;
    else if (pMuEvent->triggerIdCollection().nominal().isTrigger(35001)) triggerId = 35001;
    else if (pMuEvent->triggerIdCollection().nominal().isTrigger(35009)) triggerId = 35009;

    if (!( (triggerId == 35004 || triggerId == 35007) ||
	  ((triggerId == 35001 || triggerId == 35009) && pMuEvent->ctbMultiplicity() > 15) )) {
      trigger = 10.; // no clue
    } else {
      trigger = 1.; // minbias
    }
  }

  else {
    if (!pMuEvent->l3EventSummary().unbiasedTrigger()) {
      // cout << "FlowCutEvent: L3 biased trigger event " << endl;
      return kFALSE;
    }

    UInt_t triggerWord = pMuEvent->l0Trigger().triggerWord();
    
    switch (triggerWord) {
    case 4096:  trigger = 1.;  break; // minbias
    case 4352:  trigger = 2.;  break; // central
    case 61952: trigger = 3.;  break; // laser
    default:    trigger = 10.; break; // no clue
    }
  }

  if (mTriggerCut && trigger != mTriggerCut) {
    mTriggerCutN++;
    return kFALSE;
  }

  // Centrality
  // Centrality=0 is not retrievable
  Int_t* cent = 0;
  Int_t centrality = 0;

  if (pMuEvent->runInfo().centerOfMassEnergy() >= 199.) {
    if (fabs(pMuEvent->magneticField()) >= 4.) { // year=2, Au+Au, Full Field
      cent = Flow::cent200Full;
    } else { // year=2, Au+Au, Half Field
      cent = Flow::cent200Half;
    }
  } else if (pMuEvent->runInfo().centerOfMassEnergy() <= 25.){ // year=2, 22 GeV
    cent = Flow::cent22;
  } else if(pMuEvent->runInfo().centerOfMassEnergy() > 60. && pMuEvent->runInfo().centerOfMassEnergy() < 65) { //62 GeV
    cent = Flow::cent62;
  }

  Int_t tracks =  pMuEvent->refMultNeg() + pMuEvent->refMultPos();

  if      (tracks < cent[0])  { centrality = 0; }
  else if (tracks < cent[1])  { centrality = 1; }
  else if (tracks < cent[2])  { centrality = 2; }
  else if (tracks < cent[3])  { centrality = 3; }
  else if (tracks < cent[4])  { centrality = 4; }
  else if (tracks < cent[5])  { centrality = 5; }
  else if (tracks < cent[6])  { centrality = 6; }
  else if (tracks < cent[7])  { centrality = 7; }
  else if (tracks < cent[8])  { centrality = 8; }
  else                        { centrality = 9; }

  if (mCentCuts[0] && mCentCuts[1] >= mCentCuts[0] && 
      (centrality < mCentCuts[0] || centrality > mCentCuts[1])) {
    mCentCut++;
    return kFALSE;
  }
  
  // update normal event counter
  mEventN++;

  // Vertex x
  Float_t vertexX = pMuEvent->primaryVertexPosition().x();
  if (mVertexXCuts[1] > mVertexXCuts[0] &&
     (vertexX < mVertexXCuts[0] || vertexX >= mVertexXCuts[1])) {
    mVertexXCut++;
    return kFALSE;
  }

  // Vertex y
  Float_t vertexY = pMuEvent->primaryVertexPosition().y();
  if (mVertexYCuts[1] > mVertexYCuts[0] &&
     (vertexY < mVertexYCuts[0] || vertexY >= mVertexYCuts[1])) {
    mVertexYCut++;
    return kFALSE;
  }

  // Vertex z
  Float_t vertexZ = pMuEvent->primaryVertexPosition().z();
  if (mVertexZCuts[1] > mVertexZCuts[0] &&
     (vertexZ < mVertexZCuts[0] || vertexZ >= mVertexZCuts[1])) {
    mVertexZCut++;
    return kFALSE;
  }

  mGoodEventN++;
  return kTRUE;

}
  
//-----------------------------------------------------------------------

Bool_t StFlowCutEvent::CheckEtaSymmetry(StEvent* pEvent) {
  // Returns kTRUE if StEvent survives this Eta symmetry cut
  // Call at the end of the event after doing CheckTrack for each track
  // If kFALSE you should delete the last event

  if (((StFlowCutTrack::EtaSymPosTpc() == 0 || StFlowCutTrack::EtaSymNegTpc() == 0) &&     // at least one half is empty
       !(StFlowCutTrack::EtaSymPosTpc() == 0 &&  StFlowCutTrack::EtaSymNegTpc() == 0)) ||  // but not both halves 
      ((StFlowCutTrack::EtaSymPosFtpc() == 0 || StFlowCutTrack::EtaSymNegFtpc() == 0) &&   // at least one FTPC is empty
       !(StFlowCutTrack::EtaSymPosFtpc() == 0 && StFlowCutTrack::EtaSymNegFtpc() == 0))) { // but not both FTPCs 
      // This looks ugly because there is no XOR and events w/o the FTPC or TPC switched on will be cut, otherwise.
    return kFALSE; // possible beam gas event
  }

  // Tpc
  float etaSymPosTpcN = (float)StFlowCutTrack::EtaSymPosTpc();
  float etaSymNegTpcN = (float)StFlowCutTrack::EtaSymNegTpc();
  float etaSymTpc = (etaSymPosTpcN - etaSymNegTpcN) / (etaSymPosTpcN + etaSymNegTpcN);
  // Ftpc
  float etaSymPosFtpcN = (float)StFlowCutTrack::EtaSymPosFtpc();
  float etaSymNegFtpcN = (float)StFlowCutTrack::EtaSymNegFtpc();
  float etaSymFtpc = (etaSymPosFtpcN - etaSymNegFtpcN) / (etaSymPosFtpcN + etaSymNegFtpcN);
  StFlowCutTrack::EtaSymClear();

  StPrimaryVertex* pVertex = pEvent->primaryVertex(0);
  if (!pVertex) return kFALSE;
  const StThreeVectorF& vertex = pVertex->position();
  Float_t vertexZ = vertex.z();
  // Tpc
  float etaSymZSlopeTpc = 0.003;
  etaSymTpc += (etaSymZSlopeTpc * vertexZ); // correction for acceptance
  etaSymTpc *= ::sqrt((double)(etaSymPosTpcN + etaSymNegTpcN)); // corrected for statistics
  // Ftpc
  //float etaSymZSlopeFtpc = 0.003;  // Has to be evaluated, still, therefore ...
  //etaSymFtpc += (etaSymZSlopeFtpc * vertexZ); // ... NOT correctly corrected for acceptance
  etaSymFtpc *= ::sqrt((double)(etaSymPosFtpcN + etaSymNegFtpcN)); // corrected for statistics

  if (mEtaSymTpcCuts[1] > mEtaSymTpcCuts[0] && 
      (etaSymTpc < mEtaSymTpcCuts[0] || etaSymTpc >= mEtaSymTpcCuts[1])) {
    mEtaSymTpcCutN++;
    mGoodEventN--;
    return kFALSE;
  }

  else if (mEtaSymFtpcCuts[1] > mEtaSymFtpcCuts[0] && 
      (etaSymFtpc < mEtaSymFtpcCuts[0] || etaSymFtpc >= mEtaSymFtpcCuts[1])) {
    mEtaSymFtpcCutN++;
    mGoodEventN--;
    return kFALSE;
  }

  return kTRUE;
}

//-----------------------------------------------------------------------

Bool_t StFlowCutEvent::CheckEtaSymmetry(StFlowPicoEvent* pPicoEvent) {
  // Returns kTRUE if picoevent survives this Eta symmetry cut
  // Call at the end of the event after doing CheckTrack for each track
  // If kFALSE you should delete the last event

  if (((StFlowCutTrack::EtaSymPosTpc() == 0 || StFlowCutTrack::EtaSymNegTpc() == 0) &&     // at least one half is empty
       !(StFlowCutTrack::EtaSymPosTpc() == 0 &&  StFlowCutTrack::EtaSymNegTpc() == 0)) ||  // but not both halves 
      ((StFlowCutTrack::EtaSymPosFtpc() == 0 || StFlowCutTrack::EtaSymNegFtpc() == 0) &&   // at least one FTPC is empty
       !(StFlowCutTrack::EtaSymPosFtpc() == 0 && StFlowCutTrack::EtaSymNegFtpc() == 0))) { // but not both FTPCs 
      // This looks ugly because there is no XOR and events w/o the FTPC or TPC switched on will be cut, otherwise.
    return kFALSE; // possible beam gas event
  }

  // Tpc
  float etaSymPosTpcN = (float)StFlowCutTrack::EtaSymPosTpc();
  float etaSymNegTpcN = (float)StFlowCutTrack::EtaSymNegTpc();
  float etaSymTpc = (etaSymPosTpcN - etaSymNegTpcN) / (etaSymPosTpcN + etaSymNegTpcN);
  // Ftpc
  float etaSymPosFtpcN = (float)StFlowCutTrack::EtaSymPosFtpc();
  float etaSymNegFtpcN = (float)StFlowCutTrack::EtaSymNegFtpc();
  float etaSymFtpc = (etaSymPosFtpcN - etaSymNegFtpcN) / (etaSymPosFtpcN + etaSymNegFtpcN);
  StFlowCutTrack::EtaSymClear();

  Float_t vertexZ = pPicoEvent->VertexZ();
  // Tpc
  float etaSymZSlopeTpc = 0.003;
  etaSymTpc += (etaSymZSlopeTpc * vertexZ); // correction for acceptance
  etaSymTpc *= ::sqrt((double)(etaSymPosTpcN + etaSymNegTpcN)); // corrected for statistics
  // Ftpc
  //float etaSymZSlopeFtpc = 0.003;  // Has to be evaluated, still, therefore ...
  //etaSymFtpc += (etaSymZSlopeFtpc * vertexZ); // ... NOT correctly corrected for acceptance
  etaSymFtpc *= ::sqrt((double)(etaSymPosFtpcN + etaSymNegFtpcN)); // corrected for statistics

  if (mEtaSymTpcCuts[1] > mEtaSymTpcCuts[0] && 
      (etaSymTpc < mEtaSymTpcCuts[0] || etaSymTpc >= mEtaSymTpcCuts[1])) {
    mEtaSymTpcCutN++;
    mGoodEventN--;
    return kFALSE;
  }

  else if (mEtaSymFtpcCuts[1] > mEtaSymFtpcCuts[0] && 
      (etaSymFtpc < mEtaSymFtpcCuts[0] || etaSymFtpc >= mEtaSymFtpcCuts[1])) {
    mEtaSymFtpcCutN++;
    mGoodEventN--;
    return kFALSE;
  }

  return kTRUE;
}

//-----------------------------------------------------------------------

Bool_t StFlowCutEvent::CheckEtaSymmetry(StMuEvent* pMuEvent) {
  // Returns kTRUE if muevent survives this Eta symmetry cut
  // Call at the end of the event after doing CheckTrack for each track
  // If kFALSE you should delete the last event

  if (((StFlowCutTrack::EtaSymPosTpc() == 0 || StFlowCutTrack::EtaSymNegTpc() == 0) &&     // at least one half is empty
       !(StFlowCutTrack::EtaSymPosTpc() == 0 &&  StFlowCutTrack::EtaSymNegTpc() == 0)) ||  // but not both halves 
      ((StFlowCutTrack::EtaSymPosFtpc() == 0 || StFlowCutTrack::EtaSymNegFtpc() == 0) &&   // at least one FTPC is empty
       !(StFlowCutTrack::EtaSymPosFtpc() == 0 && StFlowCutTrack::EtaSymNegFtpc() == 0))) { // but not both FTPCs 
      // This looks ugly because there is no XOR and events w/o the FTPC or TPC switched on will be cut, otherwise.
    return kFALSE; // possible beam gas event
  }

  // Tpc
  float etaSymPosTpcN = (float)StFlowCutTrack::EtaSymPosTpc();
  float etaSymNegTpcN = (float)StFlowCutTrack::EtaSymNegTpc();
  float etaSymTpc = (etaSymPosTpcN - etaSymNegTpcN) / (etaSymPosTpcN + etaSymNegTpcN);
  // Ftpc
  float etaSymPosFtpcN = (float)StFlowCutTrack::EtaSymPosFtpc();
  float etaSymNegFtpcN = (float)StFlowCutTrack::EtaSymNegFtpc();
  float etaSymFtpc = (etaSymPosFtpcN - etaSymNegFtpcN) / (etaSymPosFtpcN + etaSymNegFtpcN);
  StFlowCutTrack::EtaSymClear();

  const StThreeVectorF& vertex = pMuEvent->primaryVertexPosition();
  Float_t vertexZ = vertex.z();
  // Tpc
  float etaSymZSlopeTpc = 0.003;
  etaSymTpc += (etaSymZSlopeTpc * vertexZ); // correction for acceptance
  etaSymTpc *= ::sqrt((double)(etaSymPosTpcN + etaSymNegTpcN)); // corrected for statistics
  // Ftpc
  //float etaSymZSlopeFtpc = 0.003;  // Has to be evaluated, still, therefore ...
  //etaSymFtpc += (etaSymZSlopeFtpc * vertexZ); // ... NOT correctly corrected for acceptance
  etaSymFtpc *= ::sqrt((double)(etaSymPosFtpcN + etaSymNegFtpcN)); // corrected for statistics

  if (mEtaSymTpcCuts[1] > mEtaSymTpcCuts[0] && 
      (etaSymTpc < mEtaSymTpcCuts[0] || etaSymTpc >= mEtaSymTpcCuts[1])) {
    mEtaSymTpcCutN++;
    mGoodEventN--;
    return kFALSE;
  }

  else if (mEtaSymFtpcCuts[1] > mEtaSymFtpcCuts[0] && 
      (etaSymFtpc < mEtaSymFtpcCuts[0] || etaSymFtpc >= mEtaSymFtpcCuts[1])) {
    mEtaSymFtpcCutN++;
    mGoodEventN--;
    return kFALSE;
  }

  return kTRUE;

}

//-----------------------------------------------------------------------

void StFlowCutEvent::PrintCutList() {
  // Prints the list of cuts

  cout << "#######################################################" << endl;
  cout << "# Primary Vertex Triggered Events= " << mEventN << endl;
  cout << "# Event Cut List:" << endl;
  cout << "#   Mult cuts= " << mMultCuts[0] << ", " << mMultCuts[1]
       << " :\t Events Cut= " << mMultCut << endl;
  cout << "#   Centrality cuts= " << mCentCuts[0] << ", " << mCentCuts[1]
       << " :\t Events Cut= " << mCentCut << endl;
  cout << "#   VertexX cuts= " << mVertexXCuts[0] << ", " << mVertexXCuts[1]
       << " :\t Events Cut= " << mVertexXCut << "\t (" <<  setprecision(3) << 
    (float)mVertexXCut/(float)mEventN/perCent << "% cut)" << endl;
  cout << "#   VertexY cuts= " << mVertexYCuts[0] << ", " << mVertexYCuts[1]
       << " :\t Events Cut= " << mVertexYCut << "\t (" <<  setprecision(3) << 
    (float)mVertexYCut/(float)mEventN/perCent << "% cut)" << endl;
  cout << "#   VertexZ cuts= " << mVertexZCuts[0] << ", " << mVertexZCuts[1]
       << " :\t Events Cut= " << mVertexZCut << "\t (" <<  setprecision(3) << 
    (float)mVertexZCut/(float)mEventN/perCent << "% cut)" << endl;
  cout << "#   EtaSymTpc cuts= " << mEtaSymTpcCuts[0] << ", " << mEtaSymTpcCuts[1] 
       << " :\t Events Cut= " << mEtaSymTpcCutN << "\t (" <<  setprecision(3) << 
    (float)mEtaSymTpcCutN/(float)mEventN/perCent << "% cut)" << endl;
  cout << "#   EtaSymFtpc cuts= " << mEtaSymFtpcCuts[0] << ", " << mEtaSymFtpcCuts[1] 
       << " :\t Events Cut= " << mEtaSymFtpcCutN << "\t (" <<  setprecision(3) << 
    (float)mEtaSymFtpcCutN/(float)mEventN/perCent << "% cut)" << endl;
  cout << "#   Trigger cut= " << mTriggerCut 
       << " :\t\t Events Cut= " << mTriggerCutN << "\t (" <<  setprecision(3) << 
    (float)mTriggerCutN/(float)mEventN/perCent << "% cut)" << endl;
  cout << "# Good Events = " << mGoodEventN << ", " << setprecision(3) <<
    (float)mGoodEventN/(float)mEventN/perCent << "%" << endl;
  cout << "#######################################################" << endl;

}

////////////////////////////////////////////////////////////////////////////
//
// $Log: StFlowCutEvent.cxx,v $
// Revision 1.35  2004/08/24 20:24:32  oldi
// Minor modifications to avoid compiler warnings.
// Small bug fix (didn't affect anyone yet).
//
// Revision 1.34  2004/07/07 22:31:06  oldi
// Fix of a severe bug which threw away about 1/3 of all events by cutting on
// vertex_x < 0 && vertex_y < 0 && vertex_z < 0 instead of fabs(...) < 0.
// Thanks to Kirill, who found this (and suffered most).
//
// Revision 1.33  2004/05/31 20:09:35  oldi
// PicoDst format changed (Version 7) to hold ZDC SMD information.
// Trigger cut modified to comply with TriggerCollections.
// Centrality definition for 62 GeV data introduced.
// Minor bug fixes.
//
// Revision 1.32  2004/05/05 21:13:45  aihong
// Gang's code for ZDC-SMD added
//
// Revision 1.31  2003/09/02 17:58:11  perev
// gcc 3.2 updates + WarnOff
//
// Revision 1.30  2003/07/30 22:05:28  oldi
// To get rid of beam gas events events with one empty FTPC or one empty half of
// the TPC are removed.
//
// Revision 1.29  2003/02/25 19:28:38  posk
// Changed a few unimportant default cuts.
//
// Revision 1.28  2003/01/10 16:41:53  oldi
// Several changes to comply with FTPC tracks:
// - Switch to include/exclude FTPC tracks introduced.
//   The same switch changes the range of the eta histograms.
// - Eta symmetry plots for FTPC tracks added and separated from TPC plots.
// - PhiWgts and related histograms for FTPC tracks split in FarEast, East,
//   West, FarWest (depending on vertex.z()).
// - Psi_Diff plots for 2 different selections and the first 2 harmonics added.
// - Cut to exclude mu-events with no primary vertex introduced.
//   (This is possible for UPC events and FTPC tracks.)
// - Global DCA cut for FTPC tracks added.
// - Global DCA cuts for event plane selection separated for TPC and FTPC tracks.
// - Charge cut for FTPC tracks added.
//
// Revision 1.27  2002/06/10 22:50:56  posk
// pt and eta weighting now default.
// DcaGlobalPart default now 0 to 1 cm.
// Event cut order changed.
//
// Revision 1.26  2002/06/07 22:18:37  kirill
// Introduced MuDst reader
//
// Revision 1.25  2002/05/24 11:04:18  snelling
// Added a cut to remove the events triggered by L3
//
// Revision 1.24  2002/03/15 16:43:21  snelling
// Added a method to recalculate the centrality in StFlowPicoEvent
//
// Revision 1.23  2002/01/30 13:04:10  oldi
// Trigger cut implemented.
//
// Revision 1.22  2001/05/22 20:17:13  posk
// Now can do pseudorapidity subevents.
//
// Revision 1.21  2000/12/12 20:22:05  posk
// Put log comments at end of files.
// Deleted persistent StFlowEvent (old micro DST).
//
// Revision 1.20  2000/11/30 16:40:20  snelling
// Protection agains loading probability pid caused it not to work anymore
// therefore protection removed again
//
// Revision 1.19  2000/09/05 16:11:30  snelling
// Added global DCA, electron and positron
//
// Revision 1.18  2000/08/31 18:58:17  posk
// For picoDST, added version number, runID, and multEta for centrality.
// Added centrality cut when reading picoDST.
// Added pt and eta selections for particles corr. wrt event plane.
//
// Revision 1.17  2000/08/10 23:00:19  posk
// New centralities. pt and eta cuts.
//
// Revision 1.15  2000/07/14 23:49:03  snelling
// Changed to ConstIterator for new StEvent and removed comparison int uint
//
// Revision 1.14  2000/07/12 17:54:33  posk
// Added chi2 and dca cuts. Multiplied EtaSym by ::sqrt(mult).
// Apply cuts when reading picoevent file.
//
// Revision 1.13  2000/06/30 14:48:29  posk
// Using MessageMgr, changed Eta Symmetry cut.
//
// Revision 1.12  2000/06/01 18:26:32  posk
// Increased precision of Track integer data members.
//
// Revision 1.11  2000/05/26 21:29:26  posk
// Protected Track data members from overflow.
//
// Revision 1.9  2000/03/02 23:02:38  posk
// Changed extensions from .hh and .cc to .h and .cxx .
//
// Revision 1.5  1999/12/15 22:01:22  posk
// Added StFlowConstants.hh
//
// Revision 1.4  1999/12/04 00:10:30  posk
// Works with the new StEvent
//
// Revision 1.3  1999/11/30 18:52:47  snelling
// First modification for the new StEvent
//
// Revision 1.2  1999/11/24 18:17:09  posk
// Put the methods which act on the data in with the data in StFlowEvent.
//
// Revision 1.1  1999/11/05 00:06:41  posk
// First versions of Flow cut classes.
//
////////////////////////////////////////////////////////////////////////////
