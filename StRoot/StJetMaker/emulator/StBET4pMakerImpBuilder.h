// -*- mode: c++;-*-
// $Id: StBET4pMakerImpBuilder.h,v 1.3 2008/08/02 22:43:31 tai Exp $
// Copyright (C) 2008 Tai Sakuma <sakuma@bnl.gov>
#ifndef STBET4PMAKERIMPBUILDER_H
#define STBET4PMAKERIMPBUILDER_H

class StMuDstMaker;
class StBET4pMakerImp;
class StjTreeEntryMaker;

namespace StSpinJet {


class StBET4pMakerImpBuilder { 

public:
  StBET4pMakerImpBuilder() { }
  virtual ~StBET4pMakerImpBuilder() { }

  StBET4pMakerImp* build(bool useTPC = true, bool useBEMC = true, bool useEEMC = false, bool use2003Cuts = false, bool use2005Cuts = false, bool use2006Cuts = false, StMuDstMaker* maker = 0, bool doTowerSwapFix = true);
  StBET4pMakerImp* build(bool useTPC = true, bool useBEMC = true, bool useEEMC = false, bool use2003Cuts = false, bool use2005Cuts = false, bool use2006Cuts = false, StjTreeEntryMaker* maker = 0);


private:

};

}

#endif // STBET4PMAKERIMPBUILDER_H
