/***************************************************************************
 *
 * $Id: StTpcDb.cxx,v 1.13 1999/12/16 22:00:53 hardtke Exp $
 *
 * Author:  David Hardtke
 ***************************************************************************
 *
 * Description: This is the interface between the database and the offline
 *              TPC software.  This classes takes care of the annoying
 *              calls to the root infrastucture, packages and manipulates
 *              the data, and returns the data to the user via simple
 *              interface classes.    
 *
 ***************************************************************************
 *
 * $Log: StTpcDb.cxx,v $
 * Revision 1.13  1999/12/16 22:00:53  hardtke
 * add CVS tags
 *
 **************************************************************************/
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// StTpcDb                                                              //
//                                                                      //
//                                                                      //
 
#include "StMaker.h"
#include "St_Table.h"
#include "StTpcDb.h"

StTpcDb* gStTpcDb = 0;

//
// C and Fortran routines:
//________________________________________
int type_of_call numberOfPadsAtRow_(int *row) {
    return gStTpcDb->PadPlaneGeometry()->numberOfPadsAtRow(*row);
}
//
// C++ routines:
//_____________________________________________________________________________


#ifdef __ROOT__
ClassImp(StTpcDb)
#endif
//_____________________________________________________________________________
StTpcDb::StTpcDb(St_DataSet* input) {
 assert(gStTpcDb==0);
 memset(this,0,sizeof(StTpcDb));
 if (input){
   const Char_t *bases[] = {"Calibrations","Geometry","Conditions"};
   int lBases = sizeof(bases)/sizeof(Char_t *);
   St_DataSetIter dataBase(input);
   for (int i = 0;i<lBases;i++,dataBase.Cd("/") )
     if ( !(tpc[i] = dataBase.Cd(bases[i]) ? dataBase("tpc") : 0 ) ){
      char message[80];
      cout << message << "StTpcDb::Error Getting TPC database: " << bases[i];
      gMessMgr->Message(message,"E");}
 }
 else{
   gMessMgr->Message("StTpcDb::Error Creating StTpcDb: Need to specify input DataSet","E");
 }
 gStTpcDb = this;
}

//_____________________________________________________________________________
StTpcDb::StTpcDb(StMaker* maker) {
 assert(gStTpcDb==0);
 memset(this,0,sizeof(StTpcDb));
 if (maker) GetDataBase(maker);
 gStTpcDb = this;
}

//_____________________________________________________________________________
void StTpcDb::GetDataBase(StMaker* maker) {
 if (maker){
   const Char_t *bases[] = {"Calibrations/","Geometry/","Conditions/"};
   int lBases = sizeof(bases)/sizeof(Char_t *);
   for (int i = 0;i<lBases;i++) {
     TString dbFullPath = "StDb/";
     TString dbPath = bases[i];
     dbPath += "tpc";
     dbFullPath += dbPath;
     if ( ( tpc[i] = maker->GetDataBase(dbPath)) || 
          ( tpc[i] = maker->GetDataBase(dbFullPath)) ) continue;
      char message[80];
      cout << message << "StTpcDb::Error Getting TPC database: " << bases[i] << "   ";
      gMessMgr->Message(message,"E");
   //     gMessMgr->Message("StTpcDb::Error Getting TPC Calibrations database","E");
   }
 }
 else{
   gMessMgr->Message("StTpcDb::Error Getting TPC database","E");
 }
}

//_____________________________________________________________________________
StTpcDb::~StTpcDb() {
  //delete PadPlane;
  //delete WirePlane;
  //delete dimensions;
  //delete slowControlSim;
  //delete electronics;

for (int i = 0;i<24;i++) {
  //    delete gain[i]; delete t0[i];
}

gStTpcDb = 0;
}
//_____________________________________________________________________________

StTpcPadPlaneI* StTpcDb::PadPlaneGeometry(){
  if (!PadPlane){            // get pad plane from data base
   const int dbIndex = kGeometry;
   if (tpc[dbIndex]){
    St_DataSet *tpd = tpc[dbIndex]->Find("tpcPadPlanes");
    if (!tpd) {
      gMessMgr->Message("StTpcDb::Error Finding Tpc Pad Planes","E");
      return 0;
    }   
   PadPlane = new StRTpcPadPlane((St_tpcPadPlanes*)tpd);
   }
  }
  return PadPlane;
}

//_____________________________________________________________________________
StTpcWirePlaneI* StTpcDb::WirePlaneGeometry(){
  if (!WirePlane){            // get wire plane from data base
   const int dbIndex = kGeometry;
   if (tpc[dbIndex]){
    St_DataSet* tpd = tpc[dbIndex]->Find("tpcWirePlanes");
    if (!(tpd && tpd->HasData()) ){
     gMessMgr->Message("StTpcDb::Error Finding Tpc Wire Planes","E");
     return 0;
    }   
    WirePlane = new StRTpcWirePlane((St_tpcWirePlanes*)tpd);
   }
  }
 return WirePlane;
}

//_____________________________________________________________________________
StTpcDimensionsI* StTpcDb::Dimensions(){
  if (!dimensions){            // get wire plane from data base
   const int dbIndex = kGeometry;
   if (tpc[dbIndex]){
    St_DataSet* tpd = tpc[dbIndex]->Find("tpcDimensions");
    if (!(tpd && tpd->HasData()) ){
     gMessMgr->Message("StTpcDb::Error Finding Tpc Dimensions","E");
     return 0;
    }
    dimensions =  new StRTpcDimensions((St_tpcDimensions*)tpd);
   }
  }
 return dimensions;
}

//_____________________________________________________________________________
StTpcSlowControlSimI* StTpcDb::SlowControlSim(){
  if (!slowControlSim){            // get wire plane from data base
   const int dbIndex = kCalibrarion;
   if (tpc[dbIndex]){
    St_DataSet* tpd = tpc[dbIndex]->Find("tpcSlowControlSim");
    if (!(tpd && tpd->HasData()) ){
     gMessMgr->Message("StTpcDb::Error Finding Slow Control Simulations Parameters","E");
     return 0;
    }
   slowControlSim = new StRTpcSlowControlSim((St_tpcSlowControlSim*)tpd);
   }
  }
 return slowControlSim;
}

//_____________________________________________________________________________
StTpcElectronicsI* StTpcDb::Electronics(){
  if (!electronics){            // get electronics from data base
   const int dbIndex = kGeometry;
   if (tpc[dbIndex]){
    St_DataSet* tpd = tpc[dbIndex]->Find("tpcElectronics");
    if (!(tpd && tpd->HasData()) ){
     gMessMgr->Message("StTpcDb::Error Finding Tpc Electronics","E");
     return 0;
    }
    electronics = new StRTpcElectronics((St_tpcElectronics*)tpd);
   }
  }
 return electronics;
}

//_____________________________________________________________________________
StTpcGainI* StTpcDb::Gain(int sector){
  if(sector<1||sector>24){
    gMessMgr->Message("StTpcDb::Gains request for invalid sector","E");
    return 0;
  }
  if(!gain[sector-1]){
   const int dbIndex = kCalibrarion;
   char dbname[25];
   sprintf(dbname,"Sector_%.2d/tpcGainFactors",sector);
   printf("Getting %s \n",dbname);
   if (tpc[dbIndex]){
    St_DataSet* tpd = tpc[dbIndex]->Find(dbname);
    if (!(tpd && tpd->HasData()) ){
     gMessMgr->Message("StTpcDb::Error Finding Tpc Gain Factors","E");
     return 0;
    }
    StRTpcGain* wptemp = new StRTpcGain((St_tpcGainFactors* )tpd);
    wptemp->SetPadPlanePointer(PadPlaneGeometry());
    gain[sector-1] = (StTpcGainI*)wptemp;
   }
  }
 return gain[sector-1];
}

//_____________________________________________________________________________
StTpcT0I* StTpcDb::T0(int sector){
  if(sector<1||sector>24){
    gMessMgr->Message("StTpcDb::T0s request for invalid sector","E");
    return 0;
  }
  if(!t0[sector-1]){
   const int dbIndex = kCalibrarion;
   char dbname[25];
   sprintf(dbname,"Sector_%.2d/tpcTimeOffsets",sector);
   printf("Getting %s \n",dbname);
   if (tpc[dbIndex]){
    St_DataSet* tpd = (St_DataSet*)tpc[dbIndex]->Find(dbname);
    if (!(tpd && tpd->HasData()) ){
     gMessMgr->Message("StTpcDb::Error Finding Tpc Time Offsets","E");
     return 0;
    }
    StRTpcT0* wptemp = new StRTpcT0((St_tpcTimeOffsets*)tpd);
    wptemp->SetPadPlanePointer(PadPlaneGeometry());
    t0[sector-1] = (StTpcT0I*)wptemp;
   }
  }
 return t0[sector-1];
}

//_____________________________________________________________________________
St_Table *StTpcDb::getTpcTable(int i){
  return (St_Table *)tpc[i];
}





