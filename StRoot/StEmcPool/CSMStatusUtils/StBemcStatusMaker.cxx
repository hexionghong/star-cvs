#include "StBemcStatusMaker.h"

#include "StEnumerations.h"
#include "StEvent.h"
#include "StRunInfo.h"
#include "StTriggerIdCollection.h"
#include "StTriggerId.h"
#include "StEmcCollection.h"
#include "StEmcDetector.h"
#include "StEmcModule.h"
#include "StEmcRawHit.h"
#include "StL0Trigger.h"
#include "StMuDSTMaker/COMMON/StMuDst.h"
#include "StMuDSTMaker/COMMON/StMuEmcUtil.h"
#include "StMuDSTMaker/COMMON/StMuEvent.h"
#include "StMuDSTMaker/COMMON/StMuEmcCollection.h"
#include "StMuDSTMaker/COMMON/StMuTriggerIdCollection.h"
#include "StMuDSTMaker/COMMON/StMuEmcTowerData.h"
#include "StEEmcDbMaker/EEmcDbItem.h"
#include "StEmcADCtoEMaker/StEmcADCtoEMaker.h"

#include "StEmcUtil/geometry/StEmcGeom.h"

StBemcStatusMaker::StBemcStatusMaker(StMuDstMaker* maker) 
  : StMaker("bemcStatus"), mMuDstMaker(maker) {
  mOutputDirectory = "/tmp";
  mOutputFilePrefix = "junk";
  mOutputFile = NULL;
  eeDb = (StEEmcDbMaker*)GetMaker("eemcDb");
  if(eeDb==0) {
    cout << "EEmcDbMaker not in chain.  Add it." << endl;
//    assert(eeDb);
  }
}

Int_t StBemcStatusMaker::Init() {
  // create the output file
  string s = mOutputDirectory + "/" + mOutputFilePrefix + "cal.minirun.root";
  mOutputFile = new TFile(s.c_str(),"RECREATE");
  mOutputTree = new TTree("calinfo","Extraneous Information");
  mOutputTree->Branch("fillnum",&mFillNumber,"fillnum/F");
  mOutputTree->Branch("thedate",&mTheDate,"thedate/I");
  mOutputTree->Branch("thetime",&mTheTime,"thetime/I");
  mOutputTree->Branch("statusEEMC",mEemcStatusBits,"statusEEMC/F");
  mOutputTree->Branch("statusBEMC",mBemcStatusBits,"statusBEMC/F");
  mFirstEvent = kTRUE;
  if (!mOutputFile) return kStErr;
  return kStOk;
}

Int_t StBemcStatusMaker::Make() {
  
  StMuEvent *muEvent = mMuDstMaker->muDst()->event();
  Int_t runnumber=muEvent->eventInfo().runId();
//  Int_t otherrunnumber = GetRunNumber();

  StMuTriggerIdCollection& trgIdColl=muEvent->triggerIdCollection();
  const StTriggerId& oflTrgId=trgIdColl.nominal();
  vector<unsigned int> trgId=oflTrgId.triggerIds();
  bool isGood=false;
  uint i;
  for(i = 0; i < trgId.size() ; i++){
    if(trgId[i]==10 || trgId[i]==45010 || trgId[i]==45020) isGood=true;
  }
  if (!isGood) {
//    cout <<" wrong triggers!"<<endl;
    return kStOk;
  }

  StEvent* event = static_cast<StEvent*>(GetInputDS("StEvent"));
  if (!event) {
    cout <<" Not StEvent!"<<endl;
    return kStWarn;
  }

  StEmcCollection* emcColl = event->emcCollection();
  if (!emcColl) {
    cout <<"No emc Collection"<<endl;
    return kStOk;
  }
  StEmcDetector* bemc = emcColl->detector(kBarrelEmcTowerId);
  if (!bemc) {
    return kStOk;
  }
  
//crate corruption check
  
  for(int crate = 1; crate<=MAXCRATES; crate++) {
    StEmcCrateStatus crateStatus = bemc->crateStatus(crate);
    if (crateStatus==crateHeaderCorrupt) {
      return kStOk;  //corrupt event
    }
  }

  StMuEmcCollection* muemcColl = mMuDstMaker->muDst()->muEmcCollection();

//make histograms to be saved
    
  TH2F* bemcAdcHist = getBemcAdcHist(runnumber);
  TH2F* bemcEnergyHist = getBemcEnergyHist(runnumber);
  TH2F* eemcAdcHist = getEemcAdcHist(runnumber);
  //  TH2F* eemcEnergyHist = getEemcEnergyHist(event->runId());
  if (!bemcAdcHist || !bemcEnergyHist || !eemcAdcHist 
  //      || !eemcEnergyHist
      ) {
    cout <<"Problems with Histo"<<endl;
    return kStWarn;
  }

// loop over bemc and fill histograms

  for (UInt_t i = 1; i <= bemc->numberOfModules(); i++) {
    StEmcModule* module = bemc->module(i);
    if (module) {
      StSPtrVecEmcRawHit& hits = module->hits();
      for (StEmcRawHitIterator hit = hits.begin(); hit!=hits.end(); ++hit) {
	       Int_t id = 0;
	       StEmcGeom* geom = StEmcGeom::instance("bemc");
	       geom->getId((*hit)->module(),(*hit)->eta(),(*hit)->sub(),id);
	       bemcAdcHist->Fill(id,(*hit)->adc());
	       bemcEnergyHist->Fill(id,(*hit)->energy());
      }
    }
  } 
// loop over eemc and fill histograms
  int id, sec, sub, etabin, rawadc;
  if(muemcColl) {
    for (int i = 0; i < muemcColl->getNEndcapTowerADC(); i++) {
      muemcColl->getEndcapTowerADC(i,rawadc,sec,sub,etabin);
//now we need to find the ID for it
//the EEMC does not support the primary MuDst EEMC indexing code
//classic
//btw, id = etabin + 12*(sub-1) + 5*12*(sec-1);
      mMuDstMaker->muEmcUtil()->getEndcapId(5,sec,etabin,sub,id);
      if(id != i+1) {
        cout << "my scheme for EEMC doesn't work right" << endl;
        cout << "i is " << i << " and id is " << id << endl;
        assert(id-i+1);
      }
	    eemcAdcHist->Fill(id,rawadc);
    } 
  }
  if(mFirstEvent) {
    mTheDate = GetDate();
    mTheTime = GetTime();
    mFillNumber=muEvent->runInfo().beamFillNumber(yellow);
    for(int i=0; i<720; i++) {
      muemcColl->getEndcapTowerADC(i,rawadc,sec,sub,etabin);
//      const EEmcDbItem *x=eeDb->getTile(sec,'A'+sub-1,etabin,'T');
//      mEemcFailBits[i] = x->fail;
//      mEemcStatusBits[i] = x->stat;
    }
      
    //FIND BEMC STATUSES
    mOutputTree->Fill();
    mFirstEvent=kFALSE;
  }
  
  return kStOk;
}

Int_t StBemcStatusMaker::Finish() {
  if (mOutputFile) {
    mOutputFile->Write();
    delete mOutputFile;
    mOutputFile = NULL;
  }
  return kStOk;
}

TH2F* StBemcStatusMaker::getBemcAdcHist(Int_t runNumber) {
  Char_t name[255];
  snprintf(name,254,"bemcStatusAdc_%d",runNumber);
  mOutputFile->cd();
  TH2F* hist = dynamic_cast<TH2F*>(mOutputFile->Get(name));
  if (!hist) {
    Char_t title[255];
    snprintf(title,254,"BEMC tower adc run %d",runNumber);
    hist = new TH2F(name, title,4801,-0.5,4800.5,150,0,150);
    hist->GetXaxis()->SetTitle("tower id");
    hist->GetYaxis()->SetTitle("adc");
  }
  return hist; 
}

TH2F* StBemcStatusMaker::getBemcEnergyHist(Int_t runNumber) {
  Char_t name[255];
  snprintf(name,254,"bemcStatusEnergy_%d",runNumber);
  mOutputFile->cd();
  TH2F* hist = dynamic_cast<TH2F*>(mOutputFile->Get(name));
  if (!hist) {
    Char_t title[255];
    snprintf(title,254,"BEMC tower energy run %d",runNumber);
    hist = new TH2F(name, title,4801,-0.5,4800.5,150,0,3);
    hist->GetXaxis()->SetTitle("tower id");
    hist->GetYaxis()->SetTitle("adc");
  }
  return hist; 
}

TH2F* StBemcStatusMaker::getEemcAdcHist(Int_t runNumber) {
  Char_t name[255];
  snprintf(name,254,"eemcStatusAdc_%d",runNumber);
  mOutputFile->cd();
  TH2F* hist = dynamic_cast<TH2F*>(mOutputFile->Get(name));
  if (!hist) {
    Char_t title[255];
    snprintf(title,254,"EEMC tower adc run %d",runNumber);
    hist = new TH2F(name, title,721,-0.5,720.5,150,0,150);
    hist->GetXaxis()->SetTitle("tower id");
    hist->GetYaxis()->SetTitle("adc");
  }
  return hist; 
}

TH2F* StBemcStatusMaker::getEemcEnergyHist(Int_t runNumber) {
  Char_t name[255];
  snprintf(name,254,"eemcStatusEnergy_%d",runNumber);
  mOutputFile->cd();
  TH2F* hist = dynamic_cast<TH2F*>(mOutputFile->Get(name));
  if (!hist) {
    Char_t title[255];
    snprintf(title,254,"EEMC tower energy run %d",runNumber);
    hist = new TH2F(name, title,4801,-0.5,4800.5,150,0,3);
    hist->GetXaxis()->SetTitle("tower id");
    hist->GetYaxis()->SetTitle("adc");
  }
  return hist; 
}

ClassImp(StBemcStatusMaker)
