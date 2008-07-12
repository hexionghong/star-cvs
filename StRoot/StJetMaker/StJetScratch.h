// -*- mode: c++;-*-
// $Id: StJetScratch.h,v 1.2 2008/07/12 02:56:26 tai Exp $
#ifndef STJETSCRATCH_HH
#define STJETSCRATCH_HH

#include "StMaker.h"
#include <Rtypes.h>

class TDirectory;
class TTree;

class StJetTrgWriter;

class StMuDstMaker;

namespace StSpinJet {
  class StJetTPC;
  class StJetBEMC;
  class StJetEEMC;
}

class StJetScratch : public StMaker {

public:

  StJetScratch(const Char_t *name, TDirectory* file, StMuDstMaker* uDstMaker);
  virtual ~StJetScratch() { }

  Int_t Init();
  Int_t Make();
  Int_t Finish();
    
  const char* GetCVS() const
  {static const char cvs[]="Tag $Name:  $ $Id: StJetScratch.h,v 1.2 2008/07/12 02:56:26 tai Exp $ built "__DATE__" "__TIME__; return cvs;}

private:

  TDirectory* _file;

  StMuDstMaker* _uDstMaker;

  StSpinJet::StJetTPC*  _tpc;
  StSpinJet::StJetBEMC* _bemc;
  StSpinJet::StJetEEMC* _eemc;

  TTree* _tree;

  Int_t _runNumber;
  Int_t _eventId;
  Int_t _nTracks;
  Double_t _px[4096];
  Double_t _py[4096];
  Double_t _pz[4096];
  Short_t  _flag[4096];
  UShort_t _nHits[4096];
  Short_t  _charge[4096];
  UShort_t _nHitsPoss[4096];
  UShort_t _nHitsDedx[4096];
  UShort_t _nHitsFit[4096];
  Double_t _nSigmaPion[4096];
  Double_t _Tdca[4096];
  Double_t _dcaZ[4096];
  Double_t _dcaD[4096];
  Double_t _BField[4096];
  Double_t _bemcRadius[4096];
  Double_t _etaext[4096];
  Double_t _phiext[4096];
  Double_t _dEdx[4096];
  Int_t    _trackIndex[4096];
  Short_t  _trackId[4096];

  ClassDef(StJetScratch, 0)

};

#endif // STJETSCRATCH_HH
