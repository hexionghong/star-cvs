/***************************************************************************
 *
 * $Id: StHbtEvent.hh,v 1.2 1999/06/29 17:50:27 fisyak Exp $
 *
 * Author: Mike Lisa, Ohio State, lisa@mps.ohio-state.edu
 ***************************************************************************
 *
 * Description: part of STAR HBT Framework: StHbtMaker package
 *   HbtEvent is the "transient microDST"  Objects of this class are
 *   generated from the input data by a Reader, and then presented to
 *   the Cuts of the various active Analyses.
 *
 ***************************************************************************
 *
 * $Log: StHbtEvent.hh,v $
 * Revision 1.2  1999/06/29 17:50:27  fisyak
 * formal changes to account new StEvent, does not complie yet
 *
 * Revision 1.1.1.1  1999/06/29 16:02:57  lisa
 * Installation of StHbtMaker
 *
 **************************************************************************/

#ifndef StHbtEvent_hh
#define StHbtEvent_hh

#include "StThreeVectorD.hh"
#include "StHbtMaker/Infrastructure/StHbtTrackCollection.hh"

class StHbtEvent{
public:
  StHbtEvent();
  ~StHbtEvent();

  int Mult() const;
  StThreeVectorD PrimVertPos() const;
  StHbtTrackCollection* TrackCollection() const;

  void SetMult(const int&);
  void SetPrimVertPos(const StThreeVectorD&);

private:
  int mMult;
  StThreeVectorD mPrimVertPos;
  StHbtTrackCollection* mTrackCollection;

};

inline void StHbtEvent::SetMult(const int& m){mMult=m;}
inline void StHbtEvent::SetPrimVertPos(const StThreeVectorD& vp){mPrimVertPos = vp;}

inline StHbtTrackCollection* StHbtEvent::TrackCollection() const {return mTrackCollection;}
inline int StHbtEvent::Mult() const {return mMult;}
inline StThreeVectorD StHbtEvent::PrimVertPos() const {return mPrimVertPos;}


#endif
