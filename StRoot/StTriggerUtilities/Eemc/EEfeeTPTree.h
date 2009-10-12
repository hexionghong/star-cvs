#ifndef EEfeeTPTree_h
#define EEfeeTPTree_h
/**************************************************************
 * $Id: EEfeeTPTree.h,v 1.1 2009/10/12 18:04:27 pibero Exp $
 * Emulates functionality of  Endcap FEE TP-tree
 **************************************************************/
#include <stdlib.h> 
#include <stdio.h> 
#include <string.h> 
class EEfeeTP;

class EEfeeTPTree  {  // DSM0 tree emulators
 public:
  enum {mxTP=90, mxTxt=16};
  
 private:
  EEfeeTP *feeTP[mxTP];
  int TPmap[mxTP];
  int mxChan;// for one crate, needed for counting channels
  char name[mxTxt];
  
 public:
  
  EEfeeTPTree(char *, int nc );
  ~EEfeeTPTree();
  void  clear();
  void  compute(int *rawAdc, int *feePed,int *feeMask);
  EEfeeTP * TP(int i) { return feeTP[i]; }
  const EEfeeTP * TP(int i) const { return feeTP[i]; }
  
};

#endif

/*
 * $Log: EEfeeTPTree.h,v $
 * Revision 1.1  2009/10/12 18:04:27  pibero
 * Moved StEEmcUtil/EEdsm to StTriggerUtilities/Eemc
 *
 * Revision 1.2  2009/02/24 03:56:19  ogrebeny
 * Corrected const-ness
 *
 * Revision 1.1  2007/08/17 01:15:37  balewski
 * full blown Endcap trigger simu, by Xin
 *
 *
 **************************************************************/


