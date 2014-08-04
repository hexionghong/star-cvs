/***************************************************************************
*
* $Id: StIstRawHitMaker.cxx,v 1.16 2014/08/04 17:12:48 ypwang Exp $
*
* Author: Yaping Wang, March 2013
****************************************************************************
* Description: 
* See header file.
****************************************************************************
*
* $Log: StIstRawHitMaker.cxx,v $
* Revision 1.16  2014/08/04 17:12:48  ypwang
* update chip status Db table obtain method due to the table is populated run-by-run
*
* Revision 1.15  2014/07/29 20:13:31  ypwang
* update the IST DB obtain method
*
* Revision 1.14  2014/04/15 06:47:00  ypwang
* updates for collections clear due to Clear() function removed from StIstCollection
*
* Revision 1.13  2014/04/14 02:45:56  ypwang
* update LOG_ERROR to LOG_WARN for the case when real time bin number does not equal to the value from DB
*
* Revision 1.12  2014/03/25 03:06:53  ypwang
* updates on Db table accessory method
*
* Revision 1.11  2014/03/24 15:55:08  ypwang
* minor updates due to returned const pointers in StIstDbMaker
*
* Revision 1.10  2014/03/18 02:45:19  ypwang
* update raw hit decision algorithm: removed 1st time bin restriction cut
*
* Revision 1.9  2014/02/25 01:08:30  smirnovd
* Explicit pointer type conversion
*
* Revision 1.8  2014/02/25 01:07:02  smirnovd
* Minor pointer initialization and declaration fixes
*
* Revision 1.7  2014/02/19 06:26:14  ypwang
* update raw hit decision cuts to be compatible to ZS and non-ZS data
*
* Revision 1.6  2014/02/18 07:57:09  ypwang
* add setDefaultTimeBin() while filling raw hits information
*
* Revision 1.5  2014/02/15 19:55:25  ypwang
* remove virtual type declaration from member function
*
* Revision 1.4  2014/02/08 03:34:17  ypwang
* updating scripts
*
*
****************************************************************************
* StIstRawHitMaker.cxx,v 1.0
* Revision 1.0 2013/11/04 15:55:30 Yaping
* Initial version
****************************************************************************/

#include "StIstRawHitMaker.h"

#include "StEvent.h"
#include "StRoot/St_base/StMessMgr.h"
#include "RTS/src/DAQ_FGT/daq_fgt.h"
#include "RTS/src/DAQ_READER/daq_dta.h"
#include "StChain/StRtsTable.h"

#include "StRoot/StIstUtil/StIstCollection.h"
#include "StRoot/StIstUtil/StIstRawHitCollection.h"
#include "StRoot/StIstUtil/StIstRawHit.h"
#include "StRoot/StIstDbMaker/StIstDb.h"
#include "StRoot/StIstUtil/StIstConsts.h"

#include "tables/St_istPedNoise_Table.h"
#include "tables/St_istGain_Table.h"
#include "tables/St_istMapping_Table.h"
#include "tables/St_istControl_Table.h"
#include "tables/St_istChipConfig_Table.h"

#include <string.h>
#include <time.h>

StIstRawHitMaker::StIstRawHitMaker( const char* name ): StRTSBaseMaker( "ist", name ), mIsCaliMode(0), mDoCmnCorrection(0), mIstCollectionPtr(0), mIstDb(0), mDataType(2){
   // set all vectors to zeros
   mCmnVec.resize( kIstNumApvs );
   mPedVec.resize( kIstNumElecIds );
   mRmsVec.resize( kIstNumElecIds );
   mGainVec.resize( kIstNumElecIds );
   mMappingVec.resize( kIstNumElecIds );
   mConfigVec.resize( kIstNumApvs, 1 );
};

StIstRawHitMaker::~StIstRawHitMaker(){
   // clear vectors
   mCmnVec.clear();
   mPedVec.clear();
   mRmsVec.clear();
   mGainVec.clear();
   mMappingVec.clear();
   mConfigVec.clear();
};

Int_t StIstRawHitMaker::Init(){
   LOG_INFO << "Initializing StIstRawHitMaker ..." << endm;
   Int_t ierr = kStOk;

   //prepare output data collection
   m_DataSet = new TObjectSet("istRawHitAndCluster");

   mIstCollectionPtr = new StIstCollection();
   ((TObjectSet*) m_DataSet)->AddObject(mIstCollectionPtr);

   if( ierr || !mIstCollectionPtr ) {
      LOG_WARN << "Error constructing istCollection" << endm;
      ierr = kStWarn;
   }

   return ierr;
};

Int_t StIstRawHitMaker::InitRun(Int_t runnumber) {
   Int_t ierr = kStOk;

   TObjectSet *istDbDataSet = (TObjectSet *)GetDataSet("ist_db");
   if (istDbDataSet) {
       mIstDb = (StIstDb *)istDbDataSet->GetObject();
       assert(mIstDb);
   }
   else {
       LOG_ERROR << "InitRun : no istDb" << endm;
       return kStErr;
   }

   // IST control parameters
   const istControl_st *istControlTable = mIstDb->GetControl() ;
   if (!istControlTable)  {
       	LOG_ERROR << "Pointer to IST control table is null" << endm;
       	ierr = kStErr;
   }
   else {
       	mHitCut  = istControlTable[0].kIstHitCutDefault;
       	mCmnCut  = istControlTable[0].kIstCMNCutDefault;
       	mChanMinRmsNoiseLevel = istControlTable[0].kIstChanMinRmsNoiseLevel;
       	mChanMaxRmsNoiseLevel = istControlTable[0].kIstChanMaxRmsNoiseLevel;
       	mApvMaxCmNoiseLevel   = istControlTable[0].kIstApvMaxCmNoiseLevel;
       	mALLdata = istControlTable[0].kIstAlldata;
       	mADCdata = istControlTable[0].kIstADCdata;
       	mZSdata  = istControlTable[0].kIstZSdata;
       	mDefaultTimeBin = istControlTable[0].kIstDefaultTimeBin;
       	mCurrentTimeBinNum = istControlTable[0].kIstCurrentTimeBinNum;
   }

   // IST pedestal/rms table
   const istPedNoise_st *gPN = mIstDb->GetPedNoise();
   if( !gPN ) {
	LOG_ERROR << "Pointer to IST pedestal/noise table is null" << endm;
	    ierr = kStErr;
   }
   else {
   	for(int i=0; i<kIstNumApvs; i++) {
        	LOG_DEBUG<<Form(" Print entry %d : CM noise=%f ",i,(float)gPN[0].cmNoise[i]/100.)<<endm;
		mCmnVec[i] = (float)gPN[0].cmNoise[i]/100.0;
   	}
   	for(int i=0; i<kIstNumElecIds; i++) {
        	LOG_DEBUG<<Form(" Print entry %d : pedestal=%f ",i,(float)gPN[0].pedestal[i])<<endm;
		mPedVec[i] = (float)gPN[0].pedestal[i];
   	}
   	for(int i=0; i<kIstNumElecIds; i++) {
        	LOG_DEBUG<<Form(" Print entry %d : RMS noise=%f ",i,(float)gPN[0].rmsNoise[i]/100.)<<endm;
		mRmsVec[i] = (float)gPN[0].rmsNoise[i]/100.;
   	}
   }

   // IST gain table
   const istGain_st *gG = mIstDb->GetGain();
   if( !gG ) {
	LOG_WARN << "Pointer to IST gain table is null" << endm;
        ierr = kStWarn;
   }
   else {
   	for(int i=0; i<kIstNumElecIds; i++) {
        	LOG_DEBUG<<Form(" Print entry %d : gain=%f ",i,(float)gG[0].gain[i])<<endm;
		mGainVec[i] = (float)gG[0].gain[i];
   	}
   }

   // IST mapping table
   const istMapping_st *gM = mIstDb->GetMapping();
   if( !gM ) {
       	LOG_ERROR << "Pointer to IST mapping table is null" << endm;
	ierr = kStErr;
   }
   else {
        for(int i=0; i<kIstNumElecIds; i++) {
             	LOG_DEBUG<<Form(" Print entry %d : geoId=%d ",i,gM[0].mapping[i])<<endm;
                mMappingVec[i] = gM[0].mapping[i];
        }
   }

   // IST chip configuration status table
   const istChipConfig_st *gCS = mIstDb->GetChipStatus();
   if( !gCS ) {
        LOG_ERROR << "Pointer to IST chip configuration table is null" << endm;
        ierr = kStErr;
   }
   else {
	if(runnumber == gCS[0].run) {
	     for(int i=0; i<kIstNumApvs; i++) {
                        LOG_DEBUG<<Form(" Print entry %d : status=%d ", i, gCS[0].s[i])<<endm;
                        mConfigVec[i] = gCS[0].s[i];  /* 0 dead;  1 good; 2-9 reserved good status; 10 mis-configured */
             }
	}
   }

   return ierr; 
};

Int_t StIstRawHitMaker::Make() {
    Int_t ierr = kStOk;

    if( !ierr ){
	StRtsTable* rts_tbl = 0;
	UChar_t dataFlag = mALLdata;
        static Int_t ntimebin = mCurrentTimeBinNum;

	while(1) { //loops over input raw data
	    if(dataFlag==mALLdata){
            	if(mDataType==mALLdata){
              	    rts_tbl = GetNextDaqElement("ist/zs"); 	dataFlag=mZSdata;
                    if(!rts_tbl) { 
			LOG_WARN << "NO ZS-DATA BANK FOUND!!!" << endm;
			rts_tbl = GetNextDaqElement("ist/adc"); dataFlag=mADCdata; 
		    }
           	}
	   	else if(mDataType==mADCdata){
              	    rts_tbl = GetNextDaqElement("ist/adc"); 	dataFlag=mADCdata;
           	}
	   	else if(mDataType==mZSdata){
              	    rts_tbl = GetNextDaqElement("ist/zs");  	dataFlag=mZSdata;
           	}
            }
            else if(dataFlag==mADCdata){ rts_tbl = GetNextDaqElement("ist/adc"); }
            else if(dataFlag==mZSdata){ rts_tbl = GetNextDaqElement("ist/zs"); }
            if(!rts_tbl) break;

            apv_meta_t *meta = (apv_meta_t *)rts_tbl->Meta();
	    if(meta){
            	for(int r=1; r<=kIstNumRdos; r++) {//6 rdos needed for whole IST detector
                    if(meta->arc[r].present == 0) continue ;
                    for(int arm=0; arm<kIstNumArmsPerRdo; arm++) {//6 arms per arc
                    	if(meta->arc[r].arm[arm].present == 0) continue ;
                    	for(int apv=0; apv<kIstNumApvsPerArm; apv++) { //24 apvs per arm
                            if(meta->arc[r].arm[arm].apv[apv].present == 0) continue ;
                            int nt = meta->arc[r].arm[arm].apv[apv].ntim;
                            if(ntimebin != 0 && nt != 0 && ntimebin != nt) 
			   	LOG_WARN << "Different number of timebins in different APV!!! Taking larger one!!!" << endm;
                            if(ntimebin < nt) 
			   	ntimebin = nt;
                    	}
                    }
            	}
            }
            mIstCollectionPtr->setNumTimeBins(ntimebin);

	    // arrays to store ADC information per APV chip (128 channels over all time bins)
	    Int_t signalUnCorrected[kIstNumApvChannels][ntimebin];    //signal w/o pedestal subtracted
            Float_t signalCorrected[kIstNumApvChannels][ntimebin];    //signal w/ pedestal subtracted
	    for(int l=0; l<kIstNumApvChannels; l++)    {
                for(int m=0; m<ntimebin; m++)    {
                    signalUnCorrected[l][m]  = 0;
                    signalCorrected[l][m]    = 0.;
                }
            }

	    // arrays to calculate dynamical common mode noise contribution to the APV chip in current event
            Float_t cmNoisePerChip = 0.;                              //common mode noise of the APV chip
	    Float_t sumAdcPerEvent[ntimebin];
            Int_t counterAdcPerEvent[ntimebin];
	    for(int n=0; n<ntimebin; n++)  {
                sumAdcPerEvent[n]     = 0.;
                counterAdcPerEvent[n] = 0 ;
            }

	    // electronics coordinate info.: RDO, ARM, APV
	    Int_t rdo = rts_tbl->Rdo();     // 1, 2, ..., 6
            Int_t arm = rts_tbl->Sector();  // 0, 1, ..., 5
            Int_t apv = rts_tbl->Pad();     // 0, 1, ..., 23

            Int_t flag=0;
            if(rdo<1     || rdo >  kIstNumRdos)                 flag=1;
            if(arm<0     || arm >= kIstNumArmsPerRdo)           flag=1;
            if(apv<0     || apv >= kIstNumApvsPerArm)           flag=1;
            if(flag==1) {
                LOG_INFO<< "Corrupt data  rdo: " << rdo << " arm: " << arm << " apv: " << apv <<endm;
                continue;
            }

            // Loop over the data in this APV to get raw hit info. (channel, timebin, adc)
            for(StRtsTable::iterator it=rts_tbl->begin();it!=rts_tbl->end();it++) {
		// channel info.                
		fgt_adc_t *f = (fgt_adc_t *)*it;
                Int_t channel   = f->ch;  //channel index  0, 1, ..., 127
                Int_t adc       = f->adc; //adc
                Short_t timebin = f->tb;  //time bin
		LOG_DEBUG << "channel: " << channel << "   adc: " << adc << "  time bin: " << timebin << endm;
 
                flag=0;
		if((dataFlag==mADCdata) && (adc<0 || adc>=kIstMaxAdc))	flag=1;
                if(channel<0 || channel>=kIstNumApvChannels)        	flag=1;
                if(timebin<0 || timebin>=ntimebin)           		flag=1;
                if(flag==1){
                    LOG_INFO << "Corrupt data channel: " << channel << " tbin: " << timebin << " adc: " << adc << endm;
                    continue;
                }

		signalUnCorrected[channel][timebin] = adc;
		if( !mIsCaliMode )        {
		    Int_t elecId = (rdo-1)*kIstNumArmsPerRdo*kIstNumApvsPerArm*kIstNumApvChannels + arm*kIstNumApvsPerArm*kIstNumApvChannels + apv*kIstNumApvChannels + channel; // 0, ..., 110591
                    if(elecId < 0 || elecId >= kIstNumElecIds){
                    	LOG_INFO<<"Wrong elecId: " << elecId  << endm;
                     	continue;
                    }

		    if( dataFlag==mADCdata ) { // non-ZS data
                    	signalCorrected[channel][timebin]    = (float)signalUnCorrected[channel][timebin] - mPedVec[elecId];
		    	// exclude signal-related channels for common mode noise calculation
		    	if( (signalCorrected[channel][timebin] > (-mCmnCut)*mRmsVec[elecId]) && ( signalCorrected[channel][timebin] < mCmnCut*mRmsVec[elecId] ) )     {
                    	    sumAdcPerEvent[timebin] += signalCorrected[channel][timebin];
                    	    counterAdcPerEvent[timebin]++;
                    	}
		    }
		    else {	// ZS data
		    	signalCorrected[channel][timebin]    = (float)signalUnCorrected[channel][timebin];
		    }
		}
	    } // end current APV loops

	    // calculate the dynamical common mode noise for the current chip in this event
	    Float_t commonModeNoise[ntimebin];
            for(int tbIdx=0; tbIdx<ntimebin; tbIdx++)
                commonModeNoise[tbIdx] = 0.;

	    if( !mIsCaliMode && dataFlag==mADCdata ) {
            	for(short iTb=0; iTb<ntimebin; iTb++)  {
                    if(counterAdcPerEvent[iTb]>0)
                    	commonModeNoise[iTb] = sumAdcPerEvent[iTb]/counterAdcPerEvent[iTb];
            	}
	    }

	    // fill IST raw hits for current APV chip
	    for(int iChan=0; iChan<kIstNumApvChannels; iChan++) {	
		//mapping info.
		Int_t elecId = (rdo-1)*kIstNumArmsPerRdo*kIstNumApvsPerArm*kIstNumApvChannels + arm*kIstNumApvsPerArm*kIstNumApvChannels + apv*kIstNumApvChannels + iChan;
		Int_t geoId  = mMappingVec[elecId]; // channel geometry ID which is numbering from 1 to 110592
		Int_t ladder = 1 + (geoId - 1)/(kIstApvsPerLadder * kIstNumApvChannels); // ladder geometry ID: 1, 2, ..., 24
                Int_t apvId  = 1 + (geoId - 1)/kIstNumApvChannels; // APV geometry ID: 1, ..., 864 (numbering from ladder 1 to ladder 24)
		cmNoisePerChip = mCmnVec[apvId-1];

		//store raw hits information
                StIstRawHitCollection *rawHitCollectionPtr = mIstCollectionPtr->getRawHitCollection( ladder-1 );

		if( rawHitCollectionPtr ) {
		    if( mIsCaliMode ) { //calibration mode (non-ZS data): only write raw ADC value
			if(dataFlag==mADCdata) {
			    StIstRawHit* rawHitPtr = rawHitCollectionPtr->getRawHit( elecId );

                            for(int iTimeBin=0; iTimeBin<ntimebin; iTimeBin++) {
                            	rawHitPtr->setCharge( (float)signalUnCorrected[iChan][iTimeBin], (unsigned char)iTimeBin );
                            }
                            rawHitPtr->setChannelId( elecId );
			    rawHitPtr->setGeoId( geoId );
			}
			else continue;
		    }
		    else { //physics mode: pedestal subtracted + dynamical common mode correction
			//skip dead chips and bad mis-configured chips
			if(mConfigVec[apvId-1]<1 || mConfigVec[apvId-1]>9) { //1-9 good status code
			    LOG_DEBUG<< "Skip: Dead/mis-configured APV chip geometry index: " << apvId << " on ladder " << ladder << endm;
                            continue;
			}
			//comment out by Yaping on Jul. 29, 2014. The dead and mis-configured chips can be masked out by the chip status table.
			/*
			//skip current APV channels marked as bad/dead (common mode noise set to 100.)
                	if( cmNoisePerChip > 99.0) {
                    	    LOG_DEBUG<< "Skip: Bad/dead behavior APV chip geometry index: " << apvId << " on ladder " << ladder << endm;
                    	    continue;
                	}

			//skip current APV channels behaviored noisy ...
			if( cmNoisePerChip > mApvMaxCmNoiseLevel) {
                            LOG_DEBUG<< "Skip: Noisy behavior APV chip geometry index: " << apvId << " on ladder " << ladder << endm;
                            continue;
                        }
			*/
			//skip current channel marked as bad/dead status
			if(mRmsVec[elecId] > 99.0)  {
                            LOG_DEBUG<<"Skip: Bad/dead behavior channel electronics index: " << elecId << endm;
                            continue;
                        }

			//skip current channel marked as suspicious status
			if(mRmsVec[elecId]<mChanMinRmsNoiseLevel || mRmsVec[elecId]>mChanMaxRmsNoiseLevel)  {
                            LOG_DEBUG<<"Skip: Noisy behavior channel electronics index: " << elecId << endm;
                            continue;
                        }

                    	for(int iTB=1; iTB<ntimebin-1; iTB++)    {
                      	    // raw hit decision: the method is stolen from Gerrit's ARMdisplay.C
                            if( (signalUnCorrected[iChan][iTB] > 0) && (signalUnCorrected[iChan][iTB] < kIstMaxAdc) && 
                            	(signalCorrected[iChan][iTB-1] > mHitCut * mRmsVec[elecId])     &&
                            	(signalCorrected[iChan][iTB]   > mHitCut * mRmsVec[elecId])     &&
                            	(signalCorrected[iChan][iTB+1] > mHitCut * mRmsVec[elecId]) ) {

                            	iTB = 999;
			    	UChar_t tempMaxTB = -1;
			    	Float_t tempMaxCharge = -999.0;

			    	StIstRawHit* rawHitPtr = rawHitCollectionPtr->getRawHit( elecId );

			    	for(int iTBin=0; iTBin<ntimebin; iTBin++)      {
				    if( mDoCmnCorrection && dataFlag==mADCdata )
                               	    	signalCorrected[iChan][iTBin] -= commonModeNoise[iTBin];

                                    rawHitPtr->setCharge(signalCorrected[iChan][iTBin] * mGainVec[elecId], (unsigned char)iTBin );
                              	    rawHitPtr->setChargeErr(mRmsVec[elecId] * mGainVec[elecId], (unsigned char)iTBin);

				    if(signalCorrected[iChan][iTBin] > tempMaxCharge) {
				    	tempMaxCharge = signalCorrected[iChan][iTBin];
				    	tempMaxTB = (unsigned char)iTBin;
				    }
			    	}
                              	rawHitPtr->setChannelId( elecId );
				rawHitPtr->setGeoId( geoId );
				rawHitPtr->setMaxTimeBin( tempMaxTB );
				rawHitPtr->setDefaultTimeBin( mDefaultTimeBin );
                            }//end raw hit decision cut
                    	}//end loop over time bins
		    }//end filling hit info
		}
		else {
                    LOG_WARN << "StIstRawHitMaker::Make() -- Could not access rawHitCollection for ladder " << ladder << endm;
                }
            } //end single APV chip hits filling
      	}//end while
    }//end ierr cut
    
    return ierr;
};

void StIstRawHitMaker::Clear( Option_t *opts )
{
   if(mIstCollectionPtr ) {
	for ( unsigned char i = 0; i < kIstNumLadders; ++i ) {
	    mIstCollectionPtr->getRawHitCollection(i)->Clear( "" );
	}
   }
};

ClassImp(StIstRawHitMaker);
