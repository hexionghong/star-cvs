// -*- mode: c++;-*-
// $Id: TrackToFourVec.h,v 1.1 2008/07/17 16:54:32 tai Exp $
// Copyright (C) 2008 Tai Sakuma <sakuma@bnl.gov>
#ifndef TRACKTOFOURVEC_H
#define TRACKTOFOURVEC_H

#include "FourVecList.h"

#include "TrackToTLorentzVector.h"

namespace StSpinJet {

class Track;

class TrackToFourVec {
public:
  TrackToFourVec(double mass = 0.1395700 /* pion mass as default */)
    : _track2tlorentzvector(*(new TrackToTLorentzVector(mass))) { }
  virtual ~TrackToFourVec() { delete &_track2tlorentzvector; }
  FourVec operator()(const Track& track);

private:
  TrackToTLorentzVector& _track2tlorentzvector;
};

}

#endif // TRACKTOFOURVEC_H
