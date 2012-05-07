/*
 * Implementation file for StMuPrimaryVertex
 * A simple class to store porimary vertex information
 * All functions are inline
 *
 * $Id: StMuPrimaryVertex.cxx,v 1.8 2012/05/07 14:47:06 fisyak Exp $ 
 */

#include "StMuPrimaryVertex.h"
#include "StEvent/StPrimaryVertex.h"
#include "StEventUtilities/StuRefMult.hh"
#include "StEventUtilities/StuFtpcRefMult.hh"
#include "TString.h"
ClassImp(StMuPrimaryVertex)
  
StMuPrimaryVertex::StMuPrimaryVertex(const StPrimaryVertex*& vertex) {
  mFlag           = vertex->flag();
  mPosition       = vertex->position();
  mPosError       = vertex->positionError();
  mVertexFinderId = vertex->vertexFinderId();
  mRanking        = vertex->ranking();
  mNTracksUsed    = vertex->numTracksUsedInFinder();
  mNBTOFMatch     = vertex->numMatchesWithBTOF();
  mNCTBMatch      = vertex->numMatchesWithCTB();
  mNBEMCMatch     = vertex->numMatchesWithBEMC();
  mNEEMCMatch     = vertex->numMatchesWithEEMC();

  mNBTOFNotMatch     = vertex->numNotMatchesWithBTOF();
  mNCTBNotMatch      = vertex->numNotMatchesWithCTB();
  mNBEMCNotMatch     = vertex->numNotMatchesWithBEMC();
  mNEEMCNotMatch     = vertex->numNotMatchesWithEEMC();

  mNCrossCentralMembrane = vertex->numTracksCrossingCentralMembrane();
  mNTracksWithPromptHit  = vertex->numTracksWithPromptHit();
  mNPostXTracks   = vertex->numPostXTracks();
  mSumTrackPt     = vertex->sumOfTrackPt();
  mMeanDip        = vertex->meanDip();
  mChiSquared     = vertex->chiSquared();
  mNTracks    = vertex->numberOfDaughters();
  mRefMultPos = uncorrectedNumberOfPositivePrimaries(vertex);
  mRefMultNeg = uncorrectedNumberOfNegativePrimaries(vertex); 
  mRefMultFtpcEast = uncorrectedNumberOfFtpcEastPrimaries(vertex);
  mRefMultFtpcWest = uncorrectedNumberOfFtpcWestPrimaries(vertex); 

  mIdTruth  = vertex->idTruth();
  mQuality  = vertex->qaTruth();
  mIdParent = vertex->idParent();
}
#if 0
void StMuPrimaryVertex::Print(Option_t *option) const {
  cout << "Vertex position " << mPosition << endl;
  cout << "       errors   " << mPosError << endl;
  cout << "Rank        " << mRanking << endl;
  cout << "Chisquared  " << mChiSquared << endl;
  cout << "RefMult     " << refMult() << " ( pos " << mRefMultPos << ", neg " 
       << mRefMultNeg << " )" << endl;
  cout << "Tracks used " << mNTracksUsed << endl;
  cout << "Mean dip    " << mMeanDip << endl;
}
#endif
//________________________________________________________________________________
ostream&              operator<<(ostream& os,  const StMuPrimaryVertex& v) {
  const Char_t *beam = (v.isBeamConstrained()) ? "B" : " ";
  os << Form("%1s:",beam);
  if (v.nPostXtracks() < 10) os << Form("%i/",v.nPostXtracks());
  else                                os <<       "*/";
  if (v.nPromptTracks() < 10) os << Form("%i/",v.nPromptTracks());
  else                                os <<       "*/";
  if (v.nCrossCentralMembrane() < 10) os << Form("%i/",v.nCrossCentralMembrane());
  else                                os <<       "*/";
  if ((v.nBEMCMatch()+v.nBTOFMatch()) < 10) os << Form("%i/",(v.nBEMCMatch()+v.nBTOFMatch()));
  else                                os <<       "*/";
  if ((v.nBEMCMatch()+v.nEEMCMatch()) < 10) os << Form("%i",(v.nBEMCMatch()+v.nEEMCMatch()));
  else                                os <<       "*";
  os << Form(" %8.3f+/-%6.3f %8.3f+/-%6.3f %8.3f+/-%6.3f",
	     v.position().x(),v.posError().x(),
	     v.position().y(),v.posError().y(),
	     v.position().z(),v.posError().z());
  os << Form(" Rank:%6.0f",v.ranking());
  os << Form(" M:%4i:R%4i",v.noTracks(),v.refMult());
  //  os << Form(" RefMult:%4i(%4i%4i)",v.refMult(),v.refMultPos(),-v.refMultNeg());
  os << Form(" U:%4i ",v.nTracksUsed());
  os << Form(" QA:%3i",v.qaTruth());
  return os;
}
//________________________________________________________________________________
void StMuPrimaryVertex::Print(Option_t *option) const {cout << *this << endl;}
//________________________________________________________________________________
// $Log: StMuPrimaryVertex.cxx,v $
// Revision 1.8  2012/05/07 14:47:06  fisyak
// Add handles for track to fast detector matching
//
// Revision 1.7  2011/10/17 00:19:14  fisyak
// Active handing of IdTruth
//
