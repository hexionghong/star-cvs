/***************************************************************************
 *
 * $Id: StMixerMaker.cxx,v 1.6 2000/03/15 22:23:53 pfachini Exp $
 *
 * Author: Patricia Fachini
 *
 ***************************************************************************
 *
 * Description: Maker for doing the embedding (mixing)
 *              
 ***************************************************************************
 *
 * $Log: StMixerMaker.cxx,v $
 * Revision 1.6  2000/03/15 22:23:53  pfachini
 * Now using only TPC_DB parameters
 *
 * Revision 1.5  2000/03/15 22:16:32  pfachini
 * *** empty log message ***
 *
 * Revision 1.4  2000/03/15 20:50:41  pfachini
 * The command line to write out the mixed events into a file is now commented
 *
 * Revision 1.3  2000/03/15 17:23:14  pfachini
 * Using now the Trs classes instead of having a 'local' copy
 *
 * Revision 1.2  2000/02/22 20:25:23  pfachini
 * *** empty log message ***
 *
 * Revision 1.1  2000/02/16 21:02:09  pfachini
 * First version StMixer
 *
 *
 **************************************************************************/

//////////////////////////////////////////////////////////////////////////
//
// You must select a data base initializer method
//#define TPC_DATABASE_PARAMETERS
#define ROOT_DATABASE_PARAMETERS
//
//////////////////////////////////////////////////////////////////////////

#include <stdio.h>      // For binary file input (the DAQ data file).
#include <string.h>     // For binary file input (the DAQ data file).
#include <sys/types.h>  // For binary file input (the DAQ data file).
#include <sys/stat.h>   // For binary file input (the DAQ data file).
#include <fcntl.h>      // For binary file input (the DAQ data file).

#include <iostream.h>
#include <unistd.h>    // needed for access()/sleep()
#include <fstream.h>
#include <math.h>
#include <string>
#include <algorithm>
#include <vector>
#include <list>
#include <utility>    // pair
#include <algorithm>  // min() max()
#include <ctime>      // time functions

#include "StMixerMaker.h"

// SCL
#include "StGlobals.hh"
#include "Randomize.h"
#include "StChain.h"
#include "St_DataSetIter.h"
#include "St_ObjectSet.h"

// DataBase Initialization
// Dave's Header file
#include "StTpcDb/StTpcDb.h"
#include "StTrsMaker/include/StTpcDbGeometry.hh"
#include "StTrsMaker/include/StTpcDbSlowControl.hh"
#include "StTrsMaker/include/StTpcDbElectronics.hh"

// processes
#include "StMixerFastDigitalSignalGenerator.hh"
#include "StTrsMaker/include/StTrsDigitalSignalGenerator.hh"
#include "StMixerEmbedding.hh"

// containers
#include "StTrsMaker/include/StTrsAnalogSignal.hh"
#include "StTrsMaker/include/StTrsSector.hh"
#include "StTrsMaker/include/StTrsDigitalSector.hh"
#include "StTrsMaker/include/StTpcElectronics.hh"

// outPut Data--decoder
#include "StTrsMaker/include/StTrsRawDataEvent.hh"
#include "StTrsMaker/include/StTrsDetectorReader.hh"
#include "StTrsMaker/include/StTrsZeroSuppressedReader.hh"
#include "StDaqLib/GENERIC/EventReader.hh"
#include "StSequence.hh"
//#include "StDaqLib/TPC/trans_table.hh"
#include "StTrsMaker/include/StTrsOstream.hh"

#include "StDAQMaker/StDAQReader.h"

#include "StDaqLib/TPC/trans_table.hh"

StDAQReader *daqr1;
StTPCReader *tpcr1;
StDAQReader *daqr2;
StTPCReader *tpcr2;
StTrsDetectorReader* tdr1;
StTrsDetectorReader* tdr2;
ZeroSuppressedReader* trsZsr;
ZeroSuppressedReader* trsZsr1;
ZeroSuppressedReader* trsZsr2;

ClassImp(StMixerMaker)

StMixerMaker::StMixerMaker(const char *name,char *kind1,char *kind2):
StMaker(name), 
gConfig1(kind1),
gConfig2(kind2),
mFirstSector(1),
mLastSector(24) 
{ /* nopt */ }

StMixerMaker::~StMixerMaker() { /* nopt */ }

int StMixerMaker::getSequences1(int sector,int row,int pad,int *nseq,StSequence **listOfSequences) {
    int nseqPrelim; 
    TPCSequence *listOfSequencesPrelim;
    tpcr1->getSequences(sector,row,pad,nseqPrelim,listOfSequencesPrelim);
    *nseq=nseqPrelim;
    *listOfSequences=(StSequence*)listOfSequencesPrelim;
    return kStOK;
}
int StMixerMaker::getSequences2(int sector,int row,int pad,int *nseq,StSequence **listOfSequences) {
    int nseqPrelim; 
    TPCSequence *listOfSequencesPrelim;
    tpcr2->getSequences(sector,row,pad,nseqPrelim,listOfSequencesPrelim);
    *nseq=nseqPrelim;
    *listOfSequences=(StSequence*)listOfSequencesPrelim;
    return kStOK;
}

int StMixerMaker::writeFile(char* file, int numEvents)
{
    mOutputFileName = file;
    mNumberOfEvents = numEvents;
    return kStOK;
}

Int_t StMixerMaker::Init() {

  // The global pointer to the Db is gStTpcDb and it should be created in the macro.  
  mGeometryDb = StTpcDbGeometry::instance(gStTpcDb);
  mElectronicsDb = StTpcDbElectronics::instance(gStTpcDb);
  mSlowControlDb = StTpcDbSlowControl::instance(gStTpcDb);

  mSector = new StTrsSector(mGeometryDb);
  mSector1 = new StTrsSector(mGeometryDb);
  mSector2 = new StTrsSector(mGeometryDb);
  mDigitalSignalGenerator =
	    StMixerFastDigitalSignalGenerator::instance(mElectronicsDb, mSector);

  mEmbedding = StMixerEmbedding::instance(mElectronicsDb, mSector, mSector1, mSector2);

  // Output is into an StTpcRawDataEvent* vector<StTrsDigitalSector*>
  // which is accessible via the StTrsUnpacker.  Initialize the pointers!
  mAllTheDataMixer=0;
  
  // Construct constant data sets.  This is what is passed downstream
  mAllTheDataMixer = new StTrsRawDataEvent(mGeometryDb->numberOfSectors());
  AddConst(new St_ObjectSet("MixerEvent"  , mAllTheDataMixer));

  // Stream Instantiation
  mOutputStreamMixer = new StTrsOstream(mOutputFileName,mNumberOfEvents,mGeometryDb);
  //cout << "StMixerMaker::Init()" << endl;
  return StMaker::Init();
}

Int_t StMixerMaker::Make() {
  
  //ofstream out_file1("file1.dat");
  //ofstream out_file2("file2.dat");
  //ofstream out_file1mixer("file1mixer.dat");
  //ofstream out_file2mixer("file2mixer.dat");
  
  // Make a digital Pad!
#ifndef ST_NO_TEMPLATE_DEF_ARGS
  vector<unsigned char> digitalPadData;
#else
  vector<unsigned char, allocator<unsigned char> > digitalPadData;
#endif
  
  if(!strcmp(GetConfig1(),"daq")) {
    // DAQ
    St_DataSet *dataset1;
    dataset1=GetDataSet("Input1");
    //dataset1=GetDataSet("mixer/.make/DaqFirst/.const/StDAQReader");
    assert(dataset1);
    daqr1=(StDAQReader*)(dataset1->GetObject());
    assert(daqr1);
    tpcr1=daqr1->getTPCReader(); 
    assert(tpcr1);
  } else {
    // TRS
    // Get the TRS Event Data Set 
    //St_ObjectSet* trsEventDataSet1 = (St_ObjectSet*) GetDataSet("mixer/.make/TrsFirst/.const/Event"); 
    St_ObjectSet* trsEventDataSet1 = (St_ObjectSet*) GetDataSet("Input1"); 
    // Get the pointer to the raw data. 

    StTpcRawDataEvent* trsEvent1 = (StTpcRawDataEvent*) trsEventDataSet1->GetObject(); 
    // Instantiate the DetectorReader. Version will be default if not given 
    string version = "TrsDatav1.0"; 
    tdr1 = new StTrsDetectorReader(trsEvent1, version); 
   }
   
  if(!strcmp(GetConfig2(),"daq")) {
    // DAQ
    St_DataSet *dataset2;
    //dataset2=GetDataSet("mixer/.make/DaqSecond/.const/StDAQReader");
    dataset2=GetDataSet("Input2");
    assert(dataset2);
    daqr2=(StDAQReader*)(dataset2->GetObject());
    assert(daqr2);
    tpcr2=daqr2->getTPCReader(); 
    assert(tpcr2);
  } else {
    // TRS
    // Get the TRS Event Data Set 
    //St_ObjectSet* trsEventDataSet2 = (St_ObjectSet*) GetDataSet("mixer/.make/TrsSecond/.const/Event"); 
    St_ObjectSet* trsEventDataSet2 = (St_ObjectSet*) GetDataSet("Input2"); 
    // Get the pointer to the raw data. 
    StTpcRawDataEvent* trsEvent2 = (StTpcRawDataEvent*) trsEventDataSet2->GetObject(); 
    // Instantiate the DetectorReader. Version will be default if not given 
    string version = "TrsDatav1.0"; 
    tdr2 = new StTrsDetectorReader(trsEvent2, version); 
  }
  
  for(int isector=mFirstSector; isector<=mLastSector; isector++) {

    if((!strcmp(GetConfig1(),"daq") && !strcmp(GetConfig2(),"trs")) || (!strcmp(GetConfig1(),"trs") && !strcmp(GetConfig2(),"daq"))){

     if(!strcmp(GetConfig1(),"trs")) {
       trsZsr = tdr1->getZeroSuppressedReader(isector);
     } 
     if(!strcmp(GetConfig2(),"trs")) {
       trsZsr = tdr2->getZeroSuppressedReader(isector);
     }
      unsigned char* padListRaw; 
      unsigned char* padListTrs; 
      // Remember mSector is the "normal" analog sector! 
      for(int irow=0; irow<mSector->numberOfRows(); irow++) { 
	int numberOfPadsRaw;
	if(!strcmp(GetConfig1(),"daq")) {
	  numberOfPadsRaw = tpcr1->getPadList(isector,irow+1,padListRaw);
	}
	if(!strcmp(GetConfig2(),"daq")) {
	  numberOfPadsRaw = tpcr2->getPadList(isector,irow+1,padListRaw);
	}
	if (!trsZsr) {
	  if (numberOfPadsRaw) {
	    int padraw;
	    for(int ipadraw=0; ipadraw<numberOfPadsRaw; ipadraw++) {
	      padraw=padListRaw[ipadraw];
	      int nseqraw;
	      StSequence* listOfSequencesRaw; 
	      getSequences1(isector,irow+1,padraw,&nseqraw,&listOfSequencesRaw); 
	      // Note that ipad is an index, NOT the pad number. 
	      // The pad number comes from padList[ipad] 
	      for(int iseqraw=0;iseqraw<nseqraw;iseqraw++) {
		int startTimeBinRaw=listOfSequencesRaw[iseqraw].startTimeBin;
		if(startTimeBinRaw<0) startTimeBinRaw=0;
		if(startTimeBinRaw>511) startTimeBinRaw=511;
		int seqLenRaw=listOfSequencesRaw[iseqraw].length;
		unsigned char *pointerToAdcRaw=listOfSequencesRaw[iseqraw].firstAdc;
		for(int ibinraw=startTimeBinRaw;ibinraw<(startTimeBinRaw+seqLenRaw);ibinraw++) {
		  float conversionraw=log8to10_table[*(pointerToAdcRaw++)];
		  //if (isector==21) out_file1 << "file1" << ' ' << irow+1 << ' ' << padraw << ' ' << ibinraw << ' ' << conversionraw  << '\n';
		  StTrsAnalogSignal padSignal(ibinraw,conversionraw);
		  mSector->addEntry(irow+1,padraw,padSignal);
		}
	      }// seq loop    
	    }// pad loop, don't confuse padR (table row #) with ipad (loop index)
	  }
	} else {
	  int numberOfPadsTrs = trsZsr->getPadList(irow+1, &padListTrs);
	  if (numberOfPadsRaw) {
	    int padraw;
	    for(int ipadraw=0; ipadraw<numberOfPadsRaw; ipadraw++) {
	      padraw=padListRaw[ipadraw];
	      int nseqraw;
	      StSequence* listOfSequencesRaw; 
	      getSequences1(isector,irow+1,padraw,&nseqraw,&listOfSequencesRaw); 
	      // Note that ipad is an index, NOT the pad number. 
	      // The pad number comes from padList[ipad] 
	      for(int iseqraw=0;iseqraw<nseqraw;iseqraw++) {
		int startTimeBinRaw=listOfSequencesRaw[iseqraw].startTimeBin;
		if(startTimeBinRaw<0) startTimeBinRaw=0;
		if(startTimeBinRaw>511) startTimeBinRaw=511;
		int seqLenRaw=listOfSequencesRaw[iseqraw].length;
		unsigned char *pointerToAdcRaw=listOfSequencesRaw[iseqraw].firstAdc;
		for(int ibinraw=startTimeBinRaw;ibinraw<(startTimeBinRaw+seqLenRaw);ibinraw++) {
		  float conversionraw=log8to10_table[*(pointerToAdcRaw++)];
		  //if (isector==21) out_file1 << "file1" << ' ' << irow+1 << ' ' << padraw << ' ' << ibinraw << ' ' << conversionraw  << '\n';
		  if(numberOfPadsTrs) {
		    StTrsAnalogSignal padSignal(ibinraw,conversionraw);
		    mSector1->addEntry(irow+1,padraw,padSignal);
		    //out_file1mixer << "file1mixer" << ' ' << irow+1 << ' ' << padraw << ' ' << ibinraw << ' ' << conversionraw  << '\n';
		  } else {
		    StTrsAnalogSignal padSignal(ibinraw,conversionraw);
		    mSector->addEntry(irow+1,padraw,padSignal);
		  }
		}
	      }// seq loop    
	    }// pad loop, don't confuse padR (table row #) with ipad (loop index)
	  } 
	  if (numberOfPadsTrs) {
	    int padtrs;
	    for(int ipadtrs = 0; ipadtrs<numberOfPadsTrs; ipadtrs++) { 
	      padtrs=static_cast<int>(padListTrs[ipadtrs]);
	      int nseqtrs; 
	      //cout << irow+1 << ' ' << padtrs << endl;
	      Sequence* listOfSequencesTrs; 
	      trsZsr->getSequences(irow+1,padtrs,&nseqtrs,&listOfSequencesTrs); 	  // Note that ipad is an index, NOT the pad number. 
	      // The pad number comes from padList[ipad] 
	      for(int iseqtrs=0; iseqtrs<nseqtrs; iseqtrs++) { 
		int seqLenTrs=listOfSequencesTrs[iseqtrs].Length;
		int startTimeBinTrs=listOfSequencesTrs[iseqtrs].startTimeBin;
		if(startTimeBinTrs<0) startTimeBinTrs=0;
		if(startTimeBinTrs>511) startTimeBinTrs=511;
		for(int ibintrs=startTimeBinTrs; ibintrs<(startTimeBinTrs+seqLenTrs); ibintrs++) {
		  float conversiontrs=static_cast<float>(*(listOfSequencesTrs[iseqtrs].FirstAdc)); 
		  listOfSequencesTrs[iseqtrs].FirstAdc++; 
		  //out_file2 << "file2" << ' ' << irow+1 << ' ' << padtrs << ' ' << ibintrs << ' ' << conversiontrs  << endl;
		  if(numberOfPadsRaw) {
		    StTrsAnalogSignal padSignal(ibintrs,conversiontrs);
		    mSector2->addEntry(irow+1,padtrs,padSignal);
		    //out_file2mixer << "file2mixer" << ' ' << irow+1 << ' ' << padtrs << ' ' << ibintrs << ' ' << conversiontrs  << endl;
		  } else {
		    //if (isector==21 && (irow+1)==3 && padtrs==54) cout << irow+1 << ' ' << padtrs << ' ' << ibintrs << ' ' << conversiontrs <<endl;  
		    StTrsAnalogSignal padSignal(ibintrs,conversiontrs);
		    mSector->addEntry(irow+1,padtrs,padSignal);
		  }
		}
	      }// seq loop
	    }// pad loop, don't confuse padR (table row #) with ipad (loop index)
	  }
	}// ipadrow loop
      }
    }// if loop

    if((!strcmp(GetConfig1(),"trs") && !strcmp(GetConfig2(),"trs"))) {
      
      trsZsr1 = tdr1->getZeroSuppressedReader(isector); 
      trsZsr2 = tdr2->getZeroSuppressedReader(isector); 
      //cout << trsZsr1 << ' ' << trsZsr2 << ' ' << isector <<endl;
      if (!trsZsr1 && !trsZsr2) continue;
      unsigned char* padListTrs1; 
      unsigned char* padListTrs2; 
      // Remember mSector is the "normal" analog sector! 
      for(int irow=0; irow<mSector->numberOfRows(); irow++) {
	if (!trsZsr1 || !trsZsr2) {
	  if (!trsZsr1) {
	    int numberOfPadsTrs2 = trsZsr2->getPadList(irow+1, &padListTrs2);
	    if (numberOfPadsTrs2) {
	      int padtrs;
	      for(int ipadtrs = 0; ipadtrs<numberOfPadsTrs2; ipadtrs++) { 
		padtrs=static_cast<int>(padListTrs2[ipadtrs]);
		int nseqtrs; 
		Sequence* listOfSequencesTrs; 
		trsZsr2->getSequences(irow+1,padtrs,&nseqtrs,&listOfSequencesTrs);
		// Note that ipad is an index, NOT the pad number. 
		// The pad number comes from padList[ipad] 
		for(int iseqtrs=0; iseqtrs<nseqtrs; iseqtrs++) { 
		  int seqLenTrs=listOfSequencesTrs[iseqtrs].Length;
		  int startTimeBinTrs=listOfSequencesTrs[iseqtrs].startTimeBin;
		  if(startTimeBinTrs<0) startTimeBinTrs=0;
		  if(startTimeBinTrs>511) startTimeBinTrs=511;
		  for(int ibintrs=startTimeBinTrs; ibintrs<(startTimeBinTrs+seqLenTrs); ibintrs++) {
		    float conversiontrs=static_cast<float>(*(listOfSequencesTrs[iseqtrs].FirstAdc)); 
		    listOfSequencesTrs[iseqtrs].FirstAdc++; 
		    //out_file1mixer << "file1mixer" << ' ' << irow+1 << ' ' << padtrs << ' ' << ibintrs << ' ' << conversiontrs  << endl;
		    StTrsAnalogSignal padSignal(ibintrs,conversiontrs);
		    mSector->addEntry(irow+1,padtrs,padSignal);
		  }
		}// seq loop
	      }// pad loop, don't confuse padR (table row #) with ipad (loop index)
	    }
	  } 
	  if (!trsZsr2) {
	    int numberOfPadsTrs1 = trsZsr1->getPadList(irow+1, &padListTrs1);
	    if (numberOfPadsTrs1) {
	      int padtrs;
	      for(int ipadtrs = 0; ipadtrs<numberOfPadsTrs1; ipadtrs++) { 
		padtrs=static_cast<int>(padListTrs1[ipadtrs]);
		int nseqtrs; 
		Sequence* listOfSequencesTrs; 
		trsZsr1->getSequences(irow+1,padtrs,&nseqtrs,&listOfSequencesTrs);
		// Note that ipad is an index, NOT the pad number. 
		// The pad number comes from padList[ipad] 
		for(int iseqtrs=0; iseqtrs<nseqtrs; iseqtrs++) { 
		  int seqLenTrs=listOfSequencesTrs[iseqtrs].Length;
		  int startTimeBinTrs=listOfSequencesTrs[iseqtrs].startTimeBin;
		  if(startTimeBinTrs<0) startTimeBinTrs=0;
		  if(startTimeBinTrs>511) startTimeBinTrs=511;
		  for(int ibintrs=startTimeBinTrs; ibintrs<(startTimeBinTrs+seqLenTrs); ibintrs++) {
		    float conversiontrs=static_cast<float>(*(listOfSequencesTrs[iseqtrs].FirstAdc)); 
		    listOfSequencesTrs[iseqtrs].FirstAdc++; 
		    //out_file2mixer << "file2mixer" << ' ' << irow+1 << ' ' << padtrs << ' ' << ibintrs << ' ' << conversiontrs  << endl;
		    StTrsAnalogSignal padSignal(ibintrs,conversiontrs);
		    mSector->addEntry(irow+1,padtrs,padSignal);
		  }
		}// seq loop
	      }// pad loop, don't confuse padR (table row #) with ipad (loop index)
	    }
	  } 
	}
	if (trsZsr1 && trsZsr2) {
	  int numberOfPadsTrs1 = trsZsr1->getPadList(irow+1, &padListTrs1);
	  int numberOfPadsTrs2 = trsZsr2->getPadList(irow+1, &padListTrs2);
	  //cout << numberOfPadsTrs1 << ' ' << numberOfPadsTrs2 << endl;
	  if (numberOfPadsTrs1) {
	    int padtrs;
	    for(int ipadtrs = 0; ipadtrs<numberOfPadsTrs1; ipadtrs++) { 
	      padtrs=static_cast<int>(padListTrs1[ipadtrs]);
	      int nseqtrs; 
	      Sequence* listOfSequencesTrs;
	      //cout << irow+1 << ' ' << padtrs << endl;
	      trsZsr1->getSequences(irow+1,padtrs,&nseqtrs,&listOfSequencesTrs); 
	      // Note that ipad is an index, NOT the pad number. 
	      // The pad number comes from padList[ipad] 
	      for(int iseqtrs=0; iseqtrs<nseqtrs; iseqtrs++) { 
		int seqLenTrs=listOfSequencesTrs[iseqtrs].Length;
		int startTimeBinTrs=listOfSequencesTrs[iseqtrs].startTimeBin;
		if(startTimeBinTrs<0) startTimeBinTrs=0;
		if(startTimeBinTrs>511) startTimeBinTrs=511;
		for(int ibintrs=startTimeBinTrs; ibintrs<(startTimeBinTrs+seqLenTrs); ibintrs++) {
		  float conversiontrs=static_cast<float>(*(listOfSequencesTrs[iseqtrs].FirstAdc)); 
		  listOfSequencesTrs[iseqtrs].FirstAdc++; 
		  if(numberOfPadsTrs2) {
		    //if (isector==21 && (irow+1)==3 && padtrs==54) cout << irow+1 << ' ' << padtrs << ' ' << ibintrs << ' ' << conversiontrs <<endl;  
		    StTrsAnalogSignal padSignal(ibintrs,conversiontrs);
		    mSector1->addEntry(irow+1,padtrs,padSignal);
		    //if (isector==21) out_file1mixer << "file1mixer" << ' ' << irow+1 << ' ' << padtrs << ' ' << ibintrs << ' ' << conversiontrs  << endl;
		  } else {
		    StTrsAnalogSignal padSignal(ibintrs,conversiontrs);
		    mSector->addEntry(irow+1,padtrs,padSignal);
		  }
		}
	      }// seq loop
	    }// pad loop, don't confuse padR (table row #) with ipad (loop index)
	  }
	  if (numberOfPadsTrs2) {
	    int padtrs;
	    for(int ipadtrs = 0; ipadtrs<numberOfPadsTrs2; ipadtrs++) { 
	      padtrs=static_cast<int>(padListTrs2[ipadtrs]);
	      int nseqtrs; 
	      Sequence* listOfSequencesTrs; 
	      trsZsr2->getSequences(irow+1,padtrs,&nseqtrs,&listOfSequencesTrs); 	  // Note that ipad is an index, NOT the pad number. 
	      // The pad number comes from padList[ipad] 
	      for(int iseqtrs=0; iseqtrs<nseqtrs; iseqtrs++) { 
		int seqLenTrs=listOfSequencesTrs[iseqtrs].Length;
		int startTimeBinTrs=listOfSequencesTrs[iseqtrs].startTimeBin;
		if(startTimeBinTrs<0) startTimeBinTrs=0;
		if(startTimeBinTrs>511) startTimeBinTrs=511;
		for(int ibintrs=startTimeBinTrs; ibintrs<(startTimeBinTrs+seqLenTrs); ibintrs++) {
		  float conversiontrs=static_cast<float>(*(listOfSequencesTrs[iseqtrs].FirstAdc)); 
		  listOfSequencesTrs[iseqtrs].FirstAdc++; 
		  if(numberOfPadsTrs1) {
		    StTrsAnalogSignal padSignal(ibintrs,conversiontrs);
		    mSector2->addEntry(irow+1,padtrs,padSignal);
		    //if (isector==21) out_file1mixer << "file1mixer" << ' ' << irow+1 << ' ' << padtrs << ' ' << ibintrs << ' ' << conversiontrs  << endl;
		  } else {
		    StTrsAnalogSignal padSignal(ibintrs,conversiontrs);
		    mSector->addEntry(irow+1,padtrs,padSignal);
		  }
		}
	      }// seq loop
	    }// pad loop, don't confuse padR (table row #) with ipad (loop index)
	  }
	}
      }// ipadrow loop
    }// if loop
    
    if(!strcmp(GetConfig1(),"daq") && !strcmp(GetConfig2(),"daq")) {
      
      unsigned char* padListRaw1; 
      unsigned char* padListRaw2; 
      // Remember mSector is the "normal" analog sector! 
      for(int irow=0; irow<mSector->numberOfRows(); irow++) { 
	int numberOfPadsRaw1 = tpcr1->getPadList(isector,irow+1,padListRaw1);
	int numberOfPadsRaw2 = tpcr2->getPadList(isector,irow+1,padListRaw2);
	if (!numberOfPadsRaw1 && !numberOfPadsRaw2) continue;
	if (numberOfPadsRaw1) {
	  int padraw;
	  for(int ipadraw=0; ipadraw<numberOfPadsRaw1; ipadraw++) {
	    padraw=padListRaw1[ipadraw];
	    int nseqraw;
	    StSequence* listOfSequencesRaw; 
	    getSequences1(isector,irow+1,padraw,&nseqraw,&listOfSequencesRaw); 
	    // Note that ipad is an index, NOT the pad number. 
	    // The pad number comes from padList[ipad] 
	    for(int iseqraw=0;iseqraw<nseqraw;iseqraw++) {
	      int startTimeBinRaw=listOfSequencesRaw[iseqraw].startTimeBin;
	      if(startTimeBinRaw<0) startTimeBinRaw=0;
	      if(startTimeBinRaw>511) startTimeBinRaw=511;
	      int seqLenRaw=listOfSequencesRaw[iseqraw].length;
	      unsigned char *pointerToAdcRaw=listOfSequencesRaw[iseqraw].firstAdc;
	      for(int ibinraw=startTimeBinRaw;ibinraw<(startTimeBinRaw+seqLenRaw);ibinraw++) {
		float conversionraw=log8to10_table[*(pointerToAdcRaw++)];
		if (numberOfPadsRaw2) {
		  //if (isector==21) out_file1 << "file1" << ' ' << irow+1 << ' ' << padraw << ' ' << ibinraw << ' ' << conversionraw  << '\n';
		  StTrsAnalogSignal padSignal(ibinraw,conversionraw);
		  mSector1->addEntry(irow+1,padraw,padSignal);
		} else {
		  StTrsAnalogSignal padSignal(ibinraw,conversionraw);
		  mSector->addEntry(irow+1,padraw,padSignal);
		}
	      }
	    }// seq loop    
	  }// pad loop, don't confuse padR (table row #) with ipad (loop index)
	}
	if (numberOfPadsRaw2) {
	  int padraw;
	  for(int ipadraw=0; ipadraw<numberOfPadsRaw2; ipadraw++) {
	    padraw=padListRaw2[ipadraw];
	    int nseqraw;
	    StSequence* listOfSequencesRaw; 
	    getSequences2(isector,irow+1,padraw,&nseqraw,&listOfSequencesRaw); 
	    // Note that ipad is an index, NOT the pad number. 
	    // The pad number comes from padList[ipad] 
	    for(int iseqraw=0;iseqraw<nseqraw;iseqraw++) {
	      int startTimeBinRaw=listOfSequencesRaw[iseqraw].startTimeBin;
	      if(startTimeBinRaw<0) startTimeBinRaw=0;
	      if(startTimeBinRaw>511) startTimeBinRaw=511;
	      int seqLenRaw=listOfSequencesRaw[iseqraw].length;
	      unsigned char *pointerToAdcRaw=listOfSequencesRaw[iseqraw].firstAdc;
	      for(int ibinraw=startTimeBinRaw;ibinraw<(startTimeBinRaw+seqLenRaw);ibinraw++) {
		float conversionraw=log8to10_table[*(pointerToAdcRaw++)];
		//cout << isector << ' ' << irow+1 << ' ' << padraw << ' ' << ibinraw << ' ' << conversionraw  << '\n';
		if (numberOfPadsRaw1) {
		  StTrsAnalogSignal padSignal(ibinraw,conversionraw);
		  mSector2->addEntry(irow+1,padraw,padSignal);
		} else {
		  StTrsAnalogSignal padSignal(ibinraw,conversionraw);
		  mSector->addEntry(irow+1,padraw,padSignal);
		}
	      }
	    }// seq loop    
	  }// pad loop, don't confuse padR (table row #) with ipad (loop index)
	}
      }// ipadrow loop
    }// if loop
    
    mEmbedding->doEmbedding();
    cout << "StMixerEmbedding::doEmbedding() - Sector = " << ' ' << isector << endl;
    
    // Digitize the Signals
    //
    // First make a sector where the data can go...
    StTrsDigitalSector* aDigitalSector = new StTrsDigitalSector(mGeometryDb);
    
    // Point to the object you want to fill
    //
    mDigitalSignalGenerator->fillSector(aDigitalSector);
    
    //cout << "sector" << ' ' << isector+1 << endl;
    // ...and digitize it
    //cout << "--->digitizeSignal()..." << endl;
    mDigitalSignalGenerator->digitizeSignal();
    //cout<<"--->digitizeSignal() Finished..." << endl;
    
    // Fill it into the event structure...
    // and you better check the sector number!
    
    mAllTheDataMixer->mSectors[(isector-1)] = aDigitalSector;
    // Clear and reset for next sector:
    mSector->clear();
    mSector1->clear();
    mSector2->clear();
    
  }// sector loop

  // Uncomment this line if you want to write the mixed events in a file.
  //mOutputStreamMixer->writeTrsEvent((mAllTheDataMixer));
  return kStOK;
} // Make() 

// Make sure the memory is deallocated!
Int_t StMixerMaker::Clear()
{
  //mAllTheDataMixer->clear(); //This deletes all the StTrsDigitalSectors in the StTrsRawDataEvent
  return kStOk;
}
Int_t StMixerMaker::Finish()
{
  //Clean up all the pointers that were initialized in StTrsMaker::Init()
  //if(mOutputStreamMixer) delete mOutputStream;
  return kStOK;
}

