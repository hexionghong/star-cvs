/**
 * $Id: StMiniMcPair.h,v 1.3 2004/03/31 23:42:46 calderon Exp $
 * \file  StMiniMcPair.h
 * \brief  for simplicity, this contains both the rc and mc track information.
 * 
 *
 * \author Bum Choi
 * \date   March 2001
*/

#ifndef StMiniMcPair_H
#define StMiniMcPair_H

#include "StTinyMcTrack.h"
#include "StTinyRcTrack.h"

class StMiniMcPair : public StTinyMcTrack, public StTinyRcTrack {
public:
    StMiniMcPair();
    virtual ~StMiniMcPair();
    
    void setNCommonHit(Short_t val) { mNCommonHit=val; }
    void setIsBestContam(Short_t val ) {mIsBestContam=val ; }
    void setDominatrack(Short_t val) { mDominatrack=val; }
    void setDominCommonHit(Short_t val) { mDominCommonHit=val; }
    void setAvgQuality(float val) { mAvgQuality=val; }
    float    commonFrac() const { 
	return static_cast<float>(mNCommonHit)/static_cast<float>(allPts());
    }
    float    dominFrac() const {
	return static_cast<float>(mDominCommonHit)/static_cast<float>(allPts());
    }
    int commonHits() { return mNCommonHit%100; }
    int commonHitsSvt() { return mNCommonHit/100; }
    bool isBestContam() { return mIsBestContam; }
    int dominatrack() { return mDominatrack; }
    int dominCommonHits() { return mDominCommonHit; }
    float avgQuality() { return mAvgQuality; }
private:
    Short_t      mNCommonHit; /// Common Hits is now encoded, tpc + svt * 100
    Bool_t     mIsBestContam;
    Short_t     mDominatrack;
    Short_t  mDominCommonHit;
    Float_t      mAvgQuality;
  ClassDef(StMiniMcPair,3)
};
  
#endif

//
// $Log: StMiniMcPair.h,v $
// Revision 1.3  2004/03/31 23:42:46  calderon
// Adding info to evaluate idTruth information.
// -Add key to StTinyMcTrack.h
// -Add dominatrack, common hits to dominatrack and average hit quality to StMiniMcPair.h
//
// Revision 1.2  2004/03/15 19:01:32  calderon
// StMiniMcPair now contains the TPC and SVT common hits information (encoded).
//
// Revision 1.1  2002/05/30 01:20:58  calderon
// Classes for use in a general framework for extracting efficiencies
// from both embedding and full simulations
// (after GSTAR+TRS+StEvent+StMcEvent+StAssociationMaker)
// so that the information of the track matches gets stored persistently.
//
//
