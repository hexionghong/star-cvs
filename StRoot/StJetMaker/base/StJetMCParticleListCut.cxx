// $Id: StJetMCParticleListCut.cxx,v 1.1 2008/07/22 06:36:59 tai Exp $
// Copyright (C) 2008 Tai Sakuma <sakuma@bnl.gov>
#include "StJetMCParticleListCut.h"

using namespace std;

namespace StSpinJet {

MCParticleList StJetMCParticleListCut::operator()(const MCParticleList &aList)
{
  MCParticleList ret;

  for(MCParticleList::const_iterator it = aList.begin(); it != aList.end(); ++it) {

    if(shouldNotKeep(*it)) continue;

    ret.push_back(*it);
  }

  return ret;
}


bool StJetMCParticleListCut::shouldNotKeep(const MCParticle& p4)
{
  for(CutList::iterator cut = _cutList.begin(); cut != _cutList.end(); ++cut){
    if((**cut)(p4)) return true;
  }

  return false;
}


}
