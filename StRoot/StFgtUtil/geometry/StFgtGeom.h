/*  StFgtGeom.h
 *
 *  FGT geometry class declaration.
 *
 *  \author W. Witzke (wowitz0@uky.edu)
 *
 */

#ifndef _ST_FGT_GEOM_H_
#define _ST_FGT_GEOM_H_

#include <TObject.h>
#include <TVector3.h>
#include <string>
#include <sstream>
#include <cstdlib>
#include <cmath>
#include <iostream>
#include <algorithm>

#include "StFgtGeomDefs.h"

//  StFgtGeomData stores data on each ordinate associated with each global ID
//  used to index individual geometry elements.
struct StFgtGeomData
{
    Bool_t isPhi;
    Double_t ordinate;
    Double_t lowerSpan;
    Double_t upperSpan;
};

//  StFgtGeom is a "singleton" class. Only one of it needs to exist in any
//  program. However, because the data contained in this class is entirely
//  static, the class itself is also entirely static. No instances of this
//  class can be created.
class StFgtGeom
{
    public:
	/*  Not sure that these have a point anymore.
	static StFgtGeom& getInstance()
	{
	    static StFgtGeom singleton;
	    return singleton;
	}

	//  Shouldn't need to do much here.
	~StFgtGeom() {}
	*/

	//  For all functions where they appear: Disc can be >= 0 (in theory,
	//  although only values 0-5 work at the moment, I believe). Quadrant
	//  is 0-3.  Layer is 'P' or 'R'. Strip is 0-720
	static Int_t encodeGeoId(
	    Int_t disc, Int_t quadrant, Char_t layer, Int_t strip
	)
	{
	    return
	    (
		( disc*kNumFgtQuadrants + quadrant )
		* kNumFgtLayers + ( layer == 'P' )
	    ) * kNumFgtStripsPerLayer + strip;
	}

	static void decodeGeoId(
	    Int_t geoId,
	    Short_t & disc, Short_t & quadrant, Char_t & layer, Short_t & strip
	)
	{
	    strip = geoId % kNumFgtStripsPerLayer;
	    geoId /= kNumFgtStripsPerLayer;

	    layer = ( geoId % kNumFgtLayers ) ? 'P' : 'R';
	    geoId /= kNumFgtLayers;

	    quadrant = geoId % kNumFgtQuadrants;
	    disc = geoId / kNumFgtQuadrants;
	}

	static std::string encodeGeoName(
	    Int_t disc, Int_t quadrant, Char_t layer, Int_t strip
	)
	{
            std::stringstream buff;
            buff << disc << quadrant << layer;
	    strip += 1;
            if ( strip < 10 )
                buff << "00";
            else if ( strip < 100 )
                buff << "0";

            buff << strip;
            return buff.str();
	}

	static void decodeGeoName(
	    const std::string & geoName,
	    Short_t & disc, Short_t & quadrant, Char_t & layer, Short_t & strip
	)
	{
	    disc = geoName[0] - '0';
	    quadrant = geoName[1] - '0';
	    layer = geoName[2];
	    strip = std::atoi( (geoName.substr(3)).c_str() ) - 1;
	}

	static std::string translateGeoIdToGeoName( Int_t geoId )
	{
	    Short_t disc, quadrant, strip;
	    Char_t layer;

	    decodeGeoId( geoId, disc, quadrant, layer, strip );
	    return encodeGeoName( disc, quadrant, layer, strip );
	}

	static Int_t translateGeoNameToGeoId( const std::string & geoName )
	{
	    Short_t disc, quadrant, strip;
	    Char_t layer;

	    decodeGeoName( geoName, disc, quadrant, layer, strip );
	    return encodeGeoId( disc, quadrant, layer, strip );
	}

	//  The ordinate, lowerSpan and upperSpan are all in centimeters or
	//  radians, depending on the layer.
	static void getPhysicalCoordinate(
	    Int_t geoId,
	    Short_t & disc, Short_t & quadrant, Char_t & layer,
	    Double_t & ordinate, Double_t & lowerSpan, Double_t & upperSpan
	)
	{
	    Short_t strip;

	    decodeGeoId( geoId, disc, quadrant, layer, strip );
	    ordinate =
		mStrips[
		    (layer == 'P') * kNumFgtStripsPerLayer + strip
		].ordinate;
	    lowerSpan =
		mStrips[
		    (layer == 'P') * kNumFgtStripsPerLayer + strip
		].lowerSpan;
	    upperSpan =
		mStrips[
		    (layer == 'P') * kNumFgtStripsPerLayer + strip
		].upperSpan;
	}

	//  The ordinate, lowerSpan and upperSpan are all in centimeters or
	//  radians, depending on the layer.
	static void getPhysicalCoordinate(
	    const std::string & geoName,
	    Short_t & disc, Short_t & quadrant, Char_t & layer,
	    Double_t & ordinate, Double_t & lowerSpan, Double_t & upperSpan
	)
	{
	    Short_t strip;

	    decodeGeoName( geoName, disc, quadrant, layer, strip );
	    ordinate =
		mStrips[
		    (layer == 'P') * kNumFgtStripsPerLayer + strip
		].ordinate;
	    lowerSpan =
		mStrips[
		    (layer == 'P') * kNumFgtStripsPerLayer + strip
		].lowerSpan;
	    upperSpan =
		mStrips[
		    (layer == 'P') * kNumFgtStripsPerLayer + strip
		].upperSpan;
	}

	//  Please note that the following functions do NOT access the STAR
	//  database to find mapping information. They assume the most
	//  straight-forward mapping scheme and use that.
	//  For those functions that have them, currently rdo can be 1-2, arm
	//  can be 0-5, apv can be 0-23 (although 10, 11, 22, and 23 are not
	//  technically valid) and channel is 0-127.
	static Int_t getNaiveGeoIdFromElecCoord(
	    Int_t rdo, Int_t arm, Int_t apv, Int_t channel
	)
	{
	    Short_t disc = int(arm/2) + (rdo-1)*3;
	    Short_t quadrant = (arm & 1)*2 + int( apv/12 );

	    if ( apv >= 12 )
		return
		(
		    disc*kNumFgtQuadrants + quadrant
		) * kNumFgtLayers * kNumFgtStripsPerLayer
		    + mNaiveMapping[ (apv-12)*128+channel ];
	    else
		return
		(
		    disc*kNumFgtQuadrants + quadrant
		) * kNumFgtLayers * kNumFgtStripsPerLayer
		    + mNaiveMapping[ apv*128+channel ];

	}

	static void getNaiveElecCoordFromGeoId(
            Int_t geoId, Int_t& rdo, Int_t& arm, Int_t& apv, Int_t& channel
	)
	{
           Short_t disc, quadrant, strip;
           Char_t layer;

           decodeGeoId( geoId, disc, quadrant, layer, strip );

           if( !mReverseNaiveMappingValid )
              makeReverseNaiveMappingValid();

           Int_t key = ( (layer=='P')*kNumFgtStripsPerLayer + strip );
           channel = mReverseNaiveMapping[ key ];
           apv = channel / 128;
           channel %= 128;

           if( quadrant % 2 )
              apv += 12;

           rdo = disc/3+1;
           arm = (disc % 3)*2 + (quadrant>1);
	}

	static std::string getNaiveGeoNameFromElecCoord(
	    Int_t rdo, Int_t arm, Int_t apv, Int_t channel
	)
	{
	    return
		translateGeoIdToGeoName(
		    getNaiveGeoIdFromElecCoord( rdo, arm, apv, channel )
		);
	}

	static void getNaivePhysCoordFromElecCoord(
	    Int_t rdo, Int_t arm, Int_t apv, Int_t channel,
	    Short_t & disc, Short_t & quadrant, Char_t & layer,
	    Double_t & ordinate, Double_t & lowerSpan, Double_t & upperSpan
	)
	{
	    getPhysicalCoordinate(
		getNaiveGeoIdFromElecCoord( rdo, arm, apv, channel ),
		disc, quadrant, layer, ordinate, lowerSpan, upperSpan
	    );
	}

	//  This is similar to the above functions, but it takes electronic
	//  coordinates and only returns the final ordinate. This is here
	//  primarily so that it can be used as a drop in replacement for
	//  older code that has similar functionality.
	static Double_t getNaiveMapping(
	    Int_t rdo, Int_t arm, Int_t apv, Int_t channel
	)
	{
	    if ( apv >= 12 )
		return
		    mStrips[ mNaiveMapping[ (apv-12)*128+channel ] ].ordinate;
	    else
		return
		    mStrips[ mNaiveMapping[ apv*128+channel ] ].ordinate;
	}

	static bool isNaiveR(
	    Int_t rdo, Int_t arm, Int_t apv, Int_t channel
	)
	{
	    if ( apv >= 12 )
		return
		    mNaiveMapping[ (apv-12)*128+channel ] < 720;
	    else
		return
		    mNaiveMapping[ apv*128+channel ] < 720;
	}

	//  Jan's necessary functions start here.  These were written by Jan,
	//  modified slightly by me.
	//  Jan: I have adjusted the dimensions to match FGT as build, September, 2011
	static double Rin()	{ return kFgtRin; }
	static double Rmid()	{ return kFgtRmid; }
	static double Rout()	{ return kFgtRout; }
	static double Rfirst()  { return kFgtRfirst; }
	static double Rlast()   { return kFgtRlast;}

	static double radStrip_pitch() { return kFgtRadPitch; }		//  cm
	static double phiStrip_pitch() { return kFgtPhiPitch/Rout(); }	//  rad

	static double yLimit() { return kFgtRout; }

	//  deadQuadEdge is in cm, local ref frame
	static double deadQuadEdge()	{ return kFgtDeadQuadEdge; }

	static double radStripOff() { return mRadStripOff; }
	static double phiStripOff() { return mPhiStripOff; }

	static int radStripLOCId_number() { return mRadStripLOCId_number; }
	static int phiStripLOCId_number() { return mPhiStripLOCId_number; }

	static  double phiQuadXaxis(int iquad);
	static  bool inDisc( TVector3 rLab );	
	static  bool belowFlat( TVector3 rLoc );
	static  int  getQuad( double phiLab );

	//  This is NOT a candidate for inlining.  This returns false if it is
	//  out of range.
	static bool localXYtoStripId(
	    int iquad, double x, double y,
	    int & iRadID, int & iPhiID,
	    int dbg=0
	);

	//  Jan's definitions for the final 400-800 micron pitch design
	static const double kFgtRout		= 38.25;    //	cm ,
	static const double kFgtRlast           = 38.1571;  // location of last R strip before Rout
	static const double kFgtRmid		= 19.125;   //	cm, at Rout/2.
	static const double kFgtRin		= 11.5;	    //	cm, 
	static const double kFgtRfirst          = 11.5385;  // location of first R strip after Rin
	static const double kFgtRflat		= 35.85;    //	cm, 
	static const double kFgtPhiflat		= 31.0/180.*3.1416;//  rad 
	static const double kFgtRadPitch	=  0.09538; //	nominal '800 mu pitch'
	static const double kFgtPhiPitch	=  0.08;    //	800 mu, at outer radi or at Rmid
	static const double kFgtPhiAnglePitch	=  0.002094; 
	static const double kFgtDeadQuadEdge	=  1.2;	    // (cm) effective dead area along quadrant edges

	//  Standard definitions.
	static const Int_t kNumStrips = 1440; //remove? JAN
	static const Int_t kNumChannels = 1280;
	static const Int_t kFgtMxDisk=6;    /* max # of  FGT disks @ STAR */
	static const Int_t kFgtMxQuad=4;    /* max # of quadrants in single FGT disk */
        static const Int_t kNumFgtStripsPerLayer = 720;

    protected:
	/*  Not sure that these have a point anymore.
	StFgtGeom() {};

	//  Copy constructors and assignment operators are *not* implemented.
	StFgtGeom( const StFgtGeom& );
	StFgtGeom& operator=( const StFgtGeom& );
	*/

	//  Various constants used in Jan's conversion functions.
	static double pi;
	static double doublepi;
	static double halfpi;
	static double mRadStripOff;
	static double mPhiStripOff;
	static int mRadStripLOCId_number;
	static int mPhiStripLOCId_number;
	static int mRadStripGBLId_number;
	static int mPhiStripGBLId_number;

	//  ---Private member variables---
	static StFgtGeomData mStrips[ kNumStrips ];

        // maps from (apv*128 + channel) to ((layer=='P')*kNumFgtStripsPerLayer + stripID)
	static Int_t mNaiveMapping[ kNumChannels ];

        // reverse mapping: ((layer=='P')*kNumFgtStripsPerLayer + stripID) to (apv*128 + channel)
        static Bool_t mReverseNaiveMappingValid;
	static Int_t mReverseNaiveMapping[ 2*kNumFgtStripsPerLayer ];
        static void makeReverseNaiveMappingValid();

 public:
	//  What follows are some functions to help with the
	//  localXYtoStripID function.  These are also written by Jan, modified
	//  slightly by me.

	//  These next two return -1 on error.
	static  int rad2LocalStripId( double rad, double phi, double *binFrac=0 );
	static  int phiLoc2LocalStripId( double phiLoc, double *binFrac=0 );

	static int radIdLocal2Global( int iquad, int radId )
	{
	    return radId + radStripLOCId_number() * iquad;
	}

	static int phiIdLocal2Global( int iquad, int phiId )
	{
	    return phiId + phiStripLOCId_number() * iquad;
	}

};

#endif

/*
--------- December 18, 2010 ---------
 Description of quadrant geometry by Doug Hasell , December 18, 2010 :

(Only 400_800 pitch description is left from the original text , Jan.)

	In my coordinate system the X axis points to the right, Y axis points up, and the Z axis comes out of the page towards you.  Thus the beam direction is in the positive Z direction and the beam is at X=0, Y=0.  For the rest of the discussion I don't need the Z coordinate.  So for the following the origin is at ( 0, 0 ) in the XY plane formed by the readout board.

	The FGT quadrants have a support frame.  The outer edges of this frame are defined by three lines and two arcs:

Line 1 has Y = 0.5 mm

Line 2 has X = 0.5 mm

Line 3 is perpendicular to a ray at 31 degrees, 370 mm from the origin

Arc 1 has radius = 103.5 mm

Arc 2 has radius = 394.0 mm

	The frame is everywhere 11.5 mm wide.  With the outer edges defined as above the frame width defines the inner dimensions of the frame or the "active" area of the detector.  (i.e. Y = 12, X = 12, perpendicular to 31 degrees at 358.5 mm, R = 115, and R = 382.5).

	The lines or rather string of pads connected in an arc at constant radius effectively stop at the inner edge of the frame.  Actually they extend slightly beneath the frame but this is no longer in the active area.  
	The exception to this are the lines of constant radius which intersect the flat in the outer region.  These lines end where they disappear under the flat.  Thus there are two lines at each of these radii readout on either side.  They are not connected together.

	For the 400-800 micron pitch design the lines of constant radius only arc half way across the quadrant.  Thus lines of constant radius arcing from around 0 degrees to almost 45 degrees are readout on the edge near the X axis while those arcing from just over 45 degrees to near 90 degrees are readout on the other side.

	Again R lines intersecting the flat are the exception and are connected and readout as above on one side or the other.

	The R lines have a constant pitch.   For the 400-800 micron design the pitch is 953.8 microns. Remember these are really pads connected together to simulate a line arcing at a constant radius.  This pitch is the pitch in the radial or outward direction.  The pads are actually trapezoids with the other dimensions identical to the width of the neighboring PHI lines which changes pitch with radius. 

-------------
	PHI lines or lines at a constant angle extend from the outer edge (but the inside edge of the frame) to a radius of 382.5 / 2 = 191.25 mm.  The pitch varies from 800 micron at the outer edge to 400 at half the radius.  Lines which start at the outer edge but which disappear under the frame before reaching R=191.25 are terminated where they disappear under the frame.  So the PHI lines near the frame don't make it all the way in.

	At R = 191.25 half of the PHI lines end and the other half revert to 800 micron pitch and continue on to the inner radius of the frame. Again, where they go beneath the frame they are terminated there.
 
	To determine which PHI lines go from the outer radius to the inner radius look at the angle of the PHI lines connected to connector 0 or 9.  You'll see that these change angle with a step size twice that of the PHI lines in the outer connectors 3-6.  Those PHI lines with integer multiples of this step go from the outer edge all the way to the inner edge.  Those with half integer multiples go from the outer end but end at R = 191.25 mm.

*/


/*
 *  $Id: StFgtGeom.h,v 1.19 2011/10/09 13:36:44 rfatemi Exp $
 *  $Log: StFgtGeom.h,v $
 *  Revision 1.19  2011/10/09 13:36:44  rfatemi
 *  Update with Rlast and Rfirst for location of R strips
 *
 *  Revision 1.18  2011/10/07 19:43:32  balewski
 *  make method public
 *
 *  Revision 1.17  2011/10/07 03:42:38  rfatemi
 *  Updates to get strip Id from radius r
 *
 *  Revision 1.16  2011/10/06 15:16:15  sgliske
 *  fixed spaces before Log: and Id:
 *
 *
 */
