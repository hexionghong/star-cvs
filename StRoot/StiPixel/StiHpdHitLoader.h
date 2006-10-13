#ifndef StiHpdHitLoader_H
#define StiHpdHitLoader_H
#include "Sti/StiHitLoader.h"

class StEvent;
class StMcEvent;
class StMcTrack;
class StiMcTrack;
class StiDetectorBuilder;
class StTpcHit;
class StMcHpdHit;

/*! \class StiHpdHitLoader
StiHpdHitLoader is a concrete class implementing the StiHitLoader abstract
interface. It is used to load hits from Star StEvent into the StiHitContainer
for Sti tracking. StEvent hits from the TPC are converted using the
StiHpdDetectorBuilder methods.
<p>
This class is substantially morphed from the class StiHitFiller
originally written by Mike Miller.
\author Claude A Pruneau (Wayne)
*/
class StiHpdHitLoader : public StiHitLoader<StEvent,StiDetectorBuilder>
{
public:
  StiHpdHitLoader();
  StiHpdHitLoader(StiHitContainer * hitContainer,
                    Factory<StiHit> * hitFactory,
                    StiDetectorBuilder * detector);
  virtual ~StiHpdHitLoader();
  virtual void loadHits(StEvent* source,
                        Filter<StiTrack> * trackFilter,
                        Filter<StiHit> * hitFilter);
    /*
  virtual void loadMcHits(StMcEvent* source,bool useMcAsRec,
                          Filter<StiTrack> * trackFilter,
                          Filter<StiHit> * hitFilter,
                          StMcTrack & stMcTrack,
                          StiMcTrack & stiMcTrack);
    */

 protected:
  // temporary hit ptr used to determine whether mc hits from a given event are
  // already loaded.
  UInt_t n;
  StMcHpdHit * saveHit;
  long evNum;
};

#endif
