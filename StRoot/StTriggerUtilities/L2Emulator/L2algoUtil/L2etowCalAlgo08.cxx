#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

/*********************************************************
  $Id: L2etowCalAlgo08.cxx,v 1.1 2008/01/30 00:47:16 balewski Exp $
  \author Jan Balewski, MIT, 2008 
 *****************************************************
  Descripion:
  calibrates Endcap towers, result is used by other L2-algos
 *****************************************************/


#ifdef  IS_REAL_L2  //in l2-ana  environment
  #include "../L2algoUtil/L2EmcDb.h"
  #include "../L2algoUtil/L2Histo.h"
#else
  #include "L2EmcDb.h"
  #include "L2Histo.h"
  #include "L2EmcGeom.h"
#endif

#include "L2etowCalAlgo08.h"


//=================================================
//=================================================
L2etowCalAlgo08::L2etowCalAlgo08(const char* name, L2EmcDb* db, L2EmcGeom *geoX, char* outDir)  :  L2VirtualAlgo2008( name,  db,  outDir) { 
  /* called one per days
     all memory allocation must be done here
  */

  mGeom=geoX; assert(mGeom);

  setMaxHist(32);
  createHisto();

  // initilalize ETOW-Calibrated-data
  int k;
  for(k=0;k<L2eventStream2008::mxToken;k++){
    L2EtowCalibData08 & etowCalibData=globL2eventStream2008.etow[k];  
    etowCalibData.nInputBlock=0;
  }
 }

/* ========================================
  ======================================== */
int 
L2etowCalAlgo08::initRunUser( int runNo, int *rc_ints, float *rc_floats) {

 
  // unpack params from run control GUI
  par_dbg       =  rc_ints[0];
  par_gainType  =  rc_ints[1];
  par_nSigPed   =  rc_ints[2];

  par_twEneThres = rc_floats[0];
  par_hotEtThres = rc_floats[1];;

  // verify consistency of input params
  int kBad=0;
  kBad+=0x00001 * (par_gainType<kGainZero || par_gainType>kGainOffline);
  kBad+=0x00002 * (par_nSigPed<2 || par_nSigPed>5);
  kBad+=0x00004 * (par_twEneThres<0.1 ||  par_twEneThres>1.5);

  if (mLogFile) { 
    fprintf(mLogFile,"L2%s algorithm initRun(R=%d), compiled: %s , %s\n params:\n",getName(),mRunNumber,__DATE__,__TIME__);
    fprintf(mLogFile," - use ETOW=%d,  gain Ideal=%d or Offline=%d, debug=%d\n",
	    par_gainType>=kGainIdeal, par_gainType==kGainIdeal, par_gainType==kGainOffline, par_dbg);
    fprintf(mLogFile," - thresholds: ADC-ped> %d*sigPed .AND. energy>%.2f GeV \n", par_nSigPed, par_twEneThres);

    fprintf(mLogFile," - hot tower thresholds:  ET/GeV=%.2f\n",par_hotEtThres);
    fprintf(mLogFile,"initRun() params checked for consistency, Error flag=0x%04x\n",kBad);
  }
  
  if(kBad) return kBad;

  // clear content of all histograms
  int i;
  for (i=0; i<mxHA;i++) if(hA[i])hA[i]->reset();

  // upadate title of histos
  char txt[1000];
  sprintf(txt,"ETOW tower, E_T>%.2f GeV (input); x: ETOW RDO index=chan*6+fiber; y: counts",par_hotEtThres);
  hA[10]->setTitle(txt);
  
  sprintf(txt,"ETOW tower, Et>%.2f GeV (input); x: ETOW softID=i#phi+60*i#eta",par_hotEtThres);
  hA[11]->setTitle(txt);
  sprintf(txt,"ETOW tower, Et>%.2f GeV (input); x: eta bin, [-1,+1];  y: phi bin ~ TPC sector",par_hotEtThres);
  hA[12] ->setTitle(txt);
  
  sprintf(txt,"#ETOW towers / event , Et>%.2f GeV; x: # ETOW towers; y: counts",par_hotEtThres);
  hA[14] ->setTitle(txt);
  
  // re-caluclate geometry properties
  mGeom->etow.clear(); 
  int nT=0; /* counts # of unmasekd towers */ 
  int nTg=0; /* counts # of reasonable calibrated towers */ 
  int nEneThr=0, nPedThr=0; //ETOW count # of towers above & below threshold
  if(par_gainType>=kGainIdeal)  // this disables the whole loop below
  for(i=0; i<EmcDbIndexMax; i++) {
    const L2EmcDb::EmcCDbItem *x=mDb->getByIndex(i);
    if(mDb->isEmpty(x)) continue;  /* dropped not mapped  channels */
    /*....... E N D C A P  .................*/
    if (!mDb->isETOW(x) ) continue; /* drop if not ETOW */
    if(x->fail) continue;          /* dropped masked channels */
    if(x->gain<=0) continue;       /* dropped uncalibrated towers , tmp */
    nT++;   

    float adcThres=x->ped+par_nSigPed* fabs(x->sigPed);
    float otherThr=x->ped+par_twEneThres*x->gain;
 
    if(adcThres<otherThr) { //use energy threshold if higher
      adcThres=otherThr;
      nEneThr++;
    } else {
      nPedThr++;
    }
    
    /* use rdo index to match RDO order in the ADC data banks */    
    if(x->eta<=0 || x->eta>EtowGeom::mxEtaBin) return -90;
    int ietaTw= (x->eta-1); /* correct */

    // use ideal gains for now, hardcoded
    assert(par_gainType==kGainIdeal); // offline gains not implemented - should be changed here, Jan
    mGeom->etow.gain2Ene_rdo[x->rdo]=mGeom->etow.idealGain2Ene[ietaTw];
    mGeom->etow.gain2ET_rdo[x->rdo]=mGeom->getIdealAdc2ET();
    
    mGeom->etow.thr_rdo[x->rdo]=(int) (adcThres);
    mGeom->etow.ped_rdo[x->rdo]=(int) (x->ped);
    nTg++;      
  }
  
  if (mLogFile) {
    fprintf(mLogFile,"  found  towers working=%d calibrated=%d, based on ASCII DB\n",nT,nTg);
    fprintf(mLogFile,"  thresh defined by energy=%d  or NsigPed=%d \n",nEneThr, nPedThr);
  }

  return 0; //OK


}               

/* ========================================
  ======================================== */
void 
L2etowCalAlgo08::computeEtow(int token, int eemcIn, ushort *rawAdc){
  // Etow calibration is a special case, must have one exit  at the end

  computeStart();
  token&=L2eventStream2008::tokenMask; // only to protect against a bad token, Gerard's trick
 
  //...... now token is valid  ........
  L2EtowCalibData08 & etowCalibData=globL2eventStream2008.etow[token];  
  etowCalibData.nInputBlock++;
  
  // clear data for this token from previous event
  etowCalibData.hitSize=0;

  int nTower=0; /* counts mapped & used ADC channels */
  int nHotTower=0;
  if(eemcIn && par_gainType>kGainZero) { // EVEVEVEVEVE
    // ............process this event ...............
    short rdo;
    int adc; // pedestal subtracted 
    float et;
    ushort *thr=mGeom->etow.thr_rdo;
    ushort *ped=mGeom->etow.ped_rdo;
    float *gain2ET=mGeom->etow.gain2ET_rdo;
    float *gain2Ene=mGeom->etow.gain2Ene_rdo;
    HitTower1 *hit=etowCalibData.hit;
    for(rdo=0; rdo<EtowGeom::mxRdo; rdo++){
      if(rawAdc[rdo]<thr[rdo])continue;
      adc=rawAdc[rdo]-ped[rdo];  //do NOT correct for common pedestal noise - bad for the jet finder
      et=adc/gain2ET[rdo]; 
      hit->rdo=rdo;
      hit->adc=adc;
      hit->et=et;
      hit->ene=adc/gain2Ene[rdo]; 
      hit++;
      nTower++; 
      // only monitoring
      // if(par_dbg>0) printf("pro rdo=%d adc=%d  nTw=%d\n",rdo,adc,tmpNused);
      if(et >par_hotEtThres) {
	hA[10]->fill(rdo);
	nHotTower++;
      }
    }
    etowCalibData.hitSize=nTower;
    
    // QA histos
    hA[13]->fill(nTower);
    hA[14]->fill(nHotTower);
  
  } // EVEVEVEVEVE

  // debugging should be off for any time critical computation
  if(par_dbg>0){
    printf("L2-%s-compute: set adcL size=%d\n",getName(),nTower); 
    printf("dbg=%s: found  nTw=%d\n",getName(),nTower);
    if(par_dbg>0)   print0();
    printCalibratedData(token);
  } 
  
  computeStop(token);

} 

/* ========================================
  ======================================== */
void 
L2etowCalAlgo08::computeUser(int token ){

  printf("computeUser-%s FATAL CRASH\n If you see this message it means l2new is very badly misconfigured \n and L2-etow-calib algo was not executed properly\n before calling other individual L2-algos. \n\n l2new will aborted now - fix the code, Jan B.\n",getName());
  assert(1==2);
}


/* ========================================
  ======================================== */
void
L2etowCalAlgo08::finishRunUser() {  
  /* called once at the end of the run
     write do whatever you want, log-file & histo-file are still open
     Here it seraches for hot tower, re-project histos vs. other representations
  */
  
  int eHotSum=1,eHotId=-1;
  const int *data20=hA[10]->getData();
  const L2EmcDb::EmcCDbItem *xE=mDb->getByIndex(502); // some wired default?
  
  int i;
  for(i=0; i<EmcDbIndexMax; i++) {
    const L2EmcDb::EmcCDbItem *x=mDb->getByIndex(i);
    if(mDb->isEmpty(x)) continue;
    if (!mDb->isETOW(x) ) continue;
    int ieta= (x->eta-1);
    int iphi= (x->sec-1)*EtowGeom::mxSubs + x->sub-'A' ;
    int softId= iphi+EtowGeom::mxPhiBin*ieta;
    hA[11]->fillW(softId,data20[x->rdo]);
    hA[12]->fillW(ieta, iphi,data20[x->rdo]);
    if(eHotSum<data20[x->rdo]) {
      eHotSum=data20[x->rdo];
      eHotId=softId;
      xE=x;
    }
  }
  
  if (mLogFile){
    fprintf(mLogFile,"#ETOW_hot tower _candidate_ (eHotSum=%d of %d eve) :, softID %d , crate %d , chan %d , name %s\n",eHotSum,mEventsInRun,eHotId,xE->crate,xE->chan,xE->name);
  }
  
  //...... QA tokens .....
  int tkn1=99999, tkn2=0; // min/max token
  int nTkn=0;
  int tkn3=-1, nTkn3=-1; // most often used token
  
  int k;
  for(k=0;k<L2eventStream2008::mxToken;k++){
    L2EtowCalibData08 & etowCalibData=globL2eventStream2008.etow[k];  
    if(etowCalibData.nInputBlock==0) continue;
    hA[1]->fillW(k,etowCalibData.nInputBlock);
    if(nTkn3<etowCalibData.nInputBlock){
      nTkn3=etowCalibData.nInputBlock;
      tkn3=k;
    }

    nTkn++;
    if(tkn1>k) tkn1=k;
    if(tkn2<k) tkn2=k;
  }
  if (mLogFile){
    fprintf(mLogFile,"#ETOW_token_QA:  _candidate_ hot token=%d used %d for %d events, token range [%d, %d], used %d tokens\n",tkn3,nTkn3,mEventsInRun,tkn1,tkn2,nTkn);
  }

}


//=======================================
//=======================================
void 
L2etowCalAlgo08::createHisto() {
  memset(hA,0,sizeof(hA));
  //token related spectra
  hA[1]=new  L2Histo(1,"L2-etow-calib: seen tokens;  x:  token value; y: events ",20);
  
  // ETOW  raw spectra (zz 4 lines)
  hA[10]=new L2Histo(10,"etow hot tower 1", EtowGeom::mxRdo); // title upadted in initRun
  hA[11]=new L2Histo(11,"etow hot tower 2", EtowGeom::mxRdo); // title upadted in initRun
  hA[12]=new L2Histo(12,"etow hot tower 3", EtowGeom::mxEtaBin,EtowGeom::mxPhiBin); // title upadted in initRun  
  hA[13]=new L2Histo(13,"ETOW #tower w/ energy /event; x: # ETOW towers; y: counts", 30); 
  hA[14]=new L2Histo(14,"# hot towers/event", 30); 

}



/* ========================================
  ======================================== */
void 
L2etowCalAlgo08::print0(){ // full raw input  ADC array
  // empty
 }


/**********************************************************************
  $Log: L2etowCalAlgo08.cxx,v $
  Revision 1.1  2008/01/30 00:47:16  balewski
  Added L2-Etow-calib

 
 
*/


