// $Id: StFtpcTrackMaker.cxx,v 1.10 2000/07/18 21:22:17 oldi Exp $
// $Log: StFtpcTrackMaker.cxx,v $
// Revision 1.10  2000/07/18 21:22:17  oldi
// Changes due to be able to find laser tracks.
// Cleanup: - new functions in StFtpcConfMapper, StFtpcTrack, and StFtpcPoint
//            to bundle often called functions
//          - short functions inlined
//          - formulas of StFormulary made static
//          - avoid streaming of objects of unknown size
//            (removes the bunch of CINT warnings during compile time)
//          - two or three minor bugs cured
//
// Revision 1.9  2000/07/03 12:45:23  jcs
// get (pre)Vertex coordinates directly from (pre)Vertex table instead of from
// fptpars
//
// Revision 1.8  2000/06/26 22:10:44  fisyak
// remove params
//
// Revision 1.7  2000/06/15 09:13:34  oldi
// No tracking is performed (return kStWarn instead) if the z-position of the
// main vertex is off by more than 100 cm from z = 0. Different error messages
// (depending on how far the vertex is off) are printed.
//
// Revision 1.6  2000/06/13 14:25:56  oldi
// Changed cout to gMessMgr->Message().
// Printed output changed (slightly).
//
// Revision 1.5  2000/06/07 11:16:29  oldi
// Changed 0 pointers to NULL pointers.
// Function HandleSplitTracks() called.
//
// Revision 1.4  2000/05/15 14:28:12  oldi
// problem of preVertex solved: if no main vertex is found (z = NaN) StFtpcTrackMaker stops with kStWarn,
// refitting procedure completed and included in StFtpcTrackMaker (commented),
// new constructor of StFtpcVertex due to refitting procedure,
// minor cosmetic changes
//
// Revision 1.3  2000/05/12 12:59:16  oldi
// removed delete operator for mSegment in StFtpcConfMapper (mSegment was deleted twice),
// add two new constructors for StFtpcTracker to be able to refit already existing tracks,
// minor cosmetics
//
// Revision 1.2  2000/05/11 15:14:52  oldi
// Changed class names *Hit.* due to already existing class StFtpcHit.cxx in StEvent
//
// Revision 1.1  2000/05/10 13:39:28  oldi
// Initial version of StFtpcTrackMaker
//

//----------Author:        Markus D. Oldenburg
//----------Last Modified: 17.07.2000
//----------Copyright:     &copy MDO Production 1999

#include <iostream.h>

#include "StFtpcTrackMaker.h"
#include "StFtpcVertex.hh"
#include "StFtpcConfMapper.hh"
#include "StFtpcDisplay.hh"
#include "StFtpcTrackEvaluator.hh"

#include "St_DataSet.h"
#include "St_DataSetIter.h"

#include "StChain.h"
#include "StVertexId.h"

#include "ftpc/St_fpt_Module.h"
#include "ftpc/St_fde_Module.h"
#include "tables/St_g2t_vertex_Table.h"

#include "tables/St_fpt_fptrack_Table.h"
#include "tables/St_ffs_gepoint_Table.h"
#include "tables/St_g2t_track_Table.h"
#include "tables/St_dst_vertex_Table.h"

#include "TH1.h"
#include "TH2.h"
#include "TH3.h"
#include "TProfile.h"
#include "TClonesArray.h"
#include "TCanvas.h"
#include "TFile.h"

#include "StMessMgr.h"

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// StFtpcTrkMaker class for Makers                                      //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

ClassImp(StFtpcTrackMaker)

//_____________________________________________________________________________
StFtpcTrackMaker::StFtpcTrackMaker(const char *name) : StMaker(name),  m_fdepar(0)
{
  // Default constructor.
}

//_____________________________________________________________________________
StFtpcTrackMaker::~StFtpcTrackMaker()
{
  // Destructor.
}

//_____________________________________________________________________________
Int_t StFtpcTrackMaker::Init()
{
  // Initialisation.

  St_DataSet *ftpcpars = GetInputDB("ftpc");
  assert(ftpcpars);
  St_DataSetIter  gime(ftpcpars);
  m_fdepar = (St_fde_fdepar *) gime("fdepars/fdepar");
  
  // Create Histograms    
  m_q            = new TH1F("fpt_q"         ,"FTPC track charge"                               ,  3,-2. ,  2.  );
  m_theta        = new TH1F("fpt_theta"     ,"FTPC theta"                                      ,100,-5.0,  5.0 );
  m_ndedx        = new TH1F("fde_ndedx"     ,"Number of points used in FTPC dE/dx calculation" , 10, 1. , 11.  );
  m_found        = new TH1F("fpt_nrec"      ,"FTPC: number of points found per track"          , 10, 1. ,  11. );
  m_track        = new TH1F("fpt_track"     ,"FTPC: number of tracks found"                    ,100, 1. ,5000. );    
  m_nrec_track   = new TH2F("fpt_hits_mom"  ,"FTPC: points found per track vs. momentum"       , 10, 1. ,  11. , 100, 1., 20.);
 
  return StMaker::Init();
}

//_____________________________________________________________________________
Int_t StFtpcTrackMaker::Make()
{
  // Setup and tracking.

  gMessMgr->Message("", "I", "OST") << "Tracking (FTPC) started..." << endm;

  St_DataSet *ftpc_data = GetDataSet("ftpc_hits");
  St_fpt_fptrack *fpt_fptrack = NULL;
  
  if (!ftpc_data) {
    return kStWarn;
  }
  
  //  clusters exist -> do tracking
  St_fcl_fppoint *fcl_fppoint = (St_fcl_fppoint *)ftpc_data->Find("fcl_fppoint");
  
  if (!fcl_fppoint) {
    return kStWarn;
  }
  
  Int_t iflag = 0;
  
  //pointer to preVertex dataset
  St_DataSet *preVertex = GetDataSet("preVertex"); 
  
  //iterator
  St_DataSetIter preVertexI(preVertex);
  
  //pointer to preVertex
  St_dst_vertex  *preVtx  = (St_dst_vertex *)preVertexI("preVertex");
  gMessMgr->Message("", "I", "OST") << "Using primary vertex coordinates "; 
  
  if (preVtx) {
    dst_vertex_st *preVtxPtr = preVtx->GetTable();
    
    for (Int_t i = 0; i <preVtx->GetNRows();i++,preVtxPtr++) {
      
      if (preVtxPtr->iflag == 101) {
	iflag = 101;
	*gMessMgr << "(preVertex): ";
      }
    }
  }
  
  if ( iflag != 101 ) {
    //    preVertex not found  - compute and store Holm's preVertex
    *gMessMgr << "(Holm's vertex): ";

    StFtpcVertex *vertex = new StFtpcVertex(fcl_fppoint->GetTable(), fcl_fppoint->GetNRows());

    if (isnan(vertex->GetZ())) {
      // handles problem if there are not enough tracks and therefore a vertex cannot be found
      *gMessMgr << endm;
      gMessMgr->Message("", "E", "OST") << "No vertex found! Ftpc tracking stopped!" << endm;
      delete vertex;

      // No Tracking
      return kStWarn;
    }

    else {

      if (!preVtx) {
	// no preVertex table exists
	// create preVertex table with 1 row
	preVtx = new St_dst_vertex("preVertex", 1);
	preVtx->SetNRows(1);
	AddData(preVtx);
      }
      
      else {
	// correct preVertex not found
	// add a row to preVertex
	Int_t numRowPreVtx = preVtx->GetNRows(); 
	preVtx->ReAllocate(numRowPreVtx+1);
	preVtx->SetNRows(numRowPreVtx+1);
      }
      
      dst_vertex_st *preVtxPtr = preVtx->GetTable();
      preVtxPtr = preVtxPtr + preVtx->GetNRows() - 1;
      
      // save results in preVertex    
      preVtxPtr->x = 0.0;
      preVtxPtr->y = 0.0;
      preVtxPtr->z = vertex->GetZ();
      preVtxPtr->iflag = 301;
      preVtxPtr->det_id = 4;
      preVtxPtr->id = preVtx->GetNRows();
      preVtxPtr->vtx_id = kEventVtxId;  
    }

    delete vertex;
  }
  
    dst_vertex_st *preVtxPtr = preVtx->GetTable();
    
    for (Int_t i = 0; i <preVtx->GetNRows();i++,preVtxPtr++) {
      
      if (preVtxPtr->iflag == 101) {
        break;
      }
    }

  *gMessMgr << " " << preVtxPtr->x << ", " << preVtxPtr->y << ", " << preVtxPtr->z << "." << endm;
  

  // check for the position of the main vertex

  Double_t z = TMath::Abs(preVtxPtr->z);
  
  if (z > 50.) {
    
    if (z > 162.45) {
      gMessMgr->Message("Found vertex lies inside of one Ftpc. No Ftpc tracking possible.", "E", "OTS");
      
      // No tracking!
      return kStWarn;   
    }
    
    else if (z > 100.) {
      gMessMgr->Message("Found vertex is more than 100 cm off from z = 0. Ftpc tracking makes no sense.", "E", "OTS");
      
      // No tracking!
      return kStWarn;
    }
    
    else {
      gMessMgr->Message("Found vertex is more than 50 cm off from z = 0 but  Ftpc tracking is still possible", "W", "OTS");
      // Do tracking.
    }
  }

  Double_t vertexPos[3] = {preVtxPtr->x, preVtxPtr->y, preVtxPtr->z};
  StFtpcConfMapper *tracker = new StFtpcConfMapper(fcl_fppoint, vertexPos, Debug());

  // tracking 
  tracker->MainVertexTracking();

  // for the line above you have these possibilities
  //tracker->MainVertexTracking();
  //tracker->FreeTracking();
  //tracker->LaserTracking();

  if (Debug()) {
    tracker->SettingInfo();
    tracker->CutInfo();
    tracker->TrackingInfo();
  }
  
  if (fpt_fptrack) {
    delete fpt_fptrack;
  }

  fpt_fptrack = new St_fpt_fptrack("fpt_fptrack", 20000);
  m_DataSet->Add(fpt_fptrack);
  tracker->FitAndWrite(fpt_fptrack, -preVtxPtr->id);
  
  // dE/dx calculation
  if (Debug()) {
    gMessMgr->Message("", "I", "OST") << "dE/dx module (fde) started" << endm;
  }

  Int_t Res_fde = fde(fcl_fppoint, fpt_fptrack, m_fdepar);
  
  if(Debug()) {
    gMessMgr->Message("", "I", "OST") << "dE/dx module finished: " << Res_fde << endm;
  }

  /*
  // Track Display
  
  // Uncomment this block if you want to see (I mean see!) the found tracks.
  
  StFtpcDisplay *display = new StFtpcDisplay(tracker->GetClusters(), tracker->GetTracks());
  //display->TrackInfo();
  //display->Info();
  //display->ShowClusters();
  display->ShowTracks();
  delete display;
  */

  /*
  // Track Evaluator
  
  // Uncomment this block to get information about the quality 
  // of the found tracks in comparison to the simulated input event.
  
  St_DataSet *geant = GetInputDS("geant");  
  
  StFtpcTrackEvaluator *eval = new StFtpcTrackEvaluator(geant, ftpc_data, tracker->GetVertex(), tracker->GetClusters(), tracker->GetTracks(), "ftpc_evaluator.root", "RECREATE");
  eval->Info();
  eval->FillHitsOnTrack();
  eval->FillParentHistos();
  eval->FillMomentumHistos();
  eval->FillEventHistos();
  eval->FillCutHistos();
  eval->DivideHistos();
  eval->WriteHistos();
  
  // Uncomment the following line if you want to 'see' the information (split tracks, unclean tracks, ...) 
  // evaluated by the TrackEvaluator.  
  //eval->ShowTracks();
  
  delete eval;
  */
  
  delete tracker;

  /*
  // Refitting
  // To do refitting of the tracks after some other module has found a 'better' 
  // main vertex position include the following lines and insert the new vertex position. 
  
  St_DataSet *hit_data = GetDataSet("ftpc_hits");   
  St_fcl_fppoint *points = (St_fcl_fppoint *)hit_data->Find("fcl_fppoint");
  St_DataSet *track_data = GetDataSet("ftpc_tracks"); 
  St_fpt_fptrack *tracks = (St_fpt_fptrack *)track_data->Find("fpt_fptrack");
  
  StFtpcVertex *refit_vertex = new StFtpcVertex(0., 0., 0.);   // insert vertex position (x, y, z) here!
  StFtpcTracker *refitter = new StFtpcTracker(refit_vertex, points, tracks, 1.);
  refitter->FitAndWrite(tracks);
  delete refitter;
  delete refit_vertex;
  */

  MakeHistograms();
  gMessMgr->Message("", "I", "OST") << "Tracking (FTPC) completed." << endm;

  return kStOK;;
}


//_____________________________________________________________________________
void StFtpcTrackMaker::MakeHistograms()
{
  // Fill histograms.

  St_DataSetIter ftpc_tracks(m_DataSet);

  //Get the table
  St_fpt_fptrack *trk = NULL;
  trk = (St_fpt_fptrack *) ftpc_tracks.Find("fpt_fptrack");

  if (trk) {
   // Fill histograms for FTPC fpt,fte,fde

    fpt_fptrack_st *r = trk->GetTable();
    for (Int_t i=0; i<trk->GetNRows();i++,r++) {
      m_found->Fill((float)(r->nrec));
      m_q->Fill((float)(r->q));
      m_theta->Fill(r->theta);
      m_ndedx->Fill((float)(r->ndedx));
      float mom=sqrt(r->p[0] * r->p[0] + r->p[1] * r->p[1] + r->p[2] * r->p[2]);
      m_nrec_track->Fill((float)(r->nrec),mom);
    }
  }
}


//_____________________________________________________________________________
void StFtpcTrackMaker::PrintInfo()
{
  // Prints information.

  gMessMgr->Message("", "I", "OST") << "******************************************************************" << endm;
  gMessMgr->Message("", "I", "OST") << "* $Id: StFtpcTrackMaker.cxx,v 1.10 2000/07/18 21:22:17 oldi Exp $ *" << endm;
  gMessMgr->Message("", "I", "OST") << "******************************************************************" << endm;
  
  if (Debug()) {
    StMaker::PrintInfo();
  }
}

