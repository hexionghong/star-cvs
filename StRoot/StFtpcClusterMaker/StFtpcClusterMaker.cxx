// $Log: StFtpcClusterMaker.cxx,v $
// Revision 1.66  2004/05/19 17:44:46  oldi
// For simulated data the ftpcHitCollection inside StEvent doesn't exist yet.
// We have to create it on our own. Additional check for ftpcHitCollection
// introduced in StFtpcTrackMaker.
//
// Revision 1.65  2004/04/19 22:00:46  oldi
// Minor changes.
//
// Revision 1.64  2004/04/06 18:36:51  oldi
// New data mebers for pad and time position and pad and time sigma filled.
//
// Revision 1.63  2004/02/12 19:38:46  oldi
// Removal of intermediate tables.
//
// Revision 1.62  2004/01/28 02:04:43  jcs
// replace all instances of StFtpcReducedPoint and StFtpcPoint with StFtpcConfMapPoint
//
// Revision 1.61  2004/01/28 01:41:15  jeromel
// Change OST to OS everywhere since defaultoption is now not to print
// the date.
//
// Revision 1.60  2003/11/13 14:12:17  jcs
// move pressure and gas corrections from StFtpcClusterMaker.cxx to StFtpcGasUtilities
//
// Revision 1.59  2003/10/11 08:52:46  jcs
// protect against divide by zero while calculating average body temperatures
//
// Revision 1.58  2003/10/11 04:07:48  perev
// assert for number of east temperatures added
//
// Revision 1.57  2003/09/02 17:58:14  perev
// gcc 3.2 updates + WarnOff
//
// Revision 1.56  2003/08/21 14:27:24  jcs
// remove temporary fix to prevent segmentation violation which occurred when  more than one run per job
//
// Revision 1.55  2003/07/18 18:31:47  perev
// test for nonexistance of XXXReader added
//
// Revision 1.54  2003/07/15 09:35:41  jcs
// do not re-flavor FTPC drift maps if already flavored to avoid creating
// memory leak.
// meomory leak will occur if flavor (i.e. magnetic field) changes within one
// *.fz file (only possible for MC data)
//
// Revision 1.53  2003/07/04 00:35:53  perev
// Defence against luch of DB info added
//
// Revision 1.52  2003/06/13 10:57:28  jcs
// remove debug statement
//
// Revision 1.51  2003/06/12 10:01:25  jcs
// renamed ftpcClusterGeometry database table to ftpcClusterGeom
// (name was too long)
//
// Revision 1.50  2003/06/11 12:06:03  jcs
// get inner cathode and cluster geometry parameters from database
//
// Revision 1.49  2003/06/10 13:10:05  jcs
// get min,max gas temperature and pressure limits from database instead of from
// parameters
//
// Revision 1.48  2003/05/07 15:08:56  putschke
// improvements for cathode offset corretions
//
// Revision 1.47  2003/04/15 11:34:41  putschke
// Include corrections for inner cathode offset and move some parameter to database
//
// Revision 1.46  2003/02/27 22:56:58  jcs
// use the ftpc body temperature readings to make temperature/pressure corrections
//
// Revision 1.44  2003/02/25 04:45:22  jcs
// comment out body4East temperature for AuAu central production
//
// Revision 1.43  2003/02/19 14:52:44  jcs
// calculate ftpc temperature from body temperatures
//
// Revision 1.42  2003/02/08 03:45:22  jcs
// for dAu production ONLY: hardcode gas temperatures
//
// Revision 1.41  2003/01/14 12:58:01  jcs
// use Geometry_ftpc/ftpcAsicMap to control corrections for error in Y2001-2002
// FTPC asic mapping
//
// Revision 1.40  2002/08/02 11:24:29  oldi
// Used database values are printed to screen, now.
//
// Revision 1.39  2002/07/15 13:31:09  jcs
// incorporate charge step histos into cluster finder and remove StFtpcChargeStep
//
// Revision 1.38  2002/03/22 08:52:52  jcs
// correct memory leaks found by Insure
//
// Revision 1.37  2002/03/20 11:44:35  jcs
// forgot to uncomment delete step when reactivating StFtpcChargeStep - caused
// memory leak
//
// Revision 1.36  2002/03/14 10:53:42  jcs
// turn ftpc timebin charge step histograms back on
//
// Revision 1.35  2002/03/01 14:22:20  jcs
// add additional histograms to monitor cluster finding
//
// Revision 1.34  2002/02/10 21:15:38  jcs
// create separate radial chargestep histograms for Ftpc west and east
//
// Revision 1.33  2002/01/22 02:53:09  jcs
// remove extra test from if statement
//
// Revision 1.32  2002/01/21 22:10:38  jcs
// calculate FTPC gas temperature adjusted air pressure using online db
//
// Revision 1.31  2001/12/12 16:05:28  jcs
// Move GetDataBase.. statements from Init to Make to ensure selection of
// correct flavor (Thank you Jeff)
//
// Revision 1.30  2001/11/21 12:44:44  jcs
// make ftpcGas database table available to FTPC cluster maker
//
// Revision 1.29  2001/10/29 12:53:23  jcs
// select FTPC drift maps according to flavor of magnetic field
//
// Revision 1.28  2001/10/19 09:41:22  jcs
// tZero now in data base in ftpcElectronics
//
// Revision 1.27  2001/10/12 14:33:08  jcs
// create and fill charge step histograms for FTPC East and West
//
// Revision 1.26  2001/07/26 13:53:19  oldi
// Check if FTPC data is available.
//
// Revision 1.25  2001/07/12 10:35:14  jcs
// create and fill FTPC cluster radial position histogram
//
// Revision 1.24  2001/06/16 12:59:47  jcs
// delete ftpcReader only if it is the FTPC slow simulator reader
//
// Revision 1.23  2001/06/13 14:37:54  jcs
// change StDaqReader to StDAQReader
//
// Revision 1.22  2001/04/23 19:57:51  oldi
// The chargestep histogram is only filled but the evaluated value of the
// normalized pressure is not used for a correction. This was done due to the
// fact that StFtpcChargeStep is not stable enough to determine the actual
// charge step.
// Output is sent to StMessMgr now.
//
// Revision 1.21  2001/04/04 17:08:42  jcs
// remove references to StFtpcParamReader from StFtpcDbReader
//
// Revision 1.20  2001/04/02 12:10:18  jcs
// get FTPC calibrations,geometry from MySQL database and code parameters
// from StarDb/ftpc
//
// Revision 1.19  2001/03/19 15:52:47  jcs
// use ftpcDimensions from database
//
// Revision 1.18  2001/03/06 23:33:51  jcs
// use database instead of params
//
// Revision 1.17  2001/01/25 15:25:35  oldi
// Fix of several bugs which caused memory leaks:
//  - Some arrays were not allocated and/or deleted properly.
//  - TClonesArray seems to have a problem (it could be that I used it in a
//    wrong way in StFtpcTrackMaker form where Holm cut and pasted it).
//    I changed all occurences to TObjArray which makes the program slightly
//    slower but much more save (in terms of memory usage).
//
// Revision 1.16  2000/11/24 15:02:33  hummler
// commit changes omitted in last commit
//
// Revision 1.14  2000/11/20 11:39:12  jcs
// remove remaining traces of fspar table
//
// Revision 1.13  2000/11/14 13:08:16  hummler
// add charge step calculation, minor cleanup
//
// Revision 1.12  2000/09/18 14:26:46  hummler
// expand StFtpcParamReader to supply data for slow simulator as well
// introduce StFtpcGeantReader to separate g2t tables from simulator code
// implement StFtpcGeantReader in StFtpcFastSimu
//
// Revision 1.11  2000/08/03 14:39:00  hummler
// Create param reader to keep parameter tables away from cluster finder and
// fast simulator. StFtpcClusterFinder now knows nothing about tables anymore!
//
// Revision 1.8  2000/04/13 18:08:21  fine
// Adjusted for ROOT 2.24
//
// Revision 1.7  2000/02/24 15:18:42  jcs
// inactivate histograms for MDC3
//
// Revision 1.6  2000/02/04 13:49:36  hummler
// upgrade ffs:
// -remove unused fspar table
// -make hit smearing gaussian with decent parameters and static rand engine
// -separate hit smearing from cluster width calculation
//
// Revision 1.5  2000/02/02 15:20:33  hummler
// correct acceptance at sector boundaries,
// take values from fcl_det
//
// Revision 1.4  2000/01/27 09:47:18  hummler
// implement raw data reader, remove type ambiguities that bothered kcc
//
//
//
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// StFtpcClusterMaker class for Makers                                  //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include <Stiostream.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
//VP#include "StDaqLib/GENERIC/EventReader.hh"
#include "StDAQMaker/StFTPCReader.h"

#include "StMessMgr.h"
#include "StFtpcClusterMaker.h"
#include "StFtpcParamReader.hh"
#include "StFtpcDbReader.hh"
#include "StFtpcGeantReader.hh"
#include "StFtpcClusterFinder.hh"
#include "StFtpcGasUtilities.hh"
#include "StFtpcTrackMaker/StFtpcConfMapPoint.hh"
#include "StFtpcGeantPoint.hh"
#include "StFtpcFastSimu.hh"
#include "StChain.h"
#include "St_DataSetIter.h"
#include "TH1.h"
#include "TH2.h"
#include "TObjArray.h"
#include "TObjectSet.h"
#include "PhysicalConstants.h"

#ifndef gufld
#define gufld gufld_
extern "C" void gufld(float *, float *);
#endif

#include "tables/St_fcl_ftpcsqndx_Table.h"
#include "tables/St_fcl_ftpcadc_Table.h"

#include "tables/St_g2t_vertex_Table.h"
#include "tables/St_g2t_track_Table.h"
#include "tables/St_g2t_ftp_hit_Table.h"
#include "tables/St_ffs_gepoint_Table.h"

#include "StDetectorDbMaker/StDetectorDbFTPCGas.h"
#include "St_db_Maker/St_db_Maker.h"

#include "StEvent.h"
#include "StFtpcHitCollection.h"

ClassImp(StFtpcClusterMaker)

  //_____________________________________________________________________________
StFtpcClusterMaker::StFtpcClusterMaker(const char *name):
StMaker(name),
    m_clusterpars(0),
    m_fastsimgas(0),
    m_fastsimpars(0),
    m_dimensions(0),
    m_padrow_z(0),
    m_asicmap(0),
    m_efield(0),
    m_vdrift(0),
    m_deflection(0),
    m_dvdriftdp(0),
    m_ddeflectiondp(0),
    m_ampslope(0),
    m_ampoffset(0),
    m_timeoffset(0),
    m_driftfield(0),
    m_gas(0),
    m_electronics(0),
    m_cathode(0),
    m_clustergeo(0)
{
  drawinit=kFALSE;
}
//_____________________________________________________________________________
StFtpcClusterMaker::~StFtpcClusterMaker(){
}
//_____________________________________________________________________________
Int_t StFtpcClusterMaker::InitRun(int runnumber){
  Float_t x[3] = {0,0,0};
  Float_t b[3];
  gufld(x,b);
  Double_t gFactor = b[2]/4.980;

  mDbMaker     = (St_db_Maker*)GetMaker("db");
  Int_t dbDate = mDbMaker->GetDateTime().GetDate();
  cout<<"StFtpcClusterMaker: dbDate = "<<dbDate<<endl;
  
  gMessMgr->Info() << "StFtpcClusterMaker::InitRun("<<runnumber<<") - 'flavor' FTPC drift maps for gFactor = "<<gFactor<<endm;
  
  // Load the correct FTPC drift maps depending on magnetic field

  // Full Field Positive ?
  if ( gFactor > 0.8 ) {
     SetFlavor("ffp10kv","ftpcVDrift");
     SetFlavor("ffp10kv","ftpcdVDriftdP");
     SetFlavor("ffp10kv","ftpcDeflection");
     SetFlavor("ffp10kv","ftpcdDeflectiondP");
     gMessMgr->Info() << "StFtpcClusterMaker::InitRun: flavor set to ffp10kv"<<endm;
  }
  else if ( gFactor > 0.2 ) {
     SetFlavor("hfp10kv","ftpcVDrift");
     SetFlavor("hfp10kv","ftpcdVDriftdP");
     SetFlavor("hfp10kv","ftpcDeflection");
     SetFlavor("hfp10kv","ftpcdDeflectiondP");
     gMessMgr->Info() << "StFtpcClusterMaker::InitRun: flavor set to hfp10kv"<<endm;
  }
  else if ( gFactor > -0.2 ) {
     SetFlavor("zf10kv","ftpcVDrift");
     SetFlavor("zf10kv","ftpcdVDriftdP");
     SetFlavor("zf10kv","ftpcDeflection");
     SetFlavor("zf10kv","ftpcdDeflectiondP");
     gMessMgr->Info() << "StFtpcClusterMaker::InitRun: flavor set to zf10kv"<<endm;
  }
  else if ( gFactor > -0.8 ) {
     SetFlavor("hfn10kv","ftpcVDrift");
     SetFlavor("hfn10kv","ftpcdVDriftdP");
     SetFlavor("hfn10kv","ftpcDeflection");
     SetFlavor("hfn10kv","ftpcdDeflectiondP");
     gMessMgr->Info() << "StFtpcClusterMaker::InitRun: flavor set to hfn10kv"<<endm;
  }
  else {
     SetFlavor("ffn10kv","ftpcVDrift");
     SetFlavor("ffn10kv","ftpcdVDriftdP");
     SetFlavor("ffn10kv","ftpcDeflection");
     SetFlavor("ffn10kv","ftpcdDeflectiondP");
     gMessMgr->Info() << "StFtpcClusterMaker::InitRun: flavor set to ffn10kv"<<endm;
  }     
  return 0;
}
//_____________________________________________________________________________
Int_t StFtpcClusterMaker::Init(){


  St_DataSet *ftpc = GetDataBase("ftpc");
  assert(ftpc);
  St_DataSetIter       local(ftpc);

  m_clusterpars  = (St_ftpcClusterPars *)local("ftpcClusterPars");
  m_fastsimgas   = (St_ftpcFastSimGas  *)local("ftpcFastSimGas");
  m_fastsimpars  = (St_ftpcFastSimPars *)local("ftpcFastSimPars");

  // 		Create Histograms
  m_csteps      = new TH2F("fcl_csteps"	,"FTPC charge steps by sector"	,60,-0.5,59.5, 260, -0.5, 259.5);
  m_chargestep_West = new TH1F("fcl_chargestepW","FTPC West chargestep",260, -0.5, 259.5);
  m_chargestep_East = new TH1F("fcl_chargestepE","FTPC East chargestep",260, -0.5, 259.5);
  m_flags      = new TH1F("fcl_flags"	,"FTPC cluster finder flags"	,7,0.,8.);
  m_row        = new TH1F("fcl_row"	,"FTPC rows"			,20,1.,21.);
  m_sector     = new TH1F("fcl_sector"	,"FTPC sectors"			,6,1.,7.);
  //m_pads       = new TH1F("fcl_pads"	,"FTPC pads"			,80,1.,161.);
  //m_timebins   = new TH1F("fcl_timebins","FTPC timebins"		,100,1.,257.);
  m_row_sector = new TH2F("fcl_row_sector","FTPC(fcl) row vs. sector"	,20,1.,21.,6,1.,7.);
  //m_npad_nbin  = new TH2F("fcl_pad_bin"	,"FTPC(fcl) pad vs. timebin"	,80,1.,161.,100,1.,257.);
  m_cluster_radial_West = new TH1F("fcl_radialW","FTPCW cluster radial position",700,0.,35.);
  m_cluster_radial_East = new TH1F("fcl_radialE","FTPCE cluster radial position",700,0.,35.);

  m_hitsvspad = new TH2F("fcl_hitsvspad","#hits vs. padlength",10,0.5,10.5,11,0.5,11.5);
  m_hitsvstime = new TH2F("fcl_hitsvstime","#hits vs. timelength",12,0.5,12.5,11,0.5,11.5);

  m_padvstime_West = new TH2F("fcl_padvstimeW","FTPCW padlength vs. timelength",12,0.5,12.5,10,0.5,10.5);
  m_padvstime_East = new TH2F("fcl_padvstimeE","FTPCE padlength vs. timelength",12,0.5,12.5,10,0.5,10.5);
  m_maxadc_West = new TH1F("fcl_maxadcW","FTPCW MaxAdc",50,0.5,50.5);
  m_maxadc_East = new TH1F("fcl_maxadcE","FTPCE MaxAdc",50,0.5,50.5);
  m_charge_West = new TH1F("fcl_chargeW","FTPCW charge",50,0.5,500.5);
  m_charge_East = new TH1F("fcl_chargeE","FTPCE charge",50,0.5,500.5);

  return StMaker::Init();
}
//_____________________________________________________________________________
Int_t StFtpcClusterMaker::Make()
{
  int iMake=kStOK;

  int using_FTPC_slow_simulator = 0;

  mCurrentEvent = (StEvent*) GetInputDS("StEvent");
  if (mCurrentEvent) {
    if (!(mFtpcHitColl = mCurrentEvent->ftpcHitCollection())) {
      mFtpcHitColl = new StFtpcHitCollection;
      mCurrentEvent->setFtpcHitCollection(mFtpcHitColl);
    }
  } else mFtpcHitColl = 0;

  St_DataSet *ftpc_geometry_db = GetDataBase("Geometry/ftpc");
  if ( !ftpc_geometry_db ){
     gMessMgr->Warning() << "StFtpcClusterMaker::Error Getting FTPC database: Geometry"<<endm;
     return kStWarn;
  }
  St_DataSetIter       dblocal_geometry(ftpc_geometry_db);
 
  m_dimensions = (St_ftpcDimensions *)dblocal_geometry("ftpcDimensions");
  m_padrow_z   = (St_ftpcPadrowZ *)dblocal_geometry("ftpcPadrowZ");
  m_asicmap    = (St_ftpcAsicMap *)dblocal_geometry("ftpcAsicMap");
  m_clustergeo = (St_ftpcClusterGeom *)dblocal_geometry("ftpcClusterGeom");
  m_cathode      = (St_ftpcInnerCathode *)dblocal_geometry("ftpcInnerCathode");

  if (!(m_dimensions && m_padrow_z && m_asicmap && m_clustergeo && m_cathode)) {
     gMessMgr->Warning() << "StFtpcClusterMaker::Error Getting content of FTPC database: Geometry"<<endm;
     return kStWarn;
  }
  
  St_DataSet *ftpc_calibrations_db = GetDataBase("Calibrations/ftpc");
  if ( !ftpc_calibrations_db ){
     gMessMgr->Warning() << "StFtpcClusterMaker::Error Getting FTPC database: Calibrations"<<endm;
     return kStWarn;
  }
  St_DataSetIter       dblocal_calibrations(ftpc_calibrations_db);

  m_efield     = (St_ftpcEField *)dblocal_calibrations("ftpcEField" );
  m_vdrift     = (St_ftpcVDrift *)dblocal_calibrations("ftpcVDrift" );
  m_deflection = (St_ftpcDeflection *)dblocal_calibrations("ftpcDeflection" );
  m_dvdriftdp     = (St_ftpcdVDriftdP *)dblocal_calibrations("ftpcdVDriftdP" );
  m_ddeflectiondp = (St_ftpcdDeflectiondP *)dblocal_calibrations("ftpcdDeflectiondP" );
  m_ampslope = (St_ftpcAmpSlope *)dblocal_calibrations("ftpcAmpSlope" );
  m_ampoffset = (St_ftpcAmpOffset *)dblocal_calibrations("ftpcAmpOffset");
  m_timeoffset = (St_ftpcTimeOffset *)dblocal_calibrations("ftpcTimeOffset");
  m_driftfield = (St_ftpcDriftField *)dblocal_calibrations("ftpcDriftField");
  m_gas        = (St_ftpcGas *)dblocal_calibrations("ftpcGas");
  m_electronics = (St_ftpcElectronics *)dblocal_calibrations("ftpcElectronics");

  // create parameter reader
  StFtpcParamReader *paramReader = new StFtpcParamReader(m_clusterpars,
							 m_fastsimgas,
                                                         m_fastsimpars);
  
  cout<<"paramReader->gasTemperatureWest() = "<<paramReader->gasTemperatureWest()<<endl;
  cout<<"paramReader->gasTemperatureEast() = "<<paramReader->gasTemperatureEast()<<endl;
 
  // create FTPC data base reader
  StFtpcDbReader *dbReader = new StFtpcDbReader(m_dimensions,
                                                m_padrow_z,
						m_asicmap,
                                                m_efield,
                                                m_vdrift,
                                                m_deflection,
                                                m_dvdriftdp,
                                                m_ddeflectiondp,
                                                m_ampslope,
                                                m_ampoffset,
                                                m_timeoffset,
                                                m_driftfield,
                                                m_gas,
                                                m_electronics,
						m_cathode,
					        m_clustergeo);	

  if ( paramReader->gasTemperatureWest() == 0 && paramReader->gasTemperatureEast() == 0) {
     cout<<"Using the following values from database:"<<endl;
     cout<<"          EastIsInverted            = "<<dbReader->EastIsInverted()<<endl;
     cout<<"          Asic2EastNotInverted      = "<<dbReader->Asic2EastNotInverted()<<endl;
     cout<<"          tzero                     = "<<dbReader->tZero()<<endl;
     cout<<"          temperatureDifference     = "<<dbReader->temperatureDifference()<<endl;
     cout<<"          defaultTemperatureWest    = "<<dbReader->defaultTemperatureWest()<<endl;
     cout<<"          defaultTemperatureEast    = "<<dbReader->defaultTemperatureEast()<<endl;
     cout<<"          magboltzVDrift(0,0)       = "<<dbReader->magboltzVDrift(0,0)<<endl;
     cout<<"          magboltzDeflection(0,0)   = "<<dbReader->magboltzDeflection(0,0)<<endl;
     cout<<"          offsetCathodeWest         = "<<dbReader->offsetCathodeWest()<<endl;
     cout<<"          offsetCathodeEast         = "<<dbReader->offsetCathodeEast()<<endl;
     cout<<"          angleOffsetWest           = "<<dbReader->angleOffsetWest()<<endl;
     cout<<"          angleOffsetEast           = "<<dbReader->angleOffsetEast()<<endl;
     cout<<"          minChargeWindow           = "<<dbReader->minChargeWindow()<<endl;
  }

  St_DataSet *daqDataset;
  StDAQReader *daqReader;
  StFTPCReader *ftpcReader=NULL;
  daqDataset=GetDataSet("StDAQReader");
  if(daqDataset)
    {
      gMessMgr->Message("", "I", "OS") << "Using StDAQReader to get StFTPCReader" << endm;
      assert(daqDataset);
      daqReader=(StDAQReader *)(daqDataset->GetObject());
      assert(daqReader);
      ftpcReader=daqReader->getFTPCReader();

      if (!ftpcReader || !ftpcReader->checkForData()) {
	gMessMgr->Message("", "W", "OS") << "No FTPC data available!" << endm;
        delete paramReader;
        delete dbReader;
	return kStWarn;
      }

      // test if pressure and gas temperature available from offline DB
       
      StDetectorDbFTPCGas *gas = StDetectorDbFTPCGas::instance();
      if ( !gas ){
          gMessMgr->Warning() << "StFtpcClusterMaker::Error Getting FTPC Offline database: Calibrations_ftpc/ftpcGasOut"<<endm;
          delete paramReader;
          delete dbReader;
          return kStWarn;
      }	      
        
      Int_t  returnCode;

      // use available pressure and gas temperature from offline DB to adjust 
      // the barometric pressure depending on the FTPC gas temperature
         
      StFtpcGasUtilities *gasUtils = new StFtpcGasUtilities(paramReader,
		                                   dbReader,
						   gas);

      returnCode = gasUtils->barometricPressure();

      // Calculate FTPC gas temperature from body temperatures

      //mDbMaker     = (St_db_Maker*)GetMaker("db");
      Int_t dbDate = mDbMaker->GetDateTime().GetDate();

      // For FTPC West
      
      returnCode = gasUtils->averageTemperatureWest(dbDate);
      
      // test if averageBodyTemperature for FTPC West found for first event
      if (paramReader->gasTemperatureWest() == 0) {
	 // no value found in Calibrations_ftpc/ftpcGasOut for first event
         // initialize FTPC gas temperatures to default values 
            // default values change depending on SVT high voltage on/off
            // currently using daqReader->SVTPresent() to test but may need
            // access to Conditions_svt/svtInterLocks
         cout<<"daqReader->SVTPresent() = "<<daqReader->SVTPresent()<<endl;
	 returnCode = gasUtils->defaultTemperatureWest(dbDate,daqReader->SVTPresent());
      }	 

      // For FTPC East
      
      returnCode = gasUtils->averageTemperatureEast(dbDate);
      
      // test if averageBodyTemperature for FTPC East found for first event
      if (paramReader->gasTemperatureEast() == 0 ) {
	 // no value found in Calibrations_ftpc/ftpcGasOut for first event
         // initialize FTPC gas temperatures to default values 
            // default values change depending on SVT high voltage on/off
            // currently using daqReader->SVTPresent() to test but may need
            // access to Conditions_svt/svtInterLocks
         cout<<"daqReader->SVTPresent() = "<<daqReader->SVTPresent()<<endl;
	 returnCode = gasUtils->defaultTemperatureEast(dbDate,daqReader->SVTPresent());
      }	 


      gMessMgr->Message("", "I", "OS") << " Using normalizedNowPressure = "<<paramReader->normalizedNowPressure()<<" gasTemperatureWest = "<<paramReader->gasTemperatureWest()<<" gasTemperatureEast = "<<paramReader->gasTemperatureEast()<<endm; 

       paramReader->setAdjustedAirPressureWest(paramReader->normalizedNowPressure()*((dbReader->baseTemperature()+STP_Temperature)/(paramReader->gasTemperatureWest()+STP_Temperature)));
      gMessMgr->Info() <<" paramReader->setAdjustedAirPressureWest = "<<paramReader->adjustedAirPressureWest()<<endm;
      paramReader->setAdjustedAirPressureEast(paramReader->normalizedNowPressure()*((dbReader->baseTemperature()+STP_Temperature)/(paramReader->gasTemperatureEast()+STP_Temperature)));
     gMessMgr->Info() <<" paramReader->setAdjustedAirPressureEast = "<<paramReader->adjustedAirPressureEast()<<endm;

     delete gasUtils; 
    }

  mHitArray = new TObjArray(10000);
  mHitArray->SetOwner(kTRUE); // make sure that delete calls ->Delete() as well
  AddData(new TObjectSet("ftpcClusters", mHitArray));

  // ghitarray will only be used if fast simulator is active
  TObjArray *ghitarray = new TObjArray(10000);  

  St_DataSet *raw = GetDataSet("ftpc_raw");
  if (raw) {
    //			FCL
    St_DataSetIter get(raw);
    
    St_fcl_ftpcsqndx *fcl_ftpcsqndx = (St_fcl_ftpcsqndx*)get("fcl_ftpcsqndx");
    St_fcl_ftpcadc   *fcl_ftpcadc   = (St_fcl_ftpcadc*  )get("fcl_ftpcadc");

    if (fcl_ftpcsqndx&&fcl_ftpcadc) { 

      ftpcReader=new StFTPCReader((short unsigned int *) fcl_ftpcsqndx->GetTable(),
				  fcl_ftpcsqndx->GetNRows(),
				  (char *) fcl_ftpcadc->GetTable(),
				  fcl_ftpcadc->GetNRows());

      gMessMgr->Message("", "I", "OS") << "created StFTPCReader from tables" << endm;
      using_FTPC_slow_simulator = 1;
    }
    else {
      
      gMessMgr->Message("", "I", "OS") <<"StFtpcClusterMaker: Tables are not found:" 
					<< " fcl_ftpcsqndx = " << fcl_ftpcsqndx 
					<< " fcl_ftpcadc   = " << fcl_ftpcadc << endm;
    }
  }

  if(ftpcReader) {


    if(Debug()) gMessMgr->Message("", "I", "OS") << "start running StFtpcClusterFinder" << endm;
    
    StFtpcClusterFinder *fcl = new StFtpcClusterFinder(ftpcReader, 
						       paramReader, 
                                                       dbReader,
						       mHitArray,
						       m_hitsvspad,
						       m_hitsvstime,
                                                       m_csteps,
                                                       m_chargestep_West,
                                                       m_chargestep_East);
    
    int searchresult=fcl->search();
    
    if (searchresult == 0)
      {
	iMake=kStWarn;
      }
	
    delete fcl;
    if (using_FTPC_slow_simulator) delete ftpcReader;
  }
  else {     
    //                      FFS
    St_DataSet *gea = GetDataSet("geant");
    St_DataSetIter geant(gea);
    St_g2t_vertex  *g2t_vertex  = (St_g2t_vertex *) geant("g2t_vertex");
    St_g2t_track   *g2t_track   = (St_g2t_track *)   geant("g2t_track");
    St_g2t_ftp_hit *g2t_ftp_hit = (St_g2t_ftp_hit *) geant("g2t_ftp_hit");
    if (g2t_vertex && g2t_track && g2t_ftp_hit){
      StFtpcGeantReader *geantReader = new StFtpcGeantReader(g2t_vertex,
							     g2t_track,
							     g2t_ftp_hit);

      if(Debug()) gMessMgr->Message("", "I", "OS") << "NO RAW DATA AVAILABLE - start running StFtpcFastSimu" << endm;
      
      StFtpcFastSimu *ffs = new StFtpcFastSimu(geantReader,
					       paramReader,
                                               dbReader,
					       mHitArray,
					       ghitarray);
      if(Debug()) gMessMgr->Message("", "I" "OS") << "finished running StFtpcFastSimu" << endm;
      delete ffs;
      delete geantReader;
    }
  }

  Int_t num_points = mHitArray->GetEntriesFast();
  if(num_points>0 && mFtpcHitColl) { // points found and hitCollection exists
    // Write hits to StEvent (not tested yet).
    StFtpcPoint *point;
    
    for (Int_t i=0; i<num_points; i++) {
      point = (StFtpcPoint *)mHitArray->At(i);
      point->ToStEvent(mFtpcHitColl); 
    }	
  }


  Int_t num_gpoints = ghitarray->GetEntriesFast();
  if(num_gpoints>0)
    {
      St_ffs_gepoint *ffs_gepoint = new St_ffs_gepoint("ffs_fgepoint",num_gpoints);
      m_DataSet->Add(ffs_gepoint);
      
      ffs_gepoint_st *gpointTable= ffs_gepoint->GetTable();
      
      StFtpcGeantPoint *gpoint;
      
      for (Int_t i=0; i<num_gpoints; i++) 
	{
	  gpoint = (StFtpcGeantPoint *)ghitarray->At(i);
	  gpoint->ToTable(&(gpointTable[i]));    
	}
      
      ffs_gepoint->SetNRows(num_gpoints);
    }
  
  ghitarray->Delete();
  delete ghitarray;
  // mHitArray and its contents will be deleted by StMaker::Clear() since it is sitting in a TDataSet
  delete paramReader;
  delete dbReader;
// Deactivate histograms for MDC3
  MakeHistograms(); // FTPC cluster finder histograms
  return iMake;
}


//_____________________________________________________________________________
void StFtpcClusterMaker::MakeHistograms() 
{

  //cout<<"*** NOW MAKING HISTOGRAMS FOR fcl ***"<<endl;


  if (!mHitArray) return;
  for (Int_t i=0; i<mHitArray->GetEntriesFast();i++) {
    StFtpcPoint *hit = (StFtpcPoint*)mHitArray->At(i);
    Int_t flag = hit->GetFlags();
    if (flag > 0) {
      Int_t bin = 6;
      for (Int_t twofac=32; twofac>0; twofac=twofac/2,bin--) {
	Int_t nbit = flag/twofac;
        if (nbit != 1) 	continue;
        m_flags->Fill((float)bin);
	flag = flag - nbit*twofac;        
      }//end loop twofac
    }//endif flag

   Float_t nrow = hit->GetPadRow();
   m_row->Fill(nrow);
   Float_t nsec = hit->GetSector();
   m_sector->Fill(nsec);
   m_row_sector->Fill(nrow,nsec);

//  Float_t npad = r->n_pads;
//  m_pads->Fill(npad);
//  Float_t nbin = r->n_bins;
//  m_timebins->Fill(nbin);
//  m_npad_nbin->Fill(npad,nbin);
  
   // Fill cluster radius histograms
   Float_t rpos = ::sqrt(hit->GetX()*hit->GetX() + hit->GetY()*hit->GetY());
   if (hit->GetPadRow() <=10 ) 
     {
       m_cluster_radial_West->Fill(rpos);
       m_maxadc_West->Fill(hit->GetMaxADC());
       m_charge_West->Fill(hit->GetCharge());	 
       m_padvstime_West->Fill(hit->GetNumberBins(),hit->GetNumberPads());
     }
   else if (hit->GetPadRow() >=11 ) 
     {
       m_cluster_radial_East->Fill(rpos);
       m_maxadc_East->Fill(hit->GetMaxADC());
       m_charge_East->Fill(hit->GetCharge());
       m_padvstime_East->Fill(hit->GetNumberBins(),hit->GetNumberPads());
     }
   
  }//end rows loop 
}
