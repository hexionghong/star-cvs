/*!
 * \class StFgtA2CMaker 
 * \author S. Gliske, Sept 2011
 */

/***************************************************************************
 *
 * $Id: StFgtA2CMaker.h,v 1.10 2012/01/30 11:40:04 sgliske Exp $
 * Author: S. Gliske, Oct 2011
 *
 ***************************************************************************
 *
 * Description: Converts the ADC value to charge and optionally
 * removes strips with status not passing the mask.  Computing the
 * charge currently involves
 *
 * 1) pedistal subtraction
 * 2) applying minimum threshold (both fixed and multiples of the pedistal st. err.)
 * 3) applies gains
 *
 * The status map is applied as follows: status 0x00 is good, all else
 * is bad.  Strips are removed if the status bit anded with the mask
 * is non-zero. To remove all strips with bad status, set the mask to
 * 0xFF.  To ignore status, set the mask to 0x00.  To remove only
 * strips with bit 3 set, set the mast to 0x04, etc.  Status is
 * currently only a uchar.
 *
 * Currently, these steps are only applied for time bins matching the
 * given time bin mask.  The maker can also remove raw hits from the
 * StFgtEvent for time bins that will not be used.  Eventually, this
 * maker may also include common mode noise correction.
 *
 * The time bin mask is a bit field, with the lowest bit being the
 * lowest time bin, e.g. 0x01 is the 0th time bin, while 0x10 is the 4th
 * time bin.
 *
 ***************************************************************************
 *
 * $Log: StFgtA2CMaker.h,v $
 * Revision 1.10  2012/01/30 11:40:04  sgliske
 * a2cMaker now fits the pulse shape,
 * strip containers updated
 *
 * Revision 1.9  2012/01/30 10:42:22  sgliske
 * strip containers now contain adc values for
 * all time bins.  Also fixed bug where setType modified the timebin
 * rather than the type.
 *
 * Revision 1.8  2012/01/28 11:22:53  sgliske
 * changed status check to status map
 * changed setDb to setFgtDb
 * cleaned up few other minor things
 *
 * Revision 1.7  2012/01/24 06:52:46  sgliske
 * made status cuts optional
 * and updated status to a fail condition--
 * i.e. status == 0x0 is good, otherwise is bad.
 * WARNING--this may be different than that used at first
 * in for the cosmic test stand.
 *
 * Revision 1.6  2012/01/24 05:54:51  sgliske
 * changed default name to reflect A2C,
 * as opposed to old CorMaker
 *
 * Revision 1.5  2012/01/06 17:48:00  sgliske
 * Added requested GetCVS tag
 *
 * Revision 1.4  2011/12/01 00:13:23  avossen
 * included use of db. Note: For DB use it hast to be set with
 * setDb. Instantiate StFgtDBMaker, get the StFgtDb from the getTables
 * method and give the pointer to the A2C maker
 *
 * Revision 1.3  2011/11/25 20:24:13  ckriley
 * added statusmaker functionality
 *
 * Revision 1.2  2011/11/01 18:46:14  sgliske
 * Updated to correspond with StEvent containers, take 2.
 *
 * Revision 1.1  2011/10/28 14:58:49  sgliske
 * replacement to StFgtCorAdcMaker
 *
 *
 **************************************************************************/

#ifndef _ST_FGT_A2C_MAKER_H
#define _ST_FGT_A2C_MAKER_H

#include <string>
#include "StMaker.h"

class StFgtPedReader;
class StFgtStatusReader;
class StFgtDb;

class StFgtA2CMaker : public StMaker {
 public:
   // constructors
   StFgtA2CMaker( const Char_t* name = "fgtA2CMaker" );

   // default OK
   // StFgtA2CMaker(const StFgtA2CMaker&);

   // deconstructor
   virtual ~StFgtA2CMaker();

   // equals operator -- default OK
   // StFgtA2CMaker& operator=(const StFgtA2CMaker&);

   virtual Int_t Init();
   virtual Int_t Make();

   // modifiers
   void setPedReaderFile( const Char_t* filename );
   void setStatusReaderFile( const Char_t* filename );
   void setAbsThres( Float_t thres );  // set to below -4096 to skip cut
   void setRelThres( Float_t thres );  // set to zero to skip cut
   void setFgtDb( StFgtDb *fgtDb);
   void doCutBadStatus( Bool_t doIt );
   void setStatusMask( UChar_t mask );

   virtual const char *GetCVS() const
   {static const char cvs[]="Tag $Name:  $ $Id: StFgtA2CMaker.h,v 1.10 2012/01/30 11:40:04 sgliske Exp $ built "__DATE__" "__TIME__ ; return cvs;}

 protected:
   // for the ped reader
   StFgtPedReader *mPedReader;
   std::string mPedFile;

   // for the strip status reader
   StFgtStatusReader *mStatusReader;
   std::string mStatusFile;
   Bool_t useStatusFile;

   // other parameters
   Int_t mStatusMask;
   Float_t mAbsThres, mRelThres;

   // if the user gives a ped file, use that, otherwise get peds from db
   Bool_t usePedFile;
   StFgtDb* mDb;

   // for fitting
   TF1 *mPulseShapePtr;
   TH1F *mHistPtr;
 
 private:   
   ClassDef(StFgtA2CMaker,1);

}; 

// inline functions

// deconstructor
inline StFgtA2CMaker::~StFgtA2CMaker(){ /* */ };

// modifiers
inline void StFgtA2CMaker::setPedReaderFile( const Char_t* filename ){ mPedFile = filename; usePedFile=true; };
inline void StFgtA2CMaker::setStatusReaderFile( const Char_t* filename ){ mStatusFile = filename; useStatusFile=true;};
inline void StFgtA2CMaker::setAbsThres( Float_t thres ){ mAbsThres = thres; };
inline void StFgtA2CMaker::setRelThres( Float_t thres ){ mRelThres = thres; };
inline void StFgtA2CMaker::setFgtDb(StFgtDb* db ){mDb=db; };
inline void StFgtA2CMaker::doCutBadStatus(  Bool_t doIt ){ mStatusMask = 0xFF; };
inline void StFgtA2CMaker::setStatusMask( UChar_t mask ){ mStatusMask = mask; };

#endif
