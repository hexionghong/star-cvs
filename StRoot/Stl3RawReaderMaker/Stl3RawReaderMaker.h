//
//  $Id: Stl3RawReaderMaker.h,v 1.8 2002/02/13 22:36:32 struck Exp $
//
//  $Log: Stl3RawReaderMaker.h,v $
//  Revision 1.8  2002/02/13 22:36:32  struck
//  major code clean-up for Stl3RawReaderMaker, first version of Stl3CounterMaker
//
//  Revision 1.7  2001/11/14 23:30:56  struck
//  major update: set 'unbiased'-flag, correct bugs in StGlobalTrack-filling
//
//  Revision 1.6  2001/09/27 03:49:53  struck
//  actual no. of gl3s handled flexible, max no. of gl3s and algorithms now global define-statements
//
//  Revision 1.5  2001/08/29 20:24:49  struck
//  makes Solaris compiler happy
//
//  Revision 1.4  2001/08/20 22:32:00  struck
//  first version filling L3 counters and algorithm info into StEvent
//
//


#ifndef STAR_Stl3RawReaderMaker
#define STAR_Stl3RawReaderMaker

//////////////////////////////////////////////////////////////////////////
//                                                                      //
//                       Stl3RawReaderMaker                             //
//                                                                      //
//////////////////////////////////////////////////////////////////////////
#ifndef StMaker_H
#include "StMaker.h"
#endif

#include "StDAQMaker/StDAQReader.h"
#include "Stl3CounterMaker.h"


// some foreward declaration since includes don't work
// in these cases 
class globalTrack;
class StEvent;
class StL3Trigger;
class StPrimaryVertex;


class Stl3RawReaderMaker : public StMaker {
 
 private:
    // General stuff
    TDataSet*          DAQReaderSet; //!
    StL3Reader*        ml3reader;  //!

    // StEvent
    StEvent*           mStEvent; //!
    StL3Trigger*       myStL3Trigger; //!

    // Database
    TDataSet*          mDbSet;

    // switches
    bool               mWriteStEvent; //!
    bool               mL3On;

    // counter
    GlobalCounter      mGlobalCounter[MaxNumberOfGl3Nodes];
    AlgorithmCounter   mAlgorithmCounter[MaxNumberOfGl3Nodes][MaxNumberOfAlgorithms];

    int                mNumberOfGl3Nodes;
    int                mNumberOfAlgorithms;
    int                mEventCounter;

 public: 
                  Stl3RawReaderMaker(const char *name="l3RawReader");
   virtual       ~Stl3RawReaderMaker();
   virtual Int_t Init();
   virtual Int_t Make();

   // StEvent
   Int_t fillStEvent() ;
   Int_t fillStEventWithL3GlobalTracks()  ;
   Int_t fillStEventWithi960Hits() ;

   virtual const char *GetCVS() const
  {static const char cvs[]="Tag $Name:  $ $Id: Stl3RawReaderMaker.h,v 1.8 2002/02/13 22:36:32 struck Exp $ built "__DATE__" "__TIME__ ; return cvs;}

   ClassDef(Stl3RawReaderMaker, 2)   //StAF chain virtual base class for Makers
};

#endif
