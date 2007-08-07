//////////////////////////////////////////////////////////////////////////
//
//
// StTriggerSimuMaker R.Fatemi, Adam Kocoloski , Jan Balewski  (Fall, 2007)
//
// Goal: generate trigger response based on ADC
// implemented BEMC,EEMC,....
// >StTriggerSimu/*SUB*/St*SUB*TriggerSimu.h
// >where *SUB* are the subsystems: Eemc, Bemc, Bbc, L2,.... 
//
//////////////////////////////////////////////////////////////////////////

#ifndef STAR_StTriggerSimuMaker
#define STAR_StTriggerSimuMaker

#ifndef StMaker_H
#include "StMaker.h"
#endif
class StEemcTriggerSimu;
class StBecTriggerSimu;
class St_db_Maker;

class StTriggerSimuMaker : public StMaker {
private:
  int mYear;
  int mMCflag; // set yo 0 for real data
  St_db_Maker *mDbMk;
  void addTriggerList();

public:

    StTriggerSimuMaker(const char *name="StarTrigSimu");
    virtual           ~StTriggerSimuMaker();

    void    useEemc();
    void    useBbc();
    void    useBemc();
    void    setMC(int x) {mMCflag=x;}

    //hang all activated trigger detectors below
    StEemcTriggerSimu *eemc;
    StBbcTriggerSimu *bbc;
    StBemcTriggerSimu *bemc;

    TObjArray  *mHList; // output histo access point
    void setHList(TObjArray * x){mHList=x;}
    vector <int> mTriggerList;
    virtual Int_t     Init();
    virtual Int_t     Make();
    virtual Int_t     Finish();
    virtual void      Clear(const Option_t* = "");
    virtual Int_t InitRun  (int runumber);
    bool    isTrigger(int trigId);   
    void    setDbMaker(St_db_Maker *dbMk) { mDbMk=dbMk;}
 
    ClassDef(StTriggerSimuMaker,0)
};
   
#endif



// $Id: StTriggerSimuMaker.h,v 1.5 2007/08/07 15:48:38 rfatemi Exp $
//
// $Log: StTriggerSimuMaker.h,v $
// Revision 1.5  2007/08/07 15:48:38  rfatemi
// Added BEMC access methods
//
// Revision 1.4  2007/07/23 03:03:39  balewski
// fix
//
// Revision 1.3  2007/07/22 23:09:51  rfatemi
// added access to Bbc
//
// Revision 1.2  2007/07/21 23:35:24  balewski
// works for M-C
//
// Revision 1.1  2007/07/20 21:01:41  balewski
// start
//
