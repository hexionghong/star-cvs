#include <stdio.h>
#include <stdlib.h>

#include "Jevp/StJevpPlot/JevpPlotSet.h"
#include "DAQ_READER/daqReader.h"
#include "DAQ_READER/daq_dta.h"
#include "DAQ_READER/daq_det.h"
#include <DAQ_SST/daq_sst.h> 

#include "Jevp/StJevpPlot/RunStatus.h"
#include "StEvent/StTriggerData.h"
#include <TH1I.h>
#include <TH2I.h>
#include <TFile.h>
#include <TPaveStats.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "ssdBuilder.h"
#include <RTS/include/rtsLog.h>

ClassImp(ssdBuilder);

ssdBuilder::ssdBuilder(JevpServer *parent):JevpPlotSet(parent),evtCt(0),evtTrue(0) {
  plotsetname = (char *)"ssd";
}

ssdBuilder::~ssdBuilder() 
{
  for (int ns=0;ns<nSide;ns++) 
    {
      for(int nl=0;nl<nLadderPerSide;nl++)
	{
	  if(hAdcStrip[ns][nl]) delete hAdcStrip[ns][nl];
	  if(hAdcEvent[ns][nl]) delete hAdcEvent[ns][nl];
	}    		
    }
  if(hLadderWafer[0]) delete hLadderWafer[0];
  if(hLadderWafer[1]) delete hLadderWafer[1];
}

//-------------------------
void ssdBuilder::initialize(int argc, char *argv[]) 
{
  errorMsg=0;
  char buffer[100];
  char buffer2[100];
  int merge  = 128;
  int nBinsX = nWaferPerLadder*nStripPerWafer;
  int nBinsY = 1024;
  mSector    = 0;
  mRDO       = 0;
  mSide      = 0;
  mFiber     = 0;
  mLadder    = 0;
  mWafer     = 0;
  mStrip     = 0;
  mAdc       = 0;
  mAdcLength = 0;
  //---------------------
  for ( int ns=0; ns<nSide; ns++ ) 
    {
      for(int nl=0;nl<nLadderPerSide;nl++)
	{
	  sprintf( buffer, "ADCStrip_%d_%d",ns,nl);
	  if(ns==0)
	    sprintf( buffer2, "ADC Vs. Strip number, P-side Ladder: %d",nl);
	  else 
	    sprintf( buffer2, "ADC Vs. Strip number, N-side Ladder: %d",nl);
    
	  hAdcStrip[ns][nl] = new TH2I(buffer, buffer2,nBinsX/merge,0,nBinsX,nBinsY/4,0,nBinsY);
	  hAdcStrip[ns][nl]->GetXaxis()->SetTitle("Strip #");
	  hAdcStrip[ns][nl]->GetYaxis()->SetTitle("ADC value");
	  hAdcStrip[ns][nl]->GetXaxis()->SetNdivisions(96,0,0,false);
	  //hAdcStrip[ns][nl]->GetXaxis()->SetNdivisions(nWaferPerLadder,false);
	  hAdcStrip[ns][nl]->SetStats(false);//true
	  //------
	  sprintf( buffer, "ADCEvent_%d_%d",ns,nl);
	  if(ns==0)
	    sprintf( buffer2, "ADC Vs. Event number, P-side Ladder: %d",nl);
	  else 
	    sprintf( buffer2, "ADC Vs. Event number, N-side Ladder: %d",nl);
          //hAdcStrip[ns][nlx]->GetYaxis()->SetTitleOffset(1.1);	  
	  hAdcEvent[ns][nl] = new TH2I(buffer, buffer2,12288,0,12288,nBinsY/4,0,nBinsY);
	  hAdcEvent[ns][nl]->GetXaxis()->SetTitle("Event #");
	  hAdcEvent[ns][nl]->GetYaxis()->SetTitle("ADC value");
	  hAdcEvent[ns][nl]->SetStats(false);//true
	  
	  //set labele
	  for ( int index=0; index<nWaferPerLadder; index++ ) {         
	    char label[nWaferPerLadder];
	    sprintf(label, "Wafer%d", index);
	    hAdcStrip[ns][nl]->GetXaxis()->SetBinLabel((index*nStripPerWafer+448)/merge, label);  
	  }
	}
    }
  hLadderWafer[0] = new TH2I("hLadderWaferP","P-side ladder vs wafer",20,0,20,16,0,16);
  hLadderWafer[0]->SetName("hLadderWaferP");
  hLadderWafer[0]->GetXaxis()->SetTitle("Ladder #");
  hLadderWafer[0]->GetYaxis()->SetTitle("Wafer #");
  hLadderWafer[0]->SetStats(false);
  hLadderWafer[1] = new TH2I("hLadderWaferN","N-side ladder vs wafer",20,0,20,16,0,16);
  hLadderWafer[1]->SetName("hLadderWaferN");
  hLadderWafer[1]->GetXaxis()->SetTitle("Ladder #");
  hLadderWafer[1]->GetYaxis()->SetTitle("Wafer #");
  hLadderWafer[1]->SetStats(false);
  //JEVP plots setting
  int totPlots = 2*nSide*nLadderPerSide+2;

  plots = new JevpPlot*[2*nSide*nLadderPerSide+2];

  for( int i=0;i<nSide;i++) 
    {
    for(int j=0;j<nLadderPerSide;j++)
      {
	hAdcStrip[i][j]->SetOption("colz");
	hAdcEvent[i][j]->SetOption("colz");
	
	plots[nLadderPerSide*i+j] = new JevpPlot(hAdcStrip[i][j]);
	plots[nLadderPerSide*i+j]->optlogz=true;
	plots[nSide*nLadderPerSide+nLadderPerSide*i+j] = new JevpPlot(hAdcEvent[i][j]);
	plots[nSide*nLadderPerSide+nLadderPerSide*i+j]->optlogz=true;
      }
    }
  
  //p-side

  hLadderWafer[0]->SetOption("COLZ");
  plots[2*nSide*nLadderPerSide+0] = new JevpPlot(hLadderWafer[0]);
  //addPlot(plots[2*nSide*nLadderPerSide+0]);
  //n-side
  
  hLadderWafer[1]->SetOption("COLZ");
  plots[2*nSide*nLadderPerSide+1] = new JevpPlot(hLadderWafer[1]);
  //addPlot(plots[2*nSide*nLadderPerSide+1]);
  
  //---------
  //add plots to plot set
  for ( int i=0; i<totPlots;i++ )
    {
      LOG(DBG, "Adding plot %d",i);
      addPlot(plots[i]);
    }
  /*
  errorMsg = new JLatex(.25, .12, "#color[4]{No Error Message}");
  errorMsg->SetTextSize(0.035);
  errorMsg->SetTextAlign(13);
  errorMsg->SetTextAngle(45);
  //plots[2*nSide][nLadderPerSide].addElement(errorMsg);
  plots[totPlots-1]->addElement(errorMsg);
  */
}

//-------------------------
void ssdBuilder::startrun(daqReader *rdr) 
{
  LOG ( DBG, "ssdBuilder starting run #%d", rdr->run );
  resetAllPlots();
  //errorMsg->SetText("No Error Message");    

}

#define safelog(x) ((x > 0) ? log10(x) : 0)

//-------------------------

void ssdBuilder::event(daqReader *rdr) {

  if ( !(evtCt %1000) )     LOG(DBG, "Looking at evt %d",evtCt);
  daq_dta *dd ;
  dd = rdr->det("sst")->get("adc");

  while(dd && dd->iterate()) 
    { 
    
      daq_sst_data_t *sst = (daq_sst_data_t *)dd->Void;
           
      mSector = dd->sec;
      
      if(mSector==1)
	mRDO = dd->rdo;
      else 
	mRDO = 3 + dd->rdo;
      
      mFiber = dd->pad;
     
      if ( mFiber < 0 || mFiber > 7 )        continue;      //fiber 0-7
      if ( mSector < 1 || mSector > 2 )        continue;      //sector 1-2
      if ( mRDO < 1 || mRDO > 5 )        continue;  //RDO 1-5
      LOG(DBG,"##SST ADC: Sector %d , RDO %d , Fiber %d",mSector,mRDO,mFiber);
      u_int maxI = dd->ncontent;    
      FindLadderSide(mRDO,mFiber,mLadder,mSide);   
     if(evtCt==0)
       hAdcEvent[mSide][mLadder]->GetXaxis()->SetRangeUser(0,500);
      LOG(DBG,"##SST ADC: Ladder %d , side %d",mLadder,mSide);
      
      for ( u_int i=0; i<maxI; i++ ) {
  
	mWafer = sst[i].hybrid;
	mStrip = sst[i].strip;
	mAdc   = sst[i].adc;
	if ( mStrip<0 || mStrip>767 )    continue; //strip 0-767
	if ( mWafer<0 || mWafer>15 )     continue; //wafer 0 15
	if ( mAdc > 1024 )               continue; //adc 0-1024

	FindStripNumber(mStrip);
	LOG(DBG,"##Strip %d , hybrid %d , Adc %d",mStrip,mWafer,mAdc);
	if(mSide==0)
	  hLadderWafer[0]->Fill(mLadder,mWafer);
	if(mSide==1)
	  hLadderWafer[1]->Fill(mLadder,mWafer);
	hAdcStrip[mSide][mLadder]->Fill((mStrip+mWafer*nStripPerWafer), (mAdc+350)%1024);
	if(evtCt<500)
	  hAdcEvent[mSide][mLadder]->Fill(evtCt,(mAdc+350)%1024);//default event number is 500
      }//end all RDO,Fiber,Ladder loop
     
    }

  dd = rdr->det("sst")->get("pedrms");

  while(dd && dd->iterate()) 
    { 
      daq_sst_pedrms_t *sst = (daq_sst_pedrms_t *)dd->Void;
           
      mSector = dd->sec;
      
      if(mSector==1)
	mRDO = dd->rdo;
      else 
	mRDO = 3 + dd->rdo;
      
      mFiber = dd->pad;
      u_int maxI = dd->ncontent;
      
      if ( mFiber < 0 || mFiber > 7 )        continue;      //fiber 0-7
      if ( mSector < 1 || mSector > 2 )        continue;      //sector 1-2
      if ( mRDO < 1 || mRDO > 5 )        continue;  //RDO 1-5
      LOG(DBG,"##SST PEDRMS: Sector %d , RDO %d , Fiber %d",mSector,mRDO,mFiber);
          
      FindLadderSide(mRDO,mFiber,mLadder,mSide);   
      LOG(DBG,"##SST ADC: Ladder %d , side %d",mLadder,mSide);
           
      for ( u_int i=0; i<maxI; i++ )
	{
	  for(int h=0;h<nWaferPerLadder;h++)
	    {
	    for(int c=0;c<nStripPerWafer;c++)
	      {
		int s = c;
		FindStripNumber(s);
		mPed = sst->ped[h][c];
		mRms = sst->rms[h][c]/16.0;
		
		if(mSide==0)
		  hLadderWafer[0]->Fill(mLadder,h);
		if(mSide==1)
		  hLadderWafer[1]->Fill(mLadder,h);
	
		hAdcStrip[mSide][mLadder]->Fill((s+h*nStripPerWafer),mPed);
		hAdcEvent[mSide][mLadder]->GetYaxis()->SetRangeUser(0,100);
		hAdcEvent[mSide][mLadder]->GetXaxis()->SetTitle("strip #");
		hAdcEvent[mSide][mLadder]->GetYaxis()->SetTitle("rms");
	        hAdcEvent[mSide][mLadder]->Fill((s+h*nStripPerWafer),mRms);
		//in pedestal mode, the hAdcEvent is RMS distribution.
	      }
          }  
      }
    }
  if(dd)
    evtCt++;
}
//-------------------------------

void ssdBuilder::stoprun(daqReader *rdr) 
{
  mSector = 0;
  mRDO = 0;
  mSide = 0;
  mFiber = 0;
  mLadder = 0;
  mWafer = 0;
  mStrip = 0;
}
//-----------------------------
void ssdBuilder::main(int argc, char *argv[])
{
  ssdBuilder myssd;
  
  myssd.Main(argc, argv);
  
}

//---------------------------
void ssdBuilder::FindLadderSide(int RDO,int channel,int &ladder,int &side)
{
  ladder = RDO2LADDER[RDO-1][channel]-1;//ladder [0-19];
  if(ladder>20) LOG(DBG,"RDO Number is :%d, channel number :%d",RDO,channel);
  if(RDO<3) side = 1;
  else if(RDO>3) side = 0;
  else if(channel<4) side = 1;
  else side = 0;
}
//----------------------------
void ssdBuilder::FindStripNumber(int &strip)
{
  int temp = (strip/128)*128 + ReadOutMap[strip%128];
  strip = temp-1;
}

