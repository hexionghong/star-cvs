#include <stdio.h>
//#include <iostream.h>
//#include <strings.h>
//#include <sys/types.h>
//#include <stdlib.h>
#include "St_l3_Coordinate_Transformer.h"
#include "St_l3_Coordinates.h"

#define OFFLINE
#ifdef OFFLINE
#include "StDbUtilities/StCoordinates.hh"
#include "StTpcDb/StTpcDb.h"

ClassImp(St_l3_Coordinate_Transformer);
#endif

//______________________________
St_l3_Coordinate_Transformer::St_l3_Coordinate_Transformer()
{
    // initialize transformations
    // reset start values
    max_tb_inner =0;
    max_tb_outer =0;
    transformation_errors =0;
    
    //Use_transformation_provided_by_db() ;
    Set_parameters_by_hand() ;
    //Get_parameters_from_db() ;
    //Use_transformation_provided_by_db() ;
    //Print_parameters() ;   
}
//______________________________
St_l3_Coordinate_Transformer::~St_l3_Coordinate_Transformer()
{
   if (transformation_errors>0)
       {
	   cout << transformation_errors << " transformation errors occured.\n";
       }
}
//______________________________
void St_l3_Coordinate_Transformer::raw_to_global(const St_l3_ptrs_Coordinate &raw ,St_l3_xyz_Coordinate &global )
{
    St_l3_xyz_Coordinate local(0,0,0) ;
    raw_to_local( raw ,local ) ;
    local_to_global( raw ,local ,global ) ;
}
//______________________________
void St_l3_Coordinate_Transformer::raw_to_local(const St_l3_ptrs_Coordinate &raw ,St_l3_xyz_Coordinate &local )
{
    double  pitch = (raw.Getr()<14) ?    innerSectorPadPitch : outerSectorPadPitch;
    double  pads2move =  raw.Getp() - numberOfPadsAtRow[(int)raw.Getr()-1]/2;  
    local.Setx( pitch * (pads2move - 0.5) ) ;
    local.Sety( radialDistanceAtRow[(int)(raw.Getr())-1] ) ;
		
    // Timebucket_to_z different for inner and outer sector
    if( raw.Getr() <= 13 )
	{
	    local.Setz(drift_length_inner - raw.Gett() * lengthPerTb) ;
	    if (raw.Gett()>max_tb_inner) {transformation_errors++;}
	}
    else 
	{
	    local.Setz(drift_length_outer - raw.Gett() * lengthPerTb) ;
	    if (raw.Gett()>max_tb_outer) {transformation_errors++;}
	}
}
//______________________________
void St_l3_Coordinate_Transformer::local_to_global(const St_l3_ptrs_Coordinate &raw , const St_l3_xyz_Coordinate &local ,St_l3_xyz_Coordinate &global )
{
    int sector = (int) raw.Gets();
    global.Setx( SectorCos[sector-1] * local.Getx() + SectorSin[sector-1] * local.Gety() ) ;
    
    if ( sector <= 12 )
	{
	    global.Sety( -1.*SectorSin[sector-1] * local.Getx() + SectorCos[sector-1] * local.Gety()) ;
	    global.Setz( local.Getz() ) ;
	}
    else 
	{
	    global.Sety( -1.*SectorSin[(sector-12)-1] * local.Getx() + SectorCos[(sector-12)-1] * local.Gety()) ; 
	    global.Setz( -1 * local.Getz() ) ;
	}
}
//______________________________
void St_l3_Coordinate_Transformer::global_to_raw(const St_l3_xyz_Coordinate &global , St_l3_ptrs_Coordinate &raw )
{
    St_l3_xyz_Coordinate local(0,0,0) ;
    global_to_local( global, local, raw ) ;
    local_to_raw( global ,local ,raw ) ;
}
//______________________________
void St_l3_Coordinate_Transformer::global_to_local(const St_l3_xyz_Coordinate &global, St_l3_xyz_Coordinate &local ,St_l3_ptrs_Coordinate &raw) 
{
    // Get xyz right
    double y = global.Gety() ;
    double x = 0;
    if(global.Getz()>=0)
	{
	    x = global.Getx() ; 
	    local.Setz(global.Getz());
	}
    else
	{
	    x = -(global.Getx()) ; // ATTENTION must be mirrowed for sectors 13 - 24 !!!
	    local.Setz(-(global.Getz()));
	}

    // Prepare turn operation
    double pi = 3.14159265358979323846;
    double sec_border = tan(pi/12) ; // 15 degree
    double turn_angle = -pi/6 ;  // 30 degree
    double sin_turn_angle = sin(turn_angle);
    double cos_turn_angle = cos(turn_angle);
    double sector = 0 ; 
    
    if (y>=0 && fabs(x/y)<=sec_border)
	{
	    // We are already in sector 12 
	    sector = 12 ;
	}
    else
	{
	    // We have to turn system until we are in first sector
	    while( y<0 || (fabs(x/y)>sec_border))
		{
		    double xn = x*cos_turn_angle + y*sin_turn_angle ;
		    double yn = -x*sin_turn_angle + y*cos_turn_angle ;
		    x = xn ;
		    y = yn ;
		    sector++;
		}
	}

    // Set it
    local.Setx(x);
    local.Sety(y);
    
    if (global.Getz()<0)
      {
	raw.Sets(sector+12);
      }
    else
      {
	raw.Sets(sector);
      }

}
//______________________________
void St_l3_Coordinate_Transformer::local_to_raw(const St_l3_xyz_Coordinate &global ,const St_l3_xyz_Coordinate &local , St_l3_ptrs_Coordinate &raw ) 
{
  // first lets find the row
  double y = local.Gety() ;
  int row = 0;
  int row_index = 0 ;
  while( (fabs(radialDistanceAtRow[row_index]-y) > 0.5) && (row_index < 46) )
    {
      row_index++;
    }
  if (row_index==45 || y<59)
    {
      // no matching row found
      cout << "Alert row not found !" << endl;
      return ;
    }
  else 
    {
      // yes row found !
      row = row_index+1;
    }

  // then lets go for the pad
  double x = local.Getx();
  double  pitch = (row<=13) ?    innerSectorPadPitch : outerSectorPadPitch ;
  int  half_num_pads_this_row = numberOfPadsAtRow[row-1]/2 ;
  double pad = half_num_pads_this_row + x/pitch + 0.5 ;

  // finally lets get the bucket
  double bucket = 0;
  double z = local.Getz();
  if (row<=13)
    {
      bucket = ( drift_length_inner - z )/lengthPerTb ;
      if (z>drift_length_inner) {transformation_errors++;}
    }
  else 
    {
      bucket = ( drift_length_outer - z )/lengthPerTb ;
      if (z>drift_length_outer) {transformation_errors++;}
    }
  
  // fill it
  raw.Setp(pad);
  raw.Sett(bucket);
  raw.Setr(row);
}
//______________________________
void St_l3_Coordinate_Transformer::Set_parameters_by_hand(const double mlengthPerTb, const double mdrift_length_inner, const double mdrift_length_outer)
{
    lengthPerTb = mlengthPerTb ;
    drift_length_inner = mdrift_length_inner ;
    drift_length_outer = mdrift_length_outer ;
    
    // set max timebucket
    max_tb_inner = drift_length_inner/lengthPerTb;
    max_tb_outer = drift_length_outer/lengthPerTb;
}
//______________________________
void St_l3_Coordinate_Transformer::Set_parameters_by_hand()
{
  // Set the parameters straight forward
  lengthPerTb =  0.561;
  drift_length_inner =  200.668 ;
  drift_length_outer = 201.138  ;
    
  // set max timebucket
  max_tb_inner = drift_length_inner/lengthPerTb;
  max_tb_outer = drift_length_outer/lengthPerTb;
  //cout << "Constants for transformation set by hand." << endl;
}
//______________________________
void St_l3_Coordinate_Transformer::Use_transformation_provided_by_db()
{
#ifdef OFFLINE
  // perform official transform and recalulate parameters
  // the official transform doesn't take doubles for pad and time
  // that's why we must extract the parameters ...
  StTpcPadCoordinate padco;
  StGlobalCoordinate glo;
  StTpcCoordinateTransform tra(gStTpcDb);

  // shaping time
  double tau3 = 3 * (gStTpcDb->Electronics()->tau() * 1e-09) * (gStTpcDb->DriftVelocity()) ;

  // inner row (5) in sector 1 
  padco.setSector(1);
  padco.setRow(5);
  padco.setPad(1);
  padco.setTimeBucket(100);
   
  tra(padco,glo); 
  double z_100 = glo.position().z();
  padco.setTimeBucket(0);
  tra(padco,glo);
  double z_0 = glo.position().z();
  double lengthPerTb_inner = fabs((z_0-z_100)/100) ;
  drift_length_inner = fabs(glo.position().z()+tau3);
    
  //cout << "sector : " << padco.sector() << "\t"; 
  //cout << "length per tb inner = " <<  lengthPerTb_inner << "\t";
  //cout << "innerdrift : " <<drift_length_inner  << endl;
    
  // outer row (30) in sector 1
  padco.setSector(1);
  padco.setRow(30);
  padco.setPad(1);
  padco.setTimeBucket(100);
  tra(padco,glo);
  z_100 = glo.position().z();
  padco.setTimeBucket(0);
  tra(padco,glo);
  z_0 = glo.position().z();
  double lengthPerTb_outer = fabs((z_0-z_100)/100) ;
  drift_length_outer = fabs(glo.position().z()+tau3) ;

  //cout << "sector : " << padco.sector() << "\t"; 
  //cout << "length per tb outer = " << lengthPerTb_outer << "\t";
  //cout << "outerdrift : " << drift_length_outer  << endl;

  
  if (lengthPerTb_outer == lengthPerTb_inner )
    {
      lengthPerTb = lengthPerTb_outer ;
    }
  else
    {
      cerr << "Different driftvelocities in TPC observed -> Set to 0. " << endl;
      lengthPerTb = 0;
    }
  cout << "Constants set by using official transformation." << endl;

  // set max timebucket
  max_tb_inner = drift_length_inner/lengthPerTb;
  max_tb_outer = drift_length_outer/lengthPerTb;
#else
  cout << "This is not functional online.\n" ;
#endif 

}
//______________________________
void St_l3_Coordinate_Transformer::Get_parameters_from_db()
{
#ifdef OFFLINE
  // connect to database
  double driftvelocity = gStTpcDb->DriftVelocity();
  double inner_effective_driftlength = gStTpcDb->Dimensions()->innerEffectiveDriftDistance();
  double outer_effective_driftlength = gStTpcDb->Dimensions()->outerEffectiveDriftDistance();
  double gatingrid = gStTpcDb->Dimensions()->gatingGridZ();
  double t0pad = gStTpcDb->T0(1)->getT0(1,1);
  double zinneroffset = gStTpcDb->Dimensions()->zInnerOffset();
  double zouteroffset = gStTpcDb->Dimensions()->zOuterOffset();
  double frequency =  gStTpcDb->Electronics()->samplingFrequency();
  double tzero = gStTpcDb->Electronics()->tZero();
  double tau = gStTpcDb->Electronics()->tau();
  double shapingtime = gStTpcDb->Electronics()->shapingTime();
   
  if (0)
      {
	  cout << "Fresh from the db we got : \n" ;
	  cout << "The driftvelocity          : " << driftvelocity << endl;
	  cout << "The effective innerdrift  (not used) : " << inner_effective_driftlength << endl ;
	  cout << "The effective outerdrift  (not used) : " << outer_effective_driftlength << endl ;
	  cout << "Innerzoffset    : " << zinneroffset << endl ; 
	  cout << "Outerzoffset    : " << zouteroffset << endl ; 
	  cout << "Gatinggrid      : " << gatingrid << endl ;
	  cout << "t0pad           : " << t0pad  << endl ;
	  cout << "The tzero                  : " << tzero << endl;
	  cout << "Frequency ist set to       : " << frequency << endl;
	  cout << "tau                        : " << tau << endl ;  
	  cout << "shapingtime  (not used)    : " << shapingtime << endl ;
      }

  // length per timebucket = 1 / frequency * driftvelocity
  lengthPerTb = 1 / frequency / (1E6) * driftvelocity ; // cm per timebucket

  // driftlength = gatingrid - dv * t0 (this is a general t0) + dv/frequency * 0.5 (half a timebucket to get in the middle of a tb)
  // + zoffset (differnt offset of inner and outer padrows) + t0 (per individual pad = 0 at 05/05/2000) 
  // + 3*tau*driftvelocity ( = shaping time (this depends on the used algorithm used ) this holds for weigthed mean see STARNOT
  //
  drift_length_inner = gatingrid-driftvelocity*1e-6*(-tzero+0.5/frequency)+zinneroffset+t0pad+3*tau*1e-09*driftvelocity;
  drift_length_outer = gatingrid-driftvelocity*1e-6*(-tzero+0.5/frequency)+zouteroffset+t0pad+3*tau*1e-09*driftvelocity;
 
  
  if (0)
      {
	  cout << "inner drift length : " << drift_length_inner << endl;
	  cout << "outer drift length : " << drift_length_outer << endl;
	  cout << "lengthPerTb        : " << lengthPerTb << endl;
      }
  
  // set max timebucket
  max_tb_inner = drift_length_inner/lengthPerTb;
  max_tb_outer = drift_length_outer/lengthPerTb;

#else
  cout << "This is not functional online.\n" ;
#endif 

}
//______________________________
void St_l3_Coordinate_Transformer::Print_parameters()
{
    cout << "Used parameters : " << endl ;
    cout << "Length per tb  : " << lengthPerTb << endl ;
    cout << "drift_length_inner: " << drift_length_inner << endl ;
    cout << "drift_length_outer: " << drift_length_outer << endl ;
    cout << "max_tb_inner: " << max_tb_inner << endl ;
    cout << "max_tb_outer: " << max_tb_outer  << endl ;
    
}

/////////
// init our statics
/////////
double St_l3_Coordinate_Transformer::outerSectorPadPitch = 0.67;     // cm
double St_l3_Coordinate_Transformer::innerSectorPadPitch = 0.335;    // cm

// number of pads in padrow
int St_l3_Coordinate_Transformer::numberOfPadsAtRow[45] = {
    88,96,104,112,118,126,134,142,150,158,166,174,182,
    98,100,102,104,106,106,108,110,112,112,114,116,
    118,120,122,122,124,126,128,128,130,132,134,136,
    138,138,140,142,144,144,144,144 
};
// radial distance (center pad) from detector center in cm
double St_l3_Coordinate_Transformer::radialDistanceAtRow[45] = {
    60.0, 64.8, 69.6, 74.4, 79.2, 84.0, 88.8, 93.60,     //   7 * 4.80 cm spacing
    98.8, 104., 109.20, 114.4, 119.6,                    //   5 * 5.20 cm spacing
    127.195, 129.195, 131.195, 133.195, 135.195,         //  32 * 2.00 cm spacing
    137.195, 139.195, 141.195, 143.195, 145.195,
    147.195, 149.195, 151.195, 153.195, 155.195,
    157.195, 159.195, 161.195, 163.195, 165.195,
    167.195, 169.195, 171.195, 173.195, 175.195,
    177.195, 179.195, 181.195, 183.195, 185.195,
    187.195, 189.195
};
// sector-rotation factors: 30 degree steps
double St_l3_Coordinate_Transformer::SectorSin[24] = {
     0.5,  0.866025404,
     1.0,  0.866025404,
     0.5,  0.,
    -0.5, -0.866025404,
    -1.0, -0.866025404,
    -0.5,  0.,
    -0.5, -0.866025404,
    -1.0, -0.866025404,
    -0.5,  0.,
     0.5,  0.866025404,
     1.0,  0.866025404,
     0.5,  0.,
};
// sector-rotation factors: 30 degree steps
double St_l3_Coordinate_Transformer::SectorCos[24] = {
    0.866025404,  0.5,
     0.,          -0.5,
    -0.866025404, -1.0,
    -0.866025404, -0.5,
     0.,           0.5,
     0.866025404,  1.0,
    -0.866025404, -0.5,
     0.,           0.5,
     0.866025404,  1.0,
     0.866025404,  0.5,
     0.,          -0.5,
    -0.866025404, -1.0,
};
