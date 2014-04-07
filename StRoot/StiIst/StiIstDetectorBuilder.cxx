#include <stdio.h>
#include <map>
#include <exception>
#include <stdexcept>

#include "StMessMgr.h"
#include "StThreeVectorD.hh"

#include "Sti/Base/Factory.h"
#include "Sti/StiPlanarShape.h"
#include "Sti/StiCylindricalShape.h"
#include "Sti/StiMaterial.h"
#include "Sti/StiPlacement.h"
#include "Sti/StiDetector.h"
#include "Sti/StiToolkit.h"
#include "Sti/StiIsActiveFunctor.h"
#include "Sti/StiNeverActiveFunctor.h"
#include "Sti/StiElossCalculator.h"
#include "StiIst/StiIstIsActiveFunctor.h"
#include "StiIst/StiIstDetectorBuilder.h"
#include "TDataSetIter.h"
#include "THashList.h"
#include "tables/St_HitError_Table.h"
#include "StEvent/StEvent.h"
#include "StEvent/StEnumerations.h"
#include "StEvent/StEventTypes.h"
#include "StDetectorDbMaker/StiIst1HitErrorCalculator.h"
#include "StIstDbMaker/StIstDbMaker.h"
#include "StIstUtil/StIstConsts.h"
#include "StiMaker/StiDetectorVolume.h"
#include "StBFChain/StBFChain.h"

using namespace std;
using namespace StIstConsts;


StiIstDetectorBuilder::StiIstDetectorBuilder(bool active, const string &inputFile) :
   StiDetectorBuilder("Ist", active, inputFile), mSiMaterial(0),
   mHybridMaterial(0), mIstDb(0), mTestGeomType(kDefault)
{
   StBFChain *chain = (StBFChain *) StMaker::GetChain();

   if (chain) {
      if ( chain->GetOption("StiIstCrude") )
         mTestGeomType = kCrude;
   }
}


void StiIstDetectorBuilder::buildDetectors(StMaker &source)
{
   LOG_INFO << "StiIstDetectorBuilder::buildDetectors() -I- Started " << endm;

   setNRows(1);

   // XXX:ds: Cannot rely on external maker! Must access DbMaker through
   // source.GetDataSet("istDb")
   mIstDb = (StIstDbMaker*) source.GetMaker("istDb");

   if (StiVMCToolKit::GetVMC()) {

      switch (mTestGeomType) {
      case kCrude:
         buildInactiveVolumes();
         break;
      default:
         useVMCGeometry();
      }
   }

   // XXX: Check sti geometry by converting it to drawable root objects
   TFile fileTmp("sti2rootgeo_ist.root", "RECREATE");
   StiDetectorVolume stiDetVol(*this);
   stiDetVol.Write();
   fileTmp.Close();
}


void StiIstDetectorBuilder::useVMCGeometry()
{
   cout << "StiIstDetectorBuilder::buildDetectors() -I- Use VMC geometry" << endl;

   THashList *istRot = new THashList(144, 0);
   istRot = mIstDb->GetRotations();
   unsigned int ROW = 1;

   SetCurrentDetectorBuilder(this);


   // Build the material map
   struct Material_t {
      const Char_t *name;
      StiMaterial    **p;
   };

   Material_t map[] = {
      {"AIR",     &_gasMat},
      {"SILICON", &mSiMaterial},
      {"SILICON", &mHybridMaterial}
   };

   Int_t M = sizeof(map) / sizeof(Material_t);

   for (Int_t i = 0; i < M; i++) {
      const TGeoMaterial *mat =  gGeoManager->GetMaterial(map[i].name);

      if (! mat) continue;

      Double_t PotI = StiVMCToolKit::GetPotI(mat);
      *map[i].p = add(new StiMaterial(mat->GetName(),
                                      mat->GetZ(),
                                      mat->GetA(),
                                      mat->GetDensity(),
                                      mat->GetDensity()*mat->GetRadLen(),
                                      PotI));
   }

   double ionization = mSiMaterial->getIonization();

   StiElossCalculator *ElossCalculator = new StiElossCalculator(mSiMaterial->getZOverA(),
         ionization * ionization,
         mSiMaterial->getA(), mSiMaterial->getZ(), mSiMaterial->getDensity());


   for (int ladderIdx = 0; ladderIdx < kIstNumLadders; ++ladderIdx)
   {
      for (int sensorIdx = 0; sensorIdx < kIstNumSensorsPerLadder; sensorIdx++)
      {
         unsigned int matIst = 1000 + (ladderIdx) * 6 + (sensorIdx + 1);
         LOG_DEBUG << "ladderIdx/sensorIdx/matIst : " << ladderIdx << " " << sensorIdx << " " << matIst << endm;
         TGeoHMatrix *combI = (TGeoHMatrix*) istRot->FindObject(Form("R%04i", matIst));

         if (combI) {
            combI->Print();
         } else {
            Error("useVMCGeometry()", "Could not find TGeoHMatrix for sensor %d in database", matIst);
            continue;
         }

         //jb added
         if (sensorIdx != 0) continue;

         //we place the ladder as a whole
         char name[50];
         sprintf(name, "Ist/Ladder_%d/Sensor_%d", ladderIdx + 1, sensorIdx + 1);

         TString Path("HALL_1/CAVE_1/TpcRefSys_1/IDSM_1/IBMO_1");
         Path += Form("/IBAM_%d/IBLM_%d/IBSS_1", ladderIdx + 1, sensorIdx + 1);

         gGeoManager->cd(Path); // retrieve info of IBSS volume
         TGeoNode *nodeT = gGeoManager->GetCurrentNode();
         // Extract volume geometry for this node
         TGeoBBox *box = (TGeoBBox *) nodeT->GetVolume()->GetShape();
         LOG_DEBUG << " DZ/DY/DX : " << box->GetDZ()
                   << " " << box->GetDY()
                   << " " << box->GetDX()
                   << " " << endm;

         //IBSS shape : DX =1.9008cm ; DY = .015cm ; DZ = 3.765 cm
         StiShape *sh  = new StiPlanarShape(name,
                                            kIstNumSensorsPerLadder * (box->GetDZ() + 0.10), // halfDepth + deadedge 0.16/2 + sensor gap 0.04/2
                                            2 * box->GetDY(),              // thickness
                                            box->GetDX());                 // halfWidth
         add(sh);

         Double_t     *xyz    = combI->GetTranslation();
         Double_t     *rot    = combI->GetRotationMatrix();
         StThreeVectorD centerVector(xyz[0], xyz[1], xyz[2]);
         StThreeVectorD normalVector(rot[1], rot[4], rot[7]);

         Double_t prod = centerVector * normalVector;

         if (prod < 0) normalVector *= -1;

         // Normalize normal vector, just in case....
         normalVector /= normalVector.magnitude();

         // Volume positioning
         StiPlacement *pPlacement = new StiPlacement;
         Double_t phi  = centerVector.phi();
         Double_t phiD = normalVector.phi();
         Double_t r    = centerVector.perp();
         pPlacement->setZcenter(0);
         pPlacement->setLayerRadius(r);

         pPlacement->setLayerAngle(phi);
         pPlacement->setRegion(StiPlacement::kMidRapidity);
         pPlacement->setNormalRep(phiD, r * TMath::Cos(phi - phiD), r * TMath::Sin(phi - phiD));
         assert(pPlacement);

         //Build final detector object
         StiDetector *p = getDetectorFactory()->getInstance();

         if ( !p ) {
            LOG_INFO << "StiIstDetectorBuilder::AverageVolume() -E- StiDetector pointer invalid." << endm;
            return;
         }

         p->setName(name);
         p->setIsOn(kTRUE);
         p->setIsActive(new StiIstIsActiveFunctor);

         p->setIsContinuousMedium(false);
         p->setIsDiscreteScatterer(true);
         p->setShape(sh);
         p->setPlacement(pPlacement);
         p->setGas(GetCurrentDetectorBuilder()->getGasMat());

         if (!p->getGas()) LOG_INFO << "gas not there!" << endm;

         p->setMaterial(mSiMaterial);
         p->setElossCalculator(ElossCalculator);
         p->setHitErrorCalculator(StiIst1HitErrorCalculator::instance());

         // Adding detector, note that no keys are set in IST!
         add(ROW, ladderIdx + 1, p);

         // Whole bunch of debugging information
         Float_t rad2deg = 180.0 / 3.1415927;
         LOG_DEBUG << "===>NEW:IST:pDetector:Name               = " << p->getName()                               << endm;
         LOG_DEBUG << "===>NEW:IST:pPlacement:NormalRefAngle    = " << pPlacement->getNormalRefAngle()*rad2deg    << endm;
         LOG_DEBUG << "===>NEW:IST:pPlacement:NormalRadius      = " << pPlacement->getNormalRadius()              << endm;
         LOG_DEBUG << "===>NEW:IST:pPlacement:NormalYoffset     = " << pPlacement->getNormalYoffset()             << endm;
         LOG_DEBUG << "===>NEW:IST:pPlacement:CenterRefAngle    = " << pPlacement->getCenterRefAngle()*rad2deg    << endm;
         LOG_DEBUG << "===>NEW:IST:pPlacement:CenterRadius      = " << pPlacement->getCenterRadius()              << endm;
         LOG_DEBUG << "===>NEW:IST:pPlacement:CenterOrientation = " << pPlacement->getCenterOrientation()*rad2deg << endm;
         LOG_DEBUG << "===>NEW:IST:pPlacement:LayerRadius       = " << pPlacement->getLayerRadius()               << endm;
         LOG_DEBUG << "===>NEW:IST:pPlacement:LayerAngle        = " << pPlacement->getLayerAngle()*rad2deg        << endm;
         LOG_DEBUG << "===>NEW:IST:pPlacement:Zcenter           = " << pPlacement->getZcenter()                   << endm;
         LOG_DEBUG << "===>NEW:IST:pDetector:Ladder             = " << ladderIdx + 1                                << endm;
         LOG_DEBUG << "===>NEW:IST:pDetector:Sensor             = " << sensorIdx + 1                                << endm;
         LOG_DEBUG << "===>NEW:IST:pDetector:row/ladder (ITTF)  = " << ROW << " / " << ladderIdx + 1                 << endm;
         LOG_DEBUG << "===>NEW:IST:pDetector:Active?            = " << p->isActive()                              << endm;
      }
   }

   // Build the volume map and loop over all found volumes
   Int_t NoIstVols = sizeof(IstVolumes) / sizeof(VolumeMap_t);
   gGeoManager->RestoreMasterVolume();
   gGeoManager->CdTop();

   for (Int_t i = 0; i < NoIstVols; i++) {
      gGeoManager->cd(IstVolumes[i].path);
      TGeoNode *nodeT = gGeoManager->GetCurrentNode();

      if (! nodeT) continue;;

      StiVMCToolKit::LoopOverNodes(nodeT, IstVolumes[i].path, IstVolumes[i].name, MakeAverageVolume);
   }
}
