/***************************************************************************
 *
 * $Id: StV0Vertex.hh,v 1.1 1999/01/15 20:40:19 wenaus Exp $
 *
 * Author: Thomas Ullrich, Jan 1999
 ***************************************************************************
 *
 * Description:
 *
 ***************************************************************************
 *
 * $Log: StV0Vertex.hh,v $
 * Revision 1.1  1999/01/15 20:40:19  wenaus
 * Commit Thomas' original code
 *
 * table load intfc change; include ref change
 *
 * Revision 1.3  1999/01/27 13:04:50  ullrich
#include "StVertex.hh"
 *
 * Revision 1.2  1999/01/15 22:54:16  wenaus
#include "StTables/dst_v0_vertex.h"
 *
 **************************************************************************/
#include "StEvent/StVertex.hh"
#include "tables/dst_vertex.h"
#include "tables/dst_v0_vertex.h"
#include <float.h>

class StV0Vertex : public StVertex {
    
    float dcaDaughterToV0 (unsigned int i) const;
    ~StV0Vertex();
    float dcaParentToV0() const;
    float dcaDaughterToPrimaryVertex(unsigned int i) const;
    void setDcaDaughtersToV0(unsigned int, float);

    void setDcaParentToV0(float);
    void setDcaDaughtersToPrimaryVertex(unsigned int, float);
    void setMomentumOfDaughter(StTrackSign sign, const StThreeVector<float>&);
    float mDcaDaughtersToV0[2];
    void setDcaParentToPrimaryVertex(float);
    float mDcaParentToV0;
    float mDcaDaughtersToPrimaryVertex[2];
    float mDcaDaughters;
inline float StV0Vertex::dcaDaughterToV0 (unsigned int i) const
    StThreeVector<float> mMomentumOfDaughters[2];
    float                mDcaDaughters;
	return mDcaDaughtersToV0[i];
};
    if (i < 2)
	return mDcaDaughtersToPrimaryVertex[i];
    else
	return FLT_MAX;
StV0Vertex::momentumOfDaughter(StTrackSign sign) const
inline float StV0Vertex::dcaParentToV0() const { return mDcaParentToV0; }
    return mMomentumOfDaughters[sign];
}

inline float StV0Vertex::dcaDaughters() const { return mDcaDaughters; }

inline float StV0Vertex::dcaParentToPrimaryVertex() const { return mDcaParentToPrimaryVertex; }

#endif
