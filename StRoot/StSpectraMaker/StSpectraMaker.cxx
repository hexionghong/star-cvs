// $Id: StSpectraMaker.cxx,v 1.3 1999/11/08 17:07:32 ogilvie Exp $
// $Log: StSpectraMaker.cxx,v $
// Revision 1.3  1999/11/08 17:07:32  ogilvie
// *** empty log message ***
//
// Revision 1.2  1999/11/05 18:58:49  ogilvie
// general tidy up following Mike Lisa's review. List of analyses conntrolled via
// analysis.dat, rather than hardcoded into StSpectraMaker.cxx
//
// Revision 1.1  1999/11/03 21:22:41  ogilvie
// initial version
//
//
///////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////
#include "StSpectraMaker.h"
#include "StChain.h"
#include "StRun.h"
#include "StEvent.h"
#include "StMessMgr.h"
#include "StEfficiency.h"
#include <vector>

static const char rcsid[] = "$Id: StSpectraMaker.cxx,v 1.3 1999/11/08 17:07:32 ogilvie Exp $";

StSpectraMaker::StSpectraMaker(const Char_t *name) : StMaker(name) {
}

StSpectraMaker::~StSpectraMaker() {
}

Int_t StSpectraMaker::Init() {

  mOutput = new TFile("deviant.root","RECREATE");
  //
  // create the analyses that are stored in the file analysis.dat, 
  // 
  ifstream from("StRoot/StSpectraMaker/analysis.dat");
  while (!from.eof()) {
    string particleName;
    from >> particleName;
    string analysisTitle;
    from >> analysisTitle; 
    efficiencyType efficType;
    from >> efficType; 
    char* efficiencyFile = new char[100];
    from >> efficiencyFile; 
    StEfficiency effic(efficType,efficiencyFile);
    //
    // to do, add in particle Definition to efficiency constructor
    //
    StParticleDefinition* particle = 
   	StParticleTable::instance()->findParticle(particleName);
    effic.setParticle(particleName);

    string comment;
    float lYbin, uYbin;
    from >> lYbin >> uYbin >> comment;
    int nYbin ;
    from >> nYbin >> comment;
    float lMtbin = particle->mass();
    float mtRange, uMtbin;
    from >> mtRange >> comment;
    uMtbin = lMtbin + mtRange;
    int nMtbin;
    from >> nMtbin >> comment;

    StTpcDeviantSpectraAnalysis* anal = new StTpcDeviantSpectraAnalysis;
    anal->setParticle(particleName);
    anal->setTitle(analysisTitle);
    anal->setEfficiency(effic);
   
    anal->setYAxis(lYbin,uYbin,nYbin); 
    anal->setMtAxis(lMtbin,uMtbin,nMtbin); 

    mSpectraAnalysisContainer.push_back(anal);
  }
  from.close();
 
  //
  // set common characteristics of spectra, e.g. size of bins, do this via a file?
  // loop through the analyses and load into the private data members the
  // spectra characteristics
  // 

  StTpcDeviantSpectraAnalysis* analysis = new StTpcDeviantSpectraAnalysis;
  vector<StTpcDeviantSpectraAnalysis*>::const_iterator analysisIter;

  for (analysisIter = mSpectraAnalysisContainer.begin();
	 analysisIter != mSpectraAnalysisContainer.end();
	 analysisIter++) {
    analysis = *analysisIter;
    analysis->bookHistograms();
  }
 return StMaker::Init();
}

Int_t StSpectraMaker::Make() {
  StEvent* mEvent;
  mEvent = (StEvent *) GetInputDS("StEvent");
  if (! mEvent) return kStOK; // If no event, we're done
  StEvent& ev = *mEvent;
  //
  // for each analysis, call fill histograms
  //
  StTpcDeviantSpectraAnalysis* analysis = new StTpcDeviantSpectraAnalysis;
  vector<StTpcDeviantSpectraAnalysis*>::const_iterator analysisIter;

  for (analysisIter = mSpectraAnalysisContainer.begin();
	 analysisIter != mSpectraAnalysisContainer.end();
	 analysisIter++) {
    analysis = *analysisIter;
    analysis->fillHistograms(ev);
  }  
  return kStOK;
}

void StSpectraMaker::Clear(Option_t *opt) {

  StMaker::Clear();
}

Int_t StSpectraMaker::Finish() {
  
  StTpcDeviantSpectraAnalysis* analysis = new StTpcDeviantSpectraAnalysis;
  vector<StTpcDeviantSpectraAnalysis*>::const_iterator analysisIter;

  for (analysisIter = mSpectraAnalysisContainer.begin();
	 analysisIter != mSpectraAnalysisContainer.end();
	 analysisIter++) {
    analysis = *analysisIter;
    analysis->projectHistograms();
  }

  // write out histograms
  cout << "writing out histograms" << endl;

  mOutput->Write("MyKey",kSingleKey);
  cout << "written"<< endl;
  mOutput->Close();
  return kStOK;
}

ClassImp(StSpectraMaker)



