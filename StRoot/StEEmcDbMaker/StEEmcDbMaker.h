// $Id: StEEmcDbMaker.h,v 1.9 2003/08/26 03:02:30 balewski Exp $

/*! \class StEEmcDbMaker 
\author Jan Balewski

Interface to STAR-DB. Info from various DB tables is 
correlated for every ADC channel and stored in memory in
 mDbItem[].
mDbItem1[] has single index based on the channel name, 
calculated by EEname2index() utility.

<pre>
----------------------------------------
Usage:


// My Makers  1
  StEEmcDbMaker  *myMk1=new StEEmcDbMaker("EEmcDB");
  myMk1->setTimeStampDay(20031215);  // format: yyyymmdd

  St_db_Maker *dbMk = new St_db_Maker("StarDb","MySQL:StarDb");
  // My Makers  2 
  StEEfast2slowMaker *myMk= new StEEfast2slowMaker("EE-fast2slow");
  myMk->setDb(myMk1);
 
Example how to use this maker:
www.star.bnl.gov/STAR/eemc -->How To

</pre>
*/ 

#ifndef STAR_SteemcDbMaker
#define STAR_SteemcDbMaker

#ifndef StMaker_H
#include "StMaker.h"
#endif

// needed DB c-structs  
class eemcDbADCconf_st;
class eemcDbPMTconf_st;
class eemcDbPMTcal_st;
class eemcDbPMTped_st;
class eemcDbPMTstat_st;

class  StEEmcDbIndexItem1;


class StEEmcDbMaker : public StMaker {
 private:
  // static Char_t  m_VersionCVS = "$Id: StEEmcDbMaker.h,v 1.9 2003/08/26 03:02:30 balewski Exp $";

  int mfirstSecID, mlastSecID;
  int mNSector;
  int myTimeStampDay;
  unsigned int myTimeStampUnix;
  void mReloadDb(); ///< reads data from STAR-DB
  void mOptimizeDb(); ///< creates local fast look-up tables
  
  // pointers to Db tables for each sector
  int *mDbsectorID; //!
  eemcDbADCconf_st  **mDbADCconf; //!
  eemcDbPMTconf_st  **mDbPMTconf; //!
  eemcDbPMTcal_st   **mDbPMTcal ; //!
  eemcDbPMTped_st   **mDbPMTped ; //!
  eemcDbPMTstat_st  **mDbPMTstat ; //!

  // local fast look-up tables
  StEEmcDbIndexItem1   *mDbItem1; //!  assess via logical name (sec/sub/eta)
  StEEmcDbIndexItem1   ***mLookup; //! access via crate/chan
  int mxAdcCrate, mxAdcChan;

  void mCleanDbNames(char*,int); ///< utility  
  void mPrintItems();///< utility
 
  float KsigOverPed; // defines threshold
  int nFound;
  TString dbName; //name of the DB used 

  template <class St_T, class T_st> T_st * getTable(TDataSet *eedb, int secID, TString tabName);
  

 protected:
 public:  

  void print(int k=0);
  void setSectors(int ,int); ///< limit the range of sectors for speed
  void setThreshold(float x);// defines threshold for ADCs

  const  StEEmcDbIndexItem1* getT(int sec, char sub, int eta); ///< returns full DB info for one Tower channel
  const  StEEmcDbIndexItem1* getByIndex(int index); ///< returns full DB info for one any channel
  const  StEEmcDbIndexItem1* get(int crate, int channel); ///< returns full DB info for any ADC channel

  void setTimeStampDay( int ); ///< to fix  time stamp for all events, default =not fixed 
  
  void setDBname(TString name){ dbName=name;}

  int valid(){ return nFound;} // return # of valid BD records
  unsigned int getTimeStampUnix(){return myTimeStampUnix;} ///< if zero then not fixed 

  StEEmcDbMaker(const char *name="EEmcDbMaker");

  virtual       ~StEEmcDbMaker();
  virtual Int_t Init();
  virtual Int_t  Make();
  virtual Int_t InitRun  (int runumber); ///< to access STAR-DB
  
  virtual const char *GetCVS() const {
    static const char cvs[]="Tag $Name:  $ $Id: StEEmcDbMaker.h,v 1.9 2003/08/26 03:02:30 balewski Exp $ built "__DATE__" "__TIME__ ; 
    return cvs;
  }
  
  ClassDef(StEEmcDbMaker, 1)   

 };

#endif

// $Log: StEEmcDbMaker.h,v $
// Revision 1.9  2003/08/26 03:02:30  balewski
// fix of pix-stat and other
//
// Revision 1.8  2003/08/25 17:57:12  balewski
// use teplate to access DB-tables
//
