// $Id: StFtpcVertex.cc,v 1.8 2002/04/05 16:51:16 oldi Exp $
// $Log: StFtpcVertex.cc,v $
// Revision 1.8  2002/04/05 16:51:16  oldi
// Cleanup of MomentumFit (StFtpcMomentumFit is now part of StFtpcTrack).
// Each Track inherits from StHelix, now.
// Therefore it is possible to calculate, now:
//  - residuals
//  - vertex estimations obtained by back extrapolations of FTPC tracks
// Chi2 was fixed.
// Many additional minor (and major) changes.
//
// Revision 1.7  2001/07/12 13:05:02  oldi
// QA histogram of FTPC vertex estimation is generated.
// FTPC vertex estimation is stored as pre vertex (id = 301) in any case, now.
//
// Revision 1.6  2001/01/25 15:22:39  oldi
// Review of the complete code.
// Fix of several bugs which caused memory leaks:
//  - Tracks were not allocated properly.
//  - Tracks (especially split tracks) were not deleted properly.
//  - TClonesArray seems to have a problem (it could be that I used it in a
//    wrong way). I changed all occurences to TObjArray which makes the
//    program slightly slower but much more save (in terms of memory usage).
// Speed up of HandleSplitTracks() which is now 12.5 times faster than before.
// Cleanup.
//
// Revision 1.5  2000/11/28 14:00:53  hummler
// protect vertex finder against nan
//
// Revision 1.4  2000/11/10 18:39:25  oldi
// Changes due to replacement of StThreeVector by TVector3.
// New constructor added to find the main vertex with given point array.
//
// Revision 1.3  2000/06/13 14:35:00  oldi
// Changed cout to gMessMgr->Message().
//
// Revision 1.2  2000/05/15 14:28:15  oldi
// problem of preVertex solved: if no main vertex is found (z = NaN) StFtpcTrackMaker stops with kStWarn,
// refitting procedure completed and included in StFtpcTrackMaker (commented),
// new constructor of StFtpcVertex due to refitting procedure,
// minor cosmetic changes
//
// Revision 1.1  2000/05/10 13:39:34  oldi
// Initial version of StFtpcTrackMaker
//

//----------Author:        Holm G. H&uuml;ummler, Markus D. Oldenburg
//----------Last Modified: 24.07.2000
//----------Copyright:     &copy MDO Production 1999

#include "StMessMgr.h"
#include "StFtpcVertex.hh"
#include "StFtpcPoint.hh"

#include "St_DataSet.h"
#include "St_DataSetIter.h"
#include "tables/St_g2t_vertex_Table.h"
#include "tables/St_dst_vertex_Table.h"
#include "tables/St_fcl_fppoint_Table.h"

#include "TF1.h"

////////////////////////////////////////////////////////////////////////////
//                                                                        //
// StFtpcVertex class - representation of the main vertex for the FTPC.   //
//                                                                        //
// This class contains the coordinates of the main vertex plus the usual  //
// getters and setter. It is just a wrapper of the Staf tables.           //
//                                                                        //
////////////////////////////////////////////////////////////////////////////

ClassImp(StFtpcVertex)


StFtpcVertex::StFtpcVertex()
{
  // Default constructor.

  SetX(0.);
  SetY(0.);
  SetZ(0.);

  SetXerr(0.);
  SetYerr(0.);
  SetZerr(0.);
  
  return;
}


StFtpcVertex::StFtpcVertex(fcl_fppoint_st *thisFppoint, Int_t numFppoints, TH1F *vtx_pos)
{
  // constructor with ftpc points - fits vertex from points

  // constants, to be moved to parameter database
#define HISTOBINS 300
#define HISTOMIN -75.0
#define HISTOMAX 75.0

  Float_t *rmap = new Float_t[20*6*numFppoints];
  Float_t *zmap = new Float_t[20];
  Int_t *mapMax = new Int_t[20*6];
  Int_t *myhist = new Int_t[HISTOBINS];
  Float_t hratio=HISTOBINS/(HISTOMAX-HISTOMIN);
  
  for(Int_t iii=0; iii<HISTOBINS; iii++) {
    myhist[iii]=0;
  }
  
  for(Int_t ii=0; ii<120; ii++) mapMax[ii]=0;
  
  for(Int_t i=0; i<numFppoints;i++) {
    rmap[(thisFppoint[i].row-1)+20*(thisFppoint[i].sector-1)+120*mapMax[(thisFppoint[i].row-1)+20*(thisFppoint[i].sector-1)]]=sqrt(thisFppoint[i].x*thisFppoint[i].x+thisFppoint[i].y*thisFppoint[i].y);
    zmap[thisFppoint[i].row-1]=thisFppoint[i].z;
    mapMax[(thisFppoint[i].row-1)+20*(thisFppoint[i].sector-1)]++;
  }

  for(Int_t secI=0; secI<6; secI++) {
    
    for(Int_t rowOut=0; rowOut<19; rowOut++) {

      for(Int_t rowIn=rowOut+1; rowIn<20; rowIn++) {
	
	if(rowIn<10 || rowOut>=10) {
	  
	  for(Int_t iOut=0; iOut<mapMax[rowOut+20*secI]; iOut++) {
	    Float_t ri=rmap[rowOut+20*secI+120*iOut];	    

	    for(Int_t iIn=0; iIn<mapMax[(rowIn)+20*secI]; iIn++) {
	      Float_t rj=rmap[rowIn+20*secI+120*iIn];
			  
	      if(rj>ri) {
		Float_t intersect=(rj*zmap[rowOut]-ri*zmap[rowIn])/(rj-ri);		
		
		if (vtx_pos) {
		  vtx_pos->Fill(intersect);
		}

		if(intersect>HISTOMIN && intersect<HISTOMAX) {
		  myhist[int((intersect-HISTOMIN)*hratio)]++;
		}
	      }
	    }
	  }
	}
      }
    }
  }

  Int_t maxBin=HISTOBINS/2, maxHeight=0;
  
  Float_t vertex = 0.;
  Float_t sigma = 0.;

  for(Int_t hindex=1; hindex<HISTOBINS-1; hindex++) {
    
    if(myhist[hindex]>maxHeight && myhist[hindex]>=myhist[hindex-1] && myhist[hindex]>=myhist[hindex+1]) {
      maxBin=hindex;
      maxHeight=myhist[hindex];
    }  
  }

  // check if Gaussfit will fail
  if((myhist[maxBin] == 0) 
     || (myhist[maxBin+1] == 0) 
     || (myhist[maxBin-1] == 0) 
     || (myhist[maxBin] <= myhist[maxBin+1]) 
     || (myhist[maxBin] <= myhist[maxBin-1])) {
    
    // use weighted mean instead 
    vertex=(myhist[maxBin]*((maxBin+0.5)/hratio+HISTOMIN)
	    + myhist[maxBin-1]*((maxBin-0.5)/hratio+HISTOMIN)
	    + myhist[maxBin+1]*((maxBin+1.5)/hratio+HISTOMIN))
      / (myhist[maxBin]+myhist[maxBin-1]+myhist[maxBin+1]);
  }

  else {
      
    // do gaussfit 
    sigma = sqrt (1 / ((2 * log(myhist[maxBin])) -
		       (log(myhist[maxBin+1]) + 
			log(myhist[maxBin-1]))));
    vertex =  ((maxBin+0.5)/hratio+HISTOMIN) + 
      sigma*sigma/(hratio*hratio) * (log(myhist[maxBin+1]) - 
				     log(myhist[maxBin-1]));
  } 
		  
  delete[] myhist;
  delete[] mapMax;
  delete[] zmap;
  delete[] rmap;
  
  SetX((Double_t) 0);
  SetY((Double_t) 0);
  SetXerr(0.);
  SetYerr(0.);
  if(vertex*0 != 0)
    {
      cerr << "vertex not found, setting to 0!" << endl;
      vertex = 0;
      sigma = 0.;
    }
  SetZ((Double_t) vertex);
  SetZerr((Double_t) sigma);
}


StFtpcVertex::StFtpcVertex(TObjArray *hits, TH1F *vtx_pos)
{
  // Constructor with TObjArray of ftpc points - fits vertex from points

  // constants, to be moved to parameter database
#define HISTOBINS 300
#define HISTOMIN -75.0
#define HISTOMAX 75.0

  Int_t numFppoints = hits->GetEntriesFast();

  Float_t *rmap = new Float_t[20*6*numFppoints];
  Float_t *zmap = new Float_t[20];
  Int_t *mapMax = new Int_t[20*6];
  Int_t *myhist = new Int_t[HISTOBINS];
  Float_t hratio=HISTOBINS/(HISTOMAX-HISTOMIN);
  
  for(Int_t iii=0; iii<HISTOBINS; iii++) {
    myhist[iii]=0;
  }
  
  for(Int_t ii=0; ii<120; ii++) mapMax[ii]=0;
  
  for(Int_t i=0; i<numFppoints;i++) {

    StFtpcPoint *thispoint = (StFtpcPoint *)hits->At(i);

    rmap[(thispoint->GetPadRow()-1)+20*(thispoint->GetSector()-1)+120*mapMax[(thispoint->GetPadRow()-1)+20*(thispoint->GetSector()-1)]]=sqrt(thispoint->GetX()*thispoint->GetX()+thispoint->GetY()*thispoint->GetY());
    zmap[thispoint->GetPadRow()-1]=thispoint->GetZ();
    mapMax[(thispoint->GetPadRow()-1)+20*(thispoint->GetSector()-1)]++;
  }

  for(Int_t secI=0; secI<6; secI++) {
    
    for(Int_t rowOut=0; rowOut<19; rowOut++) {

      for(Int_t rowIn=rowOut+1; rowIn<20; rowIn++) {
	
	if(rowIn<10 || rowOut>=10) {
	  
	  for(Int_t iOut=0; iOut<mapMax[rowOut+20*secI]; iOut++) {
	    Float_t ri=rmap[rowOut+20*secI+120*iOut];	    

	    for(Int_t iIn=0; iIn<mapMax[(rowIn)+20*secI]; iIn++) {
	      Float_t rj=rmap[rowIn+20*secI+120*iIn];
			  
	      if(rj>ri) {
		Float_t intersect=(rj*zmap[rowOut]-ri*zmap[rowIn])/(rj-ri);
		
		if (vtx_pos) {
		  vtx_pos->Fill(intersect);
		}

		if(intersect>HISTOMIN && intersect<HISTOMAX) {
		  myhist[int((intersect-HISTOMIN)*hratio)]++;
		}
	      }
	    }
	  }
	}
      }
    }
  }

  Int_t maxBin=HISTOBINS/2, maxHeight=0;
  
  Float_t vertex = 0.;
  Float_t sigma = 0.;

  for(Int_t hindex=1; hindex<HISTOBINS-1; hindex++) {
    
    if(myhist[hindex]>maxHeight && myhist[hindex]>=myhist[hindex-1] && myhist[hindex]>=myhist[hindex+1]) {
      maxBin=hindex;
      maxHeight=myhist[hindex];
    }  
  }

  // check if Gaussfit will fail
  if((myhist[maxBin] == 0) 
     || (myhist[maxBin+1] == 0) 
     || (myhist[maxBin-1] == 0) 
     || (myhist[maxBin] <= myhist[maxBin+1]) 
     || (myhist[maxBin] <= myhist[maxBin-1])) {
    
    // use weighted mean instead 
    vertex=(myhist[maxBin]*((maxBin+0.5)/hratio+HISTOMIN)
	    + myhist[maxBin-1]*((maxBin-0.5)/hratio+HISTOMIN)
	    + myhist[maxBin+1]*((maxBin+1.5)/hratio+HISTOMIN))
      / (myhist[maxBin]+myhist[maxBin-1]+myhist[maxBin+1]);
  }

  else {
      
    // do gaussfit 
    sigma = sqrt (1 / ((2 * log(myhist[maxBin])) -
		       (log(myhist[maxBin+1]) + 
			log(myhist[maxBin-1]))));
    vertex =  ((maxBin+0.5)/hratio+HISTOMIN) + 
      sigma*sigma/(hratio*hratio) * (log(myhist[maxBin+1]) - 
				     log(myhist[maxBin-1]));
  } 
		  
  delete[] myhist;
  delete[] mapMax;
  delete[] zmap;
  delete[] rmap;
  
  SetX(0.);
  SetY(0.);
  SetXerr(0.);
  SetYerr(0.);
  if(vertex*0 != 0)
    {
      cerr << "vertex not found, setting to 0!" << endl;
      vertex = 0.;
      sigma = 0.;
    }
  SetZ((Double_t) vertex);
  SetZerr((Double_t) sigma);
}


StFtpcVertex::StFtpcVertex(St_DataSet *const geant)
{
  // Obsolete constructor taking vertex from geant.

  if (geant) {
    St_DataSetIter geantI(geant);
    St_g2t_vertex *g2t_vertex = (St_g2t_vertex *) geantI.Find("g2t_vertex");
    
    if (g2t_vertex) {
      g2t_vertex_st   *vertex = g2t_vertex->GetTable();
      SetX((Double_t) vertex->ge_x[0]);
      SetY((Double_t) vertex->ge_x[1]);
      SetZ((Double_t) vertex->ge_x[2]);
      gMessMgr->Message("Using primary vertex coordinates (Geant): ", "I", "OST");
    }
    
    else {
      Double_t dummy = 0.0;
      SetX(dummy);
      SetY(dummy);
      SetZ(dummy);
      gMessMgr->Message("Using primary vertex coordinates (Default): ", "I", "OST");
    }

    SetXerr(0.);
    SetYerr(0.);
    SetZerr(0.);  
  }
}


StFtpcVertex::StFtpcVertex(dst_vertex_st *vertex)
{
  // constructor from Doubles
  
  SetX(vertex->x);
  SetY(vertex->y);
  SetZ(vertex->z);
  SetXerr(TMath::Sqrt(vertex->covar[0]));
  SetYerr(TMath::Sqrt(vertex->covar[2]));
  SetZerr(TMath::Sqrt(vertex->covar[5]));  
}  


StFtpcVertex::StFtpcVertex(TObjArray *tracks, StFtpcVertex *vertex, Char_t west)
{
  // constructor from track array

  TH1F x_hist("x_hist", "x position of estimated vertex", 200, -10., 10.);
  TH1F y_hist("y_hist", "y position of estimated vertex", 200, -10., 10.);
  TH1F z_hist("z_hist", "z position of estimated vertex", 200, -75., 75.);

  x_hist.Clear();
  y_hist.Clear();
  z_hist.Clear();

  TF1 gauss_x("gauss_x", "gaus", -10., 10.);
  TF1 gauss_y("gauss_y", "gaus", -10., 10.);
  TF1 gauss_z("gauss_z", "gaus", -75., 75.);
  
  StFtpcVertex v;

  if (vertex == 0) {
    // set nominal vertex to 0, 0, 0 if no nominal vertex is given
    v = StFtpcVertex(0., 0., 0., 0., 0., 0.);
  }

  else {
    v = *vertex;
  }

  for (Int_t i = 0; i < tracks->GetEntriesFast(); i++) {
    
    StFtpcTrack *track = (StFtpcTrack*)tracks->At(i);

    if (track->GetHemisphere() == west) {
      z_hist.Fill(track->z(track->pathLength(v.GetX(), v.GetY())));
    }
  }

  // fit only 20 cm in both directions of maximum
  z_hist.Fit(&gauss_z, "QN", "", z_hist.GetXaxis()->GetBinCenter(z_hist.GetMaximumBin())-20,
	     z_hist.GetXaxis()->GetBinCenter(z_hist.GetMaximumBin())+20);

  SetZ(gauss_z.GetParameter(1));
  SetZerr(gauss_z.GetParameter(2));

  for (Int_t i = 0; i < tracks->GetEntriesFast(); i++) {
    
    StFtpcTrack *track = (StFtpcTrack*)tracks->At(i);
    
    if (track->GetHemisphere() == west) {
      StThreeVector<Double_t> rv(0, 0, GetZ());
      StThreeVector<Double_t> nv(0,0,1);
      Double_t pl = track->pathLength(rv, nv);
      x_hist.Fill(track->x(pl));
      y_hist.Fill(track->y(pl));
    }
  }

  // fit only 3 cm in both directions of maximum
  x_hist.Fit(&gauss_x, "QN", "", x_hist.GetXaxis()->GetBinCenter(x_hist.GetMaximumBin())-3,
	     x_hist.GetXaxis()->GetBinCenter(x_hist.GetMaximumBin())+3);
  SetX(gauss_x.GetParameter(1));
  SetXerr(gauss_x.GetParameter(2));

  // fit only 3 cm in both directions of maximum
  y_hist.Fit(&gauss_y, "QN", "", y_hist.GetXaxis()->GetBinCenter(y_hist.GetMaximumBin())-3,
	     y_hist.GetXaxis()->GetBinCenter(y_hist.GetMaximumBin())+3);
  SetY(gauss_y.GetParameter(1));
  SetYerr(gauss_y.GetParameter(2));
}


StFtpcVertex::StFtpcVertex(Double_t pos[6])
{
  // constructor from Doubles
  
  SetX((Double_t) pos[0]);
  SetY((Double_t) pos[1]);
  SetZ((Double_t) pos[2]);
  SetXerr((Double_t) pos[3]);
  SetYerr((Double_t) pos[4]);
  SetZerr((Double_t) pos[5]);  
}  


StFtpcVertex::StFtpcVertex(Double_t pos[3], Double_t err[3])
{
  // constructor from Doubles with errors
  
  SetX((Double_t) pos[0]);
  SetY((Double_t) pos[1]);
  SetZ((Double_t) pos[2]);
  SetXerr((Double_t) err[0]);
  SetYerr((Double_t) err[1]);
  SetZerr((Double_t) err[2]);
}  


StFtpcVertex::StFtpcVertex(Double_t x, Double_t y, Double_t z, Double_t x_err, Double_t y_err, Double_t z_err)
{
  // constructor from Doubles with errors
  
  SetX(x);
  SetY(y);
  SetZ(z);
  SetXerr(x_err);
  SetYerr(y_err);
  SetZerr(z_err);
}  


StFtpcVertex::~StFtpcVertex() 
{
  // Destructor.
  // Does nothing except destruct.
}


StFtpcVertex::StFtpcVertex(const StFtpcVertex &vertex)
{
  // Copy constructor.

  SetX(vertex.GetX());
  SetY(vertex.GetY());
  SetZ(vertex.GetZ());
  SetXerr(vertex.GetXerr());
  SetYerr(vertex.GetYerr());
  SetZerr(vertex.GetZerr());
}


StFtpcVertex& StFtpcVertex::operator=(const StFtpcVertex &vertex)
{
  // Assigment operator.

  if (this != &vertex) {  // beware of selfd assignment: vertex = vertex
    SetX(vertex.GetX());
    SetY(vertex.GetY());
    SetZ(vertex.GetZ());
    SetXerr(vertex.GetXerr());
    SetYerr(vertex.GetYerr());
    SetZerr(vertex.GetZerr());
  }

  return *this;
}
