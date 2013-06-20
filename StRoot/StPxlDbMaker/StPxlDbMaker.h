// $Id: StPxlDbMaker.h,v 1.3 2013/06/20 19:19:01 bouchet Exp $
// $Log: StPxlDbMaker.h,v $
// Revision 1.3  2013/06/20 19:19:01  bouchet
// update for pxlSensorRowColumnMask tables
//
// Revision 1.2  2013/05/29 18:12:16  bouchet
// added GetCVS tags
//
// Revision 1.1  2013/05/24 15:59:53  bouchet
// first version
//
/***************************************************************************
 * Author:  J. Bouchet, M. Lomnitz , KSU
 * Description: PXL DB access Maker
 **************************************************************************/

#ifndef STPXLDBMAKER_H
#define STPXLDBMAKER_H

#ifndef StMaker_H
#include "StMaker.h"
#endif
#include "THashList.h"
class St_pxlSensorStatus;
class St_pxlSensorRowColumnMask;
class StPxlDbMaker : public StMaker {
 private:
  static THashList *fRotList;
  St_pxlSensorStatus        *mSensorStatus;        //!
  St_pxlSensorRowColumnMask *mSensorRowColumnMask; //!

 public: 
  StPxlDbMaker(const char *name="PxlDb");
  virtual       ~StPxlDbMaker();
  virtual Int_t  Init();
  virtual Int_t  InitRun(Int_t runNumber);
  virtual Int_t  Make();
  virtual Int_t  Finish();
  virtual void   Clear(const char *opt);
  virtual THashList *GetRotations() {return fRotList;}
  virtual Int_t CalculateSensorsPosition();
  virtual St_pxlSensorStatus *GetSensorStatus(){return mSensorStatus;}//will return the table for all sectors,ladders
  virtual St_pxlSensorRowColumnMask *GetSensorRowColumnMask(int mysector, int myladder){GetPxlSensorRowColumnMaskTable(mysector,myladder);return mSensorRowColumnMask;}//will return a table for a given sector, ladder
  void    GetPxlSensorStatusTable();
  void    GetPxlSensorRowColumnMaskTable(int sector, int ladder);

  virtual const char *GetCVS() const
  {static const char cvs[]="Tag $Name:  $ $Id: StPxlDbMaker.h,v 1.3 2013/06/20 19:19:01 bouchet Exp $ built "__DATE__" "__TIME__ ; return cvs;}
  ClassDef(StPxlDbMaker,0)   //StAF chain virtual base class for Makers
};
// Global pointers:
R__EXTERN StPxlDbMaker* gStPxlDbMaker;
#endif


