/***************************************************************************
 *
 * $Id: calculateEventPlaneEventCut.h,v 1.1 2001/07/20 20:03:50 rcwells Exp $
 *
 * Author: Randall Wells, Ohio State, rcwells@mps.ohio-state.edu
 ***************************************************************************
 *
 * Description: Passes HbtEvent to FlowMaker to calculate the event
 *     plane.  Warning ... this will change event charateristics!
 *
 ***************************************************************************
 *
 * $Log: calculateEventPlaneEventCut.h,v $
 * Revision 1.1  2001/07/20 20:03:50  rcwells
 * Added pT weighting and moved event angle cal. to event cut
 *
 *
 **************************************************************************/

#ifndef calculateEventPlaneEventCut_hh
#define calculateEventPlaneEventCut_hh

// do I need these lines ?
//#ifndef StMaker_H
//#include "StMaker.h"
//#endif

#include "StMaker.h"
#include "StChain.h"
#include "St_DataSetIter.h"

#include "StHbtMaker/Base/StHbtEventCut.h"
class StFlowMaker;
class StFlowEvent;
class StFlowAnalysisMaker;
class StFlowSelection;

class calculateEventPlaneEventCut : public StMaker, public StHbtEventCut {

public:

  calculateEventPlaneEventCut();
  calculateEventPlaneEventCut(calculateEventPlaneEventCut&);
  //~calculateEventPlaneEventCut();

  int NEventsPassed();
  int NEventsFailed();

  void SetFlowMaker(char* title);
  void SetFlowAnalysisMaker(char* title);
  void FillFromHBT(const int& hbt);

  virtual StHbtString Report();
  virtual bool Pass(const StHbtEvent*);

  calculateEventPlaneEventCut* Clone();

private:   // 
  StFlowMaker* mFlowMaker;                 //!
  StFlowAnalysisMaker* mFlowAnalysisMaker; //!

  int mFromHBT;
  long mNEventsPassed;
  long mNEventsFailed;

#ifdef __ROOT__
  ClassDef(calculateEventPlaneEventCut, 1)
#endif

};

inline int  calculateEventPlaneEventCut::NEventsPassed() {return mNEventsPassed;}
inline int  calculateEventPlaneEventCut::NEventsFailed() {return mNEventsFailed;}
inline void calculateEventPlaneEventCut::SetFlowMaker(char* title){
  mFlowMaker = (StFlowMaker*)GetMaker(title);
  if (!mFlowMaker) {
    cout << "No StFlowMaker found!" << endl;
    assert(0);
  }
}
inline void calculateEventPlaneEventCut::SetFlowAnalysisMaker(char* title) {
  mFlowAnalysisMaker = (StFlowAnalysisMaker*)GetMaker(title);
  if (!mFlowAnalysisMaker) {
    cout << "No StFlowAnalysisMaker found!" << endl;
    assert(0);
  }
}
inline void calculateEventPlaneEventCut::FillFromHBT(const int& hbt) {
  mFromHBT = hbt;
}
inline calculateEventPlaneEventCut* calculateEventPlaneEventCut::Clone() {
  calculateEventPlaneEventCut* c = new calculateEventPlaneEventCut(*this); return c;
}
inline calculateEventPlaneEventCut::calculateEventPlaneEventCut(calculateEventPlaneEventCut& c) : StHbtEventCut(c) {
  mNEventsPassed = 0;
  mNEventsFailed = 0;
}


#endif
