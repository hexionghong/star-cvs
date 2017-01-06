#include <stdio.h>
#include <cmath>
#include "math_constants.h"

#include <StMessMgr.h>

#include "VertexData.h"
//==========================================================
//==========================================================
VertexData::VertexData(int vertexId) {
  id=vertexId;
  r=TVector3(999,999,999);
  gPtSum=0;
  nAnyMatch=nCtb=nBemc=nEemc=nTpc=0;
  nAnyVeto=nCtbV=nBemcV=nEemcV=nTpcV=0;
}


VertexData::VertexData(const TVector3& position) :
  id(0),
  r(position), er(),
  nUsedTrack(0), Lmax(0), gPtSum(0),
  nBtof(0),  nCtb(0),  nBemc(0),  nEemc(0),  nTpc(0),  nAnyMatch(0),
  nBtofV(0), nCtbV(0), nBemcV(0), nEemcV(0), nTpcV(0), nAnyVeto(0)
{
}

//==========================================================
//==========================================================
void VertexData::print(ostream& os) const { // does not work ??
  os << " Vertex ID="<<id<<" nUsedTrack="<<nUsedTrack<<" gPtSum="<< gPtSum<<" Lmax="<< Lmax;
  os << " match: any="<<nAnyMatch<<"-"<<nAnyVeto<<" CTB="<<nCtb<<"-"<<nCtbV<<" BEMC="<<nBemc<<"-"<<nBemcV<<" EEMC="<<nEemc<<"-"<<nEemcV<<" TPC="<<nTpc<<"-"<<nTpcV << endl;
  os << " Vx = " << r.x() << " +/- " << er.x() << endl;
  os << " Vy = " << r.y() << " +/- " << er.y() << endl;
  os << " Vz = " << r.z() << " +/- " << er.z() << endl;
}
