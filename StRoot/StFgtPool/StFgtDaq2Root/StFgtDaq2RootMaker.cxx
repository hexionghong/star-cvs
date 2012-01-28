/***************************************************************************
 *
 * $Id: StFgtDaq2RootMaker.cxx,v 1.1 2012/01/28 09:29:26 sgliske Exp $
 * Author: S. Gliske, Jan 2012
 *
 ***************************************************************************
 *
 * Description: see header.
 *
 ***************************************************************************
 *
 * $Log: StFgtDaq2RootMaker.cxx,v $
 * Revision 1.1  2012/01/28 09:29:26  sgliske
 * creation
 *
 *
 **************************************************************************/

#include "StFgtDaq2RootMaker.h"
#include <TFile.h>
#include <TTree.h>

#include "StRoot/StEvent/StEvent.h"
#include "StRoot/StEvent/StFgtCollection.h"
#include "StRoot/StEvent/StFgtStrip.h"

// constructors
StFgtDaq2RootMaker::StFgtDaq2RootMaker( const Char_t* name,
                                        const Char_t* outputfile )
   : StMaker( name ),
     mFileName( outputfile ),
     mTFile(0),
     mTTree(0) { /* */ };

// deconstructor
StFgtDaq2RootMaker::~StFgtDaq2RootMaker(){
//    if( mTFile ){
//       mTFile->Close();
//       delete mTFile;
//    };
};

Int_t StFgtDaq2RootMaker::Init(){
   Int_t ierr = kStOk;

   // set the output
   LOG_INFO << "Opening file '" << mFileName << "' for output" << endm;
   mTFile = new TFile( mFileName.data(), "RECREATE", "raw fgt data from the cosmic test stand" );
   if( !mTFile->IsOpen() ){
      LOG_FATAL << "error opening file '" << mFileName << "'" << endm;
      ierr = kStFatal;
   };

   if( !ierr ){
      mTTree = new TTree ( "fgtTree", "raw fgt data from the cosmic test stand" );
      mTTree->Branch( "branch", &mData, "data[26880]/I" );
   };

   return ierr;
};

void StFgtDaq2RootMaker::Clear(const Option_t* opts ){
   for( Int_t quad = 0; quad < kNumQuad; ++quad )
      for( Int_t apv = 0; apv < kNumApv; ++apv )
         for( Int_t chan = 0; chan < kNumApv; ++chan )
            for( Int_t tb = 0; tb < kNumApv; ++tb )
               mData.quad[quad].apv[apv].chan[chan].tb[tb] = 0;
};


Int_t StFgtDaq2RootMaker::Make(){
   Int_t ierr = kStOk;

   StEvent* eventPtr = 0;
   eventPtr = (StEvent*)GetInputDS("StEvent");

   if( !eventPtr ) {
      LOG_ERROR << "Error getting pointer to StEvent from '" << ClassName() << "'" << endm;
      ierr = kStErr;
   };

   StFgtCollection* fgtCollectionPtr = 0;

   if( eventPtr ) {
      fgtCollectionPtr=eventPtr->fgtCollection();
   };

   if( !fgtCollectionPtr) {
      LOG_ERROR << "Error getting pointer to StFgtCollection from '" << ClassName() << "'" << endm;
      ierr = kStErr;
   };

   if( !ierr ){
      for( UInt_t discIdx=0; discIdx<fgtCollectionPtr->getNumDiscs(); ++discIdx ){
         StFgtStripCollection *stripCollectionPtr = fgtCollectionPtr->getStripCollection( discIdx );
         if( stripCollectionPtr ){
            StSPtrVecFgtStrip& stripVec = stripCollectionPtr->getStripVec();
            StSPtrVecFgtStripIterator stripIter;

            for( stripIter = stripVec.begin(); stripIter != stripVec.end(); ++stripIter ){
               Short_t adc = (*stripIter)->getAdc();
               Int_t tb = (*stripIter)->getTimeBin();
               Int_t rdo, arm, apv, chan, quad;
               (*stripIter)->getElecCoords( rdo, arm, apv, chan );

               if( arm == 0 )
                  quad = 0;
               else if( apv < 10 )
                  quad = 1;
               else
                  quad = 2;
               apv %= 12;

               mData.quad[quad].apv[apv].chan[chan].tb[tb] = adc;
            };
         };
      };
   };

   mTTree->Fill();

   return ierr;
};

Int_t StFgtDaq2RootMaker::Finish(){
   mTFile->Write();
   mTFile->Close();

   return kStOk;
};

ClassImp(StFgtDaq2RootMaker);
