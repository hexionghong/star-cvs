// $Id: StFtpcSlowSimMaker.cxx,v 1.1 2000/11/23 10:16:43 hummler Exp $
// $Log: StFtpcSlowSimMaker.cxx,v $
// Revision 1.1  2000/11/23 10:16:43  hummler
// New FTPC slow simulator in pure maker form
//
//
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// StFtpcSlowSimMaker class                                             //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include <iostream.h>
#include <stdlib.h>
#include "StFtpcSlowSimMaker.h"
#include "StFtpcSlowSimulator.hh"
#include "StFtpcRawWriter.hh"
#include "StFtpcClusterMaker/StFtpcParamReader.hh"
#include "StFtpcClusterMaker/StFtpcGeantReader.hh"

#include "StChain.h"
#include "St_DataSetIter.h"
#include "TH1.h"
#include "TH2.h"

#include "tables/St_g2t_track_Table.h"
#include "tables/St_g2t_ftp_hit_Table.h"
#include "tables/St_fss_param_Table.h"
#include "tables/St_fss_gas_Table.h"
#include "tables/St_fcl_padtrans_Table.h"
#include "tables/St_fcl_ftpcndx_Table.h" 
#include "tables/St_fcl_ftpcsqndx_Table.h" 
#include "tables/St_fcl_ftpcadc_Table.h" 
#include "tables/St_fcl_det_Table.h"

ClassImp(StFtpcSlowSimMaker)

//_____________________________________________________________________________
StFtpcSlowSimMaker::StFtpcSlowSimMaker(const char *name):
StMaker(name),
m_fss_gas(0),
m_fss_param(0),
m_padtrans(0),
m_det(0)
{
}
//_____________________________________________________________________________
StFtpcSlowSimMaker::~StFtpcSlowSimMaker(){
}
//_____________________________________________________________________________
Int_t StFtpcSlowSimMaker::Init(){
// Create tables
  St_DataSet *ftpc = GetDataBase("ftpc");
  assert(ftpc);
  St_DataSetIter       local(ftpc);

  m_fss_gas  = (St_fss_gas      *) local("fsspars/fss_gas");
  m_fss_param= (St_fss_param    *) local("fsspars/fss_param");
  m_padtrans = (St_fcl_padtrans *) local("fclpars/padtrans");
  m_det      = (St_fcl_det      *) local("fclpars/det");
  m_zrow     = (St_fcl_zrow     *) local("fclpars/zrow");

  
  // Create Histograms    
  m_nadc    = new TH1F("fss_total_adc","Total number of adcs in both FTPCs",1000,0.,2000000.);
  m_nsqndx  = new TH1F("fss_sqndx","FTPC raw data sequence index",100,0.,100000.);
  m_nadc_index1  = new TH2F("fss_nadc_index1","Total number of adcs vs. number of adcs in FTPC East",100,0.,2000000.,100,0.,1000000.);

  return StMaker::Init();
}
//_____________________________________________________________________________
Int_t StFtpcSlowSimMaker::Make(){

  St_DataSetIter geant(GetInputDS("geant"));
  St_g2t_vertex  *g2t_vertex  = (St_g2t_vertex *)  geant("g2t_vertex");
  St_g2t_track   *g2t_track   = (St_g2t_track *)   geant("g2t_track");
  St_g2t_ftp_hit *g2t_ftp_hit = (St_g2t_ftp_hit *) geant("g2t_ftp_hit");
  if (g2t_vertex && g2t_track && g2t_ftp_hit){
    
    St_DataSetIter local(m_DataSet); local.Cd("pixels");
    St_fcl_ftpcndx   *fcl_ftpcndx  = new St_fcl_ftpcndx("fcl_ftpcndx",2);
    local.Add(fcl_ftpcndx);
    St_fcl_ftpcsqndx *fcl_ftpcsqndx = new St_fcl_ftpcsqndx("fcl_ftpcsqndx",500000);
    local.Add(fcl_ftpcsqndx);
    St_fcl_ftpcadc   *fcl_ftpcadc  = new St_fcl_ftpcadc("fcl_ftpcadc",2000000);
    local.Add(fcl_ftpcadc);

    // create data reader
    StFtpcGeantReader *geantReader = new StFtpcGeantReader(g2t_vertex,
							   g2t_track,
							   g2t_ftp_hit);

    // create data writer
    StFtpcRawWriter *dataWriter = new StFtpcRawWriter(fcl_ftpcndx,
						      fcl_ftpcsqndx,
						      fcl_ftpcadc);
    
    // create parameter reader
    StFtpcParamReader *paramReader = new StFtpcParamReader(m_fss_gas,
							   m_fss_param,
							   m_padtrans,
							   m_det,
							   m_zrow);
  
    StFtpcSlowSimulator *slowsim = new StFtpcSlowSimulator(geantReader,
							   paramReader, 
							   dataWriter);
 
    cout<< " start StFtpcSlowSimulator "<<endl;
    Int_t Res_fss = slowsim->simulate();

    delete slowsim;
    delete paramReader;
    delete dataWriter;
    delete geantReader;

    if (Res_fss) {
      if(Debug()) cout<< " finished fss "<<endl;
    }
  }
  MakeHistograms(); // FTPC slow simulator histograms
  return kStOK;
}
//_____________________________________________________________________________
void StFtpcSlowSimMaker::MakeHistograms() {

   cout<<"*** NOW MAKING HISTOGRAMS FOR FtpcSlowSim ***"<<endl;

   // Create an iterator
   St_DataSetIter ftpc_raw(m_DataSet);
   
   //Get the tables
   St_fcl_ftpcadc   *adc = 0;
   St_fcl_ftpcndx   *ndx = 0;
   St_fcl_ftpcsqndx *sqndx = 0;
   adc              = (St_fcl_ftpcadc *) ftpc_raw.Find("fcl_ftpcadc");
   ndx              = (St_fcl_ftpcndx *) ftpc_raw.Find("fcl_ftpcndx");
   sqndx            = (St_fcl_ftpcsqndx *) ftpc_raw.Find("fcl_ftpcsqndx");
   // Fill histograms for FTPC slow simulator
   if (adc) {
     Float_t nadc = adc->GetNRows();
     printf("total # adcs = %ld, nadc = %f\n",adc->GetNRows(),nadc);
     m_nadc->Fill(nadc);
   }
   if (ndx) {
     fcl_ftpcndx_st *r = ndx->GetTable();
     Float_t index1 = ++r->index;
     printf("index1 = %d\n",r->index);
     if (adc) {
       m_nadc_index1->Fill((float)adc->GetNRows(),(float)index1); 
     }
   }
   if (sqndx) {
     fcl_ftpcsqndx_st *r = sqndx->GetTable();
     for (Int_t i=0; i<sqndx->GetNRows();i++,r++) {
       m_nsqndx->Fill((float)r->index);
     }
   }
}
//_____________________________________________________________________________

