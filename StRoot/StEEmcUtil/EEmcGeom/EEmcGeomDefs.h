// Hey Emacs this is really -*-c++-*- ! 
// \author Piotr A. Zolnierczuk    Aug 26, 2002
#ifndef EEmcGeom_EEmcDefs_h
#define EEmcGeom_EEmcDefs_h
/*********************************************************************
 * $Id: EEmcGeomDefs.h,v 1.4 2003/04/04 15:35:47 wzhang Exp $
 *********************************************************************
 * Descripion:
 * STAR Endcap Electromagnetic Calorimeter Definitions (temp file)
 *********************************************************************
 * $Log: EEmcGeomDefs.h,v $
 * Revision 1.4  2003/04/04 15:35:47  wzhang
 * added SMD constants
 *
 * Revision 1.3  2003/03/22 22:44:57  zolnie
 * make it standalone library
 *
 * Revision 1.2  2003/03/06 18:54:20  zolnie
 * improvements for track/tower matching
 *
 * Revision 1.1  2003/02/20 21:26:58  zolnie
 * added simple geometry class
 *
 * Revision 1.1  2003/02/20 21:15:16  zolnie
 * *** empty log message ***
 *
 *********************************************************************/

const int kEEmcNumDepths     =  5;
const int kEEmcNumSectors    = 12;
const int kEEmcNumSubSectors =  5;
const int kEEmcNumEtas       = 12;
const int kEEmcNumSmdPlanes  =  3;
const int kEEmcNumStrips     =288;
const int kEEmcNumEdgeStrips =283;
const int kEEmcNumSmdLayers  =  2;  // 2 SMD layers: U and V


const float kEEmcZPRE1=270.190; // [cm] z preshower1
const float kEEmcZPRE2=271.695; // [cm] z preshower2
const float kEEmcZSMD =279.542; // [cm] z location of the EEMC SMD layer 
const float kEEmcZPOST=306.158; // [cm] z postshower  

// z-shift [cm] of innermost (negative) and outermost (positve) EEMC SMD plane 
const float kEEmcSmdZPlaneShift = 1.215;

// inner-shift offset of radius [cm] of EEMC SMD layers in 3 planes
const float kEEmcSmdROffset[kEEmcNumSmdPlanes]={1.850,0.925,0.0};

// The value of the map matrix(3x12) indicates if a EEMC SMD layer in a plane 
// is a U layer(1), V layer(2), or void(0)  
const int   kEEmcSmdMapUV[kEEmcNumSmdPlanes][kEEmcNumSectors] = 
                                     {{2,1,0,2,1,0,2,1,0,2,1,0},
                                      {0,2,1,0,2,1,0,2,1,0,2,1},
                                      {1,0,2,1,0,2,1,0,2,1,0,2}};

// The value of the map matrix(3x12) indicates if a EEMC SMD module in a plane
// is an edge module 
const int   kEEmcSmdMapEdge[kEEmcNumSmdPlanes][kEEmcNumSectors] = 
                                     {{0,0,0,1,0,0,0,0,0,1,0,0},
                                      {0,0,1,0,0,0,0,0,1,0,0,0},
                                      {0,0,0,0,0,0,0,0,0,0,0,0}};

const char kEEmcSmdLayerChar[kEEmcNumSmdLayers] = {'U','V'};

const int kEEmcSmdModuleIdPhiCrossPi = 9;

#endif
/* ------------------------------------------
   MEGA 1    270.19        -   preshower 1
   MEGA 2    271.695       -   preshower 2
   MEGA 3    273.15 
   MEGA 4    274.555
   MEGA 5    275.96
   MEGA 6    277.365
   ------------------
   SMD  1    278.327
   SMD  2    279.542
   SMD  3    280.757
   ------------------
   MEGA 7    282.3630
   MEGA 8    283.7680
   MEGA 9    285.1730
   MEGA 10   286.5780
   MEGA 11   287.9830
   MEGA 12   289.3880
   MEGA 13   290.7930
   MEGA 14   292.1980
   MEGA 15   293.6030
   MEGA 16   295.0080
   MEGA 17   296.4130
   MEGA 18   297.8180
   MEGA 19   299.2230
   MEGA 20   300.6280
   MEGA 21   302.0330
   MEGA 22   303.4380
   MEGA 23   304.8430
   MEGA 24   306.1580          - postshower 
   ------------------------------------------*/
