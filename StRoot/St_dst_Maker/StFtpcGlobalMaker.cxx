// $Id: StFtpcGlobalMaker.cxx,v 1.14 2003/09/02 17:59:25 perev Exp $
// $Log: StFtpcGlobalMaker.cxx,v $
// Revision 1.14  2003/09/02 17:59:25  perev
// gcc 3.2 updates + WarnOff
//
// Revision 1.13  2002/11/28 10:08:12  jcs
// simplify id_start_vertex comment and code
//
// Revision 1.12  2002/11/28 09:42:05  oldi
// Code was prepared to fill momentum values at outermost points on tracks.
// This feature is not used up to now.
//
// Revision 1.11  2002/11/25 12:06:29  jcs
// set bit 0 of map[0]=0 for all ftpc global tracks (unconstrained fit)
//
// Revision 1.10  2002/11/06 13:48:25  oldi
// Vertex handling simplifed.
// Global/primary fit handling simplified.
//
// Revision 1.9  2002/10/31 13:43:24  oldi
// dE/dx parameters read from database, now.
// Code cleanup (indention).
//
// Revision 1.8  2002/10/29 15:57:56  jcs
// inactivate all code necessary for redoing track fit with primary vertex
//
// Revision 1.7  2002/10/11 15:47:33  oldi
// Code cleanup (several lines of code changed due to *params -> Instance()).
//
// Revision 1.6  2002/08/02 11:22:31  oldi
// MaxDCA is taken from StFtpcTrackingParams, now (it was hardcoded before).
//
// Revision 1.5  2002/04/05 16:52:47  oldi
// Minor changes:
// Global refit was removed, because TPC vertex is known at tracking time already.
// Chi2 calculation was fixed.
//
// Revision 1.4  2002/02/01 01:59:25  jcs
// redo unconstrained fit for FTPC global tracks with primary vertex
// (done in StFtpcTrackMaker with preVertex)
//
// Revision 1.3  2002/01/30 15:14:02  jcs
// incorporate fill_ftpc_dst.cc
// write out all FTPC hits, not just those on tracks
//
// Revision 1.2  2001/03/30 13:30:11  jcs
// correct Id and Log
//
//
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// StFtpcGlobalMaker class                                              //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include <Stiostream.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include "TMath.h"
#include "StFtpcGlobalMaker.h"
#include "StFtpcTrackMaker/StFtpcVertex.hh"
#include "StFtpcTrackMaker/StFtpcTracker.hh"
#include "StFtpcTrackMaker/StFtpcTrackingParams.hh"

#include "StChain.h"
#include "St_DataSet.h"
#include "St_DataSetIter.h"

#include "StMessMgr.h"

#include "StDetectorId.h"
#include "StVertexId.h"
#include "StTrackMethod.h"
#include "StDedxMethod.h"
#include "math_constants.h"

ClassImp(StFtpcGlobalMaker)
  
//_____________________________________________________________________________
StFtpcGlobalMaker::StFtpcGlobalMaker(const char *name):StMaker(name){
}

//_____________________________________________________________________________
StFtpcGlobalMaker::~StFtpcGlobalMaker(){
}

//_____________________________________________________________________________
Int_t StFtpcGlobalMaker::Init(){
  // Create tables
  
  return StMaker::Init();
}

//_____________________________________________________________________________
Int_t StFtpcGlobalMaker::Make(){
  PrintInfo();  
  int iMake = kStOK;

#ifdef REFIT_FTPC_TRACKS
  // if FTPC tracking is done before the primary vertex is found, 
  // i.e. if fpt is in ftpcChain instead of in globalChain,
  // the  tracks must be refit 
  St_DataSet *primary = GetDataSet("primary");
  if (!primary) {
    gMessMgr->Warning() << "StFtpcGlobalMaker::Make(): primary is missing" << endm;
    return kStWarn;
  }

  St_dst_vertex *vertex = (St_dst_vertex *) primary->Find("vertex");
  if (!vertex) {
    gMessMgr->Warning() << "StFtpcGlobalMaker::Make(): vertex is missing" << endm;
    return kStWarn;
  }

  dst_vertex_st *primvtx = vertex->GetTable();

  if( primvtx->vtx_id != kEventVtxId || primvtx->iflag != 1){
    for( Int_t no_rows=0; no_rows<vertex->GetNRows(); no_rows++,primvtx++){
      if( primvtx->vtx_id == kEventVtxId && primvtx->iflag == 1 ) break;
    }
  }
  if( primvtx->vtx_id != kEventVtxId || primvtx->iflag != 1){
    gMessMgr->Warning() << "StFtpcGlobalMaker::Make(): primary vertex is missing" << endm;
    return kStWarn;
  }
#endif

  St_DataSet *ftpc_tracks = GetDataSet("ftpc_tracks");
  if (!ftpc_tracks) {
    gMessMgr->Warning() << "StFtpcGlobalMaker::Make(): ftpc_tracks is missing" << endm;
    return kStWarn;
  }
  St_fpt_fptrack *fpt_fptrack = 0;
  fpt_fptrack = (St_fpt_fptrack *) ftpc_tracks->Find("fpt_fptrack");
  if (!fpt_fptrack) {
    gMessMgr->Warning() << "StFtpcGlobalMaker::Make(): fpt_fptrack is missing" << endm;
    return kStWarn;
  }
  Int_t nfptrack = fpt_fptrack->GetNRows();
  fpt_fptrack_st *fptrack = fpt_fptrack->GetTable();
  
  St_DataSet *ftpc_hits   = GetDataSet("ftpc_hits");
  if (!ftpc_hits) {
    gMessMgr->Warning() << "StFtpcGlobalMaker::Make(): ftpc_hits is missing" << endm;
    return kStWarn;
  }
  St_fcl_fppoint *fcl_fppoint = 0;
  fcl_fppoint = (St_fcl_fppoint *) ftpc_hits->Find("fcl_fppoint");
  if (!fcl_fppoint) {
    gMessMgr->Warning() << "StFtpcGlobalMaker::Make(): fcl_fppoint is missing" << endm;
    return kStWarn;
  }
  Int_t nfppoint = fcl_fppoint->GetNRows();
  fcl_fppoint_st *fppoint = fcl_fppoint->GetTable();

  Int_t iglobtrk = 0;
  St_dst_track *dst_track=0;
  St_DataSet *match = GetDataSet("match");
  if (match) {
    dst_track = (St_dst_track *) match->Find("globtrk");
    if (dst_track) {
      iglobtrk = dst_track->GetNRows();
      dst_track->ReAllocate(iglobtrk + nfptrack);
    }
  }
  if (!dst_track) {
    dst_track = new St_dst_track("globtrk", nfptrack); 
    AddData(dst_track);
  }
  dst_track_st *globtrk = dst_track->GetTable();

  St_dst_point *dst_point = new St_dst_point("point",nfppoint);  
  AddData(dst_point);
  dst_point_st *point = dst_point->GetTable();

  St_dst_dedx *dst_dedx = new St_dst_dedx("dst_dedx",nfptrack); 
  AddData(dst_dedx);
  dst_dedx_st *dedx = dst_dedx->GetTable();
  Int_t idedx = 0;
  St_DataSet *ftpcpars = GetInputDB("ftpc");
  assert(ftpcpars);
  St_DataSetIter gime(ftpcpars);
  m_fdepar = (St_fde_fdepar *) gime("fdepars/fdepar");
  fde_fdepar_st *fdepar = m_fdepar->GetTable();

  gMessMgr->Message("", "I", "OST") << "Global fit for FTPC tracks not redone, because vertex used for tracking was the primary vertex." << endm;

#ifdef REFIT_FTPC_TRACKS
  // Redo unconstrained fit with primary vertex instead of preVertex
  StFtpcVertex *refit_vertex = new StFtpcVertex(primvtx);
  gMessMgr->Info() << "Using primary vertex: "<< *refit_vertex << endm;
  Bool_t bench = (Bool_t)false;
  StFtpcTracker *refitter = new StFtpcTracker(refit_vertex, fcl_fppoint, fpt_fptrack, bench, 
					      StFtpcTrackingParams::Instance()->MaxDca(0));
  refitter->GlobalFitAnddEdxAndWrite(fpt_fptrack);
  delete refitter;
  delete refit_vertex;
#endif

  const int  MAXHITS = 10;       // Maximum number of hits on an FTPC track
  Int_t ihit, iPoint;

  // Loop over all tracks in FTPC track table
  for (Int_t itrk=0; itrk<nfptrack; itrk++,iglobtrk++,idedx++) {

    //  Primary key
    globtrk[iglobtrk].id      = iglobtrk + 1;
    fptrack[itrk].id_globtrk = globtrk[iglobtrk].id; 

    //  initialize map (=0 for global tracks = unconstrained fit)
    globtrk[iglobtrk].map[0] = 0;
    
    //  initialize map[1] = Format interpreter -  set bit 31 for FTPC
    globtrk[iglobtrk].map[1]   =  (1<<31);

    //  initialize det_id 
    globtrk[iglobtrk].det_id   = 0;

    //  Loop over all hits on track 
    for (ihit=0; ihit<MAXHITS; ihit++) {
      if (fptrack[itrk].hitid[ihit] > -1){
	//         hitid array filled by FORTRAN routine, must -1 for C routine
	iPoint =  fptrack[itrk].hitid[ihit] - 1;
	if (globtrk[iglobtrk].det_id == 0 ) { 
	  //                 Save first hit on current track and determine detector id
	  globtrk[iglobtrk].x_first[0]    = 
	    fppoint[iPoint].x;
	  globtrk[iglobtrk].x_first[1]    = 
	    fppoint[iPoint].y;
	  globtrk[iglobtrk].x_first[2]    = 
	    fppoint[iPoint].z ;
	  //                 Rows 1->10 FTPC West  det_id= kFtpcWestId
	  if (fppoint[iPoint].row >= 1 && fppoint[iPoint].row <= 10) {
	    globtrk[iglobtrk].det_id  = kFtpcWestId;    // West
	  }
	  //                 Rows 11->20 FTPC East  det_id=kFtpcEastId  
	  else if (fppoint[iPoint].row >= 11 && fppoint[iPoint].row <=20) {
	    globtrk[iglobtrk].det_id  = kFtpcEastId;   // East
	  }
	}
	globtrk[iglobtrk].map[0] |= (1<<fppoint[iPoint].row);

	fppoint[iPoint].row = fppoint[iPoint].row + 100*globtrk[iglobtrk].id;

      }  // end of processing current hit
    }  // end of processing all hits on track

    //  Track finding and track fitting method 
    //   (Method: FTPC Conformal Mapping - set bit 10 )          
    //   (Fitter: kHelix2StepId)
    globtrk[iglobtrk].method = (1<<10) + (1<<kHelix2StepId);

    //  Geant particle ID number for mass hypothesis used in tracking
    //   (Currently not set for FTPC)                               
    globtrk[iglobtrk].pid    = 0;

    //  Number of points 
    globtrk[iglobtrk].n_point = fptrack[itrk].nrec;

    //  Number of points used in fit
    globtrk[iglobtrk].n_fit_point  = fptrack[itrk].nfit;

    //  Charge 
    globtrk[iglobtrk].icharge      = fptrack[itrk].q;

    //  If this is a primary track candidate
    globtrk[iglobtrk].id_start_vertex  = 10*fptrack[itrk].id_start_vertex;


    //  radius at start of track (cm) 
    globtrk[iglobtrk].r0   = 
      ::sqrt(fptrack[itrk].v[0]*fptrack[itrk].v[0]
	   + fptrack[itrk].v[1]*fptrack[itrk].v[1]);

    //  azimuthal angle at start of track (deg)
    globtrk[iglobtrk].phi0 = 
      atan2(fptrack[itrk].v[1],fptrack[itrk].v[0])
      * C_DEG_PER_RAD;

    //  z-coordinate at start of track 
    globtrk[iglobtrk].z0 = fptrack[itrk].v[2];

    //  momentum angle at start 
    globtrk[iglobtrk].psi = 
      atan2(fptrack[itrk].p[1],fptrack[itrk].p[0]);
    if (globtrk[iglobtrk].psi < 0.0) {
      globtrk[iglobtrk].psi = 
	globtrk[iglobtrk].psi + C_2PI;
    }
    globtrk[iglobtrk].psi = 
      globtrk[iglobtrk].psi * C_DEG_PER_RAD; 

    //  1/pt at start 
    globtrk[iglobtrk].invpt =  
      1./::sqrt(fptrack[itrk].p[0]*fptrack[itrk].p[0]
	      +fptrack[itrk].p[1]*fptrack[itrk].p[1]);

    //  tan(dip) = pz/pt at start
    globtrk[iglobtrk].tanl  = fptrack[itrk].p[2]  
      *  globtrk[iglobtrk].invpt;

    //  curvature 
    globtrk[iglobtrk].curvature =  fptrack[itrk].curvature;

    // This has to go in as soon as 
    // r0out, phi0out, z0out, psiout, tanlout, invptout 
    // in the dst_track table are needed.
    /*
    //  radius at end of track (cm) 
    globtrk[iglobtrk].r0out   = 
      ::sqrt(fptrack[itrk].l[0]*fptrack[itrk].l[0]
	   + fptrack[itrk].l[1]*fptrack[itrk].l[1]);

    //  azimuthal angle at end of track (deg)
    globtrk[iglobtrk].phi0out = 
      atan2(fptrack[itrk].l[1],fptrack[itrk].l[0])
      * C_DEG_PER_RAD;

    //  z-coordinate at end of track 
    globtrk[iglobtrk].z0out = fptrack[itrk].l[2];

    // For Kalman fitting 'inner' and 'outer' momenta differ.
    // For FTPC fitting they are the same,
    // so fill with the 'inner' values.

    //  momentum angle at end 
    globtrk[iglobtrk].psiout = 
      atan2(fptrack[itrk].p[1],fptrack[itrk].p[0]);
    if (globtrk[iglobtrk].psiout < 0.0) {
      globtrk[iglobtrk].psiout = 
	globtrk[iglobtrk].psiout + C_2PI;
    }
    globtrk[iglobtrk].psiout = 
      globtrk[iglobtrk].psiout * C_DEG_PER_RAD; 

    //  1/pt at end 
    globtrk[iglobtrk].invptout =  
      1./::sqrt(fptrack[itrk].p[0]*fptrack[itrk].p[0]
	      +fptrack[itrk].p[1]*fptrack[itrk].p[1]);

    //  tan(dip) = pz/pt at end
    globtrk[iglobtrk].tanlout  = fptrack[itrk].p[2]  
      *  globtrk[iglobtrk].invptout;
    */

    //  covariance matrix 
    //  (currently not set for FTPC) 
    globtrk[iglobtrk].covar[0] = 0;
    globtrk[iglobtrk].covar[1] = 0;
    globtrk[iglobtrk].covar[2] = 0;
    globtrk[iglobtrk].covar[3] = 0;
    globtrk[iglobtrk].covar[4] = 0;
    globtrk[iglobtrk].covar[5] = 0;
    globtrk[iglobtrk].covar[6] = 0;
    globtrk[iglobtrk].covar[7] = 0;
    globtrk[iglobtrk].covar[8] = 0;
    globtrk[iglobtrk].covar[9] = 0;
    globtrk[iglobtrk].covar[10] = 0;
    globtrk[iglobtrk].covar[11] = 0;
    globtrk[iglobtrk].covar[12] = 0;
    globtrk[iglobtrk].covar[13] = 0;
    globtrk[iglobtrk].covar[14] = 0;

    //  chi-square fit
    globtrk[iglobtrk].chisq[0]      = fptrack[itrk].chisq[0]
      / (globtrk[iglobtrk].n_fit_point - 3);
    globtrk[iglobtrk].chisq[1]      = fptrack[itrk].chisq[1]
      / (globtrk[iglobtrk].n_fit_point - 2);

    //  Locate last hit on current track
    for (ihit=MAXHITS-1; ihit>=0; ihit--) {
      if (fptrack[itrk].hitid[ihit] > -1) {
	//            hitid array filled by FORTRAN routine, must -1 for C routine
	iPoint =  fptrack[itrk].hitid[ihit] - 1;
	globtrk[iglobtrk].x_last[0]     = 
	  fppoint[iPoint].x;
	globtrk[iglobtrk].x_last[1]     =  
	  fppoint[iPoint].y;
	globtrk[iglobtrk].x_last[2]     = 
	  fppoint[iPoint].z;
	break;
      }
    }

    globtrk[iglobtrk].length  = fptrack[itrk].length;

    globtrk[iglobtrk].impact  = fptrack[itrk].impact;

    //  Maximum number of points 
    globtrk[iglobtrk].n_max_point  = fptrack[itrk].nmax;

    // bitmask quality information
    globtrk[iglobtrk].iflag = 
      700 + fptrack[itrk].flag;
    if (fabs((float) globtrk[iglobtrk].icharge) != 1. ) {
      globtrk[iglobtrk].iflag   =  
	-globtrk[iglobtrk].iflag + 20;
    }
    if (fabs((float) globtrk[iglobtrk].invpt) >= 999999.)  {
      globtrk[iglobtrk].iflag   = -799;
    }


    //  Fill dst_dedx table  

    dedx[idedx].id_track = globtrk[iglobtrk].id;
    dedx[idedx].det_id = globtrk[iglobtrk].det_id;

    if(fdepar->id_method == 0)
      dedx[idedx].method = kTruncatedMeanId;
    else if (fdepar->id_method == 1)
      dedx[idedx].method = kEnsembleTruncatedMeanId;
    else
      dedx[idedx].method = kUndefinedMethodId;

    dedx[idedx].ndedx = fptrack[itrk].ndedx;
    dedx[idedx].dedx[0] = fptrack[itrk].dedx;
    dedx[idedx].dedx[1] = 0;

  }    // End of processing current track

  dst_track->SetNRows(iglobtrk);
  dst_dedx->SetNRows(idedx);

  // Now save all hits

  Int_t ipnt = 0;
  Int_t det_id=0;

  const float FTPC_FAC = 2380.0; // Multiplication factor to achieve 4 micron accuracy
  const float FTPC_MIN = -270.0;   // Minimum FTPC z-coordinate
  const float FTPC_MAX =  270.0;   // Maximum FTPC z-coordinate


  const int two10 =    1024;    // 2**10
  const int two17 =  131072;    // 2**17
  const int two20 = 1048576;    // 2**20

  unsigned int ftpcx, ftpcy, ftpcz;
  unsigned int ftpcy10, ftpcy11;

  //  Loop over all hits

  for (iPoint=0; iPoint<nfppoint; iPoint++,ipnt++) {
    if (fppoint[iPoint].row >=101) {
      point[ipnt].id_track    = fppoint[iPoint].row/100;
      fppoint[iPoint].row = fppoint[iPoint].row%100;   
    }
    else {
      point[ipnt].id_track    = 0;
    }

    //                 Rows 1->10 FTPC West  det_id = kFtpcWestId
    if (fppoint[iPoint].row >= 1 && fppoint[iPoint].row <= 10) {
      det_id = kFtpcWestId;
    }
    //                 Rows 11->20 FTPC East  det_id=kFtpcEastId  
    else if (fppoint[iPoint].row >= 11 && fppoint[iPoint].row <=20 ) {
      det_id = kFtpcEastId;
    }
    else {
      gMessMgr->Message("", "I", "OST") <<"StFtpcGlobalMaker: fppoint["<<iPoint<<"].row  = "<<fppoint[iPoint].row<<" is out of range"<< endm;
    }
    //    hw_position  (32 bits)
    //            bits  0-3   det_id
    //            bits 4-10   FTPC pad plane (1-20)
    //            bits 11-20  Sector number within pad-plane (1-6)
    //            bits 21-24  number of pads in cluster
    //            bits 25-31  number of consecutive timebins in cluster
    point[ipnt].hw_position =
      (fppoint[iPoint].n_bins<<25)
      + (fppoint[iPoint].n_pads<<21)
      + (fppoint[iPoint].sector<<11)
      + (fppoint[iPoint].row<<4)
      + det_id;

    //         Fill space point position coordinates
    if (fppoint[iPoint].x > FTPC_MIN && fppoint[iPoint].x < FTPC_MAX){
      ftpcx = (int) (FTPC_FAC*(fppoint[iPoint].x + FTPC_MAX));
    }
    else {
      ftpcx = 0;
    }
    if (fppoint[iPoint].y > FTPC_MIN && fppoint[iPoint].y < FTPC_MAX){
      ftpcy = (int) (FTPC_FAC*(fppoint[iPoint].y + FTPC_MAX));
    }
    else {
      ftpcy = 0;
    }
    if (fppoint[iPoint].z > FTPC_MIN && fppoint[iPoint].z < FTPC_MAX){
      ftpcz = (int) (FTPC_FAC*(fppoint[iPoint].z + FTPC_MAX));
    }
    else {
      ftpcz = 0;
    }
    ftpcy10 = ftpcy/two10;
    ftpcy11 = ftpcy - two10*ftpcy10;
    point[ipnt].position[0] = ftpcx + (two20*ftpcy11);
    point[ipnt].position[1] = ftpcy10 + (two10*ftpcz);


    //         Fill space point position errors (0.0<= error < 8.0)
    if (fppoint[iPoint].x_err >= 0.0 && fppoint[iPoint].x_err < 8.0){
      ftpcx =  (long) (two17*fppoint[iPoint].x_err);
    }
    else {
      ftpcx = 0;
    }
    if (fppoint[iPoint].y_err >= 0.0 && fppoint[iPoint].y_err < 8.0){
      ftpcy = (long) (two17*fppoint[iPoint].y_err);
    }
    else {
      ftpcy = 0;
    }
    if (fppoint[iPoint].z_err >= 0.0 && fppoint[iPoint].z_err < 8.0){
      ftpcz =  (long) (two17*fppoint[iPoint].z_err);
    }
    else {
      ftpcz = 0;
    }
    ftpcy10 = ftpcy/two10;
    ftpcy11 = ftpcy - (two10*ftpcy10);
    point[ipnt].pos_err[0] = ftpcx + (two20*ftpcy11);
    point[ipnt].pos_err[1] = ftpcy10 + (two10*ftpcz);

    //        Fill charge and flags for cluster
    //                     bits 0-15    charge (sum of adc channels)
    //                     bits 16-31   flags  (see fcl_fppoint.idl)
    point[ipnt].charge  =
      (fppoint[iPoint].flags<<16)
      + fppoint[iPoint].charge;

  }  // end of loop over all hits

  dst_point->SetNRows(ipnt);

  return iMake;
}
//_____________________________________________________________________________
