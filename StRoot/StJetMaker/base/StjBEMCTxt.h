// -*- mode: c++;-*-
// $Id: StjBEMCTxt.h,v 1.2 2008/08/02 19:22:42 tai Exp $
// Copyright (C) 2008 Tai Sakuma <sakuma@bnl.gov>
#ifndef STJETBEMCTXT_H
#define STJETBEMCTXT_H

#include "StjBEMC.h"

#include <string>
#include <fstream>

namespace StSpinJet {

class StjBEMCTxt : public StjBEMC {

public:
  StjBEMCTxt(const char* path);
  virtual ~StjBEMCTxt() { }

  StjTowerEnergyList getEnergyList();

private:

  std::ifstream _dataFile;
  long _currentEvent;
  std::string _oldLine;
  
};

}

#endif // STJETBEMCTXT_H
