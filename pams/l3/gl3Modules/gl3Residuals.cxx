//:>------------------------------------------------------------------
//: FILE:       gl3Residuals.cxx
//: HISTORY:
//:              4may2000 version 1.00
//:<------------------------------------------------------------------
#include "gl3Residuals.h"
#define BFACT 0.0029980

//####################################################################
//
//####################################################################
int gl3Residuals::init ( l3List* histos ) {
  char hid[50];
  char title[100];

  strcpy(hid,"residx");
  strcpy(title,"Residuals in x");
  Resx=new gl3Histo(hid,title,100,-2.,2.);
  histos->append((void*)Resx);
  strcpy(hid,"residz");
  strcpy(title,"Residuals in z");
  Resz=new gl3Histo(hid,title,100,-2.,2.);
  histos->append((void*)Resz);
  
  return 0 ;
}
//####################################################################
//
//####################################################################
int gl3Residuals::process ( gl3Event* event ) {
  
  // printf("In residuals\n");

  St_l3_Coordinate_Transformer transformer ;
  St_l3_xyz_Coordinate globalClusterXYZ(0,0,0) ;
  St_l3_xyz_Coordinate localClusterXYZ(0,0,0) ;
  St_l3_xyz_Coordinate globalTrackXYZ(0,0,0) ;
  St_l3_xyz_Coordinate localTrackXYZ(0,0,0) ;
  St_l3_ptrs_Coordinate rawHit(0,0,0,0) ;

  Ftf3DHit cross;
//float MaxDipAngle=10;
  
  for(int i=0; i<event->getNTracks(); i++)
    {
      
      gl3Track *ctrack=(gl3Track*)event->getTrack(i);
      
      if(ctrack->nHits<30) continue; //reading cosmics
      
      //calculate dipangle of this track
      //float dipangle=atan(ctrack->tanl);
      
      //calculate circle center and radius
      double x0=ctrack->r0*cos(ctrack->phi0);
      double y0=ctrack->r0*sin(ctrack->phi0);
      double trackPhi0=ctrack->psi+ctrack->q*0.5*M_PI/abs(ctrack->q);
      double rcoc=ctrack->pt/(BFACT*ctrack->bField);
      double xcoc=x0 - (rcoc*cos(trackPhi0));
      double ycoc=y0 - (rcoc*sin(trackPhi0));
     
      
      for(ctrack->startLoop(); ctrack->done(); ctrack->nextHit())
	{
	  gl3Hit *chit=(gl3Hit*)(ctrack->currentHit);
	  
	  int sector = chit->getRowSector()/100;
	  int row=chit->getRowSector()%100;
	  
	  double xc=chit->getX();
	  double yc=chit->getY();
	  double zc=chit->getZ();
	  
	  if(transformer.GetSectorCos(sector-1)!=0)//avoid dividing by zero
	    {
	      //finding line parameters of padrow plane:
	      double a=-1.*transformer.GetSectorSin(sector-1)/
                           transformer.GetSectorCos(sector-1);
	      if(sector>12) a=-a;
	      double b=transformer.GetRadialDistanceAtRow(row-1)/transformer.GetSectorCos(sector-1);
	      
	      //find crossing point between track and this line
	      if(ctrack->intersectorZLine(a,b,cross)!=0)
		{
		  printf("gl3Residuals: Track does not cross line in sector %d padrow %d\n",sector,row);
		  continue;
		}
	      
	    }
	  else
	    {//sectors 3,9,15,21
	    
	      double xHit;
	      if(sector==9 || sector==15) xHit=-transformer.GetRadialDistanceAtRow(row-1);
	      else xHit=transformer.GetRadialDistanceAtRow(row-1);
	      
	      double f1=(xHit-xcoc)*(xHit-xcoc);
	      double r2=rcoc*rcoc;
	      if(f1>r2)
		{
		  printf("gl3Residuals: Track does not cross line in sector %d padrow %d\n",sector,row);
		  continue;
		}
	      
	      double sf2=sqrt(r2-f1);
	      double y1=ycoc+sf2;
	      double y2=ycoc-sf2;
	      double yHit=y1;
	      if(fabs(y2)<fabs(y1)) yHit=y2;
	      
	      //Get z coordinate:
	      double angle  = atan2 ( (yHit-ycoc), (xHit-xcoc) ) ;
	      if ( angle < 0. ) angle = angle + 2.0 * M_PI ;
	      
	      double dangle = angle  - trackPhi0  ;
	      dangle = fmod ( dangle, 2.0 * M_PI ) ;
	      if ( (ctrack->q * dangle) > 0 ) dangle = dangle - ctrack->q * 2. * M_PI  ;
	      
	      double stot   = fabs(dangle) * rcoc ;
	      double zHit   = ctrack->z0 + stot * ctrack->tanl;
	      
	      cross.set(xHit,yHit,zHit);
	    }
	  float xt=cross.x;
	  float yt=cross.y;
	  float zt=cross.z;

	  //Rotate coordinates to local sector coordinates
          
          globalClusterXYZ.Setxyz(xc,yc,zc);
          transformer.global_to_local(globalClusterXYZ,localClusterXYZ,rawHit);
// globalToLocal(sector,row,xc,yc,zc,xcl,ycl,zcl);
          
          globalTrackXYZ.Setxyz(xt,yt,zt);
          transformer.global_to_local(globalTrackXYZ,localTrackXYZ,rawHit);
// globalToLocal(sector,row,xt,yt,zt,xtl,ytl,ztl);
	  
	  //calculate beta (crossing angle with padrow)
	  double xcocg=xcoc;
	  if (sector>12) 
	    xcocg = -xcoc;
	  
	  double ycocl = transformer.GetSectorSin(sector-1) * xcocg + transformer.GetSectorCos(sector-1) * ycoc;
	  //double beta=asin(localTrackXYZ.Gety()/rcoc-ycocl/rcoc);

	  //if(fabs(beta*todeg)>5) continue; //only consider tracks normal to padrow

	  //fill residuals in histogram
	  double resx=localTrackXYZ.Getx()-localClusterXYZ.Getx();
	  double resz=localTrackXYZ.Getz()-localClusterXYZ.Getz();
	  
	  Resx->Fill(resx,1.);
	  Resz->Fill(resz,1.);
	 	  
	}//loop over hits
      
      
    }//loop over tracks
  //printf("End of residuals\n");
  return 1 ;
}
