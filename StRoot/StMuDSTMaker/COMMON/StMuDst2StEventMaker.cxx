/***************************************************************************
 *
 * $Id: StMuDst2StEventMaker.cxx,v 1.6 2003/08/29 14:54:00 laue Exp $
 * Author: Frank Laue, BNL, laue@bnl.gov
 ***************************************************************************/
#include "StMuDst2StEventMaker.h"

#include "StMuDSTMaker/COMMON/StMuDstMaker.h"
#include "StMuDSTMaker/COMMON/StMuDst.h"
#include "StMuDSTMaker/COMMON/StMuDebug.h"
#include "StEvent/StEventTypes.h"
#include "StEvent/StTriggerIdCollection.h"
#include "StEvent/StTriggerId.h"
#include "StEvent/StTpcDedxPidAlgorithm.h"


StMuDst2StEventMaker::StMuDst2StEventMaker(const char* self ,const char* muDstMakerName) : StMaker(self) {
  mMuDstMaker = (StMuDstMaker*)GetMaker(muDstMakerName);
}

StMuDst2StEventMaker::~StMuDst2StEventMaker() { 
  /* no=op */
}
    
int StMuDst2StEventMaker::Init(){
  return 0;
}
 
void StMuDst2StEventMaker::Clear() {
    if ( mStEvent ) delete mStEvent;
    mStEvent =0;
}

int StMuDst2StEventMaker::Make(){  ///< create a StEvent from the muDst and put it into the .data tree 
  mStEvent = 0; // I do not delete the StEvent, the chain is responsible for deletion. 
                // I just set the pointer to zero, so that you never pick up the old StEvent 

  if ( mMuDstMaker ) {
    mStEvent = mMuDstMaker->muDst()->createStEvent();
    StMuDst* muDst = mMuDstMaker->muDst();
        if(mStEvent) {
      // set chain date to be the same of event date
      StEvtHddr *hd = (StEvtHddr*)GetDataSet("EvtHddr");
      if(!hd) { hd = new StEvtHddr();  AddData(hd); }
      hd->SetGMTime(mStEvent->time());
      hd->SetRunNumber(mStEvent->runInfo()->runId());
      AddData(mStEvent);     // add StEvent to the .data tree
    }

    // print all the trigger ids
    // so now you have a StEvent and you can loop
    // uncomment the following if to have to print out and tests
//     if (mStEvent) {
// 	printTriggerIds(mStEvent);
// 	loopOverTracks(mStEvent);
//     }

  }
  return 0;
}



void StMuDst2StEventMaker::printTriggerIds(StEvent* ev) {
    if ( ev->triggerIdCollection() ) {
	if ( ev->triggerIdCollection()->l1() ) {
	    if ( StMuDebug::level()>0 ) {
		cout << "l1 triggers: ";
		vector<unsigned int> v = ev->triggerIdCollection()->l1()->triggerIds();	
		for ( unsigned int i=0; i<v.size(); i++) cout << v[i] << " "; 
		cout << endl;
	    }
	}
	if ( ev->triggerIdCollection()->l2() ) {
	    if ( StMuDebug::level()>0 ) {
		cout << "l2 triggers: ";
		vector<unsigned int> v = ev->triggerIdCollection()->l2()->triggerIds();	
		for ( unsigned int i=0; i<v.size(); i++) cout << v[i] << " "; 
		cout << endl;
	    }
	}
	if ( ev->triggerIdCollection()->l3() ) {
	    if ( StMuDebug::level()>0 ) {
		cout << "l3 triggers: ";
		vector<unsigned int> v = ev->triggerIdCollection()->l3()->triggerIds();	
		for ( unsigned int i=0; i<v.size(); i++) cout << v[i] << " "; 
		cout << endl;
	    }
	}
	if ( ev->triggerIdCollection()->nominal() ) {
	    if ( StMuDebug::level()>0 ) {
		cout << "nominal triggers: ";
		vector<unsigned int> v = ev->triggerIdCollection()->nominal()->triggerIds();	
		for ( unsigned int i=0; i<v.size(); i++) cout << v[i] << " "; 
		cout << endl;
	    }
	}
    }
}

void StMuDst2StEventMaker::loopOverTracks(StEvent* ev) {
    cout << "StMuDst2StEventMaker::loopOverTracks(...)" << endl;
    StDedxPidTraits dEdxPidTraits;
    StTpcDedxPidAlgorithm pidAlgorithm;
    // get the track nodes vector and loop over nodes
    //    const StSPtrVecTrackNode nodes = ev->trackNodes();
        int numberOfNodes  = ev->trackNodes().size();
        for ( int i=0; i< numberOfNodes; i++) {
	    StTrackNode* node = ev->trackNodes()[i];
	    // print some primary track values 
	    StTrack* t = node->track(primary);
	    if (t) {
		// fill pid traits and print some values
		const StParticleDefinition* pid  = t->pidTraits(pidAlgorithm);
		// if we have pid traits
		if ( pidAlgorithm.traits() ) {
		    //		    cout << "dE/dx = " <<  pidAlgorithm.traits()->mean() << "   ";
		    //		    cout << "number of hits used in dE/dx = " << pidAlgorithm.traits()->numberOfPoints() << endl;
		}
	    }
	}
}


int StMuDst2StEventMaker::Finish() { 
  return 0;
}
    

ClassImp(StMuDst2StEventMaker)

/***************************************************************************
 *
 * $Log: StMuDst2StEventMaker.cxx,v $
 * Revision 1.6  2003/08/29 14:54:00  laue
 * Commented out printing of the trigger ids and the (test)-loop over the tracks.
 *
 * Revision 1.5  2003/08/28 13:01:45  laue
 * now passing proper maker name to the call of the StMaker(name) constructor
 *
 * Revision 1.4  2003/08/04 14:38:10  laue
 * Alex Suaide's updated for the EMC. Now EEMC is included.
 *
 * Revision 1.3  2003/04/15 18:48:35  laue
 * Minor changes to be able to filter MuDst.root files and an example
 * how to do this. The StMuDstFilterMaker is just an example, it has to be
 * customized (spoilers, chrome weels, etc.) by the user.
 *
 * Revision 1.2  2003/03/19 18:58:04  laue
 * StMuChainMaker: updates for moved file catalog
 * StTriggerIdCollection added to the createStEvent function in StMuDst.cxx
 *
 * Revision 1.1  2003/01/09 18:59:45  laue
 * initial check in of new EMC classes and the changes required
 *
 * Revision 1.15  2002/11/08 14:18:59  laue
 *
 **************************************************************************/
