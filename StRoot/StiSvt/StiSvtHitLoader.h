#ifndef StiSvtHitLoader_H
#define StiSvtHitLoader_H

#include "Sti/StiHitLoader.h"
class StEvent;
class StMcEvent;
class StiDetectorBuilder;

/*! \class StiSvtHitLoader
  StiSvtHitLoader is a concrete class implementing the StiHitLoader abstract
  interface. It is used to load hits from Star StEvent into the StiHitContainer
  for Sti tracking. StEvent hits from the TPC are converted using the 
  StiDetectorBuilder class.
  <p>
  This class is essentially morphed from the class StiHitFiller 
  originally written by Mike Miller.

  \author Claude A Pruneau (Wayne) and M.L. Miller (Yale Software)
 */
class StiSvtHitLoader : public StiHitLoader<StEvent,StMcEvent,StiDetectorBuilder>
{
public:

	StiSvtHitLoader();
	StiSvtHitLoader(StiHitContainer      * hitContainer,
			StiHitContainer      * mcHitContainer,
			Factory<StiHit>      * hitFactory,
			StiDetectorBuilder   * detector);
	virtual ~StiSvtHitLoader();
	virtual void loadHits(StEvent* source,
			      Filter<StiTrack> * trackFilter, 
			      Filter<StiHit> * hitFilter);
	virtual void loadMcHits(StMcEvent* source,
				bool useMcAsRec,
				Filter<StiTrack> * trackFilter, 
				Filter<StiHit> * hitFilter,
				StMcTrack & stMcTrack,
				StiMcTrack & stiMcTrack);
	int getLayer(int svtLayer) const;
	int getLadder(int svtLayer, int svtLadder) const;
	
};

#endif
