// $Id: summarizeEvent.cc,v 1.6 1999/08/06 21:25:33 fisyak Exp $
// $Log: summarizeEvent.cc,v $
// Revision 1.6  1999/08/06 21:25:33  fisyak
// Switch to StMessager
//
// Revision 1.5  1999/08/06 20:21:53  kathy
// back to old version that didn't write out QA info file, but now added QAInfo tag in front of information that QA team wants in summarizeEvent.cc - will also add a few more lines of output to summarizeEvent.cc soon
//
// Revision 1.4  1999/07/30 22:56:02  kathy
// added new method and input param qaflag so that if turned on, a log file will be printed out with QA information
//
// Revision 1.3  1999/06/25 19:20:41  fisyak
// Merge StRootEvent and StEvent
//
// Revision 1.3  1999/06/24 21:56:48  wenaus
// Version minimally changed from standard StAnalysisMaker
//
// Revision 1.2  1999/02/11 15:39:16  wenaus
// cleanup
//
//
///////////////////////////////////////////////////////////////////////////////
//
// summarizeEvent.cc
//
// Description: 
//  StEvent summary printout
//
// Environment:
//  Software developed for the STAR Detector at Brookhaven National Laboratory
//
// Author List: 
//  Torre Wenaus, BNL  1/99
//
// History:
//
///////////////////////////////////////////////////////////////////////////////
#include "StEvent.h"
 *
static const char rcsid[] = "$Id: summarizeEvent.cc,v 1.6 1999/08/06 21:25:33 fisyak Exp $";
 *
void summarizeEvent(StEvent& event)
{
  gMessMgr->Info() << "QAInfo: StAnalysisMaker,  Reading Event "  << 
    " Type " << event.type() << " Run " << event.runNumber() << endl;
    gMessMgr->QAInfo() << "StAnalysisMaker,  Reading Event: " << nevents
  gMessMgr->Info() << " N vertex " << event.vertexCollection()->size() << endl;
  gMessMgr->Info() << " N track " << event.trackCollection()->size() << endl;
  gMessMgr->Info() << " N TPC hit " << event.tpcHitCollection()->size() << endl;
  gMessMgr->Info() << " N FTPC hit " << event.ftpcHitCollection()->size() << endl;
  gMessMgr->Info() << " N SVT hit " << event.svtHitCollection()->size() << endl;

  gMessMgr->Info() << "QAInfo: # tracks:         " << 
              event.trackCollection()->size() << endl;
  gMessMgr->Info() << "QAInfo:  # vertices:       " << 
              event.vertexCollection()->size() << endl;
  gMessMgr->Info() << "QAInfo:  # TPC hits:       " << 
             event.tpcHitCollection()->size() << endl;
  gMessMgr->Info() << "QAInfo:  # SVT hits:       " << 
             event.svtHitCollection()->size() << endl;
  gMessMgr->Info() << "QAInfo:  # FTPC hits:      " << 
             event.ftpcHitCollection()->size() << endl;
  gMessMgr->Info() << "QAInfo:  primary vertex:   " << 
             event.primaryVertex()->position() << endl;

		       << (event.svtHitCollection() ? event.svtHitCollection()->numberOfHits() : 0) << endm;




    
    gMessMgr->QAInfo() << "# FTPC hits:      "
		       << (event.ftpcHitCollection() ? event.ftpcHitCollection()->numberOfHits() : 0) << endm;
    
    if (event.primaryVertex()) {
	gMessMgr->QAInfo() << "primary vertex:   "
			   << event.primaryVertex()->position() << endm;
    }
}
