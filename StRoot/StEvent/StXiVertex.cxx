/***************************************************************************
 *
 * $Id: StXiVertex.cxx,v 2.6 2001/06/05 21:58:26 perev Exp $
 *
 * Author: Gene Van Buren, Feb 1999, revised Thomas Ullrich Sep 99
 ***************************************************************************
 *
 * Description: vertex class for cascades
 *
 ***************************************************************************
 *
 * $Log: StXiVertex.cxx,v $
 * Revision 2.6  2001/06/05 21:58:26  perev
 * HPcorr
 *
 * Revision 2.5  2001/05/30 17:45:55  perev
 * StEvent branching
 *
 * Revision 2.4  2001/04/05 04:00:59  ullrich
 * Replaced all (U)Long_t by (U)Int_t and all redundant ROOT typedefs.
 *
 * Revision 2.3  2001/03/24 03:35:01  perev
 * clone() -> clone() const
 *
 * Revision 2.2  1999/11/04 13:31:17  ullrich
 * Changed order of constructor arguments
 *
 * Revision 2.1  1999/10/28 22:28:15  ullrich
 * Adapted new StArray version. First version to compile on Linux and Sun.
 *
 * Revision 2.0  1999/10/12 18:43:34  ullrich
 * Completely Revised for New Version
 *
 **************************************************************************/
#include <iostream.h>
#include "TClass.h"
#include "StXiVertex.h"
#include "StV0Vertex.h"
#include "StTrack.h"
#include "StTrackGeometry.h"
#include "tables/St_dst_vertex_Table.h"
#include "tables/St_dst_xi_vertex_Table.h"

static const char rcsid[] = "$Id: StXiVertex.cxx,v 2.6 2001/06/05 21:58:26 perev Exp $";

ClassImp(StXiVertex)

StXiVertex::StXiVertex()
{
    mType = kXiVtxId;
    mDaughter = 0;
    mDcaBachelorToPrimaryVertex = 0;
    mDcaDaughters = 0;
    mDcaParentToPrimaryVertex = 0;
    mV0Vertex = 0;
}

StXiVertex::StXiVertex(const dst_vertex_st& vtx,
                       const dst_xi_vertex_st& xivtx) :
    StVertex(vtx)
{
    mType = kXiVtxId;
    mDcaBachelorToPrimaryVertex = xivtx.b_b;
    mMomentumOfBachelor.setX(xivtx.px_b);
    mMomentumOfBachelor.setY(xivtx.py_b);
    mMomentumOfBachelor.setZ(xivtx.pz_b);
    mDcaDaughters = xivtx.dca;
    mDcaParentToPrimaryVertex = xivtx.b_xi;
    mV0Vertex = 0;
    mDaughter = 0;
}

StXiVertex::~StXiVertex() { /* noop */ }

StObject*
StXiVertex::clone() const { return new StXiVertex(*this); }

StVertexId
StXiVertex::type() const { return kXiVtxId; }

unsigned int
StXiVertex::numberOfDaughters() const { return mDaughter ? 1 : 0;}

StTrack*
StXiVertex::daughter(unsigned int i)
{
    return i == 0 ? (StTrack*)mDaughter : 0;
}

const StTrack*
StXiVertex::daughter(unsigned int i) const
{
    return i == 0 ? (const StTrack*)mDaughter : 0;
}

StPtrVecTrack
StXiVertex::daughters(StTrackFilter& filter)
{
    StPtrVecTrack vec;
    if (filter(mDaughter)) vec.push_back(mDaughter);
    return vec;
}

float
StXiVertex::dcaV0ToPrimaryVertex() const
{
    if (mV0Vertex)
        return mV0Vertex->dcaParentToPrimaryVertex();
    else
        return 0;
}

StThreeVectorF
StXiVertex::momentumOfV0() const
{
    if (mV0Vertex) {
        const StThreeVectorF& nMom = mV0Vertex->momentumOfDaughter(negative);
        const StThreeVectorF& pMom = mV0Vertex->momentumOfDaughter(positive);
        return (nMom + pMom);
    }
    else
        return StThreeVectorF();
}

float
StXiVertex::dcaBachelorToPrimaryVertex () const
{
    return mDcaBachelorToPrimaryVertex;
}

const StThreeVectorF&
StXiVertex::momentumOfBachelor() const { return mMomentumOfBachelor; }

StThreeVectorF
StXiVertex::momentum() const
{
    return mMomentumOfBachelor + momentumOfV0();
}

float
StXiVertex::dcaDaughters() const { return mDcaDaughters; }

float
StXiVertex::dcaParentToPrimaryVertex() const { return mDcaParentToPrimaryVertex; }

StV0Vertex*
StXiVertex::v0Vertex() const { return (StV0Vertex*&)mV0Vertex; }

StTrack*
StXiVertex::bachelor() { return mDaughter; }

void
StXiVertex::setDcaBachelorToPrimaryVertex(float val)
{
    mDcaBachelorToPrimaryVertex = val;
}

double
StXiVertex::chargeOfBachelor()
{
    StTrack* b = bachelor();
    return b ? b->geometry()->charge() : 0;
}

void
StXiVertex::setMomentumOfBachelor(const StThreeVectorF& v)
{
    mMomentumOfBachelor = v;
}

void
StXiVertex::setDcaDaughters(float val) { mDcaDaughters = val; }

void
StXiVertex::setDcaParentToPrimaryVertex(float val) { mDcaParentToPrimaryVertex = val; }

void
StXiVertex::setV0Vertex(StV0Vertex* v0vtx)
{
    mV0Vertex = v0vtx;
}

void
StXiVertex::addDaughter(StTrack* track) { mDaughter = track; }


void
StXiVertex::removeDaughter(StTrack* track)
{
    if (track == mDaughter)  mDaughter = 0;
}

void StXiVertex::Streamer(TBuffer &R__b)
{
    // Stream an object of class .

    if (R__b.IsReading()) {
       UInt_t R__s, R__c;
       Version_t R__v = R__b.ReadVersion(&R__s, &R__c);
       if (R__v > 1) {
          Class()->ReadBuffer(R__b, this, R__v, R__s, R__c);
          return;
       }
       //====process old versions before automatic schema evolution
       StVertex::Streamer(R__b);
       R__b >> (StTrack*&)mDaughter;
       R__b >> mDcaBachelorToPrimaryVertex;
       mMomentumOfBachelor.Streamer(R__b);
       R__b >> mDcaDaughters;
       R__b >> mDcaParentToPrimaryVertex;
       R__b >> (StV0Vertex*&)mV0Vertex;

       R__b.CheckByteCount(R__s, R__c, Class());
       //====end of old versions
      
    } else {
       Class()->WriteBuffer(R__b,this);
    }
} 



    
