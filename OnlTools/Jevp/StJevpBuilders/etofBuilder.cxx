#include <cstdio>
#include <cstdlib>
#include <cstdint>

#include "JevpBuilder.h"
#include "DAQ_READER/daqReader.h"
#include "DAQ_READER/daq_dta.h"
#include "DAQ_READER/daq_det.h"
#include <DAQ_ETOF/daq_etof.h>
#include <SFS/sfs_index.h>

#include "Jevp/StJevpPlot/RunStatus.h"
#include "StEvent/StTriggerData.h"
#include <TStyle.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TProfile.h>
#include <TF1.h>

#include <cmath>
#include <algorithm>
#include "etofBuilder.h"
#include <RTS/include/rtsLog.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include "etofMessageFormat.h"

#include <assert.h>



ClassImp( etofBuilder );

typedef JevpPlot* ptrJevpPlot;


etofBuilder::etofBuilder( JevpServer *parent ) : JevpBuilder( parent ) {
  plotsetname = ( char* ) "etof";

  int np = sizeof( contents ) / sizeof( TH1* );

  plots = new ptrJevpPlot[ np ]();

  // start with histograms undefined...
  memset( &contents, 0, sizeof( contents ) );
  memset( plots, 0, sizeof( JevpPlot* ) *np );
}

etofBuilder::~etofBuilder() {
  // Delete any existing histograms...
  int n = sizeof( contents ) / sizeof( TH1* );
  for( int i=0; i<n; i++ ) {
    if( plots[ i ] ) delete plots[ i ];
  }
  delete plots;
}

void etofBuilder::initialize( int argc, char* argv[] ) {

    LOG( NOTE, "etofBuilder::initialize" );

    nrOfGdpbInSys       = 12;
    nrOfGbtxPerGdpb     =  6;
    nrOfFeePerGbtx      =  5;
    nrOfGet4PerFee      =  8;
    nrOfChannelsPerGet4 =  4;

    nrOfChannelsPerFee  = nrOfChannelsPerGet4 * nrOfGet4PerFee;
    nrOfChannelsPerGbtx = nrOfChannelsPerFee  * nrOfFeePerGbtx;
    nrOfGet4PerGbtx     = nrOfGet4PerFee      * nrOfFeePerGbtx;
    nrOfChannelsPerGdpb = nrOfChannelsPerGet4 * nrOfGet4PerGbtx * nrOfGbtxPerGdpb;

    // map form id (hex number) of the GDPB to the running number ( 0 to nrOfGdpb-1 )
    gdpbMap2018[ 0x18f6 ] = 0;
    gdpbMap2018[ 0x0b59 ] = 1;
    gdpbMap2018[ 0x1898 ] = 2;
    gdpbMap2018[ 0x5f64 ] = 3;
    gdpbMap2018[ 0x18e6 ] = 4;

    gdpbMap2019[ 0x1970 ] = 0;
    gdpbMap2019[ 0x18f6 ] = 1;
    gdpbMap2019[ 0x0b59 ] = 2;
    gdpbMap2019[ 0x1898 ] = 3;
    gdpbMap2019[ 0x5bb0 ] = 4;
    gdpbMap2019[ 0x18e6 ] = 5;
    gdpbMap2019[ 0x6141 ] = 6;
    gdpbMap2019[ 0x0b5c ] = 7;
    gdpbMap2019[ 0x1861 ] = 8;
    gdpbMap2019[ 0x0b39 ] = 9;
    gdpbMap2019[ 0x5f71 ] = 10;
    gdpbMap2019[ 0x18b6 ] = 11;

    for ( auto kv : gdpbMap2018 ) {
        gdpbRevMap2018[  kv.second ] = kv.first;
    }
    for ( auto kv : gdpbMap2019 ) {
        gdpbRevMap2019[  kv.second ] = kv.first;
    }

    gdpbMap    = gdpbMap2019;
    gdpbRevMap = gdpbRevMap2019;


    vector<int> sector;
    sector.push_back( 13 );
    sector.push_back( 14 );
    sector.push_back( 15 );
    sector.push_back( 16 );
    sector.push_back( 17 );
    sector.push_back( 18 );
    sector.push_back( 19 );
    sector.push_back( 20 );
    sector.push_back( 21 );
    sector.push_back( 22 );
    sector.push_back( 23 );
    sector.push_back( 24 );

    // initialize the eLink to Get4 conversion
    int eLinkToGet4Array[ 40 ] = {
        27,  2,  7,  3, 31, 26, 30,  1,
        33, 37, 32, 13,  9, 14, 10, 15,
        17, 21, 16, 35, 34, 38, 25, 24,
         0,  6, 20, 23, 18, 22, 28,  4,
        29,  5, 19, 36, 39,  8, 12, 11 };


    for( size_t i = 0; i < nrOfGet4PerGbtx; i++ ) {
        eLinkToGet4.push_back( eLinkToGet4Array[ i ] );
        LOG( DBG, "elinkToGet4 (%d) = %d\n", i, eLinkToGet4[ i ] );
    }

    // initialize Get4 to PADI channel conversion
/*
    // 2018 version
    int get4ToPadiArray[ 32 ] = {
         3,  2,  1,  0, 
        23, 22, 21, 20,
         7,  6,  5,  4,
        27, 26, 25, 24,
        11, 10,  9,  8,
        31, 30, 29, 28,
        15, 14, 13, 12,
        19, 18, 17, 16 };
*/
    int get4ToPadiArray[ 32 ] = {
         3,  2,  1,  0,
         7,  6,  5,  4,
        11, 10,  9,  8,
        15, 14, 13, 12,
        19, 18, 17, 16,
        23, 22, 21, 20,
        27, 26, 25, 24,
        31, 30, 29, 28  };


    for( size_t i = 0; i < nrOfChannelsPerFee; i++ ) {
        get4ToPadi.push_back( get4ToPadiArray[ i ] );
        LOG( DBG, "get4ToPadi (%d) = %d\n", i, get4ToPadi[ i ] );
    }


    pulserPeakTot[ 13 ] = 98;
    pulserPeakTot[ 14 ] = 98;
    pulserPeakTot[ 15 ] = 98;
    pulserPeakTot[ 16 ] = 98;
    pulserPeakTot[ 17 ] = 98;
    pulserPeakTot[ 18 ] = 98;
    pulserPeakTot[ 19 ] = 98;
    pulserPeakTot[ 20 ] = 98;
    pulserPeakTot[ 21 ] = 98;
    pulserPeakTot[ 22 ] = 98;
    pulserPeakTot[ 23 ] = 98;
    pulserPeakTot[ 24 ] = 98;



    resetTimeLabel1Text = "";
    resetTimeLabel2Text = "";


    // Build Root Histograms...
    contents.nDigis             = new TH1D( "nDigis", "# digis inside timing window;# digis;# events", 300, 0, 600 );

    contents.nDigisVsTofTrgMult = new TH2D( "nDigisVsTofTrgMult", "# digis vs bTOF multiplicity;# digis in timing window;bTof mult in trigger data;# events", 200, 0, 800, 200, 0, 800 ); 

    contents.digiTot            = new TH1D( "digiTot",           "digi tot;TOT (bins);#digis", 256, 0, 256 );
    contents.digiTimeToTrigger  = new TH1D( "digiTimeToTrigger", "digi time to trigger;time to trigger (#mus);# digis", 500, -4., 1. );

    contents.digiCoarseTs       = new TH1D( "digiCoarseTs", "digi coarse Ts;coarse Ts (bins);# digis",  300, 0, 4800 );
    contents.digiFineTs         = new TH1D( "digiFineTs",   "digi fine Ts;fine Ts (bins);# digis",      112, 0,  112 );


    for( size_t i=0; i<nrOfGdpbInSys; i++ ) {
        contents.nDigisPerGdpb[ i ]                  = new TH1D( Form( "nDigisPerGdpb_%d", i ),  Form( "# digis on Gdpb %#06x (sector %d);# digis;# events",  gdpbRevMap[ i ], sector[ i ] ), 150, 0, 150 );
        contents.digiTotPerGdpb[ i ]                 = new TH1D( Form( "digiTotPerGdpb_%d", i ), Form( "digi tot on Gdpb %#06x (sector %d);TOT (bins);# digis", gdpbRevMap[ i ], sector[ i ] ), 256, 0, 256 );
        contents.digiTotPerGdpbInTimingWindow[ i ]   = new TH1D( Form( "digiTotPerGdpbInTimingWindow_%d", i ), Form( "digi tot on Gdpb %#06x (sector %d) in timing window;TOT (bins);# digis", gdpbRevMap[ i ], sector[ i ] ), 50, 0, 50 ); 
        contents.digiMappedChannelNumberPerGdpb[ i ] = new TH1D( Form( "digiMappedChannelNumberPerGdpb_%d", i), Form( "mapped channel number on Gdpb %#06x (sector %d);mapped channel;# digis", gdpbRevMap[ i ], sector[ i ] ), 965, -5, 960 );
        contents.nPulsersPerSide[ i ]                = new TH1D( Form( "nPulsersPerSide_%d", i ),  Form( "# pulsers on Gdpb %#06x (sector %d);# digis;# events",  gdpbRevMap[ i ], sector[ i ] ), 18, 0, 18 );

        contents.nPulsersPerSide[ i ]->GetXaxis()->SetLabelFont( 102 );

        contents.nPulsersPerSide[ i ]->GetXaxis()->SetBinLabel( 1, "1-1-1" );
        contents.nPulsersPerSide[ i ]->GetXaxis()->SetBinLabel( 2, "1-1-2" );
        contents.nPulsersPerSide[ i ]->GetXaxis()->SetBinLabel( 3, "1-2-1" );
        contents.nPulsersPerSide[ i ]->GetXaxis()->SetBinLabel( 4, "1-2-2" );
        contents.nPulsersPerSide[ i ]->GetXaxis()->SetBinLabel( 5, "1-3-1" );
        contents.nPulsersPerSide[ i ]->GetXaxis()->SetBinLabel( 6, "1-3-2" );

        contents.nPulsersPerSide[ i ]->GetXaxis()->SetBinLabel( 7, "2-1-1" );
        contents.nPulsersPerSide[ i ]->GetXaxis()->SetBinLabel( 8, "2-1-2" );
        contents.nPulsersPerSide[ i ]->GetXaxis()->SetBinLabel( 9, "2-2-1" );
        contents.nPulsersPerSide[ i ]->GetXaxis()->SetBinLabel( 10, "2-2-2" );
        contents.nPulsersPerSide[ i ]->GetXaxis()->SetBinLabel( 11, "2-3-1" );
        contents.nPulsersPerSide[ i ]->GetXaxis()->SetBinLabel( 12, "2-3-2" );

        contents.nPulsersPerSide[ i ]->GetXaxis()->SetBinLabel( 13, "3-1-1" );
        contents.nPulsersPerSide[ i ]->GetXaxis()->SetBinLabel( 14, "3-1-2" );
        contents.nPulsersPerSide[ i ]->GetXaxis()->SetBinLabel( 15, "3-2-1" );
        contents.nPulsersPerSide[ i ]->GetXaxis()->SetBinLabel( 16, "3-2-2" );
        contents.nPulsersPerSide[ i ]->GetXaxis()->SetBinLabel( 17, "3-3-1" );
        contents.nPulsersPerSide[ i ]->GetXaxis()->SetBinLabel( 18, "3-3-2" );

        contents.nPulsersPerSide[ i ]->SetMinimum( 0.9 );

        extras.nMissingPulsersPerSide[ i ] = new TH1D( Form( "nMissingPulsersPerSide_%d", i ), "", 18, 0, 18 );
        extras.nMissingPulsersPerSide[ i ]->SetLineColor( kRed );
    }



    contents.digiDensityInTimingWindow = new TH2D( "digiDensityInTimingWindow", "digi density in timing window;;(counter-1) * 32 + strip;# digis", 72, 0.5, 72.5, 96, 0.5, 96.5 );
    for( size_t i=0; i<12; i++ ) {
        contents.digiDensityInTimingWindow->GetXaxis()->SetBinLabel( i * 6 + 1, Form( "%d-1-1", i + 13 ) );
        contents.digiDensityInTimingWindow->GetXaxis()->SetBinLabel( i * 6 + 2, Form( "%d-1-2", i + 13 ) );
        contents.digiDensityInTimingWindow->GetXaxis()->SetBinLabel( i * 6 + 3, Form( "%d-2-1", i + 13 ) );
        contents.digiDensityInTimingWindow->GetXaxis()->SetBinLabel( i * 6 + 4, Form( "%d-2-2", i + 13 ) );
        contents.digiDensityInTimingWindow->GetXaxis()->SetBinLabel( i * 6 + 5, Form( "%d-3-1", i + 13 ) );
        contents.digiDensityInTimingWindow->GetXaxis()->SetBinLabel( i * 6 + 6, Form( "%d-3-2", i + 13 ) );
    }
    contents.digiDensityInTimingWindow->GetXaxis()->SetLabelSize(   0.03  );
    contents.digiDensityInTimingWindow->GetXaxis()->SetLabelOffset( 0.004 );
    contents.digiDensityInTimingWindow->GetXaxis()->SetLabelFont(   102   );
    contents.digiDensityInTimingWindow->GetXaxis()->SetTickLength(  0.01  );

    contents.digiDensityInTimingWindow->GetYaxis()->SetTitleSize(   0.05  );
    contents.digiDensityInTimingWindow->GetYaxis()->SetTitleOffset( 0.9   );
    contents.digiDensityInTimingWindow->GetYaxis()->CenterTitle(    true  );
    contents.digiDensityInTimingWindow->GetYaxis()->SetTickLength(  0.01  );

    contents.digiDensityInTimingWindow->GetZaxis()->SetTitleSize(   0.05  );
    contents.digiDensityInTimingWindow->GetZaxis()->SetTitleOffset( 0.95  );
    contents.digiDensityInTimingWindow->GetZaxis()->CenterTitle(    true  );
    contents.digiDensityInTimingWindow->GetZaxis()->SetTickLength(  0.01  );

    contents.digiDensityInTimingWindow->SetMinimum( 1. );

    contents.digiDensityAllChannels = (TH2D* ) contents.digiDensityInTimingWindow->Clone( "digiDensityAllChannels" );
    contents.digiDensityAllChannels->SetTitle( "digi density all;sector;(counter-1) * 32 + strip;# digis" );


    contents.digiDensityAllStrips = new TH2D( "digiDensityAllStrips", "digi density per strip;sector (module);(counter-1) * 32 + strip;# digis", 36, 0.5, 36.5, 96, 0.5, 96.5 );
    contents.digiDensityAllStrips->SetMinimum( 1. ); 
    for( size_t i=0; i<12; i++ ) {
        contents.digiDensityAllStrips->GetXaxis()->SetBinLabel( i * 3 + 2, Form( "%d", i + 13 ) );
    }
    contents.digiDensityAllStrips->GetXaxis()->LabelsOption( "h" );

    contents.digiDensityAllStrips->GetXaxis()->SetLabelSize(   0.055 );
    contents.digiDensityAllStrips->GetXaxis()->SetLabelOffset( 0.014 );
    contents.digiDensityAllStrips->GetXaxis()->SetTickLength(  0.01  );
    contents.digiDensityAllStrips->GetYaxis()->SetTitleOffset( 1.4   );

    contents.digiDensityAllStrips->GetYaxis()->SetTitleSize(   0.05  );
    contents.digiDensityAllStrips->GetYaxis()->SetTitleOffset( 0.9   );
    contents.digiDensityAllStrips->GetYaxis()->CenterTitle(    true  );
    contents.digiDensityAllStrips->GetYaxis()->SetTickLength(  0.01  );

    contents.digiDensityAllStrips->GetZaxis()->SetTitleSize(   0.05  );
    contents.digiDensityAllStrips->GetZaxis()->SetTitleOffset( 0.95  );
    contents.digiDensityAllStrips->GetZaxis()->CenterTitle(    true  );
    contents.digiDensityAllStrips->GetZaxis()->SetTickLength(  0.01  );   



    for( size_t i=0; i<12; i++ ) {
        contents.digiDensityAllChannels->GetXaxis()->SetBinLabel( i * 6 + 1, Form( "%d", i + 13 ) );
        contents.digiDensityAllChannels->GetXaxis()->SetBinLabel( i * 6 + 2, "" );
        contents.digiDensityAllChannels->GetXaxis()->SetBinLabel( i * 6 + 3, "" );
        contents.digiDensityAllChannels->GetXaxis()->SetBinLabel( i * 6 + 4, "" );
        contents.digiDensityAllChannels->GetXaxis()->SetBinLabel( i * 6 + 5, "" );
        contents.digiDensityAllChannels->GetXaxis()->SetBinLabel( i * 6 + 6, "" );
    }
    contents.digiDensityAllChannels->GetXaxis()->SetLabelSize(   0.045  );
    contents.digiDensityAllChannels->GetXaxis()->LabelsOption( "h" );
    contents.digiDensityAllChannels->GetXaxis()->SetLabelOffset( 0.004 );


    for( size_t i=0; i<11; i++ ) {
        contents.triggerTimeDiffSectors[ i ] = new TH1D( Form( "triggerTimeDiffSector13to%d", i+14 ), Form( "trigger time difference sector 13 - %d;# clock ticks;# events", i + 14 ), 200,  -99.5, 100.5 );
        extras.triggerTimeDiffSectorsOverflow[ i ] = new TH1D( Form( "triggerTimeDiffSector13to%dOverflow", i+14 ), Form( "trigger time difference sector 13 - %d;# clock ticks;# events", i + 14 ), 200,  -99.5, 100.5 );
        extras.triggerTimeDiffSectorsOverflow[ i ]->SetLineColor( kRed );
        extras.triggerTimeDiffSectorsOverflow[ i ]->SetFillColor( kRed );
    }

    for( size_t i=0; i<12; i++ ) {
        contents.resetTimeDiffSectors[ i ] = new TH1D( Form( "resetTimeDiffSector%dtoRef", i+13 ), Form( "reset time diff. sector %d to most freq.;# clock ticks;# events", i + 13 ), 21,  -10.5, 10.5 );
        extras.resetTimeDiffSectorsOverflow[ i ] = new TH1D( Form( "resetTimeDiffSector%dtoRefOverflow", i+13 ), Form( "reset time diff. sector %d to most freq.;# clock ticks;# events", i + 13 ), 21,  -10.5, 10.5 );
        extras.resetTimeDiffSectorsOverflow[ i ]->SetLineColor( kRed );
        extras.resetTimeDiffSectorsOverflow[ i ]->SetFillColor( kRed );
    }

    contents.missingTriggerTs       = new TH1D( "missingTriggerTs",       "missing trigger timestamps per sector;sector;# missing trigger timestamps", 12, 13, 25 );
    contents.triggerTimeToResetTime = new TH1D( "triggerTimeToResetTime", "trigger time to reset time;time difference (s);#events", 200, 0, 2000 );

    contents.missingTriggerTs->SetMinimum( 0.9 );
    contents.triggerTimeToResetTime->GetXaxis()->SetNdivisions( 4 );

    for( size_t i=0; i<216; i++ ) {
        int sec = (i / 18) + 13;
        int mod = (i % 18) / 6 + 1;
        int cou = (i %  6) / 2 + 1;
        int sid = (i %  2) + 1;
        contents.pulserDigiTimeDiff[ i ] = new TH1D( Form( "pulserDigiTimeDiff_%d", i ), Form( "sector %d module %d counter %d side %d to reference pulser;#Delta T (ns)", sec, mod, cou, sid ),               360, -540.5 * ( 6.25 / 112 ), 540.5 * ( 6.25 / 112 ) );
        extras.pulserDigiTimeDiffOverflow[ i ] = new TH1D( Form( "pulserDigiTimeDiffOverflow_%d", i ), Form( "sector %d module %d counter %d side %d to reference pulser;#Delta T (ns)", sec, mod, cou, sid ), 360, -540.5 * ( 6.25 / 112 ), 540.5 * ( 6.25 / 112 ) );
        extras.pulserDigiTimeDiffOverflow[ i ]->SetLineColor( kRed );
        extras.pulserDigiTimeDiffOverflow[ i ]->SetFillColor( kRed );
    }


    size_t np = sizeof( contents ) / sizeof( TH1* );

    for( size_t i=0; i<np; i++ ) {        
        contents.array[ i ]->SetLineColor( kBlue );

        JevpPlot* jp = new JevpPlot( contents.array[ i ] );
        jp->logy = 1;
        jp->setOptStat( 10 );


        if( contents.array[ i ] == contents.digiCoarseTs ||
            contents.array[ i ] == contents.digiFineTs   ) {
            jp->logy = 0;
        }

        if( contents.array[ i ] == contents.nDigisVsTofTrgMult ) {
            jp->logy = 0;

            multCorrLabel1 = new TLatex();
            multCorrLabel2 = new TLatex();
            multCorrLabel1->SetNDC();
            multCorrLabel2->SetNDC();
            multCorrLabel1->SetTextSize( 0.035 );
            multCorrLabel2->SetTextSize( 0.035 );
            jp->addElement( multCorrLabel1 );
            jp->addElement( multCorrLabel2 );
        }

        for( size_t j=0; j<12; j++ ) {
            if( contents.array[ i ] == contents.digiTotPerGdpbInTimingWindow[ j ] ) {
                jp->logy = 0;
            }
        }

        if( contents.array[ i ] == contents.digiDensityAllChannels    ||
            contents.array[ i ] == contents.digiDensityAllStrips      ||
            contents.array[ i ] == contents.digiDensityInTimingWindow  ) {
            jp->logy = 0;
            jp->optlogz = 1;
            jp->setOptStat( 0 );

            digiDensityLabel1 = new TLatex();
            digiDensityLabel2 = new TLatex();
            digiDensityLabel1->SetNDC();
            digiDensityLabel2->SetNDC();
            digiDensityLabel1->SetTextSize( 0.035 );
            digiDensityLabel2->SetTextSize( 0.035 );
            jp->addElement( digiDensityLabel1 );
            jp->addElement( digiDensityLabel2 );
        }

        if( contents.array[ i ] == contents.digiDensityAllStrips ) {

            TLatex* moduleLabel1[ 12 ];
            TLatex* moduleLabel2[ 12 ];
            TLatex* moduleLabel3[ 12 ];
            for( int k=0; k<12; k++ ) {
                moduleLabel1[ k ] = new TLatex( 3 * k + 0.7, -3, "1" );
                moduleLabel2[ k ] = new TLatex( 3 * k + 1.7, -3, "2" );
                moduleLabel3[ k ] = new TLatex( 3 * k + 2.7, -3, "3" );

                moduleLabel1[ k ]->SetTextSize( 0.0325 );
                moduleLabel2[ k ]->SetTextSize( 0.0325 );
                moduleLabel3[ k ]->SetTextSize( 0.0325 );

                moduleLabel1[ k ]->SetTextColor( kBlue+1 );
                moduleLabel2[ k ]->SetTextColor( kBlue+1 );
                moduleLabel3[ k ]->SetTextColor( kBlue+1 );

                jp->addElement( moduleLabel1[ k ] );
                jp->addElement( moduleLabel2[ k ] );
                jp->addElement( moduleLabel3[ k ] );
            }
        }

        for( size_t j=0; j<12; j++ ) {
            if( contents.array[ i ] == contents.nPulsersPerSide[ j ] ) {
                jp->setOptStat( 0 );
                jp->addHisto( extras.nMissingPulsersPerSide[ j ] );
            }
        }

        if( contents.array[ i ] == contents.nDigisVsTofTrgMult ) {
            jp->optlogz = 1;
        }

        for( size_t j=0; j<11; j++ ) {
            if( contents.array[ i ] == contents.triggerTimeDiffSectors[ j ] ) {
                jp->addHisto( extras.triggerTimeDiffSectorsOverflow[ j ] );
            }
        }
        for( size_t j=0; j<12; j++ ) {
            if( contents.array[ i ] == contents.resetTimeDiffSectors[ j ] ) {
                jp->addHisto( extras.resetTimeDiffSectorsOverflow[ j ] );
            }
        }
        for( size_t j=0; j<216; j++ ) {
            if( contents.array[ i ] == contents.pulserDigiTimeDiff[ j ] ) {
                jp->addHisto( extras.pulserDigiTimeDiffOverflow[ j ] );
            }
        }

        if( contents.array[ i ] == contents.triggerTimeToResetTime ) {
            jp->logy = 0;

            resetTimeLabel1 = new TLatex();
            resetTimeLabel2 = new TLatex();
            resetTimeLabel1->SetNDC();
            resetTimeLabel2->SetNDC();
            resetTimeLabel1->SetTextSize( 0.04 );
            resetTimeLabel2->SetTextSize( 0.04 );
            resetTimeLabel1->SetTextColor( kGreen+1 );
            resetTimeLabel2->SetTextColor( kGreen+1 );
            jp->addElement( resetTimeLabel1 );
            jp->addElement( resetTimeLabel2 );
        }

        addPlot( jp );
    }

    resetAllPlots();
}

void etofBuilder::event( daqReader *rdr ) {
    LOG( DBG, "-------------START EVENT----------" );
 
    float tofTrgMult = 0;
    StTriggerData* trgd = getStTriggerData( rdr );
    if( trgd ) {
        tofTrgMult = (float) trgd->tofMultiplicity( 0 );
    }

    daq_dta  *dd;
    daq_etof *etof = ( daq_etof* ) rdr->det( "etof" );
    dd = etof->get( "raw" );

    if( !dd ) {
        if( trgd ) delete trgd;
        return;
    }

    while ( dd->iterate() ) {

        unsigned int inputSizeBytes = dd->ncontent;
        unsigned int inputLength = inputSizeBytes / sizeof( uint64_t );

        LOG( DBG, "inputSizeBytes=%d", inputSizeBytes );
        LOG( DBG, "inputLength=%d = inputSizeBytes / %d", inputLength, sizeof( uint64_t ) );

        uint64_t* messageBuffer = static_cast< uint64_t* > ( dd->Void );

        uint64_t     trgGdpbFullTs   =   messageBuffer[ 0 ];
        uint64_t     trgStarFullTs   =   messageBuffer[ 1 ];
        unsigned int starToken       = ( messageBuffer[ 2 ] >> 32 ) & 0xFFF;
        unsigned int starDaqCmdIn    = ( messageBuffer[ 2 ] >> 16 ) & 0x00F;
        unsigned int starTrigCmdIn   = ( messageBuffer[ 2 ]       ) & 0x00F;
        uint64_t     eventStatusFlag =   messageBuffer[ 3 ];

        LOG( DBG, "trgGdpbFullTs=%lu",    trgGdpbFullTs   );
        LOG( DBG, "trgStarFullTs=%lu",    trgStarFullTs   );
        LOG( DBG, "starToken=%lu",        starToken       );
        LOG( DBG, "starDaqCmdIn=%lu",     starDaqCmdIn    );
        LOG( DBG, "starTrigCmdIn=%lu",    starTrigCmdIn   );
        LOG( DBG, "eventStatusFlag=%lu",  eventStatusFlag );

        // ------------------------------------------------------------------------------------------------------
        // determine number of full messages to read:
        // the packed version sent to the STAR DAQ systems is made of a 256 bit header (4 long unsigned integers)
        // followed by a buffer of gdpb::FullMessage (128 bit each).
        //
        // gdbp::Message     = 64 bit message as received from the eTOF gDPB boards
        // gdpb::FullMessage = 128 bit message, obtained by combining a
        //                     64 bit extended epoch (bits 127-64) with a gdpb::Message (bits 63-0)
        // ------------------------------------------------------------------------------------------------------

         // check if there are messages & process them
        if( inputLength > 4 ) {
            size_t nFullMessagesToRead = ( ( inputSizeBytes / sizeof( uint64_t ) ) - 4 ) / 2;

            if( year == 2018 ) {
                processMessages2018( messageBuffer, nFullMessagesToRead, tofTrgMult );
            }
            else{
                processMessages( messageBuffer, nFullMessagesToRead, tofTrgMult );
            }
        }
    } // dd->iterate

    if( trgd ) delete trgd;
}


void etofBuilder::startrun( daqReader *rdr ) {

  //  There can be events with no trigger data!
  //  in special runs!
  //
  //
  //year = (int) getStTriggerData( rdr )->year();
  year = ( rdr->run / 1000000 ) + 1999;
  LOG( DBG, "rdr->run=%d year=%d", rdr->run, year );
    
    if( year == 2018 ) {
        gdpbMap    = gdpbMap2018;
        gdpbRevMap = gdpbRevMap2018;

        for( size_t i = 0; i<nrOfGdpbInSys; i++ ) {
            if( i < gdpbRevMap.size() ) {
                contents.nDigisPerGdpb[ i ] ->SetTitle( Form( "# digis on Gdpb %#06x (sector %d);# digis;# events",  gdpbRevMap[ i ], 18 ) );
                contents.digiTotPerGdpb[ i ]->SetTitle( Form( "digi tot on Gdpb %#06x (sector %d);# digis;# events", gdpbRevMap[ i ], 18 ) );
                contents.digiMappedChannelNumberPerGdpb[ i ]->SetTitle( Form( "mapped channel number on Gdpb %#06x (sector %d);mapped channel;# digis", gdpbRevMap[ i ], 18 ) );
            }
            else {
                contents.nDigisPerGdpb[ i ]                 ->SetTitle( Form( "not existing ...;# digis;# events" ) );
                contents.digiTotPerGdpb[ i ]                ->SetTitle( Form( "not existing ...;# digis;# events" ) );
                contents.digiMappedChannelNumberPerGdpb[ i ]->SetTitle( Form( "not existing ...;# digis;# events" ) );
            }
        }
    }
    else {
        gdpbMap    = gdpbMap2019;
        gdpbRevMap = gdpbRevMap2019;
    }

    resetAllPlots();
}


void etofBuilder::stoprun( daqReader *rdr ) {
    //add label to multiplicity correlation plot
    char t1[ 256 ];
    char t2[ 256 ];

    int   nEvents = 0;
    float avDigis = 0.;

    int ybin = contents.nDigisVsTofTrgMult->GetYaxis()->FindBin( 300 );

    for( int i=0; i<contents.nDigisVsTofTrgMult->GetNbinsX(); i++ ) {
        
        for( int j=-2; j<2; j++ ) {
            int binCont = contents.nDigisVsTofTrgMult->GetBinContent( i+1, ybin + j );

            if( binCont > 0 ) {
                avDigis += binCont * contents.nDigisVsTofTrgMult->GetXaxis()->GetBinCenter( i+1 );
                nEvents += binCont;
            }
        }
    }
    if( nEvents > 0 ) {
        avDigis /= nEvents;
    }

    sprintf( t1, "av. # digis at 300 bTOF hits: %4.1f", avDigis );
    multCorrLabel1->SetText( 0.25, 0.80, t1 );

    TProfile* hprof = ( (TH2D*) contents.nDigisVsTofTrgMult )->ProfileX();
    TF1* f = new TF1( "f", "[ 0 ] * x + [ 1 ]", 25., 80. );
    hprof->Fit( "f", "RQN" );

    double par[ 2 ];
    f->GetParameters( par );

    sprintf( t2, "fitted slope: %3.2f", par[ 0 ] );
    multCorrLabel2->SetText( 0.25, 0.75, t2 );

    delete f;


    int nBinsXdigiDensity = contents.digiDensityInTimingWindow->GetNbinsX();
    int nBinsYdigiDensity = contents.digiDensityInTimingWindow->GetNbinsY();

    int threshold = 1;

    int nEmptyChannels = 0;
    std::vector< int > nEmptyChannelsCounter( 3 );

    for( int i=0; i<nBinsXdigiDensity; i++ ) {
        for( int j=0; j<nBinsYdigiDensity; j++ ) {
            if( contents.digiDensityInTimingWindow->GetBinContent( i+1, j+1 ) < threshold ) {
                nEmptyChannels++;
                nEmptyChannelsCounter.at( j / 32 )++;
            }
        }
    }

    int nEmptyStrips = 0;
    std::vector< int > nEmptyStripsCounter( 3 );

    for( int i=0; i<nBinsXdigiDensity-1; i=i+2 ) {
        for( int j=0; j<nBinsYdigiDensity; j++ ) {
            if( contents.digiDensityInTimingWindow->GetBinContent( i+1, j+1 ) < threshold || contents.digiDensityInTimingWindow->GetBinContent( i+2, j+1 ) < threshold ) {
                nEmptyStrips++;
                nEmptyStripsCounter.at( j / 32 )++;
            }
        }
    }

    //add label to digi density plot
    char t3[ 256 ];
    char t4[ 256 ];

    sprintf( t3, "empty channels: %d (%d - %d - %d)", nEmptyChannels, nEmptyChannelsCounter.at( 0 ), nEmptyChannelsCounter.at( 1 ), nEmptyChannelsCounter.at( 2 ) );
    sprintf( t4, "empty strips: %d (%d - %d - %d)",   nEmptyStrips,   nEmptyStripsCounter.at( 0 ),   nEmptyStripsCounter.at( 1 ),   nEmptyStripsCounter.at( 2 )   );
    
    digiDensityLabel1->SetText( 0.52, 0.96, t3 );
    digiDensityLabel2->SetText( 0.52, 0.92, t4 );
}


void etofBuilder::main( int argc, char *argv[] ) {
  etofBuilder me;

  me.Main( argc, argv );
}


void etofBuilder::processMessages( uint64_t* messageBuffer, size_t nFullMessagesToRead, float& tofTrgMult ) {
    LOG( DBG, " # of full messages to read: %d", nFullMessagesToRead );

    map< unsigned int, unsigned short > nDigisPerGdpb;
    for( const auto& kv: gdpbMap ) {
        nDigisPerGdpb[ kv.first ] = 0;
    }

    vector< vector< gdpbv100::FullMessage > > triggerMessages( gdpbMap.size() );

    for( size_t msgIndex = 0; msgIndex < nFullMessagesToRead; msgIndex++ ) {
        gdpbv100::FullMessage mess( messageBuffer[ 4 + 2 * msgIndex ], messageBuffer[ 4 + 2 * msgIndex + 1 ] );
        //mess.PrintMessage( gdpbv100::msg_print_Prefix | gdpbv100::msg_print_Data );

        if( mess.isStarTrigger() ) {
            unsigned int gdpb = mess.getGdpbGenGdpbId();                    

            if( gdpbMap.count( gdpb ) ) {
                triggerMessages.at( gdpbMap.at( gdpb ) ).push_back( mess );
            }
        }
    } // first message loop

    // deal with trigger and reset times of the different AFCKs
    map< short, uint64_t > gdpbTsMap;
    map< short, uint64_t > starTsMap;

    for( size_t i = 0; i < nrOfGdpbInSys; i++ ) {
        if( triggerMessages.at( i ).size() != 4 ) {
            contents.missingTriggerTs->Fill( i + 13 );
            continue;
        }

        uint64_t gdpbTsMsb = triggerMessages[ i ][ 0 ].getGdpbTsMsbStarA();
        uint64_t gdpbTsLsb = triggerMessages[ i ][ 1 ].getGdpbTsLsbStarB();
        uint64_t starTsMsb = triggerMessages[ i ][ 1 ].getStarTsMsbStarB();
        uint64_t starTsMid = triggerMessages[ i ][ 2 ].getStarTsMidStarC();

        uint64_t newGdpbTsFull = ( gdpbTsMsb << 24 )
                               + ( gdpbTsLsb       );

        uint64_t newStarTsFull = ( starTsMsb << 48 )
                               + ( starTsMid <<  8 )
                               + triggerMessages[ i ][ 3 ].getStarTsLsbStarD();

        gdpbTsMap[ i ] = newGdpbTsFull;
        starTsMap[ i ] = newStarTsFull;

        LOG( DBG, "gdpbTs(%d): %lld", i,  ( long long ) gdpbTsMap.at( i ) );
        LOG( DBG, "starTs(%d): %lld", i,  ( long long ) starTsMap.at( i ) );
    }

    if( gdpbTsMap.count( 0 ) ) {
        for( size_t i=0; i<11; i++ ) {
            if( gdpbTsMap.count( i+1 ) ) {
                int64_t diff = ( int64_t ) ( gdpbTsMap.at( 0 ) - gdpbTsMap.at( i+1 ) );

                contents.triggerTimeDiffSectors[ i ]->Fill( diff );
                if( diff > 100 ) {
                    extras.triggerTimeDiffSectorsOverflow[ i ]->AddBinContent( 199, 1 );
                    extras.triggerTimeDiffSectorsOverflow[ i ]->AddBinContent( 200, 1 );
                }
                if( diff < -99 ) {
                    extras.triggerTimeDiffSectorsOverflow[ i ]->AddBinContent( 1, 1 );
                    extras.triggerTimeDiffSectorsOverflow[ i ]->AddBinContent( 2, 1 );
                }
            }
        }
    }


    map< uint64_t, short > countsGdpbTs;
    for( const auto& kv : gdpbTsMap ) ++countsGdpbTs[ kv.second ];

    auto iterGdpb = std::max_element( countsGdpbTs.begin(), countsGdpbTs.end(),
                                      []( const pair< uint64_t, short >& p1, const pair< uint64_t, short >& p2 ) {
                                      return p1.second < p2.second; } );

    int64_t mostFrequentTriggerTs = 0;
    if( gdpbTsMap.size() > 0 ) {
        mostFrequentTriggerTs = iterGdpb->first;
    }



    map< uint64_t, short > countsStarTs;
    for( const auto& kv : starTsMap ) {
        if( kv.second == 0 ) continue;

        ++countsStarTs[ kv.second ];
    }

    int64_t mostFrequentResetTs  = 0;
    int     nCorrectResetSignals = 0;

    if( countsStarTs.size() > 0 ) {
        auto iterStar = std::max_element( countsStarTs.begin(), countsStarTs.end(),
                                        []( const pair< uint64_t, short >& p1, const pair< uint64_t, short >& p2 ) {
                                        return p1.second < p2.second; } );

        mostFrequentResetTs  = iterStar->first;
        nCorrectResetSignals = iterStar->second;
    }



    for( size_t i=0; i<12; i++ ) {
        if( starTsMap.count( i ) ) {
            int64_t diff = ( int64_t ) ( starTsMap.at( i ) - mostFrequentResetTs );
            
            if( starTsMap.at( i ) == 0 ) continue;

            contents.resetTimeDiffSectors[ i ]->Fill( diff );

            if( diff > 10 ) {
                extras.resetTimeDiffSectorsOverflow[ i ]->AddBinContent( 21, 1 );
            }
            else if( diff < -10 ) {
                extras.resetTimeDiffSectorsOverflow[ i ]->AddBinContent( 1, 1 ); 
            }
        }
    }


    double triggerTime    = mostFrequentTriggerTs * gdpbv100::kdClockCycleSizeNs;
    double triggerToReset = ( mostFrequentTriggerTs - mostFrequentResetTs ) * gdpbv100::kdClockCycleSizeNs * 1.e-9;

    contents.triggerTimeToResetTime->Fill( triggerToReset );


    char t1[ 256 ];
    char t2[ 256 ];

    sprintf( t1, "reset time stamp: %lld", ( long long int ) mostFrequentResetTs );
    sprintf( t2, "reported by %d sectors", nCorrectResetSignals );

    if( resetTimeLabel1Text != t1 ) {
        resetTimeLabel1Text = t1;
        resetTimeLabel2Text = t2;

        //turn label red if reset time is wrong
        if( mostFrequentResetTs == 0 || triggerToReset > 5000 ) {
            resetTimeLabel1->SetTextColor( kRed+1 );
            resetTimeLabel2->SetTextColor( kRed+1 );
        }
        resetTimeLabel1->SetText( 0.12, 0.90, resetTimeLabel1Text.c_str() );
        resetTimeLabel2->SetText( 0.12, 0.86, resetTimeLabel2Text.c_str() );
    }



    size_t nDigisInTimingWindow = 0;
    vector< vector< gdpbv100::FullMessage > > pulser( 216 );

    for( size_t msgIndex = 0; msgIndex < nFullMessagesToRead; msgIndex++ ) {
        gdpbv100::FullMessage mess( messageBuffer[ 4 + 2 * msgIndex ], messageBuffer[ 4 + 2 * msgIndex + 1 ] );
        //mess.PrintMessage( gdpbv100::msg_print_Prefix | gdpbv100::msg_print_Data );

        

        if( mess.isHitMsg() ) {
            unsigned int gdpb = mess.getGdpbGenGdpbId();
            unsigned int chip = mess.getGdpbGenChipId();
            unsigned int chan = mess.getGdpbHitChanId();

            // remove completely empty messages (128 bits of 0)
            if( gdpb == 0 && chip == 0 && chan == 0 ) {
                LOG( DBG, "empty message (?): %lu %lu", messageBuffer[ 4 + 2 * msgIndex ], messageBuffer[ 4 + 2 * msgIndex + 1 ] );
                continue;
            }

            // check if the reported chip && channel ids make sense
            if( gdpbMap.count( gdpb ) > 0 && chip < 240 && chan < 4 ) {

                if( mess.getGdpbHit32DllLck() != 1 ) {
                    LOG( DBG, " *** warning: DLL Lock = 0 " );
                }

                unsigned int mappedChannelNumber = hardwareMapChannelNumber( chip, chan );

                LOG( DBG, "--> hit message from AFCK %#06x with mappedChannelNr %d", gdpb, mappedChannelNumber );

                if( gdpbMap.count( gdpb ) ) {
                    contents.digiMappedChannelNumberPerGdpb[ gdpbMap.at( gdpb ) ]->Fill( mappedChannelNumber );
                }
                nDigisPerGdpb.at( gdpb ) += 1;

                unsigned int sector  = hardwareMapSector( gdpb );
                unsigned int module  = hardwareMapModule(  mappedChannelNumber );
                unsigned int counter = hardwareMapCounter( mappedChannelNumber );
                unsigned int strip   = hardwareMapStrip(   mappedChannelNumber );
                unsigned int side    = hardwareMapSide(    mappedChannelNumber );

                LOG( DBG, "--> hit message on sector %d module %d counter %d strip %d side %d", sector, module, counter, strip, side );

                contents.digiDensityAllChannels->Fill( ( sector - 13 ) * 6 + ( module - 1 ) * 2 + side, ( counter - 1 ) * 32 + strip );

                // calculate time difference to trigger 
                double digiFullTime  = mess.GetFullTimeNs();
                double timeToTrigger = digiFullTime - triggerTime;

                contents.digiTimeToTrigger->Fill( timeToTrigger / 1000. );

                unsigned int tot = mess.getGdpbHit32Tot();

                contents.digiTot->Fill( tot );

                if( gdpbMap.count( gdpb ) ) {
                    contents.digiTotPerGdpb[ gdpbMap.at( gdpb ) ]->Fill( tot );
                }

                bool isPulser = false;
                if( mappedChannelNumber % 32 == 0 ) {
                    if( timeToTrigger > -50. && timeToTrigger < 150. ) {
                        isPulser = true;
                    }

                    if( fabs( tot - pulserPeakTot.at( sector ) ) < 15 ) {
                        isPulser = true;
                    }
                }

                if( isPulser ) {
                    pulser.at( ( sector - 13 ) * 18 + ( module - 1 ) * 6 + ( counter - 1 ) * 2 + ( side - 1 ) ).push_back( mess );
                }
                else {
                    //digi inside timing window
                    if( fabs( timeToTrigger + 1845. ) < 150. ) {
                        nDigisInTimingWindow++;
                        contents.digiDensityInTimingWindow->Fill( ( sector - 13 ) * 6 + ( module - 1 ) * 2 + side, ( counter - 1 ) * 32 + strip );

                        if( gdpbMap.count( gdpb ) ) {
                            contents.digiTotPerGdpbInTimingWindow[ gdpbMap.at( gdpb ) ]->Fill( tot );
                        }
                    }

                    // cut out the pulser time window 
                    if( fabs( timeToTrigger - 50. ) > 200. ) {

                        // fine & coarse time
                        int fineTs   = mess.getGdpbHitFullTs() % 112;
                        int coarseTs = mess.getGdpbHitFullTs() / 112;

                        contents.digiFineTs  ->Fill( fineTs   );
                        contents.digiCoarseTs->Fill( coarseTs );
                    }
                }

            }
            else {
                LOG( DBG, "some id is out of range: gdpbId %#06x    chip %d    channel %d", gdpb, chip, chan );
                //mess.PrintMessage( gdpbv100::msg_print_Prefix | gdpbv100::msg_print_Data );
            }
        }
        else if( mess.isEpochMsg() ) {
            LOG( DBG, "EPOCH message" );
            //mess.PrintMessage( gdpbv100::msg_print_Prefix | gdpbv100::msg_print_Data );
        }
        else if( mess.isGet4SlCtrMsg() ) {
            LOG( DBG, "GET4 slow control message" );
            //mess.PrintMessage( gdpbv100::msg_print_Prefix | gdpbv100::msg_print_Data );
        }
        else if( mess.isSysMsg() ) {
            LOG( DBG, "SYSTEM message" );
            //mess.PrintMessage( gdpbv100::msg_print_Prefix | gdpbv100::msg_print_Data );
        }
    } // second message loop

    for( size_t i=0; i<nrOfGdpbInSys; i++ ) { 
        if( gdpbRevMap.count( i ) > 0 ) {
            contents.nDigisPerGdpb[ i ]->Fill( nDigisPerGdpb.at( gdpbRevMap.at( i ) ) );
        }
    }
    contents.nDigis->Fill( nDigisInTimingWindow );
    contents.nDigisVsTofTrgMult->Fill( nDigisInTimingWindow, tofTrgMult );


    
    map< short, double > pulserTimestamp;

    for( size_t i=0; i<216; i++ ) {
        if( pulser.at( i ).size() > 0 ) {

            double bestDiff = 100000.;
            int index = -1;

            int sector  = ( i / 18 ) + 13;
            int module  = ( i % 18 ) / 6 + 1;
            int counter = ( i %  6 ) / 2 + 1;
            int side    = ( i %  2 ) + 1;

            for( size_t j=0; j<pulser.at( i ).size(); j++ ) {
                double timeToTrigger = pulser.at( i ).at( j ).GetFullTimeNs() - triggerTime;

                double totToPeak = pulser.at( i ).at( j ).getGdpbHit32Tot() - pulserPeakTot.at( sector );

                if( pulser.at( i ).size() > 1 ) LOG( DBG, "%d pulsers @ %d-%d-%d %d: time to trigger: %5.2f  tot: %d", pulser.at( i ).size(), sector, module, counter, side, timeToTrigger, pulser.at( i ).at( j ).getGdpbHit32Tot() );

                
                // remove digis outside the "peak" region ... they are likely misidentified noise
                double currentDiff = fabs( timeToTrigger - 50. ) * 0.1 + fabs( totToPeak );
                if( currentDiff < bestDiff ) {
                    bestDiff = currentDiff;
                    index = j;
                }
            }
            if( pulser.at( i ).size() > 1 ) LOG( DBG, "-->selected index: %d", index );

            pulserTimestamp[ i ] = pulser.at( i ).at( index ).GetFullTimeNs();
            contents.nPulsersPerSide[ i / 18 ]->Fill( ( i % 18 ) );
        }
        else {
            extras.nMissingPulsersPerSide[ i / 18 ]->Fill( ( i % 18 ) );
        }
    }

    size_t nBinsPulserDiff = contents.pulserDigiTimeDiff[ 0 ]->GetNbinsX();
    float  min = contents.pulserDigiTimeDiff[ 0 ]->GetXaxis()->GetXmin();
    float  max = contents.pulserDigiTimeDiff[ 0 ]->GetXaxis()->GetXmax();

    int referencePulser = 7;

    for( size_t i=0; i<216; i++ ) {
        if( pulserTimestamp.count( referencePulser ) && pulserTimestamp.count( i ) ) {
            float diff = pulserTimestamp.at( i ) - pulserTimestamp.at( referencePulser );
            contents.pulserDigiTimeDiff[ i ]->Fill( diff );

            if( diff > max ) {
                for( size_t j=0; j<10; j++ ) {
                    extras.pulserDigiTimeDiffOverflow[ i ]->AddBinContent( nBinsPulserDiff - j, 1 );
                    LOG( DBG, "pulser channel %d with difference: %5.2f", i, diff );
                }
            }
            if( diff < min ) {
                for( size_t j=0; j<10; j++ ) {
                    extras.pulserDigiTimeDiffOverflow[ i ]->AddBinContent( j+1, 1 );
                    LOG( DBG, "pulser channel %d with difference: %5.2f", i, diff );
                }
            }
        }
    }


    // loop over modules
    for( int i=0; i<36; i++ ) {
        // loop over strips
        for( int j=0; j<96; j++ ) {

            int binContentSideOne = contents.digiDensityAllChannels->GetBinContent( ( i * 2 ) + 1, j + 1 );
            int binContentSideTwo = contents.digiDensityAllChannels->GetBinContent( ( i * 2 ) + 2, j + 1 );

            if( binContentSideOne && binContentSideTwo ) {
                contents.digiDensityAllStrips->SetBinContent( i + 1, j + 1 ,binContentSideOne + binContentSideTwo );
            }

        }
    }
}


unsigned int etofBuilder::hardwareMapElinkGet4Id( const unsigned int& chip ) {
    return eLinkToGet4.at( chip % nrOfGet4PerGbtx ) + nrOfGet4PerGbtx * ( chip / nrOfGet4PerGbtx );
}

unsigned int etofBuilder::hardwareMapChannelNumberInFee( const unsigned int& chip, const unsigned int& chan ) {
    return ( hardwareMapElinkGet4Id( chip ) % nrOfGet4PerFee ) * nrOfChannelsPerGet4 + chan;
}

unsigned int etofBuilder::hardwareMapFeeNumber( const unsigned int& chip ) {
    return hardwareMapElinkGet4Id( chip ) / nrOfGet4PerFee;
}

unsigned int etofBuilder::hardwareMapChannelNumber( const unsigned int& chip, const unsigned int& chan ) {
    return hardwareMapFeeNumber( chip ) * nrOfChannelsPerFee + get4ToPadi.at( hardwareMapChannelNumberInFee( chip, chan ) );
}

unsigned int etofBuilder::hardwareMapSector( const unsigned int& gdpb ) {
    if( gdpbMap.count( gdpb ) == 0 ) {
        return 0;
    }
    return gdpbMap.at( gdpb ) + 13;
}

unsigned int etofBuilder::hardwareMapModule( const unsigned int& mappedChannelNr ) {    
    return mappedChannelNr / ( 2 * nrOfChannelsPerGbtx ) + 1;
}

unsigned int etofBuilder::hardwareMapCounter( const unsigned int& mappedChannelNr ) {
    return ( ( mappedChannelNr % ( 2 * nrOfChannelsPerGbtx ) ) / nrOfChannelsPerFee ) % nrOfFeePerGbtx + 1;
}

unsigned int etofBuilder::hardwareMapSide( const unsigned int& mappedChannelNr ) {
    return ( mappedChannelNr % ( 2 * nrOfChannelsPerGbtx ) ) / nrOfChannelsPerGbtx + 1;
}

unsigned int etofBuilder::hardwareMapStrip( const unsigned int& mappedChannelNr ) {
    unsigned int strip = ( mappedChannelNr % ( 2 * nrOfChannelsPerGbtx ) ) % nrOfChannelsPerFee;

    if( hardwareMapSide( mappedChannelNr ) == 1 ) {
        return strip + 1;
    }
    else{
        return 32 - strip;
    }
}





void etofBuilder::processMessages2018( uint64_t* messageBuffer, size_t nFullMessagesToRead, float& tofTrgMult ) {
    LOG( DBG, " # of full messages to read: %d", nFullMessagesToRead );

    map< unsigned int, unsigned short > nDigisPerGdpb;
    for( const auto& kv: gdpbMap ) {
        nDigisPerGdpb[ kv.first ] = 0;
    }

    for( size_t msgIndex = 0; msgIndex < nFullMessagesToRead; msgIndex++ ) {
        gdpb::FullMessage mess( messageBuffer[4 + 2 * msgIndex], messageBuffer[4 + 2 * msgIndex + 1] );
        //mess.PrintMessage( gdpb::msg_print_Prefix | gdpb::msg_print_Data);

        if ( mess.isGet4Hit32Msg() ){
            unsigned int gdpb = mess.getRocNumber();
            unsigned int chip = mess.getGdpbGenChipId();
            unsigned int chan = mess.getGdpbHitChanId();

            // check if the reported gdpb, chip and channel ids make sense
            if( gdpbMap.count( gdpb ) > 0 && chip < 32 && chan < 4 ) {
                nDigisPerGdpb.at( gdpb ) += 1;

                contents.digiTot->Fill( mess.getGdpbHit32Tot() );

                if( gdpbMap.count( gdpb ) ) {
                    contents.digiTotPerGdpb[ gdpbMap.at( gdpb ) ]->Fill( mess.getGdpbHit32Tot() );
                }

                // calculate time difference to trigger 
                double triggerTime   = static_cast< double > ( messageBuffer[ 0 ] );
                double digiFullTime  = mess.GetFullTimeNs();
                double timeToTrigger = ( digiFullTime - triggerTime ) / 1000.;

                contents.digiTimeToTrigger->Fill( timeToTrigger );


                // do some mapping from gdpb, chip, chan to module, counter, side ...
                unsigned int channelNrInCard   = ( chip % nrOfGet4PerFee ) * nrOfChannelsPerGet4 + chan;
                unsigned int feeNr             = ( chip / nrOfGet4PerFee );

                unsigned int mappedChannelNumber = feeNr * nrOfChannelsPerFee + get4ToPadi[ channelNrInCard ];

                if( gdpbMap.count( gdpb ) ) {
                    contents.digiMappedChannelNumberPerGdpb[ gdpbMap.at( gdpb ) ]->Fill( mappedChannelNumber );
                }

                /*
                unsigned int strip = get4ToPadi.at( channelNrInCard );
                unsigned int side  = 0;

                if( feeNr % 2 == 1) {
                    strip = 31 - strip;
                    side  = 1;
                }

                unsigned int counterIndex;                      
                if( feeNr / 2 == 0 ) {
                    if( gdpbMap.at( gdpb ) == 0 )      counterIndex = 2;
                    else if( gdpbMap.at( gdpb ) == 1 ) counterIndex = 0;
                    else if( gdpbMap.at( gdpb ) == 2 ) counterIndex = 5;
                    else if( gdpbMap.at( gdpb ) == 3 ) counterIndex = 8;
                    else if( gdpbMap.at( gdpb ) == 4 ) counterIndex = 6;
                }
                else {
                    if( gdpbMap.at( gdpb ) == 0 )      counterIndex = 1;
                    else if( gdpbMap.at( gdpb ) == 1 ) counterIndex = 3;
                    else if( gdpbMap.at( gdpb ) == 2 ) counterIndex = 4;
                    else if( gdpbMap.at( gdpb ) == 3 ) counterIndex = 7;
                }
                */

                // fine & coarse time
                int fineTs   = mess.getGdpbHitFullTs() % 112;
                int coarseTs = mess.getGdpbHitFullTs() / 112;

                contents.digiFineTs  ->Fill( fineTs   );
                contents.digiCoarseTs->Fill( coarseTs );
            }
            else {
                LOG( DBG, "some id is out of range: gdpbId %#06x    chip %d    channel %d", gdpb, chip, chan );
                //mess.PrintMessage( gdpb::msg_print_Prefix | gdpb::msg_print_Data);
            }
            
        } // end of things to do with hit message

        
        if ( mess.isEpoch2Msg() ){
            if ( mess.getEpoch2EpochMissmatch() ) {
                //contents.nEpochMismatch->Fill( gdpbMap.at( mess.getRocNumber() ) );
            }
            if ( mess.getEpoch2EpochLost() ) {
                //contents.nEpochLoss->Fill( gdpbMap.at( mess.getRocNumber() ) );
            }
            if ( mess.getEpoch2DataLost() ) {
                //contents.nEpochDataLoss->Fill( gdpbMap.at( mess.getRocNumber() ) );
            }
            if ( mess.getEpoch2Sync() ) {
                //contents.nEpochSync->Fill( gdpbMap.at( mess.getRocNumber() ) );
            }

        } // end of things to do with epoch2 message

    } // end message loop

    size_t nDigis = 0;
    for( size_t i=0; i<nrOfGdpbInSys; i++ ) { 
        if( gdpbRevMap.count( i ) > 0 ) {
            contents.nDigisPerGdpb[ i ]->Fill( nDigisPerGdpb.at( gdpbRevMap.at( i ) ) );

            nDigis += nDigisPerGdpb.at( gdpbRevMap.at( i ) );
        }
    }

    contents.nDigis->Fill( nDigis );
    contents.nDigisVsTofTrgMult->Fill( nDigis, tofTrgMult );
}
