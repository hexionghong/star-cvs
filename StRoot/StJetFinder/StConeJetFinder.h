// -*- mode: c++;-*-
// $Id: StConeJetFinder.h,v 1.37 2008/05/06 23:33:50 tai Exp $
#ifndef StConeJetFinder_HH
#define StConeJetFinder_HH

#include "StConeJetFinderBase.h"

class StConeJetFinder : public StConeJetFinderBase {

public:
	
  StConeJetFinder(const StConePars& pars);
  virtual ~StConeJetFinder();

  void findJets(JetList& protojets);     
	
private:

  void findJetAroundThis(StEtaPhiCell* cell);

  bool shouldNotSearchForJetAroundThis(const StEtaPhiCell* cell) const;

  void addSeedsAtMidpoint();

  StEtaPhiCell* defineMidpoint(const StEtaPhiCell& pj1, const StEtaPhiCell& pj2) ;

  void doMinimization();
	
  enum SearchResult {kLeftVolume=1, kConverged=2, kContinueSearch=3};	
  SearchResult doCountingSearch();

  bool isInTheVolume(double eta, double phi);

  bool areTheyInTheSameCell(double eta1, double phi1, double eta2, double phi2);

  StJetSpliterMerger* mMerger;

};

#endif

