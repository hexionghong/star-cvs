////////////////////////////////////////////////////////////////////////
// 
// $Id: StFlowPicoEvent.cxx,v 1.2 2000/05/26 21:29:31 posk Exp $
//
// Author: Sergei Voloshin and Raimond Snellings, March 2000
//
// Description:  A persistent Flow Pico DST
//
//  The StFlowPicoEvent class has a simple event structure:
//        TClonesArray    *fTracks;
//
//   The StFlowPicoEventHeader class:
//        Int_t           mNtrack;                  // track number
//        Int_t           mEventID;                 // event ID
//        UInt_t          mOrigMult;                // number of StEvent tracks
//        UInt_t          mCentrality;              // centrality bin
//        Float_t         mVertexX, Y, Z;           // primary vertex position
//
//   The StFlowPicoEvent data member fTracks is a pointer to a TClonesArray.
//   It is an array of a variable number of tracks per Event.
//   Each element of the array is an object of class StFlowTrack 
//
////////////////////////////////////////////////////////////////////////
//
// $Log: StFlowPicoEvent.cxx,v $
// Revision 1.2  2000/05/26 21:29:31  posk
// Protected Track data members from overflow.
//
// Revision 1.1  2000/05/23 20:09:48  voloshin
// added StFlowPicoEvent, persistent FlowEvent as plain root TTree
//
// Revision 1.4  2000/05/16 20:59:33  posk
// Voloshin's flowPicoevent.root added.
//
// 
//////////////////////////////////////////////////////////////////////////

#include "StFlowPicoEvent.h"
#define PR(x) cout << "##### FlowPicoEvent: " << (#x) << " = " << (x) << endl;

ClassImp(StFlowPicoEvent)
ClassImp(StFlowPicoTrack)

TClonesArray *StFlowPicoEvent::fgTracks = 0;

//-----------------------------------------------------------------------

StFlowPicoEvent::StFlowPicoEvent()
{
  // Create an StFlowPicoEvent object.
  // When the constructor is invoked for the first time, the class static
  // variable fgTracks is 0 and the TClonesArray fgTracks is created.
  
  if (!fgTracks) fgTracks = new TClonesArray("StFlowPicoTrack", 4000);
  fTracks = fgTracks;
  mNtrack = 0;
}

//-----------------------------------------------------------------------

void StFlowPicoEvent::Clear(Option_t *option)
{
  fTracks->Clear(option);
  mNtrack=0;
}

//---------------------------------------------------------------------

StFlowPicoTrack::StFlowPicoTrack(Float_t pt, 
                                 Float_t eta, 
                                 Float_t phi,
                                 Short_t charge,
                                 Float_t dca,
                                 Float_t chi2,
                                 Int_t   fitPts,
                                 Int_t   maxPts,
                                 Float_t pidPiPlus, 
                                 Float_t pidPiMinus, 
                                 Float_t pidProton) : TObject()
{
  float maxShort  = 32.;
  float maxUShort = 64.;
  
  if (dca > maxUShort)       dca         = maxUShort;
  if (chi2 > maxUShort)      chi2        = maxUShort;
  if (pidPiPlus > maxShort)  pidPiPlus   = maxShort;
  if (pidPiMinus > maxShort) pidPiMinus  = maxShort;
  if (pidProton > maxShort)  pidProton   = maxShort;
  
  mPt         = pt;
  mPhi        = phi;
  mEta        = eta;
  mCharge     = (Char_t)charge;
  mDca        = (UShort_t)(dca*1000.);
  mChi2       = (UShort_t)(chi2*1000.);
  mFitPts     = (UChar_t)fitPts;
  mMaxPts     = (UChar_t)maxPts;
  mPidPiPlus  = (Short_t)(pidPiPlus*1000.);
  mPidPiMinus = (Short_t)(pidPiMinus*1000.);
  mPidProton  = (Short_t)(pidProton*1000.);
}








