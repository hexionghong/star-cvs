// aMCatNLOHooks.h is a part of the PYTHIA event generator.
// Copyright (C) 2018 Torbjorn Sjostrand.
// PYTHIA is licenced under the GNU GPL v2 or later, see COPYING for details.
// Please respect the MCnet Guidelines, see GUIDELINES for details.

// This program is written by Stefan Prestel.
// It illustrates how to do run PYTHIA with LHEF input, allowing a
// sample-by-sample generation of
// a) Non-matched/non-merged events
// b) MLM jet-matched events (kT-MLM, shower-kT, FxFx)
// c) CKKW-L and UMEPS-merged events
// d) UNLOPS NLO merged events
// see the respective sections in the online manual for details.

#ifndef Pythia8_aMCatNLOHooks_H
#define Pythia8_aMCatNLOHooks_H

#include "Pythia8/Pythia.h"

namespace Pythia8 {

//==========================================================================

// Use userhooks to set the number of requested partons dynamically, as
// needed when running CKKW-L or UMEPS on a single input file that contains
// all parton multiplicities.

class amcnlo_unitarised_interface : public UserHooks {

public:

  // Constructor and destructor.
  amcnlo_unitarised_interface() : mergingScheme(0), normFactor(1.) {}
  amcnlo_unitarised_interface(int mergingSchemeIn)
    : mergingScheme(mergingSchemeIn), normFactor(1.) {}
 ~amcnlo_unitarised_interface() {}

  double getNormFactor(){return normFactor;}

  // Allow to set the number of partons.
  bool canVetoProcessLevel() { return true;}
  // Set the number of partons.
  bool doVetoProcessLevel( Event& process) {

    int nPartons = 0;
    normFactor = 1.;

    // Do not include resonance decay products in the counting.
    omitResonanceDecays(process);

    // Get the maximal quark flavour counted as "additional" parton.
    int nQuarksMerge = settingsPtr->mode("Merging:nQuarksMerge");

    // Dynamically set the process string.
    if ( settingsPtr->word("Merging:Process") == "guess" ) {
      string processString = "";
      // Set incoming particles.
      int beamAid = beamAPtr->id();
      int beamBid = beamBPtr->id();
      if (abs(beamAid) == 2212) processString += "p";
      if (beamAid == 11)        processString += "e-";
      if (beamAid ==-11)        processString += "e+";
      if (abs(beamBid) == 2212) processString += "p";
      if (beamBid == 11)        processString += "e-";
      if (beamBid ==-11)        processString += "e+";
      processString += ">";
      // Set outgoing particles.
      bool foundOutgoing = false;
      for(int i=0; i < int(workEvent.size()); ++i)
        if ( workEvent[i].isFinal()
          && ( workEvent[i].colType() == 0
            || workEvent[i].idAbs() > 21
            || ( workEvent[i].id() != 21
              && workEvent[i].idAbs() > nQuarksMerge) ) ) {
          foundOutgoing = true;
          ostringstream proc;
          proc << "{" << workEvent[i].name() << "," << workEvent[i].id()
               << "}";
          processString += proc.str();
        }
      // Set the process string.
      if (foundOutgoing) settingsPtr->word("Merging:Process", processString);
    }

    // Loop through event and count.
    for(int i=0; i < int(workEvent.size()); ++i)
      if ( workEvent[i].isFinal()
        && workEvent[i].colType()!= 0
        && ( workEvent[i].id() == 21 || workEvent[i].idAbs() <= nQuarksMerge))
        nPartons++;

    // Store merging scheme.
    bool isumeps  = (mergingScheme == 1);
    bool isunlops = (mergingScheme == 2);

    // Get number of requested partons.
    string nps_nlo = infoPtr->getEventAttribute("npNLO",true);
    int np_nlo     = (nps_nlo != "") ? atoi((char*)nps_nlo.c_str()) : -1;
    string nps_lo  = infoPtr->getEventAttribute("npLO",true);
    int np_lo      = (nps_lo != "") ? atoi((char*)nps_lo.c_str()) : -1;

    if ( (settingsPtr->flag("Merging:doUNLOPSTree")
       || settingsPtr->flag("Merging:doUNLOPSSubt")) && np_lo == 0)
       return true;

    if (settingsPtr->word("Merging:process").compare("pp>aj") == 0)
      nPartons -= 1;
    if (settingsPtr->word("Merging:process").compare("pp>jj") == 0)
      nPartons -= 2;

    // Set number of requested partons.
    if (np_nlo > -1){
      settingsPtr->mode("Merging:nRequested", np_nlo);
      np_lo = -1;
    } else if (np_lo > -1){
      settingsPtr->mode("Merging:nRequested", np_lo);
      np_nlo = -1;
    } else {
      settingsPtr->mode("Merging:nRequested", nPartons);
      np_nlo = -1;
      np_lo = nPartons;
    }

    // Reset the event weight to incorporate corrective factor.
    bool updateWgt = settingsPtr->flag("Merging:includeWeightInXsection");
    double norm    = (abs(infoPtr->lhaStrategy()) == 4) ? 1./1e9 : 1.;

    // Choose randomly if this event should be treated as subtraction or
    // as regular event. Put the correct settings accordingly.
    if (isunlops && np_nlo == 0 && np_lo == -1) {
      settingsPtr->flag("Merging:doUNLOPSTree", false);
      settingsPtr->flag("Merging:doUNLOPSSubt", false);
      settingsPtr->flag("Merging:doUNLOPSLoop", true);
      settingsPtr->flag("Merging:doUNLOPSSubtNLO", false);
      settingsPtr->mode("Merging:nRecluster",0);
      normFactor *= 1.;
    } else if (isunlops && np_nlo > 0 && np_lo == -1) {
      normFactor *= 2.;
      if (rndmPtr->flat() < 0.5) {
        normFactor *= -1.;
        settingsPtr->flag("Merging:doUNLOPSTree", false);
        settingsPtr->flag("Merging:doUNLOPSSubt", false);
        settingsPtr->flag("Merging:doUNLOPSLoop", false);
        settingsPtr->flag("Merging:doUNLOPSSubtNLO", true);
        settingsPtr->mode("Merging:nRecluster",1);
      } else {
        settingsPtr->flag("Merging:doUNLOPSTree", false);
        settingsPtr->flag("Merging:doUNLOPSSubt", false);
        settingsPtr->flag("Merging:doUNLOPSLoop", true);
        settingsPtr->flag("Merging:doUNLOPSSubtNLO", false);
        settingsPtr->mode("Merging:nRecluster",0);
      }
    } else if (isunlops && np_nlo == -1 && np_lo > -1) {
      normFactor *= 2.;
      if (rndmPtr->flat() < 0.5) {
        normFactor *= -1.;
        settingsPtr->flag("Merging:doUNLOPSTree", false);
        settingsPtr->flag("Merging:doUNLOPSSubt", true);
        settingsPtr->flag("Merging:doUNLOPSLoop", false);
        settingsPtr->flag("Merging:doUNLOPSSubtNLO", false);
        settingsPtr->mode("Merging:nRecluster",1);

        // Double reclustering for exclusive NLO cross sections.
        bool isnlotilde = settingsPtr->flag("Merging:doUNLOPSTilde");
        int nmaxNLO = settingsPtr->mode("Merging:nJetMaxNLO");
        if ( isnlotilde
          && nmaxNLO > 0
          && np_lo <= nmaxNLO+1
          && np_lo > 1 ){
          normFactor *= 2.;
          if (rndmPtr->flat() < 0.5)
            settingsPtr->mode("Merging:nRecluster",2);
        }
      } else {
        settingsPtr->flag("Merging:doUNLOPSTree", true);
        settingsPtr->flag("Merging:doUNLOPSSubt", false);
        settingsPtr->flag("Merging:doUNLOPSLoop", false);
        settingsPtr->flag("Merging:doUNLOPSSubtNLO", false);
        settingsPtr->mode("Merging:nRecluster",0);
      }
    } else if (isumeps && np_lo == 0) {
      settingsPtr->flag("Merging:doUMEPSTree", true);
      settingsPtr->flag("Merging:doUMEPSSubt", false);
      settingsPtr->mode("Merging:nRecluster",0);
    } else if (isumeps && np_lo > 0) {
      normFactor *= 2.;
      if (rndmPtr->flat() < 0.5) {
        normFactor *= -1.;
        settingsPtr->flag("Merging:doUMEPSTree", false);
        settingsPtr->flag("Merging:doUMEPSSubt", true);
        settingsPtr->mode("Merging:nRecluster",1);
      } else {
        settingsPtr->flag("Merging:doUMEPSTree", true);
        settingsPtr->flag("Merging:doUMEPSSubt", false);
        settingsPtr->mode("Merging:nRecluster",0);
      }
      // Reset the event weight to incorporate corrective factor.
      if ( updateWgt) {
        infoPtr->updateWeight(infoPtr->weight()*norm*normFactor);
        normFactor = 1.;
      }
    }

    // Done
    return false;
  }

private:

  int mergingScheme;
  double normFactor;
};

//==========================================================================

} // end namespace Pythia8

#endif // end Pythia8_aMCatNLOHooks_H
