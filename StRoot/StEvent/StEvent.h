/***************************************************************************
 *
 * $Id: StEvent.h,v 1.2 1999/02/10 02:17:34 fisyak Exp $
 *
 * Author: Thomas Ullrich, Jan 1999
 *
 * History:
 * 15/01/1999 T. Wenaus  Add table-based constructor
 *
 ***************************************************************************
 *
 * Description:
 *
 ***************************************************************************
 *
 * $Log: StEvent.h,v $
 * Revision 1.2  1999/02/10 02:17:34  fisyak
 * Merging with new Torre stuff
 *
 * Revision 1.4  1999/02/23 21:20:06  ullrich
 * Modified EMC hit collections.
 *
 * Revision 1.3  1999/01/30 23:03:11  wenaus
 * table load intfc change; include ref change
 *
 * Revision 1.2  1999/01/15 22:53:40  wenaus
 * version with constructors for table-based loading
 *
class StVecPtrVertex;
#ifdef __ROOT__
#include "TObject.h"
#endif
 * an empty collection in case the pointer is null.
#ifndef __CINT__
#include <utility>
#else
  template< class T > class utility;
#endif
#include <TString.h>
#ifndef __CINT__
#endif
 *
#ifndef __ROOT__
#include <time.h>
#include "StTrackCollection.h"
#include "StFtpcHitCollection.h"
#include "StVertexCollection.h"
#include "StSvtHitCollection.h"
#include "StTpcHitCollection.h"
#include "StEmcHitCollection.h"
#include "StSmdHitCollection.h"
#include "StEmcPreShowerHit.h"
#include "StSmdPhiHit.h"
#include "StSmdEtaHit.h"
#include "tables/dst_event_header.h"
#include "tables/dst_event_summary.h"
  Long_t second;
class StEvent : public TObject {
};
 *
    StEvent(const StEvent&);
 * Revision 2.2  1999/11/04 13:30:42  ullrich
 * Added constructor without summary table
 * Adapted new StArray version. First version to compile on Linux and Sun.
    StEvent(StRun*, dst_event_header_st&, dst_event_summary_st&);
 **************************************************************************/
#include "StTrackDetectorInfo.h"
    Int_t operator==(const StEvent &right) const;
#if 0
#ifdef __CINT__
    const TString*                type() const;
#else
    const TString&                type() const;
#endif
     pair<Long_t, Long_t>             id() const;
#endif
    Long_t                       time() const;
    ULong_t                runNumber() const;              
    ULong_t                triggerMask() const;
    ULong_t                bunchCrossingNumber() const;
    Double_t                       luminosity() const;
    StRun*                       run();
    StVertex*                    primaryVertex();
    StDstEventSummary*              summary();
    StTrackCollection*           trackCollection();
    StTpcHitCollection*          tpcHitCollection();
    StSvtHitCollection*          svtHitCollection();
    StFtpcHitCollection*         ftpcHitCollection();
    StEmcHitCollection*          emcHitCollection();
    StSmdHitCollection*          smdHitCollection();
    StVertexCollection*          vertexCollection();
    StTriggerDetectorCollection* triggerDetectorCollection();
    StL0Trigger*                 l0Trigger();                        
    Float_t                        beamPolarization(StBeamDirection, StBeamPolarizationAxis);

    void setType(const TString*);
#if 0
    void setId(const pair<Long_t, Long_t>&);
#endif
    void setTime(Long_t);
    void setRunNumber(ULong_t);                
    void setTriggerMask(ULong_t);              
    void setBunchCrossingNumber(ULong_t);      
    void setLuminosity(Double_t);               
    void setRun(StRun*);                            
    void setPrimaryVertex(StVertex*);                  
    void setSummary(StDstEventSummary*);                        
    void setTrackCollection(StTrackCollection*);                
    void setTpcHitCollection(StTpcHitCollection*);               
    void setSvtHitCollection(StSvtHitCollection*);               
    void setFtpcHitCollection(StFtpcHitCollection*);              
    void setEmcHitCollection(StEmcHitCollection*);              
    void setSmdHitCollection(StSmdHitCollection*);              
    void setVertexCollection(StVertexCollection*);               
    void setTriggerDetectorCollection(StTriggerDetectorCollection*);      
    void setL0Trigger(StL0Trigger*);                      
    void setBeamPolarization(StBeamDirection, StBeamPolarizationAxis, Float_t);                   
    virtual void setTriggerDetectorCollection(StTriggerDetectorCollection*);      
    virtual void setL0Trigger(StL0Trigger*);                      
    TString                       mType;
#if 0
    pair<Long_t, Long_t>             mId;                      
#endif
    ULong_t                mRunNumber;
    Long_t                       mTime;
    ULong_t                mTriggerMask;
    ULong_t                mBunchCrossingNumber;
    Double_t                       mLuminosity;

    StRun*                       mRun;
    Double_t                     mLuminosity;
    StTrackCollection*           mTracks;
    StVertexCollection*          mVertices;
    StDstEventSummary*           mSummary;
    //    StRun*                       mRun;
    StVertex*                    mPrimaryVertex;
    StEmcHitCollection*          mEmcHits;
    StSmdHitCollection*          mSmdHits;
    StEmcPreShowerHitCollection* mEmcPreShowerHits;
    StTriggerDetectorCollection* mTriggerDetectors;
    Float_t                        mBeamPolarizationEast[3];
    Float_t                        mBeamPolarizationWest[3];

    Float_t                      mBeamPolarizationEast[3];
    Float_t                      mBeamPolarizationWest[3];
#ifdef __ROOT__
	ClassDef(StEvent,1)  //StEvent structure
#endif
    const StEvent& operator=(const StEvent&);
    StTriggerDetectorCollection* mTriggerDetectors;  
  ClassDef(StEvent,1)  //StEvent structure
#if 0
inline const TString& StEvent::type() const { return mType;}
inline pair<Long_t, Long_t> StEvent::id() const { return mId;}
#endif
inline Long_t StEvent::time() const { return mTime;}

inline ULong_t StEvent::runNumber() const { return mRunNumber;}             

inline ULong_t StEvent::triggerMask() const { return mTriggerMask;}

inline ULong_t StEvent::bunchCrossingNumber() const { return mBunchCrossingNumber;}

inline Double_t StEvent::luminosity() const { return mLuminosity;}

inline StRun* StEvent::run() { return mRun;}

inline StVertex* StEvent::primaryVertex() { return mPrimaryVertex;}

inline StDstEventSummary* StEvent::summary() { return mSummary;}

inline StTrackCollection* StEvent::trackCollection() { return mTracks;}

inline StTpcHitCollection* StEvent::tpcHitCollection() { return mTpcHits;}

inline StSvtHitCollection* StEvent::svtHitCollection() { return mSvtHits;}

inline StFtpcHitCollection* StEvent::ftpcHitCollection() { return mFtpcHits;}

inline StEmcHitCollection* StEvent::emcHitCollection() { return mEmcHits;}

inline StSmdHitCollection* StEvent::smdHitCollection() { return mSmdHits;}

inline StVertexCollection* StEvent::vertexCollection() { return mVertices;}

inline StTriggerDetectorCollection* StEvent::triggerDetectorCollection() { return mTriggerDetectors;}

inline StL0Trigger* StEvent::l0Trigger() { return mL0Trigger;}                        
#ifndef __CINT__
ostream&  operator<<(ostream& os, const StEvent&);
#endif

inline Float_t StEvent::beamPolarization(StBeamDirection dir, StBeamPolarizationAxis axis)
{
    if (dir == east)
	return mBeamPolarizationEast[axis];
#endif
    else
	return mBeamPolarizationWest[axis];
}

    StSPtrVecTrackDetectorInfo   mTrackDetectorInfo;
    StSPtrVecTrackNode           mTrackNodes;
    StSPtrVecPrimaryVertex       mPrimaryVertices;
    StSPtrVecV0Vertex            mV0Vertices;
    StSPtrVecXiVertex            mXiVertices;
    StSPtrVecKinkVertex          mKinkVertices;

    static TString               mCvsTag;
    mutable StSPtrVecTrackDetectorInfo*  mTrackDetectorInfo;
    mutable StSPtrVecTrackNode*          mTrackNodes;

    mutable StSPtrVecPrimaryVertex*      mPrimaryVertices;
           mContentLength };
    
    mutable StSPtrVecObject  mContent;
    static TString           mCvsTag;

private:
    StEvent& operator=(const StEvent&);
    StEvent(const StEvent&);
    void initToZero();
    void init(const event_header_st&);
    
    ClassDef(StEvent,1)
};
#endif







