//
///////////////////////////////////////////////////////////////////////////////
//
// StAngleCorrMaker
//
// Description: 
//  Calculates high-pt angular correlations from StEvent
//
// Environment:
//  Software developed for the STAR Detector at Brookhaven National Laboratory
//
// Author List:
//  Craig Ogilvie MIT 
//  Torre Wenaus, BNL
//
// History:
//
///////////////////////////////////////////////////////////////////////////////
#include "StAngleCorrMaker.h"
#include "StTrackForPool.hh"
#include "StChain/StChain.h"
#include "StRun.h"
#include "StEvent.h"

#include <TOrdCollection.h>
#include <TH1.h>
#include <TCanvas.h>
#include <TFile.h>

#define TRACKSMAX 100000 

Int_t StAngleCorrMaker::Init() {

  // set up a TOrdCollection as a pool of tracks
  // StTrackForPool has momentum and event number information

  mCollectionOfTracks= new TOrdCollection(TRACKSMAX);
  mNumberEventsInPool= 0;
  mNumberTracksInPool =0;
  // output file
  mOutput = new TFile("corr.root","RECREATE");

  // book an histogram for the numerator of the angular correlation
  int nbin = 60;
  float lbin =0. ;
  float ubin = 180.;
  mHistPhiNumerator = new TH1F("phiNumerator",
			       "relative phi in real events",
			       nbin,lbin,ubin);
  mHistPhiDenominator = new TH1F("phiDenominator",
				 "relative phi in real events",
			         nbin,lbin,ubin);

  return StMaker::Init();
}

Int_t StAngleCorrMaker::Make() {

#if 0
  StEventReaderMaker* evMaker = (StEventReaderMaker*) gStChain->Maker("events");
  if (! event()) return kStOK; // If no event, we're done
  StEvent& ev = *(evMaker->event());
#endif
  StEvent* mEvent = (StEvent *) GetInputDS("StEvent");
  if (!mEvent) return kStOK; // If no event, we're done
  StEvent& ev = *mEvent;

  int eventNumber = GetNumber();
  // process all posssible pairs of primary tracks
  // fill histograms, and add tracks to pool of tracks
  //
  analyseRealPairs(ev,eventNumber);
  mNumberEventsInPool++;
  //
  // check how many tracks in pool
  //
  StTrackForPool *trackfrompool;
  trackfrompool = (StTrackForPool* ) mCollectionOfTracks->Last();
  Int_t poolCounter = mCollectionOfTracks->IndexOf(trackfrompool);
  double aveTracksPerEvent = 
    float(( poolCounter + 1)) / float( mNumberEventsInPool);
  //
  // include in this decision some estimate of average tracks per event
  //
  double numPossiblePairs = pow((float(poolCounter) - aveTracksPerEvent),2);
  if (numPossiblePairs > 2000000 ) {
    analyseMixedPairs();
    //  empty collection 
    //   delete mCollectionOfTracks;
    // mCollectionOfTracks= new TOrdCollection(TRACKSMAX);
    mCollectionOfTracks->Delete();
    mNumberEventsInPool = 0;
    mNumberTracksInPool = 0;  
    trackfrompool = (StTrackForPool* ) mCollectionOfTracks->Last();
    poolCounter = mCollectionOfTracks->IndexOf(trackfrompool);
    cout << "empty pool has " << poolCounter << " tracks" <<endl;     
  };
  return kStOK;
}


StAngleCorrMaker::StAngleCorrMaker(const Char_t *name, const Char_t *title) : StMaker(name, title) {  
}

StAngleCorrMaker::~StAngleCorrMaker() {
}



void StAngleCorrMaker::Clear(Option_t *opt) {

  SafeDelete(m_DataSet);
}

Int_t StAngleCorrMaker::Finish() {

  analyseMixedPairs();
  //  empty collection 
  mCollectionOfTracks->Delete();
  mNumberEventsInPool = 0; 
  // write out histograms
  cout << "writing out histograms" << endl;
  mOutput->Write("MyKey",kSingleKey);
  mOutput->Close();

  return kStOK;
}

ClassImp(StAngleCorrMaker)
