// $Id: St_ebye_Maker.cxx,v 1.7 1999/01/21 19:13:44 dhammika Exp $
// $Log: St_ebye_Maker.cxx,v $
// Revision 1.7  1999/01/21 19:13:44  dhammika
// Updated ebye stuff which works for one event only
//
// Revision 1.7  1999/01/05 14:11:08  dhammika
// Updated to be in synch with stardev and the latest SCA V2.0 
//
// Revision 1.6  1998/10/31 00:26:13  fisyak
// Makers take care about branches
//
// Revision 1.5  1998/10/06 18:00:35  perev
// cleanup
//
// Revision 1.4  1998/09/23 20:22:57  fisyak
// Prerelease SL98h
//
// Revision 1.3  1998/09/15 20:55:20  fisyak
// Split St_DataSet -> St_DataSet + St_DataSetIter
//
// Revision 1.2  1998/08/07 19:26:10  dhammika
// event by event chain in root
//
// Revision 1.1  1998/08/05 14:33:36  fisyak
// Add ebye
//
// Revision 1.2  1998/07/21 01:04:39  fisyak
// Clean up
//
// Revision 1.1  1998/07/21 00:36:46  fisyak
// tcl and tpt
//
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// St_ebye_Maker class for Makers                                        //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include <iostream.h>
#include "St_ebye_Maker.h"
#include "StChain.h"
#include "St_DataSetIter.h"
#include "St_XDFFile.h"

//#include "St_dst_run_header_Table.h"
//#include "St_dst_event_header_Table.h"
//#include "St_dst_track_Table.h"

//#include "St_sca_switch_Table.h"
//#include "St_sca_filter_const_Table.h"

#include "ebye/St_sca_filter_Module.h"
#include "ebye/St_sca_runsca_Module.h"
#include "ebye/St_sca_makeprior_Module.h"
#include "ebye/St_sca_makeref_Module.h"

ClassImp(St_ebye_Maker)
#ifdef   DEBUG
#undef   DEBUG 
#endif
#define  DEBUG  1

//_____________________________________________________________________________
//St_ebye_Maker::St_ebye_Maker():
//m_sca_switch(0),
//m_sca_const(0),
//m_sca_filter_const(0),
//m_dst_track(0),
//m_particle(0),
//m_sca_in(0),
//m_sca_out(0),
//m_sca_prior(0),
//m_sca_ensemble_ave(0)
//{
//   drawinit=kFALSE;
//}
//_____________________________________________________________________________
St_ebye_Maker::St_ebye_Maker(const char *name, const char *title):
StMaker(name,title),
m_sca_switch(0),
m_sca_const(0),
m_sca_filter_const(0),
m_sca_prior(0),
m_sca_ensemble_ave(0),
this_dst_run_header(0),
this_dst_event_header(0),
this_dst_track(0),
this_sca_in(0),
this_sca_out(0)
{
   drawinit=kFALSE;
}
//_____________________________________________________________________________
St_ebye_Maker::~St_ebye_Maker()
{
}
//_____________________________________________________________________________
Int_t St_ebye_Maker::Init(){
  // Get the run header
  dst_run_header_st *run_header = 0;
  St_DataSet *runhead = gStChain->DataSet("run_summary");
  St_DataSetIter dstrun(runhead);
  if (DEBUG) runhead->ls("*");
  this_dst_run_header = (St_dst_run_header *) dstrun("run_header");
  if (this_dst_run_header) {
    run_header = this_dst_run_header->GetTable();
    if (run_header)
      cout << " ===> <St_ebye_Maker::Init()>: DST event type =" << run_header->event_type<< endl;
    else
      cout << " ===> <St_ebye_Maker::Init()>: Null pointer to run header table" << endl;
  }
  else {
    cout << " ===> <St_ebye_Maker::Init()>: No DST run header table; create one" << endl;
    this_dst_run_header  = new St_dst_run_header("run_header",1);
    dstrun.Add(this_dst_run_header,"dst");
    run_header = this_dst_run_header->GetTable();
    run_header->run_id =       1;
    this_dst_run_header->AddAt(&run_header,0);
    if(DEBUG)this_dst_run_header->ls("*");
    cout << " ===> <St_ebye_Maker::Init()>: Created run header table" << endl;
  }
  // Create tables
  St_DataSet *params = gStChain->DataSet("params");
  if (!params) {
    cout << " ===> <St_ebye_Maker::Init()>: No params Dataset; create params " << endl;
    params  = new St_DataSet("params");
  }
  if (DEBUG) {
    printf(" ===> <St_ebye_Maker::Init()>: *params = %d\n",params);
  } 
  St_DataSetIter     local(params);
  St_DataSet *ebye = local("ebye");
  //SafeDelete(ebye);  
  if (! ebye) ebye = local.Mkdir("ebye");
  //Char_t *ebye_pars = "${STAR}/params/ebye/sca_params.xdf";
  Char_t *ebye_pars = "/star/u2/dhammika/newupdate/params/ebye/sca_params.xdf";
  St_XDFFile::GetXdFile(ebye_pars,ebye);
  St_DataSet *sca = local("ebye/sca");
  if (!sca) { 
    printf(" ===> <St_ebye_Maker::Init()>: <<< ERROR >>> the file \"%s\" has no \"sca\" dataset\n",ebye_pars);
    return kStErr;
  }
  if (DEBUG)printf(" ===> <St_ebye_Maker::Init()>: Begin Iterating sca \n");
  St_DataSetIter scatable(sca);
  if (DEBUG)printf(" ===> <St_ebye_Maker::Init()>: Done Iterating sca \n");

  m_sca_switch           = (St_sca_switch *)       scatable("sca_switch");
  m_sca_const            = (St_sca_const *)        scatable("sca_const");
  m_sca_filter_const     = (St_sca_filter_const *) scatable("sca_filter_const");
  if(DEBUG)m_sca_const->ls("*");
  
  if (DEBUG) printf (" ===> <St_ebye_Maker::Init()>: \n \t m_sca_switch       = %d, \n \t m_sca_const        = %d, \n \t m_sca_filter_const = %d \n", 
		     m_sca_switch,m_sca_const,m_sca_filter_const);
  // Set switches to make propir
  sca_switch_st *sca_switch   = m_sca_switch->GetTable();
  sca_switch->makePrior       = 0;
  sca_switch->makeEnsembleAve = 0;
  sca_switch->doAnalysis      = 0;
  
  // Create Histograms    

  Int_t iret = StMaker::Init();
  if (DEBUG) printf (" ===> <St_ebye_Maker::Init()>: StMaker::Init() returned iret = %d \n",iret);
  return iret;
}
//_____________________________________________________________________________
Int_t St_ebye_Maker::Make(){
  Int_t iret = kStErr;
  //  PrintInfo();

  if (DEBUG)cout << " ===> <St_ebye_Maker::Make()>: Begin ebye Make" << endl;
  // Create the new tables
  
  this_sca_in            = new St_sca_in("sca_in",10000);
  this_sca_out           = new St_sca_out("sca_out",10000);
  m_DataSet->Add(this_sca_in);
  m_DataSet->Add(this_sca_out);
  
  St_DataSet *dst_set = gStChain->DataSet("dst");     
  if (!dst_set) {
    cout << " ===> <St_ebye_Maker::Make()>: <<< ERROR >>> No DST dataset " << endl;
    return kStErr;
  }
  else
    if(DEBUG)dst_set->ls("*");
  St_DataSetIter       dsttables(dst_set);
  this_dst_event_header  = (St_dst_event_header *)  dsttables("event_header");
  //this_dst_track         = (St_dst_track *)         dsttables("globtrk");
  this_dst_track         = (St_dst_track *)dsttables.FindObject("globtrk");  
  if (!this_dst_track ){
    cout << " ===> <St_ebye_Maker::Make()>: <<< ERROR >>> NULL pointer this_dst_track" << endl;
    dsttables.Du();  // This line is a new one
    return kStErr;
  }
  else
    dsttables.Du();  // This line is a new one

  if(DEBUG)this_dst_track->ls("*");
  iret = this_dst_track->HasData();
  if (!iret) {
    cout << " ===> <St_ebye_Maker::Make()>: <<< ERROR >>> No DST tracks" << endl;
    return kStErr;
  }
  if (DEBUG) printf(" ===> <St_ebye_Maker::Make()>: Begin sca_filter \n");
  iret = sca_filter(this_dst_run_header
		    ,this_dst_event_header
		    ,this_dst_track
		    ,m_sca_filter_const
		    ,m_sca_switch
		    ,m_sca_const
		    ,this_sca_in
		    );
  
  if (iret !=  kSTAFCV_OK){
    cout << " ===> <St_ebye_Maker::Make()>: <<< ERROR >>> sca_filter  failed" << endl;
    return kStErr;
  }
  
  if (DEBUG) printf(" ===> <St_ebye_Maker::Make()>: Begin sca_runsca \n");
  iret = sca_runsca(m_sca_switch
		    ,m_sca_const
		    ,this_sca_in
		    ,m_sca_ensemble_ave
		    ,m_sca_prior
		    ,this_sca_out
		    );
  
  if (iret !=  kSTAFCV_OK){
    cout << " ===> <St_ebye_Maker::Make()>: <<< ERROR >>> sca_runsca  failed " << endl;
    return kStErr;
  }
  if (DEBUG)cout << " ===> <St_ebye_Maker::Make()>: End ebye Make" << endl;
  //Histograms     
  return kStOK;
}
//_____________________________________________________________________________
Int_t St_ebye_Maker::SetmakePrior(Bool_t flag){
  if (!m_sca_switch) return kStErr;
  // Set switches to make propir
  sca_switch_st *sca_switch   = m_sca_switch->GetTable();
  sca_switch->makePrior       = flag;
  // Allocate dynamic memory for prior
  m_sca_prior         = new St_sca_prior("sca_prior", 300000);
  m_DataSet->Add(m_sca_prior);

  return kStOK;
}
//_____________________________________________________________________________
Int_t St_ebye_Maker::SetmakeEnsembleAve(Bool_t flag){
  if (!m_sca_switch) return kStErr;
  St_DataSet  *calib = gStChain->DataSet("calib");
  if (!calib) {
    cout << " ===> <St_ebye_Maker::Init()>: No calib  Dataset; create calib  " << endl;
    calib  = new St_DataSet("calib");
  }
  if (DEBUG) {
    printf(" ===> <St_ebye_Maker::Init()>: *calib  = %d\n",calib);
  }
  St_DataSetIter      local(calib);
  St_DataSet *ebye  = local("ebye");
  //SafeDelete(ebye);  
  if (! ebye) ebye = local.Mkdir("ebye");
  //Char_t *sca_prior = "${STAR_CALIB}/ebye/sca_prior_dir.xdf";
  Char_t *sca_prior = "/star/u2/dhammika/newupdate/calib/ebye/sca_prior_dir.xdf";
  St_XDFFile::GetXdFile(sca_prior,ebye);
  St_DataSet *scaprior = local("ebye/sca_prior_dir");
  if (!scaprior) { 
    printf(" ===> <St_ebye_Maker::SetmakeEnsembleAve()>: <<< ERROR >>>  the file \"%s\" has no \"sca_prior_dir\" dataset\n",sca_prior);
    return kStErr;
  }
  
  St_DataSetIter scatable(scaprior);
  m_sca_prior           = (St_sca_prior *)       scatable("sca_prior");
  if (!m_sca_prior)
    cout << " ===> <St_ebye_Maker::SetdoAnalysis()>: <<< ERROR >>> No sca_prior table " << endl;
  else
    if(DEBUG)m_sca_prior->ls("*"); 
  
  // Set switches to make propir
  sca_switch_st *sca_switch   = m_sca_switch->GetTable();
  sca_switch->makeEnsembleAve = flag;

  // Allocate dynamic memory for ensemble average
  m_sca_ensemble_ave  = new St_sca_out("sca_ensemble_ave",1);
  m_DataSet->Add(m_sca_ensemble_ave);

  return kStOK;
}
//_____________________________________________________________________________
Int_t St_ebye_Maker::SetdoAnalysis(Bool_t flag){

  if (DEBUG) printf (" ===> <St_ebye_Maker::SetdoAnalysis()>: Begin\n");
  if (!m_sca_switch) return  kStErr;
  St_DataSet *calib = gStChain->DataSet("calib");
  if (!calib) {
    cout << " ===> <St_ebye_Maker::Init()>: No calib  Dataset; create calib  " << endl;
    calib  = new St_DataSet("calib");
  }
  if (DEBUG) {
    printf(" ===> <St_ebye_Maker::Init()>: *calib  = %d\n",calib);
  }
  St_DataSetIter      local(calib);
  St_DataSet *ebye  = local("ebye");
  //SafeDelete(ebye);  
  if (! ebye) {
    if (DEBUG) printf(" ===> <St_ebye_Maker::SetdoAnalysis()>: calib/ebye doesn't exist. Create it\n");
    ebye = local.Mkdir("ebye");
  }
  //Char_t *sca_prior = "${STAR_CALIB}/ebye/sca_prior_dir.xdf";
  Char_t *sca_prior = "/star/u2/dhammika/newupdate/calib/ebye/sca_prior_dir.xdf";
  St_XDFFile::GetXdFile(sca_prior,ebye);
  //Char_t *sca_ensmave = "${STAR_CALIB}/ebye/sca_ensemble_dir.xdf";
  Char_t *sca_ensmave = "/star/u2/dhammika/newupdate/calib/ebye/sca_ensemble_dir.xdf";
  St_XDFFile::GetXdFile(sca_ensmave,ebye);
  St_DataSet *scaprior = local("ebye/sca_prior_dir");
  if (!scaprior) { 
    printf(" ===> <St_ebye_Maker::SetdoAnalysis()>: <<< ERROR >>> the file \"%s\" has no \"sca_prior_dir\" dataset\n",sca_prior);
    return kStErr;
  }
  St_DataSetIter priortable(scaprior);
  m_sca_prior           = (St_sca_prior *)    priortable("sca_prior");
  if (!m_sca_prior)
    cout << " ===> <St_ebye_Maker::SetdoAnalysis()>: <<< ERROR >>> No sca_prior table " << endl;
  else
    if(DEBUG)m_sca_prior->ls("*"); 
  St_DataSet *scaref = local("ebye/sca_ensemble_dir");
  if (!scaref) { 
    printf(" ===> <St_ebye_Maker::SetdoAnalysis()>: <<< ERROR >>> the file \"%s\" has no \"sca_ensemble_dir\" dataset\n",sca_ensmave);
    return kStErr;
  }
  St_DataSetIter ensavetable(scaref);
  m_sca_ensemble_ave    = (St_sca_out *)     ensavetable("sca_ensemble_ave");
  if (!m_sca_ensemble_ave)
    cout << " ===> <St_ebye_Maker::SetdoAnalysis()>: <<< ERROR >>> No sca_ensemble_ave table " << endl;
  else
    if(DEBUG)m_sca_ensemble_ave->ls("*"); 

  // Set switches to make propir
  sca_switch_st *sca_switch   = m_sca_switch->GetTable();
  sca_switch->doAnalysis      = flag;
  if (DEBUG) printf (" ===> <St_ebye_Maker::SetdoAnalysis()>: End\n");
  return kStOK;
}
//_____________________________________________________________________________
Int_t St_ebye_Maker::SetnEvents(Int_t nEvents){
  if (!m_sca_switch) return kStErr;
  // Set switches to make propir
  sca_switch_st *sca_switch   = m_sca_switch->GetTable();
  sca_switch->nEvents         = nEvents;
  return kStOK;
}
//_____________________________________________________________________________
Int_t St_ebye_Maker::PutPrior(){
  Int_t iret = kStErr;

  iret = sca_makeprior(m_sca_prior);
  if (iret !=  kSTAFCV_OK){
    cout << " ===> <St_ebye_Maker::PutPrior()>: <<< ERROR >>> sca_makeprior  failed" << endl;
    return kStErr;
  }
  return kStOK;
}
//_____________________________________________________________________________
Int_t St_ebye_Maker::PutEnsembleAve(){
  Int_t iret = kStErr;

  iret = sca_makeref(m_sca_switch
		     ,m_sca_ensemble_ave);
  if (iret !=  kSTAFCV_OK){
    cout << " ===> <St_ebye_Maker::PutEnsembleAve()>: <<< ERROR >>> sca_makeref failed" << endl;
    return kStErr;
  }
  return kStOK;
}
//_____________________________________________________________________________
void St_ebye_Maker::PrintInfo(){
  printf("**************************************************************\n");
  printf("* $Id: St_ebye_Maker.cxx,v 1.7 1999/01/21 19:13:44 dhammika Exp $\n");
  //  printf("* %s    *\n",m_VersionCVS);
  printf("**************************************************************\n");
  if (gStChain->Debug()) StMaker::PrintInfo();
}
