/***************************************************************************
 *
 * $Id: TMemStat.cxx,v 1.7 2003/10/25 02:58:50 jeromel Exp $
 *
 ***************************************************************************
 *
 * Description:
 *
 ***************************************************************************
 **************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <malloc.h>
#include <unistd.h>
#include "TMemStat.h"
#include "TList.h"
#include "TError.h"

Double_t  TMemStat::fgUsed=0;
TList    *TMemStat::fgList=0;
ClassImp(TMemStat)

#define LOWEST_VAL 0.0000001                                           /*! \def LOWEST_VAL */


//______________________________________________________________________________
TMemStat::TMemStat(const char *name):TNamed(name,"")
{
  int n = (char*)&fTally - (char*)&fLast + sizeof(fTally);
  memset(&fLast,0,n);
  fMin =  1.e+33; 
  fMax = -1.e+33; 
  if (!fgList) fgList=new TList;
  fgList->Add(this);
}
//______________________________________________________________________________
TMemStat::~TMemStat()
{
  fgList->Remove(this);
  if (!fgList->First()) {delete fgList; fgList=0;}
}

//______________________________________________________________________________
void TMemStat::Start()
{
  fLast = Used();
}

//______________________________________________________________________________
void TMemStat::Stop()
{
  fTally++;
  Double_t dif = Used() - fLast;

  //printf("DEBUG >> time distance between two stops Used=%f Last=%f\n",Used(),fLast);
  if ( dif < LOWEST_VAL )  dif  = 0.0;
  if ( dif < fMin )        fMin = dif;
  if ( dif > fMax )        fMax = dif;

  fAver += dif;
  fRms  += (dif*dif);

}
//______________________________________________________________________________
void TMemStat::Print(const char *) const
{
  if (!fTally) return;
  Double_t aver = fAver/fTally;
  Double_t rms  = ::sqrt(fabs(fRms/fTally - aver*aver));

  //printf("DEBUG :: %.10f %d %.10f %.10f\n",fAver,fTally,fRms,aver);
  if ( aver < LOWEST_VAL ) aver = 0.0;
  if ( rms  < LOWEST_VAL ) rms  = 0.0;

  printf("%40s(%d)%12.6f%12.6f%12.6f%12.6f\n",
	 GetName(),fTally,fMin,aver,fMax,rms);
}
//______________________________________________________________________________
void TMemStat::Summary()
{

   Double_t dmin=1.e+33,daver=0,dmax=-1.e+33,drms=0,dtally=0,dmp;

   if(!fgList) return;
   fgList->Sort();
   printf("%40s%12s%12s%12s%12s\n",
	  "TMemStat::Summary(calls)","Min ","Aver ","Max ","RMS ");

   for(int i=0 ; i < 40+4*12 ; i++) printf("=");
   printf("\n");
   
   TListIter next(fgList); 
   TMemStat  *m;
   while((m = (TMemStat*)next())){
     if(!m->fTally)	continue;
     m->Print();
     dtally++;
     if (m->fMin < dmin) dmin=m->fMin;
     if (m->fMax > dmax) dmax=m->fMax;
     dmp = m->fAver/m->fTally;
     daver += dmp; 
     drms  += dmp*dmp;
   }
   if(!dtally) return;

   for(int i=0 ; i < 40+4*12 ; i++) printf("-");
   printf("\n");

   daver /= dtally;
   drms   = ::sqrt(fabs(drms/dtally-daver*daver));
   printf("%40s(%d)%12.6f%12.6f%12.6f%12.6f\n",
	  "Total",(int)dtally,dmin,daver,dmax,drms);

   for(int i=0 ; i < 40+4*12 ; i++) printf("=");
   printf("\n");

}

//______________________________________________________________________________
Double_t TMemStat::Used()
{
  struct mallinfo info;
  info = mallinfo();
  return double(info.uordblks + info.usmblks)/1000000;
}

//______________________________________________________________________________
Double_t TMemStat::ProgSize()
{
  static char *ps = 0;
  Double_t res=0;  
  if (!ps) {
    int pid = ::getpid();
    ps = (char*)malloc(20);
    sprintf(ps,"ps -l -p %d",pid);
  }
  FILE *pipe = ::popen(ps,"r");
  if (!pipe) return 0.;
  
  char   psBuf[130];
  psBuf[0] = ' ';
  while( !feof( pipe ) ) {
    psBuf[1]=0;
    if(!fgets( psBuf+1, 128, pipe)) continue;
//    printf("%s\n",psBuf);
    int ifild=0;char *c;

    for (c=psBuf; c[0]; c++) {
      if (c[0]==' ' && c[1]!=' ') ifild++;
      if (ifild == 10) break;
    }
    res = (Double_t) atoi(c+1);
    if (res) break;
  }
  ::pclose(pipe);
  res *=::getpagesize()/(1024.*1024.);

  return res;
}

//______________________________________________________________________________
void TMemStat::PrintMem(const char *tit)
{
  Double_t used = Used();
  Double_t exec = ProgSize();

  if (tit) printf("\nTMemStat::%s",tit);
  printf("\t total =%10.6f heap =%10.6f (%+10.6f)\n",exec, used,used-fgUsed);
  fgUsed = used;
}
//______________________________________________________________________________
void TMemStat::Streamer(TBuffer&)
{Assert(0);}
//______________________________________________________________________________
// void TMemStat::ShowMembers(TMemberInspector& insp, char* parent){}
             
