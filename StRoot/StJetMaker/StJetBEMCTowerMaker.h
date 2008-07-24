// -*- mode: c++;-*-
// $Id: StJetBEMCTowerMaker.h,v 1.7 2008/07/24 20:57:03 tai Exp $
#ifndef STJETBEMCTOWERMAKER_HH
#define STJETBEMCTOWERMAKER_HH

#include "StMaker.h"
#include <Rtypes.h>

class StJetTowerEnergyListWriter;

class TDirectory;
class TTree;

class StMuDstMaker;

namespace StSpinJet {
  class StJetBEMC;
  class StJetBEMCEnergyCut;
}

class StJetBEMCTowerMaker : public StMaker {

public:

  StJetBEMCTowerMaker(const Char_t *name, TDirectory* file, StMuDstMaker* uDstMaker);
  virtual ~StJetBEMCTowerMaker() { }

  Int_t Init();
  Int_t Make();
  Int_t Finish();
    
  const char* GetCVS() const
  {static const char cvs[]="Tag $Name:  $ $Id: StJetBEMCTowerMaker.h,v 1.7 2008/07/24 20:57:03 tai Exp $ built "__DATE__" "__TIME__; return cvs;}

private:

  TDirectory* _file;

  StMuDstMaker* _uDstMaker;

  StSpinJet::StJetBEMC* _bemc;
  StSpinJet::StJetBEMCEnergyCut* _bemcCut;

  StJetTowerEnergyListWriter* _writer;

  ClassDef(StJetBEMCTowerMaker, 0)

};

#endif // STJETBEMCTOWERMAKER_HH
