///////////////////////////////////////////////////////////////////////////////
//
// $Id: StFlowPhiWgtMaker.h,v 1.3 2003/09/10 19:47:15 perev Exp $
//
// Authors: Art Poskanzer and Jamie Dunlop, May 2003
//
///////////////////////////////////////////////////////////////////////////////
//
// Description:  Maker to produce PhiWgt files
//
///////////////////////////////////////////////////////////////////////////////

#ifndef StFlowPhiWgtMaker_H
#define StFlowPhiWgtMaker_H
#include <Stiostream.h>
#include "StMaker.h"
#include "StFlowMaker/StFlowConstants.h"
#include "TString.h"
class StFlowEvent;
class StFlowSelection;
class TFile;
class TH1D;

class StFlowPhiWgtMaker : public StMaker {
  //  Makes histograms for PhiWgt and writes them out.
  //  It reads particle quantities from StFlowEvent.
 
public:

  /// Constructor
           StFlowPhiWgtMaker(const Char_t* name="FlowPhiWgt");

  virtual  ~StFlowPhiWgtMaker();

  Int_t    Init();
  Int_t    Make();
  Int_t    Finish();

  virtual  const char *GetCVS() const {static const char cvs[]=
    "Tag $Name:  $ $Id: StFlowPhiWgtMaker.h,v 1.3 2003/09/10 19:47:15 perev Exp $ built "__DATE__" "__TIME__ ;
    return cvs;}

private:

  static const int nCens = 10;

  TFile* phiWgtFile[nCens];

  void FillParticleHistograms();

  StFlowEvent*     pFlowEvent;  //! pointer to StFlowEvent
  StFlowSelection* pFlowSelect; //! selection object

  // for each harmonic, each selection, and each centrality
  struct histHars {
    TH1D*       mHistPhiFarEast;
    TH1D*       mHistPhiEast;
    TH1D*       mHistPhiWest;
    TH1D*       mHistPhiFarWest;
    TH1D*       mHistPhiFtpcFarEast;
    TH1D*       mHistPhiFtpcEast;
    TH1D*       mHistPhiFtpcWest;
    TH1D*       mHistPhiFtpcFarWest;
    TH1D*       mHistPhiWgtFarEast;
    TH1D*       mHistPhiWgtEast;
    TH1D*       mHistPhiWgtWest;
    TH1D*       mHistPhiWgtFarWest;
    TH1D*       mHistPhiWgtFtpcFarEast;
    TH1D*       mHistPhiWgtFtpcEast;
    TH1D*       mHistPhiWgtFtpcWest;
    TH1D*       mHistPhiWgtFtpcFarWest;
    };

  struct histCens;	
  friend struct histCens;
  struct histCens {
    struct histHars histHar[Flow::nHars];
  };

  struct hists;	
  friend struct hists;
  struct hists {
    struct histCens histCen[nCens];
  };
  struct hists hist[Flow::nSels]; //!

  TString      MakerName;

  ClassDef(StFlowPhiWgtMaker,0)              // macro for rootcint
};

#endif

///////////////////////////////////////////////////////////////////////////////
//
// $Log: StFlowPhiWgtMaker.h,v $
// Revision 1.3  2003/09/10 19:47:15  perev
// ansi corrs
//
// Revision 1.2  2003/09/02 17:58:11  perev
// gcc 3.2 updates + WarnOff
//
// Revision 1.1  2003/05/16 20:44:51  posk
// First commit of StFlowPhiWgtMaker
//
//
/////////////////////////////////////////////////////////////////////////////////
