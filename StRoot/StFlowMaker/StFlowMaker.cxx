//////////////////////////////////////////////////////////////////////
//
// $Id: StFlowMaker.cxx,v 1.70 2002/02/05 07:19:26 snelling Exp $
//
// Authors: Raimond Snellings and Art Poskanzer, LBNL, Jun 1999
//          FTPC added by Markus Oldenburg, MPI, Dec 2000
//
//////////////////////////////////////////////////////////////////////
//
// Description: Maker to fill StFlowEvent from StEvent
//
//////////////////////////////////////////////////////////////////////

#include <iostream.h>
#include <stdlib.h>
#include <math.h>
#include "StFlowMaker.h"
#include "StFlowEvent.h"
#include "StFlowPicoEvent.h"
#include "StFlowCutEvent.h"
#include "StFlowCutTrack.h"
#include "StFlowSelection.h"
#include "StFlowConstants.h"
#include "PhysicalConstants.h"
#include "SystemOfUnits.h"
#include "StEvent.h"
#include "StEventTypes.h"
#include "StThreeVector.hh"
#include "StIOMaker/StIOMaker.h"
#include "TFile.h"
#include "TTree.h"
#include "TBranch.h"
#include "TChain.h"
#include "StuRefMult.hh"
#include "StPionPlus.hh"
#include "StPionMinus.hh"
#include "StProton.hh"
#include "StKaonMinus.hh"
#include "StKaonPlus.hh"
#include "StAntiProton.hh"
#include "StDeuteron.hh"
#include "StElectron.hh"
#include "StPositron.hh"
#include "StTpcDedxPidAlgorithm.h"
#include "StuProbabilityPidAlgorithm.h"
#include "StMessMgr.h"
#include "StHbtMaker/StHbtMaker.h"
#include "StHbtMaker/Infrastructure/StHbtTrack.hh"
#define PR(x) cout << "##### FlowMaker: " << (#x) << " = " << (x) << endl;

ClassImp(StFlowMaker)

//-----------------------------------------------------------------------

StFlowMaker::StFlowMaker(const Char_t* name): 
  StMaker(name), 
  mPicoEventWrite(kFALSE), mPicoEventRead(kFALSE), pEvent(NULL) {
  pFlowSelect = new StFlowSelection();
  SetPicoEventDir("./");
}

StFlowMaker::StFlowMaker(const Char_t* name,
			 const StFlowSelection& flowSelect) :
  StMaker(name), 
  mPicoEventWrite(kFALSE), mPicoEventRead(kFALSE), pEvent(NULL) {
  pFlowSelect = new StFlowSelection(flowSelect); //copy constructor
  SetPicoEventDir("./");
}

//-----------------------------------------------------------------------

StFlowMaker::~StFlowMaker() {
}

//-----------------------------------------------------------------------

Int_t StFlowMaker::Make() {
  if (Debug()) gMessMgr->Info() << "FlowMaker: Make() " << endm;

  // Delete previous StFlowEvent
  if (pFlowEvent) delete pFlowEvent;
  pFlowEvent = NULL;

  // Get the input file name from the IOMaker
  if (!mPicoEventRead && pIOMaker) {
    mEventFileName = strrchr(pIOMaker->GetFile(),'/')+1;
    if (Debug()) { 
      gMessMgr->Info() << "FlowMaker: filename: " << mEventFileName << endm;
      gMessMgr->Info() << "FlowMaker: Old filename: " 
		       << mEventFileNameOld << endm;  
    }

    if (mEventFileName != mEventFileNameOld) { 
      if (Debug()) gMessMgr->Info() << "FlowMaker: New file opened " << endm;
      if (mPicoEventWrite && pPicoDST->IsOpen()) {
	pPicoDST->Write(0, TObject::kOverwrite);
	pPicoDST->Close();
      }
      if (mPicoEventWrite) {
	if (pPicoEvent) delete pPicoEvent;
	if (pPicoDST) delete pPicoDST;
	pPicoEvent = NULL;
	pPicoDST = NULL;
	InitPicoEventWrite();
      }
      mEventFileNameOld = mEventFileName;
    }
  }

  
  // Get a pointer to StEvent
  if (!mPicoEventRead) {

    pEvent = dynamic_cast<StEvent*>(GetInputDS("StEvent"));
    if (!pEvent) {
      if (Debug()) { 
	gMessMgr->Info() << "FlowMaker: no StEvent " << endm;
      }
      return kStOK; // If no event, we're done
    }

    // Check the event cuts and fill StFlowEvent
    if (StFlowCutEvent::CheckEvent(pEvent)) {
      // Instantiate a new StFlowEvent
      pFlowEvent = new StFlowEvent;
      if (!pFlowEvent) return kStOK;
      FillFlowEvent();
      if (!pFlowEvent) return kStOK;  // could have been deleted
      if (mPicoEventWrite) FillPicoEvent();
    } else {
      Long_t eventID = pEvent->id();
      gMessMgr->Info() << "##### FlowMaker: event " << eventID 
		       << " cut" << endm;
      return kStOK; // to prevent seg. fault when no event survives
    }

  } else if (mPicoEventRead) {
    // Instantiate a new StFlowEvent
    pFlowEvent = new StFlowEvent;
    if (!pFlowEvent) return kStOK;
    if (!FillFromPicoDST(pPicoEvent)) return kStEOF; // false if EOF
    if (!pFlowEvent) return kStOK; // could have been deleted
  }
  
  UInt_t flowEventMult;
  if (!pFlowEvent) { flowEventMult = 0; }
  else { flowEventMult = pFlowEvent->FlowEventMult(); }

  if (flowEventMult) { // to prevent seg. fault when no particles
    int runID = pFlowEvent->RunID();
    if (runID != mRunID) {
      double beamEnergy = pFlowEvent->CenterOfMassEnergy();
      short beamMassE   = pFlowEvent->BeamMassNumberEast();
      short beamMassW   = pFlowEvent->BeamMassNumberWest();
      gMessMgr->Info() << "##### FlowMaker: " << runID << ", " << beamEnergy
		       << " GeV/c " << beamMassE << " + " << beamMassW << endm;
      mRunID = runID;
    }
  }

  if (Debug()) StMaker::PrintInfo();
  
  return kStOK;
}

//-----------------------------------------------------------------------

Int_t StFlowMaker::Init() {
  if (Debug()) gMessMgr->Info() << "FlowMaker: Init()" << endm;

  // Open PhiWgt file
  ReadPhiWgtFile();

  Int_t kRETURN = kStOK;

  if (!mPicoEventRead) {
    // get input file name
    pIOMaker = (StIOMaker*)GetMaker("IO");
    if (pIOMaker) {
      mEventFileName = strrchr(pIOMaker->GetFile(),'/')+1;
      mEventFileNameOld = mEventFileName; 
    
      gMessMgr->Info() << "##### FlowMaker: truncated filename " 
		       <<  mEventFileName << endm;
    }
    StuProbabilityPidAlgorithm::readParametersFromFile("PIDTable.root");
  }

  if (mPicoEventWrite) kRETURN += InitPicoEventWrite();
  if (mPicoEventRead)  kRETURN += InitPicoEventRead();

  gMessMgr->SetLimit("##### FlowMaker", 5);
  gMessMgr->Info("##### FlowMaker: $Id: StFlowMaker.cxx,v 1.70 2002/02/05 07:19:26 snelling Exp $");
  if (kRETURN) gMessMgr->Info() << "##### FlowMaker: Init return = " << kRETURN << endm;

  return kRETURN;
}

//-----------------------------------------------------------------------

Int_t StFlowMaker::InitRun() {
  if (Debug()) gMessMgr->Info() << "FlowMaker: InitRun()" << endm;

  return kStOK;
}

//-----------------------------------------------------------------------

Int_t StFlowMaker::Finish() {
  if (Debug()) gMessMgr->Info() << "FlowMaker: Finish()" << endm;

  // Print message summary
  cout << endl;
  gMessMgr->Summary(3);
  cout << endl;

  // Print the selection object details
  pFlowSelect->PrintList();

  // Print the cut lists
  cout << "#######################################################" << endl;
  cout << "##### FlowMaker: Cut Lists" << endl;
  StFlowCutEvent::PrintCutList();
  StFlowCutTrack::PrintCutList();
  pFlowEvent->PrintSelectionList();

  if (mPicoEventWrite && pPicoDST->IsOpen()) {
    pPicoDST->Write(0, TObject::kOverwrite);
    pPicoDST->Close();
  }

  return StMaker::Finish();
}

//-----------------------------------------------------------------------

Int_t StFlowMaker::ReadPhiWgtFile() {
  // Read the PhiWgt root file

  if (Debug()) gMessMgr->Info() << "FlowMaker: ReadPhiWgtFile()" << endm;

  TDirectory* dirSave = gDirectory;
  TFile* pPhiWgtFile = new TFile("flowPhiWgt.hist.root", "READ");
  if (!pPhiWgtFile->IsOpen()) {
    gMessMgr->Info("##### FlowMaker: No PhiWgt file. Will set weights = 1.");
  }
  gDirectory = dirSave;

  // Fill mPhiWgt for each selection and each harmonic
  for (int k = 0; k < Flow::nSels; k++) {
    char countSels[2];
    sprintf(countSels,"%d",k+1);
    for (int j = 0; j < Flow::nHars; j++) {
      char countHars[2];
      sprintf(countHars,"%d",j+1);
      // Tpc
      TString* histTitle = new TString("Flow_Phi_Weight_Sel");
      histTitle->Append(*countSels);
      histTitle->Append("_Har");
      histTitle->Append(*countHars);
      // Tpc (FarEast)
      TString* histTitleFarEast = new TString("Flow_Phi_Weight_FarEast_Sel");
      histTitleFarEast->Append(*countSels);
      histTitleFarEast->Append("_Har");
      histTitleFarEast->Append(*countHars);
      // Tpc (East)
      TString* histTitleEast = new TString("Flow_Phi_Weight_East_Sel");
      histTitleEast->Append(*countSels);
      histTitleEast->Append("_Har");
      histTitleEast->Append(*countHars);
      // Tpc (West)
      TString* histTitleWest = new TString("Flow_Phi_Weight_West_Sel");
      histTitleWest->Append(*countSels);
      histTitleWest->Append("_Har");
      histTitleWest->Append(*countHars);
      // Tpc (FarWest)
      TString* histTitleFarWest = new TString("Flow_Phi_Weight_FarWest_Sel");
      histTitleFarWest->Append(*countSels);
      histTitleFarWest->Append("_Har");
      histTitleFarWest->Append(*countHars);
      // Ftpc (East)
      TString* histTitleFtpcEast = new TString("Flow_Phi_Weight_FtpcEast_Sel");
      histTitleFtpcEast->Append(*countSels);
      histTitleFtpcEast->Append("_Har");
      histTitleFtpcEast->Append(*countHars);
      // Ftpc (West)
      TString* histTitleFtpcWest = new TString("Flow_Phi_Weight_FtpcWest_Sel");
      histTitleFtpcWest->Append(*countSels);
      histTitleFtpcWest->Append("_Har");
      histTitleFtpcWest->Append(*countHars);
      if (pPhiWgtFile->IsOpen()) {
	TH1* phiWgtHist = dynamic_cast<TH1*>(pPhiWgtFile->Get(histTitle->Data()));
	if (k==0 && j==0 && phiWgtHist) {
	  mOnePhiWgt = kTRUE;
	  gMessMgr->Info("##### FlowMaker: Using old type phi weight file.");
	}
	if (mOnePhiWgt) {
	  for (int n = 0; n < Flow::nPhiBins; n++) {
	    mPhiWgt[k][j][n] = (phiWgtHist) ? 
	      phiWgtHist->GetBinContent(n+1) : 1.;
	  }
	} else {
	  TH1* phiWgtHistFarEast = dynamic_cast<TH1*>(pPhiWgtFile->
						      Get(histTitleFarEast->Data()));
	  TH1* phiWgtHistEast = dynamic_cast<TH1*>(pPhiWgtFile->
						   Get(histTitleEast->Data()));
	  TH1* phiWgtHistWest = dynamic_cast<TH1*>(pPhiWgtFile->
						   Get(histTitleWest->Data()));
	  TH1* phiWgtHistFarWest = dynamic_cast<TH1*>(pPhiWgtFile->
						      Get(histTitleFarWest->Data()));
	  for (int n = 0; n < Flow::nPhiBins; n++) {
	    mPhiWgtFarEast[k][j][n] = (phiWgtHistFarEast) ? 
	      phiWgtHistFarEast->GetBinContent(n+1) : 1.;
	    mPhiWgtEast[k][j][n] = (phiWgtHistEast) ? 
	      phiWgtHistEast->GetBinContent(n+1) : 1.;
	    mPhiWgtWest[k][j][n] = (phiWgtHistWest) ? 
	      phiWgtHistWest->GetBinContent(n+1) : 1.;
	    mPhiWgtFarWest[k][j][n] = (phiWgtHistFarWest) ? 
	      phiWgtHistFarWest->GetBinContent(n+1) : 1.;
	  }
	}
	TH1* phiWgtHistFtpcEast = dynamic_cast<TH1*>(pPhiWgtFile->
						     Get(histTitleFtpcEast->Data()));
	TH1* phiWgtHistFtpcWest = dynamic_cast<TH1*>(pPhiWgtFile->
						     Get(histTitleFtpcWest->Data()));
	{for (int n = 0; n < Flow::nPhiBinsFtpc; n++) {
	  mPhiWgtFtpcEast[k][j][n] = (phiWgtHistFtpcEast) ? 
	    phiWgtHistFtpcEast->GetBinContent(n+1) : 1.;
	  mPhiWgtFtpcWest[k][j][n] = (phiWgtHistFtpcWest) ? 
	    phiWgtHistFtpcWest->GetBinContent(n+1) : 1.;
	}}
      } else {
	for (int n = 0; n < Flow::nPhiBins; n++) {
	  mPhiWgt[k][j][n]        = 1.;
	  mPhiWgtFarEast[k][j][n] = 1.;
	  mPhiWgtEast[k][j][n]    = 1.;
	  mPhiWgtWest[k][j][n]    = 1.;
	  mPhiWgtFarWest[k][j][n] = 1.;
	}
	for (int n = 0; n < Flow::nPhiBinsFtpc; n++) {
	  mPhiWgtFtpcEast[k][j][n] = 1.;
	  mPhiWgtFtpcWest[k][j][n] = 1.;
	}
      }
      delete histTitle;
      delete histTitleFarEast;
      delete histTitleEast;
      delete histTitleWest;
      delete histTitleFarWest;
      delete histTitleFtpcEast;
      delete histTitleFtpcWest;
    }
  }

  // Close PhiWgt file
  if (pPhiWgtFile->IsOpen()) pPhiWgtFile->Close();

  return kStOK;
}

//-----------------------------------------------------------------------

void StFlowMaker::FillFlowEvent() {
  // Make StFlowEvent from StEvent

  if (Debug()) gMessMgr->Info() << "FlowMaker: FillFlowEvent()" << endm;

  // Fill PhiWgt array
  if (mOnePhiWgt) {
    pFlowEvent->SetOnePhiWgt();
    pFlowEvent->SetPhiWeight(mPhiWgt);
  } else {
    pFlowEvent->SetPhiWeightFarEast(mPhiWgtFarEast);
    pFlowEvent->SetPhiWeightEast(mPhiWgtEast);
    pFlowEvent->SetPhiWeightWest(mPhiWgtWest);
    pFlowEvent->SetPhiWeightFarWest(mPhiWgtFarWest);
  }
  pFlowEvent->SetPhiWeightFtpcEast(mPhiWgtFtpcEast);
  pFlowEvent->SetPhiWeightFtpcWest(mPhiWgtFtpcWest);

  // Get Trigger information
  StL0Trigger* pTrigger = pEvent->l0Trigger();
  if (pTrigger) {
    pFlowEvent->SetL0TriggerWord(pTrigger->triggerWord());
  }

  // Get event id 
  pFlowEvent->SetEventID((Int_t)(pEvent->id()));
  pFlowEvent->SetRunID((Int_t)(pEvent->runId()));

  if (pEvent->runInfo()) {
    pFlowEvent->SetCenterOfMassEnergy(pEvent->runInfo()->centerOfMassEnergy());
    pFlowEvent->SetMagneticField(pEvent->runInfo()->magneticField());
    pFlowEvent->SetBeamMassNumberEast(pEvent->runInfo()->beamMassNumber(east));
    pFlowEvent->SetBeamMassNumberWest(pEvent->runInfo()->beamMassNumber(west));
  } else {
    gMessMgr->Info() << "FlowMaker: no Run Info, reverting to year 1 settings "
		     << endm;
    pFlowEvent->SetCenterOfMassEnergy(130);
    pFlowEvent->SetMagneticField(4.98);
    pFlowEvent->SetBeamMassNumberEast(208);
    pFlowEvent->SetBeamMassNumberWest(208);
  }

  // Get primary vertex position
  const StThreeVectorF& vertex = pEvent->primaryVertex(0)->position();
  pFlowEvent->SetVertexPos(vertex);

  // include trigger (ZDC and CTB)
  Float_t ctb  = -1.;
  Float_t zdce = -1.;
  Float_t zdcw = -1.;
  StTriggerDetectorCollection *triggers = pEvent->triggerDetectorCollection();
  if (triggers)	{
    StCtbTriggerDetector &CTB = triggers->ctb();
    StZdcTriggerDetector &ZDC = triggers->zdc();
    // get CTB
    for (UInt_t slat = 0; slat < CTB.numberOfSlats(); slat++) {
      for (UInt_t tray = 0; tray < CTB.numberOfTrays(); tray++) {
	ctb += CTB.mips(tray,slat,0);
      }
    }
    //get ZDCe and ZDCw        
    zdce = ZDC.adcSum(east);
    zdcw = ZDC.adcSum(west);
  } 
  pFlowEvent->SetCTB(ctb);
  pFlowEvent->SetZDCe(zdce);
  pFlowEvent->SetZDCw(zdcw);
  
  // Get initial multiplicity before TrackCuts 
  UInt_t origMult = pEvent->primaryVertex(0)->numberOfDaughters(); 
  pFlowEvent->SetOrigMult(origMult);
  PR(origMult);
  pFlowEvent->SetUncorrNegMult(uncorrectedNumberOfNegativePrimaries(*pEvent));
  pFlowEvent->SetUncorrPosMult(uncorrectedNumberOfPositivePrimaries(*pEvent));

  // define functor for pid probability algorithm
  StuProbabilityPidAlgorithm uPid(*pEvent);

  // loop over tracks in StEvent
  int goodTracks    = 0;
  int goodTracksEta = 0;
  StTpcDedxPidAlgorithm tpcDedxAlgo;
  Float_t nSigma;
  Float_t dcaSigned;

  StSPtrVecTrackNode& trackNode = pEvent->trackNodes();

  for (unsigned int j=0; j < trackNode.size(); j++) {
    StGlobalTrack* gTrack = 
      static_cast<StGlobalTrack*>(trackNode[j]->track(global));
    StPrimaryTrack* pTrack = 
      static_cast<StPrimaryTrack*>(trackNode[j]->track(primary));

    // Tricking flowMaker to use global tracks 
    // StPrimaryTrack* pTrack = (StPrimaryTrack*)(trackNode[j]->track(global));

    if (pTrack && pTrack->flag() > 0) {
      StThreeVectorD p = pTrack->geometry()->momentum();
      StThreeVectorD g = gTrack->geometry()->momentum(); // p of global track
      // calculate the number of tracks with positive flag & |eta| < 0.75
      if (fabs(p.pseudoRapidity()) < 0.75) goodTracksEta++;
      if (StFlowCutTrack::CheckTrack(pTrack)) {
	// Instantiate new StFlowTrack
	StFlowTrack* pFlowTrack = new StFlowTrack;
	if (!pFlowTrack) return;
	pFlowTrack->SetPhi(p.phi());
	pFlowTrack->SetPhiGlobal(g.phi());
	pFlowTrack->SetEta(p.pseudoRapidity());
	pFlowTrack->SetEtaGlobal(g.pseudoRapidity());
	pFlowTrack->SetPt(p.perp());
	pFlowTrack->SetPtGlobal(g.perp());
	pFlowTrack->SetCharge(pTrack->geometry()->charge());

	dcaSigned = CalcDcaSigned(vertex,gTrack);
	pFlowTrack->SetDcaSigned(dcaSigned);
	pFlowTrack->SetDca(pTrack->impactParameter());
	pFlowTrack->SetDcaGlobal(gTrack->impactParameter());
	pFlowTrack->SetChi2((Float_t)(pTrack->fitTraits().chi2()));
	pFlowTrack->SetFitPts(pTrack->fitTraits().numberOfFitPoints());
	pFlowTrack->SetMaxPts(pTrack->numberOfPossiblePoints());

	Double_t pathLength = gTrack->geometry()->helix().pathLength(vertex);
	StThreeVectorD distance = gTrack->geometry()->helix().at(pathLength);
	pFlowTrack->SetDcaGlobal3(distance - vertex);

	pFlowTrack->SetTrackLength(pTrack->length());
	pFlowTrack->SetNhits(pTrack->detectorInfo()->numberOfPoints(kTpcId));
	
	pTrack->pidTraits(tpcDedxAlgo);       // initialize
	nSigma = (float)tpcDedxAlgo.numberOfSigma(StPionPlus::instance());
	pFlowTrack->SetPidPiPlus(nSigma);
	nSigma = (float)tpcDedxAlgo.numberOfSigma(StPionMinus::instance());
	pFlowTrack->SetPidPiMinus(nSigma);
	nSigma = (float)tpcDedxAlgo.numberOfSigma(StProton::instance());
	pFlowTrack->SetPidProton(nSigma);
	nSigma = (float)tpcDedxAlgo.numberOfSigma(StAntiProton::instance());
	pFlowTrack->SetPidAntiProton(nSigma);
	nSigma = (float)tpcDedxAlgo.numberOfSigma(StKaonMinus::instance());
	pFlowTrack->SetPidKaonMinus(nSigma);
	nSigma = (float)tpcDedxAlgo.numberOfSigma(StKaonPlus::instance());
	pFlowTrack->SetPidKaonPlus(nSigma);
	nSigma = (float)tpcDedxAlgo.numberOfSigma(StDeuteron::instance());
	pFlowTrack->SetPidDeuteron(nSigma);
	if (pTrack->geometry()->charge() < 0) {
	  pFlowTrack->SetPidAntiDeuteron(nSigma);
	}
	nSigma = (float)tpcDedxAlgo.numberOfSigma(StElectron::instance());
	pFlowTrack->SetPidElectron(nSigma);
	nSigma = (float)tpcDedxAlgo.numberOfSigma(StPositron::instance());
	pFlowTrack->SetPidPositron(nSigma);

	pFlowTrack->SetTopologyMap(pTrack->topologyMap());
	
	// dE/dx
	StPtrVecTrackPidTraits traits = pTrack->pidTraits(kTpcId);
        unsigned int size = traits.size();
        if (size) {
	  StDedxPidTraits* pid;
	  for (unsigned int i = 0; i < traits.size(); i++) {
	    pid = dynamic_cast<StDedxPidTraits*>(traits[i]);
	    if (pid && pid->method() == kTruncatedMeanId) break;
	  }
	  assert(pid); 
	  pFlowTrack->SetDedx(pid->mean());
	  pFlowTrack->SetNdedxPts(pid->numberOfPoints());
        }

	// Probability pid
	const StParticleDefinition* def = pTrack->pidTraits(uPid);
	pFlowTrack->SetMostLikelihoodPID(uPid.mostLikelihoodParticleGeantID());
	pFlowTrack->SetMostLikelihoodProb(uPid.mostLikelihoodProbability());
	if (uPid.isExtrap()) pFlowTrack->SetExtrapTag(1); //merging area. 
	else pFlowTrack->SetExtrapTag(0); 
	if (pTrack->geometry()->charge() < 0) {
	  pFlowTrack->SetElectronPositronProb(uPid.beingElectronProb());
	  pFlowTrack->SetPionPlusMinusProb(uPid.beingPionMinusProb());
	  pFlowTrack->SetKaonPlusMinusProb(uPid.beingKaonMinusProb());
	  pFlowTrack->SetProtonPbarProb(uPid.beingAntiProtonProb());
	} else if (pTrack->geometry()->charge() > 0) {
	  pFlowTrack->SetElectronPositronProb(uPid.beingPositronProb());
	  pFlowTrack->SetPionPlusMinusProb(uPid.beingPionPlusProb());
	  pFlowTrack->SetKaonPlusMinusProb(uPid.beingKaonPlusProb());
	  pFlowTrack->SetProtonPbarProb(uPid.beingProtonProb());
	}
	
	pFlowEvent->TrackCollection()->push_back(pFlowTrack);
	goodTracks++;
      }
    }
  }

  // Check Eta Symmetry
  if (!StFlowCutEvent::CheckEtaSymmetry(pEvent)) {  
    delete pFlowEvent;             //  delete this event
    pFlowEvent = NULL;
    return;
  }

  pFlowEvent->SetMultEta(goodTracksEta);
  pFlowEvent->SetCentrality(goodTracksEta);
  (pFlowEvent->ProbPid()) ? pFlowEvent->SetPidsProb() : 
    pFlowEvent->SetPidsDeviant();
  
  pFlowEvent->TrackCollection()->random_shuffle();

  pFlowEvent->SetSelections();
  (pFlowEvent->EtaSubs()) ? pFlowEvent->MakeEtaSubEvents() :
    pFlowEvent->MakeSubEvents();

}

//----------------------------------------------------------------------

void StFlowMaker::FillFlowEvent(StHbtEvent* hbtEvent) {
  // For use with HBT Maker. By Randy Wells.

  if (Debug()) gMessMgr->Info() << "FlowMaker: FillFlowEvent(HbtEvent)" << endm;

  // Delete previous StFlowEvent
  if (pFlowEvent) delete pFlowEvent;
  pFlowEvent = NULL;
  // Instantiate a new StFlowEvent
  pFlowEvent = new StFlowEvent;

  cout << "Inside FlowMaker::FillFlowEvent(HbtEvent)..." << endl;

  // set phiweights
  if (mOnePhiWgt) {
    pFlowEvent->SetOnePhiWgt();
    pFlowEvent->SetPhiWeight(mPhiWgt);
  } else {
    pFlowEvent->SetPhiWeightFarEast(mPhiWgtFarEast);
    pFlowEvent->SetPhiWeightEast(mPhiWgtEast);
    pFlowEvent->SetPhiWeightWest(mPhiWgtWest);
    pFlowEvent->SetPhiWeightFarWest(mPhiWgtFarWest);
  }
  pFlowEvent->SetPhiWeightFtpcEast(mPhiWgtFtpcEast);
  pFlowEvent->SetPhiWeightFtpcWest(mPhiWgtFtpcWest);

  // Event ID and Run ID
  // ????????
  // Primary Vertex
  pFlowEvent->SetVertexPos( hbtEvent->PrimVertPos() );
  // Triggers
  pFlowEvent->SetCTB( hbtEvent->CtbMult() );
  pFlowEvent->SetZDCe( hbtEvent->ZdcAdcEast() );
  pFlowEvent->SetZDCw( hbtEvent->ZdcAdcWest() );
  // Get initial multiplicity before TrackCuts
  UInt_t origMult = hbtEvent->NumberOfTracks();
  pFlowEvent->SetOrigMult(origMult);
  PR(origMult);
  // Fill track info
  int goodTracks    = 0;
  int goodTracksEta = 0;
  StHbtTrack* pParticle;
  StHbtTrackIterator pIter;
  StHbtTrackIterator startLoop = hbtEvent->TrackCollection()->begin();
  StHbtTrackIterator endLoop   = hbtEvent->TrackCollection()->end();
  for (pIter=startLoop; pIter!=endLoop; pIter++){
    pParticle = *pIter;
    // Instantiate new StFlowTrack
    StFlowTrack* pFlowTrack = new StFlowTrack;
    if (!pFlowTrack) return;
    double px = pParticle->P().x();
    double py = pParticle->P().y();
    double phi = atan2(py,px);
    if (phi<0.0) phi+=twopi;
    pFlowTrack->SetPhi( phi );
    pFlowTrack->SetPhiGlobal( phi );
    double pz = pParticle->P().z();
    double pTotal = pParticle->P().mag();
    double eta = 0.5*log( (1.0+pz/pTotal)/(1.0-pz/pTotal) );
    pFlowTrack->SetEta( eta );
    pFlowTrack->SetEtaGlobal( eta );
    pFlowTrack->SetPt( pParticle->Pt() );
    pFlowTrack->SetPtGlobal( pParticle->Pt() );
    pFlowTrack->SetCharge( int(pParticle->Charge()) );
    double dcaXY = pParticle->DCAxy();
    double dcaZ = pParticle->DCAz();
    double dca = sqrt( dcaXY*dcaXY + dcaZ*dcaZ );
    //pFlowTrack->SetDca( dca );
    pFlowTrack->SetDcaGlobal( dca );
    pFlowTrack->SetChi2( pParticle->ChiSquaredXY() );
    pFlowTrack->SetFitPts( pParticle->NHits() );
    pFlowTrack->SetMaxPts( pParticle->NHitsPossible() );
    // PID
    pFlowTrack->SetPidPiPlus( pParticle->NSigmaPion() );
    pFlowTrack->SetPidPiMinus( pParticle->NSigmaPion() );
    pFlowTrack->SetPidProton( pParticle->NSigmaProton() );
    pFlowTrack->SetPidAntiProton( pParticle->NSigmaProton() );
    pFlowTrack->SetPidKaonMinus( pParticle->NSigmaKaon() );
    pFlowTrack->SetPidKaonPlus( pParticle->NSigmaKaon() );
    pFlowTrack->SetPidDeuteron( 0.0 );
    pFlowTrack->SetPidAntiDeuteron( 0.0 );
    pFlowTrack->SetPidElectron( pParticle->NSigmaElectron() );
    pFlowTrack->SetPidPositron( pParticle->NSigmaElectron() );
    // dEdx
    if ( pParticle->NSigmaKaon() > 2.0 ) {
      if (pParticle->Charge() > 0 ) {
	pFlowTrack->SetMostLikelihoodPID(14); // proton
	pFlowTrack->SetMostLikelihoodProb( 0.99 ); // guaranteed
      }
      else {
	pFlowTrack->SetMostLikelihoodPID(15); // anti-proton
	pFlowTrack->SetMostLikelihoodProb( 0.99 ); // guaranteed
      }
    }
    if ( pParticle->NSigmaPion() > 2.0 ) {
      if (pParticle->Charge() > 0 ) {
	pFlowTrack->SetMostLikelihoodPID(11); // kaon
	pFlowTrack->SetMostLikelihoodProb( 0.99 ); // guaranteed
      }
      else {
	pFlowTrack->SetMostLikelihoodPID(12); // anti-kaon
	pFlowTrack->SetMostLikelihoodProb( 0.99 ); // guaranteed
      }
    }
    if ( pParticle->NSigmaPion() < -2.0 ) {
      if (pParticle->Charge() < 0 ) {
	pFlowTrack->SetMostLikelihoodPID(3); // electron
	pFlowTrack->SetMostLikelihoodProb( 0.99 ); // guaranteed
      }
      else {
	pFlowTrack->SetMostLikelihoodPID(2); // positron
	pFlowTrack->SetMostLikelihoodProb( 0.99 ); // guaranteed
      }
    }
    pFlowTrack->SetExtrapTag(0); // none are in the PID merging area
    pFlowEvent->TrackCollection()->push_back(pFlowTrack);
    goodTracks++;
  }

  pFlowEvent->SetMultEta(goodTracksEta);
  pFlowEvent->SetCentrality(goodTracksEta);
  pFlowEvent->TrackCollection()->random_shuffle();
  pFlowEvent->SetSelections();
  pFlowEvent->MakeSubEvents();
}

//----------------------------------------------------------------------

void StFlowMaker::FillPicoEvent() {
  // Make StFlowPicoEvent from StFlowEvent

  if (Debug()) gMessMgr->Info() << "FlowMaker: FillPicoEvent()" << endm;

  if (!pPicoEvent) {
    gMessMgr->Warning("##### FlowMaker: No FlowPicoEvent");
    return;
  }
  StFlowPicoTrack* pFlowPicoTrack = new StFlowPicoTrack();
  
  pPicoEvent->SetVersion(5);         // version 5 
  pPicoEvent->SetEventID(pFlowEvent->EventID());
  pPicoEvent->SetRunID(pFlowEvent->RunID());
  pPicoEvent->SetL0TriggerWord(pFlowEvent->L0TriggerWord());

  pPicoEvent->SetCenterOfMassEnergy(pFlowEvent->CenterOfMassEnergy());
  pPicoEvent->SetMagneticField(pFlowEvent->MagneticField());
  pPicoEvent->SetBeamMassNumberEast(pFlowEvent->BeamMassNumberEast());
  pPicoEvent->SetBeamMassNumberWest(pFlowEvent->BeamMassNumberWest());

  pPicoEvent->SetOrigMult(pFlowEvent->OrigMult());
  pPicoEvent->SetUncorrNegMult(pFlowEvent->UncorrNegMult());
  pPicoEvent->SetUncorrPosMult(pFlowEvent->UncorrPosMult());
  pPicoEvent->SetMultEta(pFlowEvent->MultEta());
  pPicoEvent->SetCentrality(pFlowEvent->Centrality());
  pPicoEvent->SetVertexPos(pFlowEvent->VertexPos().x(),
			   pFlowEvent->VertexPos().y(),
			   pFlowEvent->VertexPos().z());
  pPicoEvent->SetCTB(pFlowEvent->CTB());
  pPicoEvent->SetZDCe(pFlowEvent->ZDCe());
  pPicoEvent->SetZDCw(pFlowEvent->ZDCw());
  
  StFlowTrackIterator itr;
  StFlowTrackCollection* pFlowTracks = pFlowEvent->TrackCollection();
  
  for (itr = pFlowTracks->begin(); itr != pFlowTracks->end(); itr++) {
    StFlowTrack* pFlowTrack = *itr;
    pFlowPicoTrack->SetPt(pFlowTrack->Pt());
    pFlowPicoTrack->SetPtGlobal(pFlowTrack->PtGlobal());
    pFlowPicoTrack->SetEta(pFlowTrack->Eta());
    pFlowPicoTrack->SetEtaGlobal(pFlowTrack->EtaGlobal());
    pFlowPicoTrack->SetDedx(pFlowTrack->Dedx());
    pFlowPicoTrack->SetPhi(pFlowTrack->Phi());
    pFlowPicoTrack->SetPhiGlobal(pFlowTrack->PhiGlobal());
    pFlowPicoTrack->SetCharge(pFlowTrack->Charge());
    pFlowPicoTrack->SetDcaSigned(pFlowTrack->DcaSigned());
    pFlowPicoTrack->SetDca(pFlowTrack->Dca());
    pFlowPicoTrack->SetDcaGlobal(pFlowTrack->DcaGlobal());
    pFlowPicoTrack->SetChi2(pFlowTrack->Chi2());
    pFlowPicoTrack->SetFitPts(pFlowTrack->FitPts());
    pFlowPicoTrack->SetMaxPts(pFlowTrack->MaxPts());
    pFlowPicoTrack->SetNhits(pFlowTrack->Nhits());
    pFlowPicoTrack->SetNdedxPts(pFlowTrack->NdedxPts());
    pFlowPicoTrack->SetDcaGlobal3(pFlowTrack->DcaGlobal3().x(),
				  pFlowTrack->DcaGlobal3().y(),
				  pFlowTrack->DcaGlobal3().z());
    pFlowPicoTrack->SetTrackLength(pFlowTrack->TrackLength());
    pFlowPicoTrack->SetMostLikelihoodPID(pFlowTrack->MostLikelihoodPID()); 
    pFlowPicoTrack->SetMostLikelihoodProb(pFlowTrack->MostLikelihoodProb());
    pFlowPicoTrack->SetExtrapTag(pFlowTrack->ExtrapTag());
    pFlowPicoTrack->SetElectronPositronProb(pFlowTrack->ElectronPositronProb());
    pFlowPicoTrack->SetPionPlusMinusProb(pFlowTrack->PionPlusMinusProb());
    pFlowPicoTrack->SetKaonPlusMinusProb(pFlowTrack->KaonPlusMinusProb());
    pFlowPicoTrack->SetProtonPbarProb(pFlowTrack->ProtonPbarProb());
    if (pFlowPicoTrack->Charge() > 0) {
      pFlowPicoTrack->SetPidPion(pFlowTrack->PidPiPlus());
      pFlowPicoTrack->SetPidProton(pFlowTrack->PidProton());
      pFlowPicoTrack->SetPidKaon(pFlowTrack->PidKaonPlus());
      pFlowPicoTrack->SetPidDeuteron(pFlowTrack->PidDeuteron());
      pFlowPicoTrack->SetPidElectron(pFlowTrack->PidPositron());
    } else {
      pFlowPicoTrack->SetPidPion(pFlowTrack->PidPiMinus());
      pFlowPicoTrack->SetPidProton(pFlowTrack->PidAntiProton());
      pFlowPicoTrack->SetPidKaon(pFlowTrack->PidKaonMinus());
      pFlowPicoTrack->SetPidDeuteron(pFlowTrack->PidAntiDeuteron());
      pFlowPicoTrack->SetPidElectron(pFlowTrack->PidElectron());
    }
    pFlowPicoTrack->SetTopologyMap(pFlowTrack->TopologyMap().data(0),
				   pFlowTrack->TopologyMap().data(1));
    pPicoEvent->AddTrack(pFlowPicoTrack);
  }
  
  pFlowTree->Fill();  //fill the tree
  pPicoEvent->Clear();
  
  delete pFlowPicoTrack;  
}

//-----------------------------------------------------------------------

Bool_t StFlowMaker::FillFromPicoDST(StFlowPicoEvent* pPicoEvent) {
  // Make StFlowEvent from StFlowPicoEvent
  
  if (Debug()) gMessMgr->Info() << "FlowMaker: FillFromPicoDST()" << endm;
  if (Debug()) gMessMgr->Info() << "FlowMaker: Pico Version: " <<  
		 pPicoEvent->Version() << endm;

  if (!pPicoEvent || !pPicoChain->GetEntry(mPicoEventCounter++)) {
    cout << "##### FlowMaker: no more events" << endl; 
    return kFALSE; 
  }
  
  // Set phi weights
  if (mOnePhiWgt) {
    pFlowEvent->SetOnePhiWgt();
    pFlowEvent->SetPhiWeight(mPhiWgt);
  } else {
    pFlowEvent->SetPhiWeightFarEast(mPhiWgtFarEast);
    pFlowEvent->SetPhiWeightEast(mPhiWgtEast);
    pFlowEvent->SetPhiWeightWest(mPhiWgtWest);
    pFlowEvent->SetPhiWeightFarWest(mPhiWgtFarWest);
  }
  pFlowEvent->SetPhiWeightFtpcEast(mPhiWgtFtpcEast);
  pFlowEvent->SetPhiWeightFtpcWest(mPhiWgtFtpcWest);
  
  // Check event cuts
  if (!StFlowCutEvent::CheckEvent(pPicoEvent)) {  
    Int_t eventID = pPicoEvent->EventID();
    gMessMgr->Info() << "##### FlowMaker: picoevent " << eventID 
  		     << " cut" << endm;
    delete pFlowEvent;             // delete this event
    pFlowEvent = NULL;
    return kTRUE;
  }

  switch (pPicoEvent->Version()) {
  case 5: FillFromPicoVersion5DST(pPicoEvent);
    break;
  case 4: FillFromPicoVersion4DST(pPicoEvent);
    break;
  case 3: FillFromPicoVersion3DST(pPicoEvent);
    break;
  case 2: FillFromPicoVersion2DST(pPicoEvent);
    break;
  case 1: FillFromPicoVersion1DST(pPicoEvent);
    break;
  case 0: FillFromPicoVersion0DST(pPicoEvent);
    break;
  default:
    cout << "##### FlowMaker: Illegal pico file version" << endl;
    return kStFatal;
  }
  
  // Check Eta Symmetry
  if (!StFlowCutEvent::CheckEtaSymmetry(pPicoEvent)) {  
    Int_t eventID = pPicoEvent->EventID();
    gMessMgr->Info() << "##### FlowMaker: picoevent " << eventID 
		     << " cut" << endm;
    delete pFlowEvent;             // delete this event
    pFlowEvent = NULL;
    return kTRUE;
  }
  
  (pFlowEvent->ProbPid()) ? pFlowEvent->SetPidsProb() : 
    pFlowEvent->SetPidsDeviant();

  pFlowEvent->TrackCollection()->random_shuffle();

  pFlowEvent->SetSelections();
  (pFlowEvent->EtaSubs()) ? pFlowEvent->MakeEtaSubEvents() :
    pFlowEvent->MakeSubEvents();
    
  return kTRUE;
}

//-----------------------------------------------------------------------

Bool_t StFlowMaker::FillFromPicoVersion0DST(StFlowPicoEvent* pPicoEvent) {
  // Make StFlowEvent from StFlowPicoEvent
  
  if (Debug()) gMessMgr->Info() << "FlowMaker: FillFromPicoVersion0DST()" << endm;
  
  pFlowEvent->SetEventID(pPicoEvent->EventID());
  UInt_t origMult = pPicoEvent->OrigMult();
  pFlowEvent->SetOrigMult(origMult);
  PR(origMult);
  pFlowEvent->SetVertexPos(StThreeVectorF(pPicoEvent->VertexX(),
					  pPicoEvent->VertexY(),
					  pPicoEvent->VertexZ()) );
  pFlowEvent->SetCTB(pPicoEvent->CTB());
  pFlowEvent->SetZDCe(pPicoEvent->ZDCe());
  pFlowEvent->SetZDCw(pPicoEvent->ZDCw());
  
  int    goodTracks    = 0;
  UInt_t goodTracksEta = 0;
  // Fill FlowTracks
  for (Int_t nt=0; nt<pPicoEvent->GetNtrack(); nt++) {
    StFlowPicoTrack* pPicoTrack = (StFlowPicoTrack*)pPicoEvent->Tracks()
      ->UncheckedAt(nt);
    if (fabs(pPicoTrack->Eta()) < 0.75) goodTracksEta++;
    if (pPicoTrack && StFlowCutTrack::CheckTrack(pPicoTrack)) {
      // Instantiate new StFlowTrack
      StFlowTrack* pFlowTrack = new StFlowTrack;
      if (!pFlowTrack) return kFALSE;
      pFlowTrack->SetPt(pPicoTrack->Pt());
      pFlowTrack->SetPhi(pPicoTrack->Phi());
      pFlowTrack->SetEta(pPicoTrack->Eta());
      pFlowTrack->SetDedx(pPicoTrack->Dedx());
      pFlowTrack->SetCharge(pPicoTrack->Charge());
      pFlowTrack->SetChi2(pPicoTrack->Chi2()/10000.);
      pFlowTrack->SetFitPts(pPicoTrack->FitPts());
      pFlowTrack->SetMaxPts(pPicoTrack->MaxPts());
      
      pFlowEvent->TrackCollection()->push_back(pFlowTrack);
      goodTracks++;
    }
  }
  
  // Recreate centrality
  pFlowEvent->SetMultEta(goodTracksEta);
  pFlowEvent->SetCentrality(goodTracksEta);
  
  return kTRUE;
}

//-----------------------------------------------------------------------

Bool_t StFlowMaker::FillFromPicoVersion1DST(StFlowPicoEvent* pPicoEvent) {
  // Make StFlowEvent from StFlowPicoEvent
  
  if (Debug()) gMessMgr->Info() << "FlowMaker: FillFromPicoVersion1DST()" << endm;

  pFlowEvent->SetEventID(pPicoEvent->EventID());
  UInt_t origMult = pPicoEvent->OrigMult();
  pFlowEvent->SetOrigMult(origMult);
  PR(origMult);
  pFlowEvent->SetVertexPos(StThreeVectorF(pPicoEvent->VertexX(),
					  pPicoEvent->VertexY(),
					  pPicoEvent->VertexZ()) );
  pFlowEvent->SetMultEta(pPicoEvent->MultEta());
  pFlowEvent->SetCentrality(pPicoEvent->MultEta());
  pFlowEvent->SetRunID(pPicoEvent->RunID());
  pFlowEvent->SetCTB(pPicoEvent->CTB());
  pFlowEvent->SetZDCe(pPicoEvent->ZDCe());
  pFlowEvent->SetZDCw(pPicoEvent->ZDCw());
  
  int goodTracks = 0;
  // Fill FlowTracks
  for (Int_t nt=0; nt<pPicoEvent->GetNtrack(); nt++) {
    StFlowPicoTrack* pPicoTrack = (StFlowPicoTrack*)pPicoEvent->Tracks()
      ->UncheckedAt(nt);
    if (pPicoTrack && StFlowCutTrack::CheckTrack(pPicoTrack)) {
      // Instantiate new StFlowTrack
      StFlowTrack* pFlowTrack = new StFlowTrack;
      if (!pFlowTrack) return kFALSE;
      pFlowTrack->SetPt(pPicoTrack->Pt());
      pFlowTrack->SetPhi(pPicoTrack->Phi());
      pFlowTrack->SetEta(pPicoTrack->Eta());
      pFlowTrack->SetDedx(pPicoTrack->Dedx());
      pFlowTrack->SetCharge(pPicoTrack->Charge());
      pFlowTrack->SetDca(pPicoTrack->Dca());
      pFlowTrack->SetDcaGlobal(pPicoTrack->DcaGlobal());
      pFlowTrack->SetChi2(pPicoTrack->Chi2());
      pFlowTrack->SetFitPts(pPicoTrack->FitPts());
      pFlowTrack->SetMaxPts(pPicoTrack->MaxPts());
      if (pPicoTrack->Charge() < 0) {
	pFlowTrack->SetPidPiMinus(pPicoTrack->PidPion());
	pFlowTrack->SetPidAntiProton(pPicoTrack->PidProton());
	pFlowTrack->SetPidKaonMinus(pPicoTrack->PidKaon());
	pFlowTrack->SetPidAntiDeuteron(pPicoTrack->PidDeuteron());
	pFlowTrack->SetPidElectron(pPicoTrack->PidElectron());
      } else {
	pFlowTrack->SetPidPiPlus(pPicoTrack->PidPion());
	pFlowTrack->SetPidProton(pPicoTrack->PidProton());
	pFlowTrack->SetPidKaonPlus(pPicoTrack->PidKaon());
	pFlowTrack->SetPidDeuteron(pPicoTrack->PidDeuteron());
	pFlowTrack->SetPidPositron(pPicoTrack->PidElectron());
      }
      
      pFlowEvent->TrackCollection()->push_back(pFlowTrack);
      goodTracks++;
    }
  }
  
  return kTRUE;
}

//-----------------------------------------------------------------------

Bool_t StFlowMaker::FillFromPicoVersion2DST(StFlowPicoEvent* pPicoEvent) {
  // Make StFlowEvent from StFlowPicoEvent
  
  if (Debug()) gMessMgr->Info() << "FlowMaker: FillFromPicoVersion2DST()" << endm;

  pFlowEvent->SetEventID(pPicoEvent->EventID());
  UInt_t origMult = pPicoEvent->OrigMult();
  pFlowEvent->SetOrigMult(origMult);
  PR(origMult);
  pFlowEvent->SetVertexPos( StThreeVectorF(pPicoEvent->VertexX(),
					   pPicoEvent->VertexY(),
					   pPicoEvent->VertexZ()) );
  pFlowEvent->SetMultEta(pPicoEvent   ->MultEta());
  pFlowEvent->SetCentrality(pPicoEvent->MultEta());
  pFlowEvent->SetRunID(pPicoEvent     ->RunID());
  pFlowEvent->SetCTB(pPicoEvent       ->CTB());
  pFlowEvent->SetZDCe(pPicoEvent      ->ZDCe());
  pFlowEvent->SetZDCw(pPicoEvent      ->ZDCw());
  
  int goodTracks = 0;
  // Fill FlowTracks
  for (Int_t nt=0; nt<pPicoEvent->GetNtrack(); nt++) {
    StFlowPicoTrack* pPicoTrack = (StFlowPicoTrack*)pPicoEvent->Tracks()
      ->UncheckedAt(nt);
    if (pPicoTrack && StFlowCutTrack::CheckTrack(pPicoTrack)) {
      // Instantiate new StFlowTrack
      StFlowTrack* pFlowTrack = new StFlowTrack;
      if (!pFlowTrack) return kFALSE;
      pFlowTrack->SetPt(pPicoTrack->Pt());
      pFlowTrack->SetPtGlobal(pPicoTrack->PtGlobal());
      pFlowTrack->SetPhi(pPicoTrack->Phi());
      pFlowTrack->SetPhiGlobal(pPicoTrack->PhiGlobal());
      pFlowTrack->SetEta(pPicoTrack->Eta());
      pFlowTrack->SetEtaGlobal(pPicoTrack->EtaGlobal());
      pFlowTrack->SetDedx(pPicoTrack->Dedx());
      pFlowTrack->SetCharge(pPicoTrack->Charge());
      pFlowTrack->SetDca(pPicoTrack->Dca());
      pFlowTrack->SetDcaGlobal(pPicoTrack->DcaGlobal());
      pFlowTrack->SetChi2(pPicoTrack->Chi2());
      pFlowTrack->SetFitPts(pPicoTrack->FitPts());
      pFlowTrack->SetMaxPts(pPicoTrack->MaxPts());
      if (pPicoTrack->Charge() < 0) {
	pFlowTrack->SetPidPiMinus(pPicoTrack->PidPion());
	pFlowTrack->SetPidAntiProton(pPicoTrack->PidProton());
	pFlowTrack->SetPidKaonMinus(pPicoTrack->PidKaon());
	pFlowTrack->SetPidAntiDeuteron(pPicoTrack->PidDeuteron());
	pFlowTrack->SetPidElectron(pPicoTrack->PidElectron());
      } else {
	pFlowTrack->SetPidPiPlus(pPicoTrack->PidPion());
	pFlowTrack->SetPidProton(pPicoTrack->PidProton());
	pFlowTrack->SetPidKaonPlus(pPicoTrack->PidKaon());
	pFlowTrack->SetPidDeuteron(pPicoTrack->PidDeuteron());
	pFlowTrack->SetPidPositron(pPicoTrack->PidElectron());
      }
      
      pFlowEvent->TrackCollection()->push_back(pFlowTrack);
      goodTracks++;
    }
  }
  
  return kTRUE;
}

//-----------------------------------------------------------------------

Bool_t StFlowMaker::FillFromPicoVersion3DST(StFlowPicoEvent* pPicoEvent) {
  // Make StFlowEvent from StFlowPicoEvent
  
  if (Debug()) gMessMgr->Info() << "FlowMaker: FillFromPicoVersion3DST()" << endm;

  pFlowEvent->SetEventID(pPicoEvent->EventID());
  UInt_t origMult = pPicoEvent->OrigMult();
  pFlowEvent->SetOrigMult(origMult);
  PR(origMult);
  pFlowEvent->SetVertexPos(StThreeVectorF(pPicoEvent->VertexX(),
					  pPicoEvent->VertexY(),
					  pPicoEvent->VertexZ()) );
  pFlowEvent->SetMultEta(pPicoEvent   ->MultEta());
  pFlowEvent->SetCentrality(pPicoEvent->MultEta());
  pFlowEvent->SetRunID(pPicoEvent     ->RunID());
  pFlowEvent->SetCTB(pPicoEvent       ->CTB());
  pFlowEvent->SetZDCe(pPicoEvent      ->ZDCe());
  pFlowEvent->SetZDCw(pPicoEvent      ->ZDCw());
  
  int goodTracks = 0;
  // Fill FlowTracks
  for (Int_t nt=0; nt < pPicoEvent->GetNtrack(); nt++) {
    StFlowPicoTrack* pPicoTrack = (StFlowPicoTrack*)pPicoEvent->Tracks()
      ->UncheckedAt(nt);
    if (pPicoTrack && StFlowCutTrack::CheckTrack(pPicoTrack)) {
      // Instantiate new StFlowTrack
      StFlowTrack* pFlowTrack = new StFlowTrack;
      if (!pFlowTrack) return kFALSE;
      pFlowTrack->SetPt(pPicoTrack->Pt());
      pFlowTrack->SetPtGlobal(pPicoTrack->PtGlobal());
      pFlowTrack->SetPhi(pPicoTrack->Phi());
      pFlowTrack->SetPhiGlobal(pPicoTrack->PhiGlobal());
      pFlowTrack->SetEta(pPicoTrack->Eta());
      pFlowTrack->SetEtaGlobal(pPicoTrack->EtaGlobal());
      pFlowTrack->SetDedx(pPicoTrack->Dedx());
      pFlowTrack->SetCharge(pPicoTrack->Charge());
      pFlowTrack->SetDca(pPicoTrack->Dca());
      pFlowTrack->SetDcaGlobal(pPicoTrack->DcaGlobal());
      pFlowTrack->SetChi2(pPicoTrack->Chi2());
      pFlowTrack->SetFitPts(pPicoTrack->FitPts());
      pFlowTrack->SetMaxPts(pPicoTrack->MaxPts());
      pFlowTrack->SetMostLikelihoodPID(pPicoTrack->MostLikelihoodPID()); 
      pFlowTrack->SetMostLikelihoodProb(pPicoTrack->MostLikelihoodProb());
      pFlowTrack->SetExtrapTag(pPicoTrack->ExtrapTag());
      if (pPicoTrack->Charge() < 0) {
	pFlowTrack->SetPidPiMinus(pPicoTrack->PidPion());
	pFlowTrack->SetPidAntiProton(pPicoTrack->PidProton());
	pFlowTrack->SetPidKaonMinus(pPicoTrack->PidKaon());
	pFlowTrack->SetPidAntiDeuteron(pPicoTrack->PidDeuteron());
	pFlowTrack->SetPidElectron(pPicoTrack->PidElectron());
      } else {
	pFlowTrack->SetPidPiPlus(pPicoTrack->PidPion());
	pFlowTrack->SetPidProton(pPicoTrack->PidProton());
	pFlowTrack->SetPidKaonPlus(pPicoTrack->PidKaon());
	pFlowTrack->SetPidDeuteron(pPicoTrack->PidDeuteron());
	pFlowTrack->SetPidPositron(pPicoTrack->PidElectron());
      }

      pFlowEvent->TrackCollection()->push_back(pFlowTrack);
      goodTracks++;
    }
  }
  
  return kTRUE;
}

//-----------------------------------------------------------------------

Bool_t StFlowMaker::FillFromPicoVersion4DST(StFlowPicoEvent* pPicoEvent) {
  // Make StFlowEvent from StFlowPicoEvent
  
  if (Debug()) gMessMgr->Info() << "FlowMaker: FillFromPicoVersion4DST()" << endm;

  pFlowEvent->SetEventID(pPicoEvent->EventID());
  UInt_t origMult = pPicoEvent->OrigMult();
  pFlowEvent->SetOrigMult(origMult);
  PR(origMult);
  pFlowEvent->SetUncorrNegMult(pPicoEvent->UncorrNegMult());
  pFlowEvent->SetUncorrPosMult(pPicoEvent->UncorrPosMult());
  pFlowEvent->SetVertexPos(StThreeVectorF(pPicoEvent->VertexX(),
					  pPicoEvent->VertexY(),
					  pPicoEvent->VertexZ()) );
  pFlowEvent->SetMultEta(pPicoEvent->MultEta());
  pFlowEvent->SetCentrality(pPicoEvent->MultEta());
  pFlowEvent->SetRunID(pPicoEvent->RunID());
  pFlowEvent->SetL0TriggerWord(pPicoEvent->L0TriggerWord());

  pFlowEvent->SetCenterOfMassEnergy(pPicoEvent->CenterOfMassEnergy());
  pFlowEvent->SetBeamMassNumberEast(pPicoEvent->BeamMassNumberEast());
  pFlowEvent->SetBeamMassNumberWest(pPicoEvent->BeamMassNumberWest());

  pFlowEvent->SetCTB(pPicoEvent->CTB());
  pFlowEvent->SetZDCe(pPicoEvent->ZDCe());
  pFlowEvent->SetZDCw(pPicoEvent->ZDCw());
  
  int goodTracks = 0;
  // Fill FlowTracks
  for (Int_t nt=0; nt < pPicoEvent->GetNtrack(); nt++) {
    StFlowPicoTrack* pPicoTrack = (StFlowPicoTrack*)pPicoEvent->Tracks()
      ->UncheckedAt(nt);
    if (pPicoTrack && StFlowCutTrack::CheckTrack(pPicoTrack)) {
      // Instantiate new StFlowTrack
      StFlowTrack* pFlowTrack = new StFlowTrack;
      if (!pFlowTrack) return kFALSE;
      pFlowTrack->SetPt(pPicoTrack->Pt());
      pFlowTrack->SetPtGlobal(pPicoTrack->PtGlobal());
      pFlowTrack->SetPhi(pPicoTrack->Phi());
      pFlowTrack->SetPhiGlobal(pPicoTrack->PhiGlobal());
      pFlowTrack->SetEta(pPicoTrack->Eta());
      pFlowTrack->SetEtaGlobal(pPicoTrack->EtaGlobal());
      pFlowTrack->SetDedx(pPicoTrack->Dedx());
      pFlowTrack->SetCharge(pPicoTrack->Charge());
      pFlowTrack->SetDcaSigned(pPicoTrack->DcaSigned());
      pFlowTrack->SetDca(pPicoTrack->Dca());
      pFlowTrack->SetDcaGlobal(pPicoTrack->DcaGlobal());
      pFlowTrack->SetChi2(pPicoTrack->Chi2());
      pFlowTrack->SetFitPts(pPicoTrack->FitPts());
      pFlowTrack->SetMaxPts(pPicoTrack->MaxPts());
      pFlowTrack->SetNhits(pPicoTrack->Nhits());
      pFlowTrack->SetNdedxPts(pPicoTrack->NdedxPts());
      pFlowTrack->SetDcaGlobal3(StThreeVectorD(pPicoTrack->DcaGlobalX(),
					       pPicoTrack->DcaGlobalY(),
					       pPicoTrack->DcaGlobalZ()) );
      pFlowTrack->SetTrackLength(pPicoTrack->TrackLength());
      pFlowTrack->SetMostLikelihoodPID(pPicoTrack->MostLikelihoodPID()); 
      pFlowTrack->SetMostLikelihoodProb(pPicoTrack->MostLikelihoodProb());
      pFlowTrack->SetExtrapTag(pPicoTrack->ExtrapTag());
      pFlowTrack->SetElectronPositronProb(pPicoTrack->ElectronPositronProb()); 
      pFlowTrack->SetPionPlusMinusProb(pPicoTrack->PionPlusMinusProb()); 
      pFlowTrack->SetKaonPlusMinusProb(pPicoTrack->KaonPlusMinusProb()); 
      pFlowTrack->SetProtonPbarProb(pPicoTrack->ProtonPbarProb()); 
      if (pPicoTrack->Charge() < 0) {
	pFlowTrack->SetPidPiMinus(pPicoTrack->PidPion());
	pFlowTrack->SetPidAntiProton(pPicoTrack->PidProton());
	pFlowTrack->SetPidKaonMinus(pPicoTrack->PidKaon());
	pFlowTrack->SetPidAntiDeuteron(pPicoTrack->PidDeuteron());
	pFlowTrack->SetPidElectron(pPicoTrack->PidElectron());
      } else {
	pFlowTrack->SetPidPiPlus(pPicoTrack->PidPion());
	pFlowTrack->SetPidProton(pPicoTrack->PidProton());
	pFlowTrack->SetPidKaonPlus(pPicoTrack->PidKaon());
	pFlowTrack->SetPidDeuteron(pPicoTrack->PidDeuteron());
	pFlowTrack->SetPidPositron(pPicoTrack->PidElectron());
      }

      if (pPicoTrack->TopologyMap0() || pPicoTrack->TopologyMap1()) {
	// topology map found
	pFlowTrack->SetTopologyMap(StTrackTopologyMap(pPicoTrack->TopologyMap0(),
						      pPicoTrack->TopologyMap1()) );
      }

      pFlowEvent->TrackCollection()->push_back(pFlowTrack);
      goodTracks++;
    }
  }
  
  return kTRUE;
}

//-----------------------------------------------------------------------

Bool_t StFlowMaker::FillFromPicoVersion5DST(StFlowPicoEvent* pPicoEvent) {
  // Make StFlowEvent from StFlowPicoEvent
  
  if (Debug()) gMessMgr->Info() << "FlowMaker: FillFromPicoVersion5DST()" << endm;

  pFlowEvent->SetEventID(pPicoEvent->EventID());
  UInt_t origMult = pPicoEvent->OrigMult();
  pFlowEvent->SetOrigMult(origMult);
  PR(origMult);
  pFlowEvent->SetUncorrNegMult(pPicoEvent->UncorrNegMult());
  pFlowEvent->SetUncorrPosMult(pPicoEvent->UncorrPosMult());
  pFlowEvent->SetVertexPos(StThreeVectorF(pPicoEvent->VertexX(),
					  pPicoEvent->VertexY(),
					  pPicoEvent->VertexZ()) );
  pFlowEvent->SetMultEta(pPicoEvent->MultEta());
  pFlowEvent->SetCentrality(pPicoEvent->MultEta());
  pFlowEvent->SetRunID(pPicoEvent->RunID());
  pFlowEvent->SetL0TriggerWord(pPicoEvent->L0TriggerWord());

  pFlowEvent->SetCenterOfMassEnergy(pPicoEvent->CenterOfMassEnergy());
  pFlowEvent->SetMagneticField(pPicoEvent->MagneticField());
  pFlowEvent->SetBeamMassNumberEast(pPicoEvent->BeamMassNumberEast());
  pFlowEvent->SetBeamMassNumberWest(pPicoEvent->BeamMassNumberWest());

  pFlowEvent->SetCTB(pPicoEvent->CTB());
  pFlowEvent->SetZDCe(pPicoEvent->ZDCe());
  pFlowEvent->SetZDCw(pPicoEvent->ZDCw());
  
  int goodTracks = 0;
  // Fill FlowTracks
  for (Int_t nt=0; nt < pPicoEvent->GetNtrack(); nt++) {
    StFlowPicoTrack* pPicoTrack = (StFlowPicoTrack*)pPicoEvent->Tracks()
      ->UncheckedAt(nt);
    if (pPicoTrack && StFlowCutTrack::CheckTrack(pPicoTrack)) {
      // Instantiate new StFlowTrack
      StFlowTrack* pFlowTrack = new StFlowTrack;
      if (!pFlowTrack) return kFALSE;
      pFlowTrack->SetPt(pPicoTrack->Pt());
      pFlowTrack->SetPtGlobal(pPicoTrack->PtGlobal());
      pFlowTrack->SetPhi(pPicoTrack->Phi());
      pFlowTrack->SetPhiGlobal(pPicoTrack->PhiGlobal());
      pFlowTrack->SetEta(pPicoTrack->Eta());
      pFlowTrack->SetEtaGlobal(pPicoTrack->EtaGlobal());
      pFlowTrack->SetDedx(pPicoTrack->Dedx());
      pFlowTrack->SetCharge(pPicoTrack->Charge());
      pFlowTrack->SetDcaSigned(pPicoTrack->DcaSigned());
      pFlowTrack->SetDca(pPicoTrack->Dca());
      pFlowTrack->SetDcaGlobal(pPicoTrack->DcaGlobal());
      pFlowTrack->SetChi2(pPicoTrack->Chi2());
      pFlowTrack->SetFitPts(pPicoTrack->FitPts());
      pFlowTrack->SetMaxPts(pPicoTrack->MaxPts());
      pFlowTrack->SetNhits(pPicoTrack->Nhits());
      pFlowTrack->SetNdedxPts(pPicoTrack->NdedxPts());
      pFlowTrack->SetDcaGlobal3(StThreeVectorD(pPicoTrack->DcaGlobalX(),
					       pPicoTrack->DcaGlobalY(),
					       pPicoTrack->DcaGlobalZ()) );
      pFlowTrack->SetTrackLength(pPicoTrack->TrackLength());
      pFlowTrack->SetMostLikelihoodPID(pPicoTrack->MostLikelihoodPID()); 
      pFlowTrack->SetMostLikelihoodProb(pPicoTrack->MostLikelihoodProb());
      pFlowTrack->SetExtrapTag(pPicoTrack->ExtrapTag());
      pFlowTrack->SetElectronPositronProb(pPicoTrack->ElectronPositronProb()); 
      pFlowTrack->SetPionPlusMinusProb(pPicoTrack->PionPlusMinusProb()); 
      pFlowTrack->SetKaonPlusMinusProb(pPicoTrack->KaonPlusMinusProb()); 
      pFlowTrack->SetProtonPbarProb(pPicoTrack->ProtonPbarProb()); 
      if (pPicoTrack->Charge() < 0) {
	pFlowTrack->SetPidPiMinus(pPicoTrack->PidPion());
	pFlowTrack->SetPidAntiProton(pPicoTrack->PidProton());
	pFlowTrack->SetPidKaonMinus(pPicoTrack->PidKaon());
	pFlowTrack->SetPidAntiDeuteron(pPicoTrack->PidDeuteron());
	pFlowTrack->SetPidElectron(pPicoTrack->PidElectron());
      } else {
	pFlowTrack->SetPidPiPlus(pPicoTrack->PidPion());
	pFlowTrack->SetPidProton(pPicoTrack->PidProton());
	pFlowTrack->SetPidKaonPlus(pPicoTrack->PidKaon());
	pFlowTrack->SetPidDeuteron(pPicoTrack->PidDeuteron());
	pFlowTrack->SetPidPositron(pPicoTrack->PidElectron());
      }

      if (pPicoTrack->TopologyMap0() || pPicoTrack->TopologyMap1()) {
	// topology map found
	pFlowTrack->SetTopologyMap(StTrackTopologyMap(pPicoTrack->TopologyMap0(),
						      pPicoTrack->TopologyMap1()) );
      }

      pFlowEvent->TrackCollection()->push_back(pFlowTrack);
      goodTracks++;
    }
  }
  
  return kTRUE;
}

//-----------------------------------------------------------------------

void StFlowMaker::PrintSubeventMults() {
  // Used for testing

  if (Debug()) gMessMgr->Info() << "FlowMaker: PrintSubeventMults()" << endm;
  
  int j, k, n;
  
  pFlowSelect->SetSubevent(-1);
  for (j = 0; j < Flow::nHars; j++) {
    pFlowSelect->SetHarmonic(j);
    for (k = 0; k < Flow::nSels; k++) {
      pFlowSelect->SetSelection(k);
      cout << "j,k= " << j << k << " : " << pFlowEvent->Mult(pFlowSelect) 
	   << endl;
    }
  }
  
  for (j = 0; j < Flow::nHars; j++) {
    pFlowSelect->SetHarmonic(j);
    for (k = 0; k <Flow:: nSels; k++) {
      pFlowSelect->SetSelection(k);
      for (n = 0; n < Flow::nSubs+1; n++) {
	pFlowSelect->SetSubevent(n);
	cout << "j,k,n= " << j << k << n << " : " << 
	  pFlowEvent->Mult(pFlowSelect) << endl;
      }
    }
  }
  
}

//-----------------------------------------------------------------------

Int_t StFlowMaker::InitPicoEventWrite() {
  if (Debug()) gMessMgr->Info() << "FlowMaker: InitPicoEventWrite()" << endm;
  
  Int_t split   = 99;      // by default split Event into sub branches
  Int_t comp    = 1;       // by default file is compressed
  Int_t bufsize = 256000;
  if (split) bufsize /= 16;
  
  // creat a Picoevent and an output file
  pPicoEvent = new StFlowPicoEvent();   

  TString* filestring = new TString(mPicoEventDir);
  filestring->Append(mEventFileName);
  filestring->Append(".flowpicoevent.root");
  pPicoDST = new TFile(filestring->Data(),"RECREATE","Flow Pico DST file");
  if (!pPicoDST) {
    cout << "##### FlowMaker: Warning: no PicoEvents file = " 
	 << filestring->Data() << endl;
    return kStFatal;
  }
  pPicoDST->SetCompressionLevel(comp);
  gMessMgr->Info() << "##### FlowMaker: PicoEvents file = " 
		   << filestring->Data() << endm;
  
  // Create a ROOT Tree and one superbranch
  pFlowTree = new TTree("FlowTree", "Flow Pico Tree");
  if (!pFlowTree) {
    cout << "##### FlowMaker: Warning: No FlowPicoTree" << endl;
    return kStFatal;
  }
  
  pFlowTree->SetAutoSave(1000000);  // autosave when 1 Mbyte written
  pFlowTree->Branch("pPicoEvent", "StFlowPicoEvent", &pPicoEvent,
		    bufsize, split);

  delete filestring;
  
  return kStOK;
}

//-----------------------------------------------------------------------

Int_t StFlowMaker::InitPicoEventRead() {
  if (Debug()) gMessMgr->Info() << "FlowMaker: InitPicoEventRead()" << endm;
  
  pPicoEvent = new StFlowPicoEvent(); 
  pPicoChain = new TChain("FlowTree");
  
  for (Int_t ilist = 0;  ilist < pPicoFileList->GetNBundles(); ilist++) {
    pPicoFileList->GetNextBundle();
    if (Debug()) gMessMgr->Info() << " doFlowEvents -  input fileList = " 
				  << pPicoFileList->GetFileName(0) << endm;
    pPicoChain->Add(pPicoFileList->GetFileName(0));
  }
  
  pPicoChain->SetBranchAddress("pPicoEvent", &pPicoEvent);
  
  Int_t nEntries = (Int_t)pPicoChain->GetEntries(); 
  gMessMgr->Info() << "##### FlowMaker: events in Pico-DST file = "
		   << nEntries << endm;
  
  mPicoEventCounter = 0;
  
  return kStOK;
}

//-----------------------------------------------------------------------

Float_t StFlowMaker::CalcDcaSigned(const StThreeVectorF vertex, 
				   const StTrack* gTrack) {
  // find the distance between the center of the circle and vertex.
  // if the radius of curvature > distance, then call it positive
  // Bum Choi

  double xCenter = gTrack->geometry()->helix().xcenter();
  double yCenter = gTrack->geometry()->helix().ycenter();
  double radius = 1.0/gTrack->geometry()->helix().curvature();

  double dPosCenter = sqrt( (vertex.x() - xCenter) * (vertex.x() - xCenter) +
			    (vertex.y() - yCenter) * (vertex.y() - yCenter));

  return (Float_t)(radius - dPosCenter);
}

//////////////////////////////////////////////////////////////////////
//
// $Log: StFlowMaker.cxx,v $
// Revision 1.70  2002/02/05 07:19:26  snelling
// Quick fix for problems with backward compatibility (changed ClassDef back)
//
// Revision 1.69  2002/02/01 23:06:34  snelling
// Added entries for header information in flowPico (not everthing is available yet)
//
// Revision 1.68  2002/01/14 23:39:34  posk
// Moved print commands to Finish().
//
// Revision 1.67  2002/01/11 19:08:43  posk
// Restored SetDca for backwards compatability.
//
// Revision 1.66  2002/01/07 23:32:01  posk
// Added return to prevent seg. fault when event skipped.
//
// Revision 1.65  2002/01/07 21:42:49  posk
// Protection for seg. fault when no particles.
//
// Revision 1.64  2001/12/18 19:22:15  posk
// "proton" and "antiproton" changed to "pr+" and "pr-".
// Compiles on Solaris.
//
// Revision 1.63  2001/12/11 21:33:55  posk
// Went from one to four sets of histograms for making the event plane isotropic.
// StFlowEvent::PhiWeight() has changed arguments and return value.
// The ptWgt saturates above 2 GeV/c.
//
// Revision 1.62  2001/11/09 21:10:45  posk
// Switched from CERNLIB to TMath. Little q is now normalized.
//
// Revision 1.61  2001/08/01 19:39:40  snelling
// Added the trigger word
//
// Revision 1.60  2001/07/27 20:33:40  snelling
// switched from StRun to StEvtHddr.
//
// Revision 1.59  2001/07/27 01:26:19  snelling
// Added and changed variables for picoEvent. Changed trackCut class to StTrack
//
// Revision 1.58  2001/07/24 22:29:17  snelling
// First attempt to get a standard root pico file again, added variables
//
// Revision 1.57  2001/07/02 20:19:12  posk
// Moved call to SetPids() above call to SetSelections().
//
// Revision 1.56  2001/06/06 13:02:58  rcwells
// Added SetPtWgt(Bool_t) function to StFlowEvent
//
// Revision 1.55  2001/06/04 18:57:05  rcwells
// Adding filling from HbtEvents
//
// Revision 1.54  2001/05/23 18:11:14  posk
// Removed SetPids().
//
// Revision 1.53  2001/05/22 20:17:34  posk
// Now can do pseudorapidity subevents.
//
// Revision 1.52  2001/04/25 17:46:33  perev
// HPcorrs
//
// Revision 1.51  2000/12/29 19:40:39  snelling
// Used the new calibration file for PID
//
// Revision 1.50  2000/12/12 20:22:05  posk
// Put log comments at end of files.
// Deleted persistent StFlowEvent (old micro DST).
//
// Revision 1.49  2000/12/10 02:01:13  oldi
// A new member (StTrackTopologyMap mTopology) was added to StFlowPicoTrack.
// The evaluation of either a track originates from the FTPC or not is
// unambiguous now. The evaluation itself is easily extendible for other
// detectors (e.g. SVT+TPC). Old flowpicoevent.root files are treated as if
// they contain TPC tracks only (backward compatibility).
//
// Revision 1.48  2000/12/08 17:03:38  oldi
// Phi weights for both FTPCs included.
//
// Revision 1.47  2000/12/06 15:38:46  oldi
// Including FTPC.
//
// Revision 1.46  2000/11/30 16:40:21  snelling
// Protection against loading probability pid caused it not to work anymore
// therefore protection removed again
//
// Revision 1.45  2000/11/07 02:36:41  snelling
// Do not init prob pid when not used
//
//////////////////////////////////////////////////////////////////////
