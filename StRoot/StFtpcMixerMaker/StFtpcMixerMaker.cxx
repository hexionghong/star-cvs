//==================================================================================================
// StFtpcMixerMaker: Merges simulated and real data (embedding), needs two input chains
//
// Author: Frank Simon (fsimon@bnl.gov)
//==================================================================================================

//! FTPC Mixer Maker, main part of FTPC embedding Framework

/*! \class StFtpcMixerMaker
    \author  Frank Simon (fsimon@bnl.gov)

    The StFtpcMixerMaker is the main part of the FTPC embedding framework, analoguous to the
    StMixerMaker for the TPC. This maker is used to merge two seperate input datasets 
    (daq & simu, daq & daq, simu & simu) into one simulation dataset. This is done on a ADC
    timebin basis, the sequences are created again from scratch by calling StFtpcSequencer.

    StFtpcMixerMaker is run from a modified bfcMixer.C macro (including the FTPC embedding stuff)
        
*/

#include <stdio.h>      
#include <iostream.h>
#include <fstream.h>
#include <ctime>      // time functions

#include "StFtpcMixerMaker.h"

// Message System
#include "StMessMgr.h"

// SCL
#include "StChain.h"
#include "St_DataSetIter.h"
#include "St_ObjectSet.h"

// DataBase 
#include "StFtpcClusterMaker/StFtpcDbReader.hh"

// outPut Data--decoder
#include "StDaqLib/GENERIC/EventReader.hh"
#include "StSequence.hh"

#include "StDAQMaker/StDAQReader.h"

// Raw Output stuff
#include "StFtpcSequencer.hh"


// FTPC tables for SlowSimulator

#include "tables/St_fcl_fppoint_Table.h"
#include "tables/St_fcl_ftpcsqndx_Table.h"
#include "tables/St_fcl_ftpcadc_Table.h"


ClassImp(StFtpcMixerMaker)


//! Constructor
StFtpcMixerMaker::StFtpcMixerMaker(const char *name, const char *kind1, const char *kind2):
StMaker(name), 
 mConfig1(kind1),
mConfig2(kind2)  
{
  gMessMgr->Info() << "StFtpcMixerMaker constructed" <<endm;
}


//! Destructor
StFtpcMixerMaker::~StFtpcMixerMaker() { }



//! InitRun method, reads in the database (FTPC geometry and calibration) 
Int_t StFtpcMixerMaker::InitRun(int RunId) {

  // get Database 
  // as long as calibrations do not change from event to event InitRun is the right place 

  St_DataSet *ftpc_geometry_db = GetDataBase("Geometry/ftpc");
  if ( !ftpc_geometry_db ){
    gMessMgr->Warning() << "StFtpcMixerMaker::Error Getting FTPC database: Geometry"<<endm;
    return kStWarn;
  }
  St_DataSetIter       dblocal_geometry(ftpc_geometry_db);
  
  m_dimensions = (St_ftpcDimensions *)dblocal_geometry("ftpcDimensions");
  m_padrow_z   = (St_ftpcPadrowZ  *)dblocal_geometry("ftpcPadrowZ");
  m_asicmap    = (St_ftpcAsicMap *)dblocal_geometry("ftpcAsicMap");

  
  St_DataSet *ftpc_calibrations_db = GetDataBase("Calibrations/ftpc");
  if ( !ftpc_calibrations_db ){
    gMessMgr->Warning() << "StFtpcMixerMaker::Error Getting FTPC database: Calibrations"<<endm;
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


  return kStOk;
}


//! Make method, does all the work
/*! StFtpcMixerMaker::Make() works in three steps:
    - both input datasets are read
    - the two inputs are mixed together
    - output sequences created with StFtpcSequencer
*/
Int_t StFtpcMixerMaker::Make() {
  
  // used to keep track of which FTPC readers are created here
  Int_t reader1Created = 0;
  Int_t reader2Created = 0;

  gMessMgr->Info() << "StFtpcMixerMaker starting..." <<endm;

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
                                                m_electronics);


  // get the two datasets

  if(!strcmp(getConfig1(),"daq")) {
    // DAQ
    gMessMgr->Info() << "StFtpcMixerMaker: Getting dataset 1 (DAQ)" << endm;
    St_DataSet *dataset1;
    dataset1=GetDataSet("Input1");               // get DataSet from chain1
    if (!dataset1) {
      gMessMgr->Warning() << "StFtpcMixerMaker: Missing Data! Bailing out..." <<endm;
      assert(dataset1);
    }
    daqr1=(StDAQReader*)(dataset1->GetObject()); // get DAQReader
    if (!daqr1) {
      gMessMgr->Warning() << "StFtpcMixerMaker: Missing Data! Bailing out..." <<endm;
      assert(daqr1);
    }
    ftpcr1=daqr1->getFTPCReader();               // get FTPCReader
    if (!ftpcr1) {
      gMessMgr->Warning() << "StFtpcMixerMaker: Missing Data! Bailing out..." <<endm;
      assert(ftpcr1);
    }

    if (!ftpcr1->checkForData()) {
      gMessMgr->Warning() << "No FTPC DAQ data available!" << endm;
	return kStWarn;
    }
    else {
      gMessMgr->Info() << "FTPC DAQ Dataset found!" <<endm;
    }

  } else {

    // FTPC SlowSimulator:
    gMessMgr->Info() << "StFtpcMixerMaker: Getting dataset 1 (SlowSimulator)" << endm;
    St_DataSet *dataset1;
    dataset1=GetDataSet("Input1");               // get DataSet from chain1
    if (!dataset1) {
      gMessMgr->Warning() << "StFtpcMixerMaker: Missing Data! Bailing out..." <<endm;
      assert(dataset1);
    }
    St_DataSet *raw = GetDataSet("ftpc_raw");
    if (raw) {
      gMessMgr->Info() << "FTPC SlowSimulator Dataset found!"<<endm;
      //                  FCL
      St_DataSetIter get(raw);

      St_fcl_ftpcsqndx *fcl_ftpcsqndx = (St_fcl_ftpcsqndx*)get("fcl_ftpcsqndx");
      St_fcl_ftpcadc   *fcl_ftpcadc   = (St_fcl_ftpcadc*  )get("fcl_ftpcadc");

      if (fcl_ftpcsqndx&&fcl_ftpcadc) {

	ftpcr1=new StFTPCReader((short unsigned int *) fcl_ftpcsqndx->GetTable(),
				    fcl_ftpcsqndx->GetNRows(),
				    (char *) fcl_ftpcadc->GetTable(),
				    fcl_ftpcadc->GetNRows());

	reader1Created = 1;
	gMessMgr->Info() << "created StFTPCReader from tables" << endm;
      }
      else {

	 gMessMgr->Info() <<"StFtpcMixerMaker: Tables are not found:"
					  << " fcl_ftpcsqndx = " << fcl_ftpcsqndx
					  << " fcl_ftpcadc   = " << fcl_ftpcadc << endm;
	 gMessMgr->Warning() << "StFtpcMixerMaker exiting... "<<endm;
	return kStOK;
      }
    }
    else {
      gMessMgr->Warning() << "Error getting FTPC SlowSimulator Dataset!" << endm;
    }

   }

  if(!strcmp(getConfig2(),"daq")) {
    // DAQ
    gMessMgr->Info() << "StFtpcMixerMaker: Getting dataset 2 (DAQ)" << endm;
    St_DataSet *dataset2;
    dataset2=GetDataSet("Input2");               // get DataSet from chain1
    if (!dataset2) {
      gMessMgr->Warning() << "StFtpcMixerMaker: Missing Data! Bailing out..." <<endm;
      assert(dataset2);
    }
    daqr2=(StDAQReader*)(dataset2->GetObject()); // get DAQReader
    if (!daqr2) {
      gMessMgr->Warning() << "StFtpcMixerMaker: Missing Data! Bailing out..." <<endm;
      assert(daqr2);
    }
    ftpcr2=daqr2->getFTPCReader();               // get FTPCReader
    if (!ftpcr2) {
      gMessMgr->Warning() << "StFtpcMixerMaker: Missing Data! Bailing out..." <<endm;
      assert(ftpcr2);
    }

    if (!ftpcr2->checkForData()) {
        gMessMgr->Warning()  << "No FTPC DAQ data available!" << endm;
	return kStWarn;
    }
    else {
      gMessMgr->Info() << "FTPC DAQ Dataset found!" <<endm;
    }

  } else {
    // FTPC SlowSimulator:
    gMessMgr->Info() << "StFtpcMixerMaker: Getting dataset 2 (SlowSimulator)" << endm;
    St_DataSet *dataset2;
    dataset2=GetDataSet("Input2");               // get DataSet from chain1
    if (!dataset2) {
      gMessMgr->Warning() << "StFtpcMixerMaker: Missing Data! Bailing out..." <<endm;
      assert(dataset2);
    }
    St_DataSet *raw = GetDataSet("ftpc_raw");
    if (raw) {
      gMessMgr->Info() << "FTPC SlowSimulator Dataset found!" << endm;
      //                  FCL
      St_DataSetIter get(raw);

      St_fcl_ftpcsqndx *fcl_ftpcsqndx = (St_fcl_ftpcsqndx*)get("fcl_ftpcsqndx");
      St_fcl_ftpcadc   *fcl_ftpcadc   = (St_fcl_ftpcadc*  )get("fcl_ftpcadc");



      if (fcl_ftpcsqndx&&fcl_ftpcadc) {

	ftpcr2=new StFTPCReader((short unsigned int *) fcl_ftpcsqndx->GetTable(),
				    fcl_ftpcsqndx->GetNRows(),
				    (char *) fcl_ftpcadc->GetTable(),
				    fcl_ftpcadc->GetNRows());

	reader2Created = 1;
	gMessMgr->Info() << "created StFTPCReader from tables" << endm;
      }
      else {

	gMessMgr->Info() <<"StFtpcMixerMaker: Tables are not found:"
					  << " fcl_ftpcsqndx = " << fcl_ftpcsqndx
					  << " fcl_ftpcadc   = " << fcl_ftpcadc << endm;
	gMessMgr->Warning() << "StFtpcMixerMaker exiting... " << endm;
	return kStOK;
      }
    }
    else {
      gMessMgr->Warning() << "Error getting FTPC SlowSimulator Dataset!" << endm;
    }

  }


  // Ok, all the readers are there, now let's take a look at the data of the FTPC


   // Array to store all ADC values & to do the mixing
  int *iADC = new int[dbReader->numberOfPadrows()
		      *dbReader->numberOfSectors()
		      *dbReader->numberOfPads()
		      *dbReader->numberOfTimebins()];

  for (Int_t i = 0; i < dbReader->numberOfPadrows() *dbReader->numberOfSectors() *dbReader->numberOfPads() *dbReader->numberOfTimebins(); i++)
    iADC[i] = 0;


  int iRow, iSec, iPad, iHardSec, iHardRow;
  int firstPadrowToSearch = dbReader->firstPadrowToSearch() - 1;

  TPCSequence *CurrentSequence;

  // Let's loop, do the mixing

  for (iRow = firstPadrowToSearch; iRow < dbReader->numberOfPadrows(); iRow++)  // loop over Rows
    {
      for (iSec = dbReader->firstSectorToSearch()-1; iSec < dbReader->lastSectorToSearch(); iSec++)  // loop over Sectors
	{
	  // get hardware (daq) numbering
	  iHardSec = dbReader->numberOfSectors() * (int)(iRow/2) + iSec + 1;
	  iHardRow = iRow%2 + 1;

	  // get occupied pads for first reader
	  unsigned char *(padlist[2]);
	  int iOccPads = ftpcr1->getPadList(iHardSec, iHardRow, padlist[iHardRow-1]);

	  int numberOfSequences;


	  for (int iPadCounter = 0; iPadCounter < iOccPads; iPadCounter++) // Loop over occupied pads
	    {
	      iPad = padlist[iHardRow-1][iPadCounter]; // carefull: This pad counts from 1 to 160!

	      ftpcr1->getSequences(iHardSec, iHardRow, iPad, &numberOfSequences, CurrentSequence);

	      iPad--; // iADC array counts pads from 0 to 159, important for StFtpcSequencer
	      
	      for (int iSeqCounter = 0; iSeqCounter < numberOfSequences; iSeqCounter++) // Loop over Sequences in Pad
		{
		  for (int iEntry = 0; iEntry < CurrentSequence[iSeqCounter].Length; iEntry++) // Loop over entries in sequence
		    {
		      iADC[iRow*dbReader->numberOfSectors() *dbReader->numberOfPads() *dbReader->numberOfTimebins()
			   + iSec*dbReader->numberOfPads() *dbReader->numberOfTimebins()
			   + iPad * dbReader->numberOfTimebins() + CurrentSequence[iSeqCounter].startTimeBin + iEntry] 
			+= (int)CurrentSequence[iSeqCounter].FirstAdc[iEntry];
		    }
		} // finish Loop over Sequences in Pad
	    } // finish Loop over occupied pads

	  iOccPads = ftpcr2->getPadList(iHardSec, iHardRow, padlist[iHardRow-1]);

	  for (int iPadCounter = 0; iPadCounter < iOccPads; iPadCounter++) // Loop over occupied pads
	    {
	      iPad = padlist[iHardRow-1][iPadCounter]; // carefull: This pad counts from 1 to 160!

	      ftpcr2->getSequences(iHardSec, iHardRow, iPad, &numberOfSequences, CurrentSequence);

	      iPad--; // iADC array counts pads from 0 to 159, important for StFtpcSequencer

	      for (int iSeqCounter = 0; iSeqCounter < numberOfSequences; iSeqCounter++) // Loop over Sequences in Pad
		{
		  for (int iEntry = 0; iEntry < CurrentSequence[iSeqCounter].Length; iEntry++) // Loop over entries in sequence
		    {
		      iADC[iRow*dbReader->numberOfSectors() *dbReader->numberOfPads() *dbReader->numberOfTimebins()
			   + iSec*dbReader->numberOfPads() *dbReader->numberOfTimebins()
			   + iPad * dbReader->numberOfTimebins() + CurrentSequence[iSeqCounter].startTimeBin + iEntry] 
			+= (int)CurrentSequence[iSeqCounter].FirstAdc[iEntry];

		    }

		} // finish Loop over Sequences in Pad
	    } // finish Loop over occupied pads

	} // finish Loop over Sectors
    } // finish Loop over Rows


  // Embedding done, now write out the Sequences

  gMessMgr->Info() << "FTPC Embedding done... "<< endm;

  St_DataSetIter local(m_DataSet);

  St_fcl_ftpcndx *fcl_ftpcndx_out = new St_fcl_ftpcndx("fcl_ftpcndx", 2);
  local.Add(fcl_ftpcndx_out);
  St_fcl_ftpcsqndx *fcl_ftpcsqndx_out = new St_fcl_ftpcsqndx("fcl_ftpcsqndx", 500000);
  local.Add(fcl_ftpcsqndx_out);
  St_fcl_ftpcadc *fcl_ftpcadc_out = new St_fcl_ftpcadc("fcl_ftpcadc", 2000000);
  local.Add(fcl_ftpcadc_out);

  gMessMgr->Info() << "Output sequences created..." << endm;

  // create StFtpcSequencer to write ADC data to sequences (zero suppressed)
  StFtpcSequencer *ftpcSequencer = new StFtpcSequencer(fcl_ftpcndx_out, fcl_ftpcsqndx_out, fcl_ftpcadc_out);

  ftpcSequencer->writeArray(iADC, dbReader->numberOfPadrows(), dbReader->numberOfSectors(), dbReader->numberOfPads(), dbReader->numberOfTimebins());


  delete ftpcSequencer;
  delete dbReader;
  if (reader1Created) delete ftpcr1;
  if (reader2Created) delete ftpcr2;
  delete[] iADC;

  gMessMgr->Info() << "FtpcMixerMaker done..." << endm;

  return kStOK;
} // Make()


//! Clear method
void StFtpcMixerMaker::Clear(Option_t *opt)
{
  StMaker::Clear();
}


//! Finish method
Int_t StFtpcMixerMaker::Finish()
{
  return kStOK;
}

 /***************************************************************************
 *
 * $Id: StFtpcMixerMaker.cxx,v 1.1 2003/02/14 18:11:25 fsimon Exp $
 *
 * $Log: StFtpcMixerMaker.cxx,v $
 * Revision 1.1  2003/02/14 18:11:25  fsimon
 * Initial commit of FTPC embedding code
 *
 *
 ***************************************************************************/
