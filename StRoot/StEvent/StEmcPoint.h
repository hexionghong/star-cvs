/*!
 * \class StEmcPoint 
 * \author Akio Ogawa, Mar 2000
 */
/***************************************************************************
 *
 * $Id: StEmcPoint.h,v 2.6 2004/07/15 16:36:24 ullrich Exp $
 *
 * Author: Akio Ogawa, Mar 2000
 ***************************************************************************
 *
 * Description: Base class for electromagnetic calorimeter Point
 *
 ***************************************************************************
 *
 * $Log: StEmcPoint.h,v $
 * Revision 2.6  2004/07/15 16:36:24  ullrich
 * Removed all clone() declerations and definitions. Use StObject::clone() only.
 *
 * Revision 2.5  2002/02/22 22:56:47  jeromel
 * Doxygen basic documentation in all header files. None of this is required
 * for QM production.
 *
 * Revision 2.4  2001/04/05 04:00:35  ullrich
 * Replaced all (U)Long_t by (U)Int_t and all redundant ROOT typedefs.
 *
 * Revision 2.3  2001/03/24 03:34:45  perev
 * clone() -> clone() const
 *
 * Revision 2.2  2000/05/22 19:21:54  akio
 * Bug fix, add delta into EMcPoint, wider bits for Eta in RawHit
 *
 * Revision 2.1  2000/03/23 22:24:07  akio
 * Initial version of Emc Point, and Inclusion of track pointers
 *
 *
 **************************************************************************/
#ifndef StEmcPoint_hh
#define StEmcPoint_hh

#include "StHit.h"
#include "StEnumerations.h"

class StEmcPoint : public StHit {
public:
    StEmcPoint();
    StEmcPoint(const StThreeVectorF&,
	       const StThreeVectorF&,
	       const StThreeVectorF&,
               unsigned int, float,
	       float, float,
	       unsigned char = 0);
    ~StEmcPoint();
    
    float   energy() const;
    float   chiSquare() const;
    void setEnergy(const float);
    void setChiSquare(const float);
    StThreeVectorF size() const;
    void setSize(const StThreeVectorF&);
    
    float   energyInDetector(const StDetectorId) const;
    float   sizeAtDetector(const StDetectorId) const;
    void setEnergyInDetector(const StDetectorId, const float);
    void setSizeAtDetector(const StDetectorId, const float);
    
    float deltaEta() const;
    float deltaPhi() const;
    float deltaU() const;
    float deltaV() const;
    void  setDeltaEta(const float);
    void  setDeltaPhi(const float);
    void  setDeltaU(const float);
    void  setDeltaV(const float);
    
    StPtrVecEmcCluster&       cluster(const StDetectorId);
    const StPtrVecEmcCluster& cluster(const StDetectorId) const;

    void addCluster(const StDetectorId, const StEmcCluster*);
    
    StPtrVecEmcPoint&       neighbor();
    const StPtrVecEmcPoint& neighbor() const;

    void addNeighbor(const StEmcPoint*);
    
    int                  nTracks() const;
    StPtrVecTrack&       track();
    const StPtrVecTrack& track() const;
    
    void addTrack(StTrack*);
    
protected:
    Float_t            mEnergy;
    Float_t            mChiSquare;
    StThreeVectorF     mSize;
    Float_t            mEnergyInDetector[4];
    Float_t            mSizeAtDetector[4];
    Float_t            mDelta[2];
    StPtrVecEmcCluster mCluster[4];
    StPtrVecEmcPoint   mNeighbors;
    StPtrVecTrack      mTracks;
    
    int getDetId(const StDetectorId) const;
    ClassDef(StEmcPoint,1)
};
#endif


