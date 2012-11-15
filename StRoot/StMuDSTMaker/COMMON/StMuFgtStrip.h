/***************************************************************************
 *
 * $Id: StMuFgtStrip.h,v 1.1 2012/11/15 22:27:24 sangalin Exp $
 * Author: S. Gliske, Jan 2012
 *
 ***************************************************************************
 *
 * Description: data for individual strip of the FGT.  At time of
 * creation was a nearly identical copy to the StFgtStrip class, but
 * without the StObject inheritance.
 *
 ***************************************************************************
 *
 * $Log: StMuFgtStrip.h,v $
 * Revision 1.1  2012/11/15 22:27:24  sangalin
 * Copied over from StFgtDevel.
 *
 * Revision 1.10  2012/07/20 16:11:24  sgliske
 * Added StFgtStripAssociation, and removed all dynamically
 * allocated memory from StMuFgt* containers.
 * Also removed StMuFgtInfo
 *
 * Revision 1.9  2012/04/13 18:56:31  sgliske
 * More adjustments based on the review:
 * - Lastest StEvents from Thomas U.
 * - StFgtA2CMaker can no longer remove strips other than bad status or bad ped
 * - other related updates
 *
 * Revision 1.8  2012/03/20 19:58:33  sgliske
 * fixed 'get' functions
 *
 * Revision 1.7  2012/03/07 19:21:01  sgliske
 * updated based on changes to StEvent
 *
 * Revision 1.6  2012/03/07 15:23:53  sgliske
 * StFgtStrip no longer has a type field
 *
 * Revision 1.5  2012/01/30 16:38:04  sgliske
 * updated class def number
 *
 * Revision 1.4  2012/01/30 11:40:05  sgliske
 * a2cMaker now fits the pulse shape,
 * strip containers updated
 *
 * Revision 1.3  2012/01/30 10:42:23  sgliske
 * strip containers now contain adc values for
 * all time bins.  Also fixed bug where setType modified the timebin
 * rather than the type.
 *
 * Revision 1.2  2012/01/04 22:34:56  sgliske
 * Fixed some bugs
 *
 * Revision 1.1  2012/01/04 19:15:34  sgliske
 * Reintroduced support for the FGT in MuDst
 *
 *
 **************************************************************************/

#ifndef _ST_MU_FGT_STRIP_H_
#define _ST_MU_FGT_STRIP_H_

#include <TObject.h>
#include "StRoot/StFgtUtil/StFgtConsts.h"

class StFgtStrip;

class StMuFgtStrip : public TObject {
 public:
   // constructors
   StMuFgtStrip();

   // to convert from StFgtStrips
   StMuFgtStrip( const StFgtStrip& other );
   StMuFgtStrip& operator=(const StFgtStrip& other );

   // defaults
   // StMuFgtStrip(const StMuFgtStrip&);  use default
   // StMuFgtStrip& operator=(const StMuFgtStrip&); use default
   // ~StMuFgtStrip(); use default

   // accessors
   Int_t   getGeoId() const;
   Short_t getAdc( Int_t tb ) const;
   Short_t getClusterSeedType() const;
   Float_t getCharge() const;
   Float_t getChargeUncert() const;

   // modifiers
   void setGeoId           ( Int_t geoId );
   void setAdc             ( Short_t adc, Int_t tb );
   void setClusterSeedType ( Short_t type );
   void setCharge          ( Float_t charge );
   void setChargeUncert    ( Float_t uncert );
    
 protected:
   // data members
   Int_t   mGeoId;                   // indexing: 6 disk * 4 quad * 2 planes * 720 strips
   Short_t mAdc[kMuFgtNumTimeBins];  // 
   Short_t mClusterSeedType;         // as defined in StEvent/StEnumerations.h
   Float_t mCharge;                  // before GEM, units (C), relation: ADC = ped + charge*gain(r,phi,disc)
   Float_t mChargeUncert;            // 

 private:   
   ClassDef(StMuFgtStrip,4);
}; 

// inline functions

inline Int_t   StMuFgtStrip::getGeoId()           const { return mGeoId; };
inline Float_t StMuFgtStrip::getCharge()          const { return mCharge; };
inline Float_t StMuFgtStrip::getChargeUncert()    const { return mChargeUncert; };
inline Short_t StMuFgtStrip::getClusterSeedType() const { return mClusterSeedType; };

inline Short_t StMuFgtStrip::getAdc( Int_t tb ) const {
   return (tb > -1 && tb < kMuFgtNumTimeBins) ? mAdc[tb] : 0;
};

inline void StMuFgtStrip::setGeoId           ( Int_t geoId ){ mGeoId = geoId; };
inline void StMuFgtStrip::setCharge          ( Float_t charge ){ mCharge = charge; };
inline void StMuFgtStrip::setChargeUncert    ( Float_t charge ){ mCharge = charge; };
inline void StMuFgtStrip::setClusterSeedType ( Short_t type ){ mClusterSeedType = type; };

inline void StMuFgtStrip::setAdc( Short_t adc, Int_t tb ) {
   if( tb > -1 && tb < kMuFgtNumTimeBins ) 
      mAdc[tb] = adc;
};

#endif

