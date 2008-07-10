// $Id: StJetBEMCMuDst.cxx,v 1.4 2008/07/10 20:48:58 tai Exp $
#include "StJetBEMCMuDst.h"

#include <StMuDSTMaker/COMMON/StMuDstMaker.h>
#include <StMuDSTMaker/COMMON/StMuDst.h>
#include <StMuDSTMaker/COMMON/StMuEvent.h>

#include <StEvent/StEmcRawHit.h>
#include <StEvent/StEmcCollection.h>
#include <StEvent/StEmcModule.h>
#include <StEvent/StEmcDetector.h>

#include <StEmcUtil/geometry/StEmcGeom.h>

#include <StEmcRawMaker/defines.h>
#include <StEmcRawMaker/StBemcTables.h>

namespace StSpinJet {


StJetBEMCMuDst::StJetBEMCMuDst(StMuDstMaker* uDstMaker, StBemcTables* bemcTables)
  : mMuDstMaker(uDstMaker)
  , _bemcTables(bemcTables)
{

}

StJetBEMCMuDst::StJetBEMCMuDst(StMuDstMaker* uDstMaker, bool doTowerSwapFix)
  : mMuDstMaker(uDstMaker)
  , _bemcTables(new StBemcTables(doTowerSwapFix))
 {

 }


TowerEnergyList StJetBEMCMuDst::getEnergyList()
{
  _bemcTables->loadTables((StMaker*)mMuDstMaker);

  TowerEnergyList ret;

  StEmcDetector* detector = mMuDstMaker->muDst()->emcCollection()->detector(kBarrelEmcTowerId);

  static const int nBemcModules = 120;

  for(int m = 1; m <= nBemcModules; ++m) {

    StSPtrVecEmcRawHit& rawHits = detector->module(m)->hits();

    for(UInt_t k = 0; k < rawHits.size(); ++k) {
  	    
      ret.push_back(readTowerHit(*rawHits[k]));
    }
  }

  return ret;
}

TowerEnergy StJetBEMCMuDst::readTowerHit(const StEmcRawHit& hit)
{
  TowerEnergy ret;

  ret.detectorId = 9;

  int towerId;
  StEmcGeom::instance("bemc")->getId(hit.module(), hit.eta(), abs(hit.sub()), towerId);

  ret.towerId = towerId;

  float towerX, towerY, towerZ;
  StEmcGeom::instance("bemc")->getXYZ(towerId, towerX, towerY, towerZ);

  ret.towerX = towerX;
  ret.towerY = towerY;
  ret.towerZ = towerZ;

  StThreeVectorF vertex = mMuDstMaker->muDst()->event()->primaryVertexPosition();

  ret.vertexX = vertex.x();
  ret.vertexY = vertex.y();
  ret.vertexZ = vertex.z(); 

  ret.energy = hit.energy();
  ret.adc    = hit.adc();

  float pedestal, rms;
  int CAP(0);
  _bemcTables->getPedestal(BTOW, towerId, CAP, pedestal, rms);
  ret.pedestal = pedestal;
  ret.rms = rms;

  int status;
  _bemcTables->getStatus(BTOW, towerId, status);
  ret.status = status;

  return ret;
}


}
