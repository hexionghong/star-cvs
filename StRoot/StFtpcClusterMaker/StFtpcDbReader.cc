// $Id: StFtpcDbReader.cc,v 1.34 2005/12/12 14:46:53 jcs Exp $
//
// $Log: StFtpcDbReader.cc,v $
// Revision 1.34  2005/12/12 14:46:53  jcs
// comment out debug output
//
// Revision 1.33  2005/12/12 13:40:12  jcs
// simplify StFtpcDbReader
//
// Revision 1.32  2005/10/26 14:03:43  jcs
// Change setMicrosecondsPerTimebin
//
// Revision 1.31  2005/10/25 14:38:28  jcs
// correct electronicsTable definition used for StFtpcSlowSimMaker
//
// Revision 1.30  2005/10/24 13:43:01  jcs
// If microsecondsPerTimebin calculated from RHIC clock frequency, store in
// Calibrations_ftpc/ftpcElectronics database table. Otherwise get default
// value from Calibrations_ftpc/ftpcElectronics database table.
//
// Revision 1.29  2005/04/13 08:14:17  jcs
// Invert sector numbering for Ftpc East when accessing the gain table (ftpcAmpSlope)
// since this wasn't done when the gain table was produced
//
// Revision 1.28  2004/07/18 14:10:09  jcs
// get adjustAverageWest/East from Calibrations_ftpc/ftpcGas
//
// Revision 1.27  2004/01/28 01:41:15  jeromel
// Change OST to OS everywhere since defaultoption is now not to print
// the date.
//
// Revision 1.26  2003/07/03 13:21:50  fsimon
// Added cathode offset information to constructor for SlowSimulator
//
// Revision 1.25  2003/06/12 10:01:25  jcs
// renamed ftpcClusterGeometry database table to ftpcClusterGeom
// (name was too long)
//
// Revision 1.24  2003/06/11 12:06:03  jcs
// get inner cathode and cluster geometry parameters from database
//
// Revision 1.23  2003/06/10 13:13:51  jcs
// get mix,max gas temperature and pressure limits from database
//
// Revision 1.22  2003/05/06 20:19:40  mora
// Add a new constructor only with FTPC dimensions and geometry for ITTF
//
// Revision 1.21  2003/02/27 22:53:12  jcs
// make default temperature values from database available to the FTPC slow simulator (needed for embedding)
//
// Revision 1.20  2003/02/19 14:51:46  jcs
// get default temperatures from database
//
// Revision 1.19  2003/01/15 10:39:31  jcs
// Correct error in message
//
// Revision 1.18  2003/01/14 12:58:01  jcs
// use Geometry_ftpc/ftpcAsicMap to control corrections for error in Y2001-2002
// FTPC asic mapping
//
// Revision 1.17  2003/01/07 16:15:18  jcs
// get all values in ftpcGas table for cluster finding
//
// Revision 1.16  2002/10/16 12:25:13  fsimon
// Cleanup: Use gMessMgr instead of cout
//
// Revision 1.15  2002/10/15 09:46:44  fsimon
// Constructor used by SlowSimulator changed to include Db access to
// ftpcAmpSlope, ftpcAmpOffset and ftpcTimeOffset
//
// Revision 1.14  2002/10/11 15:37:36  oldi
// Minor cosmetics.
//
// Revision 1.13  2002/01/31 19:35:38  jcs
// get temperatureDifference from data base
//
// Revision 1.12  2002/01/21 22:12:53  jcs
// add gas temperature difference between west and east FTPC to database
//
// Revision 1.11  2001/11/21 12:36:27  jcs
// make ftpcGas database table available to FTPC cluster maker
//
// Revision 1.10  2001/10/29 12:54:43  jcs
// add new constructor for StFtpcDriftMapMaker
//
// Revision 1.9  2001/10/19 09:40:11  jcs
// tZero now in data base in ftpcElectronics
//
// Revision 1.8  2001/08/21 19:51:06  jcs
// correct value of sizeOfTimebin now in MySQL database - remove from code
//
// Revision 1.7  2001/08/16 18:32:20  jcs
// set value of tzero = 1.937
//
// Revision 1.6  2001/07/12 10:26:34  jcs
// temporarily set mMicrosecondsPerTimeBin and mTZero in code instead of in MySQLDB
//
// Revision 1.5  2001/07/11 21:18:02  jcs
// changes for new FTPC database structures
//
// Revision 1.4  2001/04/04 17:08:42  jcs
// remove references to StFtpcParamReader from StFtpcDbReader
//
// Revision 1.3  2001/04/02 12:10:22  jcs
// get FTPC calibrations,geometry from MySQL database and code parameters
// from StarDb/ftpc
//
// Revision 1.2  2001/03/19 15:52:47  jcs
// use ftpcDimensions from database
//
// Revision 1.1  2001/03/06 23:34:00  jcs
// use database instead of params
//
//

#include "StFtpcDbReader.hh"
#include "StMessMgr.h"

// for StFtpcClusterMaker
StFtpcDbReader::StFtpcDbReader(St_ftpcDimensions    *dimensions,
                               St_ftpcPadrowZ       *zrow,
			       St_ftpcAsicMap       *asicmap,
                               St_ftpcEField        *efield,
                               St_ftpcVDrift        *vdrift,
                               St_ftpcDeflection    *deflection,
                               St_ftpcdVDriftdP     *dvdriftdp,
                               St_ftpcdDeflectiondP *ddeflectiondp,
                               St_ftpcAmpSlope      *ampslope,
                               St_ftpcAmpOffset     *ampoffset,
                               St_ftpcTimeOffset    *timeoffset,
                               St_ftpcDriftField    *driftfield,
                               St_ftpcGas           *gas,
                               St_ftpcElectronics   *electronics,
			       St_ftpcInnerCathode  *cathode,
			       St_ftpcClusterGeom *clustergeo)
{

  returnCode = kStOK;
  returnCode += FtpcDimensions(dimensions);
  returnCode += FtpcPadrowZ(zrow);
  returnCode += FtpcAsicMap(asicmap);
  returnCode += FtpcEField(efield);
  returnCode += FtpcVDrift(vdrift);
  returnCode += FtpcDeflection(deflection);
  returnCode += FtpcdVDriftdP(dvdriftdp);
  returnCode += FtpcdDeflectiondP(ddeflectiondp);
  returnCode += FtpcAmpSlope(ampslope);
  returnCode += FtpcAmpOffset(ampoffset);
  returnCode += FtpcTimeOffset(timeoffset);
  returnCode += FtpcDriftField(driftfield);
  returnCode += FtpcGas(gas);
  returnCode += FtpcElectronics(electronics);
  returnCode += FtpcInnerCathode(cathode);
  returnCode += FtpcClusterGeom(clustergeo);

  //gMessMgr->Message("StFtpcDbReader constructed for StFtpcClusterMaker"  ,"I"); 

}

// for StFtpcSlowSimMaker
StFtpcDbReader::StFtpcDbReader(St_ftpcDimensions    *dimensions,
			       St_ftpcAsicMap       *asicmap,
                               St_ftpcEField        *efield,
                               St_ftpcVDrift        *vdrift,
                               St_ftpcDeflection    *deflection,
                               St_ftpcdVDriftdP     *dvdriftdp,
                               St_ftpcdDeflectiondP *ddeflectiondp,
                               St_ftpcGas           *gas,
                               St_ftpcDriftField    *driftfield,
                               St_ftpcElectronics   *electronics,
			       St_ftpcAmpSlope      *ampslope,
			       St_ftpcAmpOffset     *ampoffset,
			       St_ftpcTimeOffset    *timeoffset,
			       St_ftpcInnerCathode  *cathode)
{


  returnCode = kStOK;
  returnCode += FtpcDimensions(dimensions);
  returnCode += FtpcAsicMap(asicmap);
  returnCode += FtpcEField(efield);
  returnCode += FtpcVDrift(vdrift);
  returnCode += FtpcDeflection(deflection);
  returnCode += FtpcdVDriftdP(dvdriftdp);
  returnCode += FtpcdDeflectiondP(ddeflectiondp);
  returnCode += FtpcGas(gas);
  returnCode += FtpcDriftField(driftfield);
  returnCode += FtpcElectronics(electronics);
  returnCode += FtpcAmpSlope(ampslope);
  returnCode += FtpcAmpOffset(ampoffset);
  returnCode += FtpcTimeOffset(timeoffset);
  returnCode += FtpcInnerCathode(cathode);

  //gMessMgr->Message("StFtpcDbReader constructed for StFtpcSlowSimMaker"  ,"I"); 
}

// for StFtpcDriftMapMaker
StFtpcDbReader::StFtpcDbReader(St_ftpcDimensions    *dimensions,
                               St_ftpcPadrowZ       *zrow,
                               St_ftpcEField        *efield,
                               St_ftpcVDrift        *vdrift,
                               St_ftpcDeflection    *deflection,
                               St_ftpcdVDriftdP     *dvdriftdp,
                               St_ftpcdDeflectiondP *ddeflectiondp,
                               St_ftpcGas           *gas,
                               St_ftpcDriftField    *driftfield)
{


  returnCode = kStOK;
  returnCode += FtpcDimensions(dimensions);
  returnCode += FtpcPadrowZ(zrow);
  returnCode += FtpcEField(efield);
  returnCode += FtpcVDrift(vdrift);
  returnCode += FtpcDeflection(deflection);
  returnCode += FtpcdVDriftdP(dvdriftdp);
  returnCode += FtpcdDeflectiondP(ddeflectiondp);
  returnCode += FtpcGas(gas);
  returnCode += FtpcDriftField(driftfield);

  //gMessMgr->Message("StFtpcDbReader constructed for StFtpcDriftMapMaker"  ,"I"); 
}
///////////////////////////////////////////////////////////////////////////////////////
// for Sti/StFtpcDetectorBuilder
StFtpcDbReader::StFtpcDbReader(St_ftpcDimensions    *dimensions,
                               St_ftpcPadrowZ       *zrow         )
{


  returnCode = kStOK;
  returnCode += FtpcDimensions(dimensions);
  returnCode += FtpcPadrowZ(zrow);

  //gMessMgr->Message("StFtpcDbReader constructed for Sti/StFtpcDetectorBuilder"  ,"I"); 
 
}

StFtpcDbReader::~StFtpcDbReader()
{
//   cout << "StFtpcDbReader destructed" << endl;
}

//===============================================================

Int_t StFtpcDbReader::FtpcDimensions(St_ftpcDimensions *dimensions)
{

  //  just copy dimensions table start to pointer
  ftpcDimensions_st* dimensionsTable = (ftpcDimensions_st*)dimensions->GetTable();
  if(dimensionsTable){
    mNumberOfPadrows            = dimensionsTable->totalNumberOfPadrows;
    mNumberOfPadrowsPerSide     = dimensionsTable->numberOfPadrowsPerSide;
    mFirstPadrowToSearch        = dimensionsTable->firstPadrowToSearch;
    mLastPadrowToSearch         = dimensionsTable->lastPadrowToSearch;   
    mNumberOfSectors            = dimensionsTable->numberOfSectorsPerPadrow;
    mFirstSectorToSearch        = dimensionsTable->firstSectorToSearch;
    mLastSectorToSearch         = dimensionsTable->lastSectorToSearch;
    mPhiOrigin                  = dimensionsTable->phiOrigin;
    mPhiPerSector               = dimensionsTable->phiPerSector;
    mNumberOfPads               = dimensionsTable->numberOfPadsPerSector;
    mPadLength                  = dimensionsTable->padLength;
    mRadiansPerPad              = dimensionsTable->radiansPerPad;
    mRadiansPerBoundary         = dimensionsTable->radiansPerGap;
    mNumberOfTimebins           = dimensionsTable->numberOfTimebinsPerSector;
    mSensitiveVolumeInnerRadius = dimensionsTable->innerRadiusSensitiveVolume;
    mSensitiveVolumeOuterRadius = dimensionsTable->outerRadiusSensitiveVolume;
   return kStOK;
  } else {
    gMessMgr->Message( " No data in table class St_ftpcDimensions","E");
    return kStWarn;
  }
}

//----------------------------------------------------------------------------

Int_t StFtpcDbReader::FtpcPadrowZ(St_ftpcPadrowZ *zrow)
{

  //  just copy zrow table start to pointer
  ftpcPadrowZ_st* padrowzTable = (ftpcPadrowZ_st*)zrow->GetTable();
  if(padrowzTable){
     mPadrowZPosition = (Float_t *)padrowzTable->z;
     return kStOK;
  } else {
     gMessMgr->Message( " No data in table class St_ftpcPadrowZ","E");
     return kStWarn;
  }
}

//----------------------------------------------------------------------------

Int_t StFtpcDbReader::FtpcAsicMap(St_ftpcAsicMap *asicmap)
{

  //  just copy asicmap table start to pointer
  ftpcAsicMap_st* asicmapTable = (ftpcAsicMap_st*)asicmap->GetTable();
  if(asicmapTable){
     mEastIsInverted = asicmapTable->EastIsInverted;
     mAsic2EastNotInverted = asicmapTable->Asic2EastNotInverted;
     return kStOK;
  } else {
     gMessMgr->Message( " No data in table class St_ftpcAsicMap","E");
     return kStWarn;
  }
}

//----------------------------------------------------------------------------

Int_t StFtpcDbReader::FtpcEField(St_ftpcEField *efield)
{

  //  just copy EField table start to pointer
  ftpcEField_st* efieldTable = (ftpcEField_st*)efield->GetTable();
  if(efieldTable){
     mMagboltzEField = (Float_t *)efieldTable->e;
     return kStOK;
  } else {
     gMessMgr->Message( " No data in table class St_ftpcEField","E");
     return kStWarn;
  }
}

//----------------------------------------------------------------------------
Int_t StFtpcDbReader::FtpcVDrift(St_ftpcVDrift *vdrift)
{

  //  just copy VDrift table start to pointer
  ftpcVDrift_st* vdriftTable = (ftpcVDrift_st*)vdrift->GetTable();
  if(vdriftTable){
     mMagboltzVDrift = (Float_t *)vdriftTable->v;
     return kStOK;
  } else {
     gMessMgr->Message( " No data in table class St_ftpcVDrift","E");
     return kStWarn;
  }
}
//----------------------------------------------------------------------------
Int_t StFtpcDbReader::FtpcDeflection(St_ftpcDeflection *deflection)
{

  //  just copy Deflection table start to pointer
  ftpcDeflection_st* deflectionTable = (ftpcDeflection_st*)deflection->GetTable();
  if(deflectionTable){
     mMagboltzDeflection = (Float_t *)deflectionTable->psi;
     return kStOK; 
  } else {
     gMessMgr->Message( " No data in table class St_ftpcDeflection","E");
     return kStWarn;
  }
}

//----------------------------------------------------------------------------
Int_t StFtpcDbReader::FtpcdVDriftdP(St_ftpcdVDriftdP *dvdriftdp)
{

  //  just copy dVDriftdP table start to pointer
  ftpcdVDriftdP_st* dvriftdpTable = (ftpcdVDriftdP_st*)dvdriftdp->GetTable();
  if(dvriftdpTable){
     mMagboltzdVDriftdP = (Float_t *)dvriftdpTable->dv_dp;
     return kStOK; 
  } else {
     gMessMgr->Message( " No data in table class St_ftpcdVDriftdP","E");
     return kStWarn;
  }
}

//----------------------------------------------------------------------------
Int_t StFtpcDbReader::FtpcdDeflectiondP(St_ftpcdDeflectiondP *ddeflectiondp)
{

  //  just copy dDeflectiondP table start to pointer
  ftpcdDeflectiondP_st* ddeflectiondpTable = (ftpcdDeflectiondP_st*)ddeflectiondp->GetTable();
  if(ddeflectiondpTable){
     mMagboltzdDeflectiondP = (Float_t *)ddeflectiondpTable->dpsi_dp;
     return kStOK; 
  } else {
     gMessMgr->Message( " No data in table class St_ftpcdDeflectiondP","E");
     return kStWarn;
  }
}

//----------------------------------------------------------------------------
Int_t StFtpcDbReader::FtpcAmpSlope(St_ftpcAmpSlope *ampslope)
{

  //  just copy AmpSlope table start to pointer
  ampslopeTable = (ftpcAmpSlope_st*)ampslope->GetTable();
  if(!ampslopeTable){
     gMessMgr->Message( " No data in table class St_ftpcAmpSlope","E");
     return kStWarn;
  }
  return kStOK; 
}

//----------------------------------------------------------------------------
Int_t StFtpcDbReader::FtpcAmpOffset(St_ftpcAmpOffset *ampoffset)
{

  //  just copy AmpOffset table start to pointer
  ampoffsetTable = (ftpcAmpOffset_st*)ampoffset->GetTable();
  if(!ampoffsetTable){
     gMessMgr->Message( " No data in table class St_ftpcAmpOffset","E");
     return kStWarn;
  }
  return kStOK;
}

//----------------------------------------------------------------------------
Int_t StFtpcDbReader::FtpcTimeOffset(St_ftpcTimeOffset *timeoffset)
{

  //  just copy TimeOffset table start to pointer
  timeoffsetTable = (ftpcTimeOffset_st*)timeoffset->GetTable();
  if(!timeoffsetTable){
    gMessMgr->Message( " No data in table class St_ftpcTimeOffset","E");
    return kStWarn;
  }
  return kStOK;
}

//----------------------------------------------------------------------------
Int_t StFtpcDbReader::FtpcDriftField(St_ftpcDriftField *driftfield)
{
  //  just copy driftfield table start to pointer
  ftpcDriftField_st* driftfieldTable = (ftpcDriftField_st*)driftfield->GetTable();
  if(driftfieldTable){
    mNumberOfMagboltzBins = driftfieldTable->numberOfEFieldBinsUsed;
    mMaximumNumberOfMagboltzBins = driftfieldTable->maximumNumberOfEFieldBins;
    mDriftCathodeVoltage = driftfieldTable->driftCathodeVoltage;
    mMinimumDriftField  = driftfieldTable->minimumDriftField;
    mStepSizeDriftField = driftfieldTable->stepSizeDriftField;
    mRadiusTimesField   = driftfieldTable->radiusTimesField;
    return kStOK;
  } else {
    gMessMgr->Message( " No data in table class St_ftpcDriftField","E");
    return kStWarn;
  }
}

//----------------------------------------------------------------------------
Int_t StFtpcDbReader::FtpcGas(St_ftpcGas *gas)
{
  //  just copy gas table start to pointer
  ftpcGas_st* gasTable = (ftpcGas_st*)gas->GetTable();
  if(gasTable){
   mPercentAr              = gasTable->percentAr;
   mPercentCO2             = gasTable->percentCO2;
   mPercentNe              = gasTable->percentNe;
   mPercentHe              = gasTable->percentHe;
   mGasGain                = gasTable->gasGain;
   mGasAttenuation         = gasTable->gasAttenuation;
   mGasIonizationPotential = gasTable->gasIonizationPotential;
   mBaseTemperature        = gasTable->baseTemperature;
   mBasePressure           = gasTable->basePressure;
   mPressureOffset         = gasTable->pressureOffset;
   mTemperatureDifference  = gasTable->temperatureDifference;
   mDefaultTemperatureWest = gasTable->defaultTemperatureWest;
   mDefaultTemperatureEast = gasTable->defaultTemperatureEast;
   mMinPressure            = gasTable->minPressure;
   mMaxPressure            = gasTable->maxPressure; 
   mMinGasTemperature      = gasTable->minGasTemperature;
   mMaxGasTemperature      = gasTable->maxGasTemperature; 
   mAdjustAverageWest      = gasTable->adjustAverageWest;
   mAdjustAverageEast      = gasTable->adjustAverageEast;
   return kStOK;
  } else {
    gMessMgr->Message( " No data in table class St_ftpcGas","E");
    return kStWarn;
  }
}

//----------------------------------------------------------------------------
Int_t StFtpcDbReader::FtpcElectronics(St_ftpcElectronics *electronics)
{
  //  just copy electronics table start to pointer
  ftpcElectronics_st *electronicsTable = (ftpcElectronics_st*)electronics->GetTable();
  if(electronicsTable){
     mTZero = electronicsTable->tZero;
     mMicrosecondsPerTimebin = electronicsTable->uSecondsPerTimebin;
     return kStOK;
  } else {
     gMessMgr->Message( " No data in table class St_ftpcElectronics","E");
     return kStWarn;
  }
}

//----------------------------------------------------------------------------
Int_t StFtpcDbReader::FtpcInnerCathode(St_ftpcInnerCathode *cathode)
{
  //  just copy inner cathode table start to pointer
  ftpcInnerCathode_st* cathodeTable = (ftpcInnerCathode_st*)cathode->GetTable();
  if(cathodeTable){
     mOffsetCathodeWest = cathodeTable->offsetCathodeWest;
     mOffsetCathodeEast = cathodeTable->offsetCathodeEast;
     mAngleOffsetWest   = cathodeTable->angleOffsetWest;
     mAngleOffsetEast   = cathodeTable->angleOffsetEast;
     return kStOK;
  } else {
     gMessMgr->Message( " No data in table class St_ftpcInnerCathode","E");
     return kStWarn;
  }    
}

//----------------------------------------------------------------------------
Int_t StFtpcDbReader::FtpcClusterGeom(St_ftpcClusterGeom *clustergeo)
{

  //  just copy cluster geometry table start to pointer
  ftpcClusterGeom_st* clustergeoTable = (ftpcClusterGeom_st*)clustergeo->GetTable();
  if(clustergeoTable){
    mMinTimeBin = clustergeoTable->minTimebin;
    mMinTimeBinMed = clustergeoTable->minTimebinMed;
    mMinTimeBinOut = clustergeoTable->minTimebinOut;
    mMaxTimeLength = clustergeoTable->maxTimelength;
    mMaxTimeLengthMed = clustergeoTable->maxTimelengthMed;
    mMaxTimeLengthOut =  clustergeoTable->maxTimelengthOut;
    mMaxPadLength  = clustergeoTable->maxPadlength;  
    mMaxPadLengthMed  = clustergeoTable->maxPadlengthMed;
    mMaxPadLengthOut  = clustergeoTable->maxPadlengthOut;
    mDeltaTime = clustergeoTable->deltaTime;
    mDeltaPad  = clustergeoTable->deltaPad;
    mMinChargeWindow = clustergeoTable->minChargeWindow;
    return kStOK;
  } else {
    gMessMgr->Message( " No data in table class St_ftpcClusterGeom","E");
    return kStWarn;
  }
}
//===============================================================

Float_t StFtpcDbReader::padrowZPosition(Int_t i) 
{
  if(i>=0 && i<numberOfPadrows())
    {
      return mPadrowZPosition[i];
    }
  else
    {
      gMessMgr->Message("StFtpcDbReader: padrowZPosition index out of range, using 0", "W", "OS");
      return mPadrowZPosition[0];
    }
}


Float_t StFtpcDbReader::magboltzEField(Int_t i)
{
  if(i>=0 && i<mNumberOfMagboltzBins)
    {
      return mMagboltzEField[i];
    }
  else
    {
      gMessMgr->Message("StFtpcDbReader: magboltzEField index out of range, using 0", "W", "OS");
      return mMagboltzEField[0];
    }
}

Float_t StFtpcDbReader::magboltzVDrift(Int_t i, Int_t padrow)
{
  if(i>=0 && i<mNumberOfMagboltzBins && padrow>=0 && padrow<numberOfPadrowsPerSide())
    {
      return mMagboltzVDrift[padrow+numberOfPadrowsPerSide()*i];
    }
  else
    {
      gMessMgr->Message("StFtpcDbReader: magboltzVDrift index out of range, using 0", "W", "OS");
      return mMagboltzVDrift[0];
    }
}

Float_t StFtpcDbReader::magboltzDeflection(Int_t i, Int_t padrow)
{
  if(i>=0 && i<mNumberOfMagboltzBins && padrow>=0 && padrow<numberOfPadrowsPerSide())
    {
      return mMagboltzDeflection[padrow+numberOfPadrowsPerSide()*i];
    }
  else
    {
      gMessMgr->Message("StFtpcDbReader: magboltzDeflection index out of range, using 0", "W", "OS");
      return mMagboltzDeflection[0];
    }
}

Float_t StFtpcDbReader::magboltzdVDriftdP(Int_t i, Int_t padrow)
{
  if(i>=0 && i<mNumberOfMagboltzBins && padrow>=0 && padrow<numberOfPadrowsPerSide())
    {
      return mMagboltzdVDriftdP[padrow+numberOfPadrowsPerSide()*i];
    }
  else
    {
      gMessMgr->Message("StFtpcDbReader: magboltzdVDriftdP index out of range, using 0", "W", "OS");
      return mMagboltzdVDriftdP[0];
    }
}

Float_t StFtpcDbReader::magboltzdDeflectiondP(Int_t i, Int_t padrow)
{
  if(i>=0 && i<mNumberOfMagboltzBins && padrow>=0 && padrow<numberOfPadrowsPerSide())
    {
      return mMagboltzdDeflectiondP[padrow+numberOfPadrowsPerSide()*i];
    }
  else
    {
      gMessMgr->Message("StFtpcDbReader: magboltzdDeflectiondP index out of range, using 0", "W", "OS");
      return mMagboltzdDeflectiondP[0];
    }
}

Float_t StFtpcDbReader::amplitudeSlope(Int_t i, Int_t padrow)
{
  if(i>0 && i<=(numberOfSectors()*numberOfPads()) && padrow>=0 && padrow<numberOfPadrows())
    {
       // Since the gain tables were produced without inverting the sector
       // numbering in FTPC East, we invert the sector numbering when accessing
       // the gain table
       if (padrow>=10) {
           //cout<<" padrow = "<<padrow<<" i = "<<i;
           Int_t isec = (i-1)/numberOfPads();
           Int_t ipad = i -isec*numberOfPads();
           i = (5-isec)*numberOfPads()+ipad;
           //cout<<" isec = "<<isec<<" ipad = "<<ipad<<" i =  "<<i<<endl;;
       }
       return ampslopeTable[padrow].slope[i-1];
    }
  else
    {
      gMessMgr->Message("StFtpcDbReader: amplitudeSlope index out of range, using 0", "W", "OS");
      return ampslopeTable[0].slope[0];
    }
}

Float_t StFtpcDbReader::amplitudeOffset(Int_t i, Int_t padrow)
{
  if(i>0 && i<=(numberOfSectors()*numberOfPads()) && padrow>=0 && padrow<numberOfPadrows())
    {
       return ampoffsetTable[padrow].offset[i-1];
    }
  else
    {
      gMessMgr->Message("StFtpcDbReader: amplitudeOffset index out of range, using 0", "W", "OS");
      return ampoffsetTable[0].offset[0];
    }
}

Float_t StFtpcDbReader::timeOffset(Int_t i, Int_t padrow)
{
  if(i>0 && i<=(numberOfSectors()*numberOfPads()) && padrow>=0 && padrow<numberOfPadrows())
    {
       return timeoffsetTable[padrow].offset[i-1];
    }
  else
    {
      gMessMgr->Message("StFtpcDbReader: timeOffset index out of range, using 0", "W", "OS");
      return timeoffsetTable[0].offset[0];
    }
}


Int_t StFtpcDbReader::setMagboltzEField(Int_t i, Float_t newvalue)
{
  if(i>=0 && i<mNumberOfMagboltzBins)
    {
      mMagboltzEField[i]=newvalue;
      return 1;
    }
  else
    {
      gMessMgr->Message("StFtpcDbReader: magboltzEField index out of range, not changed", "W", "OS");
      return 0;
    }
}

Int_t StFtpcDbReader::setMagboltzVDrift(Int_t i, Int_t padrow, Float_t newvalue)
{
  if(i>=0 && i<mNumberOfMagboltzBins && padrow>=0 && padrow<numberOfPadrowsPerSide())
    {
      mMagboltzVDrift[padrow+numberOfPadrowsPerSide()*i]=newvalue;
      return 1;
    }
  else
    {
      gMessMgr->Message("StFtpcDbReader: magboltzVDrift index out of range, not changed", "W", "OS");
      return 0;
    }
}

Int_t StFtpcDbReader::setMagboltzDeflection(Int_t i, Int_t padrow, Float_t newvalue)
{
  if(i>=0 && i<mNumberOfMagboltzBins && padrow>=0 && padrow<numberOfPadrowsPerSide())
    {
      mMagboltzDeflection[padrow+numberOfPadrowsPerSide()*i]=newvalue;
      return 1;
    }
  else
    {
      gMessMgr->Message("StFtpcDbReader: magboltzDeflection index out of range, not changed", "W", "OS");
      return 0;
    }
}

Int_t StFtpcDbReader::setMagboltzdVDriftdP(Int_t i, Int_t padrow, Float_t newvalue)
{
  if(i>=0 && i<mNumberOfMagboltzBins && padrow>=0 && padrow<numberOfPadrowsPerSide())
    {
      mMagboltzdVDriftdP[padrow+numberOfPadrowsPerSide()*i]=newvalue;
      return 1;
    }
  else
    {
      gMessMgr->Message("StFtpcDbReader: magboltzdVDriftdP index out of range, not changed", "W", "OS");
      return 0;
    }
}

Int_t StFtpcDbReader::setMagboltzdDeflectiondP(Int_t i, Int_t padrow, Float_t newvalue)
{
  if(i>=0 && i<mNumberOfMagboltzBins && padrow>=0 && padrow<numberOfPadrowsPerSide())
    {
      mMagboltzdDeflectiondP[padrow+numberOfPadrowsPerSide()*i]=newvalue;
      return 1;
    }
  else
    {
      gMessMgr->Message("StFtpcDbReader: magboltzdDeflectiondP index out of range, not changed", "W", "OS");
      return 0;
    }
}

Int_t StFtpcDbReader::setMicrosecondsPerTimebin(Float_t newvalue)
{
   if ( newvalue > 0.0 ) {
     mMicrosecondsPerTimebin = newvalue;
     gMessMgr->Info()<<"mMicrosecondsPerTimebin = "<<mMicrosecondsPerTimebin<<" calculated from RHIC Clock Frequency"<<endm;
   }
   else  {
     gMessMgr->Info()<<"mMicrosecondsPerTimebin = "<<mMicrosecondsPerTimebin<<" default value from database"<<endm;
   }
   return 0;
}
