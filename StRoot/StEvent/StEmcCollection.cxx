 /***************************************************************************
 *
 * $Id: StEmcCollection.cxx,v 2.4 2001/04/05 04:00:48 ullrich Exp $
 *
 * Author: Akio Ogawa, Nov 1999
 ***************************************************************************
 *
 * Description:
 *
 ***************************************************************************
 *
 * $Log: StEmcCollection.cxx,v $
 * Revision 2.4  2001/04/05 04:00:48  ullrich
 * Replaced all (U)Long_t by (U)Int_t and all redundant ROOT typedefs.
 *
 * Revision 2.3  2000/07/28 19:49:27  akio
 * Change in Detector Id for Endcap SMD
 *
 * Revision 2.2  2000/03/23 22:24:06  akio
 * Initial version of Emc Point, and Inclusion of track pointers
 *
 * Revision 2.1  2000/02/23 17:34:05  ullrich
 * Initial Revision
 *
 **************************************************************************/
#include "StEmcCollection.h"
#include "StEmcDetector.h"

ClassImp(StEmcCollection)

static const char rcsid[] = "$Id: StEmcCollection.cxx,v 2.4 2001/04/05 04:00:48 ullrich Exp $";

StEmcCollection::StEmcCollection() {/* noop*/}

StEmcCollection::~StEmcCollection(){
  for(int i=0; i<8; i++){
    if(mDetector[i]) delete mDetector[i];
  }
}
    
const StEmcDetector*
StEmcCollection::detector(StDetectorId id) const
{
    if(id >= kBarrelEmcTowerId && id <= kEndcapSmdVStripId)
        return mDetector[id-kBarrelEmcTowerId];
    else
        return 0;
}

StEmcDetector*
StEmcCollection::detector(StDetectorId id)
{
    if(id >= kBarrelEmcTowerId && id <= kEndcapSmdVStripId)
        return mDetector[id-kBarrelEmcTowerId];
    else
        return 0;
}

void
StEmcCollection::setDetector(StEmcDetector* val)
{
    if (val) {
        unsigned int id = val->detectorId();
        if (id >= kBarrelEmcTowerId && id <= kEndcapSmdVStripId) {
            if (mDetector[id-kBarrelEmcTowerId]) delete mDetector[id-kBarrelEmcTowerId];
            mDetector[id-kBarrelEmcTowerId] = val;
        }
    }
}

const StSPtrVecEmcPoint&
StEmcCollection::barrelPoints() const { return mBarrel; }

StSPtrVecEmcPoint&
StEmcCollection::barrelPoints() { return mBarrel; }

const StSPtrVecEmcPoint&
StEmcCollection::endcapPoints() const { return mEndcap; }

StSPtrVecEmcPoint&
StEmcCollection::endcapPoints() { return mEndcap; }

void
StEmcCollection::addBarrelPoint(const StEmcPoint* p){mBarrel.push_back(p);}

void
StEmcCollection::addEndcapPoint(const StEmcPoint* p){mEndcap.push_back(p);}
