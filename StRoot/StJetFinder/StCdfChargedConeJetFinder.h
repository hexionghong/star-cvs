//StCdfChargedConeJetFinder.h
//M.L. Miller (Yale Software)
//12/02

/*!
  This is an implemenation of the CDF Charged jet finder (analagous to "Simple UA1 Jet Finder" but
  with charged tracks instead of calorimeter cells.  For efficiency, we still organize the tracks
  into a grid, but we sort the grid by it's leading charged particle.
 */

#ifndef StCdfChargedConeJetFinder_HH
#define StCdfChargedConeJetFinder_HH

#include "StConeJetFinder.h"

/*!
  \class StCdfChargedConePars
  \author M.L. Miller (MIT Software)
  A simple class to encapsulate the requisite run-time parameters of the cdf-charged cone jet algorithm.
*/

///Run time pars
class StCdfChargedConePars : public StConePars
{
private:    
    ClassDef(StCdfChargedConePars,1)
	};


class StCdfChargedConeJetFinder : public StConeJetFinder
{
public:

    //cstr-dstr
    StCdfChargedConeJetFinder(const StCdfChargedConePars& pars);
    virtual ~StCdfChargedConeJetFinder();
    
    //inherited interface
    virtual void findJets(JetList& protojets);
    //virtual void clear();
    virtual void print();
    
protected:
    ///create a StCdfChargedJetEtCell object
    virtual StJetEtCell* makeCell(double etaMin, double etaMax, double phiMin, double phiMax);
    virtual bool acceptSeed(const StJetEtCell* cell);
    virtual bool acceptPair(const StJetEtCell* center, const StJetEtCell* assoc) const;
};

#endif
