// $Id: StEEmcFastMaker.h,v 1.14 2010/08/05 21:23:45 stevens4 Exp $


/* \class StEEmcFastMaker        
\author Jan Balewski

 Fast simulator of the E-EMC tower response converts energy loss in the tower tails generated by GEANT directly to ADC using eta-dependent conversion factor. The following assumptions are made:

   <li> energy deposit in any tail is equal to the sum of contribution from all particles passing its volume. Number of fibers within the tail and type of scintillator is ignored.
   <li> all fibers transport to PMT 100% of energy deposited, except tails with a total deposit below 100 keV.
   <li> total Geant energy deposit is recorded as in StEvent
   <li> ADC=4095 * geantEnergy / samplingFrac / 60GeV / cosh(eta) is recorded as in StEvent 

<pre>
 Details of the code
-----------------------------

Decoding of E-EMC hits in StEvent
<hr>
 __ENDCAP__        
   TOWERS: det = kEndcapEmcTowerId
     sector 1-12   -->module 0-11
     subsector A-E -->submodule 0-4  
     jeta 1-12     -->eta  0-11
  
   PRE1-, PRE2-, POST-shower: all det = kEndcapEmcPreshowerId
     sector,subsector,eta : the same
     add 0, 5, or 10 to submodule for PRE1-, PRE2-, POST-shower

   SmdU : det=kEndcapSmdUStripId
   SmdV : det=kEndcapSmdVStripId
     sector 1-12    -->module 0-11
     stripID 1-288  -->eta 0-287
                    -->submodule=0 (not used)

---------------------------------------
StEEmcFastMaker::Make() {

 EEmcMCData  *evIN->readEventFromChain(this); // acquire E-EMC hit/track list

 EEevent eeveRaw; 
 evIN->write(&eeveRaw); // copy E-EMC hit/track to local TTree

 EEevent  eeveRaw.sumRawMC(eeve); //convert it to hit/tail list

  EE2ST(eeve, stevent); // add hit/tail list to StEvent

}

----------------------------------
usage in bfc.C
  

 StEEmcFastMaker *myMk=new StEEmcFastMaker;
 Char_t *after = "BbcSimulation";
 StMaker *xMk = chain->GetMaker(after);
 assert(xMk);
 chain->AddAfter(after,myMk);

</pre>
Example how to read back E-EMC data from StEvent:
www.star.bnl.gov/STAR/eemc -->How To

*/

#ifndef STAR_StEEmcFastMaker
#define STAR_StEEmcFastMaker


#ifndef StMaker_H
#include "StMaker.h"
#endif
class EEeventDst;
class StEvent;
class EEmcMCData;
class StEmcCollection;

/* Translation of StEmcCollection names:  EEMC -->BEMC

  WARN: preserve sub<16=2^4, eta<512=2^9, mod<128=2^7

 Jan
*/


class StEEmcFastMaker : public StMaker {

 public:
   
  static Float_t   getSamplingFraction();
  static Float_t  *getTowerGains();
  static Float_t   getSmdGain();///< (adc=g*de ) fixed gain for SMD
  static Float_t   getPreshowerGain();///< (adc=g*de ) fixed gain for pre/post shower

  static  Int_t getMaxAdc() { return 4095; } // [ADC channels]
  static  Int_t getMaxET() { return 60 ; } // [GeV]

 private:

  void mEE2ST(EEeventDst*, StEmcCollection* emcC); ///< TTree-->StEvent

 protected:
 public: 
  StEEmcFastMaker(const char *name="EEmcFastSim");
  virtual       ~StEEmcFastMaker();
  virtual Int_t Init();
  virtual Int_t  Make();
  virtual void Clear(Option_t *option="");
 
  void SetLocalStEvent();
  void SetEmcCollectionLocal(bool x=true){mEmcCollectionIsLocal=x;}
  void SetEmbeddingMode(){SetEmcCollectionLocal(true);}
  void UseFullTower(bool flag = true) { mUseFullTower = flag; } // always create all hits, even if ADC=0
  void UseFullPreShower(bool flag = true) { mUseFullPreShower = flag; }	// includes pre1/pre2/post
  void UseFullSmdu(bool flag = true) { mUseFullSmdu = flag; }
  void UseFullSmdv(bool flag = true) { mUseFullSmdv = flag; }
  StEmcCollection * GetLocalEmcCollection() { return mLocalStEmcCollection;}

  virtual const char *GetCVS() const {
    static const char cvs[]="Tag $Name:  $ $Id: StEEmcFastMaker.h,v 1.14 2010/08/05 21:23:45 stevens4 Exp $ built "__DATE__" "__TIME__ ; 
    return cvs;
  }
 private:

  EEmcMCData  *mevIN; 		///< decoded raw .fzd event
  EEeventDst  *meeve;    	///<  result stored in TTRee 
  float   *mfixTgain; 		///<  (adc=g*de )ideal gains for Towers

  StEmcCollection *mLocalStEmcCollection; // for special uses (embedding)
  bool mEmcCollectionIsLocal;
  bool mUseFullTower;
  bool mUseFullPreShower;
  bool mUseFullSmdu;
  bool mUseFullSmdv;
  
  ClassDef(StEEmcFastMaker,0)   
};
    
#endif


// $Log: StEEmcFastMaker.h,v $
// Revision 1.14  2010/08/05 21:23:45  stevens4
// Update sampling fraction to 4.8%
//
// Revision 1.13  2010/07/29 16:12:03  ogrebeny
// Update after the peer review
//
// Revision 1.12  2009/12/09 20:38:00  ogrebeny
// User-switchable function added to always create all hits, even if ADC=0. Requested by Pibero for the trigger simulator.
//
// Revision 1.11  2007/03/23 03:26:23  balewski
// Corretions from Victor
//
// Revision 1.10  2007/01/24 21:07:02  balewski
// 1) no cout or printf, only new Logger
// 2) EndcapMixer:
//    - no assert()
//    - locks out on first fatal error til the end of the job
//
// Revision 1.9  2007/01/12 23:57:12  jwebb
// Calculation of ideal gains moved into static member function getTowerGains()
// to allow slow simulator to access them.
//
// Revision 1.8  2006/12/12 20:29:13  balewski
// added hooks for Endcap embedding
//
// Revision 1.7  2005/06/09 20:04:23  balewski
// upgrade for embedding
//
// Revision 1.6  2005/06/03 19:20:47  balewski
// *** empty log message ***
//
// Revision 1.5  2004/05/26 21:28:37  jwebb
// o Changes to StEEmcFastMaker to provide methods to get sampling fraction,
//   gains, etc...
//
// o StMuEEmcSimuMaker is now just a shell of its former self
//
// o Added StMuEEmcSimuReMaker.  This maker takes a muDst as input, and uses
//   the database maker to "massage" the ADC response, to better simulate
//   the calorimeter as installed.  For now, it simply uses the geant
//   energy response, combined with a single sampling fraction and the
//   database gains and pedestals to come up with a new ADC response.
//
// Revision 1.4  2004/04/08 21:33:49  perev
// Leak off
//
// Revision 1.3  2003/09/10 19:47:08  perev
// ansi corrs
//
// Revision 1.2  2003/02/20 05:15:51  balewski
// *** empty log message ***
//
// Revision 1.1  2003/01/28 23:12:59  balewski
// star
//
