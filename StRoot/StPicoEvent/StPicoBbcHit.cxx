#include "StPicoBbcHit.h"


// \class StPicoBbcHit
// \author Mike Lisa
// \date 6 March 2018
// \brief Holds signals for ***phototubes*** in INNER STAR
//   Beam-beam counter (BBC)

/*************************************************
 * Hit Class for the STAR Beam-Beam Counter (BBC)
 * There is one "hit" for each (good) *phototube* in the detector
 *   that has a nonzero ADC value
 * 
 * Total size of this object is 6 bytes
 * This object contains only
 *  1) the id of the PMT, compacted into a Short_t
 *  2) the QT information (ADC, TDC, TAC, hasTac flag, status flag)
 *     compacted into 32 bits (Int_t)
 * 
 *
 * The BBC is a painful detector, because it has 16 phototubes
 *  distributed to 18 tiles, per side.  This class stores the
 *  data by phototube (as it must).
 *
 * To access GEOMETRICAL information (e.g. location of a tile, etc)
 *  use the StBbcGeom class.  All you have to do is send StBbcGeom
 *  the StPicoBbcHit::id(), and you'll get all you want.
 *
 * - Mike Lisa March 2018
 ************************************************/

#include "StPicoBbcHit.h"

ClassImp(StPicoBbcHit)

//_________________
StPicoBbcHit::StPicoBbcHit() : StPicoBbcHit(0, 0, 0, 0, 0, false, false) {
  /* no-op */
}

//_________________
StPicoBbcHit::StPicoBbcHit(int PMTnumber,
			   int EW, int ADC, int TAC,
			   int TDC, bool hasTAC,
			   bool statusIsGood) :
  mId( std::abs(PMTnumber)*EW ),
  mQTdata( (ADC & 0x0FFF) | (TAC & 0x0FFF) << 12 | (TDC & 0x001F) << 24 | hasTAC << 29 | statusIsGood << 30 ) {
  /* no-op */
}

//_________________
StPicoBbcHit::StPicoBbcHit(const StPicoBbcHit &hit) {
  mId = hit.mId;
  mQTdata = hit.mQTdata;
}

//_________________
StPicoBbcHit::~StPicoBbcHit() {
  /* no-op */
}
